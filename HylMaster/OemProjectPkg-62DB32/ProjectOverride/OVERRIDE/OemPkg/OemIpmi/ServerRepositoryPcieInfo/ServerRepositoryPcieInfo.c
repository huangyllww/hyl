//**********************************************************************

// $Header: OEM IPMI
//
// $Revision: 0.01
//
// $Date:  <lvych-20161228> 
//
//**********************************************************************
#include <Efi.h>
#include <token.h>
#include <OemIpmiNetFnDefinitions.h>
#include <OemPCIeDev.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
//#include <IndustryStandard/Pci22.h>
#include <PciBus.h>
#include <AmiLib.h>					//<tanzhk-20170307-001 BMC Web can't show NIC info.>
//<Yaosr001-20200605 After sending Pcie asset info to BMC, send Pcie error log to BMC again. +>
#include <AmdRas.h>
#include <Include/AmdCpmRas.h>
#include <Protocol/AmiHygonElogProtocol.h>
#include <Library/PcdLib.h>
#include <OemSmbios.h>//dengyh 20211231 smbios type 9 
//<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.+  >
#if  defined(REPORT_NIC_FW_VERSION_INRO_TO_BMC) && (REPORT_NIC_FW_VERSION_INRO_TO_BMC==1)
#define     I350_EERD 0x14
#define     IXGBE_EERD 0x10014
#define     I350_FWVER_WORD_INDEX 5
#define     X550_FWVER_WORD_INDEX 0x18
#define     I350_ETID_WORDH_INDEX 0x43
#define     I350_ETID_WORDL_INDEX 0x42
#define     X550_ETID_WORDH_INDEX 0x2E
#define     X550_ETID_WORDL_INDEX 0x2D
#define     BAR0 0x10
#define     BAR1 0x14
#define     RD_START  BIT0
#define     RD_DONE   BIT1
#define   FWVER_MASK 0xF0FF
#define   INVALID_FWVER 0xFFFF
#endif
//<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.-  >

extern EFI_RUNTIME_SERVICES     *gRT;
extern EFI_GUID                 gEfiGlobalVariableGuid;
AMI_HYGON_ELOG_PROTOCOL         *mAmiHygonElogProtocol;
//<Yaosr001-20200605 After sending Pcie asset info to BMC, send Pcie error log to BMC again. ->

