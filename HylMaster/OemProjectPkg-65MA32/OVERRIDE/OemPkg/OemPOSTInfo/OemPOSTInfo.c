#include "OemPOSTInfo.h"
#include "OemPCIeDev.h"//<ChengXW-20181009 Optimized codes for Display Network Card Info. function>
#include <Library/PcdLib.h> //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation>
#include <Protocol/AmiCpuInfo.h> //<Yaosr001-20200511 Display CPU cores during the post.>
#include <OemPCIeDevCodeId.h>

#define MAX_NEWSTRING_SIZE           0x200 //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation>
#define TSTR_SIZE   0x200

static POST_INFO  gPostInfo={0};

extern EFI_BOOT_SERVICES *gBS;  //<Wangyia001-20170314 Post info display>
extern EFI_RUNTIME_SERVICES *gRT;
extern PCI_DEV_NINFO        PCIDevDB[];//<ChengXW-20181009 Optimized codes for Display Network Card Info. function>

EFI_HII_HANDLE gPostHiiHandle = NULL;

AMI_POST_MANAGER_PROTOCOL *gPostManager = NULL;

AMITSESETUP               TSEVar;  //<Yaosr001-20180804 show post info in the terminal when Quiet Boot set to Enabled>

//<Yaosr001-20180724 display BMC info +>
#define NETFN_OEM         0x3A
#define BMC_LUN             0x00
#define GET_BMC_BUILDTIME   0x25

EFI_IPMI_TRANSPORT *IpmiTransport = NULL;

//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. +>
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
//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. ->

CHAR16*
OemAscii2Unicode (
  OUT CHAR16         *UnicodeStr,
  IN  CHAR8          *AsciiStr
  )
{
  CHAR16 *Str = UnicodeStr;

  if (NULL == AsciiStr || NULL == UnicodeStr) {
    return NULL;
  }

  do {
    *(UnicodeStr++) = (CHAR16) *AsciiStr;
  } while ('\0' != (*(AsciiStr++)));

  return Str;
}

EFI_STATUS
IpmiWaitSetInProgressClear (
  IN  UINT8 LanChannelNumber
  )
{
    EFI_STATUS                      Status;
    UINT8                           ResponseSize;
    UINT16                          Retries = 1000;//<sunhp001-20170719 mantis#15131,add retry times to avoid return device error>
    EFI_IPMI_GET_LAN_CONFIG_PRAM    GetLanParamCmd;
    EFI_GET_LAN_CONFIG_PARAM0       GetSetInProgResponse;

    //<Sunhp001-20180911-remove wait setinprogress clear before bios get/set bmc ip+>
    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "%a Entry...\n", __FUNCTION__));
    #if defined(NO_WAIT_SETINPROGRESS) && (NO_WAIT_SETINPROGRESS==1)
    SERVER_IPMI_DEBUG ((EFI_D_ERROR, "No need wait setinprogress, return EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    #endif
    //<Sunhp001-20180911-remove wait setinprogress clear before bios get/set bmc ip->

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

        //<ChengXW001-20170906 35N16 Mantis #15934, avoid return device error +>
        //if (EFI_ERROR (Status)) {
        //    return Status;
        //}
        
        if ((!EFI_ERROR (Status)) && (GetSetInProgResponse.Param0.SetInProgress == 0)) {
            break;
        }
        //<ChengXW001-20170906 35N16 Mantis #15934, avoid return device error ->
        
        gBS->Stall (1000);
    } while (Retries-- > 0);

    if (++Retries == 0) {
        return EFI_TIMEOUT;
    }
    return EFI_SUCCESS;
}
//<Yaosr001-20180724 display BMC info ->

EFI_STRING
EFIAPI
HiiGetString (
  IN EFI_HII_HANDLE  HiiHandle,
  IN EFI_STRING_ID   StringId,
  IN CONST CHAR8     *Language  OPTIONAL
  );

extern EFI_STATUS
GetConnectedDevices(
    CHAR16                          *DevNumStr,
    CHAR16                          *ControllerNumStr,
    EFI_USB_PROTOCOL                *UsbProtocol,
    CONNECTED_USB_DEVICES_NUM       *Devs
);

extern UINT8
GetMassDeviceName(
    CHAR16              *DevName,
    UINT8               DevAddr,
    EFI_USB_PROTOCOL *UsbProtocol
);


/**

  Acquire the string associated with the Index from SMBIOS structure and return it.
  The caller is responsible for freeing the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.  

**/
extern EFI_STATUS
SmbiosGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
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

