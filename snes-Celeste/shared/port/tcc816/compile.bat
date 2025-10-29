@echo off
REM TCC816 (pvsneslib) compilation wrapper
REM Usage: compile.bat [TCC816 arguments]

REM Set up pvsneslib environment
call C:\pvsneslib\devEnv.bat

REM Add pvsneslib include paths
set "ARGS=%ARGS% -IC:\pvsneslib\pvsneslib\include -IC:\pvsneslib\devkitsnes\include"

REM Get the current working directory (where make was called from)
set "WORK_DIR=%CD%"

REM Create build directory
if not exist "%WORK_DIR%\build" mkdir "%WORK_DIR%\build"

REM Process arguments and convert relative paths to absolute paths
set "ARGS="
:parse_args
if "%~1"=="" goto :run_tcc816

REM Convert specific relative paths to absolute paths
if "%~1"=="-r=build" (
    set "ARGS=%ARGS% -o %WORK_DIR%\build\mainBankZero.obj"
) else if "%~1"=="-l=example=../shared/port/tcc816/exampleHeader.xml" (
    REM TCC doesn't use linker scripts like other compilers
    REM Skip this argument
) else if "%~1"=="mainBankZero.c" (
    set "ARGS=%ARGS% "%WORK_DIR%\mainBankZero.c""
) else if "%~1"=="../shared/src/initsnes.c" (
    set "ARGS=%ARGS% "%WORK_DIR%\..\shared\src\initsnes.c""
) else if "%~1"=="-I../shared/src" (
    set "ARGS=%ARGS% -I%WORK_DIR%\..\shared\port\tcc816 -I%WORK_DIR%\..\shared\src"
) else if "%~1"=="-I." (
    set "ARGS=%ARGS% -I%WORK_DIR%"
) else (
    REM Pass through other arguments unchanged
    set "ARGS=%ARGS% %~1"
)

shift
goto :parse_args

:run_tcc816
REM Run TCC816 with converted arguments
816-tcc.exe %ARGS%

REM Check if compilation was successful
if %ERRORLEVEL% neq 0 (
    echo TCC816 compilation failed with error code %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

echo TCC816 compilation completed successfully
