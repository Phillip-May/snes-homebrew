param(
    [Parameter(Mandatory = $true)]
    [string]$RomPath
)

$ErrorActionPreference = "Stop"

if (!(Test-Path $RomPath)) {
    throw "ROM not found: $RomPath"
}

$bytes = [System.IO.File]::ReadAllBytes($RomPath)
$copierHeaderSize = if (($bytes.Length % 1024) -eq 512) { 512 } else { 0 }
$romSize = $bytes.Length - $copierHeaderSize
if ($romSize -lt 0x8000) {
    throw "ROM too small for SNES LoROM header: $RomPath"
}

$headerOffset = $copierHeaderSize + 0x7FC0
$checksumOffset = $headerOffset + 0x1C
if (($checksumOffset + 3) -ge $bytes.Length) {
    throw "SNES checksum fields are outside ROM: $RomPath"
}

for ($i = 0; $i -lt 4; $i++) {
    $bytes[$checksumOffset + $i] = 0
}

$checksum = 0
for ($i = $copierHeaderSize; $i -lt $bytes.Length; $i++) {
    $checksum = ($checksum + $bytes[$i]) -band 0xFFFF
}

$complement = ($checksum -bxor 0xFFFF) -band 0xFFFF
$bytes[$checksumOffset + 0] = $complement -band 0xFF
$bytes[$checksumOffset + 1] = ($complement -shr 8) -band 0xFF
$bytes[$checksumOffset + 2] = $checksum -band 0xFF
$bytes[$checksumOffset + 3] = ($checksum -shr 8) -band 0xFF

[System.IO.File]::WriteAllBytes($RomPath, $bytes)
Write-Host ("SNES checksum fixed: checksum=0x{0:X4} complement=0x{1:X4}" -f $checksum, $complement)
