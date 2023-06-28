//#include "wchar.h"
//#include "memory.h"
//#include "stdlib.h"
//#include "types.h"
#define _EFI 1

#include "Efi.h"

#ifdef _EFI
#include "os_hook.h"
#define EFI_GUID SCE_GUID
typedef GUID EFI_GUID;  // why is this needed???
#endif

#include "EfiCommon.h"
//Added for EDK
#include "os_hook.h"
#include "EDK_header.h"
/*#ifdef _LNX
#include "EDK_header.h"
#include "os_hook.h"
#endif*/

#include EFI_PROTOCOL_DEFINITION(DevicePath)
//Commented to make it work for EDK
/*#ifdef WIN32
#include "EfiDevicePath.h"
#endif*/

#include "UEFI20DevicePath.h"

UINT8 gBBSDeviceTypes[]=
{
  BBS_TYPE_FLOPPY, BBS_TYPE_HARDDRIVE,
  BBS_TYPE_CDROM, BBS_TYPE_PCMCIA,
  BBS_TYPE_USB, BBS_TYPE_EMBEDDED_NETWORK,
  BBS_TYPE_BEV, BBS_TYPE_UNKNOWN
};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataOffset
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 GetDevicePathType( UINT8 *DevicePathInfo )
{
  EFI_DEVICE_PATH_PROTOCOL *DevicePath = ( EFI_DEVICE_PATH_PROTOCOL * )DevicePathInfo;

  return DevicePath->Type;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataWidth
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 GetDevicePathSubType( UINT8 *DevicePathInfo )
{
  EFI_DEVICE_PATH_PROTOCOL *DevicePath = ( EFI_DEVICE_PATH_PROTOCOL * )DevicePathInfo;

  return DevicePath->SubType;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataOffset
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT16 GetBBSDevicePathDeviceType( UINT8 *DevicePathInfo )
{
  BBS_BBS_DEVICE_PATH *BBSDevicePath = ( BBS_BBS_DEVICE_PATH * )DevicePathInfo;

  return BBSDevicePath->DeviceType;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataOffset
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT32 GetBBSDeviceCount()
{
  return sizeof( gBBSDeviceTypes );
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:	_GetControlDataOffset
//
// Description:
//
// Input:	UINT8 *HiiData
//
// Output:	UINT16
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 GetBBSDeviceTypeByIndex( UINT32 Index )
{
  if (Index >= GetBBSDeviceCount())
    Index = GetBBSDeviceCount() - 1;
  return gBBSDeviceTypes[Index];
}

wchar_t *GetDevicePathTypeString( UINT32 Type )
{
  wchar_t *BBSDeviceTypesString[]=
  {
    L"BBS_TYPE_FLOPPY", L"BBS_TYPE_HARDDRIVE",
    L"BBS_TYPE_CDROM", L"BBS_TYPE_PCMCIA",
    L"BBS_TYPE_USB", L"BBS_TYPE_EMBEDDED_NETWORK",
    L"BBS_TYPE_BEV", L"BBS_TYPE_UNKNOWN"
  };

  // Return BBSDeviceTypesString[Type - 1]. Array Index based of 0
  if (Type >= GetBBSDeviceCount())
    Type = GetBBSDeviceCount();
  return BBSDeviceTypesString[Type - 1];
}
