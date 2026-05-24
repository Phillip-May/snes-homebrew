param(
    [string]$ElfPath = "build/mainBankZero_llvm-mos.smc.elf",
    [int]$MinBank6Size = 0,
    [switch]$RequireBank6,
    [switch]$AllowNoBank6,
    [string]$LlvmMosPath = $env:LLVM_MOS_PATH
)

$ErrorActionPreference = "Stop"

if (!(Test-Path $ElfPath)) {
    throw "ELF not found: $ElfPath"
}

if ([string]::IsNullOrWhiteSpace($LlvmMosPath)) {
    $LlvmMosPath = Join-Path $env:SystemDrive "llvm-mos"
}

$objdump = Join-Path $LlvmMosPath "bin\llvm-objdump.exe"
if (!(Test-Path $objdump)) {
    throw "llvm-objdump not found: $objdump"
}

$sectionText = & $objdump -h $ElfPath | Out-String
$symbolText = & $objdump -t $ElfPath | Out-String
$disassemblyText = & $objdump -d $ElfPath | Out-String

function Assert-SymbolInSection($SymbolName, $SectionName) {
    $line = ($script:symbolText -split "`r?`n" | Where-Object { $_ -match ("\s" + [regex]::Escape($SymbolName) + "\s*$") } | Select-Object -First 1)
    if (!$line) {
        throw "Required symbol not found: $SymbolName"
    }
    if ($line -notmatch ("\s" + [regex]::Escape($SectionName) + "\s")) {
        throw "Symbol $SymbolName is not in $SectionName. Found: $line"
    }
    Write-Host "verified $SymbolName in $SectionName"
}

function Get-SectionSize($SectionName) {
    $m = [regex]::Match($script:sectionText, "^\s*\d+\s+" + [regex]::Escape($SectionName) + "\s+([0-9a-fA-F]{8})\s+", [System.Text.RegularExpressions.RegexOptions]::Multiline)
    if (!$m.Success) {
        throw "Section not found: $SectionName"
    }
    return [Convert]::ToInt32($m.Groups[1].Value, 16)
}

function Get-OptionalSectionSize($SectionName) {
    $m = [regex]::Match($script:sectionText, "^\s*\d+\s+" + [regex]::Escape($SectionName) + "\s+([0-9a-fA-F]{8})\s+", [System.Text.RegularExpressions.RegexOptions]::Multiline)
    if (!$m.Success) {
        return 0
    }
    return [Convert]::ToInt32($m.Groups[1].Value, 16)
}

function Get-SymbolAddress($SymbolName) {
    $line = ($script:symbolText -split "`r?`n" | Where-Object { $_ -match ("\s" + [regex]::Escape($SymbolName) + "\s*$") } | Select-Object -First 1)
    if (!$line) {
        throw "Required symbol not found: $SymbolName"
    }
    $m = [regex]::Match($line, "^\s*([0-9a-fA-F]+)\s+")
    if (!$m.Success) {
        throw "Could not parse symbol address for $SymbolName. Found: $line"
    }
    return [Convert]::ToInt32($m.Groups[1].Value, 16)
}

function Assert-SnesRamHeadroom {
    $ramLimit = 0x2000
    $minHeadroom = 0x80
    $heapStart = Get-SymbolAddress "__heap_start"
    $headroom = $ramLimit - $heapStart
    if ($headroom -lt $minHeadroom) {
        throw ("SNES near-RAM headroom too small: __heap_start=0x{0:X4}, limit=0x{1:X4}, free=0x{2:X}, required>=0x{3:X}. Reduce .bss/.noinit/static-stack pressure before adding code." -f $heapStart, $ramLimit, $headroom, $minHeadroom)
    }
    Write-Host ("verified SNES near-RAM headroom: __heap_start=0x{0:X4}, free=0x{1:X}" -f $heapStart, $headroom)
}

