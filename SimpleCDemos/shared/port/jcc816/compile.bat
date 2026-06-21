@echo off
REM JCC816 compilation wrapper
REM Usage: compile.bat [JCC816 arguments]

REM Set up JCC816 environment (JCC816_HOME may come from the Makefile/toolchains.mk)
if not defined JCC816_HOME set JCC816_HOME=C:\path\to\JCC816
call "%JCC816_HOME%\devEnvWin.bat"

REM Get the current working directory (where make was called from)
set "WORK_DIR=%CD%"

REM Create build directory
if not exist "%WORK_DIR%\build" mkdir "%WORK_DIR%\build"

REM Process arguments and convert relative paths to absolute paths
set "ARGS="
:parse_args
if "%~1"=="" goto :run_jcc816

REM Convert specific relative paths to absolute paths
if "%~1"=="-r=build" (
    set "ARGS=%ARGS% -r=%WORK_DIR%\build"
) else if "%~1"=="-l=example=../shared/port/jcc816/exampleHeader.xml" (
    set "ARGS=%ARGS% -l=example=%WORK_DIR%\..\shared\port\jcc816\exampleHeader.xml"
) else if "%~1"=="mainBankZero.c" (
    set "ARGS=%ARGS% "%WORK_DIR%\mainBankZero.c""
) else if "%~1"=="../shared/src/initsnes.c" (
    set "ARGS=%ARGS% "%WORK_DIR%\..\shared\src\initsnes.c""
) else (
    REM Pass through other arguments unchanged
    set "ARGS=%ARGS% %~1"
)

shift
goto :parse_args

:run_jcc816
REM Run JCC816 with converted arguments
jcc816.bat %ARGS%

REM Rename output to match our naming convention
if exist "%WORK_DIR%\example.sfc" (
    copy "%WORK_DIR%\example.sfc" "%WORK_DIR%\build\mainBankZero.sfc"
    del "%WORK_DIR%\example.sfc"
    echo Output: %WORK_DIR%\build\mainBankZero.sfc
)
