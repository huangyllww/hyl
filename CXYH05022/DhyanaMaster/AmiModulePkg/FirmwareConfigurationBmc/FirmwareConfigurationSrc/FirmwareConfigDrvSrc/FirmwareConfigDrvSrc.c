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
#include "Token.h"
#include <Include/AMIVfr.h>
#include <Library/DebugLib.h>
#include "Library/UefiLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Library/BaseLib.h"
#include "FirmwareConfigDrvSrc.h"
#include "BiosAttribTypeLib.h"
#include "JsonHiiLibraryApi.h"
#include <JsonUtilityLib.h>
#include "Protocol/AMIPostMgr.h"
#include "Library/AmiJsonLib.h"
#include "ConditionalExpressionApi.h"
#include <Protocol\IpmiTransportProtocol.h>//<kuaibai-20180802-Auto disable if bmc redfish is off>
#include <OemIpmiNetFnDefinitions.h>//<kuaibai-20180802-Auto disable if bmc redfish is off>
#include <Protocol\GraphicsOutput.h>//<kuaibai-20180905 Fix OOB message will block setup prompt message>

//<kuaibai-20181106-Fix oob current value abnormally kept +>
#define OemAttributes (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define SCE_UTILITY_INDICATION_VARIABLE_GUID \
        { 0x5a71834c, 0xb9ba, 0x48ee, { 0xac, 0x37, 0xf1, 0x1d, 0xbc, 0x4c, 0x6f, 0x6f} }
//<kuaibai-20181106-Fix oob current value abnormally kept ->

#define AMIEMAINDICATIONBIT1 0x00000001
#define AMIEMAINDICSENDSETUPXML 0x00000002

EFI_HANDLE 			gImageHandle;
static EFI_EVENT    gNvramUpdateEvent;
EFI_REST_PROTOCOL 	*gRestInstance = (EFI_REST_PROTOCOL *)NULL;
EFI_HANDLE          gProtocolHandle;//<kuaibai-20181222-fix hang b2 when ipmi abnormal>
BOOLEAN 			gResetBiosConfig = FALSE;
BOOLEAN             gNvramChanged = FALSE;
void 				*gSetFailMapIdList = NULL;
UINT32 				gFailMapListSize = 0;
EFI_HII_HANDLE 	gHiiHandle = NULL;
//<kuaibai-20180708 +>
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_SYSTEM_TABLE         *gST;
extern EFI_RUNTIME_SERVICES     *gRT;
//extern EFI_BOOT_SERVICES               *pBS;// = NULL;
//<kuaibai-20180708 ->
EFI_GRAPHICS_OUTPUT_BLT_PIXEL gForeGround = {0xFF,0xFF,0xFF,0};
EFI_GRAPHICS_OUTPUT_BLT_PIXEL gBackGround = {0,0,0,0};

CHAR16 *ConfiguringFW = (CHAR16 *)NULL;
CHAR16 *FWServerIP = (CHAR16 *)NULL;
CHAR16 *Response = (CHAR16 *)NULL;
CHAR16 *Success = (CHAR16 *)NULL;
CHAR16 *TimeOut = (CHAR16 *)NULL;
CHAR16 *ConfiguringFWFailed = (CHAR16 *)NULL;
CHAR16 *LoadDefaults = (CHAR16 *)NULL;
CHAR16 *AttribRegistry = (CHAR16 *)NULL;
CHAR16 *IndexJson = (CHAR16 *)NULL;
CHAR16 *ImportFWConfig = (CHAR16 *)NULL;
CHAR16 *ImportPassword = (CHAR16 *)NULL ;
CHAR16 *PushUIApp = (CHAR16 *)NULL ;
CHAR16 *EraseLine = (CHAR16 *)NULL;
AMI_POST_MANAGER_PROTOCOL *gPostMgr = (AMI_POST_MANAGER_PROTOCOL *)NULL;
AMI_CONDITIONAL_MAPIDLIST_PROTOCOL *gAmiCondMapIdListProtocol = NULL;

static EFI_EVENT gTSE_EnterSetupEvent = NULL;

EFI_STATUS 			Get_BiosSetupJsonData(OUT CONST CHAR16** Out_JsonBuffer,OUT UINTN *Out_JsonbufSize);
extern cJSON *Analysis_BiosSetupJsonData(CHAR8 *biosJsonString);//<kuaibai-20190326 improvement of OOB files process>
extern cJSON *Special_BiosSetupJsonAttributeData(CHAR8 *biosJsonString);//Spiral
extern cJSON *Special_BiosSetupJsonCurrentValue(CHAR8 *biosJsonString);//Spiral
/**
    If BUFFER size is too large, to PRINT the entire message

    @retval void
     
**/
void CustomisedDebugMessage(  CHAR8 *Message, CHAR8 *MsgBuffer, UINTN MsgBufferSize )
{
#if FWBMC_DEBUG_MODE
    if( (Message != NULL) &&(MsgBuffer != NULL) && (MsgBufferSize != 0))
    {
        UINTN Count = 0;
        UINTN Index =0;
        UINTN StringIndex =0;
        CHAR8 TempChar = '\0';
        if(MAX_DEBUG_DISPLAY_CHAR < MsgBufferSize)
        {
            Count = MsgBufferSize/MAX_DEBUG_DISPLAY_CHAR;
            
            for(Index = 0;Index<Count;Index++)
            {
                StringIndex = Index * MAX_DEBUG_DISPLAY_CHAR;
                TempChar = MsgBuffer[StringIndex+MAX_DEBUG_DISPLAY_CHAR];
                MsgBuffer[StringIndex+MAX_DEBUG_DISPLAY_CHAR] = '\0';
                if(Index == 0)
                    DEBUG((DEBUG_ERROR, Message,MsgBuffer));
                else
                    DEBUG((DEBUG_ERROR, "%a", MsgBuffer+StringIndex));
                MsgBuffer[StringIndex+MAX_DEBUG_DISPLAY_CHAR] = TempChar;
            }   
            StringIndex = Index * MAX_DEBUG_DISPLAY_CHAR;
            if(StringIndex < MsgBufferSize)
                DEBUG((DEBUG_ERROR, "%a", MsgBuffer+StringIndex));
        }
        else
        {
            DEBUG((DEBUG_ERROR, Message,MsgBuffer));
        }
    }
#endif
}

