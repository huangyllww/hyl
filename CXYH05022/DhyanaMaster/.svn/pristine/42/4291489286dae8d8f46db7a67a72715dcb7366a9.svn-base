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



#include <Library/BaseMemoryLib.h>
#include <Library/NetLib.h>
#include "UefiRestNetworkDriver.h"

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
  )
{
  *((BOOLEAN *) Context) = TRUE;
}

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
  )
{
  UINTN                 Index;
  
  if (HeaderCount == 0 || Headers == NULL || FieldName == NULL) {
    return NULL;
  }

  for (Index = 0; Index < HeaderCount; Index++){
    //
    // Field names are case-insensitive (RFC 2616).
    //
    if (AsciiStriCmp (Headers[Index].FieldName, FieldName) == 0) {
      return &Headers[Index];
    }
  }
  return NULL;
}

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
  )
{
  EFI_STATUS                Status;
  EFI_HTTP_CONFIG_DATA      HttpConfigData;
  EFI_HTTPv4_ACCESS_POINT   Http4AccessPoint;
  EFI_HTTPv6_ACCESS_POINT   Http6AccessPoint;
  EFI_HTTP_PROTOCOL         *Http;
  EFI_EVENT                 Event;
  
  if (Image == NULL || Controller == NULL || ConfigData == NULL || HttpIo == NULL) {
	  return EFI_INVALID_PARAMETER;
  }

  if (IpVersion != IP_VERSION_4 && IpVersion != IP_VERSION_6) {
	  return EFI_UNSUPPORTED;
  }

  ZeroMem (HttpIo, sizeof (HTTP_IO));
  
  //
  // Create the HTTP child instance and get the HTTP protocol.
  //  
  Status = NetLibCreateServiceChild (
             Controller,
             Image,
             &gEfiHttpServiceBindingProtocolGuid,
             &HttpIo->Handle
             );
  if (EFI_ERROR (Status)) {
	  return Status;
  }

  Status = gBS->OpenProtocol (
                  HttpIo->Handle,
                  &gEfiHttpProtocolGuid,
                  (VOID **) &Http,
                  Image,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) || (Http == NULL)) {
    goto ON_ERROR;
  }

  //
  // Init the configuration data and configure the HTTP child.
  //
  HttpIo->Image       = Image;
  HttpIo->Controller  = Controller;
  HttpIo->IpVersion   = IpVersion;
  HttpIo->Http        = Http;

  ZeroMem (&HttpConfigData, sizeof (EFI_HTTP_CONFIG_DATA));
  HttpConfigData.HttpVersion        = HttpVersion11;
  HttpConfigData.TimeOutMillisec    = ConfigData->Config4.RequestTimeOut;
  if (HttpIo->IpVersion == IP_VERSION_4) {
    HttpConfigData.LocalAddressIsIPv6 = FALSE;
    
    Http4AccessPoint.UseDefaultAddress = ConfigData->Config4.UseDefaultAddress;
    Http4AccessPoint.LocalPort         = ConfigData->Config4.LocalPort;
    IP4_COPY_ADDRESS (&Http4AccessPoint.LocalAddress, &ConfigData->Config4.LocalIp);
    IP4_COPY_ADDRESS (&Http4AccessPoint.LocalSubnet, &ConfigData->Config4.SubnetMask);
    HttpConfigData.AccessPoint.IPv4Node = &Http4AccessPoint;   
  } else {
    HttpConfigData.LocalAddressIsIPv6 = TRUE;
    Http6AccessPoint.LocalPort        = ConfigData->Config6.LocalPort;
    IP6_COPY_ADDRESS (&Http6AccessPoint.LocalAddress, &ConfigData->Config6.LocalIp);
    HttpConfigData.AccessPoint.IPv6Node = &Http6AccessPoint;
  }
  
  Status = Http->Configure (Http, &HttpConfigData);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  //
  // Create events for variuos asynchronous operations.
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RestHttpCommonNotify,
                  &HttpIo->IsTxDone,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  HttpIo->ReqToken.Event = Event;
  HttpIo->ReqToken.Message = &HttpIo->ReqMessage;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RestHttpCommonNotify,
                  &HttpIo->IsRxDone,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  HttpIo->RspToken.Event = Event;
  HttpIo->RspToken.Message = &HttpIo->RspMessage;

  return EFI_SUCCESS;
  
