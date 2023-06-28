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




#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Http.h>
#include <Protocol/Rest.h>
#include <Protocol/HttpUtilities.h>
#include <Protocol/IpmiUsbTransportProtocol.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include "string.h"

//TODO: Remove the following #defines. Common place needed
#define HTTP_DEFAULT_PORT        80
#define HTTP_END_OF_HDR_STR      "\r\n\r\n"
#define HTTP_CRLF_STR            "\r\n"
#define HTTP_VERSION_STR         "HTTP/1.1"
#define HTTP_VERSION_CRLF_STR    " HTTP/1.1\r\n"
#define HTTP_GET_STR             "GET "
#define HTTP_HEAD_STR            "HEAD "
#define HTTP_POST_STR            "POST "
#define HTTP_PUT_STR             "PUT "
#define HTTP_DELETE_STR          "DELETE "
#define HTTP_PATCH_STR           "PATCH "

UINTN Sprintf( char *Buffer, char *Format, ...);
EFI_REST_PROTOCOL* gRestService = (EFI_REST_PROTOCOL*)NULL;

EFI_STATUS SendBMCGetRequest (EFI_REST_PROTOCOL 	*RestInstance, CHAR8 *RequestStr, VOID **Response, UINTN *ResponseLength)
{
    EFI_STATUS Status;
    CHAR8 *EndofHeader;
    CHAR8 *HttpHeaders;
    UINTN SizeofHeaders;
    CHAR8 *Tmp, *UrlPtr;
    CHAR8 *HeaderTmp;
    INTN ResCmp;
	UINT32 RequestStrLen = 0;
    EFI_HTTP_MESSAGE *HttpRequest = (EFI_HTTP_MESSAGE *)NULL;
    EFI_HTTP_UTILITIES_PROTOCOL *HttpUtilities = (EFI_HTTP_UTILITIES_PROTOCOL *)NULL;
    EFI_HTTP_MESSAGE ResponseMessage;
    EFI_HTTP_HEADER *TempHeaders = (EFI_HTTP_HEADER *)NULL;
    UINTN TempHeaderCount = 0;
    
	if (NULL != RequestStr)
		RequestStrLen = (UINT32)AsciiStrLen(RequestStr);
    Status = gBS->LocateProtocol(&gEfiHttpUtilitiesProtocolGuid, NULL,
            (VOID **) &HttpUtilities);
    if (EFI_ERROR (Status)) {
        DEBUG((-1,"RestService: Locate HttpUtilities protocol Error %r\n", Status));
        return Status;
    }
    DEBUG((-1, "\nRestService: Protocols success"));
 
    HttpRequest = AllocatePool(sizeof(EFI_HTTP_MESSAGE));
    if(HttpRequest == NULL){
        return EFI_OUT_OF_RESOURCES;
    }
                
    HttpRequest->Data.Request = NULL;
    HttpRequest->Headers = NULL;
    HttpRequest->Body = NULL;
    HttpHeaders = NULL;
    SizeofHeaders = 0;
    EndofHeader = NULL;
    

    if ((RequestStr == NULL) || (RequestStrLen == 0))
	{
        Status = EFI_INVALID_PARAMETER;
        goto Error;
    }
    //
    // Check whether the EFI_HTTP_UTILITIES_PROTOCOL is available.
    //
    if (HttpUtilities == NULL) 
	{
	    Status = EFI_NOT_READY;
        goto Error;
    }
    
    HttpRequest->Data.Request = AllocatePool(sizeof(EFI_HTTP_REQUEST_DATA));
    if(HttpRequest->Data.Request == NULL){
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    //
    // Search for HTTP Version.
    //
    Tmp = RequestStr;
    ResCmp = AsciiStrnCmp(Tmp, HTTP_GET_STR, AsciiStrLen(HTTP_GET_STR));
    if (ResCmp == 0) {
        HttpRequest->Data.Request->Method = HttpMethodGet;
        UrlPtr = Tmp + AsciiStrLen(HTTP_GET_STR);
        goto Crlfparse;
    }
    ResCmp = AsciiStrnCmp(Tmp, HTTP_POST_STR, AsciiStrLen(HTTP_POST_STR));
    if (ResCmp == 0) {
        HttpRequest->Data.Request->Method = HttpMethodPost;
        UrlPtr = Tmp + AsciiStrLen(HTTP_POST_STR);
        goto Crlfparse;
    }
    ResCmp = AsciiStrnCmp(Tmp, HTTP_PUT_STR, AsciiStrLen(HTTP_PUT_STR));
    if (ResCmp == 0) {
        HttpRequest->Data.Request->Method = HttpMethodPut;
        UrlPtr = Tmp + AsciiStrLen(HTTP_PUT_STR);
        goto Crlfparse;
    }
    ResCmp = AsciiStrnCmp(Tmp, HTTP_PATCH_STR, AsciiStrLen(HTTP_PATCH_STR));
    if (ResCmp == 0) {
        HttpRequest->Data.Request->Method = HttpMethodPatch;
        UrlPtr = Tmp + AsciiStrLen(HTTP_PATCH_STR);
        goto Crlfparse;
    }
    Status = EFI_NOT_FOUND;
    goto Error;
    
Crlfparse:  


    //
    // Search for HTTP Version.
    //
    Tmp = AsciiStrStr(UrlPtr, HTTP_VERSION_STR);
    if (Tmp == NULL) {
        Status = EFI_NOT_FOUND;
        goto Error;
    }

    *(Tmp - 1) = '\0';
    
    HttpRequest->Data.Request->Url = AllocateZeroPool((AsciiStrLen(UrlPtr) + 1)*sizeof (CHAR16));
    if (HttpRequest->Data.Request->Url == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    AsciiStrToUnicodeStr(UrlPtr, HttpRequest->Data.Request->Url);

    Tmp = AsciiStrStr(Tmp, HTTP_CRLF_STR);
    if (Tmp == NULL) {
        Status = EFI_NOT_FOUND;
        goto Error;
    }
    HeaderTmp = Tmp + AsciiStrLen(HTTP_CRLF_STR);

    ResponseMessage.Data.Response = NULL;

    ResponseMessage.Data.Response = AllocatePool(sizeof(EFI_HTTP_RESPONSE_DATA));
    if(ResponseMessage.Data.Response == NULL){
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    
    if(RequestStrLen - (HeaderTmp - RequestStr) == 0){
        HttpRequest->Body = NULL;
        HttpRequest->BodyLength = 0;
        HttpRequest->Headers = NULL;
        HttpRequest->HeaderCount = 0;
        Status = RestInstance->SendReceive(RestInstance, HttpRequest, &ResponseMessage);
        DEBUG((-1, "\nRestService: Get RestInstance->SendReceiv Status - %r,Size:%d\n", Status,ResponseMessage.BodyLength));
        if (EFI_ERROR (Status)) {
               DEBUG((-1,"RestService: Send Command Error %r\n", Status));
               goto Error;
        }

       *Response = ResponseMessage.Body;
       *ResponseLength = ResponseMessage.BodyLength;

       Status = EFI_SUCCESS;
       goto Error;
    }
        
    EndofHeader = AsciiStrStr(HeaderTmp, HTTP_END_OF_HDR_STR);
    if (EndofHeader == NULL) {
        Status = EFI_NOT_FOUND;
        goto Error;
    }

    SizeofHeaders = EndofHeader - HeaderTmp + 2;

    DEBUG((-1,"RestService: Size = %d, Headers\n%a\n", SizeofHeaders, HeaderTmp));

    //
    // Parse the HTTP header into array of key/value pairs.
    //
    Status = HttpUtilities->Parse(HttpUtilities, HeaderTmp, SizeofHeaders,
            &TempHeaders, &TempHeaderCount);
    if (EFI_ERROR (Status)) {
        DEBUG((-1, "\nRestService: Http Parse Error - %r\n", Status));
        goto Error;
    }

    HttpRequest->Headers = TempHeaders;
    HttpRequest->HeaderCount = TempHeaderCount;
    //
    // Receive the response body.
    //
    HttpRequest->BodyLength = RequestStrLen - (EndofHeader - RequestStr) - AsciiStrLen(HTTP_END_OF_HDR_STR);
    if (HttpRequest->BodyLength > 0) {

        HttpRequest->Body = AllocatePool(HttpRequest->BodyLength);
        if (HttpRequest->Body == NULL) {
            Status = EFI_OUT_OF_RESOURCES;
            goto Error;
        }
        CopyMem(HttpRequest->Body, EndofHeader + AsciiStrLen(HTTP_END_OF_HDR_STR),
                HttpRequest->BodyLength);
        
        DEBUG((-1,"Send HttpRequest->BodyLength:%d\n", HttpRequest->BodyLength));

        Status = RestInstance->SendReceive(RestInstance, HttpRequest, &ResponseMessage);
        DEBUG((-1, "\nRestService: POST/PUT RestInstance->SendReceiv Status - %r,Size:%d\n", Status,ResponseMessage.BodyLength));

        if (EFI_ERROR (Status)) {
        	DEBUG((-1,"Send Command Error %r\n", Status));
            goto Error;
        }
        
        *Response = ResponseMessage.Body;
        *ResponseLength = ResponseMessage.BodyLength;
        
    } else {
        HttpRequest->Body = NULL;
    }

 Error: 
 
    if (HttpRequest->Data.Request->Url != NULL) {
        FreePool(HttpRequest->Data.Request->Url);
    }
    if (HttpRequest->Data.Request != NULL) {
        FreePool(HttpRequest->Data.Request);
    }
    if (HttpRequest->Headers != NULL) {
        FreePool(HttpRequest->Headers);
    }
    if (HttpRequest->Body != NULL) {
        FreePool(HttpRequest->Body);
    }
    if (HttpRequest != NULL) {
        FreePool(HttpRequest);
    }

    return Status;

}




EFI_REST_PROTOCOL* GetRestInstance()
{
	EFI_STATUS 			Status = EFI_NOT_FOUND;
	UINTN 				HandleCount = 0;
	UINTN 				iIndex = 0;
	EFI_HANDLE 			*Handles = NULL;
	void                *DxeIpmiUsbInstance = NULL;
	EFI_REST_PROTOCOL 	*RestInstance = (EFI_REST_PROTOCOL *)NULL;
	
	if(NULL == gRestService)
	{
		Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiRestProtocolGuid, NULL, &HandleCount, &Handles);
		if (!EFI_ERROR(Status))
		{
			for (iIndex = 0; iIndex < HandleCount; iIndex++ )
			{
				if (!EFI_ERROR(gBS->HandleProtocol (Handles [iIndex], &gEfiDxeIpmiUsbTransportProtocolGuid, (void**)&DxeIpmiUsbInstance)))
				{
					Status = gBS->HandleProtocol (Handles [iIndex], &gEfiRestProtocolGuid, &RestInstance);
					if (!EFI_ERROR(Status))
					{
						gRestService = RestInstance;
						break;
					}		
				}
			}
		}
	}
	return gRestService;
}

EFI_STATUS SendPatchReq ()
{
	CHAR8 		GetUrlPath[128] = {0};
	CHAR8 		*RequestStr = NULL;
    CHAR8 		*Response = (CHAR8 *) NULL;
    UINTN 		ResponseLength = 0;
    EFI_STATUS	Status = EFI_SUCCESS;    
	EFI_REST_PROTOCOL 	*RestInstance = (EFI_REST_PROTOCOL *)NULL;
	CHAR8 *PostData = NULL;
	UINTN PostDataLength = 0;
	
	RestInstance = GetRestInstance();

	if(RestInstance)
	{
		
		PostData = (CHAR8*)AllocateZeroPool((512)*(sizeof(CHAR8)));

		if (NULL == PostData)
		{
			return EFI_OUT_OF_RESOURCES;
		}
		
		Sprintf(PostData,"%s","{\
				\"Boot\":\
				{\
				\"BootSourceOverrideEnabled\": \"Disabled\",\
				\"BootSourceOverrideTarget\": \"None\",\
				\"UefiTargetBootSourceOverride\": \"\"\
				}\
				}\
				");
		PostDataLength = AsciiStrLen(PostData);
		RequestStr = (CHAR8 *)AllocateZeroPool(PostDataLength + 128);
		if (NULL == RequestStr)
		{
			if (PostData)
				FreePool (PostData);
			
			return EFI_OUT_OF_RESOURCES;
		}
		//<xuyj1001-20200401 Add BIOS OOB Function +>
		Sprintf(RequestStr, 
		        "PATCH /redfish/v1/Systems/Self HTTP/1.1\r\n"
		        "Content-length: %d\r\n"
		        "\r\n%s", PostDataLength, PostData);
		//<xuyj1001-20200401 Add BIOS OOB Function ->
		Status = SendBMCGetRequest(RestInstance,RequestStr,(VOID **)&Response, &ResponseLength);

	  if (RequestStr)
			FreePool (RequestStr);
	  if (PostData)
			FreePool (PostData);
	}

    return Status;
}

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

