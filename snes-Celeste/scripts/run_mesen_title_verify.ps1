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

$lua = Join-Path $Root "scripts\mesen_title_verify.lua"
$result = Join-Path $Root "build\mesen_title_verify_result.txt"
$checksums = Join-Path $Root "build\title_screen_checksums.lua"
if (Test-Path $result) { Remove-Item -LiteralPath $result -Force }
if (!(Test-Path $checksums)) {
    & python (Join-Path $Root "python\convert_title_screen_snes.py")
    if ($LASTEXITCODE -ne 0) { throw "Failed to regenerate title screen checksums" }
}

Write-Host "[3/3] Run Mesen title verification" -ForegroundColor Cyan
$args = @("--fullscreen=0", "--lua", "$lua", "$rom")
$txt = Invoke-MesenProcess -MesenPath $MesenPath -ArgumentList $args -ResultPath $result -TimeoutSeconds $TimeoutSeconds -IsFinal { param($text) $text -ne "RUNNING" -and $text.Length -gt 0 }

if (!(Test-Path $result)) {
    throw "Title verification result not generated: $result"
}

$txt = (Get-Content -Raw $result).Trim()
Write-Host "Title verify result: $txt"
if ($txt -notlike "PASS*") {
    throw "Title verification failed: $txt"
}

exit 0
