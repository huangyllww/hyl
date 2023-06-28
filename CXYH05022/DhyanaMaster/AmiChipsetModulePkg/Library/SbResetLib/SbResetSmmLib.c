//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//*************************************************************************
/** @file SbResetSmmLib.c
    This file contains code for SbResetSmmLib.c
    in the DXE stage.

**/
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

// UDK Header
#include <PiSmm.h>
#include <Core/PiSmmCore/PiSmmCore.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/AmiUsbController.h>

#include <token.h>

// Produced Protocols

// Consumed Protocols

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
VOID
GetUsbProtocol (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  );

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

VOID	*gSbResetReg = NULL;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------


/**

        
    @param Protocol 
    @param Interface 
    @param Handle 

         
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
GetUsbProtocolInSmm (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
	GetUsbProtocol(NULL, NULL);
	return EFI_SUCCESS;
}

/**
    This function to construct the smm and runtime code for reset
    service.

               
    @param ImageHandle 
    @param SystemTable 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
EFIAPI
SbResetSmmLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	EFI_STATUS	Status = EFI_SUCCESS;

	if (gSmst != NULL) {
#if AMIUSB_SUPPORT
		Status = gSmst->SmmRegisterProtocolNotify(&gAmiUsbSmmProtocolGuid, GetUsbProtocolInSmm, &gSbResetReg);
#endif
	}

	return Status;
}

/**
    This function to deconstruct the smm and runtime code for reset
    service.

               
    @param ImageHandle 
    @param SystemTable 

               
    @retval EFI_SUCCESS Module initialized successfully
    @retval EFI_ERROR Initialization failed (see error for more details)

**/
EFI_STATUS
EFIAPI
SbResetSmmLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
	EFI_STATUS		Status = EFI_SUCCESS;
	PROTOCOL_NOTIFY	*ProtNotify = NULL;

	//
	// There is no close event service in SMST.
	// This is a workaround to unregister the notification while drivers unload.
	//
	if (gSbResetReg != NULL) {
		ProtNotify = (PROTOCOL_NOTIFY*)gSbResetReg;
		RemoveEntryList (&ProtNotify->Link);
	}

	return Status;
}
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
