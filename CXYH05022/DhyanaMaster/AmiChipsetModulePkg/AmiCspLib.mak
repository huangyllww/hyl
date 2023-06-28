#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************

#*************************************************************************
# $Header: $
#
# $Revision: $
#
# $Date: $
#*************************************************************************
#<AMI_FHDR_START>
#---------------------------------------------------------------------------
# Name:     AmiCspLib.mak
#
# Description:  
#   Make file for the CSP Library. CSP library contains all library 
#   functions related to chipset and CPU. The main idea for this library is 
#   to consolidate porting hooks related to various components 
#   (like CSM, SMM etc) in respective chipset files say NB, SB, CPU etc.  
#   This library can be built us a LIB file or as various FFS files.  
#   The MACROS and library init functions mask the actual implementation
#   which is controlled by the SDL token
#
#---------------------------------------------------------------------------
#<AMI_FHDR_END>

Prepare :  $(BUILD_DIR)/AmiCSPLibInc.h $(BUILD_DIR)/AmiCspLibPei.inf $(BUILD_DIR)/AmiCspLibDxe.inf

AMI_CSP_LIB_INCLUDE_FILES_CLEAN = $(subst +,$(SPACE),$(AMI_CSP_LIB_INCLUDE_FILES))
#---------------------------------------------------------------------------
#       Generate CSP Library
#---------------------------------------------------------------------------
$(BUILD_DIR)/AmiCSPLibInc.h : $(AMI_CSP_LIB_INCLUDE_FILES_CLEAN)
	$(ECHO) \
"#include \"token.h\"$(EOL)\
#include \"Pei.h\"$(EOL)\
$(if $(AMI_CSP_LIB_INCLUDE_FILES_CLEAN) ,\
$(foreach S_DEF, $(AMI_CSP_LIB_INCLUDE_FILES_CLEAN),#include<$(S_DEF)>$(EOL)))"\
> $(BUILD_DIR)/AmiCSPLibInc.h

$(BUILD_DIR)/AmiCspLibPei.inf : $(BUILD_DIR)/token.h $(AmiCspLib_DIR)/AmiCspLib.mak
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = AmiCspLibPei$(EOL)\
  FILE_GUID                      = 4B8F8866-D803-4b13-9A55-44BAEA414905$(EOL)\
  MODULE_TYPE                    = PEIM$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = AmiCspLib|PEIM PEI_CORE SEC $(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_SOURCES)))$(EOL)\
  $(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiChipsetModulePkg/AmiChipsetModulePkg.dec$(EOL)\
  AmiChipsetPkg/AmiChipsetPkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PACKAGES))$(EOL)\
  $(EOL)\
[Ppis]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_PPIS))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_LIBRARIES))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PEI_LIBRARIES))$(EOL)"\
[Pcd] $(EOL)\
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress$(EOL)\
  gAmiChipsetModulePkgTokenSpaceGuid.PcdGpioPortBaseAddress$(EOL)\
  gAmiChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddress$(EOL)\
> $(BUILD_DIR)/AmiCspLibPei.inf

$(BUILD_DIR)/AmiCspLibDxe.inf : $(BUILD_DIR)/token.h $(AmiCspLib_DIR)/AmiCspLib.mak
	$(ECHO)  \
"[Defines]$(EOL)\
  INF_VERSION                    = 0x00010005$(EOL)\
  BASE_NAME                      = AmiCspLibDxe$(EOL)\
  FILE_GUID                      = 94F399A7-C224-40AD-B695-FDF7EFDE4805$(EOL)\
  MODULE_TYPE                    = DXE_DRIVER$(EOL)\
  VERSION_STRING                 = 1.0$(EOL)\
  LIBRARY_CLASS                  = AmiCspLib|DXE_DRIVER DXE_CORE DXE_RUNTIME_DRIVER UEFI_DRIVER SMM_CORE DXE_SMM_DRIVER UEFI_APPLICATION$(EOL)\
$(EOL)\
[Sources]$(EOL)\
  $(patsubst %,../%,$(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_SOURCES)))$(EOL)\
  $(EOL)\
[Packages] $(EOL)\
  MdePkg/MdePkg.dec$(EOL)\
  AmiCompatibilityPkg/AmiCompatibilityPkg.dec$(EOL)\
  AmiModulePkg/AmiModulePkg.dec$(EOL)\
  AmiChipsetModulePkg/AmiChipsetModulePkg.dec$(EOL)\
  AmiChipsetPkg/AmiChipsetPkg.dec$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_PACKAGES))$(EOL)\
  $(EOL)\
[Protocols]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_PROTOCOLS))$(EOL)\
  $(EOL)\
[LibraryClasses]$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_LIBRARIES))$(EOL)\
  $(subst $(SPACE),$(EOL)$(SPACE),$(AMI_CSP_DXE_LIBRARIES))$(EOL)"\
[Pcd] $(EOL)\
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress$(EOL)\
  gAmiChipsetModulePkgTokenSpaceGuid.PcdGpioPortBaseAddress$(EOL)\
  gAmiChipsetPkgTokenSpaceGuid.PcdAcpiIoPortBaseAddress$(EOL)\
> $(BUILD_DIR)/AmiCspLibDxe.inf

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************