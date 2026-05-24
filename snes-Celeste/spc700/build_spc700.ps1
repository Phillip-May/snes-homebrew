param(
    [string]$LlvmMosPath = $env:LLVM_MOS_PATH
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($LlvmMosPath)) {
    $LlvmMosPath = "C:\llvm-mos"
}

$clang = Join-Path $LlvmMosPath "bin\mos-sim-spc700-clang.bat"
if (-not (Test-Path $clang)) {
    Write-Warning "Skipping spc700 build: $clang not found"
    exit 0
}

Push-Location $PSScriptRoot
try {
    & $clang -Os -o main.spc main.c -T link.ld -save-temps
    python -c "spc=bytearray(open('main.spc','rb').read());target=65920;dsp=spc[-128:];ram=spc[:-128];pad=max(0,target-len(spc));open('main.spc','wb').write(ram+bytearray(pad)+dsp)"
}
finally {
    Pop-Location
}
