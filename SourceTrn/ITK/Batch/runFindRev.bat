@echo off
call ..\setEnv.bat

set TC_KEEP_SYSTEM_LOG=Y
set TC_TMP_DIR=syslogs
if not exist %TC_TMP_DIR% md %TC_TMP_DIR%

TestFindRev -id=000102 -revId=000102 
pause