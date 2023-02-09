//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file FirmwareConfigDrvSrc.c
    Firmware Configuration source driver

**/

//---------------------------------------------------------------------------

#include "Efi.h"
#include "AmiDxeLib.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <Library/DebugLib.h>
#include "Library/UefiLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Library/BaseLib.h"
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/IpmiUsbTransportProtocol.h>
#include "Token.h"
#include "../FirmwareConfigDrvSrc.h"
#include "../../ConfigurationDriver/FirmwareConfigDrvBin.h"
#include "BiosAttribTypeLib.h"
#include "SetupDataXmlInterface.h"
#include <JsonUtilityLib.h>
#include "Library/AmiJsonLib.h" //<kuaibai-20190326 improvement of OOB files process>
extern EFI_GUID gSetupInfoProtocolGuid;

EFI_STATUS InitBmcInstance()
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_STATUS OverAllStatus = EFI_NOT_FOUND;
  UINTN HandleCount = 0;
  UINTN iIndex = 0;
  EFI_HANDLE *Handles = NULL;
  EFI_REST_PROTOCOL *RestInstance = (EFI_REST_PROTOCOL *)NULL;
  CHAR16 *RequestStr16 = (CHAR16 *)NULL;
  void *DxeIpmiUsbInstance = NULL;

  Status = pBS->LocateHandleBuffer(ByProtocol, &FwcgEfiRestProtocolGuid, NULL, &HandleCount, &Handles);
  if (!EFI_ERROR(Status))
  {
    for (iIndex = 0; iIndex < HandleCount; iIndex++)
    {
      Status = pBS->HandleProtocol(Handles[iIndex], &FwcgEfiRestProtocolGuid, &RestInstance);
      if (!EFI_ERROR(Status))
      {
        Status = pBS->HandleProtocol(Handles[iIndex], &gEfiDxeIpmiUsbTransportProtocolGuid, (void **)&DxeIpmiUsbInstance);
        if (!EFI_ERROR(Status))
        {
          gProtocolHandle = Handles[iIndex]; //<kuaibai-20181222-fix hang b2 when ipmi abnormal>
          gRestInstance = RestInstance;
          OverAllStatus = EFI_SUCCESS;
          break;
        }
      }
    }
    FreePool(Handles);
  }
  return OverAllStatus;
}

