 //**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file RestIpmiProto.c
 This file contains Rest over Ipmi driver request and response routines.

 **/

#include "RestIpmiDriver.h"
#include "Http11.h"

/**
 Translate the status code in HTTP message to EFI_HTTP_STATUS_CODE defined 
 in UEFI 2.5 specification.

 @param[in]  StatusCode         The status code value in HTTP message.

 @return                        Value defined in EFI_HTTP_STATUS_CODE .

 **/
EFI_HTTP_STATUS_CODE HttpMappingToStatusCode( IN UINTN StatusCode) {
	switch (StatusCode) {
	case 100:
		return HTTP_STATUS_100_CONTINUE;
	case 101:
		return HTTP_STATUS_101_SWITCHING_PROTOCOLS;
	case 200:
		return HTTP_STATUS_200_OK;
	case 201:
		return HTTP_STATUS_201_CREATED;
	case 202:
		return HTTP_STATUS_202_ACCEPTED;
	case 203:
		return HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION;
	case 204:
		return HTTP_STATUS_204_NO_CONTENT;
	case 205:
		return HTTP_STATUS_205_RESET_CONTENT;
	case 206:
		return HTTP_STATUS_206_PARTIAL_CONTENT;
	case 300:
		return HTTP_STATUS_300_MULTIPLE_CHIOCES;
	case 301:
		return HTTP_STATUS_301_MOVED_PERMANENTLY;
	case 302:
		return HTTP_STATUS_302_FOUND;
	case 303:
		return HTTP_STATUS_303_SEE_OTHER;
	case 304:
		return HTTP_STATUS_304_NOT_MODIFIED;
	case 305:
		return HTTP_STATUS_305_USE_PROXY;
	case 307:
		return HTTP_STATUS_307_TEMPORARY_REDIRECT;
	case 400:
		return HTTP_STATUS_400_BAD_REQUEST;
	case 401:
		return HTTP_STATUS_401_UNAUTHORIZED;
	case 402:
		return HTTP_STATUS_402_PAYMENT_REQUIRED;
	case 403:
		return HTTP_STATUS_403_FORBIDDEN;
	case 404:
		return HTTP_STATUS_404_NOT_FOUND;
	case 405:
		return HTTP_STATUS_405_METHOD_NOT_ALLOWED;
	case 406:
		return HTTP_STATUS_406_NOT_ACCEPTABLE;
	case 407:
		return HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED;
	case 408:
		return HTTP_STATUS_408_REQUEST_TIME_OUT;
	case 409:
		return HTTP_STATUS_409_CONFLICT;
	case 410:
		return HTTP_STATUS_410_GONE;
	case 411:
		return HTTP_STATUS_411_LENGTH_REQUIRED;
	case 412:
		return HTTP_STATUS_412_PRECONDITION_FAILED;
	case 413:
		return HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE;
	case 414:
		return HTTP_STATUS_414_REQUEST_URI_TOO_LARGE;
	case 415:
		return HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE;
	case 416:
		return HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIED;
	case 417:
		return HTTP_STATUS_417_EXPECTATION_FAILED;
	case 500:
		return HTTP_STATUS_500_INTERNAL_SERVER_ERROR;
	case 501:
		return HTTP_STATUS_501_NOT_IMPLEMENTED;
	case 502:
		return HTTP_STATUS_502_BAD_GATEWAY;
	case 503:
		return HTTP_STATUS_503_SERVICE_UNAVAILABLE;
	case 504:
		return HTTP_STATUS_504_GATEWAY_TIME_OUT;
	case 505:
		return HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED;

	default:
		return HTTP_STATUS_UNSUPPORTED_STATUS;
	}
}

UINTN HttpMethodLength(EFI_HTTP_METHOD Method) {
	UINTN MethodLen = 0;
	switch (Method) {
	case HttpMethodGet:
		MethodLen = sizeof(HTTP_METHOD_GET);
		break;
	case HttpMethodHead:
		MethodLen = sizeof(HTTP_METHOD_HEAD);
		break;
	case HttpMethodPost:
		MethodLen = sizeof(HTTP_METHOD_POST);
		break;
	case HttpMethodPut:
		MethodLen = sizeof(HTTP_METHOD_PUT);
		break;
	case HttpMethodPatch:
		MethodLen = sizeof(HTTP_METHOD_PATCH);
		break;
	case HttpMethodDelete:
		MethodLen = sizeof(HTTP_METHOD_DELETE);
		break;
	default:
		ASSERT(FALSE);
	}
	return MethodLen;
}

