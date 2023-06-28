
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

//<kuaibai-20181013-Delete AMIFWUpdate option if none request +>
#include "wchar.h"
#include "string.h"

typedef struct _BOOT_OPTION_TSE
{
    UINT32  Active;
    UINT16  PathLength;
    CHAR16  Name[1];
} BOOT_OPTION_TSE;

#define MAX_LENGTH 32
//<kuaibai-20181013-Delete AMIFWUpdate option if none request ->

#define AMI_AFU_FFS_FILE_GUID { 0x1DE64B8E, 0x138B, 0x4258, { 0xB7, 0xDD, 0xF2, 0xD8, 0xEC, 0x14, 0x2A, 0x9E } }

EFI_STATUS SendRecieveBMCRequest (
		EFI_REST_PROTOCOL *RestInstance, 
		EFI_HTTP_METHOD HttpMethod,
		CHAR8 *HttpUrl, 
		CHAR8 *HttpHeaders, 
		CHAR8 *HttpBody, 
		VOID **Response, 
		UINT32 *ResponseLength, 
		EFI_HTTP_STATUS_CODE *StatusCode);


#pragma pack(1)

struct {
	VENDOR_DEVICE_PATH Media;
	EFI_DEVICE_PATH_PROTOCOL End;
} FwShellDp = {
	{
        {
            MEDIA_DEVICE_PATH, MEDIA_VENDOR_DP,
            sizeof(VENDOR_DEVICE_PATH)
        },
        AMI_MEDIA_DEVICE_PATH_GUID
    },
	{
        END_DEVICE_PATH, END_ENTIRE_SUBTYPE,
        sizeof(EFI_DEVICE_PATH_PROTOCOL)
    }
};

struct {
	VENDOR_DEVICE_PATH Media;
	MEDIA_FW_VOL_FILEPATH_DEVICE_PATH Afu;
	EFI_DEVICE_PATH_PROTOCOL End;
} FwAfuDp = {
	{
        {
            MEDIA_DEVICE_PATH, 
            MEDIA_VENDOR_DP,
            {sizeof(VENDOR_DEVICE_PATH), 0}
        },
        AMI_MEDIA_DEVICE_PATH_GUID
    },
	{
        {
            MEDIA_DEVICE_PATH, 
            MEDIA_PIWG_FW_FILE_DP,
            {sizeof(MEDIA_FW_VOL_FILEPATH_DEVICE_PATH), 0}
        },
        AMI_AFU_FFS_FILE_GUID
	},
	{
        END_DEVICE_PATH, 
        END_ENTIRE_SUBTYPE,
        {sizeof(EFI_DEVICE_PATH_PROTOCOL), 0}
    }
};
#pragma pack()

EFI_STATUS FillBootIndex(UINTN InNumber, CHAR16 OutString[])
{
    CHAR16 digit[] = L"0123456789ABCDEF";
    INTN i;
    
    if(InNumber < 0 || InNumber > 0xFFFE){
    	return EFI_INVALID_PARAMETER;
    }
    for(i = 3; i >= 0; i--){
        if(InNumber == 0) OutString[i] = L'0';
        else{ 
		OutString[i] = digit[InNumber % 16];
		InNumber = InNumber / 16;
        }
    }
    return EFI_SUCCESS;
}

EFI_STATUS GetNVRamVariable (CHAR16 *VarName, EFI_GUID *Guid,void **NVData,UINTN *Size,UINT32 *Attrib)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    UINT32 DataAttribute=0;
    UINTN VariableSize=0;

    VariableSize = *Size; 
    *NVData = AllocateZeroPool (VariableSize);
    if (NULL == *NVData)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    Status = gRT->GetVariable (VarName, Guid, &DataAttribute, &VariableSize, *NVData);
    if (EFI_BUFFER_TOO_SMALL == Status)
    {
        FreePool(*NVData);
        *NVData = AllocateZeroPool  (VariableSize);
        if (NULL == *NVData)
        {
            return EFI_OUT_OF_RESOURCES;
        }
        Status = gRT->GetVariable (VarName, Guid, &DataAttribute, &VariableSize, *NVData);
    }
    if(EFI_SUCCESS == Status)
    {
        if (Attrib)
        {
            *Attrib = DataAttribute;
        }
        *Size = VariableSize ;
    }
    return Status;
}

//<kuaibai-20190320-Auto control BMC virtual usb status +>
typedef struct {
  BOOLEAN  Flag;
  UINT8    Type;
  UINT8    Reserved;
} OEM_OOB_FW;

