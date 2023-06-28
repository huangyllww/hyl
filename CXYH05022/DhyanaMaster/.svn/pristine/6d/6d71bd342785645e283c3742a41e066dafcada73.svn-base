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

/** @file RestIpmiProto.h
    This file contains definitions Rest over Ipmi driver request and response routines.

**/

#ifndef __DXE_REST_IPMI_USB_PROTO_H__
#define __DXE_REST_IPMI_USB_PROTO_H__


EFI_STATUS
RestIpmiUsbRequest (
     IN EFI_HTTP_MESSAGE              *HttpRequest,
     IN  UINT32						  ParamsPaddingSize,
     OUT CHAR8                        **InRequestStr,
     OUT UINT32                       *RequestSize
  );

EFI_STATUS
RestIpmiUsbResponse (
	     IN CHAR8              *InResponseStr,
	     IN UINT32		ResponseStrLen,
	     OUT EFI_HTTP_MESSAGE   *HttpResponse
	  );


#endif

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