/**
    Display status message for FWConfig

    @retval void
     
**/
void FWConfigStatusDisplay( CHAR16 *Message )
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForeGround = {0xFF,0xFF,0xFF,0};
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackGround = {0,0,0,0};
    EFI_STATUS  Status = EFI_NOT_FOUND;
    

	if( (NULL != gPostMgr) && (Message != NULL) )
	{
	  
		Status = gPostMgr->DisplayQuietBootMessage((CHAR16 *)EraseLine, 0, 0, CA_AttributeLeftBottom, ForeGround, BackGround) ;
		if (!EFI_ERROR (Status))
		{       
			gPostMgr->DisplayQuietBootMessage((CHAR16 *)Message, 0, 0, CA_AttributeLeftBottom, ForeGround, BackGround) ;
		}
		else
		{
			gPostMgr->DisplayPostMessage ((CHAR16 *)Message);
		}
	}
	else
	{
		DEBUG((DEBUG_ERROR, "In FwConfigDrv Post manager protocol not found or message NULL.\n"));
	}

}

VOID FwMemFreePointer( VOID **ptr )
{
	if ( ( ptr == NULL ) || ( *ptr == NULL ) )
		return;

//<kuaibai-20180708>	pBS->FreePool( *ptr );
	gBS->FreePool( *ptr );
	*ptr = NULL;
}

VOID UpdateFirmWareConfiguaration ()
{
	EFI_STATUS 		Status = EFI_SUCCESS;
	EFI_TPL			OldTpl = TPL_APPLICATION;
	CONST CHAR16 	*String = (CHAR16 		*)NULL;
	CONST CHAR8 	*String8 = (CHAR8 		*)NULL;
	UINTN			Size = 0;
	FIRWARE_CONFIG_ATTRIB_PROTOCOL *FirmwareConfigurationProtocol = NULL;	

	//<kuaibai-20180708> OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);	// guarantees that RestoreTPL won't ASSERT
	OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);	// guarantees that RestoreTPL won't ASSERT
	//<kuaibai-20180708> pBS->RestoreTPL (TPL_APPLICATION);
	gBS->RestoreTPL (TPL_APPLICATION);
#if REMOTE_CONFIG_TSE_DEPENDENCY
	//<kuaibai-20180708> Status = pBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
	Status = gBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
	if (!EFI_ERROR (Status))
	{
	    Status = FirmwareConfigurationProtocol->GetFirmwareCallback(&String, &Size);
	}
#else
	Status = Get_BiosSetupJsonData(&String, &Size);
#endif
	
	    if (!EFI_ERROR (Status))
	{
		Dump_ContentToFile(TRUE,L"BiosAttributeRegistry.json",String,Size);
		FreePool(String);
	}
	String = NULL;
	Size = 0;
	CreateIndexJsonFile(&String8,&Size);
	if(String8)
	{
		Dump_ContentToFile(FALSE,L"index.json",String8,Size);
		FreePool(String8);
		String8 = NULL;
	}
	//<kuaibai-20180708> pBS->RaiseTPL (TPL_HIGH_LEVEL); 			// guarantees that RestoreTPL won't ASSERT
	gBS->RaiseTPL (TPL_HIGH_LEVEL); 			// guarantees that RestoreTPL won't ASSERT
	//<kuaibai-20180708> pBS->RestoreTPL (OldTpl);
	gBS->RestoreTPL (OldTpl);

}

//<kuaibai-20190326 improvement of OOB files process +>
/**
    This function will be called to send cmd to BMC that the 
    Rf files transfer ok.

    @param  NONE

    @retval EFI_STATUS
    
**/
EFI_STATUS
UploadRfDone()
{
  EFI_STATUS         Status = EFI_SUCCESS;
  EFI_IPMI_TRANSPORT *IpmiTransportProtocol;
  UINT8              *RequestData = NULL;
  UINT8              RequestSize = MAX_FILENAME_SIZE;
  UINT8              ResponseData[2] = {0};
  UINT8              ResponseDataSize = 2;
  UINT8              BMC_Error_Count = 0;

  Status = gBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&IpmiTransportProtocol);
  if (EFI_ERROR(Status)) return Status;

  Status = gBS->AllocatePool(EfiBootServicesData, RequestSize, (VOID**)&RequestData);
  if (EFI_ERROR(Status)) return Status;
  gBS->SetMem(RequestData,RequestSize,0);

  Sprintf(RequestData, 
          "BiosAttributeRegistry%s.%d.%d.%d.json",
          (CHAR8 *)Get_ProjectTag(), (int)Get_ProjectMajorVersion(), (int)Get_ProjectMinorVersion(), (int)Get_ProjectBuildVersion()
          );

  if (!EFI_ERROR(Status))
  {
    // CMD 0x3E: finish upload OOB files
    _IPMI_RETRY:
    Status = IpmiTransportProtocol->SendIpmiCommand (
                                IpmiTransportProtocol,
                                NETFN_OEM,
                                BMC_LUN,
                                0x3E,
                                RequestData,
                                RequestSize,
                                ResponseData,
                                &ResponseDataSize
                                );
    DEBUG((DEBUG_ERROR, "\n[SK] UploadRfDone SendIpmiCommand = %r !!\n",Status));
    if (EFI_ERROR(Status))
    {
      if (BMC_Error_Count < 3)
      {
          BMC_Error_Count++;   
          goto _IPMI_RETRY;
      }
    }
    return Status;
  }
  return Status;
}
//<kuaibai-20190326 improvement of OOB files process ->

