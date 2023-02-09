//**********************************************************************
// $Header: OEM IPMI CPU Info
//
// $Revision: 0.01
//
// $Date:  <Lizhq-20161217> 
//         Base On OEM IPMI SPEC 20161217, create OEM IPMI CPU Information module.
//**********************************************************************
#include <setup.h>
#include <OemIpmiNetFnDefinitions.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricTopologyServices.h>  //<Wangyia001-20170506 Send CPU info to BMC>
#include <Protocol/AmiCpuInfo.h>  //<Wangyia001-20170506 Send CPU info to BMC>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/PcdLib.h> //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation>
//<Kangmm-20201125-Send CPU SN to BMC +>
#include <Protocol/AmdNbioSmuServicesProtocol.h>
//<Kangmm-20201125-Send CPU SN to BMC ->
//<YinQiang001-20190910-Send CPU TDP to BMC +>
#include <Library/PciLib.h>
#define DIE_PER_SCKT 4
#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8
//<YinQiang001-20190910-Send CPU TDP to BMC ->
#define MAX_NEWSTRING_SIZE           0x200
#define MAXIMUM_CPU_BRAND_STRING_LENGTH 64//48 Sync with OEM IPMI SPEC, the string length is 64 bytes
//
// Extern Variables
//
static EFI_HOB_GUID_TYPE         *GuidHob = NULL;
extern EFI_BOOT_SERVICES *gBS;  //<Wangyia001-20170506 Send CPU info to BMC>

//<YinQiang001-20190910-Send CPU TDP to BMC +>
VOID
OemNbSmnRead (
  IN       UINT32              DieNum,
  IN       UINT32              Address,
  IN       UINT32          *Value
  )
{
    UINT32 read_value=0;
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    read_value=PciRead32(NB_SMN_DATA_3_BIOS);
    *Value=read_value;
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
}
//<YinQiang001-20190910-Send CPU TDP to BMC ->

EFI_STATUS
SmbiosGetStructure (
    IN      UINT16  Handle,
    IN      EFI_SMBIOS_TYPE         Type,
    OUT     EFI_SMBIOS_TABLE_HEADER **Record
    )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_PROTOCOL               *Smbios;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHeader;
  EFI_STATUS                        Status;

  if(Record == NULL)
    return EFI_INVALID_PARAMETER;

  Status = gBS->LocateProtocol (
        &gEfiSmbiosProtocolGuid,
        NULL,
        (VOID **) &Smbios
        );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = 0;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &Type, Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    SmbiosHeader = (EFI_SMBIOS_TABLE_HEADER *) *Record;
    if (SmbiosHeader->Handle == Handle) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


