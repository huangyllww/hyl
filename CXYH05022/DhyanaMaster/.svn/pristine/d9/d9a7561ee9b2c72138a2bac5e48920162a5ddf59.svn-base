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



#include "RestBoot.h"
#include <Protocol/Http.h>
#include <Protocol/HttpUtilities.h>
#include <Protocol/IpmiUsbTransportProtocol.h>



EFI_STATUS SendRecieveBMCRequest (
		EFI_REST_PROTOCOL *RestInstance, 
		EFI_HTTP_METHOD HttpMethod,
		CHAR8 *HttpUrl, 
		CHAR8 *HttpHeaders, 
		CHAR8 *HttpBody, 
		VOID **Response, 
		UINT32 *ResponseLength, 
		EFI_HTTP_STATUS_CODE *StatusCode)
{
    EFI_STATUS			Status;
    UINTN			HttpHeadersSize = 0;
    EFI_HTTP_MESSAGE		*HttpRequest = (EFI_HTTP_MESSAGE *)NULL;
    EFI_HTTP_MESSAGE 		HttpResponse;
    EFI_HTTP_UTILITIES_PROTOCOL	*HttpUtilities = (EFI_HTTP_UTILITIES_PROTOCOL *)NULL;
    
    
    *StatusCode = HTTP_STATUS_UNSUPPORTED_STATUS;
    if (NULL == HttpUrl || AsciiStrLen(HttpUrl) == 0)
    {
	    return EFI_INVALID_PARAMETER;
    }
  
    Status = gBS->LocateProtocol(&gEfiHttpUtilitiesProtocolGuid, NULL,
            (VOID **) &HttpUtilities);
    if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR,"SendBMCRequest: Locate HttpUtilities protocol Error %r\n", Status));
        return Status;
    }
 
    HttpRequest = AllocatePool(sizeof(EFI_HTTP_MESSAGE));
    if(HttpRequest == NULL){
        return EFI_OUT_OF_RESOURCES;
    }
                
    HttpRequest->Data.Request = NULL;
    HttpRequest->Headers = NULL;
    HttpRequest->HeaderCount = 0;
    HttpRequest->Body = NULL;
    HttpRequest->BodyLength = 0;
    
    HttpResponse.Data.Response = NULL;
    HttpResponse.Headers = NULL;
    HttpResponse.HeaderCount = 0;
    HttpResponse.Body = NULL;
    HttpResponse.BodyLength = 0;

    
    HttpRequest->Data.Request = AllocatePool(sizeof(EFI_HTTP_REQUEST_DATA));
    if(HttpRequest->Data.Request == NULL){
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    
    HttpRequest->Data.Request->Method = HttpMethod;

    HttpRequest->Data.Request->Url = AllocateZeroPool((AsciiStrLen(HttpUrl) + 1)*sizeof (CHAR16));
    if (HttpRequest->Data.Request->Url == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    AsciiStrToUnicodeStr(HttpUrl, HttpRequest->Data.Request->Url);

    if (HttpHeaders != NULL) 
    {
        HttpHeadersSize = AsciiStrLen(HttpHeaders);
	//
	// Parse the HTTP header into array of key/value pairs.
	//
	Status = HttpUtilities->Parse(HttpUtilities, HttpHeaders, HttpHeadersSize,
	    &HttpRequest->Headers, &HttpRequest->HeaderCount);
	if (EFI_ERROR (Status)) {
		DEBUG((EFI_D_ERROR, "SendBMCRequest: Http Parse Headers Error - %r\n", Status));
		goto Error;
	}
    }
    
    //
    // Receive the response body.
    //
    if (HttpBody != NULL) 
    {
	    HttpRequest->BodyLength = AsciiStrLen(HttpBody);
	    if (HttpRequest->BodyLength > 0) {
	
		HttpRequest->Body = AllocatePool(HttpRequest->BodyLength);
		if (HttpRequest->Body == NULL) {
		    Status = EFI_OUT_OF_RESOURCES;
		    goto Error;
		}
		CopyMem(HttpRequest->Body, HttpBody, HttpRequest->BodyLength);
	    }     
    }
    
    HttpResponse.Data.Response = AllocatePool(sizeof(EFI_HTTP_RESPONSE_DATA));
    if(HttpResponse.Data.Response == NULL){
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    
    
    
    Status = RestInstance->SendReceive(RestInstance, HttpRequest, &HttpResponse);
    
    if (EFI_ERROR (Status)) {
	    DEBUG((EFI_D_ERROR,"SendBMCRequest: Send Command Error %r\n", Status));
            goto Error;
    }
    
    *Response = AllocatePool(HttpResponse.BodyLength);
    if(*Response == NULL){
        Status = EFI_OUT_OF_RESOURCES;
        goto Error;
    }
    
    CopyMem(*Response, HttpResponse.Body, HttpResponse.BodyLength);
    *ResponseLength = (UINT32)HttpResponse.BodyLength;
    *StatusCode = HttpResponse.Data.Response->StatusCode;

 Error: 

    if (HttpResponse.Data.Response != NULL) {
	FreePool(HttpResponse.Data.Response);
    }
    if (HttpResponse.Headers != NULL) {
        FreePool(HttpResponse.Headers);
    }
    if (HttpResponse.Body != NULL) {
        FreePool(HttpResponse.Body);
    }
 
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

