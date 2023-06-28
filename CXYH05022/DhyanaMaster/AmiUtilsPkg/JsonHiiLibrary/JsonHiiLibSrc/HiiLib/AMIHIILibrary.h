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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/HiiLib/AMIHIILibrary.h 1     5/29/13 11:40a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:40a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/HiiLib/AMIHIILibrary.h $
// 
// 1     5/29/13 11:40a Arund
//
// 1     9/28/10 11:55a Timothym
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
#include "Efi.h"
//----------------------------------------------------------------------------
//EFI_STATUS InitializeDriverLIB( EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable);
EFI_STATUS HiiLib_RetrieveHIIData( UINT8 **HiiData, UINTN *DataSize, UINT32 *CRC32 );
//----------------------------------------------------------------------------
