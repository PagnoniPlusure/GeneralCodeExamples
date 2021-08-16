@echo off
call ..\setEnv.bat

set TC_KEEP_SYSTEM_LOG=Y
set TC_TMP_DIR=syslogs
if not exist %TC_TMP_DIR% md %TC_TMP_DIR%

TestLogin -u=infodba -p=infodbrra -g=dba -id=000022 -revid=A
pause