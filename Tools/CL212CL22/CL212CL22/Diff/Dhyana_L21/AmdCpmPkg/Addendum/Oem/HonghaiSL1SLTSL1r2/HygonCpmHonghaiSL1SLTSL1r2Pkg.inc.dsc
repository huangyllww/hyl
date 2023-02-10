[Defines]
 DEFINE AGESA_PKG_PATH   = AgesaModulePkg
 DEFINE AGESA_PKG_DEC    = AgesaCommonModulePkg
 DEFINE FCH_PKG_PATH     = $(AGESA_PKG_PATH)/Fch/Taishan
 DEFINE FCH_PKG_DEC      = FchTaishan

[LibraryClasses]
  #
  # Cpm specific common libraries
  #
  AmdCpmBaseLib|AmdCpmPkg/Library/Proc/Base/AmdCpmBaseLib.inf
  AmdCpmCpuLib|AmdCpmPkg/Library/Proc/Cpu/AmdCpmCpu.inf
  AmdCpmFchLib|AmdCpmPkg/Library/Proc/Fch/AmdCpmFch.inf

[Components.IA32]
  AmdCpmPkg/Addendum/Oem/HonghaiSL1SLTSL1r2/Pei/HygonCpmOemInitPei/HygonCpmOemInitPeimHonghaiSL1SLTSL1r2.inf
  AmdCpmPkg/Addendum/Oem/HonghaiSL1SLTSL1r2/Pei/HygonSataWorkaround/HygonSataWorkaround.inf
  AmdCpmPkg/Kernel/Pei/AmdCpmInitPeim.inf
  AmdCpmPkg/Features/BoardId/Pei/AmdBoardIdPei.inf
  AmdCpmPkg/Features/GpioInit/Pei/AmdCpmGpioInitPeim.inf
  AmdCpmPkg/Features/I2cMaster/Pei/AmdI2cMasterPei.inf
  AmdCpmPkg/Features/PcieInit/Pei/AmdCpmPcieInitPeim.inf
  AmdCpmPkg/Devices/Ds125Br401a/Pei/Ds125Br401aPei.inf
  AmdCpmPkg/Devices/M24LC128/Pei/M24Lc128Pei.inf
  AmdCpmPkg/Devices/Pca9535a/Pei/Pca9535aPei.inf
  AmdCpmPkg/Devices/Pca9545a/Pei/Pca9545aPei.inf
  AmdCpmPkg/Devices/Sff8472/Pei/Sff8472Pei.inf
  AmdCpmPkg/Devices/At24c08d/Pei/At24c08dPei.inf
  AmdCpmPkg/Features/XgbeInit/Pei/XgbeInitPei.inf
  AmdCpmPkg/Features/EqTable/Pei/EqTable.inf

[Components.X64]
#  AmdCpmPkg/Addendum/Oem/HonghaiSL1SLTSL1r2/Dxe/HygonXgbeWorkaround/HygonXgbeWorkaround.inf
#  AmdCpmPkg/Addendum/Oem/HonghaiSL1SLTSL1r2/Dxe/PspPlatformDriver/PspPlatform.inf
  AmdCpmPkg/Kernel/Asl/AmdCpmInitAsl.inf
  AmdCpmPkg/Kernel/Dxe/AmdCpmInitDxe.inf
  AmdCpmPkg/Kernel/Smm/AmdCpmInitSmm.inf
  AmdCpmPkg/Features/BoardId/Dxe/AmdBoardIdDxe.inf
  AmdCpmPkg/Features/PcieInit/Asl/AmdCpmPcieInitAsl.inf
  AmdCpmPkg/Features/PcieInit/Dxe/AmdCpmPcieInitDxe.inf
  AmdCpmPkg/Features/I2cMaster/Dxe/AmdI2cMasterDxe.inf
  AmdCpmPkg/Features/GpioInit/Dxe/AmdCpmGpioInitDxe.inf
  AmdCpmPkg/Features/GpioInit/Smm/AmdCpmGpioInitSmm.inf
  AmdCpmPkg/Features/PlatformRas/Dxe/AmdPlatformRasDxe.inf
  AmdCpmPkg/Features/PlatformRas/Smm/AmdPlatformRasSmm.inf
  AmdCpmPkg/Features/xGbEI2cMaster/xGbEI2cMasterDxe.inf
  AmdCpmPkg/Devices/Pca9535a/Dxe/Pca9535aDxe.inf
  AmdCpmPkg/Devices/Pca9545a/Dxe/Pca9545aDxe.inf
  AmdCpmPkg/Devices/Sff8472/Dxe/Sff8472Dxe.inf