BOOLEAN
SetFunctionVar(
  BOOLEAN flag,
  UINT8   type
)
{
  EFI_STATUS    Status = EFI_SUCCESS;
  OEM_OOB_FW  *OemOobFwVar = NULL;
  UINT32        Attribs = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;

  OemOobFwVar->Flag = flag;
  OemOobFwVar->Type = type;
  OemOobFwVar->Reserved = 0;

  Status = gRT->SetVariable(
                L"OEM_OOB_FW_VAR",
                &gAmiTseOemPortingVar1Guid,
                Attribs,
                sizeof(OEM_OOB_FW), 
                (VOID *)&OemOobFwVar
                );
  DEBUG((EFI_D_INFO, "[SK] SetFunctionVar OEM_OOB_FW_VAR:%r, Flag:%x;\n",Status,OemOobFwVar->Flag));
  if(!EFI_ERROR(Status))
    return TRUE;
  else
    return FALSE;
}
//<kuaibai-20190320-Auto control BMC virtual usb status ->

EFI_STATUS
SetBiosUpdBootOption(UINT8* OptionalData, UINTN OptionalDataSize)
{
	EFI_STATUS      Status = EFI_SUCCESS;
	EFI_LOAD_OPTION *FwUpdateOption;
	UINTN           AMIFWUpdateSize = 0, FwAfuDpSize = 0, BootOptionSize = 0;
	UINT8           *NewFwBootOption = NULL;
	CHAR16          *AMIFWUpdateStr = L"AMIFWUpdate";
	UINT32          VarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
	UINTN           BootDataLength=0;
	CHAR16          VarName[] = L"BootXXXX";
	UINT16          VarIndex = 0;
	UINT16          lastVarIndex = 0;
    
	if(OptionalData == NULL && OptionalDataSize != 0)
	{
	  DEBUG((EFI_D_INFO, "[SK] SetBiosUpdBootOption EFI_INVALID_PARAMETER;\n"));
		return EFI_INVALID_PARAMETER;
	}

	do
	{
		FillBootIndex(VarIndex, VarName + 4);

		Status = GetNVRamVariable((CHAR16*)VarName, &gEfiGlobalVariableGuid, (void**)&NewFwBootOption, &BootDataLength, &VarAttrib);
		if(Status == EFI_SUCCESS)
		{
		  if(StrStr((CHAR16*)(NewFwBootOption + sizeof(EFI_LOAD_OPTION)), AMIFWUpdateStr))
		  {
        gRT->SetVariable(VarName, &gEfiGlobalVariableGuid, 0, 0, NULL);
        lastVarIndex = VarIndex;
        break;
		  }
      VarIndex++;
      lastVarIndex = VarIndex;
      FreePool(NewFwBootOption);
		  NewFwBootOption = NULL;
		}
		else
		{
		    VarIndex++;
		}
	} while (VarIndex <= 0xFF);
	
	if(NewFwBootOption != NULL)
	{
		FreePool(NewFwBootOption);
		NewFwBootOption = NULL; //<kuaibai-20190119-Fixed AMIFwUpdate boot number to avoid update fail>
	}
	
	AMIFWUpdateSize = StrSize(AMIFWUpdateStr);
	FwAfuDpSize = sizeof(FwAfuDp);
	BootOptionSize = sizeof(EFI_LOAD_OPTION) + AMIFWUpdateSize + FwAfuDpSize + OptionalDataSize;
	NewFwBootOption = AllocateZeroPool(BootOptionSize);
	if(NewFwBootOption == NULL){
		return EFI_OUT_OF_RESOURCES;
	}
	FwUpdateOption = (EFI_LOAD_OPTION *)NewFwBootOption;
	
//<kuaibai002-20180817-Hide AMIFWUpdate boot option in Setup menu +>
	//FwUpdateOption->Attributes = 1; 
#if HIDE_FWUPDATE_BOOT_OPTION
	FwUpdateOption->Attributes = LOAD_OPTION_HIDDEN | LOAD_OPTION_ACTIVE;
#else
	FwUpdateOption->Attributes = LOAD_OPTION_ACTIVE; //Active
#endif
//<kuaibai002-20180817-Hide AMIFWUpdate boot option in Setup menu ->
	
	FwUpdateOption->FilePathListLength = (UINT16)FwAfuDpSize;
//	FwUpdateOption.Description = L"AMIFWUpdate";
	NewFwBootOption += sizeof(EFI_LOAD_OPTION);
	MemCpy(NewFwBootOption, (UINT8*)AMIFWUpdateStr, AMIFWUpdateSize);
//	FwUpdateOption.FilePathList = FwAfuDp;
	NewFwBootOption += AMIFWUpdateSize;
	MemCpy(NewFwBootOption, (UINT8*)(&FwAfuDp), FwAfuDpSize);
//	FwUpdateOption.OptionalData = OptionalData; 
	NewFwBootOption += FwAfuDpSize;
	MemCpy(NewFwBootOption, OptionalData, OptionalDataSize);
//<kuaibai-20190119-Fixed AMIFwUpdate boot number to avoid update fail +>	
  // FillBootIndex(lastVarIndex, VarName + 4);
  FillBootIndex(0x5A, VarName + 4); //Fixed boot number by spiral
	Status = gRT->SetVariable(
	              VarName,
	              &gEfiGlobalVariableGuid,
			          VarAttrib,
			          BootOptionSize,
			          (VOID*)FwUpdateOption
			          );
	DEBUG((EFI_D_INFO, "[SK] SetVariable FwUpdateOption = %r;\n",Status));
	DEBUG((EFI_D_INFO, "[SK] VarName = %s\n",VarName));

	FreePool(FwUpdateOption);
	FwUpdateOption = NULL;
	lastVarIndex = 0x5A;

	if(!EFI_ERROR(Status))
	{
		Status = gRT->SetVariable(
		            L"BootNext",
		            &gEfiGlobalVariableGuid,
		            VarAttrib,
		            sizeof(lastVarIndex),
		            (VOID*)&lastVarIndex);
		DEBUG((EFI_D_INFO, "[SK] Set BootNext = %x %r;\n",lastVarIndex, Status));

		SetFunctionVar(TRUE, 0x01);//<kuaibai-20190320-Auto control BMC virtual usb status>
	}
//<kuaibai-20190119-Fixed AMIFwUpdate boot number to avoid update fail -> 
	return Status;
}	
	
