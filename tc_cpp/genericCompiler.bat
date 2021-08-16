@echo off
cls
setLocal ENABLEDELAYEDEXPANSION

REM set WORKSPACEFOLDER=%CD%

set COMPILESCRIPT="%~dp0compile"

if "%2"=="-exe" ( 
	set LINKFILENAME=linkitk
) else if "%2"=="-ch" ( 
	set LINKFILENAME=link_custom_exits
) else (
	echo Unsupported build type %2
	exit /b 1
)

set LINKSCRIPT="%~dp0%LINKFILENAME%"

if exist "%WORKSPACEFOLDER%\.vscode\compile.bat" (
	set COMPILESCRIPT="%WORKSPACEFOLDER%\.vscode\compile"
)
if exist "%WORKSPACEFOLDER%\.vscode\%LINKFILENAME%.bat" (
	set LINKSCRIPT="%WORKSPACEFOLDER%\.vscode\%LINKFILENAME%"
)

echo.
echo ==============================================================================
echo Compile script is: %COMPILESCRIPT%
echo Link script is: %LINKSCRIPT%
echo ==============================================================================
echo.

set CAN_LINK==0
set result=0

set REBUILDALL=

REM if "%2"=="-exe" (
REM 	set BUILDTYPE=
REM ) else if "%2"=="-dll" (
REM 	set BUILDTYPE=-dll
REM )

if "%3"=="-rebuild" (
	set REBUILDALL=1
	rd /s /q output
)

if not exist output (
	md output
)
cd output



set USER_INCLUDES=-I. -Isrc
set SOA_INCLUDES=-I%SOA_CLIENT_HOME%\cpp\includes\cpp\include -I%SOA_CLIENT_HOME%\toolbox\xml4c\include
set SOAMNGD_INCLUDES=-I%SOA_CLIENT_HOME%\cpp\includes\cpp-managed\include
set TC_DEFAULT_INCLUDES=-I%TC_INCLUDE% -I%TC_ROOT%\include_cpp
set PLAT=wntx64
set LP=-LIBPATH:

set LIB_PATH=%LP%%SOA_CLIENT_HOME%\cpp\libs\%PLAT% %LP%%TC_ROOT%\lib
set USER_LIBS=

rem call custom includes
call "%WORKSPACEFOLDER%\.vscode\customIncludes.bat"

set ALL_INCLUDES=%USER_INCLUDES% %SOA_INCLUDES% %SOAMNGD_INCLUDES% %TC_DEFAULT_INCLUDES%

set ALLFILES=
if not defined IPLIB (
	set IPLIB=none
)

for /f %%f in ('dir /s /b ..\src\*.c*') do (
	set ALLFILES=!ALLFILES! %%f
)
if defined ALLFILES (
	ECHO.
	ECHO.
	ECHO *******************************************
	ECHO                 COMPILING
	ECHO *******************************************
	echo.
	call %COMPILESCRIPT% -debug -DIPLIB=%IPLIB% !ALLFILES!
	if not "!ERRORLEVEL!"=="0" (
		set CAN_LINK=1
	)
) else (
	ECHO.
	ECHO Nothing to do, skip compile
	ECHO.
)

if %CAN_LINK%==0 (
	ECHO.
	ECHO.
	ECHO *******************************************
	ECHO                  LINKING
	ECHO *******************************************
	echo.
	if exist ..\bin (
		rd /s /q ..\bin
	)
	md ..\bin
	set LINK_FAIL=0
	if "%2"=="-ch" (
		call %LINKSCRIPT% %1
	) else (
		call %LINKSCRIPT% -o %1 *.obj
	)
	if not "!ERRORLEVEL!"=="0" (
		set LINK_FAIL=1
	)
	cd ..\bin
	del *.ilk *.lib *.exp
	ECHO.
	if !LINK_FAIL!==0 (
		ECHO =========SUCCESS===========
	) else (
		ECHO =========FAILED===========
		set result=1
	)
) else (
	ECHO.
	ECHO =========FAILED===========
	set result=1
)

cd /d %WORKSPACEFOLDER%
del /q *.pdb >nul
if %result%==0 (
	call "%WORKSPACEFOLDER%\.vscode\postBuild.bat"
)



exit /b %result%

:isFileNewer
rem returns 1 if %1 is newer than %2, 0 otherwise
set %3=0
xcopy /L /D /Y %1 %2*|findstr /B /C:"1 " >NUL && set %3=1
exit /b 0