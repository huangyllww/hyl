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
// Name:        AmiPldmBmcInterfaces.h
//
// Description: 
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>

#ifndef _AMIPLDMBMCINTERFACES_H_
#define _AMIPLDMBMCINTERFACES_H_

// {F9A83EB3-CBEA-4f30-BDD2-9BC22749A1A5}
#define AMIPLDM_BMC_DRIVER_PROTOCOL_GUID   \
    { \
        0xf9a83eb3, 0xcbea, 0x4f30, {0xbd, 0xd2, 0x9b, 0xc2, 0x27, 0x49, 0xa1, 0xa5 } \
    }

typedef struct _AMIPLDM_INTERFACE_PROTOCOL AMIPLDM_INTERFACE_PROTOCOL;

typedef struct _PLDMTables
{
	UINT8 *StringTable;
	UINT32 StringSize;
	UINT8 *AttributeTable;
	UINT32 AttributeSize;
	UINT8 *ValueTable;
	UINT32 ValueSize;
	UINT8 *MapStringTable;
	UINT32 MapSize;
} PLDMTables;

typedef struct _PLDMTableTags
{
	UINT32 StringTableTag;
	UINT32 AttributeTableTag;
	UINT32 ValueTableTag;
	UINT32 MapTableTag;
} PLDMTableTags;

typedef VOID (EFIAPI *AMI_PLDM_SET_MODE)(BOOLEAN PldmMode);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_EXPORT)(PLDMTables *PldmTables, BOOLEAN ExportVariables);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_IMPORT)(PLDMTables *PldmTables);
typedef EFI_STATUS (EFIAPI *AMI_HIIHANDLE_EXPORT)(EFI_HII_HANDLE *Handles,UINT64 Count,PLDMTables *PldmTables, BOOLEAN ExportVariables);
typedef EFI_STATUS (EFIAPI *AMI_HIIHANDLE_IMPORT)(EFI_HII_HANDLE Handle, PLDMTables *PldmTables);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_SAVE_IN_BMC)(PLDMTables *PldmTables);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_GET_FROM_BMC)(PLDMTables *PldmTables);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_SAVE_TAG_IN_BMC)(PLDMTableTags *TableTags);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_GET_TAG_FROM_BMC)(PLDMTableTags *TableTags);

typedef EFI_STATUS (EFIAPI *AMI_PLDM_SAVE_CUSTOM_METHOD)(PLDMTables *PldmTables);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_GET_CUSTOM_METHOD)(PLDMTables *PldmTables);

struct _AMIPLDM_INTERFACE_PROTOCOL
{
    UINT32 Revision;
	AMI_PLDM_SET_MODE SetPldmMode;
	AMI_PLDM_EXPORT CreatePldmTables;
    AMI_HIIHANDLE_EXPORT CreatePldmByHiiHandle;
	AMI_PLDM_IMPORT ImportPldmTables;
	AMI_HIIHANDLE_IMPORT ImportPldmByHiiHandle;
    AMI_PLDM_SAVE_IN_BMC SaveConfigurationInBmc;
    AMI_PLDM_GET_FROM_BMC RestoreConfigurationFromBmc;
    AMI_PLDM_SAVE_TAG_IN_BMC SetTagsInBmc;
    AMI_PLDM_GET_TAG_FROM_BMC GetTagsFromBmc;
    
    AMI_PLDM_SAVE_CUSTOM_METHOD SavePldmByCustomMethod;
    AMI_PLDM_GET_CUSTOM_METHOD RestorePldmByCustomMethod; 
};

extern EFI_GUID gAmiPldmInterfaceProtocolGuid;

#endif  //_AMIPLDMBMCINTERFACES_H_

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