EFI_STATUS SetFwUpdBootOption()
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_LOAD_OPTION *FwUpdateOption;
    UINTN AMIShellUpdateSize = 0, FwShellDpSize = 0, BootOptionSize = 0;
    UINT8* NewFwBootOption = NULL;
    CHAR16* AMIShellUpdateStr = L"UEFI: Built-in EFI Shell";
    BOOLEAN EfiShellPresent = FALSE;
    UINT32 VarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    UINTN BootDataLength=0;
    CHAR16 VarName[] = L"BootXXXX";
    UINT16 VarIndex = 0;
    UINT16 lastVarIndex = 0;
    
    do
    {
    	FillBootIndex(VarIndex, VarName + 4);
            
        Status = GetNVRamVariable((CHAR16*)VarName, &gEfiGlobalVariableGuid, (void**)&NewFwBootOption, &BootDataLength, &VarAttrib);
        if(Status == EFI_SUCCESS)
        {
            if(MemCmp((VOID*)(NewFwBootOption + sizeof(EFI_LOAD_OPTION) + StrSize((CHAR16*)(NewFwBootOption + sizeof(EFI_LOAD_OPTION)))), (VOID*)&FwShellDp, sizeof(FwShellDp)) == 0)
            {
            	EfiShellPresent = TRUE;
                lastVarIndex = VarIndex;
                break;
            }
            VarIndex++;
            lastVarIndex = VarIndex;
	    FreePool(NewFwBootOption);
	    NewFwBootOption = NULL;
        }
        else
        {
            VarIndex++;
        }
    }while (VarIndex <= 0xFF);
    
    if(NewFwBootOption != NULL){
	    FreePool(NewFwBootOption);
    }
	
    if(!EfiShellPresent){
		AMIShellUpdateSize = StrSize(AMIShellUpdateStr);
		FwShellDpSize = sizeof(FwShellDp);
		BootOptionSize = sizeof(EFI_LOAD_OPTION) + AMIShellUpdateSize + FwShellDpSize;
		NewFwBootOption = AllocateZeroPool(BootOptionSize);
		if(NewFwBootOption == NULL){
			return EFI_OUT_OF_RESOURCES;
		}
		FwUpdateOption = (EFI_LOAD_OPTION *)NewFwBootOption;
		FwUpdateOption->Attributes = 1;
		FwUpdateOption->FilePathListLength = (UINT16)FwShellDpSize;
	//	FwUpdateOption.Description = L"UEFI: Built-in EFI Shell";
		NewFwBootOption += sizeof(EFI_LOAD_OPTION);
		MemCpy(NewFwBootOption, (UINT8*)AMIShellUpdateStr, AMIShellUpdateSize);
	//	FwUpdateOption.FilePathList = FwShellDp;
		NewFwBootOption += AMIShellUpdateSize;
		MemCpy(NewFwBootOption, (UINT8*)(&FwShellDp), FwShellDpSize);
		
		FillBootIndex(lastVarIndex, VarName + 4);
		Status = gRT->SetVariable(
				VarName, &gEfiGlobalVariableGuid,
				VarAttrib,
				BootOptionSize, (VOID*)FwUpdateOption);
	
		FreePool(FwUpdateOption);
    }
    
	if(!EFI_ERROR(Status)){
		Status = gRT->SetVariable(
				L"BootNext", &gEfiGlobalVariableGuid,
				VarAttrib,
				sizeof(lastVarIndex), (VOID*)&lastVarIndex);
	}
	
	return Status;
}

