#include "AliPostInfo.h"
#include <Library/PcdLib.h> 
#include <Protocol/AmiCpuInfo.h>


static POST_INFO  gPostInfo={0};

extern EFI_BOOT_SERVICES *gBS;  //<Wangyia001-20170314 Post info display>
extern EFI_RUNTIME_SERVICES *gRT;

EFI_HII_HANDLE gPostHiiHandle = NULL;

AMI_POST_MANAGER_PROTOCOL *gPostManager = NULL;

AMITSESETUP               TSEVar;  

#define NETFN_OEM         0x3A
#define BMC_LUN             0x00
#define GET_BMC_BUILDTIME   0x25

EFI_IPMI_TRANSPORT *IpmiTransport = NULL;


EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground = POST_FONT_FG;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background = POST_FONT_BG;
UINT8    ShowInfoRows = 0;

EFI_STATUS PostManagerDisplayQuietBootMessage( CHAR16 *message, UINT8 ShowInfoRows)
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(gPostManager == NULL){
    Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid,NULL,&gPostManager);
    if (EFI_ERROR(Status)) {
      gPostManager=NULL;
      return Status;
    }
  }

  if (1 == TSEVar.AMISilentBoot){
    Status = gPostManager->DisplayQuietBootMessage(message, 0, 17*ShowInfoRows, CA_AttributeLeftTop,
            Foreground, Background);
  }

  return Status;
}

EFI_STATUS
IpmiWaitSetInProgressClear (
  IN  UINT8 LanChannelNumber
  )
{
    EFI_STATUS                      Status;
    UINT8                           ResponseSize;
    UINT16                          Retries = 1000;
    EFI_IPMI_GET_LAN_CONFIG_PRAM    GetLanParamCmd;
    EFI_GET_LAN_CONFIG_PARAM0       GetSetInProgResponse;

    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a Entry...\n", __FUNCTION__));
    #if defined(NO_WAIT_SETINPROGRESS) && (NO_WAIT_SETINPROGRESS==1)
    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "No need wait setinprogress, return EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    #endif

    GetLanParamCmd.LanChannel.ChannelNumber = LanChannelNumber;
    GetLanParamCmd.LanChannel.Reserved = 0;
    GetLanParamCmd.LanChannel.GetParam = 0; 
    GetLanParamCmd.ParameterSelector = 0;       
    GetLanParamCmd.SetSelector = 0;
    GetLanParamCmd.BlockSelector = 0;

    //
    // Wait for Set-In progress bit to clear
    //
    do {
        ResponseSize = sizeof (GetSetInProgResponse);
        Status = IpmiTransport->SendIpmiCommand (
                     IpmiTransport,
                     IPMI_NETFN_TRANSPORT,
                     BMC_LUN,
                     IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                     (UINT8*) &GetLanParamCmd,
                     sizeof (GetLanParamCmd),
                     (UINT8*) &GetSetInProgResponse,
                     &ResponseSize );
        
        if ((!EFI_ERROR (Status)) && (GetSetInProgResponse.Param0.SetInProgress == 0)) {
            break;
        }
        
        gBS->Stall (1000);
    } while (Retries-- > 0);

    if (++Retries == 0) {
        return EFI_TIMEOUT;
    }
    return EFI_SUCCESS;
}


EFI_STRING
EFIAPI
HiiGetString (
  IN EFI_HII_HANDLE  HiiHandle,
  IN EFI_STRING_ID   StringId,
  IN CONST CHAR8     *Language  OPTIONAL
  );



/**
    function to free the pointers

    @param ptr 

    @retval void

**/
VOID MemFreePointer( VOID **ptr )
{
  if ( ( ptr == NULL ) || ( *ptr == NULL ) )
    return;

  pBS->FreePool( *ptr );
  *ptr = NULL;
}

