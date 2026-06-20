@if not defined VBCC_HOME set VBCC_HOME=C:\vbcc65816\vbcc65816\vbcc65816_win\vbcc
@set VBCC=%VBCC_HOME%
@set PATH=%VBCC%\bin;%PATH%
@set VBCC_PATH=%VBCC%
@"%VBCC%\bin\vlink.exe" %*
