#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
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
# $Header: /Alaska/Tools/template.mak 6     1/13/10 2:13p Felixp $
#
# $Revision: 6 $
#
# $Date: 1/13/10 2:13p $
#**********************************************************************
# Revision History
# ----------------
# $Log: /Alaska/Tools/template.mak $
# 
# 6     1/13/10 2:13p Felixp
# 
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:	TSEOEM.mak
#
# Description:	
#
#<AMI_FHDR_END>
#**********************************************************************
$(BUILD_DIR)\HotKeyMsg.obj : $(OemTSE_DIR)\HotKeyMsg.c
	$(CC) $(CFLAGS) /Fo$(BUILD_DIR)\ $(OemTSE_DIR)\HotKeyMsg.c
	
$(BUILD_DIR)\PasswordOverride.obj : $(OemTSE_DIR)\PasswordOverride.c
	$(CC) $(CFLAGS) /Fo$(BUILD_DIR)\ $(OemTSE_DIR)\PasswordOverride.c

AMITSEBin : $(BUILD_DIR)\HotKeyMsg.obj $(BUILD_DIR)\PasswordOverride.obj
#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************