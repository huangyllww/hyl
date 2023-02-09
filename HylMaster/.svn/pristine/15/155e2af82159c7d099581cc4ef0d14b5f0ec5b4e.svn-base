//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************

/** @file NVLockSMIFlashHook.c
    This file contains code that defines SmiFlash Hook Functions

**/

#include "AmiDxeLib.h"

#define BUFFER_SIZE 256
extern EFI_RUNTIME_SERVICES	*pRS;

typedef EFI_STATUS (*TOGGLE_SMI_FLASH_FLAG)(BOOLEAN Start);
typedef struct
{
    TOGGLE_SMI_FLASH_FLAG ToggleSmiFlashUpdateFlag;
} AMI_SETUPNV_CONTROL_PROTOCOL;

AMI_SETUPNV_CONTROL_PROTOCOL *NvramControl;

/**
    This Procedure locates SetupNV Control Protocol.

    @param NONE

    @retval NONE

**/
AMI_SETUPNV_CONTROL_PROTOCOL *LocateSetupNvControlProtocol(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32 Attributes = 0;
    UINTN VarSize = BUFFER_SIZE;
    
    AMI_SETUPNV_CONTROL_PROTOCOL *NvControl = NULL;
    static EFI_GUID gAmiSetupNvControlProtocolGuid = \
    { 0x1bd378d6, 0xd2d6, 0x417f, { 0x87, 0xf6, 0x9e, 0x52, 0xf2, 0x8d, 0xcb, 0x2f } };

    if (NULL == (NvControl = GetSmstConfigurationTable(&gAmiSetupNvControlProtocolGuid)) )
    {
        //If there isn't any SetVariable call from SMM, then the protocol will not be installed.
        //And calling GetSmstConfigurationTable will fail.
        //So Below code is to install the NvControl Protocol if the protocol is not installed before.
        UINT16 Language[BUFFER_SIZE] = {0};
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: GetSmstConfigurationTable NvControl:%x!!", NvControl));
        Status = pRS->GetVariable(L"PlatformLang", &gEfiGlobalVariableGuid, &Attributes, &VarSize, Language);
        DEBUG((DEBUG_ERROR, "\nSetupNVLockNotify: GetVariable Status:%x!!", Status));

        if (Status == EFI_SUCCESS)
        {
            Status = pRS->SetVariable( L"PlatformLang", &gEfiGlobalVariableGuid, Attributes , VarSize, Language);
            DEBUG((DEBUG_ERROR, "\nSetupNVLockNotify: SetVariable Status:%x!!", Status));
            NvControl = GetSmstConfigurationTable(&gAmiSetupNvControlProtocolGuid);
        }
    }

    return NvControl;
}

/**
    Hook function gets control when update through SmiFlash is started.

    @param NONE

    @retval NONE

**/
VOID SmiUpdateStart(VOID)
{
    // Set SmiFlashUpdate flag TRUE in SetupNVLock.
    NvramControl = LocateSetupNvControlProtocol();

    if (NvramControl)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ToggleSmiFlashUpdateFlag!!"));
        NvramControl->ToggleSmiFlashUpdateFlag(FALSE);
    }

    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: SmiUpdate Start !!"));
    return;
}

/**
    Hook function gets control when update through SmiFlash is completed.

    @param NONE

    @retval NONE

**/
VOID SmiUpdateEnd(VOID)
{
    // Set SmiFlashUpdate flag FALSE in SetupNVLock.
    NvramControl = LocateSetupNvControlProtocol();

    if (NvramControl)
    {
        DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: ToggleSmiFlashUpdateFlag!!"));
        NvramControl->ToggleSmiFlashUpdateFlag(TRUE);
    }

    DEBUG((DEBUG_INFO, "\nSetupNVLockNotify: SmiUpdate End !!"));
    return;
}


//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2018, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
