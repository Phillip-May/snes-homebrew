param(
    [string]$InputFile,
    [string]$OutputFile,
    [string]$ObjcopyPath
)

# iNES header for GNROM (Mapper 66)
# 32KB PRG-ROM (2 banks), 8KB CHR-ROM (1 bank)
# Horizontal mirroring, Mapper 66
# Mapper encoding: byte 6 bits 0-3 = lower nibble, byte 7 bits 4-7 = upper nibble
# Mapper 66 = 0x42 = lower nibble 2, upper nibble 4
$inesHeader = @(
    0x4E, 0x45, 0x53, 0x1A,  # "NES" + EOF
    0x02,                    # 2 PRG-ROM banks (32KB)
    0x01,                    # 1 CHR-ROM bank (8KB)
    0x02,                    # Flags 6: bits 0-3 = mapper lower (2), bits 4-7 = flags (0 = horizontal mirroring, no battery, no trainer, no four-screen)
    0x40,                    # Flags 7: bits 0-3 = flags (0), bits 4-7 = mapper upper (4)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  # Reserved/padding
)

# Convert header array to bytes
$headerBytes = [byte[]]$inesHeader

# Expected sizes
$prgRomSize = 32 * 1024  # 32KB
$chrRomSize = 8 * 1024   # 8KB

# Extract PRG-ROM section (addresses 0x8000-0xFFFF)
$tempPrg = "$env:TEMP\nes_prg.bin"
$objcopyPrgCmd = "& '$ObjcopyPath' -O binary --only-section=.text --only-section=.rodata --only-section=.data '$InputFile' '$tempPrg'"

Write-Host "Extracting PRG-ROM sections from ELF..."
Invoke-Expression $objcopyPrgCmd

# Extract CHR-ROM section (address 0x01000000)
$tempChr = "$env:TEMP\nes_chr.bin"
$objcopyChrCmd = "& '$ObjcopyPath' -O binary --only-section=.chr_rom '$InputFile' '$tempChr'"

Write-Host "Extracting CHR-ROM section from ELF..."
Invoke-Expression $objcopyChrCmd

# Read PRG-ROM
$prgRom = @()
if (Test-Path $tempPrg) {
    $prgData = [System.IO.File]::ReadAllBytes($tempPrg)
    if ($prgData.Length -lt $prgRomSize) {
        Write-Warning "PRG-ROM is smaller than expected. Padding..."
        $prgRom = $prgData + [byte[]](New-Object byte[] ($prgRomSize - $prgData.Length))
    } else {
        $prgRom = $prgData[0..($prgRomSize - 1)]
    }
} else {
    Write-Warning "PRG-ROM section not found, creating empty PRG-ROM"
    $prgRom = [byte[]](New-Object byte[] $prgRomSize)
}

# Read CHR-ROM
$chrRom = @()
if (Test-Path $tempChr) {
    $chrData = [System.IO.File]::ReadAllBytes($tempChr)
    if ($chrData.Length -lt $chrRomSize) {
        Write-Warning "CHR-ROM is smaller than expected. Padding..."
        $chrRom = $chrData + [byte[]](New-Object byte[] ($chrRomSize - $chrData.Length))
    } elseif ($chrData.Length -gt $chrRomSize) {
        $chrRom = $chrData[0..($chrRomSize - 1)]
    } else {
        $chrRom = $chrData
    }
} else {
    Write-Warning "CHR-ROM section not found, creating empty CHR-ROM"
    $chrRom = [byte[]](New-Object byte[] $chrRomSize)
}

# Combine: header + PRG-ROM + CHR-ROM
$outputData = $headerBytes + $prgRom + $chrRom

# Write output file
[System.IO.File]::WriteAllBytes($OutputFile, $outputData)

Write-Host "Created iNES ROM: $OutputFile"
Write-Host "  Header: $($headerBytes.Length) bytes"
Write-Host "  PRG-ROM: $($prgRom.Length) bytes"
Write-Host "  CHR-ROM: $($chrRom.Length) bytes"
Write-Host "  Total: $($outputData.Length) bytes"

# Cleanup
Remove-Item $tempPrg -ErrorAction SilentlyContinue
Remove-Item $tempChr -ErrorAction SilentlyContinue
