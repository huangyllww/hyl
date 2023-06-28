/** @file
  The header files of implementation of EFI_REST_PROTOCOL protocol interfaces.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __UEFI_REST_PROTO_H__
#define __UEFI_REST_PROTO_H__

/**
  Sends an HTTP request to this REST service, and returns a response when the data 
  is retrieved from the service. RequestMessage contains the HTTP request to the 
  REST resource identified by RequestMessage.Request.Url. 
  
  The ResponseMessage is the returned HTTP response for that request, including any 
  HTTP status.

  @param[in]  This                Pointer to EFI_REST_PROTOCOL instance.
  @param[in]  RequestMessage	  Pointer to the HTTP request data for this resource.
  @param[out] ResponseMessage	  Pointer to the HTTP response data obtained for this request.

  @retval EFI_SUCCESS             Operation succeeded.
  @retval EFI_INVALID_PARAMETER   This, RequestMessage, or ResponseMessage are NULL.
  @retval EFI_DEVICE_ERROR        An unexpected system or network error occurred.
**/
EFI_STATUS
EFIAPI
EfiRestSendReceive (
  IN  EFI_REST_PROTOCOL         *This,
  IN  EFI_HTTP_MESSAGE *RequestMessage,
  OUT EFI_HTTP_MESSAGE *ResponseMessage
  );

/**
  optional interface to obtain the current time from this
  REST service instance. If this REST service does not support retrieving the time, 
  this function returns EFI_UNSUPPORTED.

  @param[in]  This                Pointer to EFI_REST_PROTOCOL instance.
  @param[out]  Time      Pointer to storage to receive a snapshot of the current time of the REST service.

  @retval EFI_SUCCESS             Operation succeeded.
  @retval EFI_INVALID_PARAMETER   This or Time are NULL.
  @retval EFI_DEVICE_ERROR        An unexpected system or network error occurred.
  @retval EFI_UNSUPPORTED         The RESTful service does not support returning the time.
**/
EFI_STATUS
EFIAPI
EfiRestGetServiceTime (
  IN  EFI_REST_PROTOCOL         *This,
  IN  EFI_TIME      *Time
  );

#endif