EFI_STATUS ConstructAttributeAndPost()
{
    CONST CHAR16    *String = (CHAR16 *)NULL;
    UINTN       	Size = 0;
    CHAR8 			*String8;
    CHAR8 			*String8_tmp;//Spiral
    EFI_STATUS  	Status = EFI_SUCCESS;
    FIRWARE_CONFIG_ATTRIB_PROTOCOL *FirmwareConfigurationProtocol = NULL;
    
#if REMOTE_CONFIG_TSE_DEPENDENCY
    DEBUG((DEBUG_ERROR, "\n[SK] 0ConstructAttributeAndPost.\n"));
    //<kuaibai-20180708> Status = pBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
    Status = gBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
    DEBUG((DEBUG_ERROR, "\n[SK] LocateProtocol gFirmwareConfigAttribGuid = %r\n",Status));
    if (!EFI_ERROR (Status))
    {
        Status = FirmwareConfigurationProtocol->GetFirmwareCallback(&String, &Size);
        DEBUG((DEBUG_ERROR, "\n[SK] GetFirmwareCallback = %r\n",Status));
    }
#else
    Status = Get_BiosSetupJsonData(&String, &Size);
#endif  

    if (!EFI_ERROR (Status))
    {
        cJSON *StrippedJson = NULL; //<kuaibai-20190326 improvement of OOB files process>
        cJSON *CustomizedJson = NULL; //Spiral

        //String8 = ConvertChar16toChar8(String);
        String8_tmp = ConvertChar16toChar8(String);//Spiral
        CustomisedDebugMessage("\nAttribute Json Data:\n%a",String8_tmp,Size);//Spiral
        DEBUG((DEBUG_ERROR, "\n[SK] Before PostBiosAttributeData"));

        CustomizedJson = Special_BiosSetupJsonAttributeData(String8_tmp);//Spiral
        String8 = cJSON_PrintUnformatted(CustomizedJson);//Spiral
        cJSON_Delete(CustomizedJson);//Spiral

//<kuaibai-20190326 improvement of OOB files process +>
        Size = AsciiStrLen (String8);
        Status = SendBiosRfData (String8, Size, "Attribute");
        if (EFI_ERROR (Status))
        {
          DEBUG((DEBUG_ERROR, "\n[SK] SendBiosRfData Attribute:%r",Status));
          return Status;
        }
        StrippedJson = Analysis_BiosSetupJsonData(String8);
        String8 = cJSON_PrintUnformatted(StrippedJson);
        Size = AsciiStrLen (String8);
        //DEBUG((DEBUG_ERROR, "\n[SK]: Analysis_BiosSetupJsonData result:\n%a\n",cJSON_Print(StrippedJson)));
        Status = SendBiosRfData (String8, Size, "Stripped");
        if (EFI_ERROR (Status))
        {
          DEBUG((DEBUG_ERROR, "\n[SK] SendBiosRfData Stripped:%r",Status));
          return Status;
        }
        cJSON_Delete(StrippedJson);
        Status = UploadRfDone();
        if (EFI_ERROR (Status))
        {
          DEBUG((DEBUG_ERROR, "\n[SK] UploadRfDone:%r",Status));
          return Status;
        }
        //Status = PostBiosAttributeData (String8);
//<kuaibai-20190326 improvement of OOB files process ->
        DEBUG((DEBUG_ERROR, "\n[SK] After PostBiosAttributeData %r",Status));
    }
    else
    	return EFI_NOT_FOUND;
//<kuaibai-20181008 Add token to control oob webui support+>
#if OOB_WEBUI_ENABLE
    DEBUG((DEBUG_ERROR, "\n[SK] Before PushUiAppFiles.\n")); 
    PushUiAppFiles ();
    DEBUG((DEBUG_ERROR, "\n[SK] After PushUiAppFiles.\n"));
#endif
//<kuaibai-20181008 Add token to control oob webui support->
    return Status;
}

EFI_STATUS ConstructIndexAndPost ()
{
    EFI_STATUS      Status = EFI_SUCCESS;
    CHAR8 *IndexJson, *IndexJson_tmp; //Spiral
    UINTN JsonbufSize;
    CHAR8 *PostData = (CHAR8 *)NULL;
    UINTN PostDataLength = 0;
    CHAR8 *RequestStr = NULL;
    cJSON *CustomizedJson = NULL; //Spiral

#if REMOTE_COND_EXPRESSION_SUPPORT
    DEBUG((DEBUG_ERROR, "\n[SK] ---1 ConstructIndexAndPost\n"));
    Status = BuildMapIdList ();
    DEBUG((DEBUG_ERROR, "\n[SK] ---1 BuildMapIdList = %r\n",Status));
    if (EFI_ERROR (Status))
    {
        return Status;
    }
#endif

    CreateIndexJsonFile(&IndexJson_tmp,&JsonbufSize);
    DEBUG((DEBUG_ERROR, "\n[SK] ---1 CreateIndexJsonFile\n"));
    if(NULL == IndexJson_tmp)
    {
        return EFI_NOT_FOUND;
    }

    CustomisedDebugMessage("\nCreated IndexJsonFile:\n%a",IndexJson_tmp,JsonbufSize);

    if ((UINT8)GetFw_ManualJsonCreation())
    {
        Dump_ContentToFile(FALSE,L"index.json",IndexJson_tmp,JsonbufSize);
    }
    
    CustomizedJson = Special_BiosSetupJsonCurrentValue(IndexJson_tmp);//Spiral
    IndexJson = cJSON_PrintUnformatted(CustomizedJson);//Spiral
    JsonbufSize = AsciiStrLen (IndexJson);//Spiral
    cJSON_Delete(CustomizedJson);//Spiral
    
    PostData = (CHAR8 *)AllocateZeroPool (JsonbufSize + 128);
    if( NULL == PostData)
    {
        return EFI_OUT_OF_RESOURCES;
    }
    Sprintf (PostData, "%s", IndexJson);
    PostDataLength = AsciiStrLen (PostData);

    Status = PostBiosIndexData (PostData, PostDataLength);
    DEBUG((DEBUG_ERROR, "\n[SK] ---1 PostBiosIndexData = %r\n",Status));
    if (PostData)
    	FreePool (PostData);

    return Status;
}

//<AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Procedure: ConvertChar16toChar8
//
// Description: Converts CHAR16 string to CHAR8
//
// Input:
//  InputString     - Input CHAR16 String
//  OutString       - Output CHAR8 string
//
// Output: void
//
//---------------------------------------------------------------------------
//<AMI_PHDR_END>
void ConvertChar8toChar16(CHAR8 *InputString, CHAR16 *OutString)
{
    UINTN Index = 0;

    if (*InputString == '\0')
    {
        return;
    }

    while (InputString[Index] != '\0')
    {
        OutString[Index] = (CHAR16)InputString[Index];
        Index++;
    }

    OutString[Index] = '\0';
}

