#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************


Prepare: AmiAPCIL


AmiAPCIL:
ifeq ($(Build_PSP_EarlyVGA),1)
	if exist $(PSP_EARLY_VGA_FILENAME) $(RM) $(PSP_EARLY_VGA_FILENAME)
	$(AMI_APCIL_TOOL) $(AmiPsPEarlyVga_DIR)$(PATH_SLASH)AST2500_Part1.apcil $(BUILD_DIR)$(PATH_SLASH)AST2500_Part1.bin
	$(AMI_APCIL_TOOL) $(AmiPsPEarlyVga_DIR)$(PATH_SLASH)AST2500_Part2.apcil $(BUILD_DIR)$(PATH_SLASH)AST2500_Part2.bin
	$(AMI_BMP_APCIL_TOOL) $(PCIBAR_SPACE) $(PSP_EARLY_VGA_OEMLOGO_FILENAME) $(BUILD_DIR)$(PATH_SLASH)oemlogo.apcil $(BUILD_DIR)$(PATH_SLASH)oemlogo_apcil.bin
	$(CP) /b $(BUILD_DIR)$(PATH_SLASH)AST2500_Part1.bin+$(BUILD_DIR)$(PATH_SLASH)oemlogo_apcil.bin+$(BUILD_DIR)$(PATH_SLASH)AST2500_Part2.bin $(PSP_EARLY_VGA_FILENAME)
endif



#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************