[PcdsFixedAtBuild]
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNumberOfPhysicalSocket|1

  # V9 Pcds
  # IDS Debug Lib Pcds
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutForceEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintRedirectIOEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0xfedc9000
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintFilter|0x100401008A300408

  # Customized DIMM Vendor Strings
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord0|0x2C00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord1|0xAD00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord2|0xC100
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord3|0xCE00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord4|0x4F00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord5|0x9801
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord6|0xFE02
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord7|0x0B03
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord8|0x2503
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord9|0x8303
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord10|0xCB04
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord11|0xC106

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr0|"Micron Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr1|"Hynix"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr2|"Infineon (Siemens)"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr3|"Samsung"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr4|"Transcend Information"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr5|"Kingston"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr6|"ELPIDA"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr7|"Nanya Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr8|"Kingmax Semiconductor"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr9|"Buffalo (Formerly Melco)"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr10|"A-DATA Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr11|"ASint Technology"

[PcdsDynamicDefault]
  ## Customize AGESA GNB
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgGnbIoapicAddress|0xFEC01000

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgAzaliaSsid|0xC9501D94

  ## Customize AGESA FCH
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLegacyFree|FALSE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdHpetEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdHpetMsiDis|FALSE

##  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{ \
##  0x00, 0x00, 0xB0, 0xAC, \
##  0x02, 0x48, 0x00, 0x08, \
##  0x02, 0x78, 0x00, 0x19, \
##  0x02, 0x79, 0x00, 0x19, \
##  0x02, 0x58, 0x00, 0x18, \
##  0x02, 0x59, 0x00, 0x18, \
##  0x02, 0x5A, 0x00, 0x18, \
##  0x02, 0x70, 0x00, 0x01, \
##  0xFF, 0xFF, 0xFF, 0xFF }
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{0x00, 0x00, 0xB0, 0xAC,  0x02, 0x48, 0x00, 0x08,  0x02, 0x78, 0x00, 0x19,  0x02, 0x79, 0x00, 0x19,  0x02, 0x58, 0x00, 0x18,  0x02, 0x59, 0x00, 0x18,  0x02, 0x5A, 0x00, 0x18,  0x02, 0x70, 0x00, 0x01,  0xFF, 0xFF, 0xFF, 0xFF}

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNativePcieSupport|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataClass|0x06

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhci0Enable|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcClockDriveStrength|0x1

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSmbusSsid|0x790B1D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataAhciSsid|0x79011D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaidSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaid5Ssid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataIdeSsid|0x00000000
  #gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEhciSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhciSsid|0x145C1D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcSsid|0x790E1D94
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSdSsid|0x00000000

  #gEfiAmdAgesaModulePkgTokenSpaceGuid.FchRTDeviceD3ColdMap|0
  gEfiAmdAgesaModulePkgTokenSpaceGuid.FchRTSataDevSlpPort0S5Pin|0x9

  # Platform RAS
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRasSupport|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFchApuRasSmiSupport|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNbioPoisonConsumption|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdNbioRASControl|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdApcbToBoardChanXLatTab|{0x3,0x3, 0x2,0x2, 0x7,0x7, 0x6,0x6, 0xFF,0xFF}

  # USB feature configuration
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdDieNumUsbPortAndOCPinMap|{0x00,0x0, 0x01,0x1, 0x02,0x2, 0x03,0x3, 0x04,0x0, 0x05,0x1, 0x10,0x0, 0x11,0x1, 0x12,0x2, 0x13,0x3, 0x14,0x0, 0x15,0x1, 0xFF}
  