//<Yaosr001-20180804 show post info in the terminal when Quiet Boot set to Enabled +>
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
//<Yaosr001-20180804 show post info in the terminal when Quiet Boot set to Enabled ->

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
  //<Yaosr001-20180804 show post info in the terminal when Quiet Boot set to Enabled +>
  if (1 == TSEVar.AMISilentBoot){
    Status = WriteStringToTerminals(message);
  }else{
    Status = gPostManager->DisplayPostMessage(message);
  }
  //<Yaosr001-20180804 show post info in the terminal when Quiet Boot set to Enabled ->
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
  //<Yaosr001-20200511 Display CPU cores during the post. +>
  AMI_CPU_INFO                          *GetCpuInfo = NULL;
  AMI_CPU_INFO_PROTOCOL                 *Cpu;
  EFI_GUID                              gAmiCpuInfoProtocolGuid = AMI_CPU_INFO_PROTOCOL_GUID;
  UINT8                                 NumPhysicalCores;
  //<Yaosr001-20200511 Display CPU cores during the post. ->
  //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation +>
  CHAR16                                *SocketString = NULL;
  UINTN                                 StringLen;
  
  SocketString    =  AllocateZeroPool(MAX_NEWSTRING_SIZE);
  ASSERT_EFI_ERROR (SocketString != NULL);

  AsciiStrToUnicodeStr(PcdGetPtr(PcdAmdSmbiosSocketDesignationSocket0), SocketString);
  StringLen = StrLen(SocketString);
  SocketString[StringLen-1] = 0;
  //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation ->
  for (SocketIndex = 0; SocketIndex < NSOCKETS; SocketIndex++) {
	  gPostInfo.CpuPostInfo[SocketIndex].VersionStr = NULL;
  }
  //<Yaosr001-20200511 Display CPU cores during the post. +>
  Status = pBS->LocateProtocol (&gAmiCpuInfoProtocolGuid, NULL, &Cpu);
  if (EFI_ERROR(Status)) return Status;
  Cpu->GetCpuInfo(Cpu, 0, &GetCpuInfo);
  if(GetCpuInfo->NumHts == 0)
    NumPhysicalCores = GetCpuInfo->NumCores;
  else
    NumPhysicalCores = GetCpuInfo->NumCores / 2;
  //<Yaosr001-20200511 Display CPU cores during the post. ->

  Status = pBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  TRACE((-1, "GetCpuInfo gEfiSmbiosProtocolGuid Status: %r\n", Status));
  if(EFI_ERROR(Status)) return Status;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;

  NewString    = MallocZ (TSTR_SIZE);
  for (i = 0; ; ++i) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    TRACE((-1, "SmbiosRecord Status: %r\n", Status));
    SmbiosType4Record = (SMBIOS_TABLE_TYPE4 *) SmbiosRecord;
    // if Status [6] is set then socket is populated
    if ((SmbiosType4Record->Status & 0x40) == 0) continue; //Not populated.

    StrIndex = SmbiosType4Record->Socket;
    Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &NewString);
    TRACE((-1, "SmbiosType4Record Status: %r\n", Status));
    ASSERT_EFI_ERROR(Status);

    //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation +>
    // string should be 'Px'
    /*if (StrnCmp(NewString, L"P", 1) != 0) continue;
    SocketIndex = StrDecimalToUintn(NewString + StrLen(L"P"));*/
    if (StrnCmp(NewString, SocketString, StringLen-1) != 0) continue;

    SocketIndex = StrDecimalToUintn(NewString + StringLen-1);
    //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation ->
    ASSERT(SocketIndex <= 2);

    if (SocketIndex < NSOCKETS) {
      TRACE((-1, "SocketIndex: %d\n", SocketIndex));
      gPostInfo.CpuPostInfo[SocketIndex].Present = 0xFF;
      gPostInfo.CpuPostInfo[SocketIndex].SocketID = (UINT8)SocketIndex;
      gPostInfo.CpuPostInfo[SocketIndex].Id = *(UINT32 *) &SmbiosType4Record->ProcessorId.Signature;
      gPostInfo.CpuPostInfo[SocketIndex].Freq = SmbiosType4Record->CurrentSpeed;
      gPostInfo.CpuPostInfo[SocketIndex].CurrentCore = NumPhysicalCores;//<Yaosr001-20200511 Display CPU cores during the post.>
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
  EFI_PHYSICAL_ADDRESS                     Tom1;
  EFI_PHYSICAL_ADDRESS                     Tom2;
  EFI_PHYSICAL_ADDRESS                     SystemRam = 0;
  UINT32                                   MemoryFreq = 0;
  UINT32                                   MemSpeed = 0;
  UINT32                                   i;
  SMBIOS_TABLE_TYPE17                      *SmbiosType17Record;
  EFI_SMBIOS_HANDLE                        SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                      *Smbios;
  EFI_SMBIOS_TYPE                          SmbiosType;
  EFI_SMBIOS_TABLE_HEADER                  *SmbiosRecord;
  //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. +>
  CHAR16                                    *MemManufacturer;
  UINT8                                     EccCapbility;
  UINT8                                     StrIndex;
  UINT16                                    MemSingleSize;
  CHAR16                                    *MemLocator;
  UINT8                                     MemNumbers = 0;
  //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. ->
  
  Tom2 = Shr64 (GetTom2(), 20);
  Tom1 = Shr64 (GetTom(), 20);
  SystemRam = (Tom2) ? (Tom2 - 0x1000) : 0;
  SystemRam += Tom1;
  
  gPostInfo.MemPostInfo.memSize = SystemRam >> 10; //GB

  Status = pBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  TRACE((-1, "GetMemoryInfo gEfiSmbiosProtocolGuid Status: %r\n", Status));
  if(EFI_ERROR(Status)) return Status;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  
  for (i = 0; ; ++i) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    TRACE((-1, "SmbiosRecord Status: %r\n", Status));
    SmbiosType17Record = (SMBIOS_TABLE_TYPE17 *) SmbiosRecord;
    
    //<Wangyia001-20170330 revice Post info and some info in Main Screen +>
    //MemSpeed = SmbiosType17Record->Speed;
    MemSpeed = SmbiosType17Record->ConfiguredMemoryClockSpeed;
    //<Wangyia001-20170330 revice Post info and some info in Main Screen ->

    if (MemSpeed != 0) {
      MemoryFreq = MemSpeed;
      TRACE((-1, "Freq = %d\n", MemoryFreq));
      //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. +>
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

      gPostInfo.MemPostExtendInfo[MemNumbers].MemManufacturer = MemManufacturer;
      gPostInfo.MemPostExtendInfo[MemNumbers].EccCapbility = EccCapbility;
      gPostInfo.MemPostExtendInfo[MemNumbers].MemLocator = MemLocator;
      gPostInfo.MemPostExtendInfo[MemNumbers].Size = MemSingleSize >> 10;
      MemNumbers++;
      //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. ->
    }
  }

  gPostInfo.MemPostInfo.memFreq = MemoryFreq;
  gPostInfo.MemPostInfo.MemCount = MemNumbers;//<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post.>
  return EFI_SUCCESS;
}
//<Wangyia001-20170314 Post info display ->

