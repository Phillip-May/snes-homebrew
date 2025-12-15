param(
    [string]$InputFile,
    [string]$OutputFile
)

$bytes = [System.IO.File]::ReadAllBytes($InputFile)
if ($bytes.Length -ge 7) {
    $bytes[6] = $bytes[6] -band 0xFE
}
[System.IO.File]::WriteAllBytes($OutputFile, $bytes)