EFI_STATUS WriteStringToTerminals(CHAR16 *Message)
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       *HandleBuffer = NULL;
  UINTN                            HandleCount;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *pConOut;
  VENDOR_DEVICE_PATH               *pDp;
  UINTN                            Index;
      
  Status = pBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleTextOutProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer);
   
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  for (Index = 0; Index < HandleCount; Index++) {
    Status = pBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }
           
    pDp = (VENDOR_DEVICE_PATH *)DPGetLastNode(DevicePath);
    if ((MESSAGING_DEVICE_PATH == pDp->Header.Type) && (MSG_VENDOR_DP == pDp->Header.SubType)) {
      if ((0 != guidcmp(&pDp->Guid,&gEfiVT100Guid)) &&   
          (0 != guidcmp(&pDp->Guid,&gEfiVT100PlusGuid)) &&   
          (0 != guidcmp(&pDp->Guid,&gEfiVTUTF8Guid)) &&   
          (0 != guidcmp(&pDp->Guid,&gEfiPcAnsiGuid))) {   
        continue;
      }

      Status = pBS->HandleProtocol(
                      HandleBuffer[Index],
                      &gEfiSimpleTextOutProtocolGuid,
                      (VOID **)&pConOut);
      if (EFI_ERROR (Status)) {
        continue;
      }

      pConOut->EnableCursor (pConOut, FALSE);
      pConOut->OutputString (pConOut, L"\r\n");
      pConOut->OutputString (pConOut, Message);
    }
  }
  
  MemFreePointer((VOID **) &HandleBuffer);
  return (Status);
}

EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if(gPostManager == NULL){
    Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid,NULL,&gPostManager);    
    if (EFI_ERROR(Status)) {
      gPostManager=NULL;
      return Status;
    }
  }
  if (1 == TSEVar.AMISilentBoot){
    Status = WriteStringToTerminals(message);
  }else{
    Status = gPostManager->DisplayPostMessage(message);
  }
  return Status;
}

VOID Next_Line()
{
  EFI_STRING        Str = NULL;
  
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BLANK), NULL );
  if (Str != NULL) {
    PostManagerDisplayPostMessage (Str);
    MemFreePointer( (VOID **)&Str );
  }
  
}

EFI_STATUS GetCPUInfo(VOID)
{
  EFI_STATUS Status;
  
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  SMBIOS_TABLE_TYPE4                    *SmbiosType4Record;
  EFI_SMBIOS_TYPE                       SmbiosType;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord;
  CHAR16                                *NewString;
  CHAR16                                *VersionString;
  UINT8                                 i;
  UINTN                                 SocketIndex;
  UINT8                                 StrIndex;
  AMI_CPU_INFO                          *GetCpuInfo = NULL;
  AMI_CPU_INFO_PROTOCOL                 *Cpu;
  EFI_GUID                              gAmiCpuInfoProtocolGuid = AMI_CPU_INFO_PROTOCOL_GUID;
  UINT8                                 NumPhysicalCores;
  CHAR16                                *SocketString = NULL;
  UINTN                                 StringLen;
  
  SocketString    =  AllocateZeroPool(TSTR_SIZE);
  ASSERT_EFI_ERROR (SocketString != NULL);

  AsciiStrToUnicodeStr(PcdGetPtr(PcdAmdSmbiosSocketDesignationSocket0), SocketString);
  StringLen = StrLen(SocketString);
  SocketString[StringLen-1] = 0;

  for (SocketIndex = 0; SocketIndex < NSOCKETS; SocketIndex++) {
	  gPostInfo.CpuPostInfo[SocketIndex].VersionStr = NULL;
  }
  Status = pBS->LocateProtocol (&gAmiCpuInfoProtocolGuid, NULL, &Cpu);
  if (EFI_ERROR(Status)) return Status;
  Cpu->GetCpuInfo(Cpu, 0, &GetCpuInfo);
  if(GetCpuInfo->NumHts == 0)
    NumPhysicalCores = GetCpuInfo->NumCores;
  else
    NumPhysicalCores = GetCpuInfo->NumCores / 2;

  Status = pBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  DEBUG((DEBUG_INFO, "GetCpuInfo gEfiSmbiosProtocolGuid Status: %r\n", Status));
  if(EFI_ERROR(Status)) return Status;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;

  NewString    = MallocZ (TSTR_SIZE);
  for (i = 0; ; ++i) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    DEBUG((DEBUG_INFO, "SmbiosRecord Status: %r\n", Status));
    SmbiosType4Record = (SMBIOS_TABLE_TYPE4 *) SmbiosRecord;
    // if Status [6] is set then socket is populated
    if ((SmbiosType4Record->Status & 0x40) == 0) continue; //Not populated.

    StrIndex = SmbiosType4Record->Socket;
    Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &NewString);
    DEBUG((DEBUG_INFO, "SmbiosType4Record Status: %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    if (StrnCmp(NewString, SocketString, StringLen-1) != 0) continue;

    SocketIndex = StrDecimalToUintn(NewString + StringLen-1);
    ASSERT(SocketIndex <= 2);

    if (SocketIndex < NSOCKETS) {
      DEBUG((DEBUG_INFO, "SocketIndex: %d\n", SocketIndex));
      gPostInfo.CpuPostInfo[SocketIndex].Present = 0xFF;
      gPostInfo.CpuPostInfo[SocketIndex].SocketID = (UINT8)SocketIndex;
      gPostInfo.CpuPostInfo[SocketIndex].Id = *(UINT32 *) &SmbiosType4Record->ProcessorId.Signature;
      gPostInfo.CpuPostInfo[SocketIndex].Freq = SmbiosType4Record->CurrentSpeed;
      gPostInfo.CpuPostInfo[SocketIndex].CurrentCore = NumPhysicalCores;
      VersionString = MallocZ (TSTR_SIZE);
      StrIndex = SmbiosType4Record->ProcessorVersion;
      if (SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &VersionString) != EFI_SUCCESS) {
        Swprintf(VersionString, L"Unknown");
      }
      gPostInfo.CpuPostInfo[SocketIndex].VersionStr = VersionString;
    }
  }
  MemFreePointer( (VOID **)&NewString );
  return EFI_SUCCESS;
}

