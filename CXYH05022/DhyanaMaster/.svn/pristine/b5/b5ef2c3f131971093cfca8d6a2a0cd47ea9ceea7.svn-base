//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file UsbSb.c
    USB South Bridge Porting Hooks

**/

#include <AmiUsb.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/AmiUsbSmmGlobalDataValidationLib.h>
#include <Library/DxeSmmUsbSbLib.h>



/**
    This function is for enable/disable Timer SMI.
    @param  EnableSmi        TRUE: Enable timer SMI; otherwise, disable timer SMI.
    @retval EFI_STATUS       Status of the operation
**/

EFI_STATUS
EFIAPI
SettingTimerSMI(
  IN BOOLEAN  EnableSmi
){
   EFI_STATUS Status = EFI_SUCCESS;
   //
   //  You can use gUsbIntTimerHandle to get  PeriodicTime handle.
   //
   if (EnableSmi){
       //
       // Re-enable Periodic Timer SMI 
       //
   } else{        
       //
       // Disable Periodic Timer SMI
       //
   }
   
  return  Status;
}


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
