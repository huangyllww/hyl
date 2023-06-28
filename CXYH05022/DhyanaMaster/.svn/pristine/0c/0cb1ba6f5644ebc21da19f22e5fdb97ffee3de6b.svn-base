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

/** @file IndexCreationBin.h
    Defines structure for FirmwareConfigData protocol  

**/

#ifndef _INDEXCREATIONBIN_H_
#define	_INDEXCREATIONBIN_H_

typedef enum{
    QUESTIONIMPORT,
    PASSWORDIMPORT,
    RESETBIOSCONFIG
}JsonImportType;

EFI_STATUS CreateIndexJsonFile (OUT CHAR8 **Out_JsonBuffer,OUT UINTN *Out_JsonbufSize);
EFI_STATUS ImportIndexJsonData (CHAR8 *IndexBuffer, UINT32 IndexBufferSize, JsonImportType ImportType );

#endif

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
