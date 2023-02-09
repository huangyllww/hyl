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

/** @file RestIpmiCommon.h
    REST IPMI Common definitions.

**/

#ifndef __REST_IPMI_USB_COMMON_H__
#define __REST_IPMI_USB_COMMON_H__

//
// REST IPMI Instance signature
//

#define REST_IPMI_USB_SIGNATURE              SIGNATURE_32 ('r', 's', 'i', 'u')

//
// CONTAINING_RECORD - returns a pointer to the structure
//      from one of it's elements.
// REST_IPMI_USB_CR - Returns pointer to Ipmi Usb Instance data structure
//
#define REST_IPMI_USB_CR( Record, TYPE, Field ) \
    ( (TYPE *) ( (CHAR8 *) (Record) - (CHAR8 *) &( ( (TYPE *) 0 )->Field ) ) )

#define INSTANCE_DATA_FROM_REST_IPMI_USB_TRANSPORT_THIS(a) \
    REST_IPMI_USB_CR( a, REST_IPMI_USB_INSTANCE_DATA, RestIpmiUsbTransport )


//
// REST IPMI USB Bmc Instance private data.
//
typedef struct {
  UINTN                     	Signature;
  EFI_REST_PROTOCOL	   			RestIpmiUsbTransport;
  EFI_HANDLE                	IpmiUsbHandle;
  VOID                      	*IpmiUsb;
  UINT32                        MaxRestIpmiPacketSize;
} REST_IPMI_USB_INSTANCE_DATA;

#pragma pack()

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
