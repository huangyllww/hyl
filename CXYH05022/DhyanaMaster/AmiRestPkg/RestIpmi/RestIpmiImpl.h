//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file RestIpmiImpl.h
    This file contains definitions and includes for Rest over Ipmi driver.

**/

#ifndef __DXE_REST_IPMI_USB_IMPL_H__
#define __DXE_REST_IPMI_USB_IMPL_H__

//----------------------------------------------------------------------

#include "Token.h"
#include <Protocol/Rest.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/IpmiUsbTransportProtocol.h>
#include <Library/TimerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include "RestIpmiCommon.h"

//----------------------------------------------------------------------

//
// Function Prototypes
//

EFI_STATUS GetMaxRestIpmiPacketSize(
		IN IPMI_USB_TRANSPORT *IpmiUsb, 
		OUT UINT32 *MaxSize);

EFI_STATUS
SendRecieveRestIpmiUsbCommand (
  IN  EFI_REST_PROTOCOL        *This,
  IN EFI_HTTP_MESSAGE *RequestMessage,
  OUT EFI_HTTP_MESSAGE *ResponseMessage);


EFI_STATUS
GetServiceTimeRestIpmiUsbCommand (
  IN  EFI_REST_PROTOCOL        *This,
  OUT EFI_TIME *Time);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
