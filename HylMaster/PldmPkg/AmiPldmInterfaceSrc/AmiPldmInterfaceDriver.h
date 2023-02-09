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
// $Archive: $
//
// $Author: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
//*****************************************************************//
// Revision History
// ----------------
// $Log: $
//
//
//
//
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        AmiPldmInterfaceDriver.h
//
// Description: 
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _PLDMBMCDRIVER_H_
#define _PLDMBMCDRIVER_H_

#include "Efi.h"
typedef unsigned char BYTE;
typedef unsigned long ULONG;
typedef unsigned char *LPBYTE;
typedef unsigned int UINT;

#define PLDM_GET_BIOSTABLE              0x01
#define PLDM_SET_BIOSTABLE              0x02
#define PLDM_GET_BIOS_TABLE_TAGS        0x04
#define PLDM_SET_BIOS_TABLE_TAGS        0x05

#define GETDATANEXTPART     0x00;
#define GETDATAFIRSTPART    0x01;

#define TABLE_START               0x01
#define TABLE_MIDDLE              0x02
#define TABLE_END                 0x04
#define TABLE_START_AND_END       0x05
//Load optimal defaults codes
#define BMC_OEM_NET_FUNCTION    0x32
//****Defined codes ***///
#define TABLEDATASIZE50KB    0xC800
#define TABLEDATASIZE32KB    0x8000
#define TABLEDATASIZE30KB    0x7F00

#define INVALID_DATA_TRANSFER_HANDLE        0x80
#define INVALID_TRANSFER_OPERATION_FLAG     0x81
#define INVALID_TRANSFER_FLAG           0x82
#define BIOS_TABLE_UNAVAILABLE          0x83
#define INVALID_BIOS_TABLE_DATA_INTEGRITY_CHECK 0x84
#define INVALID_BIOS_TABLE_TYPE         0x85
#define BIOS_TABLE_TAG_UNAVAILABLE      0x86
#define INVALID_BIOS_TABLE_TAG_TYPE     0x87
#define INVALID_BIOS_ATTR_HANDLE        0x88

typedef struct _BMC_PLDMPAYLOAD
{
    BYTE    InstanceID: 5;
    BYTE    Reserved: 1;
    BYTE    Datagram: 1;
    BYTE    Request: 1;
    BYTE    PLDMType: 6;
    BYTE    HdrVer: 2;
    BYTE    PLDMCommand;
} BMC_PLDMPAYLOAD;

#pragma pack(1)
typedef struct _PLDM_GETBIOSTABLE_REQUEST
{
    UINT32  Handle;
    BYTE    TransferOperationFlag;
    BYTE    TableType;
} PLDM_GETBIOSTABLE_REQUEST;

typedef struct _PLDM_GETBIOSTABLE_RESPONSE
{
    BYTE    CompletionCode;
    UINT32  NextHandle;
    BYTE    TransferFlag;
} PLDM_GETBIOSTABLE_RESPONSE;

typedef struct _PLDM_SETBIOSTABLE_REQUEST
{
    UINT32  Handle;
    BYTE    TransferFlag;
    BYTE    TableType;
} PLDM_SETBIOSTABLE_REQUEST;

typedef struct _PLDM_SETBIOSTABLE_RESPONSE
{
    BYTE    CompletionCode;
    UINT32  NextDataTransferHandle;
}PLDM_SETBIOSTABLE_RESPONSE;

typedef struct _PLDM_GET_BIOS_TABLE_TAGS_REQUEST
{
    BYTE    NumberOfTables;
    BYTE    TableType[4];
} PLDM_GET_BIOS_TABLE_TAGS_REQUEST;

typedef struct _PLDM_GET_BIOS_TABLE_TAGS_RESPONSE
{
    BYTE    CompletionCode;
    UINT32 TableTag[4];
} PLDM_GET_BIOS_TABLE_TAGS_RESPONSE;

typedef struct _PLDM_SET_BIOS_TABLE_TAGS_REQUEST
{
    BYTE    NumberOfTables;
    BYTE    StringTableType;
    UINT32    StringTableTag;
    BYTE    attTableType;
    UINT32    attTableTag;
    BYTE    ValTableType;
    UINT32    ValTableTag;
    BYTE    MApTableType;
    UINT32    MapTableTag;
} PLDM_SET_BIOS_TABLE_TAGS_REQUEST;

typedef struct _PLDM_SET_BIOS_TABLE_TAGS_RESPONSE
{
    BYTE    CompletionCode;
} PLDM_SET_BIOS_TABLE_TAGS_RESPONSE;

#pragma pack()

EFI_STATUS EFIAPI PldmGetHiiData (UINT8 **HiiData, UINTN *DataSize, UINT32 *CRC32);
VOID EFIAPI PldmSetPldmMode (BOOLEAN PldmMode);
EFI_STATUS EFIAPI PldmCreatePldmTables (PLDMTables *PldmTables, BOOLEAN ExportVariables);
EFI_STATUS EFIAPI PldmImportPldmTables (PLDMTables *PldmTables);
EFI_STATUS EFIAPI PldmCreatePldmByHiiHandle (EFI_HII_HANDLE *Handles,UINT64 Count,PLDMTables *PldmTables, BOOLEAN ExportVariables);
EFI_STATUS EFIAPI PldmImportPldmByHiiHandle (EFI_HII_HANDLE Handles,PLDMTables *PldmTables);
EFI_STATUS EFIAPI PldmSaveConfigurationInBmc (PLDMTables *PldmTables);
EFI_STATUS EFIAPI PldmRestoreConfigurationFromBmc (PLDMTables *PldmTables);
EFI_STATUS EFIAPI PldmSetTagsInBmc (PLDMTableTags *TableTags);
EFI_STATUS EFIAPI PldmGetTagsFromBmc (PLDMTableTags *TableTags);

EFI_STATUS EFIAPI SavePldmByCustomMethod (PLDMTables *PldmTables);
EFI_STATUS EFIAPI RestorePldmByCustomMethod (PLDMTables *PldmTables);

EFI_STATUS PldmInterfaceUnload ( EFI_HANDLE ImageHandle );

EFI_STATUS ExecuteBMCOemIpmiCommand(
    UINT8 NetFunction, UINT8 Lun, UINT8 Command,
    UINT8 *CommandData, UINT32 CommandDataSize,
    UINT8 **ResponseData, UINT32 *ResponseDataSize, UINT8 *CommandCompletionCode
);

UINT8 GetOemIpmiCommand ();
UINT8 GetBmcOemNetFunction ();
UINT8 GetBmcOemLun ();

#define PLDM_INTERFACE_REVISION 10

#endif /* _PLDMBMCDRIVER_H_ */