EFI_STATUS GetUsbInfo(VOID)
{
  EFI_STATUS          Status;
  EFI_USB_PROTOCOL    *UsbProtocol;
  EFI_GUID            UsbProtocolGuid = EFI_USB_PROTOCOL_GUID;
  CHAR16              ControllerNumStr[MAX_DEVS_LINE_LENGTH];
  //CHAR16              UsbDevNumStr[MAX_DEVS_LINE_LENGTH];
  UINT8               NextDev;
  POST_DEV_DATA       *UsbDevData;
  //CONNECTED_USB_DEVICES_NUM       DevNum;
  UINT8               i;
  
  Status = pBS->LocateProtocol(&UsbProtocolGuid, NULL, &UsbProtocol);
//####  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
      return Status;
  }
  
  // Number of connected devices
  GetConnectedDevices(gPostInfo.UsbPostInfo.UsbDevNumStr, ControllerNumStr, UsbProtocol, &gPostInfo.UsbPostInfo.DevNum);
  
  // Mass storage device names
  for (i = 0, NextDev = 0; i < 100; i++) {
      UsbDevData = MallocZ(sizeof(POST_DEV_DATA));
      NextDev = GetMassDeviceName((CHAR16 *)UsbDevData->DevName, NextDev, UsbProtocol);
      if (NextDev == 0xFF) {
          MemFreePointer( (VOID **)&UsbDevData );
          break;    // No more devices
      }
      Status=AppendItemLst((T_ITEM_LIST*)&gPostInfo.UsbPostInfo.UsbDevInitCnt, UsbDevData);
      if (NextDev & 0x80) {
          break;     // Last device
      }
  }
  return EFI_SUCCESS;
}

EFI_STATUS GetStorageInfo(VOID)
{
  EFI_STATUS          Status = EFI_UNSUPPORTED;
  UINTN               Index  = 0;
  UINTN               DiskNum= 0;
  DRIVE_INFO          *DriveInfo  = NULL;
  OEM_STORAGE_INFO    *OemStorageInfo = NULL;
  UINT8                AsciiCodeData[MAX_DEVS_LINE_LENGTH];
  POST_DEV_DATA        *StorageDevData;
  OEM_STORAGE_INFO_PROTOCOL   *OemStorageInfoProtocol = NULL;

  //TRACE((-1, __FUNCTION__" Start\n"));

  Status = pBS->LocateProtocol (&gOemStorageInfoProtocolGuid, NULL, &OemStorageInfoProtocol);
  if (EFI_ERROR(Status)) {
    TRACE((-1, "Locate Storage information protocol status: %r\n", Status));
    return Status;
  }

  OemStorageInfoProtocol->GetOemStorageInfo(OemStorageInfoProtocol, &OemStorageInfo);
  if (OemStorageInfo == NULL) {
    TRACE((-1, "Did not found on storages info\n", Status));
    return Status;
  }

  DiskNum = OemStorageInfo->ActualNumber;
  //DEBUG((EFI_D_ERROR, "Found Disk Number:%d OemStorageInfo:0x%x\n", DiskNum, OemStorageInfo));

  for(Index=0; Index<DiskNum; Index++) {
    ZeroMem(&AsciiCodeData, MAX_DEVS_LINE_LENGTH);
    StorageDevData = MallocZ(sizeof(POST_DEV_DATA));
    DriveInfo = OemStorageInfo->DriveInfo[Index];
    Sprintf((char*)AsciiCodeData, "%a", (char*)DriveInfo->HddModelName);
    AsciiStrToUnicodeStr(AsciiCodeData, (CHAR16 *)StorageDevData->DevName);    
    Status=AppendItemLst((T_ITEM_LIST*)&gPostInfo.StoragePostInfo.StorageDevInitCnt, StorageDevData);
  }

  return EFI_SUCCESS;
  
}

VOID DisplayCPUInfo()
{
  EFI_STATUS        Status = EFI_UNSUPPORTED;
  EFI_STRING        Str = NULL;
  EFI_STRING        Text = NULL;
  EFI_STRING        TmpStr = NULL;
  UINT8             Index;
  
  
  Status = GetCPUInfo();
  TRACE((-1, "Get CPU Info status: %r\n", Status));
  if (!EFI_ERROR(Status)){
    //It can not be here if did not get the right processor info.
    Text = MallocZ(TSTR_SIZE);
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_CPU),NULL );
    TmpStr = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_CPUTITLE),NULL );
    if ((Str != NULL) && (Text != NULL) && (TmpStr != NULL)) {
      PostManagerDisplayPostMessage(TmpStr);
      for (Index = 0; Index < NSOCKETS; Index++) {
        if (gPostInfo.CpuPostInfo[Index].Present != 0){
          Swprintf(Text, Str, Index, gPostInfo.CpuPostInfo[Index].VersionStr);
          PostManagerDisplayPostMessage (Text);
          //<Yaosr001-20200511 Display CPU cores during the post. +>
          TmpStr = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_CPU_CORENUM),NULL );
          if (TmpStr != NULL) {
            Swprintf(Text, TmpStr, gPostInfo.CpuPostInfo[Index].CurrentCore);
            PostManagerDisplayPostMessage (Text);
          }
          //<Yaosr001-20200511 Display CPU cores during the post. ->
        }
      } //End for(Index=0; Index < NSOCKETS; Index++)
      MemFreePointer( (VOID **)&Str );
      MemFreePointer( (VOID **)&Text );
    } //End if (Str != NULL) {
  } //End  if (!EFI_ERROR(Status))
  
}

VOID DisplayMemoryInfo()
{
  EFI_STATUS        Status = EFI_UNSUPPORTED;
  EFI_STRING        Str = NULL;
  EFI_STRING        Text = NULL;
  //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. +>
  EFI_STRING        EccString = '\0';
  UINT8             Index;
  //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. ->
  
  Status = GetMemoryInfo();
  TRACE((-1, "GetMemoryInfo: %r\n", Status));
  if(!EFI_ERROR(Status)){
    //It can not be here if did not get the right memory info.
    Text = MallocZ(TSTR_SIZE);
    //<Fancf-20200725 Add strings Chinese display. +>
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_MEMINFO),NULL );
    PostManagerDisplayPostMessage(Str);
    //<Fancf-20200725 Add strings Chinese display. ->
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_MEM),NULL );
    if ((Str != NULL) && (Text != NULL)) {
      Swprintf(Text, Str, gPostInfo.MemPostInfo.memSize,gPostInfo.MemPostInfo.memFreq, gPostInfo.MemPostInfo.MemCount); //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post.>
      PostManagerDisplayPostMessage (Text);
      MemFreePointer( (VOID **)&Str );
      MemFreePointer( (VOID **)&Text );
    }

    //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. +>
    Text = MallocZ(TSTR_SIZE);
    for(Index = 0; Index < gPostInfo.MemPostInfo.MemCount; Index++){
        switch (gPostInfo.MemPostExtendInfo[Index].EccCapbility){
         case 0x01:
             EccString = L"Non-ECC";
             break;
         case 0x02:
             EccString = L"ECC";
             break;
         default:
             break;
        }
        Swprintf(Text, L"%s: %s  %dGB  %s", gPostInfo.MemPostExtendInfo[Index].MemLocator, gPostInfo.MemPostExtendInfo[Index].MemManufacturer, gPostInfo.MemPostExtendInfo[Index].Size, EccString);
        PostManagerDisplayPostMessage (Text);
    }
    if (Text != NULL)
      MemFreePointer( (VOID **)&Text );
    //<Yaosr001-20200506 Display Memory manufacturer/count/EccCapbility/Locator during the post. ->
  } //End if(!EFI_ERROR(Status)){
}

