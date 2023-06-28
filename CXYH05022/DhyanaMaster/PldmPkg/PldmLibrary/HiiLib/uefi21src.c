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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/HiiLib/uefi21src.c 1     5/29/13 11:40a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:40a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/HiiLib/uefi21src.c $
// 
// 1     5/29/13 11:40a Arund
//
// 3     5/28/09 11:10a Presannar
// Bug Fixes Move to next Form
//
// 2     5/15/09 12:40p Presannar
// Added fn GetIFRPackageFromUEFI21HII to retrieve IFR package from HII
// data buffer
//
// 1     5/12/09 10:53a Presannar
// AMISCE Initial source
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
//#include "Tiano.h"
//#include "EfiDriverLib.h"
//#include "EfiHii.h"

#include "Efi.h"
#include "Protocol/HiiDatabase.h"
#include "Library/UefiHiiServicesLib.h"
#include "stdlib.h"
#include "wchar.h"
//----------------------------------------------------------------------------
EFI_HII_DATABASE_PROTOCOL *gHiiDatabaseProtocol = NULL;
//EFI_GUID gEfiHiiDatabaseProtocolGuid = EFI_HII_DATABASE_PROTOCOL_GUID;

extern EFI_BOOT_SERVICES *gBS;

//extern UINT8 *HiiBuffer;
//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitializeUEFI21Protocols
//
// Description: UEFI 2.1 Hii Database Initialization
//
// Paramaters:  VOID
//
// Return:      EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InitializeUEFI21Protocols()
{
  return gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, &gHiiDatabaseProtocol);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RetrieveUEFI21HIIData
//
// Description: Retrive UEFI 2.1 Hii Database
//
// Paramaters:  OUT UINT8 **HIIData - HII DB buffer
//              OUT UINTN *DataSize - HII DB size
//
// Return:      EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RetrieveUEFI21HIIData(UINT8 **HIIData, UINTN *DataSize)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HII_HANDLE Handle = NULL;

  UINT8 *Buffer = NULL;
  UINTN BufferLength = 0;

  Status = gHiiDatabaseProtocol->ExportPackageLists( gHiiDatabaseProtocol, Handle, &BufferLength,
					                                  ( EFI_HII_PACKAGE_LIST_HEADER *)Buffer );
  if( EFI_ERROR ( Status ) )
  {
    if( Status == EFI_BUFFER_TOO_SMALL)
    {
      //Buffer = ( UINT8 * )EfiLibAllocateZeroPool( BufferLength * sizeof( UINT8 ));
      Buffer = ( UINT8 * )calloc( BufferLength, sizeof(UINT8));
      if( Buffer == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        goto DONE;
      }

      Status = gHiiDatabaseProtocol->ExportPackageLists( gHiiDatabaseProtocol, Handle, &BufferLength,
                                                ( EFI_HII_PACKAGE_LIST_HEADER * )Buffer );
    }
    if( EFI_ERROR( Status ) )
    {
      goto DONE;
    }
  }

  *HIIData = Buffer;
//  HiiBuffer = Buffer;
  *DataSize = BufferLength;

DONE:
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   GetIFRPackageFromUEFI21HII
//
// Description: Retrive IFR Packages from HII Database buffer
//
// Paramaters:  IN UINT8 *HIIData - HII DB buffer
//              IN UINTN HIISize - HII DB size
//              OUT UINT8 **IFRData - HII DB buffer
//              OUT UINTN *IFRSize - HII DB size
//
// Return:      EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetIFRPackageFromUEFI21HII(UINT8 *HIIData, UINTN HIISize, UINT8 **IFRData, UINTN *IFRSize)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HII_PACKAGE_LIST_HEADER *PackageListHdr = NULL;
  EFI_HII_PACKAGE_HEADER *PackageHdr = NULL;

  UINT32 Offset = 0;
  UINT32 IFRDataSize = 0;
  // Allocate IFR Buffer
  UINT8 *IFRBuffer = ( UINT8 * )calloc( HIISize, sizeof( UINT8 ));
  UINT8 *BufPtr = IFRBuffer;

  if( IFRBuffer == NULL )
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto DONE;
  }

  do
  {
    UINT32 PkgListOffset = 0;

    PackageListHdr = ( EFI_HII_PACKAGE_LIST_HEADER * )( HIIData + Offset );
    PkgListOffset += sizeof( EFI_HII_PACKAGE_LIST_HEADER );
    do
    {
      PackageHdr = ( EFI_HII_PACKAGE_HEADER * )( HIIData + PkgListOffset );
      if( PackageHdr->Type == EFI_HII_PACKAGE_FORMS )
      {
    	  gBS->CopyMem( BufPtr, PackageHdr, PackageHdr->Length);
        BufPtr += PackageHdr->Length;
        IFRDataSize += PackageHdr->Length;
      }
      PkgListOffset += PackageHdr->Length;
    }while( PkgListOffset < PackageListHdr->PackageLength );
    Offset += PackageListHdr->PackageLength;
  }while( Offset < HIISize );

  *IFRData = IFRBuffer;
  *IFRSize = IFRDataSize;

DONE:
  return Status;
}
