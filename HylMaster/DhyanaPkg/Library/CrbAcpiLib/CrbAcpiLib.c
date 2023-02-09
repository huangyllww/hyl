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

/** @file CrbAcpiLib.c
    This file contains 2 eLinks for CRB ACPI Enabled/Disabled 
    event(s).

**/


//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <PiDxe.h>
#include <PiSmm.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Library/DebugLib.h>
#include <Token.h>
#include <AmiCspLib.h>


//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)

// Function Definition(s)

//----------------------------------------------------------------------------

/**
    This routine will be called when ACPI enabled.

    @param DispatchHandle Handle to the Dispatcher
    @param DispatchContext SW SMM dispatcher context

    @retval VOID

    @note  Porting if needed.
**/

VOID CRBAcpiEnabled (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext )
{
	DEBUG ((-1,"<< CRB CrbAcpiEnabled >>\n"));
}

/**
    This routine will be called when ACPI disabled.

    @param DispatchHandle Handle to the Dispatcher
    @param DispatchContext SW SMM dispatcher context

    @retval VOID

    @note  Porting if needed.
**/

VOID CRBAcpiDisabled (
    IN EFI_HANDLE                   DispatchHandle,
    IN EFI_SMM_SW_REGISTER_CONTEXT  *DispatchContext )
{
	DEBUG ((-1,"<< CRB CrbAcpiDisabled >>\n"));
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
