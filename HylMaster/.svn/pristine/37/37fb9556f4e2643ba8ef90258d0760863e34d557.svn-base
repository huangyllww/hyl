//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
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
// $Log:  $
// 
// 
//*****************************************************************//
//*****************************************************************//
//<AMI_FHDR_START>
//---------------------------------------------------------------------------
// Name:		FileSystemAccess.h
//
// Description: This file contains the defines and function prototypes of
//              all the functions used in FileSystemAccess.c
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------


#ifndef __FILESYSTEM_INFO_H__
#define __FILESYSTEM_INFO_H__

#include "Efi.h"
#include "AmiLib.h"
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#ifndef	MAX_VOL_LBL_LEN
#define MAX_VOL_LBL_LEN 256
#endif

//Target Disk Details Struct
typedef struct _TARGET_FSINFO_
{
    EFI_HANDLE FsHandle;
    CHAR16 VolumeName[MAX_VOL_LBL_LEN];
    CHAR16 FriendlyName[MAX_VOL_LBL_LEN];
}TARGET_FSINFO;


//---------------------------------------------------------------------------
// Function Definition
//---------------------------------------------------------------------------
EFI_STATUS AMI_FS_GetFileSystemsInformation(TARGET_FSINFO **TargetDIskInfo,UINT32 *DiskCount);
EFI_STATUS AMI_FS_GetFSHandleforBlock(EFI_HANDLE BlockIOHandle,EFI_HANDLE **FsHandle,UINT32 *HandleCount);
CHAR16 * AMI_FS_GetVolumeNameForFSHandle( EFI_HANDLE FsHandle);
CHAR16* AMI_FS_GetDiskName(EFI_HANDLE Handle);
EFI_HANDLE USBBlockioHandle(EFI_HANDLE InFSHandle);
VOID InitGlobals(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE * UefiSysTable);
#endif  //__FILESYSTEM_INFO_H__

//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
