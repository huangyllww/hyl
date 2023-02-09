//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file 
PldmSceStruct.h

Header file contains PLDM SCE related declarations

*/

#ifndef _PLDM_SCE_STRUCT_H_
#define _PLDM_SCE_STRUCT_H_

#include "Pi/PiBootMode.h"

#define EFI_PLDM_FILE_PATH_GUID \
	{ 0xa27ca271, 0x7b56, 0x4187, 0xae, 0x89, 0x69, 0x68, 0xe4, 0x78, 0x1d, 0xc3 }

#define EFI_PLDM_HASH_GUID \
	{ 0xe94c338, 0x82b9, 0x4fc3, 0xb6, 0xc9, 0xa3, 0xc9, 0x58, 0xce, 0xd, 0x8c }

#define EFI_PLDM_CAPSULE_GUID \
	{ 0x3b6ec65b, 0x1d2d, 0x4fc7, 0xba, 0xe2, 0x70, 0x7b, 0x28, 0x39, 0x85, 0xa0 }

#define AMI_DIGITAL_SIGNATURE_PROTOCOL_GUID  \
    { 0x5f87ba17, 0x957d, 0x433d, 0x9e, 0x15, 0xc0, 0xe7, 0xc8, 0x79, 0x88, 0x99 }

#define EFI_HASH_ALGORITHM_SHA256_GUID \
  { \
    0x51aa59de, 0xfdf2, 0x4ea3, {0xbc, 0x63, 0x87, 0x5f, 0xb7, 0x84, 0x2e, 0xe9 } \
  } 

#define AFU_BCP_NVRAM_FLASH_GUID  \
    { 0x90311f9a, 0x295a, 0x4367, 0xae, 0x49, 0x60, 0x82, 0x1f, 0x27, 0x4d, 0x3b }

#define EFI_BCP_FV_DATA_GUID \
  { \
    0x11316db2, 0xd5a0, 0x4af0, {0xa1, 0xcd, 0x1a, 0x4a, 0xff, 0x26, 0x9a, 0xa1 } \
  } 

#define FLASH_DEVICE_FV_BASE (0xFFFFFFFF - FLASH_SIZE + 1)
#define BCP_FFS_DATA_START_OFFSET   156
#define BCP_PLDM_SIGNATURE_OFFSET   160

#pragma pack (1)
typedef struct
{
	UINT32 		Version;
	UINT32		Signature;
	UINT32		StringOffset;
	UINT32		StringSize;
	UINT32		AttribOffset;
	UINT32		AttribSize;
	UINT32		ValueOffset;
	UINT32		ValueSize;
	UINT32		MappingString;
	UINT32		MapStringSize;
}SCE_PLDM_DATA_HEADER;

typedef struct _PLDM_FSINFO_
{
	UINT8  *PldmStringTable;     // value table pointer
	UINT32  PldmStringTableSize;
	UINT8  *PldmAttribTable; // attribute table from foreign system
	UINT32  PldmAttribTableSize;
	UINT8  *PldmValueTable; // x-AMI string table from foreign system
	UINT32  PldmValueTableSize;
	UINT8  *PldmMapStringTable; // english string table from foreign system
	UINT32  PldmMapStringTableSize;
}PLDM_FSINFO;

extern EFI_STATUS
ImportMappedPldmValues(
    UINT8  *HiiData,        // pointer to HII export data
    UINT32  HiiSize,        // size of Hii Data
    UINT8  *ValueTable,     // value table pointer
    UINT32  ValueSize,
    UINT8  *AttributeTable, // attribute table from foreign system
    UINT32  AttributeSize, 
    UINT8  *MapStringTable, // x-AMI string table from foreign system
    UINT32  MapStringSize,
    UINT8  *EngStringTable, // english string table from foreign system
    UINT32  EngStringSize
    );

#define AMI_BCP_FLASH_PROTOCOL_GUID \
 { 0x36af3f94, 0x6bdb, 0x4c32, { 0x80, 0x6b, 0x8c, 0x3d, 0x35, 0xba, 0x8b, 0xff } }