EFI_STATUS
SmbiosGetOptionalStringByIndex (
    IN      CHAR8                   *OptionalStrStart,
    IN      UINT8                   Index,
    OUT     CHAR16                  **String
    )
{
  UINTN          StrSize;

  if ((Index == 0) || (OptionalStrStart == NULL) || (String == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart); // size includes null terminator
    // SMBIOS strings are NULL terminated, and end of all strings is indicated with NULL
    // loop until at end of all SMBIOS strings (found NULL terminator at string index past string's NULL), and Index != 0
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    return EFI_INVALID_PARAMETER;
  } else {
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}

VOID
GetCpuExInfoFromSmbios(
    IN PACKED_CPU_EX_INFO *OemCpuExInfo,
    IN UINT8               CpuIndex)
{
  EFI_STATUS                            Status;
  UINT8                                 StrIndex;
  UINTN                                 SocketIndex;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  SMBIOS_TABLE_TYPE4                    *SmbiosType4Record;
  SMBIOS_TABLE_TYPE7                    *SmbiosType7Record;
  EFI_SMBIOS_TYPE                       SmbiosType;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord;
  EFI_SMBIOS_TABLE_HEADER               *SmbiosRecord2;
  CHAR16                                *NewString = NULL;
  //<Yaosr-20190313 Add CPU Info Send to BMC +>
  UINT8                                   ExtFamily;
  UINT8                                   BaseFamily;
  UINT8                                   ExtModel;
  UINT8                                   BaseModel;
  //<Yaosr-20190313 Add CPU Info Send to BMC ->
  //<YinQiang001-20190910-Send CPU TDP to BMC +>
  UINT32                                Value1 = 0;
  UINT32                                Value2 = 0;
  UINT32                                TDP = 0;
  //<YinQiang001-20190910-Send CPU TDP to BMC ->

//<Kangmm-20201125-Send CPU SN to BMC +>
  HYGON_PROCESSOR_SN_PROTOCOL           *mProcessorSNProtocol;
  UINT32                                CPUSN[7];//Mizl-20210326-Enlarge CPUSN for Memory Leak+>
//<Kangmm-20201125-Send CPU SN to BMC ->
  
  //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation +>
  CHAR16                                *SocketString = NULL;
  UINTN                                 StringLen;

  SocketString    =  AllocateZeroPool(MAX_NEWSTRING_SIZE);
  ASSERT_EFI_ERROR (SocketString != NULL);
  
  AsciiStrToUnicodeStr(PcdGetPtr(PcdAmdSmbiosSocketDesignationSocket0), SocketString);
  StringLen = StrLen(SocketString);
  SocketString[StringLen-1] = 0;
  //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation ->

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);

  NewString    =  AllocateZeroPool(MAX_NEWSTRING_SIZE);
  ASSERT_EFI_ERROR (NewString != NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  while (1) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    SmbiosType4Record = (SMBIOS_TABLE_TYPE4 *) SmbiosRecord;
    StrIndex = SmbiosType4Record->Socket;
    if (SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType4Record + SmbiosType4Record->Hdr.Length), StrIndex, &NewString) == EFI_SUCCESS) {
      //<Yaosr001-20181201 Optimize the method of iidentifying Smbios type4 SocketDesignation +>
      // string should be 'Px'
      /*if (StrnCmp(NewString, L"P", 1) != 0) continue;
      SocketIndex = StrDecimalToUintn(NewString + StrLen(L"P"));*/
      if (StrnCmp(NewString, SocketString, StringLen-1) != 0) continue;

      SocketIndex = StrDecimalToUintn(NewString + StringLen-1);
      //<Yaosr001-20181201 Optimize the method of identifying Smbios type4 SocketDesignation ->
      if(SocketIndex != (UINTN)CpuIndex) continue;

      //CpuId
      CopyMem(OemCpuExInfo[CpuIndex].CpuId, &SmbiosType4Record->ProcessorId, sizeof(PROCESSOR_ID_DATA));
      //CpuFre
      OemCpuExInfo[CpuIndex].CpuFre = SmbiosType4Record->CurrentSpeed;
      //<Kangmm-20191128 Fixed that CPU cores and threads display error on the BMC Web. +>
      //CoreCount
      if((SmbiosType4Record->CoreCount == 0xFF) && (SmbiosType4Record->CoreCount2 != 0)) {
        OemCpuExInfo[CpuIndex].CoreCount = SmbiosType4Record->EnabledCoreCount2;
        //<Yaosr-20190313 Add CPU Info Send to BMC +>
        OemCpuExInfo[CpuIndex].MaxCoreCount = (UINT8)SmbiosType4Record->CoreCount2;
        OemCpuExInfo[CpuIndex].MaxThreadCount = (UINT8)SmbiosType4Record->ThreadCount2;
        //<Yaosr-20190313 Add CPU Info Send to BMC ->
      } else {
        OemCpuExInfo[CpuIndex].CoreCount = (UINT16)SmbiosType4Record->EnabledCoreCount;
        //<Yaosr-20190313 Add CPU Info Send to BMC +>
        OemCpuExInfo[CpuIndex].MaxCoreCount = (UINT8)SmbiosType4Record->CoreCount;
        OemCpuExInfo[CpuIndex].MaxThreadCount = (UINT8)SmbiosType4Record->ThreadCount;
        //<Yaosr-20190313 Add CPU Info Send to BMC ->
      }
      if (PcdGet8(PcdAmdSmtMode))
      {
        OemCpuExInfo[CpuIndex].ThreadCount = (UINT16)(2 * (OemCpuExInfo[CpuIndex].CoreCount));
      } else {
        OemCpuExInfo[CpuIndex].ThreadCount = OemCpuExInfo[CpuIndex].CoreCount;
      }
      //<Kangmm-20191128 Fixed that CPU cores and threads display error on the BMC Web. ->
      //L1CacheSize
      if (SmbiosGetStructure (SmbiosType4Record->L1CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
        SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
        if ((SmbiosType7Record->CacheConfiguration & 0x7) == 0) { // [2:0] is Cache Level; verify it matches
          // [15] indicates granularity; 0=1 KB, 1=64 KB
          // set cache size in bytes by setting the value to 1024*size
          OemCpuExInfo[CpuIndex].L1CacheSize = SmbiosType7Record->InstalledSize & 0x7FFF;
          // if granularity is 64 KB, then multiply by 64
          if (SmbiosType7Record->InstalledSize & 0x8000) {
            OemCpuExInfo[CpuIndex].L1CacheSize  *= 64;
          }
        }
      }
      //L2CacheSize
      if (SmbiosGetStructure (SmbiosType4Record->L2CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
        SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
        if ((SmbiosType7Record->CacheConfiguration & 0x7) == 1) { // [2:0] is Cache Level; verify it matches
          // [15] indicates granularity; 0=1 KB, 1=64 KB
          // set cache size in bytes by setting the value to 1024*size
          OemCpuExInfo[CpuIndex].L2CacheSize  = SmbiosType7Record->InstalledSize & 0x7FFF;
          // if granularity is 64 KB, then multiply by 64
          if (SmbiosType7Record->InstalledSize & 0x8000) {
            OemCpuExInfo[CpuIndex].L2CacheSize  *= 64;
          }
        }
      }
      //L3CacheSize
      if (SmbiosGetStructure (SmbiosType4Record->L3CacheHandle, EFI_SMBIOS_TYPE_CACHE_INFORMATION, &SmbiosRecord2) == EFI_SUCCESS) {
        SmbiosType7Record = (SMBIOS_TABLE_TYPE7 *) SmbiosRecord2;
        if ((SmbiosType7Record->CacheConfiguration & 0x7) == 2) { // [2:0] is Cache Level; verify it matches
          // [15] indicates granularity; 0=1 KB, 1=64 KB
          // set cache size in bytes by setting the value to 1024*size
          OemCpuExInfo[CpuIndex].L3CacheSize = SmbiosType7Record->InstalledSize & 0x7FFF;
          // if granularity is 64 KB, then multiply by 64
          if (SmbiosType7Record->InstalledSize & 0x8000) {
            OemCpuExInfo[CpuIndex].L3CacheSize *= 64;
          }
        }
      }
      //<Yaosr-20190313 Add CPU Info Send to BMC +>
      ExtFamily = SmbiosType4Record->ProcessorId.Signature.ProcessorXFamily;
      BaseFamily = SmbiosType4Record->ProcessorId.Signature.ProcessorFamily;
      ExtModel = SmbiosType4Record->ProcessorId.Signature.ProcessorXModel;
      BaseModel = SmbiosType4Record->ProcessorId.Signature.ProcessorModel;

      OemCpuExInfo[CpuIndex].CpuIdModel = (ExtModel << 4) + BaseModel;
      OemCpuExInfo[CpuIndex].CpuIdFamily = BaseFamily + ExtFamily;
      OemCpuExInfo[CpuIndex].CpuIdStepping = SmbiosType4Record->ProcessorId.Signature.ProcessorSteppingId;
      //CpuSysClock
      OemCpuExInfo[CpuIndex].CpuSysClock = SmbiosType4Record->ExternalClock;
      //CpuPCIeLinkRate
      OemCpuExInfo[CpuIndex].CpuMaxPcieLinkRate = CPU_MaxPCIe_Link_Rate;
      //<Yaosr-20190313 Add CPU Info Send to BMC ->
      
      //<YinQiang001-20190910-Send CPU TDP to BMC +>
      OemNbSmnRead(0, 0x5d374, &Value1);
      OemNbSmnRead(0, 0x5d378, &Value2);
      TDP = ((Value2&0x00000001)<<7)|((Value1&0xFE000000)>>25);
      if (TDP > 0)
        OemCpuExInfo[CpuIndex].CpuTDP = TDP;
      else
        OemCpuExInfo[CpuIndex].CpuTDP = 0;
      DEBUG((-1, "0x5d378 =0x%x,0x5d374 =0x%x,TDP = %d\n", Value2,Value1,TDP));
      //<YinQiang001-20190910-Send CPU TDP to BMC ->

      //<Kangmm-20201125-Send CPU SN to BMC +>
      Status = gBS->LocateProtocol (&gHygonProcessorServicesProtocolGuid, NULL, (VOID **) &mProcessorSNProtocol);
      ASSERT_EFI_ERROR (Status);
      //Mizl-20210326-Enlarge CPUSN for Memory Leak+>
      mProcessorSNProtocol->GetProcessorSN(CpuIndex,&CPUSN[0]);
      OemCpuExInfo[CpuIndex].CpuSN = CPUSN[0];
      //Mizl-20210326-Enlarge CPUSN for Memory Leak->
      //<Kangmm-20201125-Send CPU SN to BMC ->
      
      //<Fancf-20200722 Add CPU MaxSpeed Info Send to BMC +>
#if defined(CPU_MAXSPEED_TO_BMC) && (CPU_MAXSPEED_TO_BMC == 1)
      OemCpuExInfo[CpuIndex].MaxSpeed = SmbiosType4Record->MaxSpeed; 
#endif
      //<Fancf-20200722 Add CPU MaxSpeed Info Send to BMC ->
      
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuId: 0x%llx\n", CpuIndex, *((UINT64 *)OemCpuExInfo[CpuIndex].CpuId)));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuFre: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuFre));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CoreCount: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CoreCount));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].ThreadCount: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].ThreadCount));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].L1CacheSize: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].L1CacheSize));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].L2CacheSize: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].L2CacheSize));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].L3CacheSize: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].L3CacheSize));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuSysClock: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuSysClock));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].MaxCoreCount: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].MaxCoreCount));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].MaxThreadCount: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].MaxThreadCount));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuIdModel: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuIdModel));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuIdFamily: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuIdFamily));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuIdStepping: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuIdStepping));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuMaxPcieLinkRate: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuMaxPcieLinkRate));
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuTDP: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuTDP)); //<YinQiang001-20190910-Send CPU TDP to BMC>
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].CpuSN: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].CpuSN)); //<Kangmm-20201125-Send CPU SN to BMC>
      //<Fancf-20200722 Add CPU MaxSpeed Info Send to BMC +>
