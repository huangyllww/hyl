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
PldmPreserve.h

Header file contains PLDM SCE related declarations

*/

#ifndef _PLDM_PRESERVE_H_
#define _PLDM_PRESERVE_H_

#include "Pi/PiBootMode.h"
#include "PldmSceStruct.h"

typedef struct _AMIPLDM_PRESERVE_INTERFACE_PROTOCOL AMIPLDM_PRESERVE_INTERFACE_PROTOCOL;
typedef struct _PresrvePLDMTables
{
    UINT8 *StringTable;
    UINT32 StringSize;
    UINT8 *AttributeTable;
    UINT32 AttributeSize;
    UINT8 *ValueTable;
    UINT32 ValueSize;
    UINT8 *MapStringTable;
    UINT32 MapSize;
} PresrvePLDMTables;
typedef EFI_STATUS (EFIAPI *AMI_PLDM_PRESERVE_EXPORT)(PresrvePLDMTables *PldmTables, BOOLEAN ExportVariables, void *Buffer, UINTN *BufferSize, UINTN Operation);
typedef EFI_STATUS (EFIAPI *AMI_PLDM_PRESERVE_IMPORT)(PresrvePLDMTables *PldmTables, void *Buffer, UINTN *BufferSize, UINTN Operation);
struct _AMIPLDM_PRESERVE_INTERFACE_PROTOCOL
{
    AMI_PLDM_PRESERVE_EXPORT CreatePldm;
    AMI_PLDM_PRESERVE_IMPORT ImportPldm;
};
extern EFI_GUID gAmiPreservePldmInterfaceProtocolGuid;
EFI_STATUS InstallPreserveInterface(IN EFI_HANDLE ImageHandle) ;
#define BCP_ONLY_DEFAULTS 0
#define BCP_ALL 1
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