BOOLEAN CheckCDRomBootOption(
		EFI_HANDLE 	DeviceHandle, EFI_DEVICE_PATH_PROTOCOL **CdRomDp
)
{
    EFI_STATUS Status;
    EFI_DEVICE_PATH_PROTOCOL *Dp, *BlockIoDp;

    // If the device handle is not null, try to get the device path protocol
    if(DeviceHandle == NULL)
        return FALSE;

    // Check to see if the current handle supports EfiDevicePathProtocolGuid if it does return the device path interface
    Status = gBS->HandleProtocol(DeviceHandle, &gEfiDevicePathProtocolGuid, &BlockIoDp);
    
    if(!EFI_ERROR(Status)) {
        Dp = BlockIoDp;
        // Check each node until the end for the MEDIA_DEVICE_PATH type and MEDIA_VENDOR_DP subtype
        while(!isEndNode(Dp)) {
            if(Dp->Type == MEDIA_DEVICE_PATH && 
               Dp->SubType == MEDIA_CDROM_DP) {
            	*CdRomDp = BlockIoDp;
                return TRUE;
            }
            // If nothing was found go to the next node
            Dp = NEXT_NODE(Dp);
        }
    }
    return FALSE;
}

extern EFI_HANDLE gRestBootImageHandle;


EFI_STATUS CheckBootEfi(EFI_DEVICE_PATH_PROTOCOL *Dp)
{
	EFI_STATUS Status;
	EFI_HANDLE Handle;
	UINT8 FileNodeBuffer[sizeof(EFI_DEVICE_PATH_PROTOCOL)+sizeof(EFI_REMOVABLE_MEDIA_FILE_NAME)];
	EFI_DEVICE_PATH_PROTOCOL *FileNode = (EFI_DEVICE_PATH_PROTOCOL*)FileNodeBuffer;
	
	FileNode->Type=MEDIA_DEVICE_PATH;
	FileNode->SubType=MEDIA_FILEPATH_DP;
	FileNode->Length[0]=(UINT8)(sizeof(EFI_DEVICE_PATH_PROTOCOL)+sizeof(EFI_REMOVABLE_MEDIA_FILE_NAME));
	FileNode->Length[1]=0;
	StrCpy((CHAR16*)(FileNode+1),EFI_REMOVABLE_MEDIA_FILE_NAME);
	Dp = AppendDevicePathNode(Dp,FileNode);
	Status=gBS->LoadImage(TRUE, gRestBootImageHandle, Dp, NULL, 0, &Handle);
	FreePool(Dp);
	return Status;
}


