#ifndef _UEFI_2_0_DEVICE_PATH_H
#define _UEFI_2_0_DEVICE_PATH_H
#if defined(_LNX) || defined(_BSD)
#include "os_hook.h"
#endif
typedef struct
{
  UINT32      DeviceType;
  UINT16      Length;
  //UINT16    Order[];
}LEGACY_ORDER_ENTRY;

UINT8 GetDevicePathType( UINT8 *DevicePathInfo );
UINT8 GetDevicePathSubType( UINT8 *DevicePathInfo );
UINT16 GetBBSDevicePathDeviceType( UINT8 *DevicePathInfo );
UINT32 GetBBSDeviceCount();
UINT8 GetBBSDeviceTypeByIndex( UINT32 Index );
wchar_t *GetDevicePathTypeString( UINT32 Type );
#endif /* #ifndef _UEFI_2_0_DEVICE_PATH_H */