//<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.+  >
#if  defined(REPORT_NIC_FW_VERSION_INRO_TO_BMC) && (REPORT_NIC_FW_VERSION_INRO_TO_BMC==1)
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   GetFwVersion
//
// Description: Get eeprom fw version
//
// Input: 
//
// Output: 
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS GetFwVersion(
    EFI_PCI_IO_PROTOCOL               *PciIo,
    PACKED_NIC_INFO                   *OemNicInfo
)
{
  UINTN                           MMIOAddress;
  UINT16                          LVendorID, LDeviceID;
  UINT8                           BaseClassCode;
  UINT32                                                    Data32;
  UINT16                                                    Retry_count=0;
  UINT32                                                  TempEtid;
  
  if(PciIo == NULL || OemNicInfo == NULL)
    return EFI_INVALID_PARAMETER;

  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x0B,1,&BaseClassCode);
  if(BaseClassCode != 2)
    return EFI_UNSUPPORTED;

  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x00,1,&LVendorID);
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint16,0x02,1,&LDeviceID);
  DEBUG((EFI_D_ERROR, " *** GetFwVersion  LVendorID/LDeviceID    is  0x%x / 0x%x   ***\n", LVendorID, LDeviceID));
  
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,BAR0,1,&Data32);
  MMIOAddress = Data32;
  if((MMIOAddress & 0x7) == 4)  {//64-bit Bar //<lvych00320160329>
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint32,BAR1,1,&Data32);
    MMIOAddress |= ((UINT64)Data32<<32);
  }
  
  if ((MMIOAddress & 01) == 0) {
      MMIOAddress = MMIOAddress & (UINT64)-16;
      DEBUG((EFI_D_ERROR, " *** GetFwVersion  MMIOAddress = 0x%x   ***\n", MMIOAddress));
      if (LVendorID  == 0x8086){ //Intel network 
            switch (LDeviceID) {
             //Intel I350
             case 0x1521: 
                    //Get FW version
                        MmioWrite32(MMIOAddress + I350_EERD,(I350_FWVER_WORD_INDEX << 2)|RD_START);
                        do{
                                gBS->Stall(10000);//10ms
                                Data32 = MmioRead32(MMIOAddress + I350_EERD);
                                Retry_count++;
                        }while(((Data32 & RD_DONE) != RD_DONE)&&(Retry_count <5));
                    
                        if((Data32 & RD_DONE) == RD_DONE){
                                OemNicInfo->FwVersion = (UINT16)((Data32 >>16) & FWVER_MASK);
                        }else{
                                OemNicInfo->FwVersion = INVALID_FWVER;
                        }
                        //Get FW ETID
                        MmioWrite32(MMIOAddress + I350_EERD,(I350_ETID_WORDH_INDEX << 2)|RD_START);
                        do{
                                gBS->Stall(10000);//10ms
                                Data32 = MmioRead32(MMIOAddress + I350_EERD);
                                Retry_count++;
                        }while(((Data32 & RD_DONE) != RD_DONE)&&(Retry_count <5));
                    
                        if((Data32 & RD_DONE) == RD_DONE){
                                TempEtid = Data32 >>16;
                        }else{
                                TempEtid = INVALID_FWVER;
                        }
                        
                        MmioWrite32(MMIOAddress + I350_EERD,(I350_ETID_WORDL_INDEX << 2)|RD_START);
                        do{
                                gBS->Stall(10000);//10ms
                                Data32 = MmioRead32(MMIOAddress + I350_EERD);
                                Retry_count++;
                        }while(((Data32 & RD_DONE) != RD_DONE)&&(Retry_count <5));
                    
                        if((Data32 & RD_DONE) == RD_DONE){
                                TempEtid = (TempEtid <<16)|(Data32 >>16);
                        }else{
                                TempEtid = (TempEtid <<16)|INVALID_FWVER;
                        }
                        
                        OemNicInfo->FwEtid = TempEtid;
                        
                        break;
                        
             case 0x1563:  //X550
                    //Get FW version
                        MmioWrite32(MMIOAddress + IXGBE_EERD,(X550_FWVER_WORD_INDEX << 2)|RD_START);
                        do{
                                gBS->Stall(10000);//10ms
                                Data32 = MmioRead32(MMIOAddress + IXGBE_EERD);
                                Retry_count++;
                        }while(((Data32 & RD_DONE) !=RD_DONE)&&(Retry_count <5));
                    
                        if((Data32 & RD_DONE) == RD_DONE){
                                OemNicInfo->FwVersion = (UINT16)((Data32 >>16) & FWVER_MASK);
                        }else{
                                OemNicInfo->FwVersion = INVALID_FWVER;
                        }
                        //Get FW ETID
                        MmioWrite32(MMIOAddress + IXGBE_EERD,(X550_ETID_WORDH_INDEX << 2)|RD_START);
                        do{
                                gBS->Stall(10000);//10ms
                                Data32 = MmioRead32(MMIOAddress + IXGBE_EERD);
                                Retry_count++;
                        }while(((Data32 & RD_DONE) != RD_DONE)&&(Retry_count <5));
                    
                        if((Data32 & RD_DONE) == RD_DONE){
                                TempEtid = Data32 >>16;
                        }else{
                                TempEtid = INVALID_FWVER;
                        }
                        
                        MmioWrite32(MMIOAddress + IXGBE_EERD,(X550_ETID_WORDL_INDEX << 2)|RD_START);
                        do{
                                gBS->Stall(10000);//10ms
                                Data32 = MmioRead32(MMIOAddress + IXGBE_EERD);
                                Retry_count++;
                        }while(((Data32 & RD_DONE) != RD_DONE)&&(Retry_count <5));
                    
                        if((Data32 & RD_DONE) == RD_DONE){
                                TempEtid = (TempEtid <<16)|(Data32 >>16);
                        }else{
                                TempEtid = (TempEtid <<16)|INVALID_FWVER;
                        }
                        
                        OemNicInfo->FwEtid = TempEtid;
                        break;    
                        
             default:
                        OemNicInfo->FwVersion = INVALID_FWVER;
                        
                      TempEtid =INVALID_FWVER;
                      TempEtid = (TempEtid <<16)|INVALID_FWVER;
                      OemNicInfo->FwEtid =TempEtid;
                      
                      break;
            } //switch (LDeviceID) 
      } else{
              OemNicInfo->FwVersion = INVALID_FWVER;  // porting according to device.
              TempEtid =INVALID_FWVER;
              TempEtid = (TempEtid <<16)|INVALID_FWVER;
              OemNicInfo->FwEtid =TempEtid;
      }
  DEBUG((EFI_D_ERROR, " *** GetFwVersion  FwVersion = 0x%x   ***\n", OemNicInfo->FwVersion));
  DEBUG((EFI_D_ERROR, " *** GetFwVersion  FwEtID = 0x%x   ***\n", OemNicInfo->FwEtid));
  } 
  return EFI_SUCCESS;
}
#endif
//<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.-  >
//dengyh 20211231 smbios type 9 +
/**

    This is a worker function to extract the SMBIOS string from the
    specified SMBIOS structure data.

    @param StructureHandle - SMBIOS structure data pointer.
    @param StringNumber - The string number to get.
    @param StringData - The string is copied here.

    @return EFI_STATUS
    @retval EFI_SUCCESS or valid error code.

    @note
    This function is called by GetSmbiosDeviceString.
    Assumes the string is large enough (error checking must be made by caller).

**/

