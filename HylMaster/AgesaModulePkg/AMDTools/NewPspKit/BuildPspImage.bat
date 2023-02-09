:: *****************************************************************************
:: *
:: *
:: * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
:: *
:: * HYGON is granting you permission to use this software (the Materials)
:: * pursuant to the terms and conditions of your Software License Agreement
:: * with HYGON.  This header does *NOT* give you permission to use the Materials
:: * or any rights under HYGON's intellectual property.  Your use of any portion
:: * of these Materials shall constitute your acceptance of those terms and
:: * conditions.  If you do not agree to the terms and conditions of the Software
:: * License Agreement, please do not use any portion of these Materials.
:: *
:: * CONFIDENTIALITY:  The Materials and all other information, identified as
:: * confidential and provided to you by HYGON shall be kept confidential in
:: * accordance with the terms and conditions of the Software License Agreement.
:: *
:: * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
:: * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
:: * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
:: * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
:: * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
:: * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
:: * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
:: * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
:: * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
:: * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
:: * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
:: * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
:: * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
:: *
:: * HYGON does not assume any responsibility for any errors which may appear in
:: * the Materials or any other related information provided to you by HYGON, or
:: * result from use of the Materials or any related information.
:: *
:: * You agree that you will not reverse engineer or decompile the Materials.
:: *
:: * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
:: * further information, software, technical information, know-how, or show-how
:: * available to you.  Additionally, HYGON retains the right to modify the
:: * Materials at any time, without notice, and is not obligated to provide such
:: * modified Materials to you.
:: *
:: * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
:: * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
:: * the right to make the modified version available for use with HYGON's PRODUCT.
:: ******************************************************************************

:: BuildPspImage InputBiosFile rtmfile PspDirectoryCfg OutputBiosFile KeyMode

@echo off
::Flag to control use python script or executable file generate from python
::TRUE:   Script file
::FALSE:  Executable file
IF "%USE_PYTHON_SCRIPT%" == "" (
  @set USE_PYTHON_SCRIPT=FALSE
)

::Input parameter check
IF /I "%5" == "" GOTO HELP
::System variable Check
SETLOCAL EnableDelayedExpansion

if /I "%5" == "TK" (
  IF "%OPENSSL_PATH%" == "" (
    SET OPENSSL_PATH=C:\OpenSSL-Win32
  )
  ::CHECK_PATH
  IF NOT EXIST !OPENSSL_PATH!\bin\openssl.exe (
    @echo !!!OPENSSL_PATH system variable is NOT set correctly!!
    goto ERROR
  )
)
IF /I "%USE_PYTHON_SCRIPT%" == "TRUE" (
  IF EXIST "%PYTHON_PATH%\python.exe" (
    @set BUILDPSPDIRECTORY=%PYTHON_PATH%\python.exe BuildPspDirectory.py
  )ELSE IF EXIST "%PYTHON_PATH%\bin\python.exe" (
    @set BUILDPSPDIRECTORY=%PYTHON_PATH%\bin\python.exe BuildPspDirectory.py
  )ELSE (
    @echo !!!PYTHON_PATH system variable is NOT set correctly!!
    goto ERROR
  )
)ELSE (
  @set BUILDPSPDIRECTORY=BuildPspDirectory.exe
)

IF "%PSPKIT_PATH%" == "" (
  SET PSPKIT_PATH=%cd%
)
::CHECK_PATH
IF NOT EXIST %PSPKIT_PATH%\BuildPspImage.bat (
  @echo !!!PSPKIT_PATH system variable is NOT set correctly!!
  goto ERROR
)

IF "%PSP_FW_PATH%" == "" (
  SET PSP_FW_PATH=%cd%
)


if /I "%5" == "PK" (
  IF "%SPCLIENT_PATH%" == "" (
    set SPCLIENT_PATH=C:\SPClient
  )
  IF NOT EXIST !SPCLIENT_PATH!\SPClient.jar (
    @echo !!!SPCLIENT_PATH enviroment variable haven't been set correctly
    goto ERROR
  )
  IF "%SP_FUNCTION%" == "" (
    @echo !!!SP_FUNCTION enviroment variable haven't been set correctly
    goto ERROR
  )
)

IF "%TEMP_PATH%" == "" (
  @SET TEMP_PATH=%PSPKIT_PATH%\Temp
)

:START
::BIOS images
@set InputBiosFile=%1
@set RTM_FILE=%2
@set PspDirectoryCfg=%3
@set FINAL_BIOS=%4

::Related PATH
@SET PSP_DIR_TOOL_PATH=%PSPKIT_PATH%\PspDirectoryTool

