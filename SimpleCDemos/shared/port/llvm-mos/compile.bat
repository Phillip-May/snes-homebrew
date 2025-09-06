@echo off
call C:\llvm-mos\devEnv.bat
if not exist build mkdir build
%*
