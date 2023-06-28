//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file

  This file contains SMM related Whea code

*/

#include "SmmWheaInit.h"
#include "WheaErst.h"

WHEA_MODULE_PRIVATE_DATA   *mWheaData;
WHEA_INTERFACE             *mWheaInterface,*mNonSmmWheaInterface;
EFI_EVENT                  gReadyToBootEvent;

/**

  Clears the buffer

  @param  Buffer - pointer to memory buffer
  @param  Size - memory buffer size in bytes

*/

VOID
WheaZeroMemory (
  IN  VOID  *Buffer,
  IN  UINTN Size
 )
{
    UINT8   *Ptr;

    Ptr = Buffer;
    while (Size--) {
        *(Ptr++) = 0;
    }
}

/**
  Entry point for Whea driver
  
  @param ImageHandle - The firmware allocated handle for the EFI image.
  @param  SystemTable - A pointer to the EFI System Table.

  @retval always return EFI_SUCCESS 
*/

EFI_STATUS
SmmInitializeWhea (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
    EFI_STATUS                    Status;
    EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch = NULL;
    EFI_SMM_SW_REGISTER_CONTEXT   SwContext;
    EFI_HANDLE                    SwHandle;
    EFI_HANDLE                    NewHandle;
    EFI_WHEA_PROTOCOL             *WheaProtocol;

    Status = gSmst->SmmAllocatePool(EfiRuntimeServicesData,sizeof(WHEA_INTERFACE),&mWheaInterface);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = gBS->LocateProtocol(&gEfiWheaProtocolGuid, NULL, &WheaProtocol);

    if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Couldn't find gEfiWheaProtocolGuid protocol: %r\n", Status));
        return Status;
    }
    mNonSmmWheaInterface = WHEA_INTERFACE_INSTANCE_FROM_THIS(WheaProtocol);
    mWheaInterface->WheaData= mNonSmmWheaInterface->WheaData;
    mWheaData = mWheaInterface->WheaData;

    // Install SW SMI handlers, Handler functions are available in respective 
    // source files.
    Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    if (EFI_ERROR (Status)) {
        //DEBUG ((EFI_D_ERROR, "Couldn't find SmmSwDispatch protocol: %r\n", Status));
        return Status;
    }  

    SwContext.SwSmiInputValue = ERST_EXECUTE_OPERATION_CMD;
    Status = SwDispatch->Register (SwDispatch, ErstSWSMIHandler, &SwContext, &SwHandle);
    ASSERT_EFI_ERROR(Status);
   
    WheaErstInit();
    
    mWheaInterface->Signature = EFI_WHEA_INTERFACE_SIGNATURE;

    NewHandle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                   &NewHandle,
                   &gEfiSmmWheaProtocolGuid ,
                   EFI_NATIVE_INTERFACE,
                   &(mWheaInterface->EfiWheaProtocol)
                   );

    return EFI_SUCCESS;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