BOOLEAN CheckCurrentAttribVersion (CHAR8 *JsonBuffer)
{
	CHAR8 		*FileNameBuffer = (CHAR8 *) NULL;
	CHAR8 		*VersionBuffer = (CHAR8 *) NULL;
	CHAR8 		*StartAddress = (CHAR8 *) NULL;
    CHAR8		*EndAddress = (CHAR8 *) NULL;
    CHAR8 		String8Frmt[128] = "BiosAttributeRegistry%s.%d.%d.%d";
    CHAR8 		String8[128] = {""};
    CHAR8		*VersionString = (CHAR8 *) NULL;
	UINT32 		JsonBufferLength = 0;
    UINT32      iIndex = 0;
    BOOLEAN 	MajorSet = FALSE;
    BOOLEAN 	MinorSet = FALSE;
	EFI_STATUS 	Status = EFI_SUCCESS;
	UINT32 		MajorVer = 0;
	UINT32 		MinorVer = 0;
	UINT32 		BuildVer = 0;
	
    Sprintf (String8, String8Frmt, (CHAR8 *)Get_ProjectTag(), (UINT32)Get_ProjectMajorVersion(), (UINT32)Get_ProjectMinorVersion(), (UINT32)Get_ProjectBuildVersion());
    
    FileNameBuffer = strstr (JsonBuffer, String8);
	if (NULL == FileNameBuffer)
	{
		return FALSE;
	}
	//Comparing file existence is enough but for safety compare version also
	VersionBuffer = strstr (FileNameBuffer, "version");
	if (NULL == VersionBuffer)
	{
		return FALSE;
	}
	
    while (VersionBuffer && *VersionBuffer != ':')
    	VersionBuffer++;
	
	VersionBuffer += 2;	//+2 for : and "
    
    StartAddress = VersionBuffer;
    EndAddress = strstr (StartAddress, "\"");
	JsonBufferLength = (UINT32)(EndAddress - StartAddress);
	
	VersionString = (VOID *) AllocateZeroPool(50);
	if (NULL == VersionString)
	{
		return FALSE;
	}
	
	strncpy (VersionString, StartAddress, JsonBufferLength);
	StartAddress = VersionString;
	EndAddress = StartAddress + strlen (VersionString);
	
    while (iIndex < (JsonBufferLength))
    {
    	CHAR8 	ProjVersion [3] = {0};
    	CHAR8	*MiddleAddress = NULL;
        UINT32 	StrLength = 0;

    	MiddleAddress = strstr (StartAddress, ".");
        if(MiddleAddress != NULL)
        {
        	StrLength = (UINT32)(MiddleAddress - StartAddress);
        }
        else
        {
        	StrLength = (UINT32)(EndAddress - StartAddress);
        }
        
        iIndex += StrLength+1;
        strncpy (ProjVersion, StartAddress, StrLength);
        if(!MajorSet)
        {
        	MajorVer = (UINT32)atoi(ProjVersion);
        	MajorSet = TRUE;
        }
        else if (!MinorSet)
        {
        	MinorVer = (UINT32)atoi(ProjVersion);
        	MinorSet = TRUE;
		}
        else
        {
        	BuildVer = (UINT32)atoi(ProjVersion);
        }
        
        StartAddress = StartAddress + StrLength;
        StartAddress++;
    }
    DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Src  MajorVer= %d MinorVer =%d BuildVer=%d\n",MajorVer,MinorVer,BuildVer));
	if ((MajorVer != (UINT32)Get_ProjectMajorVersion()) || (MinorVer != (UINT32)Get_ProjectMinorVersion()) || (BuildVer != (UINT32)Get_ProjectBuildVersion()))
	{
		    DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Change Not Matched\n"));

		return FALSE;
	}
	else
	{
		 DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Change Matched\n"));
		return TRUE;
	}
    return FALSE;
}

EFI_STATUS CheckandUpdateAttributeRegVersion()
{
	EFI_STATUS	Status = EFI_SUCCESS;
	CHAR8 		*JsonBuffer = (CHAR8 *) NULL;
	UINTN 		BufferSize = 0;
    UINT8       VarData = 0;
    UINT32      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    UINTN       Size = sizeof (UINT8);

	Status = GetBiosFileLists (&JsonBuffer, &BufferSize);
	if (!EFI_ERROR (Status) && (NULL != JsonBuffer))
	{
        if (GetFw_ManualJsonCreation())
        {
            Dump_ContentToFile(FALSE, L"biosfilelist.json", JsonBuffer, BufferSize);
        }
	    DEBUG((DEBUG_ERROR, "\nGET /ami/bios-files Response:\n%a",JsonBuffer));
		if (FALSE == CheckCurrentAttribVersion (JsonBuffer))
		{
	        DEBUG((DEBUG_ERROR, "\nCheckandUpdateAttributeRegVersion Version Didn't Match:ConstructAttributeAndPost Updated"));
	        DEBUG((DEBUG_ERROR, "\n[SK] PERF_START.\n"));
	        PERF_START (NULL,"AttribJson-Post", NULL, 0);
			Status = ConstructAttributeAndPost();
			DEBUG((DEBUG_ERROR, "\n[SK] 111ConstructAttributeAndPost = %r\n",Status));
			PERF_END (NULL,"AttribJson-Post", NULL, 0);
			//Status = UpdateAttributeVersionFile(); 	//Commented because server will update the bios-files list automatically
#if LEGACY_THIRD_PARTY_METHOD == 0 //<kuaibai-20190326 improvement of OOB files process>
			Status = ConstructIndexAndPost ();
#endif
            DEBUG((DEBUG_ERROR, "\n[SK] 111ConstructIndexAndPost = %r\n",Status));
			
            //<kuaibai-20180708> pRS->SetVariable (L"EmaFirstLaunchIndicator", &gEmaFirstLaunchIndicatorGuid, Attributes, Size, &VarData);   
            gRT->SetVariable (L"EmaFirstLaunchIndicator", &gEmaFirstLaunchIndicatorGuid, Attributes, Size, &VarData);   
		}
#if SEND_FIRMWARE_LAYOUT_ON_FIRST_LAUNCH
		else
		{
	        //<kuaibai-20180708> Status= pRS->GetVariable (L"EmaFirstLaunchIndicator", &gEmaFirstLaunchIndicatorGuid, NULL, &Size, &VarData);
	        Status= gRT->GetVariable (L"EmaFirstLaunchIndicator", &gEmaFirstLaunchIndicatorGuid, NULL, &Size, &VarData);
	        if (EFI_NOT_FOUND == Status)
	        {
	            //<kuaibai-20180708> Status = pRS->SetVariable (L"EmaFirstLaunchIndicator", &gEmaFirstLaunchIndicatorGuid, Attributes, Size, &VarData);   
	            Status = gRT->SetVariable (L"EmaFirstLaunchIndicator", &gEmaFirstLaunchIndicatorGuid, Attributes, Size, &VarData);   
	            DEBUG((DEBUG_ERROR, "\n Attribute Version Matched but Attributes may differ. So construct attribute registry again and post again"));
	            PERF_START (NULL,"AttribJson-Post", NULL, 0);
	            Status = ConstructAttributeAndPost();
	            PERF_END (NULL,"AttribJson-Post", NULL, 0);
#if LEGACY_THIRD_PARTY_METHOD == 0 //<kuaibai-20190326 improvement of OOB files process>
	            Status = ConstructIndexAndPost ();
#endif
	        }
		}
#endif
        if (JsonBuffer)
        	FreePool (JsonBuffer);
	}
	else
	{
	    DEBUG((DEBUG_ERROR, "\nCheckandUpdateAttributeRegVersion BiosAttributeRegistry.json not present"));
	    PERF_START (NULL,"AttribJson-Post", NULL, 0);
		Status = ConstructAttributeAndPost();
		PERF_END (NULL,"AttribJson-Post", NULL, 0);
#if LEGACY_THIRD_PARTY_METHOD == 0 //<kuaibai-20190326 improvement of OOB files process>
      Status = ConstructIndexAndPost ();
#endif
		//Status = UpdateAttributeVersionFile(); 	//Commented because server will update the bios-files list automatically
	}
	return Status;
}