function Assert-NoBankLocalBankSwitchCalls {
    $functionSections = @{}
    foreach ($line in ($script:symbolText -split "`r?`n")) {
        $m = [regex]::Match($line, "^\s*[0-9a-fA-F]+\s+\S+\s+F\s+(\S+)\s+[0-9a-fA-F]+\s+(\S+)\s*$")
        if ($m.Success) {
            $functionSections[$m.Groups[2].Value] = $m.Groups[1].Value
        }
    }

    $currentFunction = $null
    $currentSection = $null
    $badCalls = New-Object System.Collections.Generic.List[string]
    foreach ($line in ($script:disassemblyText -split "`r?`n")) {
        $header = [regex]::Match($line, "^\s*[0-9a-fA-F]+\s+<([^>]+)>:")
        if ($header.Success) {
            $currentFunction = $header.Groups[1].Value
            $currentSection = if ($functionSections.ContainsKey($currentFunction)) { $functionSections[$currentFunction] } else { $null }
            continue
        }

        if ($currentSection -match "^rom_bank_[0-7]$") {
            $call = [regex]::Match($line, "^\s*([0-9a-fA-F]+):.*\b(?:jsr|jmp)\s+\$[0-9a-fA-F]+\s+<(port_prg_bank_(?:push|pop|switch)|snes_prg_bank_switch_pc)>")
            if ($call.Success) {
                $badCalls.Add(("{0} in {1} ({2}) calls {3}: {4}" -f $call.Groups[1].Value, $currentFunction, $currentSection, $call.Groups[2].Value, $line.Trim()))
            }
        }
    }

    if ($badCalls.Count -gt 0) {
        throw ("Unsafe SNES bank switch from bank-local code. port_prg_bank_* may only be called from mirrored fixed code:`n" + ($badCalls -join "`n"))
    }

    Write-Host "verified no bank-local calls to SNES PRG bank switch helpers"
}

function Assert-NoUnresolvedBankLocalCalls {
    $functions = @{}
    foreach ($line in ($script:symbolText -split "`r?`n")) {
        $m = [regex]::Match($line, "^\s*([0-9a-fA-F]+)\s+\S+\s+F\s+(\S+)\s+([0-9a-fA-F]+)\s+(\S+)\s*$")
        if ($m.Success) {
            $name = $m.Groups[4].Value
            $functions[$name] = @{
                Addr = [Convert]::ToInt32($m.Groups[1].Value, 16)
                Section = $m.Groups[2].Value
                Size = [Convert]::ToInt32($m.Groups[3].Value, 16)
                Name = $name
            }
        }
    }

    $currentFunction = $null
    $currentSection = $null
    $badCalls = New-Object System.Collections.Generic.List[string]
    foreach ($line in ($script:disassemblyText -split "`r?`n")) {
        $header = [regex]::Match($line, "^\s*([0-9a-fA-F]+)\s+<([^>]+)>:")
        if ($header.Success) {
            $currentFunction = $header.Groups[2].Value
            $currentSection = if ($functions.ContainsKey($currentFunction)) { $functions[$currentFunction].Section } else { $null }
            continue
        }

        $bankMatch = [regex]::Match($currentSection, "^rom_bank_([0-7])$")
        if (!$bankMatch.Success) {
            continue
        }

        $call = [regex]::Match($line, '^\s*([0-9a-fA-F]+):.*\b(?:jsr|jmp)\s+\$([0-9a-fA-F]{4})\b')
        if (!$call.Success) {
            continue
        }

        $target = [Convert]::ToInt32($call.Groups[2].Value, 16)
        if ($target -lt 0x8000 -or $target -ge 0xB000) {
            continue
        }

        $bank = [Convert]::ToInt32($bankMatch.Groups[1].Value, 10)
        $fullTarget = ($bank -shl 16) + $target
        $owner = $null
        foreach ($fn in $functions.Values) {
            if ($fn.Section -ne $currentSection) {
                continue
            }
            if ($fn.Addr -le $fullTarget -and ($fn.Size -eq 0 -or ($fn.Addr + $fn.Size) -gt $fullTarget)) {
                $owner = $fn
                break
            }
        }

        if ($null -eq $owner) {
            $badCalls.Add(("{0} in {1} ({2}) targets local ${3:X4} with no function in the active bank: {4}" -f $call.Groups[1].Value, $currentFunction, $currentSection, $target, $line.Trim()))
        }
    }

    if ($badCalls.Count -gt 0) {
        throw ("Unsafe unresolved SNES bank-local call. Local `$8000-`$AFFF JSR/JMP targets must resolve inside the caller's active bank:`n" + ($badCalls -join "`n"))
    }

    Write-Host "verified bank-local JSR/JMP targets resolve within the active bank"
}