EFI_STATUS
GetSmbiosStringByNumber (
    IN  UINT8    *StructureHandle,
    IN  UINT8    StringNumber,
    OUT UINT8    **StringData )
{
    *StringData = StructureHandle +
                ((SMBIOS_STRUCTURE_HEADER*)StructureHandle)->Length;

    while (--StringNumber) {
        while(**StringData != 0) {
            (*StringData)++;
        }
        (*StringData)++;
    }

    return EFI_SUCCESS;
}
//dengyh 20211231 smbios type 9 -

//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message +>
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   OemIpmiSetRBPcieInfo
//
// Description: 
//
// Input: 
//
// Output: 
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
OemIpmiSetRBPcieInfo(VOID)
{
  EFI_STATUS                      Status;
  OEM_PCIE_DEV_DATA_PROTOCOL      *OemPcieDevProtocol;
  PCI_DEV_SETUP_FORM_DATA         *PcieDevData;
//<zhangyan-20200910 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.+  >
#if  defined(REPORT_NIC_FW_VERSION_INRO_TO_BMC) && (REPORT_NIC_FW_VERSION_INRO_TO_BMC==1)
  PCI_DEV_SETUP_DATA              *Devitem;
  PCI_DEV_INFO                    *pdev;
  EFI_PCI_IO_PROTOCOL             *PciIo;
#endif
//<zhangyan-20200910 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.-  >
  PACKED_PCIE_INFO                OemPcieInfo;
  PACKED_NIC_INFO                 OemNicInfo;
  UINT8                           NicCount,PcieCount;
  UINT8                           NicCategory;
  UINT8                           DevIdx,PortIdx;
  PCI_DEV_IDX_DATA                *IdxDev;
  PCIE_DEV_INFO                   PcieDevInfo;
  PCIE_NIC_INFO                   *PcieNicInfo;
  UINT8   			  			  ResponseDataSize=0;
  //dengyh 20211231 smbios type 9+
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_SMBIOS_TYPE             SmbiosType;
  SMBIOS_TABLE_TYPE_9         *SmbiosType9Record;
  EFI_SMBIOS_TABLE_HEADER     *SmbiosRecord;
  CHAR8                       *SlotDesignationAscii = NULL;
  UINT8                       i;
  //dengyh 20211231 smbios type 9-
  DEBUG((EFI_D_ERROR, " *** OemIpmiSetNBPcieInfo ***\n")); 
  
  Status = gBS->LocateProtocol (
          &gOemPcieDevProtocolGuid,
          NULL,
          &OemPcieDevProtocol
          );
  if ( EFI_ERROR (Status) ) 
  {
      return Status;
  }
  PcieDevData = NULL;
  Status = OemPcieDevProtocol->GetPcieDevList(&PcieDevData);
  if ( EFI_ERROR (Status) ) 
  {
      return Status;
  }
  //dengyh 20211231 smbios type 9 +
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  DEBUG((EFI_D_ERROR, "%a gEfiSmbiosProtocolGuid Status: %r\n", __FUNCTION__, Status));
  if(EFI_ERROR(Status)) return Status;
  //dengyh 20211231 smbios type 9 -
  NicCount = 0;
  PcieCount = 0;
  //dengyh 20211231 smbios type 9 +
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
  //dengyh 20211231 smbios type 9 -
  for(DevIdx=0;DevIdx<PcieDevData->PciDevCount;DevIdx++){
      IdxDev = PcieDevData->PciDevIdxData[DevIdx];
    for(PortIdx=0;PortIdx<IdxDev->PortCount;PortIdx++){
//<zhangyan-20200910 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.+  >
#if  defined(REPORT_NIC_FW_VERSION_INRO_TO_BMC) && (REPORT_NIC_FW_VERSION_INRO_TO_BMC==1)
      Devitem = &IdxDev->PortData[PortIdx];
      pdev = (PCI_DEV_INFO *)Devitem->PciDevInfo;
      PciIo = &pdev->PciIo;
#endif
//<zhangyan-20200910 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.-  >
      ZeroMem(&OemPcieInfo, sizeof(PACKED_PCIE_INFO)); //<Lizhq035-20170103 Variable init>
      ZeroMem(&PcieDevInfo, sizeof(PCIE_DEV_INFO));
      Status = OemPcieDevProtocol->GetPcieDevInfo(PcieDevData,IdxDev->PciDevIdx,PortIdx,&PcieDevInfo);
      OemPcieInfo.DATA1=PcieCount;
      OemPcieInfo.PcieDevicePresent = PcieDevInfo.PcieDevicePresent;
      OemPcieInfo.CpuNum = PcieDevInfo.CpuNum;
      OemPcieInfo.PcieSegmentNum = PcieDevInfo.SegNum;
      OemPcieInfo.ParentPcieBusNum = PcieDevInfo.RbBus;
      OemPcieInfo.ParentPcieDevNum = PcieDevInfo.RbDev;
      OemPcieInfo.ParentPcieFunNum = PcieDevInfo.RbFunc;
      OemPcieInfo.PcieBusNum = PcieDevInfo.BusNum;
      OemPcieInfo.PcieDevNum = PcieDevInfo.DevNum;
      OemPcieInfo.PcieFunNum = PcieDevInfo.FuncNum;
      OemPcieInfo.PcieVenderId = PcieDevInfo.VenderId;
      OemPcieInfo.PcieDeviceId = PcieDevInfo.DeviceId;
      OemPcieInfo.PcieBaseClass = PcieDevInfo.BaseClassCode;
      OemPcieInfo.PcieSubClass =  PcieDevInfo.SubClassCode;
      OemPcieInfo.SubVenderId = PcieDevInfo.SubVenderId;
      OemPcieInfo.SubSystemId = PcieDevInfo.SubSystemId;
      OemPcieInfo.PcieProgrammingInterface = 0;
      OemPcieInfo.LinkWidth =  PcieDevInfo.LinkWidth;
      OemPcieInfo.LinkSpeed =  PcieDevInfo.LinkSpeed;
      AsciiStrCpy(OemPcieInfo.PcieBrandName, PcieDevInfo.BrandName);
      //dengyh 20211231 smbios type 9 +
      for (i = 0; ; ++i) {
              Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
              if (EFI_ERROR(Status)) {
                break;
              }
              SmbiosType9Record = (SMBIOS_TABLE_TYPE_9 *) SmbiosRecord;
              if (((UINT8)SmbiosType9Record->SegmentGroupNum == PcieDevInfo.SegNum)
                      && (SmbiosType9Record->BusNum == PcieDevInfo.BusNum)
                      && ((UINT8)(SmbiosType9Record->DevFuncNum >> 3) == PcieDevInfo.DevNum)
                      && ((SmbiosType9Record->DevFuncNum & 0x7) == PcieDevInfo.FuncNum))
              {
                  GetSmbiosStringByNumber((UINT8 *)SmbiosRecord, SmbiosType9Record->SlotDesignation, (UINT8 **)&SlotDesignationAscii);
                  AsciiStrnCpy(OemPcieInfo.PcieLocation, SlotDesignationAscii, 16);
              }
            }
      //dengyh 20211231 smbios type 9 -
      
      //*************************************************
      //***************Send PCIE Info to BMC*************
      //*************************************************
      if(PcieDevInfo.BoardType != 1){
        //Initialize OEM IPMI package header
        OemPcieInfo.SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypePcie;
        OemPcieInfo.SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
        OemPcieInfo.DataValid = BIOSPackageValid;
        
        Status = OemIpmiCmdSend(
          CMD_OEM_SET_SERVER_REPOSITORY_INFO,
          (VOID*) &OemPcieInfo,
          sizeof(PACKED_PCIE_INFO),
	      NULL,
	      &ResponseDataSize
          );
        
        DEBUG((EFI_D_ERROR, " *** Status of Set PCIE Device No %d Info is  %x    ***\n",PcieCount,Status));
          PcieCount++;
      }
  
      //*************************************************
      //**********Send network pcie device Info**********
      //*************************************************
      
      if (PcieDevInfo.BaseClassCode == PCI_CL_NETWORK){ //<Machong-20171011-Porting for Compiling Issue>
        
        ZeroMem(&OemNicInfo, sizeof(PACKED_NIC_INFO)); //<Lizhq035-20170103 Variable init>

	    //<xuran-20171130 +> BMC Web can't show Intel OPA Card GUID
      //<xuran-20171130 ->
        PcieNicInfo = (PCIE_NIC_INFO *)(PcieDevInfo.DevInfo);
        //<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.+  >        
        #if  defined(REPORT_NIC_FW_VERSION_INRO_TO_BMC) && (REPORT_NIC_FW_VERSION_INRO_TO_BMC==1)        
                GetFwVersion(PciIo,&OemNicInfo);
        #endif 
        //<Tanzhk001-20191109 Patch for mantis bug-0034029 that there is  different fw version from version gotten by ethtool.- >        

        if(PcieDevInfo.BoardType == 1){
          NicCategory = 1; //LOM
        }else{
          NicCategory = 2; //Pcie Card
        }
        OemNicInfo.NicNum = NicCount;		//<tanzhk-20170307-001 use as index for NIC.>
        OemNicInfo.NicPresent = 1;
        OemNicInfo.CpuNum = PcieDevInfo.CpuNum;
        OemNicInfo.NicPort = PcieDevInfo.PortIndex;
        OemNicInfo.NicCategory = NicCategory;
        OemNicInfo.NicSegmentNum = 0;
        OemNicInfo.NicParentBusNum = PcieDevInfo.RbBus;
        OemNicInfo.NicParentDevNum = PcieDevInfo.RbDev;
        OemNicInfo.NicParentFunNum = PcieDevInfo.RbFunc;
        OemNicInfo.NicBusNum = PcieDevInfo.BusNum;
        OemNicInfo.NicDevNum = PcieDevInfo.DevNum;
        OemNicInfo.NicFunNum = PcieDevInfo.FuncNum;
        OemNicInfo.NicVenderid = PcieDevInfo.VenderId;
        OemNicInfo.NicDeviceId = PcieDevInfo.DeviceId;
        OemNicInfo.NicBaseClass = PcieDevInfo.BaseClassCode;
        OemNicInfo.NicSubClass = PcieDevInfo.SubClassCode;
        OemNicInfo.NicProgrammingInterface = 0;
        OemNicInfo.NicSubVenderId = PcieDevInfo.SubVenderId;
        OemNicInfo.NicSubSystemId = PcieDevInfo.SubSystemId;
        OemNicInfo.NicLinkSpeed = 0;
        OemNicInfo.NicMacGuidSelect = PcieNicInfo->NicMacGuidSelect;
        CopyMem(OemNicInfo.NicMac, PcieNicInfo->NicMac, 6); //<Lizhq036-20170105 Compile error if DEBUG_MODE is disabled>
        CopyMem(OemNicInfo.NicGuid,PcieNicInfo->NicGuid, 8);  //<xuran-20171130 >
        
        CopyMem(OemNicInfo.NicBrandName, PcieDevInfo.BrandName,64);
        OemNicInfo.NicLinkSpeed = PcieNicInfo->Speed;
        AsciiStrnCpy(OemNicInfo.NicLocation, SlotDesignationAscii, 16);//dengyh 20211231 smbios type 9 
        //Initialize OEM IPMI package header
        OemNicInfo.SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypeNic;
        OemNicInfo.SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
        OemNicInfo.DataValid = BIOSPackageValid;
        Status = OemIpmiCmdSend(
          CMD_OEM_SET_SERVER_REPOSITORY_INFO,
          (VOID*) &OemNicInfo,
          sizeof(PACKED_NIC_INFO),
	      NULL,
	      &ResponseDataSize
          );
        
        DEBUG((EFI_D_ERROR, " *** Status of Set Nic Device No %d Info is  %x     ***\n\n",NicCount,Status));
        NicCount++;
      }
    }
  }
  DEBUG((EFI_D_ERROR, " *** OEM Set RB Pcie Info End! ***\n"));
  return Status;
}
//<huangjin001 20191206 Add lan Mac address in SETUP PCIE device list help message ->

