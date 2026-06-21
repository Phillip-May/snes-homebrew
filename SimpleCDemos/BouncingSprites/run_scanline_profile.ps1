param(
    [string]$MesenPath = "C:\Users\Admin\Downloads\Mesen_2.1.0_Windows\Mesen.exe",
    [string]$RomDir    = "",
    [int]$TimeoutSeconds = 90
)

$ErrorActionPreference = "Stop"
$Here = $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($RomDir)) { $RomDir = $Here }
$lua = Join-Path $Here "scanline_profile.lua"
if (!(Test-Path $MesenPath)) { throw "Mesen not found: $MesenPath" }
if (!(Test-Path $lua)) { throw "Lua not found: $lua" }

# compiler -> @{ rom; header }. "header" is the internal SNES header offset whose
# checksum/complement we repair so Mesen scores the mapping correctly: $7FC0 for
# LoROM, $FFC0 for HiROM (vbcc builds HiROM), or $null when the build already
# writes a valid checksum (tcc816) / Mesen detects it anyway (calypsi).
$roms = [ordered]@{
    "calypsi"   = @{ rom = "mainBankZero_calypsi.smc";   header = $null   }
    "llvm-mos"  = @{ rom = "mainBankZero_llvm-mos.smc";  header = 0x7FC0  }
    "vbcc65816" = @{ rom = "mainBankZero_vbcc65816.smc"; header = 0xFFC0  }
    "tcc816"    = @{ rom = "mainBankZero_tcc816.sfc";    header = $null   }
}

function New-MesenRomCopy([string]$Src, [string]$Dst, $HeaderOffset) {
    $bytes = [System.IO.File]::ReadAllBytes($Src)
    $hasCopierHeader = (($bytes.Length % 1024) -eq 512)
    $base = if ($hasCopierHeader) { 512 } else { 0 }
    if ($null -ne $HeaderOffset) {
        $co = $base + $HeaderOffset + 0x1C
        if (($co + 3) -lt $bytes.Length) {
            for ($i = 0; $i -lt 4; $i++) { $bytes[$co + $i] = 0 }
            $sum = 0
            for ($i = $base; $i -lt $bytes.Length; $i++) { $sum = ($sum + $bytes[$i]) -band 0xFFFF }
            $comp = ($sum -bxor 0xFFFF) -band 0xFFFF
            $bytes[$co + 0] = $comp -band 0xFF; $bytes[$co + 1] = ($comp -shr 8) -band 0xFF
            $bytes[$co + 2] = $sum -band 0xFF;  $bytes[$co + 3] = ($sum -shr 8) -band 0xFF
        }
    }
    $fs = [System.IO.File]::Open($Dst, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
    try {
        if (!$hasCopierHeader) { $fs.Write((New-Object byte[] 512), 0, 512) }
        $fs.Write($bytes, 0, $bytes.Length)
    } finally { $fs.Close() }
}

$results = [ordered]@{}
foreach ($entry in $roms.GetEnumerator()) {
    $compiler = $entry.Key
    $rom = Join-Path $RomDir $entry.Value.rom
    if (!(Test-Path $rom)) { Write-Host "SKIP $compiler (ROM missing: $rom)"; $results[$compiler] = "ROM-MISSING"; continue }

    $mesenRom = Join-Path $RomDir ("_mesen_" + $entry.Value.rom)
    New-MesenRomCopy -Src $rom -Dst $mesenRom -HeaderOffset $entry.Value.header

    $resultPath = Join-Path $RomDir ("scanline_result_$compiler.txt")
    if (Test-Path $resultPath) { Remove-Item -LiteralPath $resultPath -Force }

    $env:PROFILE_OUT = $resultPath
    $env:PROFILE_LABEL = $compiler

    Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | ForEach-Object { try { Stop-Process -Id $_.Id -Force } catch {} }

    Write-Host "RUN  $compiler ..." -NoNewline
    $args = @("--testRunner", "--doNotSaveSettings", "--timeout=$TimeoutSeconds",
              "--debug.scriptWindow.allowIoOsAccess=true", "$lua", "$mesenRom")
    $proc = Start-Process -FilePath $MesenPath -ArgumentList $args -PassThru

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds + 15)
    $txt = "RUNNING"
    while ((Get-Date) -lt $deadline) {
        Start-Sleep -Milliseconds 300
        if (Test-Path $resultPath) {
            $raw = (Get-Content -Raw -ErrorAction SilentlyContinue $resultPath)
            if (![string]::IsNullOrWhiteSpace($raw)) {
                $txt = $raw.Trim()
                if ($txt -like "DONE*" -or $txt -like "FAIL*") { break }
            }
        }
        if ($proc.HasExited -and (Test-Path $resultPath)) {
            $txt = (Get-Content -Raw $resultPath).Trim(); break
        }
    }
    try { if (!$proc.HasExited) { Stop-Process -Id $proc.Id -Force } } catch {}
    $results[$compiler] = $txt
    Write-Host " $txt"
    Remove-Item -LiteralPath $mesenRom -Force -ErrorAction SilentlyContinue
}

Write-Host ""
Write-Host "==================== SCANLINE PROFILE SUMMARY ===================="
foreach ($k in $results.Keys) { Write-Host ("{0,-12} {1}" -f $k, $results[$k]) }
