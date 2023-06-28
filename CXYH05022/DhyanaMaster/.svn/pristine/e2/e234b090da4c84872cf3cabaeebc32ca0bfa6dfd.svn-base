//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file NbMemCfg.h
    Definitions for Memory Config Hob passed to SmBios

**/
#ifndef __NBMEMCFG__H__
#define __NBMEMCFG__H__
#ifdef __cplusplus
extern "C" {
#endif

#include "Hob.h"

#define MEM_RESTORE_GUID \
{ 0xC3A4E49F, 0x485F, 0x4fd6, 0xA2, 0xEA, 0x2B, 0xC8, 0x74, 0x55, 0xAD, 0x4B }

#pragma pack (push, 1)
typedef struct {
     OUT   UINT32 Signature;           ///< "ASTR" for HYGON Suspend-To-RAM
     OUT   UINT16 Version;             ///< S3 Params version number
  IN OUT   UINT32 Flags;               ///< Indicates operation
  IN OUT   UINT32 NvStorage;          ///< Pointer to memory critical save state data
  IN OUT   UINT32 NvStorageSize;       ///< Size in bytes of the NvStorage region
  IN OUT   UINT32 VolatileStorage;    ///< Pointer to remaining HYGON save state data
  IN OUT   UINT32 VolatileStorageSize; ///< Size in bytes of the VolatileStorage region
} AMI_S3_PARAMS;

typedef struct {
	EFI_HOB_GUID_TYPE    Header;
    UINT8                Count;
    UINT32               Checksum;
    BOOLEAN              First;
    BOOLEAN              SaveMemContext;
    UINT8                NvTemp[0x1000];//Temporarily hardcoded
    AMI_S3_PARAMS        MemContext;
} MEM_RESTORE_HOB;
#pragma pack (pop)

extern EFI_GUID gMemRestoreGuid;

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