EFI_STATUS 
GetMemoryInfo(VOID)
{
  EFI_STATUS                               Status;
  EFI_PHYSICAL_ADDRESS                     SystemRam = 0;
  UINT32                                   MemoryFreq = 0;
  UINT32                                   MemSpeed = 0;
  UINT32                                   i;
  SMBIOS_TABLE_TYPE17                      *SmbiosType17Record;
  EFI_SMBIOS_HANDLE                        SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                      *Smbios;
  EFI_SMBIOS_TYPE                          SmbiosType;
  EFI_SMBIOS_TABLE_HEADER                  *SmbiosRecord;
  CHAR16                                    *MemManufacturer;
  UINT8                                     EccCapbility;
  UINT8                                     StrIndex;
  UINT16                                    MemSingleSize;
  CHAR16                                    *MemLocator;
  UINT8                                     MemNumbers = 0;


  Status = pBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  DEBUG((DEBUG_INFO, "GetMemoryInfo gEfiSmbiosProtocolGuid Status: %r\n", Status));
  if(EFI_ERROR(Status)) return Status;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  
  for (i = 0; ; ++i) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    DEBUG((DEBUG_INFO, "SmbiosRecord Status: %r\n", Status));
    SmbiosType17Record = (SMBIOS_TABLE_TYPE17 *) SmbiosRecord;
    
    MemSpeed = SmbiosType17Record->ConfiguredMemoryClockSpeed;

    if (MemSpeed != 0) {
      MemoryFreq = MemSpeed;
      DEBUG((DEBUG_INFO, "Freq = %d\n", MemoryFreq));
      //Manufacturer
      MemManufacturer = MallocZ (TSTR_SIZE);
      StrIndex = SmbiosType17Record->Manufacturer;
      Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType17Record + SmbiosType17Record->Hdr.Length), StrIndex, &MemManufacturer);
      ASSERT_EFI_ERROR(Status);
      //Locator
      MemLocator = MallocZ (TSTR_SIZE);
      StrIndex = SmbiosType17Record->DeviceLocator;
      Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType17Record + SmbiosType17Record->Hdr.Length), StrIndex, &MemLocator);
      ASSERT_EFI_ERROR(Status);
      //Ecc
      EccCapbility = (((SmbiosType17Record->TotalWidth - SmbiosType17Record->DataWidth)) & BIT3 )? 2 : 1;//<Yaosr001-20200630 Fixed that ECC error.>
      //single memory size
      MemSingleSize = SmbiosType17Record->Size;
      if (MemSingleSize == 0x7FFF) {
          MemSingleSize = SmbiosType17Record->ExtendedSize;
      }
      SystemRam += MemSingleSize;
      
      gPostInfo.MemPostExtendInfo[MemNumbers].MemManufacturer = MemManufacturer;
      gPostInfo.MemPostExtendInfo[MemNumbers].EccCapbility = EccCapbility;
      gPostInfo.MemPostExtendInfo[MemNumbers].MemLocator = MemLocator;
      gPostInfo.MemPostExtendInfo[MemNumbers].Size = MemSingleSize >> 10;
      MemNumbers++;
    }
  }
  gPostInfo.MemPostInfo.memSize = SystemRam >> 10; //GB
  gPostInfo.MemPostInfo.memFreq = MemoryFreq;
  gPostInfo.MemPostInfo.MemCount = MemNumbers;
  return EFI_SUCCESS;
}