$BankLocalMax = 0x3000
$FixedMirrorMax = 0x4FC0
$match = [regex]::Match($sectionText, "^\s*\d+\s+rom_bank_6\s+([0-9a-fA-F]{8})\s+", [System.Text.RegularExpressions.RegexOptions]::Multiline)
$mustRequireBank6 = $RequireBank6 -or !$AllowNoBank6
Assert-SnesRamHeadroom
Assert-NoBankLocalBankSwitchCalls
Assert-NoUnresolvedBankLocalCalls
if ($mustRequireBank6) {
    $bank1Size = Get-SectionSize "rom_bank_1"
    $bank2Size = Get-SectionSize "rom_bank_2"
    $bank3Size = Get-SectionSize "rom_bank_3"
    $bank4Size = Get-SectionSize "rom_bank_4"
    $bank1RoSize = Get-OptionalSectionSize "rom_bank_1_rodata"
    $bank2RoSize = Get-OptionalSectionSize "rom_bank_2_rodata"
    $bank3RoSize = Get-OptionalSectionSize "rom_bank_3_rodata"
    $bank4RoSize = Get-OptionalSectionSize "rom_bank_4_rodata"
    $bank5RoSize = Get-OptionalSectionSize "rom_bank_5_rodata"
    $bank5Size = Get-SectionSize "rom_bank_5"
    $bank7Size = Get-SectionSize "rom_bank_7"
    $bank6Size = if ($match.Success) { [Convert]::ToInt32($match.Groups[1].Value, 16) } else { 0 }
    $bank6RoSize = if ($sectionText -match "^\s*\d+\s+rom_bank_6_rodata\s+") { Get-SectionSize "rom_bank_6_rodata" } else { 0 }
    $fixedTextSize = Get-SectionSize ".text"
    $fixedRoSize = Get-SectionSize ".rodata"
    $fixedThunkSize = Get-SectionSize "rom_fixed"
    $bank0Size = Get-SectionSize "rom_bank_0"
    $assetSize = Get-SectionSize "rom_data_bank_1"
    $level0Size = Get-SectionSize "rom_data_bank_2"
    $level1Size = Get-SectionSize "rom_data_bank_3"
    $level2Size = Get-SectionSize "rom_data_bank_4"
    $titleSize = Get-SectionSize "rom_data_bank_5"
    $spcSize = Get-SectionSize "rom_data_bank_6"
    $headerSize = Get-SectionSize ".snes_header"

    if ($headerSize -ne 0x20) {
        throw ("SNES header section must be 0x20 bytes, found 0x{0:X}" -f $headerSize)
    }

    foreach ($pair in @(
        @("rom_bank_1", ($bank1Size + $bank1RoSize)), @("rom_bank_2", ($bank2Size + $bank2RoSize)),
        @("rom_bank_3", ($bank3Size + $bank3RoSize)), @("rom_bank_4", ($bank4Size + $bank4RoSize)),
        @("rom_bank_5", ($bank5Size + $bank5RoSize)), @("rom_bank_6", ($bank6Size + $bank6RoSize)),
        @("rom_bank_7", $bank7Size))) {
        if ($pair[1] -gt $BankLocalMax) {
            throw ("{0} local overflow: code+rodata=0x{1:X}, max=0x{2:X}" -f $pair[0], $pair[1], $BankLocalMax)
        }
    }
    if (($fixedTextSize + $fixedRoSize + $fixedThunkSize) -gt $FixedMirrorMax) {
        throw ("fixed mirror overflow: text+rodata+rom_fixed=0x{0:X}, max=0x{1:X}" -f ($fixedTextSize + $fixedRoSize + $fixedThunkSize), $FixedMirrorMax)
    }
    if ($bank0Size -gt $BankLocalMax) {
        throw ("bank0 local overflow: size=0x{0:X}, max=0x{1:X}" -f $bank0Size, $BankLocalMax)
    }
    foreach ($pair in @(
        @("rom_data_bank_1", $assetSize), @("rom_data_bank_2", $level0Size),
        @("rom_data_bank_3", $level1Size), @("rom_data_bank_4", $level2Size),
        @("rom_data_bank_5", $titleSize), @("rom_data_bank_6", $spcSize))) {
        if ($pair[1] -gt 0x7FE0) {
            throw ("{0} overflow: size=0x{1:X}, max=0x7FE0" -f $pair[0], $pair[1])
        }
    }

    Write-Host ("exec banks: b1=0x{0:X}+ro=0x{1:X}, b2=0x{2:X}+ro=0x{3:X}, b3=0x{4:X}+ro=0x{5:X}, b4=0x{6:X}+ro=0x{7:X}, b5=0x{8:X}+ro=0x{9:X}, b6=0x{10:X}+ro=0x{11:X}, b7=0x{12:X}" -f $bank1Size, $bank1RoSize, $bank2Size, $bank2RoSize, $bank3Size, $bank3RoSize, $bank4Size, $bank4RoSize, $bank5Size, $bank5RoSize, $bank6Size, $bank6RoSize, $bank7Size)
    Write-Host ("fixed mirror text+rodata+stubs: {0} bytes (0x{1:X})" -f ($fixedTextSize + $fixedRoSize + $fixedThunkSize), ($fixedTextSize + $fixedRoSize + $fixedThunkSize))
    Write-Host ("rom_bank_0 local size: {0} bytes (0x{1:X})" -f $bank0Size, $bank0Size)
    Write-Host ("data banks: assets=0x{0:X}, levels0=0x{1:X}, levels1=0x{2:X}, levels2=0x{3:X}, title=0x{4:X}, spc=0x{5:X}" -f $assetSize, $level0Size, $level1Size, $level2Size, $titleSize, $spcSize)
    Write-Host ("SNES header size: {0} bytes" -f $headerSize)

    Assert-SymbolInSection "snes_prg_bank_switch_pc" "rom_fixed"
    Assert-SymbolInSection "playerInit" "rom_bank_1"
    Assert-SymbolInSection "playerUpdate" "rom_bank_1"
    Assert-SymbolInSection "processObjectBank2" "rom_bank_2"
    Assert-SymbolInSection "processObjectBank3" "rom_bank_3"
    Assert-SymbolInSection "processObjectBank4" "rom_bank_4"
    Assert-SymbolInSection "syncCameraFromPlayer" "rom_bank_5"
    Assert-SymbolInSection "updateRoomMusic" "rom_bank_7"
    Assert-SymbolInSection "initObject" ".text"
    Assert-SymbolInSection "main" ".text"
    Assert-SymbolInSection "COP" "rom_bank_0"
    Assert-SymbolInSection "NMI" "rom_bank_0"
    Assert-SymbolInSection "title_tiledata_snes_2bpp" "rom_data_bank_5"
    Assert-SymbolInSection "spc_chunk_000" "rom_data_bank_6"
    if ($symbolText -match "switch\.table\.snes_bank") {
        throw "banked gameplay generated a switch table; keep banked code self-contained or move the table into the same bank."
    }
    Write-Host "SNES banking verification PASS (NES-style fixed mirror + executable banks 0-7 + high data banks)"
} else {
    if ($match.Success) {
        $sizeHex = $match.Groups[1].Value
        $size = [Convert]::ToInt32($sizeHex, 16)
        Write-Host ("rom_bank_6 present: {0} bytes (0x{1})" -f $size, $sizeHex)
    } else {
        Write-Host "rom_bank_6 absent (safe mode: no cross-bank code calls)."
    }
    Write-Host "SNES banking verification PASS (safe mode)"
}
exit 0