EFI_STATUS SetOSDeploymentBootOption(EFI_DEVICE_PATH_PROTOCOL *Dp)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_LOAD_OPTION *FwUpdateOption;
    UINTN AMIBmcCdRomUpdateSize = 0, FwCdRomDpSize = 0, BootOptionSize = 0;
    UINT8* NewFwBootOption = NULL;
    CHAR16* AMIBmcCdRomUpdateStr = L"BMC OS Deployment Virtual DVD";
    BOOLEAN BmcCdRomPresent = FALSE;
    UINT32 VarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    UINTN BootDataLength=0;
    CHAR16 VarName[] = L"BootXXXX";
    UINT16 VarIndex = 0;
    UINT16 lastVarIndex = 0;

    FwCdRomDpSize = DPLength(Dp);
    do
    {
    	FillBootIndex(VarIndex, VarName + 4);
            
        Status = GetNVRamVariable((CHAR16*)VarName, &gEfiGlobalVariableGuid, (void**)&NewFwBootOption, &BootDataLength, &VarAttrib);
        if(Status == EFI_SUCCESS)
        {
            if(DPCmp((EFI_DEVICE_PATH_PROTOCOL*)(NewFwBootOption + sizeof(EFI_LOAD_OPTION) + StrSize((CHAR16*)(NewFwBootOption + sizeof(EFI_LOAD_OPTION)))), Dp) == 0)
            {
            	BmcCdRomPresent = TRUE;
                lastVarIndex = VarIndex;
                break;
            }
            VarIndex++;
            lastVarIndex = VarIndex;
	    FreePool(NewFwBootOption);
	    NewFwBootOption = NULL;
        }
        else
        {
            VarIndex++;
        }
    }while (VarIndex <= 0xFF);
    
    if(NewFwBootOption){
	    FreePool(NewFwBootOption);
    }
	
    if(!BmcCdRomPresent){
		AMIBmcCdRomUpdateSize = StrSize(AMIBmcCdRomUpdateStr);
		BootOptionSize = sizeof(EFI_LOAD_OPTION) + AMIBmcCdRomUpdateSize + FwCdRomDpSize;
		NewFwBootOption = AllocateZeroPool(BootOptionSize);
		if(NewFwBootOption == NULL){
			return EFI_OUT_OF_RESOURCES;
		}
		FwUpdateOption = (EFI_LOAD_OPTION *)NewFwBootOption;
		FwUpdateOption->Attributes = 1;
		FwUpdateOption->FilePathListLength = (UINT16)FwCdRomDpSize;
	//	FwUpdateOption.Description = L"BMC OS Deployment Virtual DVD";
		NewFwBootOption += sizeof(EFI_LOAD_OPTION);
		MemCpy(NewFwBootOption, (UINT8*)AMIBmcCdRomUpdateStr, AMIBmcCdRomUpdateSize);
	//	FwUpdateOption.FilePathList = Dp;
		NewFwBootOption += AMIBmcCdRomUpdateSize;
		MemCpy(NewFwBootOption, (UINT8*)Dp, FwCdRomDpSize);
		
		FillBootIndex(lastVarIndex, VarName + 4);
		Status = gRT->SetVariable(
				VarName, &gEfiGlobalVariableGuid,
				VarAttrib,
				BootOptionSize, (VOID*)FwUpdateOption);
	
		FreePool(FwUpdateOption);
    }
    
	if(!EFI_ERROR(Status)){
		Status = gRT->SetVariable(
				L"BootNext", &gEfiGlobalVariableGuid,
				VarAttrib,
				sizeof(lastVarIndex), (VOID*)&lastVarIndex);
	}
	
	return Status;
}	



EFI_STATUS CheckOSDeploymentBootOption()
{
    EFI_STATUS  Status = EFI_SUCCESS;
	UINTN 				HandleCount = 0;
	UINTN 				iIndex = 0;
	EFI_HANDLE 			*Handles = NULL, UsbIoHandle;
	EFI_BLOCK_IO_PROTOCOL 	*BlockInstance = (EFI_BLOCK_IO_PROTOCOL *)NULL;
	EFI_USB_IO_PROTOCOL		*UsbIo = NULL;
	EFI_USB_DEVICE_DESCRIPTOR	DevDesc;
	EFI_DEVICE_PATH_PROTOCOL *CdRomDp, *UsbIoDp;
	BOOLEAN bFirstCdRomBoot = TRUE;
	
	Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &HandleCount, &Handles);
	if (!EFI_ERROR(Status))
	{
		for (iIndex = 0; iIndex < HandleCount; iIndex++ )
		{
			gBS->HandleProtocol (Handles [iIndex], &gEfiBlockIoProtocolGuid, &BlockInstance);
			if(CheckCDRomBootOption(Handles [iIndex], &CdRomDp)){
		        UsbIoDp = CdRomDp;
		        Status = gBS->LocateDevicePath(&gEfiUsbIoProtocolGuid, &UsbIoDp, &UsbIoHandle);
		        if(EFI_ERROR(Status)) continue;
		        
				Status = gBS->HandleProtocol (UsbIoHandle, &gEfiUsbIoProtocolGuid, &UsbIo);
			
				if(!EFI_ERROR(Status)){
					Status = UsbIo->UsbGetDeviceDescriptor(UsbIo,	&DevDesc);
					
					//
					// Return FALSE, For Error case
					//
					if ( EFI_ERROR(Status) ) continue;
					
					//
					// Check for Vendor ID and Product ID
					// Return TRUE, If Vendor ID and Product ID matches
					// with BMC CDROM. Else, return FALSE.
					//
					if ( (DevDesc.IdVendor == 0x46b) &&
							(DevDesc.IdProduct == 0xFF20) ) {
						Status = CheckBootEfi(CdRomDp);
						if(!EFI_ERROR(Status)){
							Status = SetOSDeploymentBootOption(CdRomDp);
							break;
						}
					} 
				}
			}
		}
		FreePool (Handles);
	}
	return Status;
}

