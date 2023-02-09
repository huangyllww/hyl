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
## @file AMISetupNVLock.mak

Prepare : $(BUILD_DIR)/NVLockElink.h

$(BUILD_DIR)/NVLockElink.h :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#define LOCK_UNLOCK_NV_LIST $(ValidateAndLockNVHook)$(EOL)\
#define VARS_TO_VALIDATE_LIST $(VarsToValidate_List)$(EOL)"\
> $(BUILD_DIR)/NVLockElink.h

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
