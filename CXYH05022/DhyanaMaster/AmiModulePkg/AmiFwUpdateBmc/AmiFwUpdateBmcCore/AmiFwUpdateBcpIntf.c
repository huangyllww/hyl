//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//**********************************************************************
// $Archive: $
//
// $Author: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
// Revision History
// ----------------
// $Log: $
//
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiFwUpdateBcpIntf.c
//
// Description: To link with the BCP module dependency functions
//
//<AMI_FHDR_END>
//**********************************************************************

#include <Efi.h>
#include <Library/UefiLib.h>
#include "Token.h"

#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
#include "PldmSceStruct.h"
#endif

typedef enum {FwRuntime,FwRecovery,FwCapsule}Flash_Type;
extern CHAR16 *UI_ROM_FileName(void) ;
extern UINTN Get_BCP_CheckBoxState() ;
extern CHAR16 gROMfilename [];

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : GetAmiBcpCapsuleServicesProtocolInstance
//
// Description  : The function returns AmiBcpServiceProtocol instance
//
// Parameter    : VOID
//              
// Return Value : AMI_BCP_SERVICE_FLASH_PROTOCOL *
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
AMI_BCP_SERVICE_FLASH_PROTOCOL* GetAmiBcpCapsuleServicesProtocolInstance(VOID)
{
    UINTN       NoHandles = 0;
    UINT32      Index = 0;
    EFI_HANDLE  *pHandleBuffer = NULL;
    AMI_BCP_SERVICE_FLASH_PROTOCOL *AmiBcpServiceProtocol = NULL;
    EFI_STATUS Status;
    EFI_GUID BcpCapsuleGuid = AMI_BCP_FLASH_PROTOCOL_GUID ;

    Status = gBS->LocateProtocol(&BcpCapsuleGuid, NULL, &AmiBcpServiceProtocol);        // Locating for BIOS Configuration preserve

    if (EFI_ERROR(Status)) 
        AmiBcpServiceProtocol = NULL;

    return AmiBcpServiceProtocol;
}
#endif

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : PreserveDatainFlash
//
// Description  : Function is responsible for preserving data in for the different type flash
//
// Parameter    : enum FlashType Flashtype
//              
// Return Value  : VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
void PreserveDatainFlash(Flash_Type Flashtype)
{
#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
        if( 1 == Get_BCP_CheckBoxState() )
        {
            void    *PldmBuffer = NULL;
            UINT32      PldmBufferSize = 0;
            
            AMI_BCP_SERVICE_FLASH_PROTOCOL *AmiBcpServiceProtocol = GetAmiBcpCapsuleServicesProtocolInstance(); // return protocol instance
            
            if(AmiBcpServiceProtocol != NULL)
            {
                AmiBcpServiceProtocol->GetPLDMBuffer( &PldmBuffer, &PldmBufferSize) ;   // Getting PLDM buffer to IMPORT
                
                if( (PldmBuffer != NULL) && (PldmBufferSize != 0) )
                {
                    if( FwCapsule == Flashtype )
                    {
                        AmiBcpServiceProtocol->CapsuleFlash(PldmBuffer, PldmBufferSize);    // Function is responsible for preserving data in CAPSULE flash
                    }
                    else if(FwRuntime == Flashtype)
                    {
                        AmiBcpServiceProtocol->RuntimeFlash(PldmBuffer, gROMfilename, PldmBufferSize, NULL);     // responsible to preserve data in RUNTIME flash
                    }
                    else if(FwRecovery == Flashtype)
                    {
                        AmiBcpServiceProtocol->RuntimeFlash(PldmBuffer, gROMfilename, PldmBufferSize, NULL); // responsible to preserve data in RECOVERY flash
                    }
                }
            }
        }
#endif
}
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2015, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