ON_ERROR:
  HttpIoDestroyIo (HttpIo);

  return Status;
}

/**
  Destroy the HTTP_IO and release the resouces. 

  @param[in]  HttpIo          The HTTP_IO which wraps the HTTP service to be destroyed.

**/
VOID
HttpIoDestroyIo (
  IN HTTP_IO                *HttpIo
  )
{
  EFI_HTTP_PROTOCOL         *Http;
  EFI_EVENT                 Event;

  if (HttpIo == NULL) {
    return;
  }

  Event = HttpIo->ReqToken.Event;
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Event = HttpIo->RspToken.Event;
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  
  Http = HttpIo->Http;
  if (Http != NULL) {
    Http->Configure (Http, NULL);
    gBS->CloseProtocol (
           HttpIo->Handle,
           &gEfiHttpProtocolGuid,
           HttpIo->Image,
           HttpIo->Controller
           );
  }

  NetLibDestroyServiceChild (
    HttpIo->Controller,
    HttpIo->Image,
    &gEfiHttpServiceBindingProtocolGuid,
    HttpIo->Handle
    );
}

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
  IN  EFI_HTTP_REQUEST_DATA  *Request,
  IN  UINTN                  HeaderCount,
  IN  EFI_HTTP_HEADER        *Headers,
  IN  UINTN                  BodyLength,
  IN  VOID                   *Body
  )
{
  EFI_STATUS                 Status;
  EFI_HTTP_PROTOCOL          *Http;

  if (HttpIo == NULL || HttpIo->Http == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HttpIo->ReqToken.Status  = EFI_NOT_READY;
  HttpIo->ReqToken.Message->Data.Request = Request;
  HttpIo->ReqToken.Message->HeaderCount  = HeaderCount;
  HttpIo->ReqToken.Message->Headers      = Headers;
  HttpIo->ReqToken.Message->BodyLength   = BodyLength;
  HttpIo->ReqToken.Message->Body         = Body;

  //
  // Queue the request token to HTTP instances.
  //
  Http = HttpIo->Http;
  HttpIo->IsTxDone = FALSE;
  Status = Http->Request (
                   Http,
                   &HttpIo->ReqToken
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Poll the network until transmit finish.
  //
  while (!HttpIo->IsTxDone) {
    Http->Poll (Http);
  }

  return HttpIo->ReqToken.Status;
}

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
  )
{
  EFI_STATUS                 Status;
  EFI_HTTP_PROTOCOL          *Http;

  if (HttpIo == NULL || HttpIo->Http == NULL || ResponseData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Queue the response token to HTTP instances.
  //
  HttpIo->RspToken.Status  = EFI_NOT_READY;
  if (RecvMsgHeader) {
    HttpIo->RspToken.Message->Data.Response = &ResponseData->Response;
  } else {
    HttpIo->RspToken.Message->Data.Response = NULL;
  }
  HttpIo->RspToken.Message->HeaderCount   = 0;
  HttpIo->RspToken.Message->Headers       = NULL;
  HttpIo->RspToken.Message->BodyLength    = ResponseData->BodyLength;
  HttpIo->RspToken.Message->Body          = ResponseData->Body;

  Http = HttpIo->Http;
  HttpIo->IsRxDone = FALSE;
  DEBUG((DEBUG_INFO, "HttpIoRecvResponse: Http->Response()\n"));
  Status = Http->Response (
                   Http,
                   &HttpIo->RspToken
                   );
  DEBUG((DEBUG_INFO, "HttpIoRecvResponse: Http->Response() done\n"));
  if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "HttpIoRecvResponse: (Http->Response())Exit Status = %r\n", Status));
    return Status;
  }

  //
  // Poll the network until receive finish.
  //
  while (!HttpIo->IsRxDone) {
    Http->Poll (Http);
  }

  //
  // Store the received data into the wrapper.
  //
  ResponseData->Status = HttpIo->RspToken.Status;
  ResponseData->HeaderCount = HttpIo->RspToken.Message->HeaderCount;
  ResponseData->Headers     = HttpIo->RspToken.Message->Headers;
  ResponseData->BodyLength  = HttpIo->RspToken.Message->BodyLength;
  DEBUG((DEBUG_INFO, "HttpIoRecvResponse: Return Status = %r, ResponseData->BodyLength: %d\n", Status, ResponseData->BodyLength));
  return Status;
}

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

