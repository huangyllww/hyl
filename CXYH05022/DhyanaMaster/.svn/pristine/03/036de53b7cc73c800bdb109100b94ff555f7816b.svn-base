//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, 	             **//
//**					Georgia 30093  					         **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
//
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AmiFwUpdateeLinks.c
//
// Description:	Defines Afuprotocol Flash Hook Functions
//
//<AMI_FHDR_END>
//**********************************************************************

#include "AmiDxeLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Token.h"
#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
#include "PldmSceStruct.h"
#endif

#define EFI_PLDM_FILE_PATH_GUID \
    { 0xa27ca271, 0x7b56, 0x4187, 0xae, 0x89, 0x69, 0x68, 0xe4, 0x78, 0x1d, 0xc3 }
#define EFI_PLDM_HASH_GUID \
    { 0xe94c338, 0x82b9, 0x4fc3, 0xb6, 0xc9, 0xa3, 0xc9, 0x58, 0xce, 0xd, 0x8c }

CHAR16   *gPldmFileName ;
UINT8    *gPldmHash;
UINTN    gHashSize;

extern EFI_BOOT_SERVICES     *gBS;
extern EFI_RUNTIME_SERVICES  *gRT;

void    *gLinkPldmBuffer = NULL;
UINT32      gLinkPldmBufferSize = 0;

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : StrLen8
//
// Description  : Function to calculate string length
//
// Parameter    : CHAR8 *
//              
// Return Value : UINTN
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN StrLen8(CHAR8 *string)
{
    UINTN i=0;
    while(string[i])
        i++;

    return i;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : StringDup16to8
//
// Description  : Function to convert char16 to char8
//
// Parameter    : CHAR16 *
//              
// Return Value : CHAR8 *
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
CHAR8 *StringDup16to8 (CHAR16 *string)
{
    CHAR8      *text;
    UINTN       i = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    
    Status = gBS->AllocatePool(EfiBootServicesData, ((1 + StrLen (string)) * sizeof(CHAR8)),(void **)&text);
    
    if (EFI_ERROR (Status))
    {
        return NULL;
    }
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR8)string[i])
            i++;
    }
    return text;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : StringDup8to16
//
// Description  : Function to convert char8 to char16
//
// Parameter    : CHAR8 *
//              
// Return Value : CHAR16 *
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
CHAR16 *StringDup8to16 (CHAR8 *string)
{
    CHAR16      *text;
    UINTN       i = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    
    Status = gBS->AllocatePool(EfiBootServicesData, ((1 + StrLen8 (string)) * sizeof(CHAR16)),(void **)&text);
    
    if (EFI_ERROR (Status))
    {
        return NULL;
    }
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
    }
    return text;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   VarGetNvramName
