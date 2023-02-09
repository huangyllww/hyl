//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**             5555 Oakbrook Pkwy   , Norcross, GA 30071       **//
//**                                                             **//
//**                     Phone: (770)-246-8600                   **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive:  $
//
// $Author:  $
//
// $Revision:  $
//
// $Date: $
//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		commonhelper.c
//
// Description:	This file is Helper file for Boot only TSE module
// 				This functions may be overridden by Other Advanced TSE module.
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#include "Protocol/HiiString.h"
#include "Protocol/HiiDatabase.h"
#include "Token.h"
#include "AmiDxeLib.h"
#include <Protocol/FirmwareVolume2.h>
#include <Library/MemoryAllocationLib.h>

extern VOID *
EFIAPI
AllocatePool (
  IN UINTN  AllocationSize
  );
CHAR8 *gLang = (CHAR8*)NULL;


extern EFI_RUNTIME_SERVICES  *gRT;
extern EFI_BOOT_SERVICES     *gBS;
extern EFI_SYSTEM_TABLE      *gST;

#define INVALID_HANDLE  ((VOID*)(UINTN)-1)
#define NULL_HANDLE NULL

extern EFI_HII_HANDLE gAMIFWUpdateHiiHandle;
extern EFI_GUID gEfiGlobalVariableGuid;

EFI_HII_STRING_PROTOCOL *gHiiStringProtocol = (EFI_HII_STRING_PROTOCOL *)NULL;
UINTN StrLen (  IN      CONST CHAR16              *String  );

EFI_STATUS HiiLibGetStringEx(
    IN EFI_HII_HANDLE HiiHandle, IN UINT16 StringId, IN CHAR8 *Language,
    IN OUT UINTN *StringSize, OUT EFI_STRING String
);

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   FWU_VarGetNvramName
//
// Description: function to get the NvRam Variable Name
//
// Input:       CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size
//
// Output:      Name String
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID *FWU_VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size )
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

EFI_STATUS UefiHiiWrapperGetString( EFI_HII_HANDLE handle, CHAR8 *Language, EFI_STRING_ID StringId, CHAR16 *buffer,UINTN *bufferSize )
{
        if ( handle == INVALID_HANDLE || (handle == NULL_HANDLE))
	        handle = gAMIFWUpdateHiiHandle;

        return gHiiStringProtocol->GetString(gHiiStringProtocol, Language, handle, StringId, buffer, bufferSize, (EFI_FONT_INFO **)NULL);
}  

CHAR16 *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR8 *lang)
{
    EFI_STATUS status = EFI_SUCCESS;
    CHAR16 *buffer = (CHAR16 *)NULL;
    UINTN bufferSize = 0,Size=0;
    CHAR8 *language = lang;

		UINTN Strlen = 0;
    if(NULL == language)//EIP-75351 Suppress the warnings from static code analyzer
        return (CHAR16 *)NULL;
    if ( handle == INVALID_HANDLE || (handle == NULL_HANDLE))
	        handle = gAMIFWUpdateHiiHandle;     
    status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);  

    if(status == EFI_BUFFER_TOO_SMALL)
    {
	    buffer = AllocatePool( bufferSize );
        if ( buffer == NULL )
            return buffer;   

        status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);
    }

		Strlen = StrLen(buffer);
    // String not found with currnet Lang
    if ((( EFI_ERROR(status) ) && (bufferSize==0)) || !Strlen)
    {
        language = "en-US";//DEFAULT_LANGUAGE_CODE;
        status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
        
        if(status == EFI_BUFFER_TOO_SMALL)
        {
        	buffer = AllocatePool( bufferSize );
            if ( buffer == NULL )
                return buffer;   
            status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
        }
        // String not found in Default Lang
        if (( EFI_ERROR(status) )&&(bufferSize==0))
        {
            if (AsciiStrCmp(language, "en-US") != 0)
            {
                //MemFreePointer( (VOID **)&language );
                language = "en-US";
                status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
                // string not found in english
                if (( EFI_ERROR(status) )&&(bufferSize==0))
                {
                    //81617 : Avoiding memory leaks in TSE
                    //MemFreePointer( (VOID **)&language );
                    goto DONE;
                }
            }
            else
            {
                //81617 : Avoiding memory leaks in TSE
                //MemFreePointer( (VOID **)&language );
                status = EFI_NOT_FOUND;
                goto DONE;
            }
        }
    }

DONE:
    if ( EFI_ERROR(status))
    {
        // Allocate a single char and return an empty string
        if(buffer)
        	FreePool( (VOID *)buffer );
        buffer = AllocatePool( sizeof(CHAR16) );
        if(buffer)
            *buffer = L'\0';
    }
    return buffer;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:          UpdateMultilanguageVar
//
// Description:        To get the language selected from NVRAM variables
//
//
// Return Value:       void
//
//--------------------------------------------------------------------
//<AMI_PHDR_END>
void UpdateMultilanguageVar()
{
UINTN Size = 0;
	gLang = (CHAR8 *)FWU_VarGetNvramName( (CHAR16 *)L"PlatformLang", &gEfiGlobalVariableGuid , (UINT32 *)NULL , &Size );
	if (NULL == gLang)
	{
		gLang = "en";
	}
}

CHAR16 *FwUHiiGetStringLanguage( VOID* Handle, UINT16 StringId , CHAR16* Lang)
{
	CHAR16 *String = (CHAR16 *)NULL  ;
	UINTN StringSize =1000 ;
	EFI_STATUS Status = 0 ;
	EFI_GUID HiiStringGUID = EFI_HII_STRING_PROTOCOL_GUID ;
	EFI_HII_STRING_PROTOCOL *HiiStringProtocol = (EFI_HII_STRING_PROTOCOL *)NULL ;
	
	if(!Handle || (Handle == INVALID_HANDLE))
	    Handle = gAMIFWUpdateHiiHandle;
	
	if((Handle == INVALID_HANDLE) || (StringId == 0))
	{
	    //return an empty string
	    CHAR16* Buffer = (CHAR16*)NULL; 
	    Buffer = AllocatePool( sizeof(CHAR16) );
		if(Buffer)
		    *Buffer = L'\0';
	    return (CHAR16 *)Buffer;
}
return Uefi21HiiGetStringLanguage(Handle, StringId, gLang);
}


EFI_STATUS AMIFWUpdateHiiInitializeProtocol(VOID)
{
		EFI_STATUS Status = EFI_SUCCESS;

		Status = gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL,(VOID**) &gHiiStringProtocol);

		return Status;
}


EFI_STATUS  RemoveStringPack()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HII_DATABASE_PROTOCOL *gHiiDatabase = (EFI_HII_DATABASE_PROTOCOL *)NULL;

    Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabase);
    if (EFI_SUCCESS == Status)
    {
            Status = gHiiDatabase->RemovePackageList(gHiiDatabase, gAMIFWUpdateHiiHandle);
            gAMIFWUpdateHiiHandle = (EFI_HII_HANDLE)NULL;
    }

    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
