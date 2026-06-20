#requires -version 5
# Build every demo with each compiler in its SUPPORTED_COMPILERS and report a
# pass/fail summary. Failures are always surfaced, never silently skipped.
#
#   .\build-all.ps1              # build everything, print summary, exit 0
#   .\build-all.ps1 -FailOnError # exit non-zero if any build failed (for CI)
[CmdletBinding()]
param([switch]$FailOnError)

$root = $PSScriptRoot

$demos = Get-ChildItem -Path $root -Directory |
    Where-Object { $_.Name -ne 'shared' -and (Test-Path (Join-Path $_.FullName 'Makefile')) } |
    Sort-Object Name

$results = New-Object System.Collections.Generic.List[object]

foreach ($demo in $demos) {
    Push-Location $demo.FullName
    $sc = (& make -s print-supported | Out-String).Trim()
    Pop-Location
    $compilers = $sc -split '\s+' | Where-Object { $_ }

    foreach ($c in $compilers) {
        $log = Join-Path $env:TEMP ("buildall_{0}_{1}.log" -f $demo.Name, ($c -replace '[^\w]', '_'))
        Push-Location $demo.FullName
        & make clean *> $null
        & make COMPILER=$c ALLOW_UNSUPPORTED=1 2>&1 | Out-File -FilePath $log -Encoding utf8
        $code = $LASTEXITCODE
        $rom = $null
        $bd = Join-Path $demo.FullName 'build'
        if (Test-Path $bd) {
            $rom = Get-ChildItem $bd -File -ErrorAction SilentlyContinue |
                Where-Object { $_.Extension -in '.smc', '.sfc', '.bin' } | Select-Object -First 1
        }
        Pop-Location

        $pass = ($code -eq 0 -and $rom)
        $status = if ($pass) { 'PASS' } else { 'FAIL' }
        $results.Add([pscustomobject]@{ Demo = $demo.Name; Compiler = $c; Status = $status; Log = $log })
        Write-Host ("{0,-20} {1,-12} {2}" -f $demo.Name, $c, $status)
    }
}

Write-Host ""
Write-Host "===== SUMMARY ====="
$pass = @($results | Where-Object Status -eq 'PASS').Count
$fail = @($results | Where-Object Status -eq 'FAIL')
Write-Host ("{0} passed, {1} failed, {2} total" -f $pass, $fail.Count, $results.Count)

if ($fail.Count -gt 0) {
    Write-Host ""
    Write-Host "FAILURES:"
    foreach ($f in $fail) {
        Write-Host ("  {0} / {1}    (log: {2})" -f $f.Demo, $f.Compiler, $f.Log)
    }
}

if ($FailOnError -and $fail.Count -gt 0) { exit 1 } else { exit 0 }
