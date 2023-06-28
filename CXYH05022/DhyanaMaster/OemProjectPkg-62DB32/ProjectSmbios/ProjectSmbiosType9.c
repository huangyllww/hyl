//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Uefi/UefiSpec.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Pci.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>

#include <Protocol/AmdCpmTableProtocol/AmdCpmTableProtocol.h>
#include <AmdCpmFunction.h>
#include <OemSmbios.h>
#include <ProjectSmbiosConfig.h>


AMD_CPM_TABLE_PROTOCOL      *AmdCpmTableProtocolPtr = NULL;
EFI_STATUS                  LastStatus = EFI_SUCCESS;


/**
GC_TODO: CheckGPIOSetting
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
EFI_STATUS
OemCheckPlatformSetting(
  IN    SMBIOS_TYPE_9 *SlotsInfo
)
{
  UINT8           iGpioIndex;
  UINT8           GPIOSetting;
  GPIOCONFIG      *GPIOConfig;
  EFI_STATUS      Status;
  
  if (AmdCpmTableProtocolPtr == NULL)
  {
      Status = gBS->LocateProtocol ( &gAmdCpmTableProtocolGuid,
                                      NULL,
                                      (VOID**)&AmdCpmTableProtocolPtr
                                      );
      if (Status != EFI_SUCCESS) {
          DEBUG ((DEBUG_ERROR , "gBS->LocateProtocol(gAmdCpmTableProtocolGuid)... ERROR\n"));
          return Status;
      }
  }

  //Get All Gpio's Setting
  for (iGpioIndex = 0; iGpioIndex < MaxGpioSetting; iGpioIndex++)
  {
      GPIOConfig = &SlotsInfo->GpioMap[iGpioIndex];
      DEBUG ((DEBUG_INFO , "GPIOConfig SocketNumber %d DieNumber %d GpioNumber %d\n", GPIOConfig->SocketNumber, GPIOConfig->DieNumber, GPIOConfig->GpioNumber));
      if(GPIOConfig->GpioNumber != INVALID_SETTING) 
          GPIOSetting = AmdCpmTableProtocolPtr->CommonFunction.GetGpio(AmdCpmTableProtocolPtr, (UINT16)((GPIOConfig->SocketNumber << 14) + (GPIOConfig->DieNumber << 10) + GPIOConfig->GpioNumber));
      else
          GPIOSetting = INVALID_SETTING;
      DEBUG ((DEBUG_INFO , "GPIOSetting %x\n", GPIOSetting));
      if ((iGpioIndex == 0) && (GPIOSetting == INVALID_SETTING)) //If first is invalid setting, it will follow last judge status
          return LastStatus;
      if ((GPIOSetting != INVALID_SETTING) && (GPIOSetting  != GPIOConfig->Validvalue))
      {
          LastStatus = EFI_INVALID_PARAMETER;
          return LastStatus;
      }
      if (GPIOSetting == INVALID_SETTING)
      {
          LastStatus = EFI_SUCCESS;
          return LastStatus;
      }
  }
  
  LastStatus = EFI_SUCCESS;
  return LastStatus;
}



//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