VOID DisplayUSBInfo()
{
  EFI_STATUS        Status = EFI_UNSUPPORTED;
  EFI_STRING        Str = NULL;
  EFI_STRING        Text = NULL;
  CHAR16            *StrPtr;
  POST_DEV_DATA     *DevData;
  
  UINT8 Index;
  
  Status = GetUsbInfo();
  TRACE((-1, "GetUsbInfo: %r\n", Status));

  Text = MallocZ(TSTR_SIZE);
  //Display Usb Devices Total:
  //<Fancf-20200725 Add strings Chinese display. +>
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_USBINFO),NULL );
  PostManagerDisplayPostMessage(Str);
  //<Fancf-20200725 Add strings Chinese display. ->
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_USB),NULL );
  StrPtr = gPostInfo.UsbPostInfo.UsbDevNumStr;
  StrPtr +=6;
  Swprintf(Text,Str,StrPtr);
  PostManagerDisplayPostMessage (Text);
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_USB_DEV),NULL );
  
  for(Index=0;Index<gPostInfo.UsbPostInfo.UsbDevCount;Index++){
    DevData = gPostInfo.UsbPostInfo.UsbDevData[Index];
    Swprintf(Text,Str,Index,DevData->DevName);
    PostManagerDisplayPostMessage (Text);
  }
  MemFreePointer( (VOID **)&Text );
  MemFreePointer( (VOID **)&Str );
  
}

VOID DisplayStorageInfo()
{
  EFI_STATUS        Status = EFI_UNSUPPORTED;
  EFI_STRING        Str = NULL;
  EFI_STRING        Text = NULL;
  POST_DEV_DATA     *DevData;
  UINT8 Index;
  
  Status = GetStorageInfo();
  TRACE((-1, "GetStorageInfo: %r\n", Status));

  Text = MallocZ(TSTR_SIZE);
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_HDD),NULL );
  PostManagerDisplayPostMessage (Str);
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_HDD_DEV),NULL );
  for(Index=0;Index<gPostInfo.StoragePostInfo.StorageDevCount;Index++){
    DevData = gPostInfo.StoragePostInfo.StorageDevData[Index];
    Swprintf(Text,Str,Index,DevData->DevName);
    PostManagerDisplayPostMessage (Text);
  }
  MemFreePointer( (VOID **)&Text );
  MemFreePointer( (VOID **)&Str );

}

//<Chendl001-20170615 Display Network Card Info +>
CHAR16 *GetNetWorkString(UINT16 VenId, UINT16 DevId, UINT16 SubSID)
{
  //<ChengXW-20181009 Optimized codes for Display Network Card Info. function +>
  PCI_DEV_NINFO *PCIDevEntry;

  PCIDevEntry = &PCIDevDB[0];
  //<ChengXW-20181009 Optimized codes for Display Network Card Info. function ->
  
  TRACE ((-1, "[OemDbg]: Get Network String:vid:%X,did:%X,sid:%X.\n", VenId,DevId,SubSID));

  do {
    if ((PCIDevEntry->VendorId == VenId) && (PCIDevEntry->DeviceId == DevId)) {
      if ((PCIDevEntry->SubsystemID == SubSID)||(PCIDevEntry->SubsystemID == 0xFFFF)) {
        return PCIDevEntry->BrandName;
      }
    }

    // End of the string array
    if ((PCIDevEntry->VendorId == 0xFFFF) && (PCIDevEntry->DeviceId == 0xFFFF)) {
      return PCIDevEntry->BrandName;
    }
    PCIDevEntry++;
  } while(TRUE);

  return NULL;
}