/**
 Generate HTTP request string.

 @param[in]  HttpInstance       Pointer to HTTP_PROTOCOL structure.
 @param[in]  Message            Pointer to storage containing HTTP message data.
 @param[in]  Url                The URL of a remote host.

 @return     Pointer to the created HTTP request string.
 @return     NULL if any error occured.

 **/
CHAR8 *
HttpGenRequestString( IN EFI_HTTP_MESSAGE *Message, IN UINT32 ParamsPaddingSize,
		IN CHAR8 *Url, OUT UINT32 *RequestSize) {
	EFI_STATUS Status;
	UINTN StrLength;
	UINT8 *Request;
	UINT8 *RequestPtr;
	UINTN HttpHdrSize = 0;
	UINTN MsgSize, MethodLen;
	BOOLEAN Success;
	VOID *HttpHdr = NULL;
	EFI_HTTP_HEADER **AppendList;
	UINTN Index;

	ASSERT(Message != NULL);

	Request = NULL;
	Success = FALSE;
	HttpHdr = NULL;
	AppendList = NULL;

	//
	// Check whether the EFI_HTTP_UTILITIES_PROTOCOL is available.
	//
	if (gHttpUtilities == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "\nREST: gHttpUtilities Error\n"));
		return NULL;
	}

	if(Message->HeaderCount != 0){
		//
		// Build AppendList
		//
		AppendList = AllocateZeroPool(
				sizeof(EFI_HTTP_HEADER *) * (Message->HeaderCount));
		if (AppendList == NULL) {
			return NULL;
		}
	
		for (Index = 0; Index < Message->HeaderCount; Index++) {
			AppendList[Index] = &Message->Headers[Index];
		}
	
	
		//
		// Build raw unformatted HTTP headers.
		//
		Status = gHttpUtilities->Build(gHttpUtilities, 0, NULL, 0, NULL,
				Message->HeaderCount, AppendList, &HttpHdrSize,
				&HttpHdr);
		FreePool(AppendList);
		if (EFI_ERROR (Status) || HttpHdr == NULL || HttpHdrSize == 0) {
			return NULL;
		}
		if(*(((UINT8*)HttpHdr) + HttpHdrSize - 1) == 0){
			HttpHdrSize--;
		}
	}
	else{
		if(Message->Body != NULL){
			HttpHdr = AllocatePool(AsciiStrLen(HTTP_CRLF_STR));
			if (HttpHdr == NULL) {
				return NULL;
			}
			HttpHdrSize = AsciiStrLen(HTTP_CRLF_STR);
			AsciiStrnCpy(HttpHdr, HTTP_CRLF_STR, HttpHdrSize);
		}
	}
	
	//
	// Calculate HTTP message length.
	//
	MethodLen = HttpMethodLength(Message->Data.Request->Method);
	MsgSize = ParamsPaddingSize + MethodLen + AsciiStrLen(Url)
			+ AsciiStrLen(HTTP_VERSION_CRLF_STR) + HttpHdrSize
			+ Message->BodyLength;

	Request = AllocateZeroPool(MsgSize + 1);
	if (Request == NULL) {
		goto Exit;
	}

	RequestPtr = Request + ParamsPaddingSize;
	//
	// Construct header request
	//
	MethodLen--;
	switch (Message->Data.Request->Method) {
	case HttpMethodGet:
		CopyMem(RequestPtr, HTTP_METHOD_GET, MethodLen);
		RequestPtr += MethodLen;
		break;
	case HttpMethodHead:
		CopyMem(RequestPtr, HTTP_METHOD_HEAD, MethodLen);
		RequestPtr += MethodLen;
		break;
	case HttpMethodPut:
		CopyMem(RequestPtr, HTTP_METHOD_PUT, MethodLen);
		RequestPtr += MethodLen;
		break;
	case HttpMethodPost:
		CopyMem(RequestPtr, HTTP_METHOD_POST, MethodLen);
		RequestPtr += MethodLen;
		break;
	case HttpMethodDelete:
		CopyMem(RequestPtr, HTTP_METHOD_DELETE, MethodLen);
		RequestPtr += MethodLen;
		break;
	case HttpMethodPatch:
		CopyMem(RequestPtr, HTTP_METHOD_PATCH, MethodLen);
		RequestPtr += MethodLen;
		break;
	default:
		ASSERT(FALSE);
		goto Exit;
	}
	*RequestPtr = ' ';
	RequestPtr++;

	StrLength = AsciiStrLen(Url);
	CopyMem(RequestPtr, Url, StrLength);
	RequestPtr += StrLength;

	StrLength = sizeof(HTTP_VERSION_CRLF_STR) - 1;
	CopyMem(RequestPtr, HTTP_VERSION_CRLF_STR, StrLength);
	RequestPtr += StrLength;

	//
	// Construct header
	//
	
	if (HttpHdr != NULL) {
		CopyMem(RequestPtr, HttpHdr, HttpHdrSize);
		RequestPtr += HttpHdrSize;
	}
	//
	// Construct body
	//
	if (Message->Body != NULL) {
		CopyMem(RequestPtr, Message->Body, Message->BodyLength);
		RequestPtr += Message->BodyLength;
	}

	//
	// Done
	//
	*RequestPtr = 0;
	*RequestSize = (UINT32) MsgSize;
	Success = TRUE;

