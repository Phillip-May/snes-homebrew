param(
    [string]$Compiler = "llvm-mos",
    [string]$MesenPath = "",
    [switch]$BuildFirst,
    [int]$TimeoutSeconds = 120
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

if ($BuildFirst) {
    Write-Host "[1/3] Build ROM ($Compiler)" -ForegroundColor Cyan
    & make COMPILER=$Compiler
    if ($LASTEXITCODE -ne 0) { throw "ROM build failed" }
} else {
    Write-Host "[1/3] Build skipped" -ForegroundColor Cyan
}

$rom = Join-Path $Root "build\mainBankZero_${Compiler}.smc"
if (!(Test-Path $rom)) { throw "ROM not found: $rom" }
if ($Compiler -eq "llvm-mos") {
    $mapPath = Join-Path $Root "build\mainBankZero_llvm-mos.map"
    if (!(Test-Path $mapPath)) { throw "Map not found: $mapPath (rebuild with llvm-mos)" }
    & python (Join-Path $Root "scripts\gen_mesen_symbols.py")
    if ($LASTEXITCODE -ne 0) { throw "Failed to generate mesen symbols" }
    & python (Join-Path $Root "scripts\check_mesen_lua.py")
    if ($LASTEXITCODE -ne 0) { throw "Mesen Lua harness check failed" }
}

Write-Host "[2/3] Resolve Mesen executable" -ForegroundColor Cyan
if ([string]::IsNullOrWhiteSpace($MesenPath)) {
    $candidates = @(
        "C:\Program Files\Mesen2\Mesen.exe",
        "C:\Program Files\Mesen\Mesen.exe",
        "C:\Users\Admin\Downloads\Mesen_2.1.0_Windows\Mesen.exe",
        "C:\Users\Admin\Downloads\Mesen_2.1.0_Windows(1)\Mesen.exe",
        "C:\Users\Admin\Documents\Mesen2\Backups\Mesen.2.1.1.exe",
        "C:\Users\Admin\Documents\Mesen2\Backups\Mesen.2.1.0.b21e273.exe"
    )
    foreach ($c in $candidates) {
        if (Test-Path $c) { $MesenPath = $c; break }
    }
}
if ([string]::IsNullOrWhiteSpace($MesenPath) -or !(Test-Path $MesenPath)) {
    throw "Mesen executable not found. Pass -MesenPath explicitly."
}

$lua = Join-Path $Root "scripts\mesen_breakable_verify.lua"
$result = Join-Path $Root "build\mesen_breakable_verify_result.txt"
if (Test-Path $result) { Remove-Item -LiteralPath $result -Force }

$mesenRom = Join-Path $Root "build\mainBankZero_${Compiler}_mesen.smc"
$romBytes = [System.IO.File]::ReadAllBytes($rom)
$hasCopierHeader = (($romBytes.Length % 1024) -eq 512)
$fs = [System.IO.File]::Open($mesenRom, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
try {
    if (!$hasCopierHeader) {
        $dummyHeader = New-Object byte[] 512
        $fs.Write($dummyHeader, 0, $dummyHeader.Length)
    }
    $fs.Write($romBytes, 0, $romBytes.Length)
} finally {
    $fs.Close()
}

Write-Host "[3/3] Run Mesen breakable verification" -ForegroundColor Cyan
Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | ForEach-Object {
    try { Stop-Process -Id $_.Id -Force -ErrorAction Stop } catch { }
}
Start-Sleep -Milliseconds 250

$args = @("--testRunner", "--doNotSaveSettings", "--timeout=$TimeoutSeconds", "--debug.scriptWindow.allowIoOsAccess=true", "$lua", "$mesenRom")
$before = @(Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | Select-Object -ExpandProperty Id)
$null = Start-Process -FilePath $MesenPath -ArgumentList $args -PassThru

$deadline = (Get-Date).AddSeconds($TimeoutSeconds)
$txt = "RUNNING"
while ((Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 250
    if (Test-Path $result) {
        $txt = (Get-Content -Raw $result).Trim()
        if (($txt -like "PASS*" -or $txt -like "FAIL*") -and $txt.Length -gt 0) { break }
    }
}

$after = @(Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | Select-Object -ExpandProperty Id)
$newPids = $after | Where-Object { $_ -notin $before }
foreach ($procId in $newPids) {
    try { Stop-Process -Id $procId -Force -ErrorAction Stop } catch { }
}

if (!(Test-Path $result)) {
    throw "Breakable result file not generated (timeout/boot failure): $result"
}

$txt = (Get-Content -Raw $result).Trim()
Write-Host "Breakable verify result: $txt"
if ($txt -notlike "PASS*") {
    throw "Breakable verification failed: $txt"
}

exit 0