EFI_STATUS GetNetworkInfo(VOID)
{
  EFI_STATUS   Status;
  EFI_HANDLE   *Handle;
  UINTN        Number,i,j;
  UINT16       VenId,DevId,SubSID;
  CHAR16       *NetworkNameStr = NULL;
  UINT8        SlotNum = 0;
  UINTN        Index;
  UINT8        NeedShow = 1;
  UINT16       Temp_VenId[MaxNetworkNum],Temp_DevId[MaxNetworkNum];
  PCI_DEV_INFO *dev;
  //<YinQiang001-20170923 bios post network devices message +>
  UINT8        Bus;            
  UINT8        Device;
  NETWORK_POST_INFO Temp_NetWorkPostInfo;
  //<YinQiang001-20170923 bios post network devices message ->

  for (Index = 0; Index < MaxNetworkNum; Index++) {
    gPostInfo.NetWorkPostInfo[Index].NetworkCardStr = NULL;
    gPostInfo.NetWorkPostInfo[Index].OnBoardDev = 1;
    Temp_VenId[Index] = Temp_DevId[Index] = 0x00;
  }

  Index = 0;

  //Get a list of all PCI devices
  Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiPciIoProtocolGuid, NULL, &Number, &Handle);
  if (EFI_ERROR(Status)) return Status;
  TRACE((-1, "GetNetworkInfoNumber: %d\n", Number));
  for(i = 0; i < Number; i++) {
    EFI_PCI_IO_PROTOCOL *PciIo;
    UINT8 PciClass[4];

    Status=pBS->HandleProtocol(Handle[i],&gEfiPciIoProtocolGuid,(VOID**)&PciIo);
    if (EFI_ERROR(Status)) continue;

    dev=(PCI_DEV_INFO*)PciIo;
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16,0x00, 1, &VenId);
    Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16,0x02, 1, &DevId);

    // Skip AMD/Hygon Root Port
    if ((VenId == 0x1D94) || (VenId == 0x1022)) continue;

    Status=PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x08, 1, &PciClass);
    TRACE ((-1, "GetBrandName:vid:%X,did:%X,PciClass[3]:%X\n", VenId,DevId,PciClass[3]));
    // This is a Ethernet controller
    
    //<YinQiang001-20170923 bios post network devices message +>
    //if( PciClass[3] == 0x02 && PciClass[2] == 0x00) {
    //  for(j = 0; j < Index; j++){
    //    if(Temp_VenId[j] == VenId && Temp_DevId[j] == DevId){
    //      NeedShow = 0;
    //      break;
    //    }
    //  }
    if (PciClass[3] == 0x02) {
      /*Bus = dev->AmiSdlPciDevData->Bus;
      Device = dev->AmiSdlPciDevData->Device;*/
      Bus = dev->Address.Addr.Bus;
      Device = dev->Address.Addr.Device;
      TRACE ((-1, "GetBrandName:Bus:%X,Device:%X\n", Bus,Device));
      for (j = 0; j < Index; j++) {
         TRACE ((-1, "GetBrandName:gPostInfo.NetWorkPostInfo[j].Bus:%X,gPostInfo.NetWorkPostInfo[j].Device:%X\n", gPostInfo.NetWorkPostInfo[j].Bus,gPostInfo.NetWorkPostInfo[j].Device));
        if (gPostInfo.NetWorkPostInfo[j].Bus == Bus && gPostInfo.NetWorkPostInfo[j].Device == Device) {
          NeedShow = 0;
          break;
        }
      }
    //<YinQiang001-20170923 bios post network devices message ->

      if(NeedShow == 1) {

        //<YinQiang001-20170923 bios post network devices message +>
        //Temp_VenId[Index] = VenId;
        //Temp_DevId[Index] = DevId;
        //<YinQiang001-20170923 bios post network devices message ->

        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x2e, 2, &SubSID);
        ASSERT_EFI_ERROR (Status); 

        NetworkNameStr = GetNetWorkString(VenId, DevId, SubSID); 
        TRACE ((-1, "GetBrandName:vid:%X,did:%X,sid:%X--%S\n", VenId,DevId,SubSID,NetworkNameStr));
        gPostInfo.NetWorkPostInfo[Index].NetworkCardStr = NetworkNameStr;
        //<YinQiang001-20170923 bios post network devices message +>
        gPostInfo.NetWorkPostInfo[Index].Device = Device;
        gPostInfo.NetWorkPostInfo[Index].Bus = Bus;
        gPostInfo.NetWorkPostInfo[Index].Slot = dev->AmiSdlPciDevData->Slot;
        //<YinQiang001-20170923 bios post network devices message ->

        if (dev->AmiSdlPciDevData->PciDevFlags.Bits.OnBoard == 0)
          gPostInfo.NetWorkPostInfo[Index].OnBoardDev = 0;
        
        //<YinQiang001-20170923 bios post network devices message +>
        for (j = 0; j < Index; j++) {
          if (gPostInfo.NetWorkPostInfo[j].Slot >  dev->AmiSdlPciDevData->Slot) {
            Temp_NetWorkPostInfo = gPostInfo.NetWorkPostInfo[j];
            gPostInfo.NetWorkPostInfo[j] = gPostInfo.NetWorkPostInfo[Index];
            gPostInfo.NetWorkPostInfo[Index] = Temp_NetWorkPostInfo;  
          }
        }
        //<YinQiang001-20170923 bios post network devices message ->

        Index ++;
      } else {
        NeedShow = 1;
        /*Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x2e, 2, &SubSID);
        ASSERT_EFI_ERROR (Status); 

        NetworkNameStr = GetNetWorkString(VenId, DevId, SubSID); 
        TRACE ((-1, "GetBrandName:vid:%X,did:%X,sid:%X--%S\n", VenId,DevId,SubSID,NetworkNameStr));
        gPostInfo.NetWorkPostInfo[Index].NetworkCardStr = NetworkNameStr;*/
      }
    }
  }

  return Status;
}

//
// Collect Network Card Info
//
VOID DisplayNetWorkInfo()
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  EFI_STRING Str = NULL;
  EFI_STRING Text = NULL;
  UINT8      Index = 0;
  //UINT32     SlotNum = 0; //<YinQiang001-20170923 bios post network devices message> //<Fancf-20200810 No longer display pcie device slot num >

  Status = GetNetworkInfo();
  TRACE((-1, "GetNetworkInfo: %r\n", Status));

  Text = MallocZ(TSTR_SIZE);
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_NETWORK),NULL );
  PostManagerDisplayPostMessage (Str);

  while(gPostInfo.NetWorkPostInfo[Index].NetworkCardStr != NULL){
    if (gPostInfo.NetWorkPostInfo[Index].OnBoardDev == 1){
      Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_ONBOARD_DEV),NULL );
      Swprintf(Text,Str,gPostInfo.NetWorkPostInfo[Index].NetworkCardStr);
    } else {
      //<Fancf-20200810 No longer display pcie device slot num +>
      //Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_SLOT_DEV),NULL );
      //<YinQiang001-20170923 bios post network devices message +> 
      //SlotNum = gPostInfo.NetWorkPostInfo[Index].Slot;
      Swprintf(Text,L"%s",gPostInfo.NetWorkPostInfo[Index].NetworkCardStr);
      //Swprintf(Text,Str,SlotNum,gPostInfo.NetWorkPostInfo[Index].NetworkCardStr);
      //<YinQiang001-20170923 bios post network devices message -> 
      //<Fancf-20200810 No longer display pcie device slot num ->
    }
    PostManagerDisplayPostMessage (Text);
    Index ++; 
  }

  MemFreePointer( (VOID **)&Text );
  MemFreePointer( (VOID **)&Str );
}
//<Chendl001-20170615 Display Network Card Info ->

