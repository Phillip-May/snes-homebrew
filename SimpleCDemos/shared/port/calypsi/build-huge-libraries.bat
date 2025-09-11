@echo off
REM Build Calypsi C libraries with huge attribute enabled
REM This script builds the C runtime library with --enable-huge-attribute
REM for use with the huge data/code model

set CALYPSI_PATH=C:\calypsi-65816-5.11
set SRC_DIR=%CALYPSI_PATH%\src
set LIB_DIR=%CALYPSI_PATH%\lib
set BUILD_DIR=%CALYPSI_PATH%\library-build

echo Building Calypsi C libraries with huge attribute...
echo Source directory: %SRC_DIR%
echo Library directory: %LIB_DIR%
echo Build directory: %BUILD_DIR%

REM Set up environment
call "%CALYPSI_PATH%\devEnv.bat"

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Build clib-hd-hd (large code model, large data model with huge attribute)
echo.
echo Building clib-hd-hd (large code model, large data model with huge attribute)...
cd /d "%SRC_DIR%"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/assert/lib_assert.c -o "%BUILD_DIR%/lib_assert.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/ctype/ctype.c -o "%BUILD_DIR%/ctype.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/errno/errno.c -o "%BUILD_DIR%/errno.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/fenv/fenv.c -o "%BUILD_DIR%/fenv.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/locale/lib_localeconv.c -o "%BUILD_DIR%/lib_localeconv.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/locale/lib_setlocale.c -o "%BUILD_DIR%/lib_setlocale.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/signal/sig_raise.c -o "%BUILD_DIR%/sig_raise.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/signal/sig_signal.c -o "%BUILD_DIR%/sig_signal.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/wctype/lib_iswctype.c -o "%BUILD_DIR%/lib_iswctype.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/wctype/lib_towlower.c -o "%BUILD_DIR%/lib_towlower.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/wctype/lib_towupper.c -o "%BUILD_DIR%/lib_towupper.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/wctype/lib_wctype.c -o "%BUILD_DIR%/lib_wctype.o"

REM Build lowlevel components
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/builtin.s -o "%BUILD_DIR%/builtin.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/controlFlow.s -o "%BUILD_DIR%/controlFlow.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/cstartup.s -o "%BUILD_DIR%/cstartup.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/defaultExit.s -o "%BUILD_DIR%/defaultExit.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/float32.s -o "%BUILD_DIR%/float32.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/float64.s -o "%BUILD_DIR%/float64.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/initialize_large.c -o "%BUILD_DIR%/initialize_large.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/initialize.c -o "%BUILD_DIR%/initialize.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/integer.s -o "%BUILD_DIR%/integer.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/memory.s -o "%BUILD_DIR%/memory.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/memcpy_far.c -o "%BUILD_DIR%/memcpy_far.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/memset_far.c -o "%BUILD_DIR%/memset_far.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/pseudoRegisters.s -o "%BUILD_DIR%/pseudoRegisters.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/setjmp.s -o "%BUILD_DIR%/setjmp.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/spill.s -o "%BUILD_DIR%/spill.o"
cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c lib/lowlevel/vswitch.c -o "%BUILD_DIR%/vswitch.o"

REM Build stdio components
for %%f in (lib/stdio/*.c) do (
    echo Compiling %%f...
    cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c "%%f" -o "%BUILD_DIR%/%%~nf.o"
)

REM Build stdlib components
for %%f in (lib/stdlib/*.c) do (
    echo Compiling %%f...
    cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c "%%f" -o "%BUILD_DIR%/%%~nf.o"
)

REM Build string components
for %%f in (lib/string/*.c) do (
    echo Compiling %%f...
    cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c "%%f" -o "%BUILD_DIR%/%%~nf.o"
)

REM Build time components
for %%f in (lib/time/*.c) do (
    echo Compiling %%f...
    cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c "%%f" -o "%BUILD_DIR%/%%~nf.o"
)

REM Build wchar components
for %%f in (lib/wchar/*.c) do (
    echo Compiling %%f...
    cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c "%%f" -o "%BUILD_DIR%/%%~nf.o"
)

REM Build softfloat components
for %%f in (softfloat/*.c) do (
    echo Compiling %%f...
    cc65816 --core=65816 -O2 --speed --code-model=large --data-model=large --enable-huge-attribute -c "%%f" -o "%BUILD_DIR%/%%~nf.o"
)

REM Create the library archive
echo.
echo Creating clib-hd-hd.a library...
cd /d "%BUILD_DIR%"
nlib -c clib-hd-hd *.o

REM Copy to lib directory
copy clib-hd-hd "%LIB_DIR%\clib-hd-hd.a"

echo.
echo Library build complete!
echo Created: %LIB_DIR%\clib-hd-hd.a
echo.
echo You can now use ROM_TYPE=huge in your builds.
echo Example: make COMPILER=calypsi ROM_TYPE=huge