typedef
EFI_STATUS
(EFIAPI *AMI_BCP_SERVICE_BUFFER)(
void **, UINT32 *
    );

typedef
EFI_STATUS
(EFIAPI *AMI_BCP_SERVICE_CAPSULE)(
void *, UINT32
    );

typedef
EFI_STATUS
(EFIAPI *AMI_BCP_SERVICE_RUNTIME)(
 void *, CHAR16 *, UINT32,EFI_HANDLE
    );

typedef
EFI_STATUS
(EFIAPI *AMI_BCP_SERVICE_FV)(
 void **, UINT32*
    );

typedef struct {
    AMI_BCP_SERVICE_BUFFER     GetPLDMBuffer;
    AMI_BCP_SERVICE_CAPSULE    CapsuleFlash;
    AMI_BCP_SERVICE_RUNTIME    RuntimeFlash;
    AMI_BCP_SERVICE_FV         FVData;
} AMI_BCP_SERVICE_FLASH_PROTOCOL;

typedef enum {
                                RUNTIME,
                                RECOVERY,
                                CAPSULE,
                                ESRT
}FLASH_UPDATE_MODE;

extern EFI_BOOT_SERVICES     *gBS;
extern EFI_SYSTEM_TABLE 	*gST;
extern EFI_RUNTIME_SERVICES *gRT;

extern EFI_STATUS 	AMI_RetrieveHIIData( UINT8 **HiiData, UINTN *DataSize, UINT32 *CRC32 );
extern UINTN		EfiStrLen (  IN CHAR16   *String  );
extern void 		EfiStrCpy(IN CHAR16 *Destination, IN CHAR16 *Source);
extern void 		*VarGetNvramName( CHAR16 *name , EFI_GUID *guid , UINT32 *attributes , UINTN *size );
extern EFI_STATUS 	VarSetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 attributes, VOID *buffer, UINTN size );
extern void 		*MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize );
CHAR16 				*PldmStrDup8to16 (CHAR8 *string);
EFI_STATUS 			Apply_Pldm_Offset( EFI_HANDLE FSHandle);
EFI_BOOT_MODE EFIAPI GetBootModeHob (VOID);
EFI_STATUS 			CompareHashAndImportPldm (UINT8 *Buffer, UINT32 BufferSize);
EFI_STATUS          CompareFVHashAndImportPldm (void);
void 				RuntimePldmImport (void);
EFI_STATUS Preserve_flash_Update( IN void *,CHAR16 *, IN UINT32, EFI_HANDLE  ) ;
EFI_STATUS Get_FV_Data( OUT void **,OUT UINT32*) ;
EFI_STATUS UpdateCapsuleToPush(IN void *, IN UINT32 ) ;
EFI_STATUS CreatePldmForConfigPreserving( OUT void **, IN OUT UINT32 * ) ;
BOOLEAN FVDataAccessType () ;
UINTN FVFlashDataSize () ;
UINTN FVFlashBlockSize () ;
UINTN FVBCPVolumeOffset () ;
UINTN FVBCPVolumeSize () ;
EFI_STATUS ChangeUpdateModePreservingToRuntime (FLASH_UPDATE_MODE FlashMode, UINT8 *Buffer, UINT32 BufferSize);
void ImportUpdateModePreservingToRuntime (void);
CHAR8* StrDup16to8(CHAR16 *String);

typedef struct {
    UINT32  Signature;
    UINT32  Size;
} ROM_INFO;

//BcpQueryVariableInfo variable related definitions
//Assuming 10Kb as max. size of variables set from the modules loaded after BCP
#define VAR_STORAGE_RESERVE_SIZE 0x4000;

typedef struct {
    UINT64 MaximumVariableStorageSize;
    UINT64 RemainingVariableStorageSize;
    UINT64 MaximumVariableSize; 
}BCP_QUERYVARIABLE_INFO;
//BcpQueryVariableInfo variable related definitions ends
#pragma pack ()

#endif

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