VOID DisplayHotKeyInfo()
{
  
  EFI_STRING        Str = NULL;
  //<Yaosr001-20190731 Support BIOS HotKey select. +>
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
  EFI_STATUS        Status;
  SETUP_DATA        SetupData;
  UINTN             SetupDataSize;
  EFI_GUID          gEfiSetupGuid = SETUP_GUID; 
  
  SetupDataSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
  TRACE ((-1, "gRT->GetVariable status %r, SetupData.BiosHotKeySupport: %d \n", Status, SetupData.BiosHotKeySupport));
  
  if(SetupData.BiosHotKeySupport == 0){
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_HOT_KEY_DISABLE_HELP) ,NULL);
  }else{
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_HOT_KEY_HELP) ,NULL);  
  }
#else
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_HOT_KEY_HELP) ,NULL);
#endif
  //<Yaosr001-20190731 Support BIOS HotKey select. ->
  if (Str != NULL) {
    PostManagerDisplayPostMessage (Str);
    MemFreePointer( (VOID **)&Str );
  }
}

//<Yaosr001-20180724 display BMC info +>
VOID
POSTShowBMCInfoOnScreen()
{
  EFI_STATUS  Status = EFI_SUCCESS;
  CHAR16      uniText[200] = L"\0";
  CHAR16      TempStr[500] = L"\0";

  UINT8       ResponseData[30];
  UINT8       ResponseDataSize;
  CHAR16      UnicodeStr[30] = {0};

  UINT8       index;
  UINT8       CommandDataSize;
  CHAR16      *IPSource[5]; //<Fancf-20200725 Add strings Chinese display. >
  UINT8       IPSourceIndex;

  EFI_BMC_SELF_TEST_LOG_PROTOCOL  *BmcSelfTestProtocol = NULL;
  EFI_IPMI_GET_LAN_CONFIG_PRAM     GetLanParamCmd;
  EFI_GET_LAN_IP_ADDRESS_SRC       LanAddressSource;
  EFI_GET_LAN_MAC_ADDRESS          LanMacAddress;
  EFI_GET_LAN_IP_ADDRESS           LanIPAddress;
  EFI_GET_LAN_SUBNET_MASK          LanSubnetMask;
  EFI_STRING  Str = NULL;//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot.>

  //<Fancf-20200725 Add strings Chinese display. +>
  IPSource[0] = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_UNSPECIFIED),NULL );
  IPSource[1] = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_IPSOURCE_STATIC_ADDRESS),NULL );
  IPSource[2] = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BMC_DHCP),NULL );
  IPSource[3] = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BIOS_DHCP),NULL );
  IPSource[4] = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_IPSOURCE_DYNAMIC_ADDRESS_BMC_NON_DHCP),NULL );
  //<Fancf-20200725 Add strings Chinese display. ->
  
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
    TRACE((-1,  " IPMI : gEfiDxeIpmiTransportProtocolGuid Status: %r \n", Status));
    if (EFI_ERROR(Status)) {
      IpmiTransport = NULL;
      return;
    }    
  }
    
  // Locate the BMC self test protocol 
  Status = pBS->LocateProtocol (
                &gEfiBmcSelfTestLogProtocolGuid,
                NULL,
                (VOID **)&BmcSelfTestProtocol
                );
  SERVER_IPMI_DEBUG ((EFI_D_INFO, "gEfiBmcSelfTestLogProtocolGuid: Status:  %r\n", Status));

  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_STATUS) ,NULL);
  PostManagerDisplayPostMessage(Str);
  // Displaying the string PASSED or FAILED depending upon the Protocol locate status and Error count
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_SELF_TEST) ,NULL);
  //<Fancf-20200725 Add strings Chinese display. +>
  if ((!EFI_ERROR(Status)) && (BmcSelfTestProtocol->BmcSelfTestErrorCount == 0)) {
    Swprintf(TempStr, Str, HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_PASSED) ,NULL));
  } else {
    Swprintf(TempStr, Str, HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_FAILED) ,NULL));
  }
  //<Fancf-20200725 Add strings Chinese display. ->
  PostManagerDisplayPostMessage(TempStr);

  //display BMC revision
  //<Kangmm001-20200428 Fix BMC version show incorrectly +>
  if ((!EFI_ERROR(Status)) && BmcSelfTestProtocol->IsGetDeviceIdCommandPassed) {
    Swprintf(uniText, L"%d.%02x", BmcSelfTestProtocol->GetDeviceIdResponse.MajorFirmwareRev, BmcSelfTestProtocol->GetDeviceIdResponse.MinorFirmwareRev);
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_REVISION) ,NULL);
    Swprintf(TempStr, Str, uniText);
    PostManagerDisplayPostMessage(TempStr);
    //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. +>
    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_VERSION),NULL );
    Swprintf(TempStr, Str, uniText);
    PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);
    //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. ->
  }
  //<Kangmm001-20200428 Fix BMC version show incorrectly ->
    
  //display bmc build time
  Status = pBS->LocateProtocol(
                 &gEfiDxeIpmiTransportProtocolGuid,
                 NULL,
                 (VOID**)&IpmiTransport
                 );
  SERVER_IPMI_DEBUG ((EFI_D_INFO, "%a Entry... gEfiDxeIpmiTransportProtocolGuid Status: %r \n", __FUNCTION__, Status));

  // Get BMC build Time Command
  ResponseDataSize = sizeof(ResponseData);
  if (!EFI_ERROR (Status)) {
    Status = IpmiTransport->SendIpmiCommand (
                IpmiTransport,
                NETFN_OEM,
                BMC_LUN,
                GET_BMC_BUILDTIME,
                NULL,
                0,
                ResponseData,
                &ResponseDataSize
                );
    SERVER_IPMI_DEBUG ((EFI_D_INFO, " In InitBmcbuildTimeDisplay : Status: %r\n", Status));
    if (Status == EFI_SUCCESS) {   
      for (index = sizeof(ResponseData) - 1; index >= 0; index--) {
        if (0x20 == ResponseData[index]) {
          ResponseData[index] = L'\0';
          break;
        }
      }
      Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BUILD_DATE) ,NULL);
      Swprintf(TempStr, Str, OemAscii2Unicode(&UnicodeStr[0], &ResponseData[0]));
      PostManagerDisplayPostMessage(TempStr);  
    }
  }
    
  //share lan
  PostManagerDisplayPostMessage(L" ");
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_SHARED_LAN),NULL );
  PostManagerDisplayPostMessage(Str);
  //
  // Common for all LAN 1 Channel
  //
  GetLanParamCmd.LanChannel.ChannelNumber = BMC_LAN1_CHANNEL_NUMBER;
  GetLanParamCmd.LanChannel.Reserved = 0;
  GetLanParamCmd.LanChannel.GetParam = 0; 
  GetLanParamCmd.SetSelector = 0;
  GetLanParamCmd.BlockSelector = 0;
  CommandDataSize = sizeof (EFI_IPMI_GET_LAN_CONFIG_PRAM);
   
  //Get IP address Source for Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanIpAddressSource;
  ResponseDataSize = sizeof (EFI_GET_LAN_IP_ADDRESS_SRC);

  // Wait until Set In Progress field is cleared  
  Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
  if (!EFI_ERROR (Status)) {
    Status = IpmiTransport->SendIpmiCommand (
                IpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                (UINT8*)&GetLanParamCmd,
                CommandDataSize,
                (UINT8*)&LanAddressSource,
                &ResponseDataSize
                );
    if (!EFI_ERROR (Status)) {
      if (LanAddressSource.AddressSrc > 4) {
        IPSourceIndex = 0;
	    } else {
        IPSourceIndex = LanAddressSource.AddressSrc;
	    }
    } 
  }
  if (EFI_ERROR (Status)) {
	 IPSourceIndex = 0;
  } 
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_IP_SOURCE),NULL );
  Swprintf(TempStr, Str, IPSource[IPSourceIndex]);
  PostManagerDisplayPostMessage(TempStr);
  
  //Get station IP address of Channel-1    
  GetLanParamCmd.ParameterSelector = IpmiLanIpAddress;
  ResponseDataSize = sizeof (EFI_GET_LAN_IP_ADDRESS);

  // Wait until Set In Progress field is cleared
  Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
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
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_IP_ADDR),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr); 
  //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. +>
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_SHARELAN_IP),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);
  //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. ->
  //Get Subnet MASK of Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanSubnetMask;
  ResponseDataSize = sizeof (EFI_GET_LAN_SUBNET_MASK);

  // Wait until Set In Progress field is cleared
  Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
  if (!EFI_ERROR (Status)) {  
    Status = IpmiTransport->SendIpmiCommand (
                IpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                (UINT8*)&GetLanParamCmd,
                CommandDataSize,
                (UINT8*)&LanSubnetMask,
                &ResponseDataSize
                );

    if (!EFI_ERROR (Status)) {
      Swprintf(uniText, L"%d.%d.%d.%d", 
              LanSubnetMask.IpAddress[0],
              LanSubnetMask.IpAddress[1], 
              LanSubnetMask.IpAddress[2], 
              LanSubnetMask.IpAddress[3]);
    } 
  }
  if (EFI_ERROR (Status)) {
    Swprintf(uniText, L"%d.%d.%d.%d", 0,0,0,0);
  }

  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_SUBNET_MASK),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr); 
   
  //Get MAC address of Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanMacAddress;
  ResponseDataSize = sizeof (LanMacAddress);

  // Wait until Set In Progress field is cleared
  Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
  if (!EFI_ERROR (Status)) {
    Status = IpmiTransport->SendIpmiCommand (
                IpmiTransport,
                IPMI_NETFN_TRANSPORT,
                BMC_LUN,
                IPMI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
                (UINT8*)&GetLanParamCmd,
                CommandDataSize,
                (UINT8*)&LanMacAddress,
                &ResponseDataSize
                );

    if (!EFI_ERROR (Status)) {
      Swprintf(uniText, L"%02x-%02x-%02x-%02x-%02x-%02x",
              LanMacAddress.MacAddress[0], 
              LanMacAddress.MacAddress[1], 
              LanMacAddress.MacAddress[2], 
              LanMacAddress.MacAddress[3], 
              LanMacAddress.MacAddress[4], 
              LanMacAddress.MacAddress[5]);
    } 
  }
  if (EFI_ERROR (Status)) {
    Swprintf(uniText, L"%02x-%02x-%02x-%02x-%02x-%02x",0,0,0,0,0,0);
  }

  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_MAC_ADDR),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr); 
  
  //Get Router Gateway IP Address of Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanDefaultGateway;
  ResponseDataSize = sizeof (LanIPAddress);

  // Wait until Set In Progress field is cleared
  Status = IpmiWaitSetInProgressClear (BMC_LAN1_CHANNEL_NUMBER);
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
    Swprintf(uniText, L"%d.%d.%d.%d",0,0,0,0);
  }
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_ROUTER_IP),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr); 

  //dedicated lan
  PostManagerDisplayPostMessage(L" ");
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_DEDICATE_LAN),NULL );
  PostManagerDisplayPostMessage(Str);
  // Common for all LAN 1 Channel
  GetLanParamCmd.LanChannel.ChannelNumber = BMC_LAN2_CHANNEL_NUMBER;
  GetLanParamCmd.LanChannel.Reserved = 0;
  GetLanParamCmd.LanChannel.GetParam = 0; 
  GetLanParamCmd.SetSelector = 0;
  GetLanParamCmd.BlockSelector = 0;
  CommandDataSize = sizeof (EFI_IPMI_GET_LAN_CONFIG_PRAM);
    
  //Get IP address Source for Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanIpAddressSource;
  ResponseDataSize = sizeof (EFI_GET_LAN_IP_ADDRESS_SRC);

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
                (UINT8*)&LanAddressSource,
                &ResponseDataSize
                );

    if (!EFI_ERROR (Status)) {
      if (LanAddressSource.AddressSrc > 4) {
        IPSourceIndex = 0;
      } else {
        IPSourceIndex = LanAddressSource.AddressSrc ;
      }
    } 
  }
  if (EFI_ERROR (Status)) {
	  IPSourceIndex = 0;
  } 
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_IP_SOURCE),NULL );
  Swprintf (TempStr, Str, IPSource[IPSourceIndex]);
  PostManagerDisplayPostMessage(TempStr); 
     
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
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_IP_ADDR),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr);
  //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. +>
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BMC_DEDICATEDLAN_IP),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);
  //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. ->
  //Get Subnet MASK of Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanSubnetMask;
  ResponseDataSize = sizeof (EFI_GET_LAN_SUBNET_MASK);
   
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
                (UINT8*)&LanSubnetMask,
                &ResponseDataSize
                );

    if (!EFI_ERROR (Status)) {
      Swprintf(uniText, L"%d.%d.%d.%d",  
              LanSubnetMask.IpAddress[0], 
              LanSubnetMask.IpAddress[1], 
              LanSubnetMask.IpAddress[2], 
              LanSubnetMask.IpAddress[3]);
    } 
  }
  if (EFI_ERROR (Status)) {
    Swprintf(uniText, L"%d.%d.%d.%d", 0,0,0,0);
  } 
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_SUBNET_MASK),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr); 

    //Get MAC address of Channel-1
    //
    GetLanParamCmd.ParameterSelector = IpmiLanMacAddress;
    ResponseDataSize = sizeof (LanMacAddress);

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
                (UINT8*)&LanMacAddress,
                &ResponseDataSize
                );

    if (!EFI_ERROR (Status)) {
      Swprintf(uniText, L"%02x-%02x-%02x-%02x-%02x-%02x",
              LanMacAddress.MacAddress[0], 
		          LanMacAddress.MacAddress[1], 
              LanMacAddress.MacAddress[2], 
              LanMacAddress.MacAddress[3], 
              LanMacAddress.MacAddress[4], 
              LanMacAddress.MacAddress[5]);
    } 
  }
  if (EFI_ERROR (Status)) {
    Swprintf(uniText, L"%02x-%02x-%02x-%02x-%02x-%02x",0,0,0,0,0,0);
  }
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_MAC_ADDR),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr);
   
  //Get Router Gateway IP Address of Channel-1
  GetLanParamCmd.ParameterSelector = IpmiLanDefaultGateway;
  ResponseDataSize = sizeof (LanIPAddress);

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
    Swprintf(uniText, L"%d.%d.%d.%d",0,0,0,0);
  }
  Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_ROUTER_IP),NULL );
  Swprintf(TempStr, Str, uniText);
  PostManagerDisplayPostMessage(TempStr); 

  MemFreePointer( (VOID **)&Str );//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot.>
}
//<Yaosr001-20180724 display BMC info ->

