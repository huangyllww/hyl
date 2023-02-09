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
/** @file SbResetServiceElinkLib.c
    This file contains implementation of Runtime Services in SMM

**/
//**********************************************************************

//======================================================================
// Module specific Includes
#include <Uefi.h>
#include <Library/ElinkLib.h>
#include <Sb.h>
#include "token.h"
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

//======================================================================
// Produced Protocols

//===========================================================================
// Time Variable GUID Definition

//===========================================================================
//			Variable Declaration

// GUID Definitions

//===========================================================================
// Function Declarations

//===========================================================================
// Function Definitions

EFI_STATUS
EFIAPI
HygonWarmReset (
  IN EFI_RESET_TYPE  ResetType
  );

static BOOLEAN 		Runtime = FALSE;
static EFI_EVENT	ResetEvent = NULL;
// <AAV> use PCD instead.
static EFI_GUID   EfiSbResetGuid = AMI_CSP_RESET_GUID;
VOID DummyFunction(IN EFI_EVENT Event, IN VOID *Context);
VOID SbRunExitBs(IN EFI_EVENT Event, IN VOID *Context) {Runtime = TRUE;}

/**
    This is for backward compatible the earlier reset mechanism
    since SB800 created.

        
    @param ResetType Reset Type

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
EFIAPI
OldHygonAptio4SbRunReset (
  IN EFI_RESET_TYPE  ResetType
  )
{
	EFI_STATUS	Status = EFI_SUCCESS;
    SB_RESET_TYPE SbResetType = SoftReset;
    UINTN  VariableSize = sizeof(UINT32);

    if (!Runtime)
    {
        //
        // ATTENTION!!!
        // Do not use this event!!!
        // Please use the generic ELINK "SbRuntimeResetElinkList" instead. (EIP77221)
        // The event of EfiSbResetGuid for some old user.
        //
        pBS->SignalEvent(ResetEvent);
        pBS->CloseEvent(ResetEvent);
    }

    // <AAV> use PCD instead.
    Status = pRS->GetVariable(
                 L"CspResetType",
                 &EfiSbResetGuid,
                 NULL,
                 &VariableSize,
                 &SbResetType);
    if (EFI_ERROR(Status))
    {
        switch (ResetType)
        {
        case EfiResetCold:
            SbResetType = HardReset;
            break;

        case EfiResetWarm:
            SbResetType = SoftReset;
            break;

        case EfiResetShutdown:
            SbResetType = ShutDown;
            break;

        default:
            SbResetType = SoftReset;
            break;
        }
    }

    // do a cold reset of the system
    if (ResetType == EfiResetShutdown) SBLib_Shutdown();

	return Status;
}


/**

        
    @param ImageHandle Image handle
    @param SystemTable pointer to the UEFI System Table

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
EFIAPI
Aptio4CspResetTypePatch (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
	EFI_STATUS		Status = EFI_SUCCESS;
	EFI_EVENT 		Event;

	InitAmiLib(ImageHandle, SystemTable);

	//
	// ATTENTION!!!
	// Do not use this event!!!
	// Please use the generic ELINK "SbRuntimeResetElinkList" instead. (EIP77221)
	// The event of EfiSbResetGuid for some old user.
	//
	Status = pBS->CreateEventEx(
				 EVT_NOTIFY_SIGNAL,
				 TPL_CALLBACK,
				 DummyFunction,
				 NULL,
				 &EfiSbResetGuid,
				 &ResetEvent);
	ASSERT_EFI_ERROR(Status);

	Event = NULL;
	Status = CreateReadyToBootEvent(
				 TPL_CALLBACK,
				 SbRunExitBs,
				 NULL,
				 &Event);
	ASSERT_EFI_ERROR(Status);

    return Status;
}

/**

        
    @param ImageHandle Image handle
    @param SystemTable pointer to the UEFI System Table

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
EFIAPI
SbResetServiceElinkLibConstructor (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
	EFI_STATUS 		Status;

	//#### TBD #### Status = ElinkRegister(PcdSbResetElink, OldHygonAptio4SbRunReset);
	Status = ElinkRegister(PcdSbResetElink, HygonWarmReset);
	ASSERT_EFI_ERROR(Status);
	if (EFI_ERROR(Status)) return Status;

	Status = Aptio4CspResetTypePatch(ImageHandle, SystemTable);

    return Status;
}

/**

        
    @param ImageHandle Image handle
    @param SystemTable pointer to the UEFI System Table

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/

EFI_STATUS
EFIAPI
SbSmmResetServiceElinkLibConstructor (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
	EFI_STATUS 		Status = EFI_SUCCESS;

	//#### TBD #### Status = ElinkRegister(PcdSbResetElink, OldHygonAptio4SbRunReset);
	Status = ElinkRegister(PcdSbResetElink, HygonWarmReset);

    return Status;
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
