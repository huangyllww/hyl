--------------------------------------------------------------------------
@Description:
  OemDxeMain Module -  OemDxe 主模块，通过提供OEM_DXE_HOOK E-LINK，供其他相关功能连接

@time:
  ###### Thu Aug 26 14:14:41 CST 2021

@KeyParts:
  OEM_DXE_HOOK  - 功能函数Hook
  OEM_DXE_HOOK_FILES_LINK - 文件Link接口，用于创建OemDxeMain.inf,并参加编译
  AdditionalOemDxeHookPackages - Packages Link接口，用于创建OemDxeMain.inf,并参加编译
  AdditionalOemDxeHookLibraryClasses - LibraryClasses Link接口，用于创建OemDxeMain.inf,并参加编译
  AdditionalOemDxeHookProtocols - Protocols Link接口，用于创建OemDxeMain.inf,并参加编译
  AdditionalOemDxeHookGuids - Guids Link接口，用于创建OemDxeMain.inf,并参加编译
  AdditionalOemDxeHookPcds - Pcds Link接口，用于创建OemDxeMain.inf,并参加编译

--------------------------------------------------------------------------
@Description:
  OemBiosPostComplete Module -  提供BIOS Post Complete 信号功能，功能默认关闭

@time:
  ###### Thu Aug 26 14:24:31 CST 2021

@KeyParts:
  BIOS_POST_COMPLT_PIN - 定义BIOS Post Complete的 GPIO 信号，默认为137

--------------------------------------------------------------------------

@Description:
  OemPCIeACSInit Module -  初始化ACS 功能，需要搭配Setup下相关设置完成

@time:
  ###### Thu Aug 26 14:24:31 CST 2021
--------------------------------------------------------------------------
@Description:
  OemPwdPolicy Module -  OEM 密码策略功能模块
  ClearPwdOnceNvramLoadDefault - CMOS Clear 之后，清除密码功能
  RestorePwdOnceNvramLoadDefault - CMOS Clear 之后，恢复密码功能
  ClearPwdByGPIO - 参考GPIO 信号，清除密码功能

@time:
  ###### Thu Aug 26 14:24:31 CST 2021

@KeyParts:
  LOAD_DEFAULTS_ON_CMOS_CLEAR
  RESTORE_PASSWORD_ON_CMOS_CLEAR
  BIOS_PASSWORD_CLEAR_PIN_SUPPORT
--------------------------------------------------------------------------
@Description:
  OemRTCInit - CMOS 清除之后，在安装 gEfiRealTimeClockArchProtocol时初始化RTC 时间

@time:
  ###### Fri Aug 27 13:40:10 CST 2021

@KeyParts:
  DEFAULT_TIME_PROJECT_YEAR
  EFI_UNSPECIFIED_TIMEZONE
