@echo off
REM Build Calypsi Standard Library with Huge Attribute
REM This script rebuilds the C runtime library with --enable-huge-attribute

set CALYPSI_ROOT=C:\calypsi-65816-5.12
set SRC_DIR=%CALYPSI_ROOT%\src
set BUILD_DIR=%CALYPSI_ROOT%\lib-huge
set CC=%CALYPSI_ROOT%\bin\cc65816
set AS=%CALYPSI_ROOT%\bin\as65816
set AR=%CALYPSI_ROOT%\bin\nlib

REM Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

echo Building Calypsi Standard Library with Huge Attribute...
echo Source: %SRC_DIR%
echo Build: %BUILD_DIR%

REM Compiler flags for huge model
set CFLAGS=--core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -D__CALYPSI__=1 -I"%SRC_DIR%\lib"

REM Build lib directory
echo.
echo Building lib directory...
cd /d "%SRC_DIR%\lib"

REM Build each subdirectory
for /d %%d in (*) do (
    echo Building %%d...
    cd /d "%%d"
    for %%f in (*.c) do (
        echo   Compiling %%f...
        "%CC%" %CFLAGS% -c "%%f" -o "%BUILD_DIR%\%%~nf.o"
        if errorlevel 1 (
            echo ERROR: Failed to compile %%f
            pause
            exit /b 1
        )
    )
    cd /d ".."
)

REM Build softfloat directory
echo.
echo Building softfloat directory...
cd /d "%SRC_DIR%\softfloat"
for %%f in (*.c) do (
    echo   Compiling %%f...
    "%CC%" %CFLAGS% -c "%%f" -o "%BUILD_DIR%\%%~nf.o"
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

REM Build assembly files from lowlevel directory
echo.
echo Building assembly files...
cd /d "%SRC_DIR%\lib\lowlevel"
for %%f in (*.s) do (
    echo   Assembling %%f...
    "%AS%" --core=65816 --data-model=huge "%%f" -o "%BUILD_DIR%\%%~nf.o"
    if errorlevel 1 (
        echo ERROR: Failed to assemble %%f
        pause
        exit /b 1
    )
)

REM Create the library archive
echo.
echo Creating library archive...
cd /d "%BUILD_DIR%"
"%AR%" clib-huge.a *.o
if errorlevel 1 (
    echo ERROR: Failed to create library archive
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Library created: %BUILD_DIR%\clib-huge.a
echo.
echo To use this library, update your Makefile to link with clib-huge.a instead of clib-lc-ld.a
echo.

pause
