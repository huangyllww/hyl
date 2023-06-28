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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/App/Efi2/inc/vars.h 1     5/29/13 11:39a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:39a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/App/Efi2/inc/vars.h $
// 
// 1     5/29/13 11:39a Arund
//
// 1     9/28/10 5:26p Timothym
//
// 1     9/04/10 9:05p Timothym
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
//#include <stdio.h>
//#include <wchar.h>

#if 1
EFI_STATUS GetNvramVariable( CHAR16 *Name, EFI_GUID *VendorGuid, UINT32 *Attributes,
                       UINTN *VarSize, UINT8 *Buffer );
#else
VOID *GetNvramVariable( CHAR16 *Name, EFI_GUID *VendorGuid, UINT32 *Attributes,
                       UINTN *VarSize );
#endif

EFI_STATUS SetNvramVariable( CHAR16 *VariableName, EFI_GUID *VariableGuid,
                          UINT32 Attributes, UINTN Size, UINT8 *Buffer );
EFI_STATUS GetNextNvramVariable( UINTN *VariableNameSize, CHAR16 *VariableName,
                           EFI_GUID *VariableGuid );

