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

#**********************************************************************
#<AMI_FHDR_START>
#
# Name:		SB.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
Prepare : $(BUILD_DIR)/SbDxeInitElink.h $(BUILD_DIR)/SbRunInitElink.h

$(BUILD_DIR)/SbDxeInitElink.h :  $(BUILD_DIR)\token.mak
	$(ECHO) \
"#define SAVE_RESTORE_CALLBACK_LIST $(SbTimerSaveRestoreRegistersCallbacks)$(EOL)"\
> $@

$(BUILD_DIR)/SbRunInitElink.h : $(BUILD_DIR)\token.mak
	$(ECHO) \
"#define SB_RUN_RESET_CALLBACK_LIST $(SB_RUN_RESET_CALLBACK_LIST)$(EOL)\
#define SB_OEM_S3_WARMRESET_CALLBACK_LIST $(SB_OEM_S3_WARMRESET_CALLBACK_LIST)$(EOL)"\
> $@

AMI_CSP_LIB_INCLUDE_FILES := \
 $(SB_INCLUDE_DIR)$(PATH_SLASH)Sb.h \
 $(SB_LIBRARY_INCLUDE_DIR)$(PATH_SLASH)SbCspLib.h \
 $(AmiChipsetPkg_Include_Library_DIR)$(PATH_SLASH)AmiChipsetIoLib.h \
$(AMI_CSP_LIB_INCLUDE_FILES)


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
