function Invoke-LlvmMosMesenPrecheck {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$Compiler,
        [switch]$CheckLuaScripts
    )

    $env:SNES_CELESTE_ROOT = ((Resolve-Path $Root).Path -replace '\\', '/')

    if ($Compiler -ne "llvm-mos") {
        return
    }

    $mapPath = Join-Path $Root "build\mainBankZero_llvm-mos.map"
    if (!(Test-Path $mapPath)) {
        throw "Map not found: $mapPath (rebuild with llvm-mos)"
    }

    & python (Join-Path $Root "scripts\gen_mesen_symbols.py")
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to generate mesen symbols"
    }

    if ($CheckLuaScripts) {
        & python (Join-Path $Root "scripts\check_mesen_lua.py")
        if ($LASTEXITCODE -ne 0) {
            throw "Mesen Lua harness check failed"
        }
    }
}

function Resolve-MesenExecutable {
    param([string]$MesenPath = "")

    if ([string]::IsNullOrWhiteSpace($MesenPath) -and ![string]::IsNullOrWhiteSpace($env:MESEN_PATH)) {
        $MesenPath = $env:MESEN_PATH
    }

    if ([string]::IsNullOrWhiteSpace($MesenPath)) {
        $candidates = @(
            "C:\Program Files\Mesen2\Mesen.exe",
            "C:\Program Files\Mesen\Mesen.exe"
        )
        foreach ($candidate in $candidates) {
            if (Test-Path $candidate) {
                return $candidate
            }
        }
    }

    if ([string]::IsNullOrWhiteSpace($MesenPath) -or !(Test-Path $MesenPath)) {
        throw "Mesen executable not found. Pass -MesenPath explicitly."
    }

    return $MesenPath
}

function New-MesenRomCopy {
    param(
        [Parameter(Mandatory = $true)][string]$SourceRom,
        [Parameter(Mandatory = $true)][string]$OutputRom
    )

    $romBytes = [System.IO.File]::ReadAllBytes($SourceRom)
    $hasCopierHeader = (($romBytes.Length % 1024) -eq 512)
    $fs = [System.IO.File]::Open($OutputRom, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
    try {
        if (!$hasCopierHeader) {
            $dummyHeader = New-Object byte[] 512
            $fs.Write($dummyHeader, 0, $dummyHeader.Length)
        }
        $fs.Write($romBytes, 0, $romBytes.Length)
    } finally {
        $fs.Close()
    }
}

function Stop-MesenProcesses {
    Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | ForEach-Object {
        try { Stop-Process -Id $_.Id -Force -ErrorAction Stop } catch { }
    }
    Start-Sleep -Milliseconds 250
}

function Wait-MesenResult {
    param(
        [Parameter(Mandatory = $true)][string]$ResultPath,
        [Parameter(Mandatory = $true)][int]$TimeoutSeconds,
        [Parameter(Mandatory = $true)][scriptblock]$IsFinal
    )

    $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
    $txt = "RUNNING"
    while ((Get-Date) -lt $deadline) {
        Start-Sleep -Milliseconds 250
        if (Test-Path $ResultPath) {
            $raw = Get-Content -Raw -ErrorAction SilentlyContinue $ResultPath
            if (![string]::IsNullOrWhiteSpace($raw)) {
                $txt = $raw.Trim()
                if (& $IsFinal $txt) {
                    break
                }
            }
        }
    }
    return $txt
}

function Invoke-MesenProcess {
    param(
        [Parameter(Mandatory = $true)][string]$MesenPath,
        [Parameter(Mandatory = $true)][string[]]$ArgumentList,
        [Parameter(Mandatory = $true)][string]$ResultPath,
        [Parameter(Mandatory = $true)][int]$TimeoutSeconds,
        [Parameter(Mandatory = $true)][scriptblock]$IsFinal
    )

    Stop-MesenProcesses
    $before = @(Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | Select-Object -ExpandProperty Id)
    $null = Start-Process -FilePath $MesenPath -ArgumentList $ArgumentList -PassThru
    $txt = Wait-MesenResult -ResultPath $ResultPath -TimeoutSeconds $TimeoutSeconds -IsFinal $IsFinal
    $after = @(Get-Process | Where-Object { $_.ProcessName -like "Mesen*" } | Select-Object -ExpandProperty Id)
    $newPids = $after | Where-Object { $_ -notin $before }
    foreach ($procId in $newPids) {
        try { Stop-Process -Id $procId -Force -ErrorAction Stop } catch { }
    }
    return $txt
}