EFI_STATUS GetBootOptionsData (EFI_REST_PROTOCOL 	*RestInstance, CHAR8 **JsonBuffer, UINT32 *BufferSize)
{
	EFI_STATUS 		Status = EFI_SUCCESS;
	EFI_HTTP_STATUS_CODE	StatusCode;
	Status = SendRecieveBMCRequest(RestInstance, HttpMethodGet, URL_BOOT_OPTIONS_BMC_REQUEST, NULL, NULL, (void **)JsonBuffer, BufferSize, &StatusCode);
	DEBUG((EFI_D_ERROR, "[SK] SendRecieveBMCRequest = %r;\n",Status)); //Spiral
	if(!EFI_ERROR(Status) && StatusCode != HTTP_STATUS_200_OK)
	{
		DEBUG((EFI_D_ERROR, "GetBootOptionsData: Error status - %r, StatusCode - %d\n", Status, StatusCode));
		Status = EFI_PROTOCOL_ERROR;
	}
	return Status;
}



#define MAX_ITOA_STR_LEN 21

CHAR8 *AmiItoA(UINT32 i, CHAR8 *Buf, UINTN BufLen)
{
	CHAR8 *p = Buf + BufLen - 1;	/* points to terminating '\0' */
	*p = '\0';
	do {
		*--p = '0' + (i % 10);
		i /= 10;
	} while (i != 0);
	return p;
}


/**
    Patch command to clear boot options in BMC
     
    @retval EFI_STATUS
     
**/ 
EFI_STATUS ClearBootOptions (
		IN EFI_REST_PROTOCOL *RestInstance 
)
{
	INT32 			BodyBufferSize = 0;
	CHAR8 			*HeadersBuffer = NULL;
	INT32 			HeadersBufferSize = 0;
	CHAR8 			*Response = (CHAR8 *) NULL;
	UINT32 			ResponseLength = 0;
	EFI_STATUS 		Status = EFI_SUCCESS;
	CHAR8 			BufLenStr[MAX_ITOA_STR_LEN], *LenStr;
	EFI_HTTP_STATUS_CODE	StatusCode;

	BodyBufferSize = (UINT32)AsciiStrLen(CLEAR_BOOT_BODY_REQUEST);
	
	LenStr = AmiItoA(BodyBufferSize, &BufLenStr[0], MAX_ITOA_STR_LEN);
	HeadersBufferSize = (UINT32)AsciiStrLen(CONTENT_LEN_HEADER_BMC_REQUEST) 
			+ (UINT32)AsciiStrLen(LenStr) + (UINT32)AsciiStrLen(HTTP_CRLF_STR);
	HeadersBuffer = (CHAR8 *)AllocateZeroPool (HeadersBufferSize + 1);
	if (NULL == HeadersBuffer)
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto Exit;
	}
	
	CopyMem(HeadersBuffer, CONTENT_LEN_HEADER_BMC_REQUEST, AsciiStrLen(CONTENT_LEN_HEADER_BMC_REQUEST));
	CopyMem(HeadersBuffer + AsciiStrLen(CONTENT_LEN_HEADER_BMC_REQUEST), LenStr, AsciiStrLen(LenStr));
	CopyMem(HeadersBuffer + AsciiStrLen(CONTENT_LEN_HEADER_BMC_REQUEST) + AsciiStrLen(LenStr), HTTP_CRLF_STR, AsciiStrLen(HTTP_CRLF_STR));
	HeadersBuffer[HeadersBufferSize] = '\0';
	
	Status = SendRecieveBMCRequest(RestInstance, HttpMethodPatch, URL_BOOT_OPTIONS_BMC_REQUEST, 
			HeadersBuffer, CLEAR_BOOT_BODY_REQUEST, (VOID **)&Response, &ResponseLength, &StatusCode);
	if(!EFI_ERROR(Status) && 
			(StatusCode != HTTP_STATUS_200_OK && StatusCode != HTTP_STATUS_204_NO_CONTENT))
	{
		DEBUG((EFI_D_ERROR, "ClearBootOptions: Error status - %r, StatusCode - %d\n", Status, StatusCode));
		Status = EFI_PROTOCOL_ERROR;
	}

Exit:

	if(Response != NULL){
		FreePool (Response);
	}
	if(HeadersBuffer != NULL){
		FreePool (HeadersBuffer);
	}
	
	return Status;
}

