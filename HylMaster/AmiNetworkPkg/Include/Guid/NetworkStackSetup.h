//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************

//**********************************************************************
/** @file NetworkStackSetup.h
    NetworkStackSetup definitions

**/
//**********************************************************************
#ifndef __NETWORK_STACK_SETUP_H__
#define __NETWORK_STACK_SETUP_H__

//<Ouyyx-20190831 To Support UEFI_PXERetryBoot +>
#include <Token.h>
//<Ouyyx-20190831 To Support UEFI_PXERetryBoot ->

#define NETWORK_STACK_GUID \
  { 0xD1405D16, 0x7AFC, 0x4695, 0xBB, 0x12, 0x41, 0x45, 0x9D, 0x36, 0x95, 0xA2 }

#pragma pack(1)
typedef struct{
  UINT8             Enable;
  UINT8             Ipv4Pxe;
  UINT8             Ipv6Pxe;
  UINT8             IpsecCertificate;
  UINT8             PxeBootWaitTime;
  UINT8             MediaDetectCount;
  UINT8             Ipv4Http;
  UINT8             Ipv6Http;
  UINT8				Ip6ConfigPolicy;
//<Ouyyx-20190831 To Support UEFI_PXERetryBoot +>
#if defined(UEFI_PXERetryBoot_SUPPORT) && (UEFI_PXERetryBoot_SUPPORT == 1) 
    UINT8             F12_Hot_Key;         //<Chendl001-20180124 Follow Tencent Requirement Add UEFI PXE Retry Funtion>  
    UINT8             UefiPxeRetryCount;   //USI-Paul0025-2017-08-16 To support Uefi PXE Retry boot Count.+>
#endif
//<Ouyyx-20190831 To Support UEFI_PXERetryBoot ->   
} NETWORK_STACK;
#pragma pack()

#define MIN_MEDIA_DETECT_COUNT       1
#define MAX_MEDIA_DETECT_COUNT      50
#define DEFAULT_MEDIA_DETECT_COUNT  MIN_MEDIA_DETECT_COUNT  // Should be within MIN_MEDIA_DETECT_COUNT and MAX_MEDIA_DETECT_COUNT range

extern EFI_GUID gEfiNetworkStackSetupGuid;

#endif
//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
