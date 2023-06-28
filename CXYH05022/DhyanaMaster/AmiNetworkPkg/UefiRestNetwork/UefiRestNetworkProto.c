/** @file
  Implementation of EFI_HTTP_PROTOCOL protocol interfaces.

  Copyright (c) 2015, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "UefiRestNetworkDriver.h"
#include "UefiRestNetworkProto.h"
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include "Token.h"

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
  )
{
    EFI_STATUS      Status = EFI_NOT_FOUND;
    EFI_TPL         OldTpl = TPL_APPLICATION;
    UINT8           RetrieveCount = 0;
    UINT32          RetrieveDelay = 0;
    UINTN           Counter = 0;
    
    if ((This == NULL) || (RequestMessage == NULL) || ((ResponseMessage == NULL))) {
        DEBUG((DEBUG_ERROR, "RestNetwork: EfiRestSendReceive EFI_INVALID_PARAMETER\n"));
        return EFI_INVALID_PARAMETER;
    }
    
    if( (RequestMessage->Data.Request->Method == HttpMethodPost) ||
            (RequestMessage->Data.Request->Method == HttpMethodGet) ||
            (RequestMessage->Data.Request->Method == HttpMethodPut) ||
            (RequestMessage->Data.Request->Method == HttpMethodPatch) ||
            (RequestMessage->Data.Request->Method == HttpMethodDelete) )
    {
        
        OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);    // guarantees that RestoreTPL won't ASSERT
        pBS->RestoreTPL (TPL_APPLICATION);          //Reducing the TPL before communicating with network

        RetrieveCount = PcdGet8 (PcdConnectionResetRetrievalCount);
        RetrieveDelay = PcdGet32 (PcdConnectionResetRetrievalDelay);
        
RETRIEVEAGAIN:        
        Status = SendRecieveRestHttpCommand(This, RequestMessage, ResponseMessage);
        
        if (EFI_CONNECTION_RESET == Status)
        {
            Counter ++;
            if (Counter <= RetrieveCount)
            {
                pBS->Stall (RetrieveDelay);
                goto RETRIEVEAGAIN;
            }
        }
        pBS->RaiseTPL (TPL_HIGH_LEVEL);             // guarantees that RestoreTPL won't ASSERT
        pBS->RestoreTPL (OldTpl);
    }
    
    return Status;
}

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
  ) 
{
	return EFI_UNSUPPORTED;
}

