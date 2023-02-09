#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
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
# $Header:  $
#
# $Revision:  $
#
# $Date:  $
#**********************************************************************
#<AMI_FHDR_START>
#
# Name:  AutoGpio.mak
#
# Description:	Make file for AutoGpio Module
#
#<AMI_FHDR_END>
#**********************************************************************

Prepare : $(BUILD_DIR)\PciSlotMapList.h $(BUILD_DIR)\OemPCIeDev.h

$(BUILD_DIR)\PciSlotMapList.h : $(BUILD_DIR)/Token.mak
	@$(ECHO) \
"#include <OemPCIeDev.h>$(EOL)\
$(EOL)\
#define PCI_SLOT_MAP_LIST $(PciSlotMapList)$(EOL)\
$(EOL)\
"\
> $(BUILD_DIR)/PciSlotMapList.h

$(BUILD_DIR)\OemPCIeDev.h :
	$(CP) /Y $(OemPCIeDev_DIR)\OemPCIeDev.h $(BUILD_DIR)\OemPCIeDev.h

clean:
	del $(BUILD_DIR)\PciSlotMapList.h
	del $(BUILD_DIR)\OemPCIeDev.h


#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2012, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
