@echo off
setlocal
if not defined LLVM_MOS_PATH set "LLVM_MOS_PATH=C:\llvm-mos"
if exist "%LLVM_MOS_PATH%\devEnv.bat" call "%LLVM_MOS_PATH%\devEnv.bat"
if not exist build mkdir build
%*
