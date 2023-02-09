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
/** @file SbGetCapsuleFlagDxeLib.c
    This file contains code for SbGetCapsuleFlagDxeLib.c
    in the DXE stage.

**/
//*************************************************************************

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

// UDK Header
#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
//###<AAV> TBD ### #include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/CapsuleVendor.h>

#include <Library/DebugLib.h>

#include <token.h>
// AMI Compatibility Header.
#include <AmiDxeLib.h> // Optional. should use Mde Library instead.

// Produced Protocols

// Consumed Protocols

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//---------------------------------------------------------------------------

/**
    return Capsule Flag status.

    @param VOID

              
    @retval TRUE TRUE means capsule mode.
    @retval FALSE TRUE means not capsule mode.

    @note  .
**/
BOOLEAN
SbGetCapsuleFlag (
  )
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT32                  Attributes;
    UINTN                   Size;
    BOOLEAN					CapsuleFlag = FALSE;

    Size = 0;
    // We use pRS instead gRT the reason is AmiCompatibilityPkg provide the pRS access in SMM.
    Status = pRS->GetVariable(EFI_CAPSULE_VARIABLE_NAME, &gEfiCapsuleVendorGuid, &Attributes, &Size, NULL);
	// if EFI_NOT_FOUND that means the EFI_CAPSULE_VARIABLE_NAME was cleared in the SecFlashUpdDxe_Init.
	// if EFI_BUFFER_TOO_SMALL that means we have capsule.
	if (Status == EFI_BUFFER_TOO_SMALL) {
		CapsuleFlag = TRUE;
	} else if (Status == EFI_NOT_FOUND) {
		CapsuleFlag = FALSE;
	}

    return CapsuleFlag;
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
