@echo off
rem echo '%temp%' /tmp > tool\msys\etc\fstab
echo HOME='%~dp0' > tool\msys\etc\home
tool\msys\msys.bat
