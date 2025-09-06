@echo off
REM CC65 compile script for SNES
REM Sets up environment and calls the compiler

REM Set CC65 environment
set CC65_HOME=C:\cc65-2.19
set PATH=%CC65_HOME%\bin;%PATH%

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Execute the command passed to this script
%*
