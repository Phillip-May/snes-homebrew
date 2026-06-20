@echo off
if not defined LLVM_MOS_HOME set LLVM_MOS_HOME=C:\llvm-mos
call "%LLVM_MOS_HOME%\devEnv.bat"
if not exist build mkdir build
%*