EFI_STATUS FirmwareConfigurationEntry()
{
	CHAR8 		*JsonBuffer = (VOID *) NULL;
	UINTN 		BufferSize = 0;
	CHAR8 		GetUrlPath[128] = {0};
	EFI_STATUS	Status = EFI_SUCCESS;
	BOOLEAN 	NvramUpdatedonImport = FALSE;
	UINT8 		ResetType = 0;
    FIRWARE_CONFIG_ATTRIB_PROTOCOL *FirmwareConfigurationProtocol = NULL;
    UINT32          Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    UINTN           Size = sizeof (UINT8);
	EFI_GUID AmiPostMgrProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
	UINT32          AmiEMAIndVarAttributes = 0; 
	UINT32      	EmaIndications = 0;
	UINT32 			checkBit = 0;
	UINT32          CheckSetupXmlBit = 0;
	EFI_STATUS 		EmaIndicationsStatus = EFI_SUCCESS;
	UINTN          	EMAIndicationSize = sizeof(UINT32);
	//pBS = gBS; //<kuaibai-20180708>
	FWConfigStatusDisplay(ConfiguringFW) ;
  
    //<kuaibai-20180708> Status= pRS->GetVariable (L"ResetType", &gResetTypeGuid, NULL, &Size, &ResetType);
    Status= gRT->GetVariable (L"ResetType", &gResetTypeGuid, NULL, &Size, &ResetType);
    if (EFI_ERROR (Status))
    {
        ResetType = 0;
        //<kuaibai-20180708> Status = pRS->SetVariable (L"ResetType", &gResetTypeGuid, Attributes, sizeof(UINT8), &ResetType);
        Status = gRT->SetVariable (L"ResetType", &gResetTypeGuid, Attributes, sizeof(UINT8), &ResetType);
    }

/////////Loading defaults starts/////////////
	Status = GetBiosConfigurationReset (&JsonBuffer, &BufferSize);
	DEBUG((DEBUG_ERROR, "\nFwConfig: GetBiosConfigurationReset Status %r BufferSize %d", Status, BufferSize));
	if (!EFI_ERROR (Status) && (0 != BufferSize))
	{
        if (GetFw_ManualJsonCreation())
        {
            Dump_ContentToFile(FALSE, L"resetbios.json", JsonBuffer, BufferSize);
        }
		Status = ParseJsonandImport (JsonBuffer, (UINT32)BufferSize, RESETBIOSCONFIG);
		DEBUG((DEBUG_ERROR, "\n[SK]Loading defaults starts:: ParseJsonandImport = %r", Status));
		if (!EFI_ERROR (Status) && (TRUE == gResetBiosConfig))
		{
			//<kuaibai-20180708> Status = pBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
			Status = gBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
			DEBUG((DEBUG_ERROR, "\n[SK]Loading defaults starts:: gFirmwareConfigAttribGuid = %r", Status));
			if (!EFI_ERROR (Status))
			{
				FWConfigStatusDisplay(LoadDefaults) ;
				PERF_START (NULL, "Loading-Defaults", NULL, 0);
				Status = FirmwareConfigurationProtocol->ResetBiosConfig (0);		//0 can be replaced with some other in future
				DEBUG((DEBUG_ERROR, "\n[SK]Loading defaults starts:: ResetBiosConfig = %r", Status));
				PERF_END (NULL, "Loading-Defaults", NULL, 0);
				if (!EFI_ERROR (Status))
				{
					goto DONE;
				}
			}
		}
	}
/////////Loading defaults ends//////////////

/////////BiosAttributeRegistry.json starts/////////////
	FWConfigStatusDisplay(AttribRegistry) ;
	PERF_START (NULL,"UpdateAttribJson", NULL, 0);
	Status = CheckandUpdateAttributeRegVersion ();
	PERF_END (NULL,"UpdateAttribJson", NULL, 0);
/////////BiosAttributeRegistry.json ends//////////////

/////////Bios SD import starts//////////////
	FWConfigStatusDisplay(ImportFWConfig) ;
    BufferSize = 0;
    JsonBuffer = NULL;
	Status = GetBiosSdData (&JsonBuffer, &BufferSize);
	DEBUG((DEBUG_ERROR, "\nFwConfig: GetBiosSdData Status %r BufferSize %d", Status, BufferSize));
    if (!EFI_ERROR(Status) && (BufferSize > 0) && (NULL != JsonBuffer))
    {
        CustomisedDebugMessage("\nIndexsd.json buffer data:\n%a",JsonBuffer,BufferSize);
        if ((UINT8)GetFw_ManualJsonCreation())
        {
            Dump_ContentToFile(FALSE, L"indexsd.json", JsonBuffer, BufferSize);
        }
        
        PERF_START (NULL,"ImportJson", NULL, 0);
        Status = SetMapStringControlDatabase(NULL,0,JsonBuffer,(UINT32)BufferSize,&gSetFailMapIdList,&gFailMapListSize);
        PERF_END (NULL,"ImportJson", NULL, 0);
        DEBUG((DEBUG_ERROR, "\nImportIndexJsonData Status:%x",Status));
        
        //Update system Reset flag
    
        //need to get the status of Nvram from JsonHIILibrary
        if(EFI_SUCCESS == Status)
        	 NvramUpdatedonImport |= TRUE;
        //NvramUpdatedonImport |= gNvramChanged;
        
        if (JsonBuffer)
        	FreePool (JsonBuffer);

    }
/////////Bios SD import ends//////////////

/////////Index.json starts//////////////
    else
    {
    	BufferSize = 0;
	    JsonBuffer = NULL;
		Status = GetBiosIndexData (&JsonBuffer, &BufferSize);
		DEBUG((DEBUG_ERROR, "\nFwConfig: GetBiosIndexData Status %r BufferSize %d", Status, BufferSize));
        if (EFI_ERROR (Status) || (0 == BufferSize) || (NULL == JsonBuffer))
        {
            PERF_START (NULL,"IndexJson-Post", NULL, 0);
#if LEGACY_THIRD_PARTY_METHOD == 0 //<kuaibai-20190326 improvement of OOB files process>
      Status = ConstructIndexAndPost ();
#endif
			PERF_END (NULL,"IndexJson-Post", NULL, 0);
			DEBUG((DEBUG_ERROR, "\nElse ConstructIndexAndPost Status:%x",Status));
        }
        if (JsonBuffer)
        	FreePool (JsonBuffer);
    }
/////////Index.json ends//////////////

/////////Password import starts//////////////
    //Get password data and import
    BufferSize = 0;
    JsonBuffer = NULL;
    BufferSize = SendPwdGetRequest(&JsonBuffer);
    DEBUG((DEBUG_ERROR, "\nFwConfig: SendPwdGetRequest BufferSize %d", BufferSize));
    if ((BufferSize > 0) && (NULL != JsonBuffer) )
    {
		FWConfigStatusDisplay(ImportPassword) ;
        DEBUG((DEBUG_ERROR, "\nPassword.json buffer data:\n%a",JsonBuffer));
        if ((UINT8)GetFw_ManualJsonCreation())
        {
           Dump_ContentToFile(FALSE, L"password.json", JsonBuffer, BufferSize);
        }
         
        PERF_START (NULL,"PwdImportJson", NULL, 0);
	    Status = ParseJsonandImport (JsonBuffer, (UINT32)BufferSize, PASSWORDIMPORT);
	    PERF_END (NULL,"PwdImportJson", NULL, 0);
	    DEBUG((DEBUG_ERROR, "\npassword ParseJsonandImport Status:%x",Status));
		
	    //Update system Reset flag
        //need to get the status of Nvram from JsonHIILibrary
        if(EFI_SUCCESS == Status)
        	 NvramUpdatedonImport |= TRUE;
	    //NvramUpdatedonImport |= gNvramChanged;
	            
	    if (JsonBuffer)
	        FreePool (JsonBuffer);
    }
/////////Password import ends//////////////

/////////Index.json starts//////////////
    //Check for NVRAM value change and Reset the system
    if (NvramUpdatedonImport)
    {
       FWConfigStatusDisplay(IndexJson) ;
       PERF_START (NULL,"IndexJson-Post", NULL, 0);
       Status = ConstructIndexAndPost ();
       PERF_END (NULL,"IndexJson-Post", NULL, 0);
       DEBUG((DEBUG_ERROR, "\nFirmwareConfigurationEntry:ConstructIndexAndPost Status:%r",Status));

DONE:
       DEBUG((DEBUG_ERROR, "\nFirmwareConfigurationEntry: System going to restart on importing the configuration"));
       //<kuaibai-20180708> pST->RuntimeServices->ResetSystem (ResetType , EFI_SUCCESS, 0, NULL);
       gST->RuntimeServices->ResetSystem (ResetType , EFI_SUCCESS, 0, NULL);
    }
    else
    {
        CHAR8   *JsonStr8 = NULL;
        //<kuaibai-20180708> EmaIndicationsStatus = pRS->GetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, &AmiEMAIndVarAttributes, &EMAIndicationSize, (VOID *)&EmaIndications);
        //<kuaibai-20181106-Fix oob current value abnormally kept +>
        //EmaIndicationsStatus = gRT->GetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, &AmiEMAIndVarAttributes, &EMAIndicationSize, (VOID *)&EmaIndications);
        EmaIndicationsStatus = gRT->GetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, NULL, &EMAIndicationSize, (VOID *)&EmaIndications);
        DEBUG((DEBUG_ERROR, "\n[SK]FwConfig: get variable Status %r x%\n",EmaIndicationsStatus,EmaIndications));
        //<kuaibai-20181106-Fix oob current value abnormally kept ->
        if((!EFI_ERROR (EmaIndicationsStatus)) && EmaIndications)
        {
            checkBit = ((UINT32)EmaIndications & AMIEMAINDICATIONBIT1);
            DEBUG((DEBUG_ERROR, "\nFirmwareConfigurationBmc: check bit :%d",checkBit));
            CheckSetupXmlBit = ((UINT32)EmaIndications & AMIEMAINDICSENDSETUPXML);
            DEBUG((DEBUG_ERROR, "\nFirmwareConfigurationBmc: CheckSetupXmlBit bit :%d", CheckSetupXmlBit));
        }

        if(checkBit)
        {
            DEBUG((DEBUG_ERROR, "\nIf check bit is TRUE:%d",checkBit));
            FWConfigStatusDisplay(IndexJson) ;
            PERF_START (NULL,"Indic-IndexJson-Post", NULL, 0);
            Status = ConstructIndexAndPost ();
            DEBUG((DEBUG_ERROR, "\n Status of ConstructIndexAndPost :%r",Status));
            PERF_END (NULL,"Indic-IndexJson-Post", NULL, 0);
            DEBUG((DEBUG_ERROR, "\nFirmwareConfigurationEntry:ConstructIndexAndPost Status:%x",Status));
            EmaIndications =((UINT32)EmaIndications & (~AMIEMAINDICATIONBIT1)); // clear bit after posting
        }
        if (CheckSetupXmlBit || ALWAYS_SEND_SETUPDATA_XML)
        {
            Status = SendSetupDataXml ();
            DEBUG((DEBUG_ERROR, "\nFirmwareConfigurationBmc: Status for sending SendSetupDataXml :%r", Status));
            EmaIndications =((UINT32)EmaIndications & (~AMIEMAINDICSENDSETUPXML)); // clear bit after posting
        }
        if (checkBit || CheckSetupXmlBit)
        {
            DEBUG((DEBUG_ERROR, "\n EmaIndications :%x",EmaIndications));
            //<kuaibai-20180708> Status = pRS->SetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, AmiEMAIndVarAttributes, sizeof(UINT32), &EmaIndications);
            //<kuaibai-20181106-Fix oob current value abnormally kept +>
            //Status = gRT->SetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, AmiEMAIndVarAttributes, sizeof(UINT32), &EmaIndications);
            Status = gRT->SetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, OemAttributes, sizeof(UINT32), &EmaIndications);// if variable is not there it will create variable and set variable with 0
            //<kuaibai-20181106-Fix oob current value abnormally kept ->
            DEBUG((DEBUG_ERROR, "\n Status of SetVariable :%r",Status));
        }
        //Update boot options and send patch request
        ConstructBootOptionList (&JsonStr8);
        if( NULL != JsonStr8 )
        {
            SendPatchRequest(JsonStr8);
            FwMemFreePointer( &JsonStr8 );
        }
    }
