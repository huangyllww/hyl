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

/** @file RestIpmiImpl.c
 Implementation of Rest over Ipmi Interface.

 **/

//----------------------------------------------------------------------
#include <RestIpmiConfig.h>
#include "RestIpmiCommandDefines.h"
#include "RestIpmiImpl.h"
#include "RestIpmiProto.h"

//----------------------------------------------------------------------

//
// Global variables
//
BOOLEAN gRestIpmiProtocolError = FALSE; // Variable to check if REST protocol instance is still valid.
#define MAX_DELAY_RESEND_COUNT	0x5 // Number of the resend attempts to wait till Redfish server is up and running
#define MAX_RESEND_DELAY	4000000 // 4 sec delay to wait till Redfish server is up and running

//
// Function to send single packet of OEM IPMI command
// This command handles all faults during communication (bad CRC from both sides, inappropriate length etc.)
//

EFI_STATUS SendRestIpmiCommand(
		IN IPMI_USB_TRANSPORT	*IpmiUsb,
		IN UINT8		*RequestData,
		IN UINT32 		RequestSize,
		OUT UINT8               *ResponseData,
		OUT UINT32              *ResponseSize )
{
	EFI_STATUS			Status, ErrorStatus;
	REST_IPMI_RESPONSE_PARAMS	*ResponseParams;
	REST_IPMI_REQUEST_PARAMS	*RequestParams = (REST_IPMI_REQUEST_PARAMS*)RequestData;
	UINT32          		ResponseSizeTmp = 0;
	UINT32 				CrcValue = 0;
	UINT32				ReseivedCrcValue = 0;
	UINT16 				ResendCounter = 0, DelayCounter = 0;
	UINT8				StartPacketBit;
	
	ErrorStatus = EFI_DEVICE_ERROR;
	
	StartPacketBit = RequestParams->PacketID.PacketIDBits.StartPacketBit;
	//
	// loop to handle faults during transfer of data
	// it does not matter what direction fault happens there are MAX_COMMAND_RESEND_COUNT attempts to try sending data
	//
	while(ResendCounter < MAX_COMMAND_RESEND_COUNT)
	{

		ResponseSizeTmp = *ResponseSize;
		
#if REST_TEST_CASE == 3
		//Test case 3 SHOULD BE REMOVED IN RELEASE VERSION!!!
		if(RequestParams->PacketID.PacketIDBits.EndTransactionBit != 0x00){
			SERVER_REST_DEBUG((EFI_D_INFO, "REST_TEST_CASE_3\n"));
			RequestParams->TransactionID = 0;
		}
#endif

#if REST_TEST_CASE == 4
		//Test case 4 SHOULD BE REMOVED IN RELEASE VERSION!!!
		if(RequestParams->PacketID.PacketIDBits.EndTransactionBit != 0x00){
			SERVER_REST_DEBUG((EFI_D_INFO, "REST_TEST_CASE_4\n"));
			RequestParams->TransactionID++;
		}
#endif
			
		//
		// Calculate CRC of packet
		//
		RequestParams->CheckSum = 0;
		gBS->CalculateCrc32(RequestData, RequestSize, &CrcValue);
		RequestParams->CheckSum = CrcValue;
		
#if REST_TEST_CASE == 5
		//Test case 5 SHOULD BE REMOVED IN RELEASE VERSION!!!
		if(RequestParams->TransactionID != 0 && RequestParams->PacketID.PacketIDBits.ResendPacketBit == 0x00){
			SERVER_REST_DEBUG((EFI_D_INFO, "REST_TEST_CASE_5\n"));
			RequestParams->CheckSum++;
		}
#endif
		
		IpmiUsb->CommandCompletionCode = 0;
		
		//
		// Send IPMI command to BMC
		//
		Status = IpmiUsb->SendOemIpmiCommand(
			IpmiUsb,
			REST_NET_FUNCTION,
			REST_LUN,
			REST_COMMAND,
			RequestData,
			RequestSize,
			ResponseData,
			&ResponseSizeTmp);

		gBS->Stall(20*1000); //20ms delay //<kuaibai-20181222-fix hang b2 when ipmi abnormal>

		ResendCounter++;
		//
		// Check if the IPMI command was successfully sent
		//
		if (EFI_ERROR(Status)){
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error Status completion code.\nStatus - %r\nCompletion code - 0x%02x\n", Status, IpmiUsb->CommandCompletionCode));
			if (Status == EFI_DEVICE_ERROR && IpmiUsb->CommandCompletionCode == ERR_REST_IPMI_INV_REQUEST_SIZE){
				SERVER_REST_DEBUG((EFI_D_ERROR, "REST: EFI_DEVICE_ERROR. Completion code 0x%02x. Command should be resent\n", IpmiUsb->CommandCompletionCode));
				if(StartPacketBit == 0) RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0xFF;
				continue;
			}
			ASSERT(IpmiUsb->CommandCompletionCode != ERR_REST_IPMI_INV_REQUEST_BYTES);
			return Status;
		}
		//
		// check size of response:
		//
		if(ResponseSizeTmp < sizeof(REST_IPMI_RESPONSE_PARAMS)){
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Wrong response size.\nCompletion Code - %d\nResponse Size - %d", IpmiUsb->CommandCompletionCode, ResponseSizeTmp));
			if(StartPacketBit == 0) RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0xFF;
			continue;
		}
		

		//
		// Check if the Redfish OData server is ready to accept the requests
		//
		if (IpmiUsb->CommandCompletionCode == ERR_REST_IPMI_NO_REDFISH_RESPONSE) {
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Completion code 0x%02x - Redfish Service is not ready. Command should be resent\n", IpmiUsb->CommandCompletionCode));
			if(StartPacketBit == 0) RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0xFF;
			// Do not count it as a next command resend attempt
			if(DelayCounter < MAX_DELAY_RESEND_COUNT){
				DelayCounter++;
				ResendCounter--; 
			}
			else DelayCounter = 0;
			//Wait for one second before resend 
			gBS->Stall(MAX_RESEND_DELAY);
			continue;
		}
		DelayCounter = 0;
		
		//
		// Check CRC and request data again (ResendPacketBit should be set) if CRC is wrong
		//
		ResponseParams = (REST_IPMI_RESPONSE_PARAMS*)ResponseData;
		ReseivedCrcValue = ResponseParams->CheckSum;
		ResponseParams->CheckSum = 0x00;
		gBS->CalculateCrc32(ResponseData, ResponseSizeTmp, &CrcValue);
		
#if REST_TEST_CASE == 6
		//Test case 6 SHOULD BE REMOVED IN RELEASE VERSION!!!
		if(StartPacketBit == 0){
			SERVER_REST_DEBUG((EFI_D_INFO, "REST_TEST_CASE_6\n"));
			RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0xFF;
			continue;
		}
#endif
		
		
		if(CrcValue != ReseivedCrcValue){
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Wrong CheckSum - %x, %x\n", CrcValue, ReseivedCrcValue));
			if(StartPacketBit == 0) RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0xFF;
			continue;
		}
		
		//
		// Check if the packet should be resent because of a corruption during transfer
		//
		if (IpmiUsb->CommandCompletionCode == ERR_REST_IPMI_INV_CHECK_SUM || 
				IpmiUsb->CommandCompletionCode == ERR_REST_IPMI_INV_REQUEST_SIZE) {
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Completion code 0x%02x. Command should be resent\n", IpmiUsb->CommandCompletionCode));
			if(StartPacketBit == 0) RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0xFF;
			continue;
		}
		
		//
		// Check response code and if it's not ERR_REST_IPMI_SUCCESS return with error
		//
		if(IpmiUsb->CommandCompletionCode != ERR_REST_IPMI_SUCCESS){
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error completion code.\nCompletion Code - %d\nResponse Size - %d", IpmiUsb->CommandCompletionCode, ResponseSizeTmp));
			ASSERT(IpmiUsb->CommandCompletionCode != ERR_REST_IPMI_INV_REQUEST_BYTES);
			if(IpmiUsb->CommandCompletionCode == ERR_REST_IPMI_BMC_MAX_COMMAND_SIZE) ErrorStatus = EFI_VOLUME_FULL;
			break;
		}
	
		RequestParams->PacketID.PacketIDBits.ResendPacketBit = 0x00;

		//
		// Success 
		//
		*ResponseSize = ResponseSizeTmp;
		return EFI_SUCCESS;
	}
	
	return ErrorStatus;
}


