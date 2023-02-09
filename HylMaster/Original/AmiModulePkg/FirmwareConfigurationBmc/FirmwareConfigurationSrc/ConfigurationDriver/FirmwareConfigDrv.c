//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file FirmwareConfigDrv.c
    Functions defined to return the SDL token values

**/

//---------------------------------------------------------------------------

#include "AmiLib.h"
#include "Token.h"
#include "FwConfigElinks.h"
#include "AmiDxeLib.h"
#include "FirmwareConfigDrvBin.h"
#include "Library/PcdLib.h"
#include <Library/HiiLib.h>
#include "Token.h"
//---------------------------------------------------------------------------
extern CHAR16 *ConfiguringFW;
extern CHAR16 *FWServerIP;
extern CHAR16 *Response;
extern CHAR16 *Success;
extern CHAR16 *TimeOut;
extern CHAR16 *ConfiguringFWFailed;
extern CHAR16 *LoadDefaults;
extern CHAR16 *AttribRegistry;
extern CHAR16 *IndexJson;
extern CHAR16 *ImportFWConfig;
extern CHAR16 *ImportPassword;
extern CHAR16 *PushUIApp;
extern CHAR16 *EraseLine;
extern EFI_HII_HANDLE gHiiHandle;

//---------------------------------------------------------------------------

FILE_XREF  gFwHtmlFiles [] = {
							FW_UI_APP_FILE_DETAILS
							{"",{ 0,0,0,0,0,0,0,0,0,0,0 }}
							};
UINTN 		gFwHtmlFilesCount = sizeof (gFwHtmlFiles)/ sizeof (FILE_XREF);
BIN_EXTNS  	gFwUiAppBinExtns [] = {FW_UI_APP_BINARY_FILE_EXTN, ""};
UINTN 		gFwUiAppBinExtnsCount = sizeof (gFwUiAppBinExtns)/ sizeof (BIN_EXTNS);

/**
    Returns Dynamic PCD token PcdManualJsonCreation value
     
    @param void

    @retval BOOLEAN
     
**/
BOOLEAN GetFw_ManualJsonCreation ()
{
   return  PcdGetExBool(&gFWCTokenSpaceGuid, PcdManualJsonCreation);
}

/**
    Returns Dynamic PCD token PcdDumpFWConfigOnly value
     
    @param void

    @retval BOOLEAN
     
**/
BOOLEAN DumpFirmwareConfigOnly()
{
    return  PcdGetExBool(&gFWCTokenSpaceGuid, PcdDumpFWConfigOnly);
}

/**
    Reads htmls from firmware or filesystem

    @param IN EFI_GUID *FileNameGuid, OUT VOID **Image, OUT UINTN *ImageSize

    @retval EFI_STATUS

**/
EFI_STATUS FwGetHtmlFromFV (IN EFI_GUID *FileNameGuid, OUT VOID **Image, OUT UINTN *ImageSize)
{
	EFI_STATUS                    Status;
	UINTN                         FvProtocolCount;
	EFI_HANDLE                    *FvHandles;
	UINTN                         Index;
	UINT32                        AuthenticationStatus;
	
#if (PI_SPECIFICATION_VERSION < 0x00010000)
	EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
#else
	EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
#endif
	
	Status = pBS->LocateHandleBuffer (
					ByProtocol,
#if (PI_SPECIFICATION_VERSION < 0x00010000)
					&gEfiFirmwareVolumeProtocolGuid,
#else
					&gEfiFirmwareVolume2ProtocolGuid,
#endif
					NULL,
					&FvProtocolCount,
					&FvHandles
					);
	if (EFI_ERROR (Status)) 
		return EFI_NOT_FOUND;
	
	for (Index = 0; Index < FvProtocolCount; Index++)
	{
		Status = pBS->HandleProtocol (
						FvHandles[Index],
#if (PI_SPECIFICATION_VERSION < 0x00010000)
						&gEfiFirmwareVolumeProtocolGuid,
#else
						&gEfiFirmwareVolume2ProtocolGuid,
#endif
						(VOID **) &Fv
						);
		
		if (EFI_ERROR (Status))
		{
			continue;
		}
		
		// Assuming Image and ImageSize are correct on input.
		Status = Fv->ReadSection (
						Fv,
						FileNameGuid,
						EFI_SECTION_RAW,
						0,
						Image,
						ImageSize,
						&AuthenticationStatus
						);
		
		if (!EFI_ERROR (Status))
		{
			pBS->FreePool(FvHandles);
			return EFI_SUCCESS;
		}
		else if (Status == EFI_BUFFER_TOO_SMALL) // ImageSize updated to needed size so return
		{
			pBS->FreePool(FvHandles);
			return EFI_BUFFER_TOO_SMALL;
		}
	}
	pBS->FreePool(FvHandles);
	return EFI_NOT_FOUND;
}

/**
    Initializing string from HII 

    @param void

    @retval void

**/
void InitializingFWConfigHIIString()
{
	ConfiguringFW = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_CONFIGURE_FW), NULL);
	FWServerIP = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_SERVER_IP), NULL);
	Response = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_RESPONSE), NULL);
	Success = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_SUCCESS), NULL);
	TimeOut = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_TIME_OUT), NULL);
	ConfiguringFWFailed = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_CONFIGURE_FW_FAILED), NULL);
	LoadDefaults = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_LOADING_DEFAULT), NULL);
	AttribRegistry = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_ATTRIB_REG), NULL);
	IndexJson = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_INDEX_JSON), NULL);
	ImportFWConfig = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_IMPORTING_FWCONFIG), NULL);
	ImportPassword = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_IMPORTING_PASSWORD), NULL);
	PushUIApp = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_UI_APP_FILES), NULL);
  EraseLine = (CHAR16*)HiiGetString(gHiiHandle, STRING_TOKEN(STR_ERASE_LINE), NULL);

}
//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
