#include "DevicePath.h"
#define _EFI 1
//
// BBS Device Path
//
#define BBS_DEVICE_PATH           0x05
#define BBS_BBS_DP                0x01
#ifndef _EFI
typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT16                          DeviceType;
  UINT16                          StatusFlag;
  CHAR8                           String[1];
} BBS_BBS_DEVICE_PATH;
#endif