//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. +>
VOID DispayBIOSVersionInfo(){

//<Hujf1001-20211230 Fix system hang when BIOS Version name too long +>
    CHAR16     *TempStr;
    EFI_STRING Str = NULL;

    Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_BIOS_VERSION),NULL );
    TempStr = AllocateZeroPool(2 + AsciiStrLen( CONVERT_TO_STRING(BIOS_TAG) ) *2 + StrLen(Str) *2);
    Swprintf(TempStr, Str, CONVERT_TO_STRING(BIOS_TAG));

    PostManagerDisplayQuietBootMessage(TempStr, ShowInfoRows++);
    MemFreePointer( (VOID **)&TempStr );
    MemFreePointer( (VOID **)&Str );
//<Hujf1001-20211230 Fix system hang when BIOS Version name too long ->
}
//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot. ->

//<Yaosr002-20200506 Display System manufacturer and product name during post. +>
VOID DisplaySysMfgAndProductName()
{
   CHAR16          *SystemManufacturer;
   CHAR16          *ProductName;
   EFI_STRING      Text = NULL;
   EFI_STRING      Str = NULL;

   // SystemManufacturer
   SystemManufacturer = (CHAR16 *)PcdGetPtr(PcdSystemManufacturer);
   // SystemProductName
   ProductName = (CHAR16 *)PcdGetPtr(PcdSystemProductName);

   Text = MallocZ(TSTR_SIZE);
   if (Text != NULL) {
     Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_SYSTEMINFO),NULL );
     PostManagerDisplayPostMessage(Str);
     
     Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_MANUFACTURER),NULL );
     Swprintf(Text, Str, SystemManufacturer);
     PostManagerDisplayPostMessage(Text);

     Str = HiiGetString( gPostHiiHandle, STRING_TOKEN(STR_POST_INFO_PRODUCTNAME),NULL );
     Swprintf(Text, Str, ProductName);
     PostManagerDisplayPostMessage(Text);

     MemFreePointer( (VOID **)&Text );
   }
}
//<Yaosr002-20200506 Display System manufacturer and product name during post. ->

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
VOID OemShowPostInfo(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
  EFI_STATUS                          Status;
  EFI_GUID                          AmiTseSetupguid = AMITSESETUP_GUID;
  //AMITSESETUP                       TSEVar;  //<Yaosr001-20180804 show post info in the terminal when Quiet Boot set to Enabled>
  UINTN                             Size;
  
  Size = sizeof (AMITSESETUP);        
  Status = gRT->GetVariable(L"AMITSESetup", \
                             &AmiTseSetupguid, \
                             NULL, \
                             &Size, \
                             &TSEVar );
  //if (TSEVar.AMISilentBoot == 0) return;
  
  ShowInfoRows = PcdGet8(PcdShowInfoCount);  //<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot.>
  
  gPostHiiHandle = HiiHandle;
  
  //Next_Line();
  
  DisplayCPUInfo();
  Next_Line();
  
  DisplayMemoryInfo();
  Next_Line();
  
  DisplayUSBInfo();
  Next_Line();
  
  DisplayStorageInfo();
  Next_Line();
  
  //<Chendl001-20170615 Display Network Card Info +>
  DisplayNetWorkInfo();
  Next_Line();
  //<Chendl001-20170615 Display Network Card Info ->

  //<Yaosr002-20200506 Display System manufacturer and product name during post. +>
  DisplaySysMfgAndProductName();
  Next_Line();
  //<Yaosr002-20200506 Display System manufacturer and product name during post. ->

  DispayBIOSVersionInfo();//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot.>
  
  //<Yaosr001-20180724 display BMC info +>
  POSTShowBMCInfoOnScreen();
  Next_Line();
  //<Yaosr001-20180724 display BMC info ->
  
  DisplayHotKeyInfo();
  Next_Line();

  PcdSet8(PcdShowInfoCount, ShowInfoRows);//<Yaosr001-20200530 Display BIOS version/BMC version/BMC IP to screen when quiet boot.>
}
//<lvych00120160816->