/////////Index.json ends//////////////

    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : FwNvramUpdateCallback
//
// Description  : Invoked when Nvram updated
//
// Parameter    : EFI_EVENT Event, VOID *Context
//
// Return Value  : VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID FwNvramUpdateCallback( EFI_EVENT Event, VOID *Context)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_TPL     OldTpl = TPL_APPLICATION;
    
    //<kuaibai-20180708> OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);    // guarantees that RestoreTPL won't ASSERT
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);    // guarantees that RestoreTPL won't ASSERT
    //<kuaibai-20180708> pBS->RestoreTPL (TPL_APPLICATION);
    gBS->RestoreTPL (TPL_APPLICATION);
    FreeHiiGlobals();
	Status = ConstructIndexAndPost();
	
    //<kuaibai-20180708> pBS->RaiseTPL (TPL_HIGH_LEVEL);             // guarantees that RestoreTPL won't ASSERT
    gBS->RaiseTPL (TPL_HIGH_LEVEL);             // guarantees that RestoreTPL won't ASSERT
    //<kuaibai-20180708> pBS->RestoreTPL (OldTpl);
    gBS->RestoreTPL (OldTpl);
    return;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : RegisterEventCallback
//
// Description  : Registers the event
//
// Parameter    : EFI_EVENT Event, VOID *Context
//
// Return Value  : VOID
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RegisterEventCallback(
    EFI_GUID *pProtocol, EFI_EVENT_NOTIFY   NotifyFunction,
    VOID *pNotifyContext, EFI_EVENT *pEvent,
    VOID **ppRegistration
)
{
    EFI_STATUS Status;

    //<kuaibai-20180708> Status = pBS->CreateEvent(
    Status = gBS->CreateEvent(
                 EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
                 NotifyFunction, pNotifyContext, pEvent
             );

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    //<kuaibai-20180708> return pBS->RegisterProtocolNotify(pProtocol, *pEvent, ppRegistration);
    return gBS->RegisterProtocolNotify(pProtocol, *pEvent, ppRegistration);
}

