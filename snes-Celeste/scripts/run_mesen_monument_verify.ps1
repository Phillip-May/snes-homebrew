param(
    [string]$Compiler = "llvm-mos",
    [string]$MesenPath = "",
    [switch]$BuildFirst,
    [int]$TimeoutSeconds = 120
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root
. (Join-Path $PSScriptRoot "mesen_runner_common.ps1")

if ($BuildFirst) {
    Write-Host "[1/3] Build ROM ($Compiler)" -ForegroundColor Cyan
    & make COMPILER=$Compiler
    if ($LASTEXITCODE -ne 0) { throw "ROM build failed" }
} else {
    Write-Host "[1/3] Build skipped" -ForegroundColor Cyan
}

$rom = Join-Path $Root "build\mainBankZero_${Compiler}.smc"
if (!(Test-Path $rom)) { throw "ROM not found: $rom" }
Invoke-LlvmMosMesenPrecheck -Root $Root -Compiler $Compiler -CheckLuaScripts

Write-Host "[2/3] Resolve Mesen executable" -ForegroundColor Cyan
$MesenPath = Resolve-MesenExecutable -MesenPath $MesenPath

$lua = Join-Path $Root "scripts\mesen_monument_verify.lua"
$result = Join-Path $Root "build\mesen_monument_verify_result.txt"
if (Test-Path $result) { Remove-Item -LiteralPath $result -Force }

$mesenRom = Join-Path $Root "build\mainBankZero_${Compiler}_mesen.smc"
New-MesenRomCopy -SourceRom $rom -OutputRom $mesenRom

Write-Host "[3/3] Run Mesen monument verification" -ForegroundColor Cyan
$args = @("--testRunner", "--doNotSaveSettings", "--timeout=$TimeoutSeconds", "--debug.scriptWindow.allowIoOsAccess=true", "$lua", "$mesenRom")
$txt = Invoke-MesenProcess -MesenPath $MesenPath -ArgumentList $args -ResultPath $result -TimeoutSeconds $TimeoutSeconds -IsFinal { param($text) ($text -like "PASS*" -or $text -like "FAIL*") -and $text.Length -gt 0 }

if (!(Test-Path $result)) {
    throw "Monument result file not generated (timeout/boot failure): $result"
}

$txt = (Get-Content -Raw $result).Trim()
Write-Host "Monument verify result: $txt"
if ($txt -notlike "PASS*") {
    throw "Monument verification failed: $txt"
}

exit 0