//
// Description: function to get the NvRam Variable Name
//
// Input:       CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size
//
// Output:      Name String
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *FW_VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size )
{
    VOID *buffer = NULL;
    UINT32 attrib = 0;
    EFI_STATUS Status = EFI_UNSUPPORTED;


    if ( attributes != NULL )
    {
        attrib = *attributes;
    }


  Status = gRT->GetVariable(
                 name,
                 guid,
                 &attrib,
                 size,
                 buffer
             );

    if ( Status != EFI_BUFFER_TOO_SMALL )
    {
        return buffer;
    }

    buffer = AllocateZeroPool ( *size + 1);

    if ( buffer == NULL )
    {
        return buffer;
    }

    Status = gRT->GetVariable(
                 name,
                 guid,
                 &attrib,
                 size,
                 buffer
             );

    if ( EFI_ERROR( Status ) )
    {
        FreePool((VOID *)buffer );
        buffer = NULL;
    }

    if ( attributes != NULL )
    {
        *attributes = attrib;
    }

    return buffer;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   GetAmiBcpServicesProtocolInstance
//
// Description: function to get the BCP protocol interface
//
// Input:       void
//
// Output:      AMI_BCP_SERVICE_FLASH_PROTOCOL*
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
AMI_BCP_SERVICE_FLASH_PROTOCOL* GetAmiBcpServicesProtocolInstance(VOID)
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

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure  : PreservePLDMVariablesToBuffer
//
// Description: Function to store PLDM variables NVRAM to BUFFER
//
// Input      : UINT32 Flag 
//
// Output     : VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID PreservePLDMVariablesToBuffer( UINT32 Flag )
{
    EFI_STATUS Status = EFI_SUCCESS ;
    EFI_GUID    PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID;
    EFI_GUID    PldmHashGuid = EFI_PLDM_HASH_GUID;
    CHAR8       *FileName8 = NULL;
    void    *FVPldmBuffer = NULL;
    UINT32      FVPldmBufferSize = 0;
        
#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
    if(FVDataAccessType())
    {
        AMI_BCP_SERVICE_FLASH_PROTOCOL *AmiBcpServiceProtocol = GetAmiBcpServicesProtocolInstance(); // return protocol instance
                    
        if(AmiBcpServiceProtocol != NULL)
        {
            if( (EFI_SUCCESS == AmiBcpServiceProtocol->FVData( &FVPldmBuffer, &FVPldmBufferSize)) && (FVPldmBuffer) &&  (FVPldmBufferSize != 0))
            {
                gLinkPldmBuffer = FVPldmBuffer;
                gLinkPldmBufferSize = FVPldmBufferSize;
            }
        }
    }
#endif
    gHashSize = 0;
    
    //For recovery also boot mode coming as flash update so removing the check
    //For capsule is not needed. So this case will be handled there
    //There won't be any filename for capsule mode

    FileName8 = FW_VarGetNvramName (L"PldmFileName", &PldmFilePathGuid, NULL, &gHashSize);
    TRACE ((-1,"BiosConfigPreserve: FileName8 is %s gHashSize is %x\n", FileName8, gHashSize));
    if (NULL != FileName8)
    {
        gPldmFileName = StringDup8to16 (FileName8);
        TRACE ((-1,"BiosConfigPreserve: gPldmFileName is %S\n", gPldmFileName));
        if (NULL == gPldmFileName)
        {
            return;
        }
        if (FileName8)
            gBS->FreePool (FileName8);
    }

    gHashSize = 0;
    gPldmHash = FW_VarGetNvramName (L"PldmHash", &PldmHashGuid, NULL, &gHashSize);
    TRACE ((-1,"BiosConfigPreserve: gPldmHash address is %x gHashSize is %x\n", gPldmHash, gHashSize));
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure  : RestorePLDMVariablesToNVRAM
//
// Description: Function to restore PLDM variables BUFFER to NVRAM
//
// Input      : UINT32 Flag 
//
// Output     : VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID RestorePLDMVariablesToNVRAM( UINT32 Flag )
{
    EFI_STATUS Status = EFI_SUCCESS ;
    EFI_GUID PldmFilePathGuid = EFI_PLDM_FILE_PATH_GUID ;
    EFI_GUID PldmHashGuid = EFI_PLDM_HASH_GUID ;
    CHAR8 *Temp = (CHAR8 *)NULL ;

    
#if(BIOS_CONFIG_PRESERVE_BIN_SUPPORT)
    
    if(FVDataAccessType())
    {
        AMI_BCP_SERVICE_FLASH_PROTOCOL *AmiBcpServiceProtocol = GetAmiBcpServicesProtocolInstance(); // return protocol instance
    
        if(AmiBcpServiceProtocol != NULL)
        {
            if( (gLinkPldmBuffer != NULL) && (gLinkPldmBufferSize != 0) )
            {
                SCE_PLDM_DATA_HEADER *ScePldmFVData = NULL;
                
                ScePldmFVData = (SCE_PLDM_DATA_HEADER *)gLinkPldmBuffer;
                TRACE ((-1,"AmiFwUpdateElinks: gLinkPldmBuffer: %x, ScePldmFVData->Signature %x \n", gLinkPldmBuffer, ScePldmFVData->Signature));
                
                TRACE((-1,"AmiFwUpdateElinks:gLinkPldmBufferSize %d \n",gLinkPldmBufferSize));
                AmiBcpServiceProtocol->RuntimeFlash( gLinkPldmBuffer, NULL, gLinkPldmBufferSize, NULL) ;   // Getting PLDM buffer to IMPORT
                if(gLinkPldmBuffer)
                {
                    FreePool(gLinkPldmBuffer);
                    gLinkPldmBuffer = NULL;
                    gLinkPldmBufferSize = 0;
                }
            }
        }
    }
#endif
    
    if ((NULL != gPldmFileName)) 
    {
        Temp = StringDup16to8( gPldmFileName ) ;
      
        Status = gRT->SetVariable(
                L"PldmFileName",
                &PldmFilePathGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                gHashSize,
                Temp
                );

        TRACE ((-1,"BiosConfigPreserve PldmFileName: Status is %r %d\n", Status, gHashSize));
        TRACE ((-1,"BiosConfigPreserve: Status for deleting PldmFileName is\n"));
    }

    if ((NULL != gPldmHash)) 
    {
        Status = gRT->SetVariable(
               L"PldmHash",
               &PldmHashGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               gHashSize,
               gPldmHash
               );
    }

    TRACE ((-1,"BiosConfigPreserve PldmHash: Status is %r %d\n", Status,gHashSize));
}
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, 	             **//
//**					Georgia 30093  					         **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