/**
    This function will be called once we get gAmiTseSetupEnterGuid or gFwConfigLaunchGuid. It's a callback
    function for these GUIDs events. It will start pushing firmware configuration files.

    @param Event - event signaled by the DXE Core upon installation
    @param Context - event context

    @retval void
    
**/
void FWConfigPushingFilesCallBack (EFI_EVENT Event, void *Context)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_TPL     OldTpl = TPL_APPLICATION;
    
    //<kuaibai-20180708> OldTpl = pBS->RaiseTPL (TPL_HIGH_LEVEL);    
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);    
    //<kuaibai-20180708> pBS->RestoreTPL (TPL_APPLICATION);
    gBS->RestoreTPL (TPL_APPLICATION);
    
    Status = FirmwareConfigurationEntry();
    DEBUG((DEBUG_INFO, "\nFwConfig: FirmwareConfigurationEntry Status:%r",Status));
   
    //<kuaibai-20180708> pBS->RaiseTPL (TPL_HIGH_LEVEL);             
    gBS->RaiseTPL (TPL_HIGH_LEVEL);             
    //<kuaibai-20180708> pBS->RestoreTPL (OldTpl);
    gBS->RestoreTPL (OldTpl);
    
    if(gTSE_EnterSetupEvent)
          //<kuaibai-20180708> pBS->CloseEvent( gTSE_EnterSetupEvent );
          gBS->CloseEvent( gTSE_EnterSetupEvent );
    
    return;
} 

//<kuaibai-20180802-Auto disable if bmc redfish is off+>
/**
    This function will be called to check if bmc redfish service be enabled,
    if disabled we won't go run following OOB process.

    @param NONE

    @retval TRUE
    @retval FALSE
    
**/
BOOLEAN
BmcRedfishServiceEnable()
{
  EFI_STATUS            Status = EFI_SUCCESS;
  EFI_IPMI_TRANSPORT    *IpmiTransportProtocol;
  UINT8                 ResponseDataSize;
  UINT8                 ResponseData[10] = {0};//<kuaibai-20180804-OOB Ipmi Status improvement>
  UINT8                 BMC_Error_Count = 0;
  
  //Locate IPMI transport.
  Status = gBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, (VOID **)&IpmiTransportProtocol);
  
  if (!EFI_ERROR(Status))
  {
    _IPMI_RETRY:
    ResponseDataSize = 10;
    Status = IpmiTransportProtocol->SendIpmiCommand (
                                IpmiTransportProtocol,
                                NETFN_OEM,
                                BMC_LUN,
                                CMD_OEM_GET_REDFISH_SERVICE,
                                NULL,
                                0,
                                ResponseData,
                                &ResponseDataSize
                                );
    DEBUG((DEBUG_ERROR, "\n[SK] OOB SendIpmiCommand = %r !!\n",Status));
//<kuaibai-20180804-OOB Ipmi Status improvement+>
    if (EFI_ERROR(Status))
    {
      if (BMC_Error_Count < 3)
      {
          BMC_Error_Count++;   
          goto _IPMI_RETRY;
      }
      return FALSE;
    }
    
    if (!ResponseData[0])
    {
//<kuaibai-20180804-OOB Ipmi Status improvement->
      DEBUG((DEBUG_ERROR, "\n[SK] redfish service is disabled! \n"));
      return FALSE;
    }
    else
    {
      DEBUG((DEBUG_ERROR, "\n[SK] redfish service is enabled! \n"));
      return TRUE;
    }
  }//if (!EFI_ERROR(Status))
  
  DEBUG((DEBUG_ERROR, "\n[SK] Locate IpmiTransport Protocol fail! \n"));
  return FALSE;
}
//<kuaibai-20180802-Auto disable if bmc redfish is off->

