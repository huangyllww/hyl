//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2008, American Megatrends, Inc.            **
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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/Efi2/vars.c 1     5/29/13 11:39a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:39a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/Efi2/vars.c $
// 
// 1     5/29/13 11:39a Arund
//
// 1     9/28/10 11:52a Timothym
//
// 1     9/04/10 9:02p Timothym
//
// 3     5/21/09 1:03p Presannar
// Addressed Code Revie comments
//
// 2     5/18/09 6:34p Presannar
// Added GetGuidAsUnicodeString and GetNextVariable
// modified GetGuidAsString to not return wide character
//
// 1     5/12/09 10:53a Presannar
// AMISCE Initial Source
//
//*************************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:
// <A description of the file>
//
//<AMI_FHDR_END>
//*************************************************************************

//----------------------------------------------------------------------------
//extern "C" {
//#include <Efi.h>
//#include <EfiLib.h>
//}
//#include <Stdio.h>
//#include <Stdlib.h>
//#include <String.h>
#include <wchar.h>
//#include <CType.h>
#include <AmiSceCommon.h>

#include "vars.h"
//----------------------------------------------------------------------------
#define STRING_GUID_LEN 36
#define WC_MAX_LENGTH 1024
#define MAX_LENGTH 1024

extern EFI_RUNTIME_SERVICES    *gRT;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SetNvramVariable
//
// Description: Set Variable to NVRAM
//
// Paramaters:  IN CHAR16 *VariableName - Variable Name
//              IN EFI_GUID *VariableGuid - Variable GUID
//              IN UINT32 Attributes - Variable Attributes
//              IN UINTN Size - Variable Data Size
//              IN UINT8 *Buffer - Variable Data
//
// Return:      EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SetNvramVariable( CHAR16 *VariableName, EFI_GUID *VariableGuid,
                          UINT32 Attributes, UINTN Size, UINT8 *Buffer )
{
  return gRT->SetVariable ( VariableName, VariableGuid, Attributes, Size,
                          ( VOID* )Buffer );
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetNvramVariable
//
// Description: Get Variable from NVRAM
//
// Paramaters:  IN CHAR16 *Name - Variable Name
//              IN EFI_GUID *VendorGuid - Variable GUID
//              OUT UINT32 *Attributes - Variable Attribute
//              OUT UINTN *VarSize - Variable Size
//
// Return:      VOID* - Variable Data
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
#if 1
EFI_STATUS GetNvramVariable( CHAR16 *Name, EFI_GUID *VendorGuid, UINT32 *Attributes,
                       UINTN *VarSize, UINT8 *Buffer )
{
    EFI_STATUS              Status;

    //
    // Call the real function
    //
    Status = gRT->GetVariable (
        Name,
        VendorGuid,
        Attributes,
        VarSize,
        Buffer
        );

    return Status;
}
#else
VOID *GetNvramVariable( CHAR16 *Name, EFI_GUID *VendorGuid, UINT32 *Attributes,
                       UINTN *VarSize )
{
    EFI_STATUS              Status;
    VOID                    *Buffer = NULL;
    UINTN                   BufferSize = 100;

    //
    // Call the real function
    //

    while (GrowBuffer (&Status, &Buffer, BufferSize))
    {
      Status = gRT->GetVariable (
                  Name,
                  VendorGuid,
                  Attributes,
                  &BufferSize,
                  Buffer
                  );
    }

    *VarSize = Buffer? BufferSize : 0;

    return Buffer;
}
#endif

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetNextNvramVariable
//
// Description: Get the Next Variable Info from NVRAM
//
// Paramaters:  OUT UINTN *VariableNameSize - Variable Size
//              OUT CHAR16 *VariableName - Variable Name
//              IN EFI_GUID *VariableGuid - Variable GUID
//
// Return:      VOID* - Variable Data
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetNextNvramVariable( UINTN *VariableNameSize, CHAR16 *VariableName, EFI_GUID *VariableGuid )
{
  return gRT->GetNextVariableName(VariableNameSize,
                                         VariableName,
                                         VariableGuid
                                         );
}
