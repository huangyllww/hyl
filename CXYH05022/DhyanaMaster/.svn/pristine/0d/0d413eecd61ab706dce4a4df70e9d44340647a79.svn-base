//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//*************************************************************************
/** @file SbRunSmm.c
    This file contains code for SouthBridge runtime SMM
    protocol

**/
//*************************************************************************

// Module specific Includes

#include "Efi.h"
#include "token.h"
#include <AmiDxeLib.h>
#include <Library/AmiChipsetRuntimeServiceLib.h>

/**
    Submit runtime services both SMM and runtime.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
SbRuntimeServiceInit (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    pRS->ResetSystem    = AmiChipsetResetSystem;
    pRS->GetTime        = AmiChipsetGetTime;
    pRS->SetTime        = AmiChipsetSetTime;
    pRS->GetWakeupTime  = AmiChipsetGetWakeupTime;
    pRS->SetWakeupTime  = AmiChipsetSetWakeupTime;

    return EFI_SUCCESS;
}

/**
    This function is the entry point for this SMM. This function
    installs the runtime services related to SB in SMM.

    @param ImageHandle Image handle
    @param SystemTable Pointer to the system table

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
SbRuntimeSmmInitEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    return InitSmmHandler(
            ImageHandle, SystemTable, SbRuntimeServiceInit, NULL
            );
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
