#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**              5555 Oak brook Pkwy, Norcorss, GA 30093             **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header:  $
#
# $Revision:  $
#
# $Date:  $
#**********************************************************************
# Revision History
# ----------------
# $Log:  $
## 
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	Supportfiles.mak
#
# Description:	Supportfiles 
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare : AmiFwBmcGenStrIncludeFile 

AmiFwBmcGenStrIncludeFile: $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#include <../$(BUILD_DIR)$(PATH_SLASH)$(PLATFORM_NAME)$(PATH_SLASH)$(TARGET)_$(TOOL_CHAIN_TAG)$(PATH_SLASH)$(AMITSE_TARGET_ARCH)/$(AMIFWUPDATE_BMC_DIR)/AmiFwUpdateBmc/DEBUG/AmiFwUpdateBmcStrDefs.h> "\
>$(BUILD_DIR)/AmiFwUpdateBmcStrDefs.h

#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**             5555 Oakbrook Pkwy, Norcross, Georgia 30093          **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