//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   ServerRepositoryPcieInfoEntryPoint
//
// Description: Entry point for ServerRepositoryPcieInfo
//
// Input: IN EFI_HANDLE ImageHandle - Handle for the image of this driver
//        IN EFI_SYSTEM_TABLE *SystemTable - Pointer to the EFI System Table
//
// Output:  EFI_SUCCESS - Protocol Callback is successfully registered
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
ServerRepositoryPcieInfoEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
	EFI_STATUS  			Status = EFI_SUCCESS;
    //<Yaosr001-20200605 After sending Pcie asset info to BMC, send Pcie error log to BMC again. +>
    UINT16                         Index, PcieErrorCnt;
    UINTN                          Size;
    CHAR16                         PcieErrorStr[17];
    EFI_GUID                       gPcieErrorGuid = {0xc9122629, 0x7e71, 0x4c09, {0xbb, 0xf1, 0x18, 0x85, 0x75, 0x6b, 0xaa, 0xdf}};
    GENERIC_PCIE_AER_ERR_ENTRY     *GenPcieAerErrEntry;
    //<Yaosr001-20200605 After sending Pcie asset info to BMC, send Pcie error log to BMC again. ->

	//InitAmiLib(ImageHandle, SystemTable);
	
	Status = OemIpmiSetRBPcieInfo();
	if (EFI_ERROR (Status)) {
		return Status;
	}

	//<Yaosr001-20200605 After sending Pcie asset info to BMC, send Pcie error log to BMC again. +>
	PcieErrorCnt = (UINT16)PcdGet16(PcdPciErrorCount);
	DEBUG ((EFI_D_ERROR, "Get PcieErrLog count: %d\n", PcieErrorCnt));
	Size = sizeof(GENERIC_PCIE_AER_ERR_ENTRY);
	for(Index = 0; Index < PcieErrorCnt; Index++) {
	  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (GENERIC_PCIE_AER_ERR_ENTRY), &GenPcieAerErrEntry);
	  if (EFI_ERROR (Status)) {
	    return Status;
	  }

	  Swprintf(PcieErrorStr, L"PcieErrLog%04X", Index);
	  Status = gRT->GetVariable(PcieErrorStr, &gPcieErrorGuid, NULL, &Size, GenPcieAerErrEntry);
	  DEBUG ((EFI_D_ERROR, "Get PcieError variable status: %r\n", Status));
	  if(!EFI_ERROR (Status)){
	    Status = gBS->LocateProtocol (
	                       &gAmiHygonElogProtocolGuid,
	                       NULL,
	                       &mAmiHygonElogProtocol
	                       ); 
	    DEBUG ((EFI_D_ERROR, "Locate AMI_AMD_ELOG_PROTOCOL status: %r\n", Status));
	    if(!EFI_ERROR (Status)){
	      mAmiHygonElogProtocol->SendElogEventPcie ((UINT8*)GenPcieAerErrEntry);
	    }
	  }
	  gBS->FreePool (GenPcieAerErrEntry);
	}
	//<Yaosr001-20200605 After sending Pcie asset info to BMC, send Pcie error log to BMC again. ->
	

	return  Status;
}

//***************************************************************
