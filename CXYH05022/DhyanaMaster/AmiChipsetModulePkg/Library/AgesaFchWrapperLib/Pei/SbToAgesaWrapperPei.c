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

//**********************************************************************
/** @file SbToAgesaWrapperPei.c
    This wrapper OBJ will be link into Agesa component.

**/
//**********************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <PiPei.h>
#include <Library/DebugLib.h>

#include <Ppi/HygonSbWrapper.h>

//---------------------------------------------------------------------------
//AMI Generic Porting starts
//---------------------------------------------------------------------------

#include "token.h"

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

VOID
CallHooks (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  AMD_CONFIG_PARAMS *StdHeader,
  IN  BOOLEAN           BeforeInit
  );

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------


/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param Pointer to PeiServices
    @param InitReset 

    @retval VOID

**/

VOID
SbAgesaCustomizeInitReset (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  AMD_RESET_PARAMS    *InitReset
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)InitReset, TRUE);
    }
}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param Pointer to PeiServices
    @param InitEarly 

    @retval VOID

**/
VOID
SbAgesaCustomizeInitEarly (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  AMD_EARLY_PARAMS    *InitEarly
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)InitEarly, TRUE);
    }
}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param Pointer to PeiServices
    @param InitPost 

    @retval VOID

**/
VOID
SbAgesaCustomizeInitPost (
  IN  EFI_PEI_SERVICES   **PeiServices,
  IN  AMD_POST_PARAMS    *InitPost
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)InitPost, TRUE);
    }
}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param Pointer to PeiServices
    @param S3LateParams 

    @retval VOID

**/
VOID
SbAgesaCustomizeS3LateRestore (
  IN  EFI_PEI_SERVICES   **PeiServices,
  IN  AMD_S3LATE_PARAMS  *S3LateParams
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)S3LateParams, TRUE);
    }
}

/**
    This is the stub function which will be called after Agesa AmdInitReset
    enrty point


    @param Pointer to PeiServices
    @param InitReset 

    @retval VOID

**/
VOID
SbAgesaHookAfterInitReset (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  AMD_RESET_PARAMS    *InitReset
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)InitReset, FALSE);
    }
}

/**
    This is the stub function which will be called after Agesa AmdInitEarly
    enrty point


    @param Pointer to PeiServices
    @param InitEarly 

    @retval VOID

**/
VOID
SbAgesaHookAfterInitEarly (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  AMD_EARLY_PARAMS    *InitEarly
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)InitEarly, FALSE);
    }
}

/**
    This is the stub function which will be called after Agesa AmdInitPost
    enrty point


    @param Pointer to PeiServices
    @param InitPost 

    @retval VOID

**/

VOID
SbAgesaHookAfterInitPost (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  AMD_POST_PARAMS     *InitPost
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)InitPost, FALSE);
    }
}

/**
    This is the stub function which will be called after Agesa S3LateRestore
    enrty point


    @param Pointer to PeiServices
    @param S3LateParams 

    @retval VOID

**/

VOID
SbAgesaHookAfterS3LateRestore (
  IN  EFI_PEI_SERVICES   **PeiServices,
  IN  AMD_S3LATE_PARAMS  *S3LateParams
  )
{
    if(PeiServices) {
        CallHooks(PeiServices, (AMD_CONFIG_PARAMS*)S3LateParams, FALSE);
    }
}

/**
    Before AGESA FchInitResetTaskTable.
    This is CSP ELINK after AgesaFchOemCallout.

    @param FchData 

               
    @retval AGESA_SUCCESS The service completed normally. Info may be logged.
    @retval AGESA_UNSUPPORTED The dispatcher or create struct had an unimplemented function requested.
        Not logged.
    @retval AGESA_BOUNDS_CHK A dynamic parameter was out of range and the service was not provided.
        Example, memory address not installed, heap buffer handle not found.
        Not Logged.
    @retval AGESA_ALERT An observed condition, but no loss of function.  See Log.
    @retval AGESA_WARNING Possible or minor loss of function.  See Log.
    @retval AGESA_ERROR Significant loss of function, boot may be possible.  See Log.
    @retval AGESA_CRITICAL Continue boot only to notify user.  See Log.
    @retval AGESA_FATAL Halt booting.  See Log, however Fatal errors pertaining to heap problems
        may not be able to reliably produce log events.

**/

AGESA_STATUS
SbAgesaFchOemCalloutPei (
  IN      VOID              *FchData
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    FCH_RESET_DATA_BLOCK        *myFchInterface;
    AMD_CONFIG_PARAMS           *StdHeader;
    EFI_PEI_SERVICES            **PeiServices;
    HYGON_FCH_AMI_POLICY_PPI      *FchPolicy;

    myFchInterface = FchData;
    StdHeader = myFchInterface->StdHeader;
    PeiServices = (EFI_PEI_SERVICES **)StdHeader->ImageBasePtr;

    Status = (*PeiServices)->LocatePpi ( \
                        PeiServices,
                        &gHygonFchAmiPolicyPpiGuid,
                        0,
                        NULL,
                        &FchPolicy);
    ASSERT_EFI_ERROR (Status);

    FchPolicy->HookAgesaFchOemCalloutPei(PeiServices, FchPolicy, myFchInterface);

    return Status;
}

/**

        
    @param PeiServices 
    @param StdHeader 
    @param BeforeInit 

    @retval VOID

**/
VOID
CallHooks (
  IN  EFI_PEI_SERVICES  **PeiServices,
  IN  AMD_CONFIG_PARAMS *StdHeader,
  IN  BOOLEAN           BeforeInit
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    HYGON_FCH_AMI_POLICY_PPI      *FchPolicy;

    Status = (*PeiServices)->LocatePpi ( \
                        PeiServices,
                        &gHygonFchAmiPolicyPpiGuid,
                        0,
                        NULL,
                        &FchPolicy);
    if(!EFI_ERROR(Status)) {
        switch (StdHeader->Func) {
        case AMD_INIT_RESET:
            if (BeforeInit) FchPolicy->HookInitReset(PeiServices, FchPolicy, (AMD_RESET_PARAMS*)StdHeader);
            else FchPolicy->HookAfterInitReset(PeiServices, FchPolicy, (AMD_RESET_PARAMS*)StdHeader);
            break;
        case AMD_INIT_EARLY:
            if (BeforeInit) FchPolicy->HookInitEarly(PeiServices, FchPolicy, (AMD_EARLY_PARAMS*)StdHeader);
            else FchPolicy->HookAfterInitEarly(PeiServices, FchPolicy, (AMD_EARLY_PARAMS*)StdHeader);
            break;
        case AMD_INIT_POST:
            if (BeforeInit) FchPolicy->HookInitPost(PeiServices, FchPolicy, (AMD_POST_PARAMS*)StdHeader);
            else FchPolicy->HookAfterInitPost(PeiServices, FchPolicy, (AMD_POST_PARAMS*)StdHeader);
            break;
          // Fall through to default case
        default:
            break;
        }
    }
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
