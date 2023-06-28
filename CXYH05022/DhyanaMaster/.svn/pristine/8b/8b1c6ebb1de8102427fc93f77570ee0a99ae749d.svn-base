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
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/HiiLib/AmiHiiLibrary.c 1     5/29/13 11:40a Arund $
//
// $Revision: 1 $
//
// $Date: 5/29/13 11:40a $
//*************************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/HiiLib/AmiHiiLibrary.c $
// 
// 1     5/29/13 11:40a Arund
//
// 2     5/15/09 12:38p Presannar
// Added code to calculate CRC32 on IFR packs instead of the complete HII
// database
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
#include "AMIHIILibrary.h"
#include "wchar.h"
#include "CRC32.h"

extern UINT64 gHiiDataSize;
//----------------------------------------------------------------------------
static BOOLEAN gUEFI_21_Supported = TRUE;
static BOOLEAN gHIIProtocolsInitialized = FALSE;
//----------------------------------------------------------------------------
//extern EFI_STATUS InitializeUEFI20Protocols();
extern EFI_STATUS InitializeUEFI21Protocols();
//extern EFI_STATUS RetrieveUEFI20HIIData(UINT8 **HIIData, UINTN *DataSize);
extern EFI_STATUS RetrieveUEFI21HIIData(UINT8 **HIIData, UINTN *DataSize);
//extern EFI_STATUS GetIFRPackageFromUEFI20HII(UINT8 *HIIData, UINTN HIISize, UINT8 **IFRData, UINTN *IFRSize);
extern EFI_STATUS GetIFRPackageFromUEFI21HII(UINT8 *HIIData, UINTN HIISize, UINT8 **IFRData, UINTN *IFRSize);
//----------------------------------------------------------------------------

extern EFI_BOOT_SERVICES *gBS;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitializeDriverLIB
//
// Description: Initialize EDK Driver LIB
//
// Paramaters:  IN EFI_HANDLE ImageHandle
//              IN IN EFI_SYSTEM_TABLE *SystemTable
//
// Return:      EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
/*EFI_STATUS InitializeDriverLIB( EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable )
{
  return EfiInitializeDriverLib(ImageHandle, SystemTable);
}*/

//----------------------------------------------------------------------------
//
// Procedure:   _InitializeHIIProtocol
//
// Description: Initialize HII protocol if gHIIProtocolsInitialized is not set.
//              This function initializes either UEFI 2.1or UEFI 2.0 protocol
//              depending on the system and sets gUEFI_21_Supported flag.
//
// Paramaters:  VOID
//
// Return:      EFI_STATUS
//----------------------------------------------------------------------------
EFI_STATUS _InitializeHIIProtocol()
{
  EFI_STATUS Status = EFI_SUCCESS;

  if( gHIIProtocolsInitialized )
  {
    goto DONE;
  }

  Status = InitializeUEFI21Protocols( );
  if( EFI_ERROR( Status ) )
  {
    /*Status = InitializeUEFI20Protocols( );
    if( EFI_ERROR( Status ) )
    {
      // Display Appropriate Error to User
      goto DONE;
    }*/
    gUEFI_21_Supported = FALSE;
  }
  gHIIProtocolsInitialized = TRUE;

DONE:
  return Status;
}

//----------------------------------------------------------------------------
//
// Procedure:   _GetIFRPackageFromHII
//
// Description: Retrieve IFR forms buffer from the HII database
//
// Paramaters:  VOID
//
// Return:      EFI_STATUS
//----------------------------------------------------------------------------
EFI_STATUS _GetIFRPackageFromHII(UINT8 *HIIData, UINTN HIISize, UINT8 **IFRData, UINTN *IFRSize)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if( gUEFI_21_Supported )
  {
    Status = GetIFRPackageFromUEFI21HII( HIIData, HIISize, IFRData, IFRSize );
  }else
  {
    //Status = GetIFRPackageFromUEFI20HII( HIIData, HIISize, IFRData, IFRSize );
  }

  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   HiiLib_RetrieveHIIData
//
// Description: Retrive the HII Database and its CRC32
//
// Paramaters:  OUT UINT8 **HiiData - HII DB Buffer
//              OUT UINTN *DataSize - HII DB Size
//              OUT UINT32 *CRC32 - HII DB CRC32
//
// Return:      EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS HiiLib_RetrieveHIIData(UINT8 **HiiData, UINTN *DataSize, UINT32 *CRC32)
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    Status = _InitializeHIIProtocol();
    if( EFI_ERROR ( Status ) )
    {
        goto DONE;
    }
    
    if( gUEFI_21_Supported )
    {
        Status = RetrieveUEFI21HIIData( HiiData, DataSize );
    }else
    {
    //Status = RetrieveUEFI20HIIData( HiiData, DataSize );
    }
    if( EFI_ERROR ( Status ) )
    {
        goto DONE;
    }
    gHiiDataSize = (UINT64)*DataSize;
    CalculateCRC_32( *HiiData, ( UINT32 )*DataSize, ( UINT32 * )CRC32 );
    
  DONE:
    return Status;
}