//<kuaibai-20181013-Delete AMIFWUpdate option if none request +>
EFI_STATUS OemGetEfiVariable(
    IN CHAR16 *sName, IN EFI_GUID *pGuid,
    OUT UINT32 *pAttributes OPTIONAL,
    IN OUT UINTN *pDataSize, OUT VOID **ppData
)
{
    EFI_STATUS Status;
    if (!*ppData) *pDataSize=0;
    Status = gRT->GetVariable(sName, pGuid, pAttributes, pDataSize, *ppData);
    if (!EFI_ERROR(Status)) return Status;
    if (Status==EFI_BUFFER_TOO_SMALL)
    {
        if (*ppData) FreePool(*ppData);
        if (!(*ppData=AllocateZeroPool(*pDataSize))) return EFI_OUT_OF_RESOURCES;
        Status = gRT->GetVariable(sName, pGuid, pAttributes, pDataSize, *ppData);
    }
    return Status;
}
/**
    Function to find&delete AMIFWUpdate boot option if it exist.

    @retval EFI_STATUS - EFI_SUCCESS
                       - EFI_NOT_FOUND
**/
EFI_STATUS 
DelBiosUpdBootOpt()
{
  UINT16  *BootOrder = NULL;
  UINTN   BootOrderSize,Size = 0;
  UINT16  Index = 0;
  UINT16  BootStr[MAX_LENGTH] = {0};
  BOOT_OPTION_TSE *BootOptions = NULL;
  EFI_STATUS Status = EFI_NOT_FOUND;

  DEBUG((DEBUG_ERROR, "[SK] DelBiosUpdBootOpt entry\n"));
  BootOrderSize = 0;
  Status = OemGetEfiVariable(
        L"BootOrder", &gEfiGlobalVariableGuid, NULL, &BootOrderSize, (VOID**)&BootOrder
    );
  if (EFI_ERROR(Status)) return Status;

  // For each Boot Option retrieve the BootXXXX varaible
  for (Index = 0; Index < (BootOrderSize/sizeof (UINT16)); Index++)
  {
      // Get next boot item variable
      swprintf(BootStr, MAX_LENGTH, L"Boot%04X", BootOrder[Index]);
      Size = 0;
      if (BootOptions != NULL) {
          FreePool(BootOptions);
          BootOptions = NULL;
      }
      Status = OemGetEfiVariable( BootStr, &gEfiGlobalVariableGuid, NULL, &Size, &BootOptions );
      if (EFI_ERROR(Status))
          continue;
      
      DEBUG((DEBUG_ERROR, "[SK] BootOptions->Name = %s\n", BootOptions->Name));
      // If BootOption Name is AMIFWUpdate then delete this option
      if(!(StrCmp(BootOptions->Name, L"AMIFWUpdate")))
      {
        DEBUG((DEBUG_ERROR, "[SK] AMIFWUpdate exist\n"));
        gRT->SetVariable(BootStr, &gEfiGlobalVariableGuid, 0, 0, NULL);
        break;
      }
  }
  //<kuaibai-20190119-Fixed AMIFwUpdate boot number to avoid update fail +>
  Status = OemGetEfiVariable(L"Boot005A", &gEfiGlobalVariableGuid, NULL, &Size, &BootOptions);
  if(!(StrCmp(BootOptions->Name, L"AMIFWUpdate")))
  {
    DEBUG((DEBUG_ERROR, "[SK] AMIFWUpdate exist\n"));
    gRT->SetVariable(L"Boot005A", &gEfiGlobalVariableGuid, 0, 0, NULL);
  }
  //<kuaibai-20190119-Fixed AMIFwUpdate boot number to avoid update fail ->
  if(BootOrder != NULL)
      FreePool(BootOrder);
  if (BootOptions != NULL)
      FreePool(BootOptions);
  
  return Status;
}
//<kuaibai-20181013-Delete AMIFWUpdate option if none request ->

EFI_STATUS CheckBootAction(EFI_REST_PROTOCOL 	*RestInstance, CHAR8* TargetBootSource)
{
	EFI_STATUS Status;
	DEBUG((EFI_D_INFO, "[SK] CheckBootAction entry;\n"));
	if(AsciiStrnCmp(BIOS_UPDATE_TARGET_BOOT_SOURCE, TargetBootSource, AsciiStrLen(BIOS_UPDATE_TARGET_BOOT_SOURCE)) == 0)
	{
		TargetBootSource+= AsciiStrLen(BIOS_UPDATE_TARGET_BOOT_SOURCE);
		while(*TargetBootSource == ' ')
		  TargetBootSource++;
		Status = SetBiosUpdBootOption(TargetBootSource, AsciiStrSize(TargetBootSource));
		DEBUG((EFI_D_INFO, "[SK] SetBiosUpdBootOption = %r;\n",Status));
	}
	else
	{
		if(AsciiStrnCmp(FW_UPDATE_TARGET_BOOT_SOURCE, TargetBootSource, AsciiStrLen(FW_UPDATE_TARGET_BOOT_SOURCE)) == 0)
		{
			Status = SetFwUpdBootOption();
		}
		else if(AsciiStrnCmp(OS_DEPLOYMENT_TARGET_BOOT_SOURCE, TargetBootSource, AsciiStrLen(OS_DEPLOYMENT_TARGET_BOOT_SOURCE)) == 0)
		{
			Status = CheckOSDeploymentBootOption();
		}
		else
		  Status = EFI_UNSUPPORTED;
		
		if(!EFI_ERROR(Status))
		{
			Status = ClearBootOptions(RestInstance);
			DEBUG((EFI_D_INFO, "ClearBootOptions status - %r\n", Status));
		}
	}
	return Status;
}


