#ifndef _OEM_STORAGE_INFO_PROTOCOL_H_
#define _OEM_STORAGE_INFO_PROTOCOL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <OemIpmiNetFnDefinitions.h>
#include <IndustryStandard/Pci30.h>
#include <Protocol\PciIo.h>

// {E3CEE688-2800-4d41-9B4A-AB95FD613EE0}
#define EFI_OEM_STORAGE_INFO_PROTOCOL_GUID \
  { \
    0xe3cee688, 0x2800, 0x4d41, { 0x9b, 0x4a, 0xab, 0x95, 0xfd, 0x61, 0x3e, 0xe0 } \
  }

typedef struct _OEM_STORAGE_INFO_PROTOCOL  OEM_STORAGE_INFO_PROTOCOL;

#pragma pack(push, 1)
typedef struct {
  EFI_PCI_IO_PROTOCOL *PciIO;
  UINT8         SegNum;
  UINT8         BusNum;
  UINT8         DevNum;
  UINT8         FunNum;
  #if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18 == 1)
  //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
  UINT8         RootBus;
  UINT8         RootDevice;
  UINT8         RootFunction;
  UINT8         HDDControllerIndex;
  //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  ->
  #endif
  PCI_DEVICE_INDEPENDENT_REGION    PciHeader;
  UINT8         HddSlot;
  UINT8         HddMode;
  CHAR8         HddSN[20];
  CHAR8         HddFirmwareVersion[8];
  CHAR8         HddModelName[40];
  UINT64        HddSize;
}DRIVE_INFO;

typedef struct {
  UINTN       InitCounter;
  UINTN       ActualNumber;
  DRIVE_INFO  **DriveInfo;
}OEM_STORAGE_INFO;
#pragma pack(pop)


typedef EFI_STATUS (*EFI_GET_OEM_STORAGE_INFO)(
  IN      OEM_STORAGE_INFO_PROTOCOL   *This,
  IN OUT  OEM_STORAGE_INFO  **OemStorageInfo
);

struct _OEM_STORAGE_INFO_PROTOCOL {
  EFI_GET_OEM_STORAGE_INFO    GetOemStorageInfo;
};

extern EFI_GUID gOemStorageInfoProtocolGuid;
#ifdef __cplusplus
}
#endif
#endif