EFI_STATUS SendBMCGetRequest(CHAR8 *RequestStr, UINT32 RequestStrLen, VOID **Response, UINTN *ResponseLength)
{
  EFI_STATUS Status;
  CHAR8 *EndofHeader;
  CHAR8 *HttpHeaders;
  UINTN SizeofHeaders;
  CHAR8 *Tmp, *UrlPtr;
  CHAR8 *HeaderTmp;
  INTN ResCmp;
  //	UINT32 RequestStrLen = 0;
  EFI_HTTP_MESSAGE *HttpRequest = (EFI_HTTP_MESSAGE *)NULL;
  EFI_HTTP_UTILITIES_PROTOCOL *HttpUtilities = (EFI_HTTP_UTILITIES_PROTOCOL *)NULL;
  EFI_HTTP_MESSAGE ResponseMessage;
  //<kuaibai-20181222-fix hang b2 when ipmi abnormal+>
  Status = pBS->HandleProtocol(gProtocolHandle, &FwcgEfiRestProtocolGuid, &gRestInstance); //Make sure the handle is always available
  if (EFI_ERROR(Status))
  {
    return EFI_DEVICE_ERROR;
  }
  //<kuaibai-20181222-fix hang b2 when ipmi abnormal->
  //	if (NULL != RequestStr)
  //		RequestStrLen = (UINT32)Strlen(RequestStr);
  Status = pBS->LocateProtocol(&FwcgEfiHttpUtilitiesProtocolGuid, NULL,
                               (VOID **)&HttpUtilities);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "Locate HttpUtilities protocol Error %r\n", Status));
    return Status;
  }
  DEBUG((DEBUG_ERROR, "\nProtocols success"));

  HttpRequest = AllocatePool(sizeof(EFI_HTTP_MESSAGE));
  if (HttpRequest == NULL)
  {
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
  if (HttpRequest->Data.Request == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  //
  // Search for HTTP Version.
  //
  Tmp = RequestStr;
  ResCmp = AsciiStrnCmp(Tmp, HTTP_GET_STR, AsciiStrLen(HTTP_GET_STR));
  if (ResCmp == 0)
  {
    HttpRequest->Data.Request->Method = HttpMethodGet;
    UrlPtr = Tmp + AsciiStrLen(HTTP_GET_STR);
    goto Crlfparse;
  }
  ResCmp = AsciiStrnCmp(Tmp, HTTP_POST_STR, AsciiStrLen(HTTP_POST_STR));
  if (ResCmp == 0)
  {
    HttpRequest->Data.Request->Method = HttpMethodPost;
    UrlPtr = Tmp + AsciiStrLen(HTTP_POST_STR);
    goto Crlfparse;
  }
  ResCmp = AsciiStrnCmp(Tmp, HTTP_PUT_STR, AsciiStrLen(HTTP_PUT_STR));
  if (ResCmp == 0)
  {
    HttpRequest->Data.Request->Method = HttpMethodPut;
    UrlPtr = Tmp + AsciiStrLen(HTTP_PUT_STR);
    goto Crlfparse;
  }
  ResCmp = AsciiStrnCmp(Tmp, HTTP_PATCH_STR, AsciiStrLen(HTTP_PATCH_STR));
  if (ResCmp == 0)
  {
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
  if (Tmp == NULL)
  {
    Status = EFI_NOT_FOUND;
    goto Error;
  }

  *(Tmp - 1) = '\0';

  HttpRequest->Data.Request->Url = AllocateZeroPool((AsciiStrLen(UrlPtr) + 1) * sizeof(CHAR16));
  if (HttpRequest->Data.Request->Url == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  AsciiStrToUnicodeStr(UrlPtr, HttpRequest->Data.Request->Url);

  Tmp = AsciiStrStr(Tmp, HTTP_CRLF_STR);
  if (Tmp == NULL)
  {
    Status = EFI_NOT_FOUND;
    goto Error;
  }
  HeaderTmp = Tmp + AsciiStrLen(HTTP_CRLF_STR);

  ResponseMessage.Data.Response = NULL;

  ResponseMessage.Data.Response = AllocatePool(sizeof(EFI_HTTP_RESPONSE_DATA));
  if (ResponseMessage.Data.Response == NULL)
  {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  if (RequestStrLen - (HeaderTmp - RequestStr) == 0)
  {
    HttpRequest->Body = NULL;
    HttpRequest->BodyLength = 0;
    HttpRequest->Headers = NULL;
    HttpRequest->HeaderCount = 0;
    Status = gRestInstance->SendReceive(gRestInstance, HttpRequest, &ResponseMessage);
    DEBUG((DEBUG_ERROR, "\nGet RestInstance->SendReceiv Status - %r,Size:%d\n", Status, ResponseMessage.BodyLength));
    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "Send Command Error %r\n", Status));
      goto Error;
    }

    *Response = ResponseMessage.Body;
    *ResponseLength = ResponseMessage.BodyLength;

    Status = EFI_SUCCESS;
    goto Error;
  }

  EndofHeader = AsciiStrStr(HeaderTmp, HTTP_END_OF_HDR_STR);
  if (EndofHeader == NULL)
  {
    Status = EFI_NOT_FOUND;
    goto Error;
  }

  SizeofHeaders = EndofHeader - HeaderTmp + 2;

  DEBUG((DEBUG_ERROR, "Size = %d, Headers\n%a\n", SizeofHeaders, HeaderTmp));

  //
  // Parse the HTTP header into array of key/value pairs.
  //
  Status = HttpUtilities->Parse(HttpUtilities, HeaderTmp, SizeofHeaders,
                                &HttpRequest->Headers, &HttpRequest->HeaderCount);
  if (EFI_ERROR(Status))
  {
    DEBUG((DEBUG_ERROR, "\nREST: Http Parse Error - %r\n", Status));
    goto Error;
  }

  //
  // Receive the response body.
  //
  HttpRequest->BodyLength = RequestStrLen - (EndofHeader - RequestStr) - AsciiStrLen(HTTP_END_OF_HDR_STR);
  if (HttpRequest->BodyLength > 0)
  {

    HttpRequest->Body = AllocatePool(HttpRequest->BodyLength);
    if (HttpRequest->Body == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      goto Error;
    }
    CopyMem(HttpRequest->Body, EndofHeader + AsciiStrLen(HTTP_END_OF_HDR_STR),
            HttpRequest->BodyLength);

    DEBUG((DEBUG_ERROR, "Send HttpRequest->BodyLength:%d\n", HttpRequest->BodyLength));

    Status = gRestInstance->SendReceive(gRestInstance, HttpRequest, &ResponseMessage);
    DEBUG((DEBUG_ERROR, "\nREST: POST/PUT RestInstance->SendReceiv Status - %r,Size:%d\n", Status, ResponseMessage.BodyLength));

    if (EFI_ERROR(Status))
    {
      DEBUG((DEBUG_ERROR, "Send Command Error %r\n", Status));
      goto Error;
    }

    *Response = ResponseMessage.Body;
    *ResponseLength = ResponseMessage.BodyLength;
  }
  else
  {
    HttpRequest->Body = NULL;
  }

Error:

  if (HttpRequest->Data.Request->Url != NULL)
  {
    FreePool(HttpRequest->Data.Request->Url);
  }
  if (HttpRequest->Data.Request != NULL)
  {
    FreePool(HttpRequest->Data.Request);
  }
  if (HttpRequest->Headers != NULL)
  {
    FreePool(HttpRequest->Headers);
  }
  if (HttpRequest->Body != NULL)
  {
    FreePool(HttpRequest->Body);
  }
  if (HttpRequest != NULL)
  {
    FreePool(HttpRequest);
  }

  return Status;
}

/**
    GETs the bios index sd data from server
     
    @param JsonBuffer buffer read from the server
    @param BufferSize size of the buffer read

    @retval EFI_STATUS
     
**/
EFI_STATUS GetBiosSdData(CHAR8 **JsonBuffer, UINTN *BufferSize)
{
  CHAR8 GetUrlPath[128] = {0};
  EFI_STATUS Status = EFI_SUCCESS;
  //<xuyj1001-20200401 Add BIOS OOB Function +>
  Sprintf(GetUrlPath, "GET /redfish/v1/Systems/Self/Bios/SD HTTP/1.1\r\n");
  //<xuyj1001-20200401 Add BIOS OOB Function ->
  DEBUG((DEBUG_ERROR, "\nGetUrlPath :%a", GetUrlPath));
  Status = SendBMCGetRequest(GetUrlPath, (UINT32)Strlen(GetUrlPath), (void **)JsonBuffer, BufferSize); //Systems/BIOS/SD/index.json
  DEBUG((DEBUG_ERROR, "\nSendBMCGetRequest Status:%x,:%x,Size:%d", Status, JsonBuffer, BufferSize));

  return Status;
}

/**
    GETs the bios index data from server
     
    @param JsonBuffer buffer read from the server
    @param BufferSize size of the buffer read

    @retval EFI_STATUS
     
**/
EFI_STATUS GetBiosIndexData(CHAR8 **JsonBuffer, UINTN *BufferSize)
{
  CHAR8 GetUrlPath[128] = {0};
  EFI_STATUS Status = EFI_SUCCESS;
  //<xuyj1001-20200401 Add BIOS OOB Function +>
  Sprintf(GetUrlPath, "GET /redfish/v1/Systems/Self/Bios HTTP/1.1\r\n");
  //<xuyj1001-20200401 Add BIOS OOB Function ->
  DEBUG((DEBUG_ERROR, "\nGetUrlPath :%a", GetUrlPath));
  Status = SendBMCGetRequest(GetUrlPath, (UINT32)Strlen(GetUrlPath), (void **)JsonBuffer, BufferSize); //Systems/BIOS/SD/index.json
  DEBUG((DEBUG_ERROR, "\nSendBMCGetRequest Status:%x,:%x,Size:%d", Status, JsonBuffer, BufferSize));

  return Status;
}

/**
    POSTs the bios index data to the server
     
    @param PostData buffer to post to the server
    @param BufferSize size of the buffer to post

    @retval EFI_STATUS
     
**/
EFI_STATUS PostBiosIndexData(CHAR8 *PostData, UINTN PostDataLength)
{
  CHAR8 GetUrlPath[128] = {0};
  CHAR8 *RequestStr = NULL;
  CHAR8 *Response = (CHAR8 *)NULL;
  UINTN ResponseLength = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  RequestStr = (CHAR8 *)AllocateZeroPool(PostDataLength + 128);
  if (NULL == RequestStr)
  {
    return EFI_OUT_OF_RESOURCES;
  }
  //<xuyj1001-20200401 Add BIOS OOB Function +>
  Sprintf(RequestStr,
          "POST /redfish/v1/Systems/Self/Bios HTTP/1.1\r\n"
          "Content-length: %d\r\n"
          "\r\n%s",
          PostDataLength, PostData);
  //<xuyj1001-20200401 Add BIOS OOB Function ->
  Status = SendBMCGetRequest(RequestStr, (UINT32)Strlen(RequestStr), (VOID **)&Response, &ResponseLength);
  DEBUG((DEBUG_ERROR, "\nPOST SendBMCGetRequest Status:%x", Status));
  if (ResponseLength && (UINT8)GetFw_ManualJsonCreation())
    Dump_ContentToFile(FALSE, L"BMCPutResponse.json", Response, ResponseLength);
  if (RequestStr)
    FreePool(RequestStr);

  return Status;
}

//<kuaibai-20190326 improvement of OOB files process +>
/**
  This procedure sends the OEM IPMI Command and provide the output.

  @param UINT8 NetFunction
  @param UINT8 Lun
  @param UINT8 Command
  @param UINT8 *CommandData
  @param UINT32 CommandDataSize
  @param UINT8 **ResponseData
  @param UINT32 *ResponseDataSize
  @param UINT8 *CommandCompletionCode

  @retval EFI_STATUS
**/
EFI_STATUS goIpmiUsbCommand(
    UINT8 NetFunction, UINT8 Lun, UINT8 Command,
    UINT8 *CommandData, UINT32 CommandDataSize,
    UINT8 **ResponseData, UINT32 *ResponseDataSize,
    UINT8 *CommandCompletionCode)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_GUID tEfiIpmiUsbGuid = EFI_IPMI_USB_TRANSPORT_PROTOCOL_GUID;
  IPMI_USB_TRANSPORT *pIpmiUsbTransportService;
  UINT8 *pTempResponse;
  UINT32 TempSize;

  if (ResponseDataSize == NULL)
    TempSize = 0;
  else
    TempSize = *ResponseDataSize;

  //Locate the IPMI USB Protocol
  Status = pBS->LocateProtocol(&tEfiIpmiUsbGuid, NULL, &pIpmiUsbTransportService);
  DEBUG((DEBUG_ERROR, "[SK] goIpmiUsbCommand LocateProtocol tEfiIpmiUsbGuid Status: %r\n", Status));

  if (EFI_SUCCESS == Status)
  {
    DEBUG((DEBUG_ERROR, "[SK] goIpmiUsbCommand TempSize: %x\n", TempSize));
    pTempResponse = (UINT8 *)AllocatePool(TempSize);

    if (NULL == pTempResponse)
    {
      DEBUG((DEBUG_ERROR, "[SK] NULL == pTempResponse\n"));
      return EFI_BUFFER_TOO_SMALL;
    }
    else
    {
      Status = pIpmiUsbTransportService->SendOemIpmiCommand(pIpmiUsbTransportService,
                                                            NetFunction, Lun, Command,
                                                            CommandData, CommandDataSize,
                                                            pTempResponse, &TempSize);
      DEBUG((DEBUG_ERROR, "[SK] 0 goIpmiUsbCommand SC: %r, CC: %x\n", Status, pIpmiUsbTransportService->CommandCompletionCode));
      *CommandCompletionCode = pIpmiUsbTransportService->CommandCompletionCode;

      if (EFI_BUFFER_TOO_SMALL == Status)
      {
        pTempResponse = (UINT8 *)AllocatePool(TempSize);
        if (NULL == pTempResponse)
        {
          DEBUG((DEBUG_ERROR, "[SK] NULL == pTempResponse\n"));
          Status = EFI_BUFFER_TOO_SMALL;
        }
        else
        {
          Status = pIpmiUsbTransportService->SendOemIpmiCommand(pIpmiUsbTransportService,
                                                                NetFunction, Lun, Command,
                                                                CommandData, CommandDataSize,
                                                                pTempResponse, &TempSize);
          DEBUG((DEBUG_ERROR, "[SK] 1 goIpmiUsbCommand SC: %r, CC: %x\n", Status, pIpmiUsbTransportService->CommandCompletionCode));
          *CommandCompletionCode = pIpmiUsbTransportService->CommandCompletionCode;
        }
      }

      if (EFI_SUCCESS == Status)
      {
        if (0 != TempSize)
        {
          *ResponseDataSize = TempSize;
          *ResponseData = (UINT8 *)AllocatePool(TempSize);

          if (NULL == *ResponseData)
          {
            DEBUG((DEBUG_ERROR, "[SK] NULL == *ResponseData\n"));
            Status = EFI_BUFFER_TOO_SMALL;
          }
          else
            CopyMem(*ResponseData, pTempResponse, *ResponseDataSize);
        }
        else
          *ResponseDataSize = 0;
      }
      if (pTempResponse)
        FreePool(pTempResponse);
    }
  }
  return Status;
}

