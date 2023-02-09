@echo OFF
echo (C)Copyright 2015, American Megatrends, Inc.  All Rights Reserved
IF [%1] == [/?] GOTO help
IF [%1] == [/i] GOTO install
GOTO help

:install
IF [%2] == [] GOTO err1
IF NOT EXIST %2 GOTO err2

echo installing DebugLoader Plugin in %2
SET VEB_ROOT=%2
set iscopy=false

IF NOT EXIST .\com.ami.debug_1.0.0.zip GOTO err3
IF EXIST .\com.ami.debug_1.0.0.zip set iscopy=true
GOTO Extract
REM Start Copying DebugLoader Plugin in VEB Path
:Extract 
IF EXIST "%VEB_ROOT%\plugins\com.ami.debug_1.0.0" rmdir /s /q "%VEB_ROOT%\plugins\com.ami.debug_1.0.0"
if "%iscopy%"=="true" wscript loaderExtract.vbs "%cd%\com.ami.debug_1.0.0.zip" "%VEB_ROOT%\plugins\"
if %errorlevel%==0 GOTO exit

echo Extracting Scripts
IF NOT EXIST .\scripts.zip GOTO errscrips
IF EXIST "%VEB_ROOT%\scripts" rmdir /s /q "%VEB_ROOT%\scripts"
wscript loaderExtract.vbs "%cd%\scripts.zip" "%VEB_ROOT%\"
if not %errorlevel%==0 GOTO err5
REM continue register
cd %VEB_ROOT%



REM Error branch starts
:err1
echo Usage:
echo install /i [vebroot]
GOTO exit

:err2
echo Incorrect VeB path
GOTO exit

:err3
echo com.ami.debug_1.0.0.zip not present in AptioVDebugger for installation.
GOTO exit

:err5
echo AptioV Debugger Loader plugins could not be copied successfully.
GOTO exit

:errscrips
echo AptioV Debugger Script.zip not present
GOTO exit



:help
echo Usage 1:
echo install /i [vebroot]
echo [vebroot] is the path where veb is installed. The command installs com.ami.debug plugin into VEB's Plugin Path
echo.


:exit
@echo ON