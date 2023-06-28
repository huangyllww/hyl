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

/** @file SbSmmCorePlatformHookLib.c
    This file contains code for South Bridge SMM Core Platform Hook

*/

#include <Library/SmmCorePlatformHookLib.h>
#include <token.h>
#include <Library/SbCspLib.h>

  
/**
    Performs South Bridge specific tasks before invoking registered SMI 
    handlers.

    @param VOID

    @retval EFI_SUCCESS - The South Bridge platform hook completes 
                          successfully.

**/
EFI_STATUS
EFIAPI
SbPlatformHookBeforeSmmDispatch (
    VOID
  )
{
    //
    // Important!!!
    // If you change the Index data of IO port or MMIO in the ELINK function of
    // PLATFORMHOOK_BEFORE_SMMDISPATCH, please remember to restore it before
    // exit the ELINK function.
    //
    return SbSmmSaveRestoreStates(TRUE);
}

/**
    Performs South Bridge specific tasks after invoking registered SMI 
    handlers.

    @param VOID

    @retval EFI_SUCCESS - The South Bridge platform hook completes 
                          successfully.

**/
EFI_STATUS
EFIAPI
SbPlatformHookAfterSmmDispatch (
    VOID
  )
{
    //
    // Important!!!
    // If you change the Index data of IO port or MMIO in the ELINK function of
    // PLATFORMHOOK_AFTER_SMMDISPATCH, please remember to restore it before
    // exit the ELINK function.
    //
    return SbSmmSaveRestoreStates(FALSE);
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