//
//
//



EFI_STATUS GetMaxRestIpmiPacketSize(
	IN IPMI_USB_TRANSPORT *IpmiUsb,
	OUT UINT32 *MaxSize)
{
	EFI_STATUS Status;
	UINT32 ResponseSize;
	REST_IPMI_REQUEST_PARAMS RestIpmiRequest = { 0 };
	REST_IPMI_RESPONSE_PARAMS RestIpmiResponse = { 0 };


	RestIpmiRequest.PacketID.RawData = 0x00;
	RestIpmiRequest.PacketID.PacketIDBits.StartPacketBit = 0xff;
	RestIpmiRequest.PacketID.PacketIDBits.EndPacketBit = 0xff;
	ResponseSize = sizeof(REST_IPMI_RESPONSE_PARAMS);

	Status = SendRestIpmiCommand(IpmiUsb, 
			(UINT8*)&RestIpmiRequest,
			sizeof(RestIpmiRequest),
			(UINT8*)&RestIpmiResponse,
			&ResponseSize);

	if (EFI_ERROR(Status)) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: GetMaxRestIpmiPacketSize Error - %r\n", Status));
		return Status;
	}

	if (IpmiUsb->CommandCompletionCode != ERR_REST_IPMI_SUCCESS){
		return EFI_DEVICE_ERROR;
	}

	*MaxSize = RestIpmiResponse.TotalSize;

	return EFI_SUCCESS;
}