Exit:

	if (!Success) {
		if (Request != NULL) {
			FreePool(Request);
		}

		Request = NULL;
	}

	if (HttpHdr != NULL) {
		FreePool(HttpHdr);
	}

	return (CHAR8*) Request;
}

/**



 **/
EFI_STATUS RestIpmiUsbRequest( IN EFI_HTTP_MESSAGE *HttpRequest,
		IN UINT32 ParamsPaddingSize, OUT CHAR8 **InRequestStr,
		OUT UINT32 *RequestSize) {
	EFI_HTTP_REQUEST_DATA *Request;
	CHAR8 *RequestStr;
	CHAR8 *Url;
	UINTN UrlLen;
	CHAR8 *FileUrl;

//	if ((HttpRequest == NULL) || (HttpRequest->Headers == NULL)) {
	if (HttpRequest == NULL) {
		return EFI_INVALID_PARAMETER;
	}

	//
	// Current implementation supports POST/PUT method.
	// If future version supports these two methods, Request could be NULL for a special case that to send large amounts
	// of data. For this case, the implementation need check whether previous call to Request() has been completed or not.
	// 
	//
	Request = HttpRequest->Data.Request;
	if ((Request == NULL) || (Request->Url == NULL)) {
		return EFI_INVALID_PARAMETER;
	}

	//
	// Only support GET and HEAD method in current implementation.
	//
	if ((Request->Method != HttpMethodGet)
			&& (Request->Method != HttpMethodHead)
			&& (Request->Method != HttpMethodPut)
			&& (Request->Method != HttpMethodDelete)
			&& (Request->Method != HttpMethodPatch)
			&& (Request->Method != HttpMethodPost)) {
		return EFI_UNSUPPORTED;
	}

	//
	// Parse the URI of the remote host.
	//
	UrlLen = StrLen(Request->Url) + 1;
	Url = AllocateZeroPool(UrlLen);
	if (Url == NULL) {
		return EFI_OUT_OF_RESOURCES;
	}

	UnicodeStrToAsciiStr(Request->Url, Url);

	//
	// Create request message.
	//
	FileUrl = Url;
	if (*FileUrl != '/') {
		//
		// Convert the absolute-URI to the absolute-path
		//
		while (*FileUrl != ':') {
			FileUrl++;
		}
		if ((*(FileUrl + 1) == '/') && (*(FileUrl + 2) == '/')) {
			FileUrl += 3;
			while (*FileUrl != '/') {
				FileUrl++;
			}
		} else {
			FreePool(Url);
			return EFI_INVALID_PARAMETER;
		}
	}
	RequestStr = HttpGenRequestString(HttpRequest, ParamsPaddingSize,
			FileUrl, RequestSize);
	FreePool(Url);
	if (RequestStr == NULL) {
		return EFI_OUT_OF_RESOURCES;
	}

	*InRequestStr = RequestStr;
	return EFI_SUCCESS;
}

