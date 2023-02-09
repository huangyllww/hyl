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
/** @file SbToAgesaWrapperDxe.c
    This file contains code for Chipset Reference Board
    Template initialization in the PEI stage

**/
//**********************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <PiDxe.h>

#include <AGESA.h>
#include <OemAgesaCustomizeDxe.h>
#include <FchPlatform.h>
#include <Amdlib.h>

#include <Protocol/FchDxeLpcClkRunInitProtocol.h>

//AMI Generic Porting starts

#include <token.h>
#include <Protocol/HygonSbPolicy.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

VOID
CallHooks (
  IN  EFI_BOOT_SERVICES *BootServices,
  IN  AMD_CONFIG_PARAMS *StdHeader,
  IN  UINT32            Func, // AMD StdHeader.Func invalid. Use Func instead of that. For the moment.
  IN  BOOLEAN           BeforeInit
  );

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

HYGON_FCH_AMI_POLICY_PROTOCOL     static *gSbPolicy = NULL;

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------


/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param BootServices 
    @param InitEnv 

    @retval VOID

**/

VOID
SbAgesaHookInitEnv (
  IN  EFI_BOOT_SERVICES *BootServices,
  IN  AMD_ENV_PARAMS    *InitEnv
  )
{
    CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitEnv, AMD_INIT_ENV, TRUE);
}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param BootServices 
    @param InitMid 

    @retval VOID

**/

VOID
SbAgesaHookInitMid (
  IN  EFI_BOOT_SERVICES *BootServices,
  IN  AMD_MID_PARAMS    *InitMid
  )
{
    CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitMid, AMD_INIT_MID, TRUE);
}
/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param BootServices 
    @param InitLate 

    @retval VOID

**/

VOID
SbAgesaHookInitLate (
  IN  EFI_BOOT_SERVICES *BootServices,
  IN  AMD_LATE_PARAMS    *InitLate
  )
{
    CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitLate, AMD_INIT_LATE, TRUE);
}

/**
    This is the stub function will call the host environment through the binary block
    interface (call-out port) to provide a user hook opportunity


    @param BootServices 
    @param S3Save 

    @retval VOID

**/

VOID
SbAgesaCustomizeS3Save (
  IN  EFI_BOOT_SERVICES   *BootServices,
  IN  AMD_RTB_PARAMS   *InitRtb
  )
{
	CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitRtb, AMD_INIT_RTB, TRUE);
}

/**
    This is the stub function which will be called after Agesa AmdInitEnv
    enrty point


    @param BootServices 
    @param InitEnv 

    @retval VOID

**/

VOID
SbAgesaHookAfterInitEnv (
  IN  EFI_BOOT_SERVICES   *BootServices,
  IN  AMD_ENV_PARAMS      *InitEnv
  )
{
    CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitEnv, AMD_INIT_ENV, FALSE);
}

/**
    This is the stub function which will be called after Agesa AmdInitMid
    enrty point


    @param BootServices 
    @param InitMid 

    @retval VOID

**/

VOID
SbAgesaHookAfterInitMid (
  IN  EFI_BOOT_SERVICES   *BootServices,
  IN  AMD_MID_PARAMS      *InitMid
  )
{
    CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitMid, AMD_INIT_MID, FALSE);
}

/**
    This is the stub function which will be called after Agesa AmdInitLate
    enrty point


    @param BootServices 
    @param InitLate 

    @retval VOID

**/

VOID
SbAgesaHookAfterInitLate (
  IN  EFI_BOOT_SERVICES   *BootServices,
  IN  AMD_LATE_PARAMS      *InitLate
  )
{
#if FCH_LPC_CLK_RUN_SUPPORT
    FCH_DXE_LPCCLK_INIT_PROTOCOL       *FchDxeLpcClkRunInit;
    EFI_STATUS                          Status;
    EFI_BOOT_SERVICES                  *myBS = BootServices;
    CallHooks(BootServices, (AMD_CONFIG_PARAMS*)InitLate, AMD_INIT_LATE, FALSE);

    Status = myBS->LocateProtocol(
                   &gFchDxeLpcClkRunInitProtocolGuid,
                   NULL,
                   &FchDxeLpcClkRunInit
             );
    if (!EFI_ERROR (Status)) {
        FchDxeLpcClkRunInit->FpFchDxeLpcClkRun(FCH_SPI_ROM_MOUNTED);
    }
#endif
}

/**
    Before AGESA FchInitEnvTaskTable.
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
SbAgesaFchOemCalloutDxe (
  IN      VOID              *FchData
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;

    gSbPolicy->HookAgesaFchOemCalloutDxe(gSbPolicy, FchData);

    return  Status;
}

/**

         
    @param BootServices 
    @param StdHeader 
    @param Func AMD StdHeader.Func invalid. Use Func instead of that. For the moment.
    @param BeforeInit 

    @retval VOID

**/
VOID
CallHooks (
  IN  EFI_BOOT_SERVICES *BootServices,
  IN  AMD_CONFIG_PARAMS *StdHeader,
  IN  UINT32            Func, // AMD StdHeader.Func invalid. Use Func instead of that. For the moment.
  IN  BOOLEAN           BeforeInit
  )
{
    EFI_STATUS                  Status = EFI_SUCCESS;
    EFI_BOOT_SERVICES           *myBS = BootServices;
    //####Status = myBS->LocateProtocol(&gAmiCrbAmdTrinityPolicyProtocol,
    //####                                NULL,
    //####                                &TrinityPolicy);
    //####if (EFI_ERROR(Status)) {
    //####    Status = myBS->LocateProtocol(&gAmiCspAmdHudsonPolicyProtocol,
    //####                                    NULL,
    //####                                    &TrinityPolicy);
    //####}
    if (gSbPolicy == NULL) {
        Status = myBS->LocateProtocol(&gHygonFchAmiPolicyProtocolGuid,
                                        NULL,
                                        &gSbPolicy);
        if (EFI_ERROR(Status)) {
            // DXE_SB_ERROR
            WriteIo8(0x80, 0xD2);
            while(1){}
        }
    }

    if(!EFI_ERROR(Status)) {
        // AMD StdHeader.Func invalid. Use Func instead of that.
        switch (Func) {
        case AMD_INIT_ENV:
            if (BeforeInit) gSbPolicy->HookInitEnv(gSbPolicy, (AMD_ENV_PARAMS*)StdHeader);
            else gSbPolicy->HookAfterInitEnv(gSbPolicy, (AMD_ENV_PARAMS*)StdHeader);
            break;
        case AMD_INIT_MID:
            if (BeforeInit) gSbPolicy->HookInitMid(gSbPolicy, (AMD_MID_PARAMS*)StdHeader);
            else gSbPolicy->HookAfterInitMid(gSbPolicy, (AMD_MID_PARAMS*)StdHeader);
            break;
        case AMD_INIT_LATE:
            if (BeforeInit) gSbPolicy->HookInitLate(gSbPolicy, (AMD_LATE_PARAMS*)StdHeader);
            else gSbPolicy->HookAfterInitLate(gSbPolicy, (AMD_LATE_PARAMS*)StdHeader);
            break;
        case AMD_INIT_RTB:
            if (BeforeInit) gSbPolicy->HookCustomizeS3Save(gSbPolicy, (AMD_LATE_PARAMS*)StdHeader);
            else gSbPolicy->HookCustomizeS3Save(gSbPolicy, (AMD_LATE_PARAMS*)StdHeader);
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
