#ifndef _MOUNT_FS_H_
#define _MOUNT_FS_H_

#include <Include/AmiLib.h>
#include "Library/UefiLib.h"
#include <Library/MemoryAllocationLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Include/MountFsApi.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/SimpleFileSystem.h>
//#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>

extern EFI_BOOT_SERVICES 		*gBS;
extern EFI_SYSTEM_TABLE 		*gST;
extern EFI_RUNTIME_SERVICES 	*pRS;

#define MIN32_DEFAULT_DISK_SIZE 34
#define MIN16_DEFAULT_DISK_SIZE 3
#define FAT32_FILE_SYSTEM 32
#define FAT16_FILE_SYSTEM 16
#define DEFAULT_BYTE_VALUE 1024
#define MOUNT_UPDATE_DURATION  6000000
#define UNMOUNT_UPDATE_DURATION  2000000
#pragma pack(1)
/* RAM disk device path structure.
 * Will use Vendor Messaging Device Path.
 */
typedef struct _MOUNT_FS_DEVICE_PATH
{
    EFI_DEVICE_PATH Header;
    EFI_GUID        Guid;
    UINT8           DiskId[8];
    EFI_DEVICE_PATH EndDevicePath;
} MOUNT_FS_DEVICE_PATH;
#pragma pack()
typedef EFI_DEVICE_PATH EFI_DEVICE_PATH_PROTOCOL;

#pragma pack(1)

/* FAT32 boot sector definition */
typedef struct sBOOTSEC32
{
    UINT8  BS_jmpBoot[3];
    UINT8  BS_OEMName[8];
    UINT16 BPB_BytsPerSec;
    UINT8  BPB_SecPerClus;
    UINT16 BPB_RsvdSecCnt;
    UINT8  BPB_NumFATs;
    UINT16 BPB_RootEntCnt;
    UINT16 BPB_TotSec16;
    UINT8  BPB_Media;
    UINT16 BPB_FATSz16;
    UINT16 BPB_SecPerTrk;
    UINT16 BPB_NumHeads;
    UINT32 BPB_HiddSec;
    UINT32 BPB_TotSec32;  
    
    UINT32 BPB_FATSz32;
    UINT16 BPB_ExtFlags;
    UINT16 BPB_FSVer;
    UINT32 BPB_RootClus;
    UINT16 BPB_FSInfo;
    UINT16 BPB_BkBootSec;
    UINT8 BPB_Reserved[12]; 
    
    UINT8  BS_DrvNum;
    UINT8  BS_Reserved1;
    UINT8  BS_BootSig;
    UINT32 BS_VolID;
    UINT8  BS_VolLab[11];
    UINT8  BS_FilSysType[8]; //26
    UINT8  BS_Code[420]; 
    UINT16 BS_Sig;
} BOOTSEC32;

/* FAT16 boot sector definition */
typedef struct sBOOTSEC16
{
    UINT8  BS_jmpBoot[3];
    UINT8  BS_OEMName[8];
    UINT16 BPB_BytsPerSec;
    UINT8  BPB_SecPerClus;
    UINT16 BPB_RsvdSecCnt;
    UINT8  BPB_NumFATs;
    UINT16 BPB_RootEntCnt;
    UINT16 BPB_TotSec16;
    UINT8  BPB_Media;
    UINT16 BPB_FATSz16;
    UINT16 BPB_SecPerTrk;
    UINT16 BPB_NumHeads;
    UINT32 BPB_HiddSec;
    UINT32 BPB_TotSec32;
    UINT8  BS_DrvNum;
    UINT8  BS_Reserved1;
    UINT8  BS_BootSig;
    UINT32 BS_VolID;
    UINT8  BS_VolLab[11];
    UINT8  BS_FilSysType[8];
    UINT8  BS_Code[448];
    UINT16 BS_Sig;
} BOOTSEC16;
#pragma pack()

/* structure for total sectors to cluster size lookup */
typedef struct sFAT16TABLE
{
    UINTN size;
    UINT8  spc;
} FAT16TABLE;

/* structure for total sectors to cluster size lookup */
typedef struct sFAT32TABLE
{
    UINTN size;
    UINT8  spc;
} FAT32TABLE;
#define PBLOCK_DEVICE_SIGNATURE 'rdsk'
/* Mounted fs device info structure */
typedef struct _MOUNT_FS_DEVICE
{
    UINTN                   Signature;
    EFI_HANDLE              Handle;
    EFI_PHYSICAL_ADDRESS    Start;
    EFI_BLOCK_IO            BlkIo;
    EFI_BLOCK_IO_MEDIA      Media;
    EFI_DEVICE_PATH         *DevicePath;
} MOUNT_FS_DEVICE;

/* FSInfo Sector Structure */
typedef struct sFSINFO_SECTOR
{
    UINT32  FSI_LeadSig;
    UINT8   FSI_Reserved1[480];
    UINT32  FSI_StrucSig;
    UINT32  FSI_Free_Count;
    UINT32  FSI_Nxt_Free;
    UINT8   FSI_Reserved2[12];
    UINT32  FSI_TrailSig;
} FSINFO_SECTOR;

/* Macro finds the device info structure given a mounted fs BlkIo interface */
#define GET_MOUNT_FS_DEVICE_INTERFACE(a) CR(a,MOUNT_FS_DEVICE,BlkIo,PBLOCK_DEVICE_SIGNATURE)

EFI_STATUS EFIAPI ReadBlockIo(IN EFI_BLOCK_IO *This,IN UINT32 MediaId, IN EFI_LBA LBA,IN UINTN BufferSize,OUT VOID *Buffer);
EFI_STATUS EFIAPI WriteBlockIo(IN EFI_BLOCK_IO *This,IN UINT32 MediaId, IN EFI_LBA LBA,IN UINTN BufferSize,OUT VOID *Buffer);
EFI_STATUS EFIAPI FlushBlockIo(    IN EFI_BLOCK_IO *This);

EFI_STATUS Mount(IN EFI_HANDLE ImgHandle,IN UINTN FileSysType, IN UINT32 DiskSize,void* Buffer,CHAR16 *VolumeName,UINTN *HandleIndex);
EFI_STATUS Unmount( EFI_HANDLE ImageHandle ,EFI_HANDLE MsFshandle );
EFI_STATUS SetFsVolumeName(UINTN HandleIndex,CHAR16* VolName );
EFI_STATUS GetFsVolumeName(UINTN HandleIndex,CHAR16* VolName );
EFI_STATUS GetMountFsHandlle(CHAR16 *VolName,UINT64 *MountFsID,UINTN *HandleIndex);

UINT8 size2spc16(UINT32) ;
UINT8 size2spc32(UINT32) ;
VOID FormatMountFs(    IN VOID*  pStart,    IN UINT32 Size,    IN UINTN FileSysType);
EFI_STATUS UnloadMountFsDriver ( EFI_HANDLE ) ;

#ifndef EFI_FILE_SYSTEM_VOLUME_LABEL_ID
#define EFI_FILE_SYSTEM_VOLUME_LABEL_ID \
  { \
    0xDB47D7D3, 0xFE81, 0x11d3, {0x9A, 0x35, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } \
  }
#endif
#endif
