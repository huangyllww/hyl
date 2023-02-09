//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  Function to disable persistent store before flash update

*/

#include <Whea\SmmWhea\SmmWheaInit.h>

/**
  This function disables persistent store before flash update occurs.
  OS cannot read/write any error after the flash update until reboot.
  
*/

VOID 
DisableSmmWheaErrorLog (VOID)
{
    EFI_STATUS                      Status;
    EFI_WHEA_PROTOCOL               *SmmWheaProtocol = NULL;
    EFI_HANDLE                      *HandleBuffer = NULL;
    UINTN                           HandleBufferSize = 0;
    UINTN                           HandleCount = 0;
    EFI_SMM_SW_DISPATCH2_PROTOCOL   *SwDispatch = NULL;
    WHEA_INTERFACE                  *mWheaInterface;

    // Locate the SmmWheaProtocol.
    Status = gSmst->SmmLocateProtocol (
                &gEfiSmmWheaProtocolGuid,
                NULL,
                &SmmWheaProtocol );
    
    if (!EFI_ERROR(Status)) {
        // Collect all Whea Interface handles.
        Status = gSmst->SmmLocateHandle(
                ByProtocol, 
                &gEfiSmmWheaProtocolGuid,
                NULL,
                &HandleBufferSize, 
                HandleBuffer );

        if(EFI_ERROR(Status)) {
            if(Status == EFI_BUFFER_TOO_SMALL) {
                // Allocate memory for Handle Buffer.
                Status = gSmst->SmmAllocatePool(
                        EfiRuntimeServicesCode,
                        HandleBufferSize,
                        (VOID**)&HandleBuffer );
                if( EFI_ERROR(Status) ) {
                    ASSERT(FALSE);
                    return ;
                }

                Status = gSmst->SmmLocateHandle( 
                        ByProtocol, 
                        &gEfiSmmWheaProtocolGuid,
                        NULL,
                        &HandleBufferSize, 
                        HandleBuffer );
            
                if (EFI_ERROR(Status)) {
                    if (HandleBuffer != NULL) {
                        gSmst->SmmFreePool(HandleBuffer);
                    }
                    return;
                }
            } else {
                return;
            }
        }
        HandleCount = HandleBufferSize/sizeof(EFI_HANDLE);
    
        while(HandleCount > 0){
            Status = gSmst->SmmHandleProtocol(
                    (WHEA_INTERFACE*)(HandleBuffer[HandleCount-1]),
                    &gEfiSmmWheaProtocolGuid,
                    (VOID*)&SmmWheaProtocol );

            if( !EFI_ERROR(Status) ) {
                mWheaInterface = WHEA_INTERFACE_INSTANCE_FROM_THIS(SmmWheaProtocol);
                mWheaInterface->WheaData->ErstEnabled = FALSE;
                //DEBUG((EFI_D_ERROR, "ERST storage disabled\n"));
            }
            HandleCount--;
        }
        // Free HandleBuffer memory
        if( HandleBuffer ) {
            gSmst->SmmFreePool(HandleBuffer);
        }
    }
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