EFI_STATUS
GetServiceTimeRestIpmiUsbCommand(
IN EFI_REST_PROTOCOL *This,
OUT EFI_TIME *Time
)
{
	EFI_STATUS Status;
	Status = EFI_UNSUPPORTED;
	return Status;
}

/**
 USB interface REST send command Implementation

 @param This - Pointer to IPMI USB transport PPI/Protocol
 @param NetFunction - Net function of the command
 @param Lun - Logical Unit number of the command
 @param Command - Command to be sent to BMC
 @param CommandData - Command data to be sent along with Command
 @param CommandDataSize - Command Data size
 @param ResponseData - Response Data from BMC
 @param ResponseDataSize - Size of the Response data

 @return EFI_STATUS Status returned from BMC while executing the command.

 **/

EFI_STATUS
SendRecieveRestIpmiUsbCommand(
IN EFI_REST_PROTOCOL *This,
IN EFI_HTTP_MESSAGE *RequestMessage,
OUT EFI_HTTP_MESSAGE *ResponseMessage
)
{
	EFI_STATUS Status;
	REST_IPMI_USB_INSTANCE_DATA *RestIpmiUsbInstance;
	IPMI_USB_TRANSPORT *IpmiUsb = NULL;
	REST_IPMI_REQUEST_PARAMS *RequestParams, RequestParamsBuffer;
	REST_IPMI_RESPONSE_PARAMS *ResponseParams, ResponseParamsBuffer;
	UINT8 TransactionID = 0;
	UINT8 *CommandDataCurrentPointer = NULL;
	UINT8 *CommandData = NULL;
	UINT32 CommandDataSize = 0, TotalSize = 0;
	UINT8 *ResponseData = NULL;
	UINT32 ResponseSize = 0;
	UINT8 *FullResponseData = NULL;
	UINT32 FullResponseSize = 0;
	UINT8 *ResponseDataCurrentPointer = NULL;
	UINT32 CurrentResponseSize = 0;
	BOOLEAN TransInterrupted = FALSE;
#if REST_TEST_CASE > 0
	REST_IPMI_REQUEST_PARAMS TestRequestParamsBuffer = {0};
	REST_IPMI_RESPONSE_PARAMS TestResponseParamsBuffer = {0};
	UINT32 TestRequestSize = 0;
	UINT32 TestResponseSize = 0;
#endif
	
	//
	// Get the pointer to RestIpmiUsbInstance from PPI
	//
	RestIpmiUsbInstance = INSTANCE_DATA_FROM_REST_IPMI_USB_TRANSPORT_THIS(This);
	SERVER_REST_DEBUG((EFI_D_INFO, "REST: SendRecieveRestIpmiUsbCommand\n"));

	//
	//  Check if error condition(IPMI protocol is not available in IPMI handle) is already
	//  found to be invalid. If so, return protocol error immediately.
	//
	if (gRestIpmiProtocolError == TRUE) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: gRestIpmiProtocolError == TRUE\n"));
	}
	if (RestIpmiUsbInstance == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: RestIpmiUsbInstance == NULL\n"));
	}
	if (RestIpmiUsbInstance->MaxRestIpmiPacketSize < 0) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: RestIpmiUsbInstance->MaxRestIpmiPacketSize = %d\n", RestIpmiUsbInstance->MaxRestIpmiPacketSize));
	}
	if (gRestIpmiProtocolError == TRUE || RestIpmiUsbInstance == NULL || 
			RestIpmiUsbInstance->MaxRestIpmiPacketSize < 0) {
		return EFI_PROTOCOL_ERROR;
	}

	//
	//  Check if IPMI USB protocol installed for BMC device is still available.
	//
