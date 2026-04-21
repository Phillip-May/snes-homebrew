param(
    [Parameter(Mandatory = $true)]
    [string]$SourceRomPath,
    [Parameter(Mandatory = $true)]
    [string]$OutputRomPath
)

$ErrorActionPreference = "Stop"

if (!(Test-Path $SourceRomPath)) {
    throw "Source ROM not found: $SourceRomPath"
}

$romBytes = [System.IO.File]::ReadAllBytes($SourceRomPath)
$hasCopierHeader = (($romBytes.Length % 1024) -eq 512)

$outDir = Split-Path -Parent $OutputRomPath
if ($outDir -and !(Test-Path $outDir)) {
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
}

$fs = [System.IO.File]::Open($OutputRomPath, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
try {
    if (!$hasCopierHeader) {
        $dummyHeader = New-Object byte[] 512
        $fs.Write($dummyHeader, 0, $dummyHeader.Length)
    }
    $fs.Write($romBytes, 0, $romBytes.Length)
}
finally {
    $fs.Close()
}

Write-Host "Mesen ROM synced: $OutputRomPath"
