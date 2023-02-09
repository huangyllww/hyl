//**********************************************************************
// $Header:
//
// $Revision: 0.01
//
// $Date:  <Lizhq-20161217> 
//
//**********************************************************************
#include <OemIpmiNetFnDefinitions.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/OemStorageInfoProtocol.h>

static  OEM_STORAGE_INFO_PROTOCOL   *OemStorageInfoProtocol = NULL;

EFI_STATUS
ServerRepositoryStorageInfoEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS          Status = EFI_UNSUPPORTED;
  UINT8               Index  = 0; //<lub_1231 +>
  UINTN               DiskNum= 0;
  PACKED_OBHDD_INFO   OemHDDInfo;
  DRIVE_INFO          *DriveInfo  = NULL;
  OEM_STORAGE_INFO    *OemStorageInfo = NULL;
  UINT8   			  ResponseDataSize=0;
  
  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n")); 

  Status = gBS->LocateProtocol (&gOemStorageInfoProtocolGuid, NULL, &OemStorageInfoProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Locate Storage information protocol status: %r\n", Status));
    return Status;
  }

  OemStorageInfoProtocol->GetOemStorageInfo(OemStorageInfoProtocol, &OemStorageInfo);
  if (OemStorageInfo == NULL) {
    DEBUG((EFI_D_ERROR, "Did not found on board storages\n", Status));
    return Status;
  }

  DiskNum = OemStorageInfo->ActualNumber;
  DEBUG((EFI_D_ERROR, "Found Disk Number:%d OemDbgStorageInfo:0x%x\n", DiskNum, OemStorageInfo));

  for(Index=0; Index<DiskNum; Index++) {
    ZeroMem(&OemHDDInfo, sizeof(PACKED_OBHDD_INFO));
    DriveInfo = OemStorageInfo->DriveInfo[Index];
    //<lub_1231 s>
    OemHDDInfo.DATA1 = Index; //DATA1= CPU sockect number  
    OemHDDInfo.DATA2 = 0;   //reserved
    OemHDDInfo.DATA3 = 0;   //reserved
    OemHDDInfo.HddGroupType = 0;  //reserved
    OemHDDInfo.HddStatus = 0;   //reserved need to update the HDD status.
    OemHDDInfo.HddRunStatus = 0;   //reserved
    //<lub_1231 e>
    OemHDDInfo.HddMode = DriveInfo->HddMode;
    OemHDDInfo.HddPreset = 1;
    OemHDDInfo.HddSizeUnit = Unit_GB;
    OemHDDInfo.HddSize = (UINT16)(DriveInfo->HddSize/1000);
    #if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18 == 1)
    //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
    OemHDDInfo.HDDControllerIndex = DriveInfo->HDDControllerIndex;
    OemHDDInfo.HDDPosition = DriveInfo->HddSlot;
    OemHDDInfo.HDDParentBus = DriveInfo->RootBus;
    OemHDDInfo.HDDParentDevice = DriveInfo->RootDevice;
    OemHDDInfo.HDDParentFunction = DriveInfo->RootFunction;
    //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  ->
    #endif
    DEBUG((EFI_D_ERROR, "%a Slot:%d Size:%dGB ", (OemHDDInfo.HddMode == TypeNvme)?"NVME":"SATA", DriveInfo->HddSlot, OemHDDInfo.HddSize));

    CopyMem(OemHDDInfo.HddSN, DriveInfo->HddSN, OEM_IPMI_HDD_SN_LENGTH);
    DEBUG((EFI_D_ERROR, "SN:%a ", OemHDDInfo.HddSN));

    CopyMem(OemHDDInfo.HddFirmwareVersion, DriveInfo->HddFirmwareVersion, OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH);
    DEBUG((EFI_D_ERROR, "FW:%a ", OemHDDInfo.HddFirmwareVersion));

    CopyMem(OemHDDInfo.HddModelName, DriveInfo->HddModelName, OEM_IPMI_HDD_MODEL_NAME_LENGTH);
    DEBUG((EFI_D_ERROR, "Model:%a\n", OemHDDInfo.HddModelName));

    //Initialize OEM IPMI package header
    OemHDDInfo.SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypeHdd;
    OemHDDInfo.SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
    OemHDDInfo.DataValid = BIOSPackageValid;

    Status |= OemIpmiCmdSend(
      CMD_OEM_SET_SERVER_REPOSITORY_INFO,
      (VOID*) &OemHDDInfo,
      sizeof(PACKED_OBHDD_INFO),
      NULL,
      &ResponseDataSize
      );

  }

  DEBUG((EFI_D_ERROR, __FUNCTION__" End Status:%r\n", Status)); 
  return  Status;
}                                                        

//***************************************************************