#if AMI_IPMI_EMULATOR_IS_AVAILABLE == 0
	Status = gBS->HandleProtocol(RestIpmiUsbInstance->IpmiUsbHandle, &gEfiDxeIpmiUsbTransportProtocolGuid, (VOID **)&IpmiUsb);
#else
	Status = gBS->HandleProtocol(RestIpmiUsbInstance->IpmiUsbHandle, &gDxeRestIpmiEmulatorGuid, (VOID **)&IpmiUsb);
#endif

	if (EFI_ERROR(Status)) {

		gRestIpmiProtocolError = TRUE;

		//
		//  Bmc Usb device is disconnected, return Protocol Error
		//
		return EFI_PROTOCOL_ERROR;
	}

	//
	// Validate input parameters
	//
	if (!RequestMessage || !ResponseMessage) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Request or Response NULL\n"));
		return EFI_INVALID_PARAMETER;
	}


	//
	// Allocate memory for response 
	// Since we do not know how big response could be, max packet size should be used
	//
	ResponseSize = RestIpmiUsbInstance->MaxRestIpmiPacketSize + sizeof(REST_IPMI_RESPONSE_PARAMS);
	ResponseData = AllocateZeroPool(ResponseSize + 1);
	if (ResponseData == NULL) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error AllocateRuntimeZeroPool - %d\n", ResponseSize));
		return EFI_OUT_OF_RESOURCES;
	}
	
	//
	// Validate command data size and command data pointer
	// and copy the command data in REST request
	//

	Status = RestIpmiUsbRequest(RequestMessage, sizeof(REST_IPMI_REQUEST_PARAMS), &CommandData, &CommandDataSize);
	if (EFI_ERROR(Status)) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error RestIpmiUsbRequest - %r\n", Status));
		goto Exit;
	}

	if (CommandData == NULL || CommandDataSize == 0) {
		SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error CommandDataSize - %d\n", CommandDataSize));
		Status = EFI_INVALID_PARAMETER;
		goto Exit;
	}
	
	// 
	// Send request data to BMC
	//
	CommandDataCurrentPointer = CommandData;
	CommandDataSize -= sizeof(REST_IPMI_REQUEST_PARAMS);
	TotalSize = CommandDataSize;

	//
	// Set request parameters of the command
	//
	RequestParams = (REST_IPMI_REQUEST_PARAMS*)CommandDataCurrentPointer;
	RequestParams->PacketID.RawData = 0x00;
	RequestParams->PacketID.PacketIDBits.StartPacketBit = 0xff;

	do {
		//
		// Set request parameters of the command
		//
		RequestParams->TotalSize = TotalSize;
		RequestParams->TransactionID = TransactionID;

		if (CommandDataSize > RestIpmiUsbInstance->MaxRestIpmiPacketSize) {
			RequestParams->PacketSize = RestIpmiUsbInstance->MaxRestIpmiPacketSize;
		}
		else {
			RequestParams->PacketSize = CommandDataSize;
			RequestParams->PacketID.PacketIDBits.EndPacketBit = 0xff;
		}

		//
		// Send REST command to BMC
		//
		Status = SendRestIpmiCommand(IpmiUsb, 
				CommandDataCurrentPointer,
				RequestParams->PacketSize + sizeof(REST_IPMI_REQUEST_PARAMS),
				ResponseData,
				&ResponseSize);
		if (EFI_ERROR(Status)){
			Status = EFI_DEVICE_ERROR;
			goto Exit;
		}


		//
		// Get Transaction ID from BMC
		//
		ResponseParams = (REST_IPMI_RESPONSE_PARAMS*)ResponseData;
		if(TransactionID == 0){
			TransactionID = ResponseParams->TransactionID;
		}
		else if(TransactionID != ResponseParams->TransactionID){
			TransInterrupted = TRUE;
			Status = EFI_DEVICE_ERROR;
			goto Exit;
		}

		if(CommandDataSize > RestIpmiUsbInstance->MaxRestIpmiPacketSize) {
			if(ResponseSize != sizeof(REST_IPMI_RESPONSE_PARAMS)){
				TransInterrupted = TRUE;
				Status = EFI_DEVICE_ERROR;
				goto Exit;
			}
			
			//
			// Move pointer to other block of data within payload
			//
			
			CommandDataCurrentPointer += RestIpmiUsbInstance->MaxRestIpmiPacketSize;
			RequestParams = (REST_IPMI_REQUEST_PARAMS*)CommandDataCurrentPointer;
			RequestParams->PacketID.RawData = 0x00;

			CommandDataSize -= RestIpmiUsbInstance->MaxRestIpmiPacketSize;
			ResponseSize = RestIpmiUsbInstance->MaxRestIpmiPacketSize + sizeof(REST_IPMI_RESPONSE_PARAMS);

		}

	} while (RequestParams->PacketID.PacketIDBits.EndPacketBit == 0x00);
	
	
	FreePool(CommandData);
	CommandData = NULL;


	// 
	// Receive response data from BMC
	//
	if (ResponseParams->PacketID.PacketIDBits.EndPacketBit == 0x00) {
		
		
#if REST_TEST_CASE == 1
		TestRequestSize = sizeof(REST_IPMI_REQUEST_PARAMS);
		TestResponseSize = sizeof(REST_IPMI_RESPONSE_PARAMS);
		TestRequestParamsBuffer.TotalSize = 0;
		TestRequestParamsBuffer.PacketSize = 0;
		TestRequestParamsBuffer.PacketID.RawData = 0x00;
		TestRequestParamsBuffer.PacketID.PacketIDBits.InterruptTransactionBit = 0xFF;
		TestRequestParamsBuffer.TransactionID = 0;
		TestRequestParamsBuffer.CheckSum = 0;
		Status = SendRestIpmiCommand(IpmiUsb, 
				(UINT8*)&TestRequestParamsBuffer,
				TestRequestSize,
				(UINT8*)&TestResponseParamsBuffer,
				&TestResponseSize);
		if (EFI_ERROR(Status)){
			SERVER_REST_DEBUG((EFI_D_INFO, "REST_TEST_CASE_1: Error Status code - %r\n", Status));
			goto Exit;
		}
#endif
#if REST_TEST_CASE == 2
		TestRequestSize = sizeof(REST_IPMI_REQUEST_PARAMS);
		TestResponseSize = sizeof(REST_IPMI_RESPONSE_PARAMS);
		TestRequestParamsBuffer.TotalSize = 0;
		TestRequestParamsBuffer.PacketSize = 0;
		TestRequestParamsBuffer.PacketID.RawData = 0x00;
		TestRequestParamsBuffer.PacketID.PacketIDBits.InterruptTransactionBit = 0xFF;
		TestRequestParamsBuffer.TransactionID = TransactionID + 1;
		TestRequestParamsBuffer.CheckSum = 0;
		Status = SendRestIpmiCommand(IpmiUsb, 
				(UINT8*)&TestRequestParamsBuffer,
				TestRequestSize,
				(UINT8*)&TestResponseParamsBuffer,
				&TestResponseSize);
		if (EFI_ERROR(Status)){
			SERVER_REST_DEBUG((EFI_D_INFO, "REST_TEST_CASE_2: Error Status code - %r\n", Status));
			goto Exit;
		}
#endif
		
		// 
		// Not all data has been received during last send command
		// Retrieve rest of data from BMC
		// First packet already has been received
		//

		if (CurrentResponseSize + (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS)) > ResponseParams->TotalSize) {
			TransInterrupted = TRUE;
			Status = EFI_DEVICE_ERROR;
			goto Exit;
		}

		FullResponseSize = ResponseParams->TotalSize;

		FullResponseData = AllocateZeroPool(FullResponseSize + 1);
		if (FullResponseData == NULL) {
			TransInterrupted = TRUE;
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error AllocateRuntimeZeroPool - %d\n", FullResponseSize));
			Status = EFI_OUT_OF_RESOURCES;
			goto Exit;
		}

		ResponseDataCurrentPointer = FullResponseData;
		CopyMem(ResponseDataCurrentPointer, ResponseData + sizeof(REST_IPMI_RESPONSE_PARAMS), ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS));
		ResponseDataCurrentPointer += (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS));
		CurrentResponseSize += (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS));


		RequestParamsBuffer.TotalSize = 0;
		RequestParamsBuffer.PacketSize = 0;
		RequestParamsBuffer.PacketID.RawData = 0x00;
		RequestParamsBuffer.TransactionID = TransactionID;
		RequestParamsBuffer.CheckSum = 0;
		do{

			ResponseSize = RestIpmiUsbInstance->MaxRestIpmiPacketSize + sizeof(REST_IPMI_RESPONSE_PARAMS);
			//
			// Send REST command to BMC
			//
			Status = SendRestIpmiCommand(IpmiUsb, 
					(UINT8*)&RequestParamsBuffer,
					sizeof(RequestParamsBuffer),
					ResponseData,
					&ResponseSize);
			if (EFI_ERROR(Status)){
				SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error Status code - %r\n", Status));
				goto Exit;
			}

			
			if (CurrentResponseSize + (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS)) > FullResponseSize) {
				SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error with current response size CurrentResponseSize-%d, FullResponseSize-%d\n", CurrentResponseSize + (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS)), FullResponseSize));
				TransInterrupted = TRUE;
				Status = EFI_DEVICE_ERROR;
				goto Exit;
			}
			//
			// Get Transaction ID from BMC
			//
			ResponseParams = (REST_IPMI_RESPONSE_PARAMS*)ResponseData;
			if(TransactionID != ResponseParams->TransactionID){
				SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error TransactionID TransactionID-%d, ResponseParams->TransactionID-%d\n", TransactionID, ResponseParams->TransactionID));
				TransInterrupted = TRUE;
				Status = EFI_DEVICE_ERROR;
				goto Exit;
			}
			
			CopyMem(ResponseDataCurrentPointer, ResponseData + sizeof(REST_IPMI_RESPONSE_PARAMS), ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS));
			ResponseDataCurrentPointer += (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS));
			CurrentResponseSize += (ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS));

		}while(ResponseParams->PacketID.PacketIDBits.EndPacketBit != 0x01);
		
		FreePool(ResponseData);
		ResponseData = NULL;

		if(CurrentResponseSize != FullResponseSize){
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error size received CurrentResponseSize-%d, FullResponseSize-%d\n", CurrentResponseSize, FullResponseSize));
			TransInterrupted = TRUE;
			Status = EFI_DEVICE_ERROR;
			goto Exit;
		}

		
		ResponseDataCurrentPointer = FullResponseData;
		CurrentResponseSize = FullResponseSize;
		

	}
	else {
		// 
		// All data has been received during last send command
		//
		if (ResponseSize <= sizeof(REST_IPMI_RESPONSE_PARAMS) ||
				ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS) != ResponseParams->TotalSize) {
			SERVER_REST_DEBUG((EFI_D_ERROR, "REST: Error ResponseSize -%d, TotalSize-%d\n", ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS), ResponseParams->TotalSize));
			TransInterrupted = TRUE;
			Status = EFI_DEVICE_ERROR;
			goto Exit;
		}
		
		ResponseDataCurrentPointer = ResponseData + sizeof(REST_IPMI_RESPONSE_PARAMS);
		CurrentResponseSize = ResponseSize - sizeof(REST_IPMI_RESPONSE_PARAMS);
	
	}

	
	//
	// Send finish transaction confirmation
	// Set parameters. EndTransactionBit should be set in order to notify BMC about end of transaction.
	//
	RequestParamsBuffer.TotalSize = 0;
	RequestParamsBuffer.PacketSize = 0;
	RequestParamsBuffer.PacketID.RawData = 0x00;
	RequestParamsBuffer.PacketID.PacketIDBits.EndTransactionBit = 0xFF;
	RequestParamsBuffer.TransactionID = TransactionID;
	RequestParamsBuffer.CheckSum = 0;
	
	ResponseSize = sizeof(ResponseParamsBuffer);
	//
	// Send Confirmation REST command to BMC
	//
	Status = SendRestIpmiCommand(IpmiUsb, 
			(UINT8*)&RequestParamsBuffer,
			sizeof(RequestParamsBuffer),
			(UINT8*)&ResponseParamsBuffer,
			&ResponseSize);

	if(!EFI_ERROR(Status)){
		//
		// Prepare Response message
		//
		Status = RestIpmiUsbResponse(ResponseDataCurrentPointer, CurrentResponseSize, ResponseMessage);
	}
	
    Exit:
    	if(TransInterrupted && TransactionID != 0){
    		//
    		// Send finish transaction confirmation
    		// Set parameters. EndTransactionBit should be set in order to notify BMC about end of transaction.
    		//
    		RequestParamsBuffer.TotalSize = 0;
    		RequestParamsBuffer.PacketSize = 0;
    		RequestParamsBuffer.PacketID.RawData = 0x00;
    		RequestParamsBuffer.PacketID.PacketIDBits.InterruptTransactionBit = 0xFF;
    		RequestParamsBuffer.TransactionID = TransactionID;
    		RequestParamsBuffer.CheckSum = 0;
    		
    		ResponseSize = sizeof(ResponseParamsBuffer);
    		//
    		// Send Interruption command to BMC
    		//
    		SendRestIpmiCommand(IpmiUsb, 
    				(UINT8*)&RequestParamsBuffer,
    				sizeof(RequestParamsBuffer),
    				(UINT8*)&ResponseParamsBuffer,
    				&ResponseSize);
    	}
	if(ResponseData != NULL){
		FreePool(ResponseData);
		ResponseData = NULL;
	}
	if(FullResponseData != NULL){
		FreePool(FullResponseData);
		FullResponseData = NULL;
	}
	if(CommandData != NULL){
		FreePool(CommandData);
		CommandData = NULL;
	}
	SERVER_REST_DEBUG((EFI_D_INFO, "REST: Finished\n"));
	
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