/**
    Entry point for the FirmwareConfiguration driver.

    @param ImageHandle Handle of this driver image
    @param SystemTable Table containing standard EFI services

    @return EFI_STATUS Status
    @retval EFI_SUCCESS for Success else error status

**/
EFI_STATUS FirmwareConfigServiceMgrEntryPoint(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS  Status = EFI_SUCCESS;
    VOID        *gNvramUpdateRegistration = NULL;
    void        *TSESetupEntryRegistration = NULL;
    EFI_GUID AmiPostMgrProtocolGuid = AMI_POST_MANAGER_PROTOCOL_GUID;
//<kuaibai-20181106-Fix oob current value abnormally kept +>
    EFI_GUID SceUtilityIndicationGuid = SCE_UTILITY_INDICATION_VARIABLE_GUID;
    UINT32      AmiEMAIndVarAttributes = 0; 
    UINT32      EmaIndications = 0;
    UINT32      UtlIndications = 0;
    UINTN       UtlIndicSize = sizeof(UINT32);
//<kuaibai-20181106-Fix oob current value abnormally kept ->
    InitAmiLib(ImageHandle,SystemTable); 
    //pBS = gBS; //<kuaibai-20180708>
    LoadStrings (ImageHandle, &gHiiHandle);
    InitializingFWConfigHIIString() ;
    InitializeStdLib ();
    
    //<kuaibai-20180708> Status = pBS->LocateProtocol(&AmiPostMgrProtocolGuid, NULL,(void**) &gPostMgr);
    Status = gBS->LocateProtocol(&AmiPostMgrProtocolGuid, NULL,(void**) &gPostMgr);

    if(TRUE == DumpFirmwareConfigOnly())
    {
    	UpdateFirmWareConfiguaration();
    	return EFI_SUCCESS;
    }

    //<kuaibai-20180802-Auto disable if bmc redfish is off+>
    if(FALSE == BmcRedfishServiceEnable())
    {
      DEBUG ((DEBUG_ERROR, "\n[SK] BMC Redfish Service is disabled!!\n"));
      return Status;
    }
    //<kuaibai-20180802-Auto disable if bmc redfish is off+>
    
    Status = InitRestInstance ();
    
    if (EFI_ERROR(Status))
    {
    	FWConfigStatusDisplay(ConfiguringFWFailed) ;
        DEBUG ((DEBUG_ERROR, "\nFwConfig: Rest instance not found!!\n"));
        return Status;
    }   
    Status = RegisterEventCallback( &gAmiTseNVRAMUpdateGuid, FwNvramUpdateCallback, NULL, &gNvramUpdateEvent, &gNvramUpdateRegistration );
    if (EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "\nFwConfig: Register NVRAM  event Failed !!\n"));
    }

//<kuaibai-20181106-Fix oob current value abnormally kept +>
#if LEGACY_THIRD_PARTY_METHOD == 0 
    Status = gRT->GetVariable (L"UtilityIndication", &SceUtilityIndicationGuid, NULL, &UtlIndicSize, (VOID *)&UtlIndications);
    if (!EFI_ERROR(Status))
    {
        DEBUG((DEBUG_ERROR, "\n[SK]FwConfig: Found SCE change! \n"));
        EmaIndications = AMIEMAINDICATIONBIT1;
        Status = gRT->SetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, OemAttributes, sizeof(UINT32), &EmaIndications);
        DEBUG((DEBUG_ERROR, "\n[SK]FwConfig: set AmiEmaIndications Status %r\n",Status));
    }
    else
    {
        DEBUG((DEBUG_ERROR, "\n[SK]FwConfig: NOT Found SCE change! \n"));
    }
#else
    EmaIndications = AMIEMAINDICATIONBIT1;
    Status = gRT->SetVariable (L"AmiEmaIndications", &gEMAIndicationsGuid, OemAttributes, sizeof(UINT32), &EmaIndications);
    DEBUG((DEBUG_ERROR, "\n[SK]FwConfig: set AmiEmaIndications Status %r\n",Status));
#endif
//<kuaibai-20181106-Fix oob current value abnormally kept ->

#if Remote_Trigger_TseSetupEntryGuid  
    //Callback function will be called if gAmiTseSetupEnterGuid is installed
    Status = RegisterEventCallback( &gAmiTseSetupEnterGuid, FWConfigPushingFilesCallBack, NULL, &gTSE_EnterSetupEvent, &TSESetupEntryRegistration );
#else
    //Callback function will be called if gFwConfigLaunchGuid is installed
    Status = RegisterEventCallback( &gFwConfigLaunchGuid, FWConfigPushingFilesCallBack, NULL, &gTSE_EnterSetupEvent, &TSESetupEntryRegistration );
#endif
    DEBUG ((DEBUG_INFO, "\nFwConfig: Event status for TSESetupEntryGuid or gFwConfigLaunchGuid %r", Status));     
        
	return EFI_SUCCESS;
}

VOID InitializeStdLib ()
{
	struct 	__filedes   *mfd;
	int		i;
	extern 	errno;
	extern 	RETURN_STATUS   EFIerrno;
	
	//Code copied from ShellAppMain to initialize gMD structure, which is used by StdLib library functions
	gMD = AllocateZeroPool(sizeof(struct __MainData));
	if( gMD == NULL )
	{
		return;
	}
	else
	{
		 //Initialize data 
		__sse2_available      = 0;
		_fltused              = 1;
		errno                 = 0;
		EFIerrno              = 0;
		
		gMD->ClocksPerSecond  = 1;
		gMD->AppStartTime     = (clock_t)((UINT32)time(NULL));
		
		// Initialize file descriptors
		mfd = gMD->fdarray;
		for(i = 0; i < (FOPEN_MAX); ++i)
		{
			mfd[i].MyFD = (UINT16)i;
		}
	}
}

EFI_STATUS BuildMapIdList ()
{
    EFI_STATUS      Status = EFI_SUCCESS;
    cJSON           *json = NULL, *jsonMembers, *jsonMember;
    UINT32          MembersSize, i;
    VOID            *MapIdList = (VOID *)NULL;
    
    //<kuaibai-20180708> Status = pBS->LocateProtocol (&gAmiConditionalMapIdListProtocolGuid, NULL, (void**)&gAmiCondMapIdListProtocol);
    Status = gBS->LocateProtocol (&gAmiConditionalMapIdListProtocolGuid, NULL, (void**)&gAmiCondMapIdListProtocol);
    if (!EFI_ERROR(Status))
    {
        Status = gAmiCondMapIdListProtocol->GetConditionalMapIdList (&MapIdList);
        if(EFI_ERROR(Status))
        {
            CHAR8       *JsonBuffer = (VOID *) NULL;
            UINTN       BufferSize = 0;
            
            Status = GetBiosIndexData (&JsonBuffer, &BufferSize);
            DEBUG((DEBUG_ERROR, "\nGetBiosIndexData 1 Status:%r",Status));
            if (!EFI_ERROR (Status) && (0 != BufferSize) && (NULL != JsonBuffer))
            {
                json = cJSON_Parse(JsonBuffer);
                if(json != NULL)
                {
                    jsonMembers = cJSON_GetObjectItem(json, BIOS_INDEX_ATTRIBUTES);
                    
                    if(jsonMembers != NULL )
                    {
                        MembersSize = cJSON_GetArraySize(jsonMembers);  
                        
                        for(i = 0; i < MembersSize; i++)
                        {
                            jsonMember = cJSON_GetArrayItem(jsonMembers, i);
                            if(jsonMember != NULL) {
                                gAmiCondMapIdListProtocol->AddConditionalMapIdToList(Str8to16 (jsonMember->string));
                            }
                        }
                    }
                    cJSON_Delete(json);
                }
                else
                {
                    DEBUG((EFI_D_ERROR, "FirmwareConfiguraiotn: Error to parse Json file - %a\n", cJSON_GetErrorPtr()));
                    Status = EFI_LOAD_ERROR;
                }
            }
        }
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