/**
  Computes and returns a 32-bit CRC for a data buffer.
  CRC32 value bases on ITU-T V.42.

  If Buffer is NULL, then ASSERT().
  If Length is greater than (MAX_ADDRESS - Buffer + 1), then ASSERT().

  @param[in]  Buffer       A pointer to the buffer on which the 32-bit CRC is to be computed.
  @param[in]  Length       The number of bytes in the buffer Data.

  @retval Crc32            The 32-bit CRC was computed for the data buffer.

**/
UINT32
EFIAPI
CalcCrc32(
    IN VOID *Buffer,
    IN UINTN Length)
{
  UINTN Index;
  UINT32 Crc;
  UINT8 *Ptr;

  ASSERT(Buffer != NULL);
  ASSERT(Length <= (MAX_ADDRESS - ((UINTN)Buffer) + 1));

  //
  // Compute CRC
  //
  Crc = 0xffffffff;
  for (Index = 0, Ptr = Buffer; Index < Length; Index++, Ptr++)
  {
    Crc = (Crc >> 8) ^ tCrcTable[(UINT8)Crc ^ *Ptr];
  }

  return Crc ^ 0xffffffff;
}

/**
    Send the bios data to BMC through IPMIUSB.
     
    @param CHAR8  *Data       buffer to send to BMC
    @param UINTN  DataLength  size of the buffer to send
    @param CHAR8  *fileType   type of the file to send

    @retval EFI_STATUS
**/
EFI_STATUS
SendBiosRfData(
    CHAR8 *Data,
    UINTN DataLength,
    CHAR8 *fileType)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8 *DataBuffer = Data;
  UINT8 *CommandData = NULL;
  UINT16 CommandSize = 0;
  UINT8 CommandCompletionCode = 0;
  UINT8 *IpmiResponseData = NULL;
  UINT32 IpmiResponseSize = 0x5;
  UINT32 TotalSize = (UINT32)DataLength;
  UINT16 PacketSize = 0;
  UINT32 BytesTransferred = 0;
  BOOLEAN CommandFinish = FALSE;
  UPLOAD_FILE_PAYLOAD_REQ SendRequest;
  CHAR8 fileName[MAX_FILENAME_SIZE] = ""; // NOTICE! the limit of "fileName" is 64 bytes.

  if (!AsciiStrCmp(fileType, "Attribute"))
  {
    DEBUG((DEBUG_INFO, "\n[SK] SendBiosRfData: Attribute"));
    Sprintf(fileName,
            "/tmp/BiosAttributeRegistry%s.%d.%d.%d.json",
            (CHAR8 *)Get_ProjectTag(), (int)Get_ProjectMajorVersion(), (int)Get_ProjectMinorVersion(), (int)Get_ProjectBuildVersion());
  }
  else if (!AsciiStrCmp(fileType, "Stripped"))
  {
    DEBUG((DEBUG_INFO, "\n[SK] SendBiosRfData: Stripped"));
    Sprintf(fileName,
            "/tmp/BiosAttributeRegistry%s.%d.%d.%d.json_stripped",
            (CHAR8 *)Get_ProjectTag(), (int)Get_ProjectMajorVersion(), (int)Get_ProjectMinorVersion(), (int)Get_ProjectBuildVersion());
  }
  else if (!AsciiStrCmp(fileType, "Index"))
  {
    DEBUG((DEBUG_INFO, "\n[SK] SendBiosRfData: Index"));
    Sprintf(fileName, "/conf/redfish/bios/bios_current_settings.json");
  }
  else
  {
    DEBUG((DEBUG_INFO, "\n[SK] SendBiosRfData: invalid file type!"));
    return EFI_UNSUPPORTED;
  }

  DEBUG((DEBUG_INFO, "\n[SK] SendBiosRfData: TotalSize=%x\n", TotalSize));

  //Check total size and update upload flag, note that the final packet's flag is 0x01
  if (TotalSize > TRANS_SIZE_PER_REQ_30KB)
  {
    PacketSize = TRANS_SIZE_PER_REQ_30KB;
    SendRequest.UploadFlag = 0x00; //normal packets
  }
  else
  {
    PacketSize = (UINT16)TotalSize;
    SendRequest.UploadFlag = 0x01; //final packet
  }

  //construct request header, offset is 0x0 in the beginning
  SendRequest.Offset = BytesTransferred;
  SendRequest.size = PacketSize;
  SendRequest.CheckSum = CalcCrc32(DataBuffer, PacketSize);
  memcpy(SendRequest.FileName, fileName, sizeof(fileName));

  //construct command data
  CommandSize = sizeof(UPLOAD_FILE_PAYLOAD_REQ) + SendRequest.size;
  CommandData = (UINT8 *)calloc(CommandSize, (sizeof(UINT8)));
  if (CommandData == NULL)
  {
    DEBUG((DEBUG_ERROR, "[SK] SendBiosRfData: out of memory!\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  memcpy(CommandData, &SendRequest, sizeof(UPLOAD_FILE_PAYLOAD_REQ));
  memcpy(CommandData + sizeof(UPLOAD_FILE_PAYLOAD_REQ), DataBuffer, PacketSize);
  DEBUG((DEBUG_INFO, "[SK] SendBiosRfData: size=%x, UploadFlag=%x, CheckSum=%x\n", SendRequest.size, SendRequest.UploadFlag, SendRequest.CheckSum));

  //loop to send IPMI command, CMD 0x2F: Upload files to BMC through IPMIUSB
  do
  {
    DEBUG((DEBUG_INFO, "[SK] SendBiosRfData in DOWHILE: BytesTransferred=%x\n", BytesTransferred));
    Status = goIpmiUsbCommand(0x3A, 0, 0x2F, CommandData, (UINT32)CommandSize, &IpmiResponseData, &IpmiResponseSize, &CommandCompletionCode);
    DEBUG((DEBUG_INFO, "[SK] SendBiosRfData in DOWHILE: SC:%r, CC:%x, CommandSize:%x, IpmiResponseSize:%x\n", Status, CommandCompletionCode, CommandSize, IpmiResponseSize));
    if (EFI_ERROR(Status))
      return Status;

    if (0 == CommandCompletionCode)
    {
      if (SendRequest.UploadFlag)
      {
        DEBUG((DEBUG_ERROR, "[SK] SendRequest.UploadFlag = 0x01!\n"));
        CommandFinish = TRUE;
        break;
      }
      else
      {
        //check the remain size and update upload flag
        UINT32 RemainSize = 0;
        BytesTransferred += (UINT32)PacketSize;
        RemainSize = TotalSize - BytesTransferred;

        if (RemainSize > TRANS_SIZE_PER_REQ_30KB)
        {
          PacketSize = TRANS_SIZE_PER_REQ_30KB;
          SendRequest.UploadFlag = 0x0;
        }
        else
        {
          PacketSize = (UINT16)RemainSize;
          SendRequest.UploadFlag = 0x1;
        }

        //construct request header
        SendRequest.Offset = BytesTransferred;
        SendRequest.size = PacketSize;
        SendRequest.CheckSum = CalcCrc32(DataBuffer + BytesTransferred, PacketSize);
        memcpy(SendRequest.FileName, fileName, sizeof(fileName));

        //construct command data
        CommandSize = sizeof(UPLOAD_FILE_PAYLOAD_REQ) + SendRequest.size;
        CommandData = (UINT8 *)realloc(CommandData, CommandSize);
        if (CommandData == NULL)
        {
          DEBUG((DEBUG_ERROR, "[SK] SendBiosRfData in DOWHILE: CommandData == NULL\n"));
          Status = EFI_OUT_OF_RESOURCES;
          break;
        }
        memcpy(CommandData, &SendRequest, sizeof(UPLOAD_FILE_PAYLOAD_REQ));
        memcpy(CommandData + sizeof(UPLOAD_FILE_PAYLOAD_REQ), DataBuffer + BytesTransferred, PacketSize);
        DEBUG((DEBUG_INFO, "[SK] SendBiosRfData in DOWHILE: PacketSize=%x, UploadFlag=%d\n", PacketSize, SendRequest.UploadFlag));

        //free pool to go next loop
        if (IpmiResponseData)
          FreePool(IpmiResponseData);
      }
    }
    else
    {
      if (CommandCompletionCode)
      {
        DEBUG((DEBUG_INFO, "[SK] else CommandCompletionCode=%x\n", CommandCompletionCode));
      }
      if (CommandData)
        free(CommandData);
      if (IpmiResponseData)
        FreePool(IpmiResponseData);
    }

    if (EFI_ERROR(Status))
      return Status;
  } while (FALSE == CommandFinish);

  if (IpmiResponseData)
    FreePool(IpmiResponseData);

  return Status;
}

/**
  Hook to modify current value of the specific items.

  @param  CHAR8 *biosJsonString
  @retval VOID

**/
cJSON *Special_BiosSetupJsonCurrentValue(CHAR8 *biosJsonString)
{
  CHAR8 *JsonValueStr = (CHAR8 *)NULL;
  cJSON *jsonBuffer = NULL;
  cJSON *MapIds = NULL, *theMapId = NULL;
  cJSON *DefValueCase = NULL, *NewValue = NULL;

  jsonBuffer = cJSON_Parse(biosJsonString);

  if (jsonBuffer != NULL)
  {
    MapIds = cJSON_GetObjectItem(jsonBuffer, "Attributes");

    if (MapIds != NULL)
    {
      //Bootup NumLock State: Enumeration "on/off"
      theMapId = cJSON_GetObjectItem(MapIds, "SETUP004");
      /*      if (theMapId != NULL)
      {
        if (!AsciiStrCmp(theMapId->valuestring, "On"))
        {
          NewValue = cJSON_CreateString("Off");
          cJSON_ReplaceItemInObject(MapIds, "SETUP004", NewValue);
        }
        theMapId = NULL;
      }
      
      //Setup Prompt Timeout: Integer
      theMapId = cJSON_GetObjectItem(MapIds, "SETUP003");
      if (theMapId != NULL)
      {
        if (theMapId->valueint == 5)
        {
          NewValue = cJSON_CreateNumberInt(6);
          cJSON_ReplaceItemInObject(MapIds, "SETUP003", NewValue);
        }
        theMapId = NULL;
      }
      
      //Quiet Boot: Boolean
      theMapId = cJSON_GetObjectItem(MapIds, "SETUP005");
      if (theMapId != NULL)
      {
        NewValue = cJSON_CreateFalse();
        cJSON_ReplaceItemInObject(MapIds, "SETUP005", NewValue);
        theMapId = NULL;
      }*/
    }
    else
      DEBUG((DEBUG_ERROR, "[SK] Attributes get fail!\n"));
  }
  else
    DEBUG((DEBUG_ERROR, "[SK] jsonBuffer is NULL!\n"));

  return jsonBuffer;
}

/**
  Hook to modify attributes data of the specific items.

  @param  CHAR8 *biosJsonString
  @retval VOID

**/
cJSON *Special_BiosSetupJsonAttributeData(CHAR8 *biosJsonString)
{
  CHAR8 *JsonValueStr = (CHAR8 *)NULL;
  cJSON *jsonBuffer = NULL, *jsonEntrys = NULL, *AttrsArray = NULL, *ArrayMember = NULL, *AttrsIDCase = NULL;
  cJSON *DefValueCase = NULL, *NewDefValueCase = NULL;
  UINT32 ArraySize, i = 0;

  jsonBuffer = cJSON_Parse(biosJsonString);
  if (jsonBuffer != NULL)
  {
    jsonEntrys = cJSON_GetObjectItem(jsonBuffer, "RegistryEntries");
    if (jsonEntrys != NULL)
    {
      AttrsArray = cJSON_GetObjectItem(jsonEntrys, "Attributes");
      if (AttrsArray != NULL)
      {
        ArraySize = cJSON_GetArraySize(AttrsArray);

        // enumerate all items and analysis them
        for (i = 0; i < ArraySize; i++)
        {
          ArrayMember = cJSON_GetArrayItem(AttrsArray, i);
          if (ArrayMember == NULL)
            return NULL;

          AttrsIDCase = cJSON_GetObjectItem(ArrayMember, "AttributeName");
          if (AttrsIDCase != NULL)
          {
            if (!AsciiStrCmp(AttrsIDCase->valuestring, "MEMORY001"))
            {
              DefValueCase = cJSON_GetObjectItem(ArrayMember, "DefaultValue");
              if (!AsciiStrCmp(DefValueCase->valuestring, "-1"))
              {
                NewDefValueCase = cJSON_CreateString("Auto");
                cJSON_ReplaceItemInObject(ArrayMember, "DefaultValue", NewDefValueCase);
              }
            }
          }
          else
          {
            DEBUG((DEBUG_ERROR, "[SK]: AttrsIDCase fail:%d!\n", i));
            continue;
          }
        }
      }
      else
        DEBUG((DEBUG_ERROR, "[SK] AttrsArray is NULL!\n"));
    }
    else
      DEBUG((DEBUG_ERROR, "[SK] jsonEntrys is NULL!\n"));

    return jsonBuffer;
  }

  return (cJSON *)NULL;
}

/**
  Analysis Bios attribute json buffer then create the stripped json.

  @param  CHAR8 *biosJsonString
  @retval VOID

**/
cJSON *Analysis_BiosSetupJsonData(CHAR8 *biosJsonString)
{
  CHAR8 *JsonValueStr = (CHAR8 *)NULL;
  cJSON *jsonBuffer = NULL, *jsonEntrys = NULL, *AttrsArray = NULL, *ArrayMember = NULL, *AttrsIDCase = NULL;
  cJSON *TypeCase = NULL, *ValueCase = NULL, *ValuesMember = NULL, *ValueName = NULL;
  cJSON *MapRoot = NULL, *Items = NULL, *Values = NULL;
  cJSON *LowerBoundCase = NULL, *UpperBoundCase = NULL;
  cJSON *MinLengthCase = NULL, *MaxLengthCase = NULL;
  UINT32 ArraySize, ValuesSize, i, j = 0;

  jsonBuffer = cJSON_Parse(biosJsonString);
  if (jsonBuffer != NULL)
  {
    // create the root object for stripped json
    MapRoot = cJSON_CreateObject();
    jsonEntrys = cJSON_GetObjectItem(jsonBuffer, "RegistryEntries");
    if (jsonEntrys != NULL)
    {
      AttrsArray = cJSON_GetObjectItem(jsonEntrys, "Attributes");
      if (AttrsArray != NULL)
      {
        //ArrayMember = AttrsArray->child;
        ArraySize = cJSON_GetArraySize(AttrsArray);

        // enumerate all items and analysis them
        for (i = 0; i < ArraySize; i++)
        {
          ArrayMember = cJSON_GetArrayItem(AttrsArray, i);
          if (ArrayMember == NULL)
            return NULL;

          AttrsIDCase = cJSON_GetObjectItem(ArrayMember, "AttributeName");
          if (AttrsIDCase != NULL)
          {
            // add MappingID item to stripped json
            Items = cJSON_CreateObject();
            cJSON_AddItemToObject(MapRoot, AttrsIDCase->valuestring, Items);
            DEBUG((DEBUG_INFO, "[SK]: AttrsIDCase:%d valuestring:%a\n", i, AttrsIDCase->valuestring));

            TypeCase = cJSON_GetObjectItem(ArrayMember, "Type");
            if (TypeCase != NULL)
            {
              // add type item to stripped json
              cJSON_AddItemToObject(Items, "Type", cJSON_CreateString(TypeCase->valuestring));

              // for boolean type items
              if (!AsciiStrCmp(TypeCase->valuestring, "Boolean"))
              {
                // boolean type is default set true/false, no need more value emulation
                continue;
              }

              // for Enumeration type items
              else if (!AsciiStrCmp(TypeCase->valuestring, "Enumeration"))
              {
                ValueCase = cJSON_GetObjectItem(ArrayMember, "Value");
                if (ValueCase != NULL)
                {
                  // add value array to every items
                  Values = cJSON_CreateArray();
                  cJSON_AddItemToObject(Items, "Value", Values);

                  // go enumerate possible values
                  ValuesSize = cJSON_GetArraySize(ValueCase);
                  ValuesMember = ValueCase->child;
                  for (j = 0; j < ValuesSize; ++j)
                  {
                    ValueName = cJSON_GetObjectItem(ValuesMember, "ValueDisplayName");
                    // add the enumerated values to value array
                    cJSON_AddItemToArray(Values, cJSON_CreateString(ValueName->valuestring));
                    //DEBUG((DEBUG_INFO, "[SK] i%d|j%d valuestring:%a\n",i,j,ValueName->valuestring));
                    ValuesMember = ValuesMember->next;
                  }
                }
                else
                {
                  DEBUG((DEBUG_ERROR, "[SK]: ValueCase abnormal:%d!\n", i));
                  continue;
                }
              }

              // for Integer type items
              else if (!AsciiStrCmp(TypeCase->valuestring, "Integer"))
              {
                LowerBoundCase = cJSON_GetObjectItem(ArrayMember, "LowerBound");
                UpperBoundCase = cJSON_GetObjectItem(ArrayMember, "UpperBound");

                cJSON_AddItemToObject(Items, "UpperBound", cJSON_CreateNumberInt(UpperBoundCase->valueint));
                cJSON_AddItemToObject(Items, "LowerBound", cJSON_CreateNumberInt(LowerBoundCase->valueint));
              }

              // for String type items
              else if (!AsciiStrCmp(TypeCase->valuestring, "String"))
              {
                MinLengthCase = cJSON_GetObjectItem(ArrayMember, "MinLength");
                MaxLengthCase = cJSON_GetObjectItem(ArrayMember, "MaxLength");

                cJSON_AddItemToObject(Items, "MinLength", cJSON_CreateNumberInt(MinLengthCase->valueint));
                cJSON_AddItemToObject(Items, "MaxLength", cJSON_CreateNumberInt(MaxLengthCase->valueint));
              }

              else if (!AsciiStrCmp(TypeCase->valuestring, "Password"))
              {
                MaxLengthCase = cJSON_GetObjectItem(ArrayMember, "MaxLength");
                cJSON_AddItemToObject(Items, "MaxLength", cJSON_CreateNumberInt(MaxLengthCase->valueint));
              }
              else
              {
                //TODO:
              }
            }
            else
            {
              DEBUG((DEBUG_ERROR, "[SK]: TypeCase fail:%d!\n", i));
              continue;
            }
          }
          else
          {
            DEBUG((DEBUG_ERROR, "[SK]: AttrsIDCase fail:%d!\n", i));
            continue;
          }
        }
      }
      else
        DEBUG((DEBUG_ERROR, "[SK] AttrsArray is NULL!\n"));
    }
    else
      DEBUG((DEBUG_ERROR, "[SK] jsonEntrys is NULL!\n"));

    // return the stripped json
    cJSON_Delete(jsonBuffer);
    return MapRoot;
  }
  return (cJSON *)NULL;
}
//<kuaibai-20190326 improvement of OOB files process ->

/**
    GETs the bios file lists from the server
     
    @param JsonBuffer buffer to get from the server
    @param BufferSize size of the buffer read from server

    @retval EFI_STATUS
     
**/
EFI_STATUS GetBiosFileLists(CHAR8 **JsonBuffer, UINTN *BufferSize)
{
  CHAR8 GetUrl[128] = {0};
  EFI_STATUS Status = EFI_SUCCESS;

  Sprintf(GetUrl, "GET /ami/bios-files HTTP/1.1\r\n");
  DEBUG((DEBUG_ERROR, "\nCheckandUpdateAttributeRegVersion GetUrlPath :%s", GetUrl));

  Status = SendBMCGetRequest(GetUrl, (UINT32)Strlen(GetUrl), (VOID **)JsonBuffer, BufferSize); //Systems/BIOS/SD/index.json
  return Status;
}

/**
    POSTs the attribute data to server
     
    @param String8 data to POST

    @retval EFI_STATUS
     
**/
EFI_STATUS PostBiosAttributeData(CHAR8 *String8)
{
  CHAR8 *PostData = (CHAR8 *)NULL;
  UINTN PostDataLength = 0;
  CHAR8 *Response = (CHAR8 *)NULL;
  UINTN ResponseLength = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  PostData = (CHAR8 *)AllocateZeroPool(AsciiStrLen(String8) + 128);
  if (NULL == PostData)
  {
    return EFI_OUT_OF_RESOURCES;
  }
  PostDataLength = AsciiStrLen(String8);

  Sprintf(PostData,
          "POST /redfish/v1/Registries/BiosAttributeRegistry%s.%d.%d.%d.json HTTP/1.1\r\n"
          "Content-length: %d\r\n"
          "\r\n%s",
          (CHAR8 *)Get_ProjectTag(), (int)Get_ProjectMajorVersion(), (int)Get_ProjectMinorVersion(), (int)Get_ProjectBuildVersion(),
          PostDataLength,
          String8);

  Status = SendBMCGetRequest(PostData, (UINT32)Strlen(PostData), (VOID **)&Response, &ResponseLength);
  DEBUG((DEBUG_ERROR, "\nPUT SendBMCGetRequest Status:%x", Status));
  if (ResponseLength && (UINT8)GetFw_ManualJsonCreation())
  {
    Dump_ContentToFile(FALSE, L"BMCAttribRegPutResponse.json", Response, ResponseLength);
  }
  if (PostData)
    FreePool(PostData);

  if ((UINT8)GetFw_ManualJsonCreation())
  {
    Dump_ContentToFile(FALSE, L"BiosAttributeRegistry.json", String8, PostDataLength);
  }
  return Status;
}

/**
    Inits the rest instance with BMC or Network stack
     
    @retval EFI_STATUS
     
**/
EFI_STATUS InitRestInstance()
{
  EFI_STATUS Status = EFI_SUCCESS;

  Status = InitBmcInstance();
  return Status;
}

/**
    Sends password GET request to BMC and gets the response
     
    @param JsonBufferData password request response data

    @retval UINTN Response buffer length
     
**/
UINTN SendPwdGetRequest(CHAR8 **JsonBufferData)
{

  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8 GetUrl[128] = {0};
  UINTN BufferSize = 0;
  CHAR8 *JsonBuffer = (CHAR8 *)NULL;
  //<xuyj1001-20200401 Add BIOS OOB Function +>
  Sprintf(GetUrl, "GET /redfish/v1/Systems/Self/Bios/Actions/Bios.ChangePassword/SD HTTP/1.1\r\n");
  //<xuyj1001-20200401 Add BIOS OOB Function ->
  Status = SendBMCGetRequest(GetUrl, (UINT32)Strlen(GetUrl), (VOID **)&JsonBuffer, &BufferSize);
  DEBUG((DEBUG_ERROR, "\nSendBMCGetRequest Status:%r, Size:%d\n", Status, BufferSize));
  *JsonBufferData = JsonBuffer;

  return BufferSize;
}

/**
    Checks for BIOS configuration reset 

    @param JsonBuffer buffer to get from the server
    @param BufferSize size of the buffer read from server
    
    @retval EFI_STATUS
     
**/
EFI_STATUS GetBiosConfigurationReset(CHAR8 **JsonBuffer, UINTN *BufferSize)
{
  CHAR8 URL[200] = {0};
  EFI_STATUS Status = EFI_SUCCESS;

  Sprintf(URL, "GET /redfish/v1/Systems/Self/Bios/Actions/Bios.ResetBios HTTP/1.1\r\n"); //<xuyj1001-20200401 Add BIOS OOB Function +>
  Status = SendBMCGetRequest(URL, (UINT32)Strlen(URL), JsonBuffer, BufferSize);

  return Status;
}

/**
    Pushes UI application files to BMC
     
    @retval EFI_STATUS
     
**/
EFI_STATUS PushUiAppFiles()
{
  UINT8 iIndex = 0;
  UINT8 jIndex = 0;
  CHAR8 *Buffer = NULL;
  CHAR8 *BinExtnBuffer = NULL;
  INTN BufferSize = 0;
  INTN BinExtnBufferSize = 0;
  CHAR8 *PostBuffer = NULL;
  INTN PostBufferSize = 0;
  BOOLEAN IsBinaryFile = FALSE;
  CHAR8 FileName[81];
  CHAR8 EncodingType;
  CHAR8 *Response = (CHAR8 *)NULL;
  UINTN ResponseLength = 0;
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN Size = 0;
  extern FILE_XREF gFwHtmlFiles[];
  extern UINTN gFwHtmlFilesCount;
  extern BIN_EXTNS gFwUiAppBinExtns[];
  extern UINTN gFwUiAppBinExtnsCount;

  for (iIndex = 0; iIndex < gFwHtmlFilesCount - 1; iIndex++)
  {
    Buffer = NULL;
    BufferSize = 0;
    IsBinaryFile = FALSE;
    BinExtnBuffer = NULL;
    BinExtnBufferSize = 0;
    Response = NULL;
    ResponseLength = 0;
    PostBufferSize = 0;

    Status = FwGetHtmlFromFV(&gFwHtmlFiles[iIndex].ResourceGUID, (void **)&Buffer, &BufferSize);
    if (EFI_ERROR(Status))
    {
      continue;
    }

    for (jIndex = 0; jIndex < gFwUiAppBinExtnsCount - 1; jIndex++)
    {
      if (!(strcmp(gFwHtmlFiles[iIndex].FileExtn, gFwUiAppBinExtns[jIndex].Filename)))
      {
        IsBinaryFile = TRUE;
        break;
      }
    }
    memset(FileName, 31, 81);

    if (TRUE == IsBinaryFile)
    {
      BinExtnBuffer = base64_encode(Buffer, BufferSize, &BinExtnBufferSize);
      if (NULL != BinExtnBuffer)
      {
        PostBuffer = (CHAR8 *)AllocateZeroPool(BinExtnBufferSize + 200);
        if (NULL == PostBuffer)
        {
          continue;
        }

        Sprintf(PostBuffer,
                "POST /ami/static-file HTTP/1.1\r\n"
                "Content-length: %d\r\n"
                "\r\n",
                BinExtnBufferSize);
        EncodingType = 'B';

        memcpy(FileName, &EncodingType, 1);
        memcpy(FileName + 1, gFwHtmlFiles[iIndex].Filename, strlen(gFwHtmlFiles[iIndex].Filename));
        memcpy(PostBuffer + strlen(PostBuffer), FileName, 81);
        PostBufferSize = strlen(PostBuffer) + BinExtnBufferSize;
        memcpy(PostBuffer + strlen(PostBuffer), BinExtnBuffer, (UINT32)BinExtnBufferSize);

        Status = SendBMCGetRequest(PostBuffer, (UINT32)PostBufferSize, (VOID **)&Response, &ResponseLength);
        FreePool(PostBuffer);
        FreePool(BinExtnBuffer);
        //<kuaibai-20181222-fix hang b2 when ipmi abnormal+>
        if (EFI_DEVICE_ERROR == Status)
        { //Eg., if BMC stopped then don't try next communication
          FreePool(Buffer);
          return Status;
        }
        //<kuaibai-20181222-fix hang b2 when ipmi abnormal->
      }
    }
    else
    {
      PostBuffer = (CHAR8 *)AllocateZeroPool(BufferSize + 200);
      if (NULL == PostBuffer)
      {
        continue;
      }
      EncodingType = 'A';
      Sprintf(PostBuffer,
              "POST /ami/static-file HTTP/1.1\r\n"
              "Content-length: %d\r\n"
              "\r\n",
              BufferSize);
      memcpy(FileName, &EncodingType, 1);
      memcpy(FileName + 1, gFwHtmlFiles[iIndex].Filename, strlen(gFwHtmlFiles[iIndex].Filename));
      memcpy(PostBuffer + strlen(PostBuffer), FileName, 81);
      PostBufferSize = strlen(PostBuffer) + BufferSize;
      memcpy(PostBuffer + strlen(PostBuffer), Buffer, (UINT32)BufferSize);

      Status = SendBMCGetRequest(PostBuffer, (UINT32)PostBufferSize, (VOID **)&Response, &ResponseLength);
      FreePool(PostBuffer);
      //<kuaibai-20181222-fix hang b2 when ipmi abnormal+>
      if (EFI_DEVICE_ERROR == Status)
      { //Eg., if BMC stopped then don't try next communication
        FreePool(Buffer);
        return Status;
      }
      //<kuaibai-20181222-fix hang b2 when ipmi abnormal->
    }
    FreePool(Buffer);
  }
#if !ALWAYS_SEND_SETUPDATA_XML
  SendSetupDataXml();
#endif
  return EFI_SUCCESS;
}

/**
    Sends Patch request to BMC
     
    @param PostData buffer to post to the BMC

    @retval EFI_STATUS
     
**/
EFI_STATUS SendPatchRequest(CHAR8 *PostData)
{
  CHAR8 GetUrlPath[128] = {0};
  CHAR8 *RequestStr = NULL;
  CHAR8 *Response = (CHAR8 *)NULL;
  UINTN ResponseLength = 0;
  EFI_STATUS Status = EFI_SUCCESS;
    //CHAR8       TempBuffer[4096] = {""};
  CHAR8       *TempBuffer = NULL; //<hujf1001-20201127 Fix system hang when system have more boot option +>
  CHAR8 *Buffer;
  UINTN PostDataLength = strlen(PostData);
  //<xuyj1001-20200401 Add BIOS OOB Function +>
  TempBuffer = AllocateZeroPool( PostDataLength + 4096); //<hujf1001-20201127 Fix system hang when system have more boot option +>
  sprintf(TempBuffer, "{\n\t\"@Redfish.Copyright\": \"Copyright 2016 Distributed Management Task Force, Inc. (DMTF). All rights reserved.\",\
                   \n\t\"@odata.context\": \"/redfish/v1/$metadata#Systems/Slef/Bios\",\
                   \n\t\"@odata.id\": \"/redfish/v1/Systems/Self/Bios\",\
                   \n\t\"@odata.type\": \"#Bios.1.0.0.Bios\",\
                   \n\t\"Id\": \"Bios\",\
                   \n\t\"Name\": \"BIOS Configuration Current Settings\",\
                   \n\t\"Description\": \"BIOS Configuration Current Settings\",\
                   \n\t\"AttributeRegistry\": \"BiosAttributeRegistry%s.%d.%d.%d\",",
          Get_ProjectTag(), (int)Get_ProjectMajorVersion(), (int)Get_ProjectMinorVersion(), (int)Get_ProjectBuildVersion());
  //<xuyj1001-20200401 Add BIOS OOB Function ->
  PostDataLength += strlen(TempBuffer) + 128;
  Buffer = AllocateZeroPool(PostDataLength);
  strcpy(Buffer, TempBuffer);
  sprintf(TempBuffer, "\n\t\"Attributes\":{\n\t\t%s\n\t}\n}", PostData);
  strcat(Buffer, TempBuffer);

  RequestStr = (CHAR8 *)AllocateZeroPool(PostDataLength + 256);
  if (NULL == RequestStr)
  {
    if (Buffer)
      FreePool(Buffer);
    return EFI_OUT_OF_RESOURCES;
  }

  PostDataLength = strlen(Buffer);
  //<xuyj1001-20200401 Add BIOS OOB Function +>
  Sprintf(RequestStr,
          "PATCH /redfish/v1/Systems/Self/Bios HTTP/1.1\r\n"
          "Content-length: %d\r\n"
          "\r\n%s",
          PostDataLength, Buffer);
  //<xuyj1001-20200401 Add BIOS OOB Function ->
  DEBUG((DEBUG_ERROR, "\n RequestStr = %a", RequestStr));

  Status = SendBMCGetRequest(RequestStr, (UINT32)Strlen(RequestStr), (VOID **)&Response, &ResponseLength);
  DEBUG((DEBUG_ERROR, "\nPOST SendBMCGetRequest Status:%x \n Response = %a", Status, Response));
  if (ResponseLength && (UINT8)GetFw_ManualJsonCreation())
    Dump_ContentToFile(FALSE, L"BMCPutResponse.json", Response, ResponseLength);

  if (RequestStr)
    FreePool(RequestStr);
  if (Buffer)
    FreePool(Buffer);

  return Status;
}

/**
  Creates setupdata.xml and send it to the OData server
    
  @param  VOID

  @return EFI_STATUS
  @retval return the status returned from LocateProtocol or SendBMCGetRequest functions

**/

EFI_STATUS SendSetupDataXml(VOID)
{
  CONST CHAR16 *String = (CHAR16 *)NULL;
  CHAR8 SetupDataXml[20] = "SetupData.xml";
  UINTN Size = 0;
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8 *Response = (CHAR8 *)NULL;
  UINTN ResponseLength = 0;
  CHAR8 *PostBuffer = NULL;
  INTN PostBufferSize = 0;
  CHAR8 FileName[81];
  CHAR8 EncodingType;
  SETUP_INFO_PROTOCOL *SetupInfoProtocol = NULL;

#if REMOTE_COND_EXPRESSION_SUPPORT
  Status = BuildMapIdList();
  if (EFI_ERROR(Status))
  {
    return Status;
  }
#endif

  Status = pBS->LocateProtocol(&gSetupInfoProtocolGuid, NULL, (void **)&SetupInfoProtocol);
  if (!EFI_ERROR(Status))
  {
    String = NULL;
    Size = 0;
    Response = NULL;
    ResponseLength = 0;
    PostBufferSize = 0;
    memset(FileName, 31, 81);

    Status = SetupInfoProtocol->GetSetupDataXml((void **)&String, &Size, 0);
    if (!EFI_ERROR(Status) && (NULL != String))
    {
      PostBuffer = (CHAR8 *)AllocateZeroPool(Size + 200);
      if (NULL != PostBuffer)
      {
        EncodingType = 'A';
        Sprintf(PostBuffer,
                "POST /ami/static-file HTTP/1.1\r\n"
                "Content-length: %d\r\n"
                "\r\n",
                Size);
        memcpy(FileName, &EncodingType, 1);
        memcpy(FileName + 1, SetupDataXml, strlen(SetupDataXml));
        memcpy(PostBuffer + strlen(PostBuffer), FileName, 81);
        PostBufferSize = strlen(PostBuffer) + Size;
        memcpy(PostBuffer + strlen(PostBuffer), String, (UINT32)Size);

        Status = SendBMCGetRequest(PostBuffer, (UINT32)PostBufferSize, (VOID **)&Response, &ResponseLength);
        FreePool(PostBuffer);
        ////Dump setupdata.xml/////
        if ((UINT8)GetFw_ManualJsonCreation())
        {
          Dump_ContentToFile(FALSE, L"SetupData.xml", String, Size);
        }
      }
      FreePool(String);
    }
  }
  return Status;
}

//<xuyj1-20211207 Add BIOS Save Configuration after update BIOS +>
#if defined(Preserve_BIOS_Setting_OOB) && (Preserve_BIOS_Setting_OOB == 1)
/**
     GETs the bios current setting data from server
      
     @param JsonBuffer buffer read from the server
     @param BufferSize size of the buffer read

     @retval EFI_STATUS
      
 **/
EFI_STATUS GetBiosCurrentSettingData(CHAR8 **JsonBuffer, UINTN *BufferSize)
{
  CHAR8 GetUrlPath[128] = {0};
  EFI_STATUS Status = EFI_SUCCESS;
  Sprintf(GetUrlPath, "GET /redfish/v1/Systems/Self/Bios HTTP/1.1\r\n");
  DEBUG((DEBUG_ERROR, "\nGetUrlPath :%a", GetUrlPath));
  Status = SendBMCGetRequest(GetUrlPath, (UINT32)Strlen(GetUrlPath), (void **)JsonBuffer, BufferSize); //Systems/BIOS/SD/index.json
  DEBUG((DEBUG_ERROR, "\nSendBMCGetRequest Status:%x,:%x,Size:%d", Status, JsonBuffer, BufferSize));

  return Status;
}

/**
     GETs the bios Save Bios flag data from server
      
     @param JsonBuffer buffer read from the server
     @param BufferSize size of the buffer read

     @retval EFI_STATUS
      
 **/

EFI_STATUS GetBiosSaveConfigFlag(CHAR8 **JsonBuffer, UINTN *BufferSize)
{
  CHAR8 GetUrlPath[128] = {0};
  EFI_STATUS Status = EFI_SUCCESS;

  Sprintf(GetUrlPath, "GET /redfish/v1/Bmc/BiosInfo HTTP/1.1\r\n");
  DEBUG((DEBUG_ERROR, "\nGetUrlPath :%a", GetUrlPath));
  Status = SendBMCGetRequest(GetUrlPath, (UINT32)Strlen(GetUrlPath), (void **)JsonBuffer, BufferSize); ///redfish/v1/Bmc/BiosInfo
  DEBUG((DEBUG_ERROR, "\nSendBMCGetRequest Status:%x,:%x,Size:%d", Status, JsonBuffer, BufferSize));

  return Status;
}
#endif
//<xuyj1-20211207 Add BIOS Save Configuration after update BIOS ->
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
