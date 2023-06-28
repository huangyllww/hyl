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


/** @file DisableGpnvErrorLogLib.c
    Function to Disable the GPNV error logging

**/

//---------------------------------------------------------------------------

#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/GpnvRedirProtocol.h>

/** Section added for disabling the GPNV error logging support.
	This function disable the GPNV error logging feature before the flash.
	Hence the user can't log any error after the flash update.
 **/

VOID DisableGpnvErrorLog (VOID)
{
    EFI_STATUS                  Status;
    EFI_HANDLE                  *HandleBuffer = NULL;
    UINTN                       HandleBufferSize = 0;
    UINTN                       HandleCount = 0;
    EFI_SM_ELOG_REDIR_PROTOCOL  *RedirectProtocol;
    BOOLEAN                     EnableElog = FALSE;
    BOOLEAN                     ElogStatus = TRUE;

    //
    // Get all Redir Elog handles.
    //
    Status = gSmst->SmmLocateHandle (
                ByProtocol,
                &gEfiRedirElogProtocolGuid,
                NULL,
                &HandleBufferSize,
                HandleBuffer );
    if (EFI_ERROR(Status)) {
        if (Status == EFI_BUFFER_TOO_SMALL) {
            //
            // Allocate memory for Handle buffer
            //
            HandleBuffer = AllocatePool (HandleBufferSize);
            if (HandleBuffer == NULL) {
                return;
            }
            Status = gSmst->SmmLocateHandle (
                        ByProtocol,
                        &gEfiRedirElogProtocolGuid,
                        NULL,
                        &HandleBufferSize,
                        HandleBuffer );
            if (EFI_ERROR(Status)) {
                if (HandleBuffer != NULL) {
                    FreePool (HandleBuffer);
                }
                return;
            }
        }
        else 
            return;
    }
    HandleCount = HandleBufferSize/sizeof(EFI_HANDLE);
    while (HandleCount > 0) {
        Status = gSmst->SmmHandleProtocol (
                    (EFI_SM_ELOG_REDIR_PROTOCOL*)(HandleBuffer[HandleCount-1]),
                    &gEfiRedirElogProtocolGuid,
                    (VOID **)&RedirectProtocol );
        //
        // Check for the GPNV interface and disable the GPNV error logging support
        //
        if ( (!EFI_ERROR(Status)) && (RedirectProtocol != NULL) ) {
            Status = RedirectProtocol->ActivateEventLog (RedirectProtocol, EfiElogRedirSmSMBIOS, &EnableElog, &ElogStatus);
            DEBUG((DEBUG_INFO, "\n RedirectProtocol->ActivateEventLog %r %x %x \n", Status, EnableElog, ElogStatus));
        }
        HandleCount--;
    }
    if (HandleBuffer != NULL) {
        FreePool (HandleBuffer);
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
