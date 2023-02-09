#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
ifdef CBSBUILD_SUPPORT
ifeq ($(CBSBUILD_SUPPORT), 1)
Prepare: CBS_XML APCB_V2 
else
Prepare: APCB_V2
endif
else
Prepare: APCB_V2
endif



export WINDDK_PATH := $(CCX86DIR)\..\..\..

ifeq ($(X64_SUPPORT),1)
	export PATH:=$(CCX64DIR);$(PATH)
	export INCLUDE:=$(WINDDK_PATH)$(PATH_SLASH)inc$(PATH_SLASH)crt;$(WINDDK_PATH)$(PATH_SLASH)inc$(PATH_SLASH)api:$(INCLUDE)
	export LIB:=$(WINDDK_PATH)$(PATH_SLASH)lib$(PATH_SLASH)Crt$(PATH_SLASH)amd64;$(WINDDK_PATH)$(PATH_SLASH)lib$(PATH_SLASH)win7$(PATH_SLASH)amd64;$(LIB)
else
	export PATH:=$(CCX86DIR);$(PATH)
	export INCLUDE:=$(WINDDK_PATH)$(PATH_SLASH)inc$(PATH_SLASH)crt;$(WINDDK_PATH)$(PATH_SLASH)inc$(PATH_SLASH)api;$(INCLUDE)
	export LIB:=$(WINDDK_PATH)$(PATH_SLASH)lib$(PATH_SLASH)Crt$(PATH_SLASH)i386;$(WINDDK_PATH)$(PATH_SLASH)lib$(PATH_SLASH)win7$(PATH_SLASH)i386;$(LIB)
endif

#workaround for BATCH .bin setting
export APCB_DATA_OUTPUT_FILE_EXT=bin
	
APCB_V2:
	cd $(APCB_SCRIPT_DIR) && $(APCB_CREATE_SCRIPT).bat clean build
#	$(CP) $(APCBTOOL_DIR)\Release\APCB_V2.bin AgesaModulePkg\Firmwares\Bootloader_A0b_Mem_rDIMM\

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************