//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:		ESADatasrcProtocol.h
//
// Description:	ESA Datasrc Protocol
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>


#ifndef _ESA_DATASRC_PROTOCOL_H_
#define _ESA_DATASRC_PROTOCOL_H_

//#define ESA_DATA_SOURCE_PROTOCOL_GUID \
//{ 0xb0c1621f, 0x8c63, 0x4655, 0x83, 0xf6, 0xb4, 0xb4, 0xbe, 0xfa, 0x1d, 0x75 }

extern EFI_GUID gDataSourceProtocolGuid;// = ESA_DATA_SOURCE_PROTOCOL_GUID;

typedef struct _ESA_DATA_SOURCE_PROTOCOL ESA_DATA_SOURCE_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *ESA_DS_GET_VALUE)(
        IN ESA_DATA_SOURCE_PROTOCOL *This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT void** Value,
        IN OUT UINT32* ByteLength
);


typedef
EFI_STATUS
(EFIAPI *ESA_DS_GET_STATE) (
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT UINT8* State
);

typedef
EFI_STATUS
(EFIAPI *ESA_DS_GET_FOCUSABLE)(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT BOOLEAN* Focusable
);

typedef
EFI_STATUS
(EFIAPI *ESA_DS_GET_DATASRC)(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT void** DataSrcStruct
);

typedef
EFI_STATUS
(EFIAPI *ESA_DS_CONTROL_CALLBACK)(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN void* Action,
        IN OUT void *Value
);

typedef struct _ESA_DATA_SOURCE_PROTOCOL
{
    EFI_GUID    DataSourceGuid;
    UINT16 EsaDataSourceProtocolVersion;
    ESA_DS_GET_VALUE EsaDataSourceGetValue;
    ESA_DS_GET_STATE EsaDataSourceGetState;
    ESA_DS_GET_FOCUSABLE EsaDataSourceGetFocusable;
    ESA_DS_GET_DATASRC EsaDataSourceGetDataSrc;
    ESA_DS_CONTROL_CALLBACK EsaDataSourceControlCallback;
}ESA_DATA_SOURCE_PROTOCOL;

#endif /* _ESA_DATASRC_INTERFACES_H_ */

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
