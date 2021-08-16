@echo off
cls
echo Setting TC env
call "%~dp0setEnv.bat"

REM Maximum number of projects in the recentlist
SET MAXRECENTS=10

call %TC_DATA%\tc_profilevars
echo Done
echo.
echo.
call "%MSDEV_HOME%\Auxiliary\Build\vcvarsall.bat" x64

set VSCTCSCRIPTS=%~dp0
set RECENTLIST="%~dp0recentList.txt"

setlocal ENABLEDELAYEDEXPANSION

if [%1]==[] (
	goto :nofile
) else (
	goto :inputfile
)

:nofile
:paintChoices
rem Direct open, access recent file list
if not exist %RECENTLIST% (
	echo No recent files available
	pause
	exit
)

cls
echo ===========Recent projects============
echo.
set /a "II=0"
for /f "usebackq delims=" %%n IN (%RECENTLIST%) do (
	echo !II!. %%n
	set IMAX=!II!
	set folders[!II!]=%%n
	set /a "II+=1"
)
REM echo Press ENTER to exit this program
echo.
call :choiceOnScreen
echo.
if "%ANSWER%"=="" goto :eof
if "!folders[%ANSWER%]!"=="" (
	echo Error, please enter a valid value
	pause
	goto :paintChoices
)

set WORKSPACEFOLDER=!folders[%ANSWER%]!
cd /d !folders[%ANSWER%]!

rem rewrites recentList by moving the answer on top
echo !folders[%ANSWER%]!>%RECENTLIST%
if !IMAX! GEQ !MAXRECENTS! (
	set /A "IMAX=!MAXRECENTS!-1"
)
for /l %%n in (0,1,!IMAX!) do ( 
	if not "%%n"=="%ANSWER%" (
		echo !folders[%%n]!>>%RECENTLIST%
	)
)
goto :runCode

:inputfile
cd /d %~dp1
set WORKSPACEFOLDER=%CD%
set /a "IMAX=-1"
if exist %RECENTLIST% (
	set /a "II=0"
	for /f "usebackq delims=" %%n IN (%RECENTLIST%) do (
		if not "%%n"=="%WORKSPACEFOLDER%" (
			set IMAX=!II!
			set folders[!II!]=%%n
			set /a "II+=1"
		)
	)
)


if !IMAX! GEQ !MAXRECENTS! (
	set /A "IMAX=!MAXRECENTS!-1"
)
echo %WORKSPACEFOLDER% into %RECENTLIST%
echo %WORKSPACEFOLDER%>%RECENTLIST%
for /l %%n in (0,1,!IMAX!) do ( 
	echo !folders[%%n]!>>%RECENTLIST%
)


:runCode
for /f "delims=" %%d in ('where code.cmd') do (
	set codeCmd=%%~dpd
	rem only takes first occurrence
	if not [!codeCmd!]==[] goto :next
)

:next
REM pause
if exist .\.vscode\onStart.bat call .\.vscode\onStart.bat
start "" "%codeCmd%\..\Code.exe" .
goto :eof

:pressAKey
set "KEY=" & for /F "delims=" %%K in ('
    2^> nul xcopy /L /W /I "%~f0" "%TEMP%"
') do if not defined KEY set "KEY=%%K"

REM if /I "!KEY:~-1!"=="X" goto :MoreCode
set ANSWER=!KEY:~-1!
goto :eof

:choiceOnScreen
rem Echo without newline
echo|set /P=Please enter your choice or press enter to exit: 
rem Press a key (no enter required)
call :pressAKey
rem Simulates echo on screen
echo %ANSWER%
goto :eof
endlocal