::Create Temporary folder, and copy all files to it
@echo rd    %TEMP_PATH% /S /Q
IF EXIST %TEMP_PATH% rd    %TEMP_PATH% /S /Q
@echo mkdir %TEMP_PATH%
mkdir %TEMP_PATH%
@echo copy  %PSP_DIR_TOOL_PATH%\*.*  %TEMP_PATH% /Y
copy  %PSP_DIR_TOOL_PATH%\*.*  %TEMP_PATH% /Y
@echo copy  %PSP_FW_PATH%\*.*        %TEMP_PATH% /Y
@echo D | xcopy  %PSP_FW_PATH%\*.*   %TEMP_PATH%\AgesaModulePkg\Firmwares /S /Y
@echo copy  %PSPKIT_PATH%\*.*       %TEMP_PATH% /Y
copy  %PSPKIT_PATH%\*.*       %TEMP_PATH% /Y
IF NOT "%CUSTOM_APCB_PATH%" == "" (
  @echo copy  %CUSTOM_APCB_PATH%\*.*   %TEMP_PATH% /Y
  mkdir %TEMP_PATH%\Build\Apcb\Release
  copy  %CUSTOM_APCB_PATH%\*.*   %TEMP_PATH%\Build\Apcb\Release /Y
)

if /I "%5" == "TK" (
@echo xcopy %OPENSSL_PATH%\*.*       %TEMP_PATH% /S /Y
  xcopy %OPENSSL_PATH%\*.*       %TEMP_PATH% /S /Y
)

if /I NOT "%5" == "NOSIGN" (
@echo copy  %RTM_FILE%               %TEMP_PATH% /Y
  IF EXIST %RTM_FILE% copy  %RTM_FILE%               %TEMP_PATH% /Y
)

::TOOLS
@set OPENSSL=bin\openssl.exe
::BINARYS
@set RTM_PRIVATE_KEY=TestRtmPrivateKey.pem
@set BIOS_L1_DIR_FILE=Output\BiosDirHeaderL1.bin
@set BIOS_L2_DIR_FILE=Output\BiosDirHeaderL2.bin
@set RTM_BIOSDIRL1_COMBINE_FILE=BiosRtmBiosL1Combine.bin
@set RTM_BIOSDIRL1_L2_COMBINE_FILE=BiosRtmBiosL1L2Combine.bin
@set RTM_FILE_L1_SIGNATURE=RTMSignature.bin
@set RTM_FILE_L1_L2_SIGNATURE=RTMSignatureL1L2.bin

pushd %TEMP_PATH%
::delete pyd & python27.dll which may cause the compatible issue with python installed on the machine when USE_PYTHON_SCRIPT=TRUE
IF /I "%USE_PYTHON_SCRIPT%" == "TRUE" (
  IF EXIST *.pyd del  *.pyd /F /Q
  IF EXIST python27.dll del  python27.dll /F /Q
)

@echo.
@echo [Generate PSP Directory]
::Build without embed RTMSignature
if /I "%5" == "NOSIGN" goto BLDBIOS
IF EXIST %BIOS_L1_DIR_FILE% del %BIOS_L1_DIR_FILE% /F /Q
IF EXIST %BIOS_L2_DIR_FILE% del %BIOS_L2_DIR_FILE% /F /Q
@echo %BUILDPSPDIRECTORY% bd %InputBiosFile% %PspDirectoryCfg%
%BUILDPSPDIRECTORY% bd %InputBiosFile% %PspDirectoryCfg%
if %ERRORLEVEL% NEQ 0 goto ERROR


@echo.
@echo [Combine RTM - PSP Directory and sign this using publc key]
@echo copy /b %RTM_FILE%+%BIOS_L1_DIR_FILE% %RTM_BIOSDIRL1_COMBINE_FILE%
copy /b %RTM_FILE%+%BIOS_L1_DIR_FILE% %RTM_BIOSDIRL1_COMBINE_FILE%
::Check if LV2 DIR Binary exist, which imply the PSP recovery support is enabled
IF EXIST %BIOS_L2_DIR_FILE% (
  @echo copy /b %RTM_FILE%+%BIOS_L1_DIR_FILE%+%BIOS_L2_DIR_FILE% %RTM_BIOSDIRL1_L2_COMBINE_FILE%
  copy /b %RTM_FILE%+%BIOS_L1_DIR_FILE%+%BIOS_L2_DIR_FILE% %RTM_BIOSDIRL1_L2_COMBINE_FILE%
)


