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


/** @file
  Support functions declaration for UEFI REST over HTTP driver.

Copyright (c) 2015 - 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials are licensed and made available under 
the terms and conditions of the BSD License that accompanies this distribution.  
The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php.                                          
    
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __UEFI_REST_HTTP_SUPPORT_H__
#define __UEFI_REST_HTTP_SUPPORT_H__

#include    "Protocol/Http.h"

//
// A wrapper structure to hold the HTTP headers.
//
typedef struct {
  UINTN                       MaxHeaderCount;
  UINTN                       HeaderCount;
  EFI_HTTP_HEADER             *Headers;
} HTTP_IO_HEADER;



/**
  Find a specified header field according to the field name.

  @param[in]   HeaderCount      Number of HTTP header structures in Headers list. 
  @param[in]   Headers          Array containing list of HTTP headers.
  @param[in]   FieldName        Null terminated string which describes a field name. 

  @return    Pointer to the found header or NULL.

**/
EFI_HTTP_HEADER *
RestHttpFindHeader (
  IN  UINTN                HeaderCount,
  IN  EFI_HTTP_HEADER      *Headers,
  IN  CHAR8                *FieldName
  );


//
// HTTP_IO configuration data for IPv4
//
typedef struct {
  EFI_HTTP_VERSION          HttpVersion;
  UINT32                    RequestTimeOut;  // In milliseconds.
  BOOLEAN                   UseDefaultAddress;
  EFI_IPv4_ADDRESS          LocalIp;
  EFI_IPv4_ADDRESS          SubnetMask;
  UINT16                    LocalPort;
} HTTP4_IO_CONFIG_DATA;

//
// HTTP_IO configuration data for IPv6
//
typedef struct {
  EFI_HTTP_VERSION          HttpVersion;
  UINT32                    RequestTimeOut;  // In milliseconds.
  BOOLEAN                   UseDefaultAddress;
  EFI_IPv6_ADDRESS          LocalIp;
  UINT16                    LocalPort;
} HTTP6_IO_CONFIG_DATA;


//
// HTTP_IO configuration
//
typedef union {
  HTTP4_IO_CONFIG_DATA       Config4;
  HTTP6_IO_CONFIG_DATA       Config6;
} HTTP_IO_CONFIG_DATA;

//
// HTTP_IO wrapper of the EFI HTTP service.
//
typedef struct {
  UINT8                     IpVersion;
  EFI_HANDLE                Image;
  EFI_HANDLE                Controller;
  EFI_HANDLE                Handle;
  
  EFI_HTTP_PROTOCOL         *Http;

  EFI_HTTP_TOKEN            ReqToken;
  EFI_HTTP_MESSAGE          ReqMessage;
  EFI_HTTP_TOKEN            RspToken;
  EFI_HTTP_MESSAGE          RspMessage;

  BOOLEAN                   IsTxDone;
  BOOLEAN                   IsRxDone;
} HTTP_IO;

//
// A wrapper structure to hold the received HTTP response data.
//
typedef struct {
  EFI_HTTP_RESPONSE_DATA      Response;
  UINTN                       HeaderCount;
  EFI_HTTP_HEADER             *Headers;
  UINTN                       BodyLength;
  CHAR8                       *Body;
  EFI_STATUS                  Status;
} HTTP_IO_RESPONSE_DATA;

/**
  Notify the callback function when an event is triggered.

  @param[in]  Event           The triggered event.
  @param[in]  Context         The opaque parameter to the function.

**/
VOID
EFIAPI
RestHttpCommonNotify (
  IN EFI_EVENT           Event,
  IN VOID                *Context
  );

/**
  Create a HTTP_IO to access the HTTP service. It will create and configure
  a HTTP child handle.

  @param[in]  Image          The handle of the driver image.
  @param[in]  Controller     The handle of the controller.
  @param[in]  IpVersion      IP_VERSION_4 or IP_VERSION_6.
  @param[in]  ConfigData     The HTTP_IO configuration data.
  @param[out] HttpIo         The HTTP_IO.
  
  @retval EFI_SUCCESS            The HTTP_IO is created and configured.
  @retval EFI_INVALID_PARAMETER  One or more parameters are invalid.
  @retval EFI_UNSUPPORTED        One or more of the control options are not
                                 supported in the implementation.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate memory.
  @retval Others                 Failed to create the HTTP_IO or configure it.

**/
EFI_STATUS
HttpIoCreateIo (
  IN EFI_HANDLE             Image,
  IN EFI_HANDLE             Controller,
  IN UINT8                  IpVersion,
  IN HTTP_IO_CONFIG_DATA    *ConfigData,
  OUT HTTP_IO               *HttpIo
  );

/**
  Destroy the HTTP_IO and release the resouces. 

  @param[in]  HttpIo          The HTTP_IO which wraps the HTTP service to be destroyed.

**/
VOID
HttpIoDestroyIo (
  IN HTTP_IO                *HttpIo
  );

/**
  Synchronously send a HTTP REQUEST message to the server.
  
  @param[in]   HttpIo           The HttpIo wrapping the HTTP service.
  @param[in]   Request          A pointer to storage such data as URL and HTTP method.
  @param[in]   HeaderCount      Number of HTTP header structures in Headers list. 
  @param[in]   Headers          Array containing list of HTTP headers.
  @param[in]   BodyLength       Length in bytes of the HTTP body.
  @param[in]   Body             Body associated with the HTTP request. 
  
  @retval EFI_SUCCESS            The HTTP request is trasmitted.
  @retval EFI_INVALID_PARAMETER  One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate memory.
  @retval EFI_DEVICE_ERROR       An unexpected network or system error occurred.
  @retval Others                 Other errors as indicated.

**/
EFI_STATUS
HttpIoSendRequest (
  IN  HTTP_IO                *HttpIo,
  IN  EFI_HTTP_REQUEST_DATA  *Request,      OPTIONAL
  IN  UINTN                  HeaderCount,
  IN  EFI_HTTP_HEADER        *Headers,      OPTIONAL
  IN  UINTN                  BodyLength,
  IN  VOID                   *Body          OPTIONAL
  );

/**
  Synchronously receive a HTTP RESPONSE message from the server.
  
  @param[in]   HttpIo           The HttpIo wrapping the HTTP service.
  @param[in]   RecvMsgHeader    TRUE to receive a new HTTP response (from message header).
                                FALSE to continue receive the previous response message.
  @param[out]  ResponseData     Point to a wrapper of the received response data.
  
  @retval EFI_SUCCESS            The HTTP response is received.
  @retval EFI_INVALID_PARAMETER  One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate memory.
  @retval EFI_DEVICE_ERROR       An unexpected network or system error occurred.
  @retval Others                 Other errors as indicated.

**/
EFI_STATUS
HttpIoRecvResponse (
  IN      HTTP_IO                  *HttpIo,
  IN      BOOLEAN                  RecvMsgHeader,
     OUT  HTTP_IO_RESPONSE_DATA    *ResponseData
  );

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