EFI_STATUS EFIAPI GetBootOptionsFromBMC(
		EFI_REST_PROTOCOL 	*RestInstance
)
{
    CHAR8 *JsonBuffer = NULL;
    UINT32 BufferSize = 0;
    EFI_STATUS  Status = EFI_SUCCESS;
    cJSON *json = NULL, *jsonBoot = NULL, *jsonBootSourceOverrideEnabled = NULL, *jsonBootSourceOverrideTarget = NULL;
    cJSON *jsonUefiTargetBootSourceOverride = NULL;

    DelBiosUpdBootOpt();//<kuaibai-20181013-Delete AMIFWUpdate option if none request>

    DEBUG((EFI_D_INFO, "[SK] GetBootOptionsFromBMC entry;\n"));
    Status = GetBootOptionsData (RestInstance, &JsonBuffer, &BufferSize);
    DEBUG((EFI_D_INFO, "[SK] GetBootOptionsData = %r;\n",Status));
    if ((Status == EFI_SUCCESS) && (BufferSize > 0) && (NULL != JsonBuffer))
    {
      DEBUG((EFI_D_INFO, "[SK] JsonBuffer %a\n",JsonBuffer,JsonBuffer));
		  json = cJSON_Parse(JsonBuffer);
		  if(json != NULL){
		    DEBUG((EFI_D_INFO, "[SK] 111\n"));
			  jsonBoot = cJSON_GetObjectItem(json, COMPUTER_SYSTEM_BOOT);
			if(jsonBoot != NULL){
			  DEBUG((EFI_D_INFO, "[SK] 222\n"));
				// Get value of BootSourceOverrideEnabled attribute
				jsonBootSourceOverrideEnabled = cJSON_GetObjectItem(jsonBoot, COMPUTER_SYSTEM_BOOT_SOURCE_OVERRIDE_ENABLED);
				
				// Check BootSourceOverrideEnabled is present and is not set to "Disable"
				if(jsonBootSourceOverrideEnabled != NULL
						&& jsonBootSourceOverrideEnabled->type == cJSON_String 
						&& AsciiStriCmp(COMPUTER_SYSTEM_DISABLE, jsonBootSourceOverrideEnabled->valuestring) != 0)
				{
				  DEBUG((EFI_D_INFO, "[SK] 333\n"));
					// Get value of BootSourceOverrideTarget attribute
					jsonBootSourceOverrideTarget = cJSON_GetObjectItem(jsonBoot, COMPUTER_SYSTEM_BOOT_SOURCE_OVERRIDE_TARGET);
					
					// Check BootSourceOverrideTarget is present and has "UefiTarget" value
					if(jsonBootSourceOverrideTarget != NULL 
							&& jsonBootSourceOverrideTarget->type == cJSON_String 
							&& AsciiStriCmp(COMPUTER_SYSTEM_UEFI_TARGET_VALUE, jsonBootSourceOverrideTarget->valuestring) == 0)
					{
						// Get value of UefiTargetBootSourceOverride attribute
						jsonUefiTargetBootSourceOverride = cJSON_GetObjectItem(jsonBoot, COMPUTER_SYSTEM_EFI_TARGET_BOOT_SOURCE_OVERRIDE);
						DEBUG((EFI_D_INFO, "[SK] 444 jsonUefiTargetBootSourceOverride = %x;\n",jsonUefiTargetBootSourceOverride));
						// Check UefiTargetBootSourceOverride is present and contains Uefi Device Path value
						if(jsonUefiTargetBootSourceOverride != NULL && 
								jsonUefiTargetBootSourceOverride->type == cJSON_String){
							DEBUG((EFI_D_ERROR, "Value of UefiTargetBootSourceOverride is %a\n", jsonUefiTargetBootSourceOverride->valuestring));
							Status = CheckBootAction(RestInstance, jsonUefiTargetBootSourceOverride->valuestring);
						}
					}
				}
			}
			cJSON_Delete(json);
		}
		else{
			DEBUG((EFI_D_ERROR, "Error to parse Json file - %a\n", cJSON_GetErrorPtr()));
		}
       	FreePool (JsonBuffer);
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