::Sign using Test Key
if /I "%5" == "TK" (
  @echo Make RTM Signature using test Key
  IF NOT EXIST %RTM_PRIVATE_KEY% (
    @echo Can't find %RTM_PRIVATE_KEY%
    goto ERROR
  )
  @echo %OPENSSL% dgst -binary -sign %RTM_PRIVATE_KEY% -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-2 -out %RTM_FILE_L1_SIGNATURE% %RTM_BIOSDIRL1_COMBINE_FILE%
  %OPENSSL% dgst -binary -sign %RTM_PRIVATE_KEY% -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-2 -out %RTM_FILE_L1_SIGNATURE% %RTM_BIOSDIRL1_COMBINE_FILE%
  @echo reversestr.exe %RTM_FILE_L1_SIGNATURE%
  reversestr.exe %RTM_FILE_L1_SIGNATURE%

  IF EXIST %RTM_BIOSDIRL1_L2_COMBINE_FILE% (
    @echo %OPENSSL% dgst -binary -sign %RTM_PRIVATE_KEY% -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-2 -out %RTM_FILE_L1_L2_SIGNATURE% %RTM_BIOSDIRL1_L2_COMBINE_FILE%
    %OPENSSL% dgst -binary -sign %RTM_PRIVATE_KEY% -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-2 -out %RTM_FILE_L1_L2_SIGNATURE% %RTM_BIOSDIRL1_L2_COMBINE_FILE%
    @echo reversestr.exe %RTM_FILE_L1_L2_SIGNATURE%
    reversestr.exe %RTM_FILE_L1_L2_SIGNATURE%
  )
  goto BLDBIOS
)
::Sign using production Key
if /I "%5" == "PK" (
  @echo Make RTM Signature using production Key
  java -version
  @echo SPCLIENT_PATH=%SPCLIENT_PATH%
  pushd %SPCLIENT_PATH%
  @echo del %RTM_FILE_L1_SIGNATURE% /F /Q
  IF EXIST %RTM_FILE_L1_SIGNATURE% del %RTM_FILE_L1_SIGNATURE% /F /Q
  @echo del %RTM_FILE_L1_L2_SIGNATURE% /F /Q
  IF EXIST %RTM_FILE_L1_L2_SIGNATURE% del %RTM_FILE_L1_L2_SIGNATURE% /F /Q
  @echo call SPClient.bat -in %cd%\%RTM_BIOSDIRL1_COMBINE_FILE% -out %RTM_FILE_L1_SIGNATURE% -sp_function "%SP_FUNCTION%"
  call SPClient.bat -in %cd%\%RTM_BIOSDIRL1_COMBINE_FILE% -out %RTM_FILE_L1_SIGNATURE% -sp_function "%SP_FUNCTION%"
  ::Generate Signature for RTM+L1+L2
  IF EXIST %cd%\%RTM_BIOSDIRL1_L2_COMBINE_FILE% (
    @echo  SPClient.bat -in %cd%\%RTM_BIOSDIRL1_L2_COMBINE_FILE% -out %RTM_FILE_L1_L2_SIGNATURE% -sp_function "%SP_FUNCTION%"
    call SPClient.bat -in %cd%\%RTM_BIOSDIRL1_L2_COMBINE_FILE% -out %RTM_FILE_L1_L2_SIGNATURE% -sp_function "%SP_FUNCTION%"
  )

  @echo Copy back to Working path, and delete file in SPClient path
  @echo copy /Y /B %RTM_FILE_L1_SIGNATURE% %TEMP_PATH%
  copy /Y /B %RTM_FILE_L1_SIGNATURE% %TEMP_PATH%
  @echo del %RTM_FILE_L1_SIGNATURE% /F /Q
  IF EXIST %RTM_FILE_L1_SIGNATURE% del %RTM_FILE_L1_SIGNATURE% /F /Q

  IF EXIST %RTM_FILE_L1_L2_SIGNATURE% (
    @echo copy /Y /B %RTM_FILE_L1_L2_SIGNATURE% %TEMP_PATH%
    copy /Y /B %RTM_FILE_L1_L2_SIGNATURE% %TEMP_PATH%
    @echo del %RTM_FILE_L1_L2_SIGNATURE% /F /Q
    IF EXIST %RTM_FILE_L1_L2_SIGNATURE% del %RTM_FILE_L1_L2_SIGNATURE% /F /Q
  )

  popd
  if %ERRORLEVEL% NEQ 0 (
    @echo [Warning] Error Detected on SPClient tool
    goto ERROR
  )
  goto BLDBIOS
)

@echo Invalid KeyMode
goto HELP

:BLDBIOS


@echo.
@echo [Finally build the Final BiosImage]
@echo %BUILDPSPDIRECTORY% bb %InputBiosFile% %PspDirectoryCfg% %FINAL_BIOS%
%BUILDPSPDIRECTORY% bb %InputBiosFile% %PspDirectoryCfg% %FINAL_BIOS%

if %ERRORLEVEL% NEQ 0 goto ERROR
@echo copy Output\%FINAL_BIOS% ..\
copy Output\%FINAL_BIOS% ..\
popd
::rd %TEMP_PATH% /S /Q
@echo.
@echo PSP contents have been embed to %FINAL_BIOS%

GOTO END

:ERROR
@echo **** Build PSP Image Fail ****
@exit /B 2
GOTO END

:HELP
@echo Embed PSP contents to Specific BIOS image
@echo.
@echo BuildPspImage.bat rtmfile PspDirectoryCfg OutputBiosFile
@echo   Positional parameters:
@echo     rtmfile           The Root trunk module of BIOS, commonly it is the SEC,PEI combined FV image
@echo     PspDirectoryCfg   Config file which describe PSP directory layout, and input bios informations
@echo     OutputBiosFile    The output bios image after embed the PSP contents
@echo     KeyMode           "TK|PK" Specify which key will be used, test key or production key
goto END
:END
