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

/** @file UefiRestNetworkImpl.c
 Implementation of Rest over Http Interface.
 **/

//----------------------------------------------------------------------
#include "UefiRestNetworkImpl.h"
#include "UefiRestNetworkSupport.h"
#include "UefiRestNetworkDriver.h"
#include "Library/NetLib.h"
//----------------------------------------------------------------------

char    *gChunkedData = NULL;
UINTN   gChunkedDataSize = 0;
int     gChunkedDataAllocatedSize = 0;

/**
  REST send command Implementation
    
  @param  This              Rest protocol instance
  @param  RequestMessage    Input request rest message
  @param  ResponseMessage   Output message filled with server response

  @return EFI_STATUS
**/

EFI_STATUS
SendRecieveRestHttpCommand(
		IN EFI_REST_PROTOCOL *This,
		IN EFI_HTTP_MESSAGE *RequestMessage,
		OUT EFI_HTTP_MESSAGE *ResponseMessage
		)
{
	EFI_STATUS Status = EFI_NOT_FOUND;
	UEFI_REST_NETWORK_INSTANCE_DATA	*RestHttpInstance = (UEFI_REST_NETWORK_INSTANCE_DATA *)NULL;
	HTTP_IO_RESPONSE_DATA *ResponseData = (HTTP_IO_RESPONSE_DATA  *)NULL;
	HTTP_IO_RESPONSE_DATA ResponseBody;
	EFI_HTTP_HEADER *Header = (EFI_HTTP_HEADER *)NULL;
	UINTN ContentLength = 0;
	CHAR8 *BodyBuffer = (CHAR8 *)NULL;
	UINTN BodyLength = 0;
	UEFI_REST_NETWORK_INSTANCE_DATA *TmpRestHttpInstance = (UEFI_REST_NETWORK_INSTANCE_DATA *)NULL;
	BOOLEAN HeaderOnly = 0;
	VOID *Parser = (VOID *)NULL;
	UINT8 *Block = (UINT8 *)NULL;
    BOOLEAN IdentityMode = FALSE;
    HTTP_IO_CONFIG_DATA ConfigData;
    EFI_SIMPLE_NETWORK_PROTOCOL *SimpleNet;
    
	//
	// Get the pointer to RestHttpInstance from PPI
	//
	RestHttpInstance = UEFI_REST_PRIVATE_DATA_FROM_PROTOCOL(This);
	
	if (RestHttpInstance == NULL) {
		DEBUG((DEBUG_INFO, "RestNetworkHttp: RestHttpInstance == NULL\n"));
		return EFI_PROTOCOL_ERROR;
	}
	
    Status = pBS->HandleProtocol(
            RestHttpInstance->ControllerHandle,
            &gEfiSimpleNetworkProtocolGuid,
            (VOID*) &SimpleNet);
    
    if (EFI_ERROR (Status)) {
        return Status;
    }
    if ((!SimpleNet->Mode->MediaPresent) || (!SimpleNet->Mode->MediaPresentSupported))
    {
        DEBUG((DEBUG_ERROR, "UefiRestNetwork: MediaPresent: %d, MediaPresentSupported: %d\n", SimpleNet->Mode->MediaPresent, SimpleNet->Mode->MediaPresentSupported));
        return EFI_NO_MEDIA;
    }
	
	DEBUG((DEBUG_INFO, "RestNetworkHttp: SendRecieveRestHttpCommand started\n"));

    //
    // Validate input parameters
    //
    if (!RequestMessage || !ResponseMessage) {
        DEBUG((DEBUG_INFO, "RestNetworkHttp: Request or Response NULL\n"));
        return EFI_INVALID_PARAMETER;
    }
    
    ZeroMem (&ConfigData, sizeof (HTTP_IO_CONFIG_DATA));
    ConfigData.Config4.HttpVersion    = HttpVersion11;
    ConfigData.Config4.RequestTimeOut = 0x2710;
    ConfigData.Config4.UseDefaultAddress = TRUE;
    
    Status = HttpIoCreateIo (
            RestHttpInstance->ImageHandle,
            RestHttpInstance->ControllerHandle,
            IP_VERSION_4,
            &ConfigData,
            &RestHttpInstance->HttpIo
            );
    DEBUG((DEBUG_ERROR, "REST_HTTP: Install HttpIoCreateIo - Status: %r\n", Status));
    if (EFI_ERROR (Status)) {
        FreePool(RestHttpInstance);
        RestHttpInstance = NULL;
        return Status;
    }
    
	Status = HttpIoSendRequest(&RestHttpInstance->HttpIo, RequestMessage->Data.Request, 
			RequestMessage->HeaderCount, RequestMessage->Headers, 
			RequestMessage->BodyLength, RequestMessage->Body);
	if(EFI_ERROR(Status)){
		DEBUG((DEBUG_INFO, "RestNetworkHttp: HttpIoSendRequest Error - %r\n", Status));
		goto HttpError;
	}

	//
	// Receive HTTP response message.
	//
	
	//
	// First step, use zero BodyLength to only receive the response headers.
	//
	ResponseData = AllocateZeroPool (sizeof(HTTP_IO_RESPONSE_DATA));
	if (ResponseData == NULL) {
		Status = EFI_OUT_OF_RESOURCES;
		goto HttpError;
	}
	Status = HttpIoRecvResponse (
			&RestHttpInstance->HttpIo,
			TRUE,
			ResponseData
			);
    if (!EFI_ERROR (Status)) {
        
        DEBUG((DEBUG_INFO, "RestNetworkHttp: Status - %r, ResponseData->Status - %d, StatusCode - %d\n", Status, ResponseData->Status, ResponseData->Response.StatusCode));
      
        Header = RestHttpFindHeader (ResponseData->HeaderCount, ResponseData->Headers, "Content-Length");
        if (Header == NULL) {
            DEBUG((DEBUG_INFO, "RestNetworkHttp: Header is 0\n"));
            ContentLength = 0;
        }
        else{
            ContentLength = AsciiStrDecimalToUintn (Header->FieldValue);
        }
        
        DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: ContentLength: %d\n", ContentLength));
        if(ContentLength > 0){
            ZeroMem (&ResponseBody, sizeof (HTTP_IO_RESPONSE_DATA));
            BodyBuffer  = (CHAR8*) AllocateZeroPool (ContentLength);
            BodyLength = 0;
            if (BodyBuffer != NULL) {
                do{
                    ResponseBody.BodyLength = ContentLength - BodyLength;
                    ResponseBody.Body       = BodyBuffer + BodyLength;
                    Status = HttpIoRecvResponse (
                            &RestHttpInstance->HttpIo,
                            FALSE,
                            &ResponseBody
                            );
                    if (EFI_ERROR (Status)) {
                        if (BodyBuffer != NULL) {
                            FreePool (BodyBuffer);
                            BodyBuffer = NULL;
                        }
                        break;
                    }

                    BodyLength += ResponseBody.BodyLength;
                }while(BodyLength < ContentLength);
            }
            else
            {
                DEBUG((DEBUG_INFO, "RestNetworkHttp: Error Allocating memory\n"));
                Status = EFI_OUT_OF_RESOURCES;
            }
        }
        else //for chunked data, ContentLength will be zero
        {
            Header = RestHttpFindHeader (ResponseData->HeaderCount, ResponseData->Headers, "Transfer-Encoding");
            if (Header == NULL) {
                ResponseMessage->Data.Response = AllocatePool(sizeof(EFI_HTTP_RESPONSE_DATA));
                ResponseMessage->Data.Response->StatusCode = ResponseData->Response.StatusCode;
                ResponseMessage->HeaderCount = ResponseData->HeaderCount;
                ResponseMessage->Headers = ResponseData->Headers;
                DEBUG((DEBUG_INFO, "RestNetworkHttp: Header is 0\n"));
                goto HttpError;
            }
            else{
                if(AsciiStrCmp(Header->FieldValue, "chunked"))
                {
                    Status = EFI_ABORTED;
                    goto HttpError;
                }
            }
            
            gChunkedData = NULL;
            gChunkedDataSize = 0;
            gChunkedDataAllocatedSize = 0;
            
            Parser = NULL;
            Status = HttpInitMsgParser (
                     RequestMessage->Data.Request->Method,
                     ResponseData->Response.StatusCode,
                     ResponseData->HeaderCount,
                     ResponseData->Headers,
                     RestParserCallback,
                     NULL, 
                     &Parser
                     );
            DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: HttpInitMsgParser status: %r\n", Status));
            if (EFI_ERROR (Status)) {
                goto ChunkedError;
            }
            else
            {        
              ZeroMem (&ResponseBody, sizeof (HTTP_IO_RESPONSE_DATA));
              DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: Chunked data reading\n"));
              Block = NULL;
              while (!HttpIsMessageComplete (Parser)) 
              {
                  if(Block)
                  {
                      FreePool(Block);
                      Block = (UINT8*)NULL;
                  }
                  Block = AllocateZeroPool (HTTP_BOOT_BLOCK_SIZE);
                  DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: After Allocate pool\n"));
                  ResponseBody.Body       = (CHAR8*) Block;
                  ResponseBody.BodyLength = HTTP_BOOT_BLOCK_SIZE;
                  Status = HttpIoRecvResponse (
                             &RestHttpInstance->HttpIo,
                             FALSE,
                             &ResponseBody
                             );
                  DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: HttpIoRecvResponse status %r ResponseBody.Status %r\n", Status, ResponseBody.Status));
                  
                  if (EFI_ERROR (Status) || EFI_ERROR (ResponseBody.Status)) {
                    if (EFI_ERROR (ResponseBody.Status)) {
                      Status = ResponseBody.Status;
                      goto ChunkedError;
                    }
                  }
                  else
                  {
                      DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: ResponseBody:\n%a\n",(CHAR8 *)ResponseBody.Body));
                  }
            
                  //
                  // Parse the new received block of the message-body, the block will be saved in cache.
                  //
                  Status = HttpParseMessageBody (
                             Parser,
                             ResponseBody.BodyLength,
                             ResponseBody.Body
                             );
                  DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: HttpParseMessageBody status %r\n", Status));
                  if (EFI_ERROR (Status)) {
                    goto ChunkedError;
                  }
              }
              BodyBuffer = gChunkedData;
              ContentLength = gChunkedDataSize;
              DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: Chunked ContentLength:%d\n", ContentLength));
            }
ChunkedError: 
            if (Parser != NULL) {
               HttpFreeMsgParser (Parser);
            }
           if(Block)
           {
              FreePool(Block);
              Block = (UINT8*)NULL;
           }
        }
        
        DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: BodyBuffer \n%a\n", BodyBuffer));
        DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: ContentLength %x\n", ContentLength));
        ResponseMessage->Body = BodyBuffer;
        ResponseMessage->BodyLength = ContentLength;
        ResponseMessage->HeaderCount = ResponseData->HeaderCount;
        ResponseMessage->Headers = ResponseData->Headers;
        ResponseMessage->Data.Response = AllocatePool(sizeof(EFI_HTTP_RESPONSE_DATA));
        ResponseMessage->Data.Response->StatusCode = ResponseData->Response.StatusCode;
        
        DEBUG((DEBUG_INFO, "RestNetworkHttp: SendRecieveRestHttpCommand finished ContentLength: %lx\n", ContentLength));
    }
    else{
        if (ResponseData != NULL) {
            if(ResponseData->HeaderCount > 0 && ResponseData->Headers != NULL){
                FreePool(ResponseData->Headers);
            }
            FreePool (ResponseData);
        }
    }
	
	DEBUG((DEBUG_INFO, "RestNetworkHttp: SendRecieveRestHttpCommand finished with error - %r\n", Status));
HttpError:
    HttpIoDestroyIo(&RestHttpInstance->HttpIo);
	return Status;
}

