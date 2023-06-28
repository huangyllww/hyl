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

/** @file FirmwareConfigDrvBin.h
    Contains structure declarations for FirmwareConfigDrv module part

**/

#ifndef _FW_CONFIG_DRV_BIN_H_
#define	_FW_CONFIG_DRV_BIN_H_

#include <Library/MemoryAllocationLib.h>

typedef struct
{
	CHAR8 		Filename [80];		//File name of the html
	CHAR8 		FileExtn [80];		//File name of the html
	EFI_GUID 	ResourceGUID;		//Resource guid for the html
} FILE_XREF;

typedef struct
{
	CHAR8 		Filename [80];		//File name of the html
} BIN_EXTNS;

EFI_STATUS 	InitBmcInstance ();
EFI_STATUS 	InitNwInstance ();
EFI_STATUS 	SendBMCGetRequest (CHAR8 *RequestStr, UINT32 RequestStrLen, VOID **Response, UINTN *ResponseLength);
void 		Dump_ContentToFile (BOOLEAN IsUnicodeChar,CONST CHAR16 *Filenamme,CONST VOID *String,UINTN Size);
EFI_STATUS 	SendGetRequest (CHAR16 *URL, VOID **Response, UINTN *ResponseLength);
EFI_STATUS 	SendPostRequest (CHAR16 *URL, CHAR8 *Postdata, UINTN PostLength);
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
#endif /* _FW_CONFIG_DRV_BIN_H_ */

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
