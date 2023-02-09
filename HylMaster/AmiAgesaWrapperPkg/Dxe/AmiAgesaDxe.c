//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <Uefi.h>
#include <AGESA.h>
#include "Library/IoLib.h"
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "token.h"
#include <Library/DebugLib.h>
#include "AmiAgesaDxe.h"
#include <Protocol/AmdCpmTableProtocol/AmdCpmTableProtocol.h>
#include <TSFch.h>


#if BIOS_POST_COMPL_ENABLE
VOID BiosCompleteEventCallback (
  IN      EFI_EVENT               Event,
  IN      VOID                    *Context
  )
{
  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  EFI_STATUS                      Status;
  UINT8                           BiosPostCmpPin = 0; 

  Status = gBS->LocateProtocol (  &gAmdCpmTableProtocolGuid,
                                  NULL,
                                  (VOID**)&CpmTableProtocolPtr
                                  );
  if (EFI_ERROR (Status)) {
      return;
  }

  BiosPostCmpPin = CpmTableProtocolPtr->CommonFunction.SmnRead8(CpmTableProtocolPtr, 
          BIOS_POST_CMPL_SOCKET, 
          BIOS_POST_CMPL_DIE, 
          (UINT32) FCH_SMN_GPIO_BASE, 
          (UINT32) ((BIOS_POST_CMPL_PIN << 2) + 2));
  
  DEBUG(( DEBUG_INFO, "BiosPostCmpPin = 0x%x\n", BiosPostCmpPin));
  
  //the highest two bits are reserved
  BiosPostCmpPin = BiosPostCmpPin & 0xC7; 
     
  //pull up
  BiosPostCmpPin |= 0x10; 
     
  //output enable
  BiosPostCmpPin |= 0x80;
     
  //output state
  if (BIOS_POST_CMPL_VALUE)
      BiosPostCmpPin |= 0x40;
  else
      BiosPostCmpPin &= 0xBF;
  
  CpmTableProtocolPtr->CommonFunction.SmnWrite8 (CpmTableProtocolPtr, 
          BIOS_POST_CMPL_SOCKET, 
          BIOS_POST_CMPL_DIE, 
          (UINT32) FCH_SMN_IOMUX_BASE, 
          (UINT32) BIOS_POST_CMPL_PIN, 
          BIOS_POST_CMPL_PIN_FUNCTION_NUMBER);
  
  DEBUG(( DEBUG_INFO, "Write BiosPostCmpPin = 0x%x\n", BiosPostCmpPin));
  CpmTableProtocolPtr->CommonFunction.SmnWrite8 (CpmTableProtocolPtr, 
          BIOS_POST_CMPL_SOCKET, 
          BIOS_POST_CMPL_DIE, 
          (UINT32) FCH_SMN_GPIO_BASE, 
          (UINT32) ((BIOS_POST_CMPL_PIN << 2) + 2), 
          BiosPostCmpPin);

  BiosPostCmpPin = CpmTableProtocolPtr->CommonFunction.SmnRead8(CpmTableProtocolPtr, 
          BIOS_POST_CMPL_SOCKET, 
          BIOS_POST_CMPL_DIE, 
          (UINT32) FCH_SMN_GPIO_BASE, 
          (UINT32) ((BIOS_POST_CMPL_PIN << 2) + 2));
  
  DEBUG(( DEBUG_INFO, "BiosPostCmpPin(%d) = 0x%x\n", BIOS_POST_CMPL_PIN, BiosPostCmpPin));
     
  gBS->CloseEvent(Event);

  return;

}

EFI_STATUS CreateBiosCompleteReadyToBootEvent(VOID)
{
  EFI_EVENT                               Event;
  EFI_STATUS                              Status;
  
  Status = gBS->CreateEventEx (
          EVT_NOTIFY_SIGNAL,
          TPL_NOTIFY,
          BiosCompleteEventCallback,
          NULL,
          &gEfiEventReadyToBootGuid,
          &Event
  );
  DEBUG((DEBUG_INFO, "CreateBiosCompleteReadyToBootEvent.Status=%r\n", Status));
  
  return Status;
}
#endif

/**
 *  AmiAgesaPeiEntry
 *
 *  Description:
 *    This function will prepare the platform configuarion for AGESA use.
 *
 *  Parameters:
 *    @param[in]        **PeiServices
 *                      FileHandle
 *
 *
 *    @retval         VOID
 *
 **/
EFI_STATUS
EFIAPI
AmiAgesaDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    EFI_STATUS                                  Status = EFI_SUCCESS;

    DEBUG ((EFI_D_ERROR, "AmiAgesaDxeEntry Entry\n"));
#if BIOS_POST_COMPL_ENABLE
    Status = CreateBiosCompleteReadyToBootEvent();
#endif
    DEBUG ((EFI_D_ERROR, "AmiAgesaDxeEntry Exit\n"));

    return  Status;
}






//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
