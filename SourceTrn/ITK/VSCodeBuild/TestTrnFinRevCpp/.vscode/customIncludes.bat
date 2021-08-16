:: ===============================================================
::
:: This batch is user's editable
:: Add here the custom paths for includes and libraries and
::   all the custom libraries
::
:: Usage
::  -Add Include Path
::      call :addIncludePath <Path>
::
::  -Add Library Path
::      call :addLibPath <Path>
::
::  -Add Include Path
::      call :addUserLib <Path>
::
:: IMPORTANT INFORMATION
::   -Please avoid absolute paths. 
::    All paths shall be relative to the WORKSPACEFOLDER variable
::    which represents the current folder
::    e.g.: %WORKSPACEFOLDER%\..\..\include_common
::   
::   -The src folder is included by default (i.e. do not add '.')
::
::   -The SOA folders are included by default, but libraries shall
::    be explicitly added
::
::   -The TC include folders are included by default
::
:: ===============================================================

:: Start edit from here-----------------------------------------------------------------------------------------
call :addIncludePath %WORKSPACEFOLDER%\..\Lib
call :addUserLib libmetaframework.lib
:: Do not edit after this line---------------------------------------------------------------------------------
exit /b 0

:addIncludePath
set USER_INCLUDES=%USER_INCLUDES% -I%1
exit /b 0

:addLibPath
set LIB_PATH=%LIB_PATH% %LP%%1
exit /b 0

:addUserLib
set USER_LIBS=%USER_LIBS% %1
exit /b 0