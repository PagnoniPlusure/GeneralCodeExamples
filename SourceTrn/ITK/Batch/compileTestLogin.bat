@echo off


call ..\setEnv.bat

REM set MSDEV_HOME=C:\PROGRA~2\MIB055~1\2017\PROFES~1\VC
set MSDEV_HOME=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC
REM call "%MSDEV_HOME%\vcvarsall.bat" x64

del *.obj *.ilk *.pdb *.exe

echo ""
echo *****************************************************************
echo *                          AMBIENTE                             *
echo *                                                               *
rem more %COMMON_DIR%\ambiente.h
set MS 
echo *                                                               *
echo *****************************************************************


call %TC_ROOT%\sample\compile -DIPLIB=none TestLogin.c
set CAN_LINK=%ERRORLEVEL%


if %CAN_LINK%==0 (
	ECHO.
	ECHO.
	ECHO *******************************************
	ECHO 		LINKING
	ECHO *******************************************
	call %TC_ROOT%\sample\linkitk -o TestLogin *.obj
	del *.obj *.ilk *.pdb						    
	pause
) else (
	del *.obj *.ilk *.pdb *.exe
	ECHO.
	ECHO =========FAILED===========
	pause
)