/**
 The function of RestIpmiUsbResponse().

 **/

EFI_STATUS RestIpmiUsbResponse(CHAR8 *InResponseStr, UINT32 ResponseStrLen,
		EFI_HTTP_MESSAGE *HttpResponse) {
	EFI_STATUS Status;
	CHAR8 *EndofHeader;
	CHAR8 *HttpHeaders;
	UINTN SizeofHeaders;
	CHAR8 *Buffer;
	UINTN BufferSize;
	UINTN StatusCode;
	CHAR8 *Tmp;
	CHAR8 *HeaderTmp;
	CHAR8 *StatusCodeStr;

	HttpResponse->Headers = NULL;
	HttpResponse->Body = NULL;
	HttpHeaders = NULL;
	SizeofHeaders = 0;
	Buffer = NULL;
	BufferSize = 0;
	EndofHeader = NULL;

	if ((InResponseStr == NULL) || (ResponseStrLen == 0)
			|| (HttpResponse == NULL)) {
		return EFI_INVALID_PARAMETER;
	}
	//
	// Search for HTTP Version.
	//
	Tmp = AsciiStrStr(InResponseStr, HTTP_VERSION);
	if (Tmp == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "\nREST: Http HTTP_VERSION Error\n"));
		return EFI_NOT_FOUND;
	}
	//
	// Search for Status Code.
	//
	StatusCodeStr = Tmp + AsciiStrLen(HTTP_VERSION) + 1;
	if (StatusCodeStr == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "\nREST: Http StatusCodeStr Error\n"));
		return EFI_NOT_FOUND;
	}
	StatusCode = AsciiStrDecimalToUintn(StatusCodeStr);

	//
	// Remove the first line of HTTP message, e.g. "HTTP/1.1 200 OK\r\n".
	//
	Tmp = AsciiStrStr(Tmp, HTTP_CRLF_STR);
	if (Tmp == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "\nREST: Http HTTP_CRLF_STR Error\n"));
		return EFI_NOT_FOUND;
	}
	HeaderTmp = Tmp + AsciiStrLen(HTTP_CRLF_STR);

	EndofHeader = AsciiStrStr(Tmp, HTTP_END_OF_HDR_STR);
	if (EndofHeader == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "\nREST: Http HTTP_END_OF_HDR_STR Error\n"));
		return EFI_NOT_FOUND;
	}

	SizeofHeaders = EndofHeader - HeaderTmp;

	//
	// Check whether the EFI_HTTP_UTILITIES_PROTOCOL is available.
	//
	if (gHttpUtilities == NULL) {
		return EFI_NOT_READY;
	}

	//
	// Parse the HTTP header into array of key/value pairs.
	//
	Status = gHttpUtilities->Parse(gHttpUtilities, HeaderTmp, SizeofHeaders,
			&HttpResponse->Headers, &HttpResponse->HeaderCount);
	if (EFI_ERROR (Status)) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "\nREST: Http Parse Error\n"));
		goto Error;
	}

	HttpResponse->Data.Response->StatusCode = HttpMappingToStatusCode(
			StatusCode);

	//
	// Receive the response body.
	//
	HttpResponse->BodyLength = ResponseStrLen
			- (EndofHeader - InResponseStr)
			- AsciiStrLen(HTTP_END_OF_HDR_STR);
	if (HttpResponse->BodyLength > 0) {
		HttpResponse->Body = AllocatePool(HttpResponse->BodyLength);
		if (HttpResponse->Body == NULL) {
			Status = EFI_OUT_OF_RESOURCES;
			goto Error;
		}
		CopyMem(HttpResponse->Body,
				EndofHeader + AsciiStrLen(HTTP_END_OF_HDR_STR),
				HttpResponse->BodyLength);
	} else {
		HttpResponse->Body = NULL;
	}
	return EFI_SUCCESS;

 Error: 
 	
 	if (HttpResponse->Headers != NULL) {
		FreePool(HttpResponse->Headers);
		HttpResponse->Headers = NULL;
	}
	if (HttpResponse->Body != NULL) {
		FreePool(HttpResponse->Body);
		HttpResponse->Body = NULL;
	}

	return Status;

}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
