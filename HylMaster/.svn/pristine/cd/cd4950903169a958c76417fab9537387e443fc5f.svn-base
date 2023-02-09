//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file FwConfigTrigFunc.c
    Contains all triggering functions

**/

//---------------------------------------------------------------------------
#include "Efi.h"
#include <Include/AmiDxeLib.h>
#include <Library/BaseLib.h>
#include <Token.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include "AMIVfr.h"
//---------------------------------------------------------------------------

extern EFI_STATUS
EfiLibNamedEventSignal (
  IN EFI_GUID            *Name
  );
extern EFI_GUID gAmiTseSetupEnterGuid;
extern EFI_GUID gFwConfigLaunchGuid;

/**
    This function will install gAmiTseSetupEnterGuid if sdl token Remote_Trigger_TseSetupEntryGuid is enabled
    else it will install gFwConfigLaunchGuid. It will be called after PostManagerHandShakeHookHook function
    before entering into Setup/any other boot option.
    
    @param void

    @retval void
    
**/
void TriggerTSEEnterSetupGuid()
{
    EFI_STATUS  Status = EFI_UNSUPPORTED;
#if Remote_Trigger_TseSetupEntryGuid
    Status = EfiLibNamedEventSignal(&gAmiTseSetupEnterGuid);
#else
    Status = EfiLibNamedEventSignal(&gFwConfigLaunchGuid);   
#endif

    DEBUG((DEBUG_INFO, "\nTriggerTSEEnterSetupGuid: Status of installing TSE Setup Entry Or FwConfig guid - %r",Status));
}

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