VOID DisplayCPUBrandName()
{
    EFI_STATUS        Status = EFI_UNSUPPORTED;
    EFI_STRING        Str = NULL;
    EFI_STRING        Text = NULL;
    
    Status = GetCPUInfo();
    DEBUG((DEBUG_INFO, "Get CPU Info status: %r\n", Status));
    if (!EFI_ERROR(Status)){
        Text = MallocZ(TSTR_SIZE);
        Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_QUITE_BOOT_POST_INFO_CPU),NULL );
        Swprintf(Text, Str, gPostInfo.CpuPostInfo[0].VersionStr);
        PostManagerDisplayQuietBootMessage(Text, ShowInfoRows++);
        MemFreePointer( (VOID **)&Str );
        MemFreePointer( (VOID **)&Text );
    }
}

VOID DisplayTotalMemoryInfo()
{
    EFI_STATUS        Status = EFI_UNSUPPORTED;
    EFI_STRING        Str = NULL;
    EFI_STRING        Text = NULL;
    EFI_STRING        EccString = '\0';
    
    Status = GetMemoryInfo();
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_QUITE_BOOT_POST_INFO_MEM),NULL );
    Text = MallocZ(TSTR_SIZE);
    if ((Str != NULL) && (Text != NULL)) {
        Swprintf(Text, Str, gPostInfo.MemPostInfo.memSize, gPostInfo.MemPostInfo.MemCount, gPostInfo.MemPostInfo.memFreq);
        PostManagerDisplayQuietBootMessage(Text, ShowInfoRows++);
        MemFreePointer( (VOID **)&Str );
        MemFreePointer( (VOID **)&Text );
    }
}

VOID DisplayHotKeyInfo()
{
  EFI_STRING        Str = NULL;

#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
  EFI_STATUS        Status;
  SETUP_DATA        SetupData;
  UINTN             SetupDataSize;
  EFI_GUID          gEfiSetupGuid = SETUP_GUID; 
  
  SetupDataSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
  DEBUG((DEBUG_INFO, "gRT->GetVariable status %r, SetupData.BiosHotKeySupport: %d \n", Status, SetupData.BiosHotKeySupport));
  
  if(SetupData.BiosHotKeySupport == 0){
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_HOT_KEY_DISABLE_HELP) ,NULL);
  }else{
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_HOT_KEY_HELP) ,NULL);  
  }
#else
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_HOT_KEY_HELP) ,NULL);
#endif

  if (Str != NULL) {
    PostManagerDisplayPostMessage (Str);
    MemFreePointer( (VOID **)&Str );
  }
}

VOID
DisplayBMCVersion()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR16      uniText[20] = L"\0";
  CHAR16      TempStr[50] = L"\0";
  EFI_BMC_SELF_TEST_LOG_PROTOCOL  *BmcSelfTestProtocol = NULL;
  EFI_STRING  Str = NULL;

  if (NULL == pBS) {
    return;
  }
  // Locate the BMC self test protocol 
  Status = pBS->LocateProtocol (
                &gEfiBmcSelfTestLogProtocolGuid,
                NULL,
                (VOID **)&BmcSelfTestProtocol
                );
  SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiBmcSelfTestLogProtocolGuid: Status:  %r\n", Status));

  //display BMC revision
  if ((!EFI_ERROR(Status)) && BmcSelfTestProtocol->IsGetDeviceIdCommandPassed) {
    Swprintf(uniText, L"%d.%02x", BmcSelfTestProtocol->GetDeviceIdResponse.MajorFirmwareRev, BmcSelfTestProtocol->GetDeviceIdResponse.MinorFirmwareRev);
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_VERSION),NULL );
    if (Str != NULL) {
        Swprintf(TempStr, Str, uniText);
        PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);
        MemFreePointer( (VOID **)&Str );
    }
  }
}