#if defined(CPU_MAXSPEED_TO_BMC) && (CPU_MAXSPEED_TO_BMC == 1)
      DEBUG((EFI_D_ERROR, "OemCpuExInfo[%d].MaxSpeed: 0x%x\n", CpuIndex, OemCpuExInfo[CpuIndex].MaxSpeed));
#endif
      //<Fancf-20200722 Add CPU MaxSpeed Info Send to BMC ->
      
    }
  }

  if(NewString)gBS->FreePool (NewString);
}

EFI_STATUS
OemIpmiSetCpuInfo(VOID)
{
  PACKED_CPU_INFO                        OemCpuInfo[NSOCKETS];
  PACKED_CPU_EX_INFO                     OemCpuExInfo[NSOCKETS];
  EFI_STATUS                             Status = EFI_SUCCESS;   
  CHAR8                                  *DummyBrandString = "HYGON CPU";
  UINT8                                  CpuIndex;
  UINTN                                  NumberOfInstalledProcessors;
  UINTN                                  NumberOfDie;
  UINTN                                  IgnoredRootBridges;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology = NULL;
  AMI_CPU_INFO                           *GetCpuInfo = NULL;
  AMI_CPU_INFO_PROTOCOL                  *Cpu = NULL;
  UINT8   			                     ResponseDataSize=0;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Entered \n"));

  //Variable init
  ZeroMem((VOID*)OemCpuInfo, NSOCKETS*sizeof(PACKED_CPU_INFO));
  ZeroMem((VOID*)OemCpuExInfo, NSOCKETS*sizeof(PACKED_CPU_EX_INFO));

  //<Wangyia001-20170506 Send CPU info to BMC +>
  // Locate FabricTopologyServicesProtocol
  Status = gBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, &IgnoredRootBridges);

  // Find AMI_CPU_INFO_PROTOCOL protocol 
  Status = gBS->LocateProtocol (&gAmiCpuInfoProtocolGuid, NULL, &Cpu);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) return Status;
  DEBUG((EFI_D_ERROR, "NumberOfInstalledProcessors: %d\n", NumberOfInstalledProcessors));

  //Get CPU occupied status
  OemCpuInfo[0].CpuPresent  = 1;
  if (NumberOfInstalledProcessors == 2) {
    OemCpuInfo[1].CpuPresent  = 1;
  } else {
    OemCpuInfo[1].CpuPresent  = 0;
  }

  Cpu->GetCpuInfo(Cpu, 0, &GetCpuInfo);
  for (CpuIndex = 0; CpuIndex < NSOCKETS; CpuIndex++, Cpu->GetCpuInfo(Cpu, CpuIndex * (GetCpuInfo->NumCores), &GetCpuInfo)) {
    OemCpuInfo[CpuIndex].DATA1 = CpuIndex; //DATA1= CPU sockect number

    //Initialize CPU brand string
    if (GetCpuInfo->BrandString[0] != 0) {
      AsciiStrCpyS(OemCpuInfo[CpuIndex].CpuBrandName, MAXIMUM_CPU_BRAND_STRING_LENGTH, GetCpuInfo->BrandString);
    } else {
      if (CpuIndex == 0) {
        AsciiStrCpyS(OemCpuInfo[CpuIndex].CpuBrandName, MAXIMUM_CPU_BRAND_STRING_LENGTH, DummyBrandString);
      } else {
        AsciiStrCpyS(OemCpuInfo[CpuIndex].CpuBrandName, MAXIMUM_CPU_BRAND_STRING_LENGTH, OemCpuInfo[0].CpuBrandName);
      }
    }
    DEBUG((EFI_D_ERROR, "CpuBrandString: %a\n", OemCpuInfo[CpuIndex].CpuBrandName));
  }
  //<Wangyia001-20170506 Send CPU info to BMC ->

  for (CpuIndex = 0; CpuIndex < NSOCKETS; CpuIndex++) {
    OemCpuInfo[CpuIndex].SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypeCpu;
    OemCpuInfo[CpuIndex].SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
    OemCpuInfo[CpuIndex].DataValid = BIOSPackageValid;
    
    Status |= OemIpmiCmdSend(
      CMD_OEM_SET_SERVER_REPOSITORY_INFO,
      (VOID*) &OemCpuInfo[CpuIndex],
      sizeof(PACKED_CPU_INFO),
      NULL,
      &ResponseDataSize
      );
  }
  
  
  //Send CPU extend info to BMC
  for (CpuIndex = 0; CpuIndex < NSOCKETS; CpuIndex++) {
    OemCpuExInfo[CpuIndex].DATA1 = CpuIndex; //DATA1= CPU sockect number
    OemCpuExInfo[CpuIndex].DATA2 = 0;   //reserved
    OemCpuExInfo[CpuIndex].DATA3 = 0;   //reserved
    
    GetCpuExInfoFromSmbios(OemCpuExInfo, CpuIndex);
  }
    
  for(CpuIndex = 0; CpuIndex < NSOCKETS; CpuIndex++) {
    //Initialize OEM IPMI package header
    OemCpuExInfo[CpuIndex].SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypeCpuEx;
    OemCpuExInfo[CpuIndex].SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
    OemCpuExInfo[CpuIndex].DataValid = BIOSPackageValid;

    Status |= OemIpmiCmdSend(
          CMD_OEM_SET_SERVER_REPOSITORY_INFO,
          (VOID*) &OemCpuExInfo[CpuIndex],
          sizeof(PACKED_CPU_EX_INFO),
          NULL,
          &ResponseDataSize
          );
  }

  
  return Status;
}

EFI_STATUS
OemCpuInfoEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS  	    Status = EFI_SUCCESS;
  
  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));  
  Status = OemIpmiSetCpuInfo();
  DEBUG((EFI_D_ERROR, "Send CPU info status: %r\n",Status)); 
  
  return  Status;
}                                                        

//***************************************************************