/**
  Callback for chunked data buffer filling
    
  @param  Data              Chunked data
  @param  Length            Chunked data length
  @param  Context           Not used

  @return EFI_STATUS
**/

EFI_STATUS RestParserCallback (
  IN HTTP_BODY_PARSE_EVENT EventType,
  IN CHAR8                      *Data,
  IN UINTN                      Length,
  IN VOID                       *Context
)
{
    char *TempPointer = (char *)NULL;
    
    if (EventType != RestBodyParseEventOnData) {
        return EFI_SUCCESS;
    }
    DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: Chunked Length: %d\n", Length));
    if (gChunkedDataSize + Length >= (UINTN)gChunkedDataAllocatedSize)
    {
        gChunkedDataAllocatedSize += 8096;
        TempPointer = ReallocatePool(AsciiStrLen(gChunkedData), gChunkedDataAllocatedSize, gChunkedData);
        if (NULL != TempPointer)
        {
            gChunkedData = TempPointer;
        }
        else
        {
            DEBUG((DEBUG_INFO, "SendRecieveRestHttpCommand: Chunked EFI_OUT_OF_RESOURCES\n"));
            return EFI_OUT_OF_RESOURCES;
        }
    }
    CopyMem (gChunkedData + gChunkedDataSize, Data, Length);
    gChunkedDataSize += Length;

    return EFI_SUCCESS;
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