VOID
DisplayBMCIPOnScreen()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR16      uniText[20] = L"\0";
  CHAR16      TempStr[50] = L"\0";
  UINT8       ResponseDataSize;
  UINT8       CommandDataSize;
  EFI_IPMI_GET_LAN_CONFIG_PRAM     GetLanParamCmd;
  EFI_GET_LAN_IP_ADDRESS           LanIPAddress;
  EFI_STRING  Str = NULL;
  
  // Locate the IPMI transport protocol
  if (NULL == pBS) {
    return;
  }

  if (NULL == IpmiTransport){
    Status = pBS->LocateProtocol(
                &gEfiDxeIpmiTransportProtocolGuid,
                NULL,
                (VOID **)&IpmiTransport
                );
    DEBUG((DEBUG_ERROR,  "%a IPMI : gEfiDxeIpmiTransportProtocolGuid Status: %r \n", __FUNCTION__, Status));
    if (EFI_ERROR(Status)) {
      IpmiTransport = NULL;
      return;
    }    
  }

  //dedicated lan
  // Common for all LAN 1 Channel
  GetLanParamCmd.LanChannel.ChannelNumber = BMC_LAN2_CHANNEL_NUMBER;
  GetLanParamCmd.LanChannel.Reserved = 0;
  GetLanParamCmd.LanChannel.GetParam = 0; 
  GetLanParamCmd.SetSelector = 0;
  GetLanParamCmd.BlockSelector = 0;
  CommandDataSize = sizeof (EFI_IPMI_GET_LAN_CONFIG_PRAM);
    
  //Get station IP address of Channel-1    
  GetLanParamCmd.ParameterSelector = IpmiLanIpAddress;
  ResponseDataSize = sizeof (EFI_GET_LAN_IP_ADDRESS);

  // Wait until Set In Progress field is cleared
  Status = IpmiWaitSetInProgressClear (BMC_LAN2_CHANNEL_NUMBER);
  if (!EFI_ERROR (Status)) {
    Status = IpmiTransport->SendIpmiCommand (
                IpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                (UINT8*)&GetLanParamCmd,
                CommandDataSize,
                (UINT8*)&LanIPAddress,
                &ResponseDataSize
                );

    if (!EFI_ERROR (Status)) {
      Swprintf(uniText, L"%d.%d.%d.%d", 
              LanIPAddress.IpAddress[0], 
              LanIPAddress.IpAddress[1], 
              LanIPAddress.IpAddress[2], 
              LanIPAddress.IpAddress[3]);
    }
  }
  if (EFI_ERROR (Status)) {
    Swprintf(uniText, L"%d.%d.%d.%d", 0,0,0,0);
  }

  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_QUITE_BOOT_POST_INFO_BMC_DEDICATEDLAN_IP),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);

  MemFreePointer( (VOID **)&Str );
}

VOID DispayBIOSVersionInfo()
{
    CHAR16     *TempStr;
    EFI_STRING Str = NULL;

    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BIOS_VERSION),NULL );
    TempStr = AllocateZeroPool(2 + AsciiStrLen( CONVERT_TO_STRING(OEM_BIOS_TAG) ) *2 + StrLen(Str) *2);
    Swprintf(TempStr, Str, CONVERT_TO_STRING(OEM_BIOS_TAG));

    PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);
    MemFreePointer( (VOID **)&TempStr );
    MemFreePointer( (VOID **)&Str );
}

VOID DisplayProductName()
{
    CHAR16          *ProductName;
    EFI_STRING      Text = NULL;
    EFI_STRING      Str = NULL;

    // SystemProductName
    ProductName = (CHAR16 *)PcdGetPtr(PcdSystemProductName);
    Text = MallocZ(TSTR_SIZE);
    if (Text != NULL) {
        Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_QUITE_BOOT_POST_INFO_PRODUCTNAME),NULL );
        Swprintf(Text, Str, ProductName);
        PostManagerDisplayQuietBootMessage(Text, ShowInfoRows++);

        MemFreePointer( (VOID **)&Text );
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   AfterInitPostScreen2
//
// Description: Display Processor&Memory information if quiet boot disabled.
//
// Input:       VOID
//
// Output:      VOID
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID AliShowPostInfo(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
  EFI_STATUS                          Status;
  EFI_GUID                          AmiTseSetupguid = AMITSESETUP_GUID;
  UINTN                             Size;
  
  Size = sizeof (AMITSESETUP);        
  Status = gRT->GetVariable(L"AMITSESetup", \
                             &AmiTseSetupguid, \
                             NULL, \
                             &Size, \
                             &TSEVar );
  
  ShowInfoRows = PcdGet8(PcdShowInfoCount); 
  
  gPostHiiHandle = HiiHandle;

  DispayBIOSVersionInfo();
  
  DisplayBMCVersion();
  
  DisplayProductName();
  
  DisplayCPUBrandName();
  
  DisplayTotalMemoryInfo();
  
  DisplayBMCIPOnScreen();
  
  DisplayHotKeyInfo();
  Next_Line();

  PcdSet8(PcdShowInfoCount, ShowInfoRows);
}

