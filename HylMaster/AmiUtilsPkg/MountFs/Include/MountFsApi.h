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
//*****************************************************************//

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//**********************************************************************

extern EFI_GUID gMountFsProtocolGuid;

typedef EFI_STATUS (EFIAPI *MOUNT_CALLBACK) (IN EFI_HANDLE ImgHandle,IN UINTN FileSysType,IN UINT32 DiskSize,IN VOID* Buffer,IN CHAR16 *VolumeName,OUT UINTN *HandleIndex);
typedef EFI_STATUS (EFIAPI *UNMOUNT_CALLBACK) (IN EFI_HANDLE ImageHandle ,IN EFI_HANDLE MountFsHandle);
typedef EFI_STATUS (EFIAPI *SET_FS_VOL_NAME) (IN UINTN HandleIndex,IN CHAR16* VolName);
typedef EFI_STATUS (EFIAPI *GET_FS_VOL_NAME) (IN UINTN HandleIndex,OUT CHAR16* VolName);
typedef EFI_STATUS (EFIAPI *GET_MOUNT_FS_HANDLE) (IN CHAR16 *VolName,IN UINT64* MountFsID,OUT UINTN *OutHandleIndex);

typedef struct _MOUNT_FS_PROTOCOL
{
    MOUNT_CALLBACK 	 	Mount;
    UNMOUNT_CALLBACK 	Unmount;
    SET_FS_VOL_NAME  	SetFsVolName;
    GET_FS_VOL_NAME  	GetFsVolName;
    GET_MOUNT_FS_HANDLE GetFsHandle;
} MOUNT_FS_PROTOCOL;

#ifndef MOUNT_FS_DEVICE_PATH_GUID
#define MOUNT_FS_DEVICE_PATH_GUID \
  { \
    0x06ED4DD0, 0xFF78, 0x11d3, {0xBD, 0xC4, 0x00, 0xA0, 0xC9, 0x40, 0x53, 0xD1 } \
  }
#endif

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
