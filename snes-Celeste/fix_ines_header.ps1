param(
    [string]$InputFile,
    [string]$OutputFile
)

# iNES header fix for UNROM-512 (Mapper 30)
# Ensures horizontal mirroring is set correctly
# llvm-mos should set mapper 30 automatically via the config file

$bytes = [System.IO.File]::ReadAllBytes($InputFile)
if ($bytes.Length -ge 7) {
    # Set horizontal mirroring (clear bit 0 of byte 6)
    $bytes[6] = $bytes[6] -band 0xFE
}
[System.IO.File]::WriteAllBytes($OutputFile, $bytes)
