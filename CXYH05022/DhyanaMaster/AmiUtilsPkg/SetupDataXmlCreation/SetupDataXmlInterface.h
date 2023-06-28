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

/** @file BiosAttribCreationSrc.h
    Contains structure declarations related bios attributes and values

**/

#ifndef _SETUPDATA_XML_INTERFACE_H_
#define	_SETUPDATA_XML_INTERFACE_H_

#include <Include/AmiDxeLib.h>
#include <Token.h>

typedef EFI_STATUS (*GET_SETUP_DATA_XML) (void **Buffer, UINTN *BuffSize, UINT8 SplCtrlIndex);
typedef struct _SETUP_INFO_PROTOCOL 
{
    GET_SETUP_DATA_XML 	GetSetupDataXml;
}SETUP_INFO_PROTOCOL;
extern EFI_GUID gSetupInfoProtocolGuid;

#endif /* _SETUPDATA_XML_INTERFACE_H_ */

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
