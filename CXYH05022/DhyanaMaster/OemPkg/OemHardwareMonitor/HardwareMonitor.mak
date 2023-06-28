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
# Name:  HardwareMonitor.mak
#
# Description:	Make file for HardwareMonitor Module
#
#<AMI_FHDR_END>
#**********************************************************************
ifeq ($(HardwareMonitor_SUPPORT), 1)

Prepare : $(BUILD_DIR)\SensorEntryStr.uni $(BUILD_DIR)/SensorEntry.sd

$(BUILD_DIR)\SensorEntryStr.uni $(BUILD_DIR)/SensorEntry.sd:
	copy OemPkg\OemHardwareMonitor\SensorEntry.uni  $(BUILD_DIR)\SensorEntryStr.uni
	OemPkg\OemHardwareMonitor\GenSensorEntry.bat $(SENSOR_ENTRY_NUMBER)

clean:
	del $(BUILD_DIR)\SensorEntryStr.uni
	del $(BUILD_DIR)\SensorEntry.sd	

endif	#ifeq ($(HardwareMonitor_SUPPORT), 1)

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
