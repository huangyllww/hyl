#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
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
#
# Name: SmbiosBoard.mak
#
# Description:
# Make file to build SmbiosOemUpdateList.h
#
#<AMI_FHDR_END>
#*************************************************************************

Prepare : $(BUILD_DIR)/SmbiosOemUpdateList.h
ifeq ($(SecSMIFlash_SUPPORT), 1)
ifeq ($(SMBIOS_DMIEDIT_DATA_LOC), 0)
    $(error  SMBIOS_DMIEDIT_DATA_LOC cannot be set to 0 when SecSMIFlash_SUPPORT = 1. Please select a different setting for SMBIOS_DMIEDIT_DATA_LOC (See explaination in Smbios.chm))
endif
endif

#----------------------------------------------------------------------------
#           Create SmbiosOemUpdate Elinks header file
#----------------------------------------------------------------------------

$(BUILD_DIR)/SmbiosOemUpdateList.h : $(BUILD_DIR)/Token.h $(SMBIOS_BOARD_DIR)/SmbiosBoard.mak
	$(ECHO) \
"#define SMBIOS_OEM_UPDATE_LIST $(SmbiosOemUpdateList)$(EOL)\
" > $(BUILD_DIR)/SmbiosOemUpdateList.h

#*************************************************************************
#*************************************************************************
#**                                                                     **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.            **
#**                                                                     **
#**                       All Rights Reserved.                          **
#**                                                                     **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
#**                                                                     **
#**                       Phone: (770)-246-8600                         **
#**                                                                     **
#*************************************************************************
#*************************************************************************
