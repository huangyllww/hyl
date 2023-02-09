//**********************************************************************

// $Header:Oem OEM IPMI
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
//Define Class Code
#define OemNvme_PCI_CLASS_MASS_STORAGE        0x01

#define OemNvme_MSG_NVME_DP     23

#include <Library/PciLib.h>
#include "PCIeSlotDes.c"
#include "AutoDxioConfig.h"
#include "AutoPCIeSlotDescriptionList.h"
#include <Library/PciLib.h>
#include "IndustryStandard/PciCodeId.h"
#include <Library/BaseGetRbBusLib.h>

BOARD_SLOT_DESCRIPTION   *BoardSlotDescListServer[] = {AUTO_PCIE_SLOT_DESC_LIST NULL};
CHAR8         *BoardSlotDescNameListServer[] = {AUTO_PCIE_SLOT_DESC_LIST_NAMES NULL};
#define        MaxDieNumber     8
UINT8           RootPortBusBase[MaxDieNumber]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
static    SLOT_DESCRIPTION    *pSlotDescription=NULL;
extern EFI_RUNTIME_SERVICES   *gRT;
#include "PCIEXPRESS.h"
//Mizl-20210228-Add Filter For 88080/88048 LSI Device+>
typedef struct 
{
  UINT16      VenID;
  UINT16      DevID;
  UINT16      SubSystemID;
  UINT16      SubVendorID;
  UINT8       ClassCode;
  UINT8       SubClassCode;
}FILTER_DEV_INFOR;

typedef struct{
  FILTER_DEV_INFOR  PBInfor;
  FILTER_DEV_INFOR  DevInfo;
}FILTER_DEVICE_ARRAY;

FILTER_DEVICE_ARRAY   aFilerDevice[]={
  {
    {0x1000,0xc010,0xFFFF,0x1000,0x06,0x04},
    {0x1000,0xb2,0xFFFF,0x1000,0x01,0x07}
  },
  {
    {0x1000,0xc010,0xFFFF,0x1000,0x06,0x04},
    {0x1000,0xc010,0xb2,0x1000,0x01,0x80}
  },
  {
    {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF,0xFF},
    {0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFF,0xFF}
  }
};


/**
    Search and return the Capability ID Address by FindCapNo.

    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Device Address for finding CapNo.
    @param FindCapNo - The Capability ID would be searched.
    @param CapPtr8 - Pointer to the Capability ID Address.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
GetLegCapStrucAddr0 (
    IN EFI_PCI_IO_PROTOCOL     *PciIo,
    IN UINT8                    FindCapNo,
    OUT UINT8                   *CapPtr8 )
{
    EFI_STATUS                  Status;
    UINT8                       Buffer8;

    Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          0x34,
                          1,
                          &Buffer8
                          );
   ASSERT_EFI_ERROR (Status);

    //Buffer8 = PciRead8(PciAddr64+0x34); //get 0x50

    *CapPtr8 = Buffer8;
    if ((*CapPtr8 == 0) || (*CapPtr8 == 0xFF)) return EFI_NOT_FOUND;

    //Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
    
    Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          *CapPtr8,
                          1,
                          &Buffer8
                          );
    ASSERT_EFI_ERROR (Status);

    while (Buffer8 != 0) {
        if (Buffer8 == FindCapNo) 
          return EFI_SUCCESS;

        //Buffer8 = PciRead8(PciAddr64 | (UINT8)(*CapPtr8+1));
        Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          *CapPtr8+1,
                          1,
                          &Buffer8
                          );
        ASSERT_EFI_ERROR (Status);

        *CapPtr8 = Buffer8;
        if (*CapPtr8 == 0) break;
        //Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
        Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          *CapPtr8,
                          1,
                          &Buffer8
                          );
        ASSERT_EFI_ERROR (Status);

    }
    return EFI_NOT_FOUND;
}

EFI_STATUS
FilterDeviceList(
  IN  PCIE_DEV_INFO         PcieDevInfo,
  IN  PCI_DEV_SETUP_DATA   *Devitem
)
{
  UINT8                   idx;
  PCI_DEV_INFO            *PbPtr = ((PCI_DEV_INFO *)(Devitem->PciDevInfo))->ParentBrg;
  EFI_PCI_IO_PROTOCOL     *PciIo;
  EFI_STATUS              Status;
  UINT16                  SubSystemId=0xFFFF,SubVenderId=0xFFFF;
  //UINT32                  SlotAddress;
  UINT8                   CapIdOffset;
  UINT16                  PCIeCap;

  DEBUG((EFI_D_ERROR, " *** FilterDeviceList ***\n")); 
  DEBUG((EFI_D_ERROR, "\r\n\
            PcieDevInfo:\r\n\
                      VenderId:0x%04x\r\n\
                      DeviceId:0x%04x\r\n\
                      SubSystemId:0x%02x\r\n\
                      SubVenderId:0x%02x\r\n\
                      BaseClassCode:0x%x\r\n\
                      SubClassCode:0x%x\r\n\
            PbPtr:\r\n\
                      VenderId:0x%04x\r\n\
                      DeviceId:0x%04x\r\n\
                      BaseClassCode:0x%x\r\n\
                      SubClassCode:0x%x\r\n",\
                      PcieDevInfo.VenderId,PcieDevInfo.DeviceId,\
                      PcieDevInfo.SubSystemId,PcieDevInfo.SubVenderId,\
                      PcieDevInfo.BaseClassCode,PcieDevInfo.SubClassCode,\
                      PbPtr->DevVenId.VenId,PbPtr->DevVenId.DevId,\
                      PbPtr->Class.BaseClassCode,PbPtr->Class.SubClassCode)); 
                      
  for(idx=0;aFilerDevice[idx].PBInfor.VenID != 0xFFFF;idx++)
  {
    if( (PcieDevInfo.VenderId != aFilerDevice[idx].DevInfo.VenID) ||\
        (PcieDevInfo.DeviceId != aFilerDevice[idx].DevInfo.DevID) ||\
        ((PcieDevInfo.SubSystemId != aFilerDevice[idx].DevInfo.SubSystemID) && (aFilerDevice[idx].DevInfo.SubSystemID != 0xFFFF))||\
        (PcieDevInfo.SubVenderId != aFilerDevice[idx].DevInfo.SubVendorID) ||\
        (PcieDevInfo.BaseClassCode != aFilerDevice[idx].DevInfo.ClassCode) ||\
        (PcieDevInfo.SubClassCode != aFilerDevice[idx].DevInfo.SubClassCode) ||\
        (PbPtr->DevVenId.VenId != aFilerDevice[idx].PBInfor.VenID) ||\
        (PbPtr->DevVenId.DevId != aFilerDevice[idx].PBInfor.DevID) ||\
        (PbPtr->Class.BaseClassCode != aFilerDevice[idx].PBInfor.ClassCode) ||\
        (PbPtr->Class.SubClassCode != aFilerDevice[idx].PBInfor.SubClassCode))
    
    continue;

    PciIo = &PbPtr->PciIo;
    
    #if 0
    Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          0xa8,
                          2,
                          &SubVenderId
                          );
    ASSERT_EFI_ERROR (Status);
    DEBUG((EFI_D_ERROR, " SubVendorID:0x%x\n",SubVenderId)); 
    if(SubVenderId != aFilerDevice[idx].PBInfor.SubVendorID)
      continue;

    
    Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          0x2e,
                          2,
                          &SubSystemId
                          );
   ASSERT_EFI_ERROR (Status);
   DEBUG((EFI_D_ERROR,"SubSystemID:%x\n",SubSystemId)); 

   if(SubSystemId != aFilerDevice[idx].PBInfor.SubSystemID)
    continue;
   #endif
   //SlotAddress = PCI_LIB_ADDRESS(PbPtr->Address.Addr.Bus,PbPtr->Address.Addr.Device,PbPtr->Address.Addr.Function,0x00);
   GetLegCapStrucAddr0(PciIo,0x10,&CapIdOffset);
   if((CapIdOffset ==0) || (CapIdOffset ==0xFF))
    continue;
   DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]CapIdOffset:%x\n\n",__FUNCTION__,__LINE__,CapIdOffset));

   Status = PciIo->Pci.Read(
                          PciIo,
                          EfiPciIoWidthUint8,
                          CapIdOffset+PCIE_CAPABILITY_OFFSET,
                          2,
                          &PCIeCap
                          );
   ASSERT_EFI_ERROR (Status);

   if(PCIeCap & BIT08)
    continue;
    
   DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]PCIeCap:%x\n\n",__FUNCTION__,__LINE__,PCIeCap));

   Devitem->ShowItem=FALSE;//mizl-20210228-Hide Device Shown At Setup
   return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}
//Mizl-20210228-Add Filter For 88080/88048 LSI Device->
/**
    @param PeiServices - Pointer to the PEI services table.
    @param PciCfg2 - Pointer to the PciCfg2 PPI.
    @param PciAddr64 - Device Address for finding CapNo.
    @param FindCapNo - The Capability ID would be searched.
    @param CapPtr8 - Pointer to the Capability ID Address.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
GetLegCapStrucAddr (
    IN UINT64                   PciAddr64,
    IN UINT8                    FindCapNo,
    OUT UINT8                   *CapPtr8 )
{
    UINT8                       Buffer8;

    Buffer8 = PciRead8(PciAddr64+0x34); //get 0x50

    *CapPtr8 = Buffer8;
    if ((*CapPtr8 == 0) || (*CapPtr8 == 0xFF)) return EFI_NOT_FOUND;

    Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
    
    while (Buffer8 != 0) {
        if (Buffer8 == FindCapNo) 
          return EFI_SUCCESS;

        Buffer8 = PciRead8(PciAddr64 | (UINT8)(*CapPtr8+1));
        *CapPtr8 = Buffer8;
        if (*CapPtr8 == 0) break;
        Buffer8 = PciRead8(PciAddr64 | *CapPtr8);
    }
    return EFI_NOT_FOUND;
}

/**
 * Read Root Max Link Width 
 * 
 * @retval EFI_STATUS
 * 
 * @note None
 * */

EFI_STATUS
GetMaxLinkWidth(
  IN    UINT8   RootBus,
  IN    UINT8   RootDevice,
  IN    UINT8   RootFunction,
  OUT   UINT8   *MaxLinkWidth
)
{
  EFI_STATUS      Status;
  UINT8           CapPtr8;
  UINT32          LinkCap;

  Status = GetLegCapStrucAddr(PCI_LIB_ADDRESS(RootBus,RootDevice,RootFunction,0x00),0x10,&CapPtr8);
  if(EFI_ERROR(Status))
    return Status;
  
  LinkCap = PciRead32(PCI_LIB_ADDRESS(RootBus,RootDevice,RootFunction,CapPtr8+0x0C));

  *MaxLinkWidth = (LinkCap >> 4) & 0x3F;

  return EFI_SUCCESS;
}
/**
    Search and return the Slot Number 

    @param RootBus - Root Bridge Bus Number
    @param RootDevice - Root Bridge Device Number
    @param RootFunction - Root Bridge Function Number
    @param TargetBus - The Device Number
    @param SlotNumber - Slot Number for Return

    @retval EFI_STATUS.

    @note  None.

**/
EFI_STATUS
SearchSlotNumber(
  IN    UINT8     RootBus,
  IN    UINT8     RootDevice,
  IN    UINT8     RootFunction,
  IN    UINT8     TargetBus,
  OUT   UINT8     *SlotNumber
)
{
  EFI_STATUS      Status;

  UINT64          RootAddress = PCI_LIB_ADDRESS(RootBus,RootDevice,RootFunction,0),NextBusAddress;
  UINT8           PcieHeader = PciRead8(RootAddress+0x0E);
  UINT8           NextBusNumber,BridgeBusNumber,BridgeDev=0,BridgeFun=0;
  UINT16          SlotImplemented;
  UINT8           CapPtr8;
  UINT32          SlotCap;

  DEBUG((EFI_D_ERROR, "[%a][%d]\n\n",__FUNCTION__,__LINE__));

  PcieHeader = PcieHeader&0x7;
  NextBusNumber = RootBus;
  NextBusAddress=RootAddress;
  DEBUG((EFI_D_ERROR, "PcieHeader:%x\n\n",PcieHeader));
  for(;PcieHeader != 0;)
  {
    
    DEBUG((EFI_D_ERROR, "[%d]PcieHeader:%x\n\n",__LINE__,PcieHeader));
    BridgeBusNumber = NextBusNumber;
    NextBusNumber = PciRead8(NextBusAddress+0x19);
    NextBusAddress = PCI_LIB_ADDRESS(NextBusNumber,0,0,0);
    PcieHeader = PciRead8(NextBusAddress+0x0E) & 0x07;
  }

  if(RootBus == BridgeBusNumber)
  {
    BridgeDev = RootDevice;
    BridgeFun = RootFunction;
  }
  Status = GetLegCapStrucAddr(PCI_LIB_ADDRESS(BridgeBusNumber,\
                                                BridgeDev,\
                                                BridgeFun,\
                                                0x00),\
                                                0x10,\
                                                &CapPtr8);
  if(!EFI_ERROR(Status))
  {
    Status = EFI_NOT_FOUND;

    SlotImplemented = PciRead16(PCI_LIB_ADDRESS(BridgeBusNumber,\
                                                BridgeDev,\
                                                BridgeFun,\
                                                CapPtr8+2));
    DEBUG((EFI_D_ERROR, "[bus:%0x][dev:%0x][func:%0x]Slot Implement:%x\n\n",BridgeBusNumber,BridgeDev,BridgeFun,(SlotImplemented & BIT08)));
    if((SlotImplemented & BIT08) == BIT08)
    {
      //DEBUG((EFI_D_ERROR, "[%a][%d]Slot Is Set\n\n",__FUNCTION__,__LINE__));
      SlotCap = PciRead32(PCI_LIB_ADDRESS(BridgeBusNumber,\
                                                BridgeDev,\
                                                BridgeFun,\
                                                CapPtr8+0x14));
      
      *SlotNumber = (SlotCap & 0xFFF80000)>>19;
      if(*SlotNumber==0)
          return EFI_NOT_FOUND;
      Status = EFI_SUCCESS;
      DEBUG((EFI_D_ERROR, "[%d]SlotNumber:%x\n\n",__LINE__,*SlotNumber));
    }

  }

  DEBUG((EFI_D_ERROR, "[%a][%d]Status:%x\n\n",__FUNCTION__,__LINE__,Status));
  return Status;
}

EFI_STATUS
GetPCIeTop(
  IN OUT  SLOT_DESCRIPTION    **SlotDescription
 // IN OUT  UINT8               *SlotDescriptionSize
)
{
    UINT8       BoardGpioVaule=0xff;
    BOARD_SLOT_DESCRIPTION *BoardslotdescTemp=NULL;
    UINT8       CountNumber=0;
    BoardGpioVaule =PcdGet8(PcdBoardGpioVaule);
    DEBUG((-1,"[OemDbg] BoardGpioVaule=%0x\n",BoardGpioVaule));
    if(BoardGpioVaule!=0xff){
        for(CountNumber=0;BoardSlotDescListServer[CountNumber]!=NULL;CountNumber++)
        {
            BoardslotdescTemp=BoardSlotDescListServer[CountNumber];
            DEBUG((-1,"[OemDbg] BoardslotdescTemp->BoardGpioNumber=%0x\n",BoardslotdescTemp->BoardGpioNumber));
            if(BoardslotdescTemp->BoardGpioNumber==BoardGpioVaule)
            {
                *SlotDescription=BoardslotdescTemp->SlotDesc;
                return EFI_SUCCESS;
            }
        }
    }
    return EFI_INVALID_PARAMETER;
}

/**
 * Update Slot Number
 * 1. According to the Basebord Table
 * 2. Try to Read the device's description if the device is not connected to baseboard
 * 3. Return Onboard if no slot number got
 * */
EFI_STATUS
UpdatePcieSlotInfor(
  IN OUT PACKED_PCIE_INFO   *PackedPcieInformation
)
{
  EFI_STATUS          Status=EFI_NOT_FOUND;
//  UINT8               GpuType;
  UINT8               iSlotDescription;//,iSize;
  //UINT8               DieNumber,InstalledCPUs,BusNumber;
  UINT8                 InstalledCPUs;

  CHAR8               SlotName[16];
  UINT8               DownStreamPortBusNumber,DownStreamSecBus,SubordinateBusNumber;
  UINT8               MaxLinkWidth=0;//SlotNumber
  UINT8               iOnBoardDevice=0,sOnBoardDevice=sizeof(OnBoardDeviceList)/sizeof(SLOT_DESCRIPTION);//Mizl-20200525-Mantis 0039640:Redfish Can Get Only One oboard Network Devcice+>
  UINT8               iNvmeDevice=0,sNvmeDevice=sizeof(NvmeList)/sizeof(SLOT_DESCRIPTION);//Mizl-20200525-Mantis 0039640:Redfish Can Get Only One oboard Network Devcice+>
  UINT8              RootBusNumber=0xff;//Socket=0,DieNumber=0,FabricID=0,RootBusNumber=0xff;
  //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 +>
  UINT8              OCPDetectValue=0xff;
  OCPDetectValue=(UINT8)PcdGet8(PcdOcpDetectValue);
  DEBUG((-1,"[OemDbg][%a][%d] OCPDetectValue=%0x\n",__FUNCTION__,__LINE__,OCPDetectValue));
  //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 ->
  /*GpuType = (UINT8)PcdGet8(PcdGPUType);	
  switch(GpuType){
    case X785_BALANCED_MODE_Not_Auto_Detect: //Mizl-20200617-Add Auto Detect GPU Function
    case X785_BALANCED_MODE:
      pSlotDescription = &X785_Balanced_Mode_Table[0];
      iSize = sizeof(X785_Balanced_Mode_Table)/sizeof(SLOT_DESCRIPTION);
      break;
    case X785_COMMON_MODE_Not_Auto_Detect: //Mizl-20200617-Add Auto Detect GPU Function
    case X785_COMMON_MODE:
      pSlotDescription = &X785_Common_Mode_Table[0];
      iSize = sizeof(X785_Common_Mode_Table)/sizeof(SLOT_DESCRIPTION);
      break;
    case X745:
      pSlotDescription = &X745_Mode[0];
      iSize = sizeof(X745_Mode)/sizeof(SLOT_DESCRIPTION);
      break;
    default:
      DEBUG((EFI_D_ERROR, "[%a][%d]Unsupported GPUType:%x\n\n",__FUNCTION__,__LINE__,GpuType));
      return EFI_INVALID_PARAMETER;
  }

  InstalledCPUs = (UINT8)PcdGet32(PcdNumberOfInstalledProcessors);
  DEBUG((EFI_D_ERROR, "[%a][%d]InstalledCPUs:%x\n\n",__FUNCTION__,__LINE__,InstalledCPUs));

  if((InstalledCPUs != 1) && (InstalledCPUs !=2))
    return EFI_INVALID_PARAMETER;

  if(InstalledCPUs ==1)
    BusNumber = 0x40;
  else
    BusNumber = 0x20;*/
  
  //DEBUG((EFI_D_ERROR, "[%a][%d]iSize:%d\n\n",__FUNCTION__,__LINE__,iSize));
 InstalledCPUs = (UINT8)PcdGet32(PcdNumberOfInstalledProcessors);
 /* Status = GetPCIeTop(&pSlotDescription);//,&SlotDescriptionSize);
  if ( EFI_ERROR (Status) ) 
    return EFI_UNSUPPORTED;
  */
  if (pSlotDescription == NULL) {
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] pSlotDescription is null\n\n",__FUNCTION__,__LINE__));
    return EFI_UNSUPPORTED;
  }
 // for(iSlotDescription=0;iSlotDescription < iSize;iSlotDescription++)
  for(iSlotDescription=0;pSlotDescription[iSlotDescription].DieNumber!=INVALID_SETTING;iSlotDescription++)
  {
    //DieNumber = PackedPcieInformation->ParentPcieBusNum/BusNumber;
     if((InstalledCPUs <2)&&(pSlotDescription[iSlotDescription].DieNumber > DIE3))
          continue;
     RootBusNumber=RootPortBusBase[pSlotDescription[iSlotDescription].DieNumber];

     /*      if(pSlotDescription[iSlotDescription].DieNumber > DIE3)
      { 
          Socket=1;
          DieNumber=pSlotDescription[iSlotDescription].DieNumber -4;
      }
      else
      {          
          Socket=0;
          DieNumber=pSlotDescription[iSlotDescription].DieNumber;
          
      }
      
      //NBIO 2 is pci lane 112-127  NBIO3 is pcie lane 80-95
    RootBusNumber= GetCPURbBusNumAndFabricId(Socket,DieNumber,&FabricID);
*/
     DEBUG((EFI_D_ERROR, "[%a][%d]RootBusNumber:%x\n",__FUNCTION__,__LINE__,RootBusNumber));
     DEBUG((EFI_D_ERROR, "[%a][%d]PackedPcieInformation->ParentPcieBusNum:%x\n",__FUNCTION__,__LINE__,PackedPcieInformation->ParentPcieBusNum));
     DEBUG((EFI_D_ERROR, "ParentPcieDevNum:%x\n",PackedPcieInformation->ParentPcieDevNum));
     DEBUG((EFI_D_ERROR, "ParentPcieFunNum:%x\n\n",PackedPcieInformation->ParentPcieFunNum));

    /*if((PackedPcieInformation->ParentPcieBusNum == RootBusNumber)\
      &&(PackedPcieInformation->ParentPcieDevNum == pSlotDescription[iSlotDescription].RootDevice)\
      &&(PackedPcieInformation->ParentPcieFunNum == pSlotDescription[iSlotDescription].RootFunction))*/
        /*DEBUG((EFI_D_ERROR, "[%a][%d]RootBusNumber:%x\n",__FUNCTION__,__LINE__,RootBusNumber));
        DEBUG((EFI_D_ERROR, "ParentPcieDevNum:%x\n",PackedPcieInformation->ParentPcieDevNum));
        DEBUG((EFI_D_ERROR, "ParentPcieFunNum:%x\n\n",PackedPcieInformation->ParentPcieFunNum));*/
        if(pSlotDescription[iSlotDescription].DownStreamDevice != 0xff)
         {
            //DEBUG((EFI_D_ERROR, "[%a][%d]PackedPcieInformation->ParentPcieBusNum:%x\n",__FUNCTION__,__LINE__,PackedPcieInformation->ParentPcieBusNum));
           //DownStreamPortBusNumber = PciRead8(((PackedPcieInformation->ParentPcieBusNum+2) << 15) + (pSlotDescription[iSlotDescription].DownStreamDevice << 12) + 0x19);
            DownStreamSecBus = PciRead8(PCI_LIB_ADDRESS(RootBusNumber+2,pSlotDescription[iSlotDescription].DownStreamDevice,0,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));            
            DEBUG((EFI_D_ERROR, "[%a][%d]DownStreamSecBus:%x\n",__FUNCTION__,__LINE__,DownStreamSecBus));
            if(pSlotDescription[iSlotDescription].DownStreamDevice1 != 0xff){
                    DownStreamPortBusNumber = PciRead8(PCI_LIB_ADDRESS(DownStreamSecBus+1,pSlotDescription[iSlotDescription].DownStreamDevice1,0,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
               /* else
                    DownStreamPortBusNumber=DownStreamSecBus;
           DEBUG((EFI_D_ERROR, "[%a][%d]DownStreamPortBusNumber:%x;PackedPcieInformation->PcieBusNum=%0x\n\n",__FUNCTION__,__LINE__,DownStreamPortBusNumber,PackedPcieInformation->PcieBusNum));
           if(DownStreamPortBusNumber != PackedPcieInformation->PcieBusNum)*/
                    DEBUG((EFI_D_ERROR, "[%a][%d]DownStreamPortBusNumber:%x;\n\n",__FUNCTION__,__LINE__,DownStreamPortBusNumber));
                    SubordinateBusNumber = PciRead8(PCI_LIB_ADDRESS(DownStreamSecBus+1,pSlotDescription[iSlotDescription].DownStreamDevice1,0,PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
                    DEBUG((EFI_D_ERROR, "[%a][%d]SubordinateBusNumber:%x;\n\n",__FUNCTION__,__LINE__,SubordinateBusNumber));


          }
         }
        else{
            DownStreamPortBusNumber = PciRead8(PCI_LIB_ADDRESS(RootBusNumber,pSlotDescription[iSlotDescription].RootDevice,pSlotDescription[iSlotDescription].RootFunction,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
            DEBUG((EFI_D_ERROR, "[%a][%d]DownStreamPortBusNumber:%x;\n\n",__FUNCTION__,__LINE__,DownStreamPortBusNumber));
       /* else
            DownStreamPortBusNumber=DownStreamSecBus;
  // DEBUG((EFI_D_ERROR, "[%a][%d]DownStreamPortBusNumber:%x;PackedPcieInformation->PcieBusNum=%0x\n\n",__FUNCTION__,__LINE__,DownStreamPortBusNumber,PackedPcieInformation->PcieBusNum));
   if(DownStreamPortBusNumber != PackedPcieInformation->PcieBusNum)*/
            SubordinateBusNumber = PciRead8(PCI_LIB_ADDRESS(RootBusNumber,pSlotDescription[iSlotDescription].RootDevice,pSlotDescription[iSlotDescription].RootFunction,PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
            DEBUG((EFI_D_ERROR, "[%a][%d]SubordinateBusNumber:%x;\n\n",__FUNCTION__,__LINE__,SubordinateBusNumber));
        }
            if((PackedPcieInformation->PcieBusNum < DownStreamPortBusNumber ) || (PackedPcieInformation->PcieBusNum > SubordinateBusNumber))
                continue;
        
        DEBUG((EFI_D_ERROR, "[%a][%d]Found the Slot's Description!!\n",__FUNCTION__,__LINE__));
        DEBUG((EFI_D_ERROR, "[%a][%d]Root Label:%a  SlotNumber:%x\n",__FUNCTION__,__LINE__,\
                                              pSlotDescription[iSlotDescription].RootLabel,\
                                              pSlotDescription[iSlotDescription].SlotNumber));
        //Mizl-20200519-Update Whole Pcie Topy+>
        // //Mizl-20200422-X745 Slot Number Shown At BMC Web is not correct+>
        // //Now we need deal with X745 for special
        // if(GpuType == X745)
        // {
        //   //Now Try to Get Parent Max Link Width
        //   MaxLinkWidth = 0;
        //   Status = GetMaxLinkWidth(PackedPcieInformation->ParentPcieBusNum,PackedPcieInformation->ParentPcieDevNum,PackedPcieInformation->ParentPcieFunNum,&MaxLinkWidth);
        //   DEBUG((EFI_D_ERROR, "[%a][%d]Slot Number:%x\n\n",__FUNCTION__,__LINE__));
        //   if(!EFI_ERROR(Status) && (MaxLinkWidth == 0x08) && \
        //     (pSlotDescription[iSlotDescription].SlotNumber > 1) &&\
        //     ((pSlotDescription[iSlotDescription].SlotNumber % 2) == 0))

        //       pSlotDescription[iSlotDescription].SlotNumber = pSlotDescription[iSlotDescription].SlotNumber+1;
        // }

		    // //Mizl-20200509-mantis 0039084 Update Device Location+>
        //Mizl-20200422-X745 Slot Number Shown At BMC Web is not correct->
        //Mizl-20200519-Update Whole Pcie Topy->
        Sprintf(SlotName,"Baseboard_Slot%02d",pSlotDescription[iSlotDescription].SlotNumber);
		    //Mizl-20200509-mantis 0039084 Update Device Location->
        MemCpy(PackedPcieInformation->PcieLocation,SlotName,sizeof(SlotName));
        DEBUG((EFI_D_ERROR, "=========================================================\n\n"));
        Status = EFI_SUCCESS;
        break;
     
  }

  if(EFI_ERROR(Status))
  {
    //Try to Read the Slot Implement
    //PciAddress = PCI_LIB_ADDRESS(PackedPcieInformation->PcieBusNum,PackedPcieInformation->PcieDevNum,PackedPcieInformation->PcieFunNum);
 /*   Status = SearchSlotNumber(  PackedPcieInformation->ParentPcieBusNum,\
                                PackedPcieInformation->ParentPcieDevNum,\
                                PackedPcieInformation->ParentPcieFunNum,\
                                PackedPcieInformation->PcieBusNum,\
                                &SlotNumber);
*/
//Mizl-20200509-mantis 0039084 Update Device Location+>
    if(EFI_ERROR(Status))
    {
      //Mizl-20200525-Mantis 0039640:Redfish Can Get Only One oboard Network Devcice+>
      //Try To Loop Onboard List
      for (iOnBoardDevice = 0; iOnBoardDevice < sOnBoardDevice; iOnBoardDevice++)
      {
        /* code */
          DEBUG((EFI_D_ERROR, "=========================================================\n\n"));
          DEBUG((EFI_D_ERROR, "OnBoardDevice\n\n"));
          DEBUG((EFI_D_ERROR, "ParentPcieDevNum:%x\n",PackedPcieInformation->ParentPcieDevNum));
          DEBUG((EFI_D_ERROR, "ParentPcieFunNum:%x\n\n",PackedPcieInformation->ParentPcieFunNum));
          DEBUG((EFI_D_ERROR, "PackedPcieInformation->ParentPcieBusNum:%x\n",PackedPcieInformation->ParentPcieBusNum));
          DEBUG((EFI_D_ERROR, "RootPortBusBase[OnBoardDeviceList[iOnBoardDevice].DieNumber:%x\n\n",RootPortBusBase[OnBoardDeviceList[iOnBoardDevice].DieNumber]));
        if(( OnBoardDeviceList[iOnBoardDevice].RootDevice == PackedPcieInformation->ParentPcieDevNum) &&\
            (OnBoardDeviceList[iOnBoardDevice].RootFunction == PackedPcieInformation->ParentPcieFunNum)&&\
            (RootPortBusBase[OnBoardDeviceList[iOnBoardDevice].DieNumber]==PackedPcieInformation->ParentPcieBusNum))
            {
            //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 +>
            if((OCPDetectValue!=0)&&((OnBoardDeviceList[iOnBoardDevice].DieNumber==0x2)&&(OnBoardDeviceList[iOnBoardDevice].RootDevice==0x3)&&
                    ((OnBoardDeviceList[iOnBoardDevice].RootFunction ==0x3)||(OnBoardDeviceList[iOnBoardDevice].RootFunction ==0x7))))
            {
                //Sprintf(SlotName,"OnBoard_%02x",OnBoardDeviceList[iOnBoardDevice].SlotNumber);//Mizl-20200506-distinct Onboard Devices by Index
                MemCpy(SlotName,"Baseboard_Slot1",sizeof(SlotName));
                continue;
            }
            else{
                DEBUG((EFI_D_ERROR, "[%a][%d]%a\n\n",__FUNCTION__,__LINE__,OnBoardDeviceList[iOnBoardDevice].RootLabel));
                //Sprintf(SlotName,"OnBoard_%02x",OnBoardDeviceList[iOnBoardDevice].SlotNumber);//Mizl-20200506-distinct Onboard Devices by Index
                MemCpy(SlotName,OnBoardDeviceList[iOnBoardDevice].RootLabel,sizeof(SlotName));
                continue;
            }
            //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 ->
            }
      }
      for (iNvmeDevice = 0; iNvmeDevice < sNvmeDevice; iNvmeDevice++)
            {
              /* code */
                DEBUG((EFI_D_ERROR, "=========================================================\n\n"));
                DEBUG((EFI_D_ERROR, "NvmeDevice\n\n"));
                DEBUG((EFI_D_ERROR, "ParentPcieDevNum:%x\n",PackedPcieInformation->ParentPcieDevNum));
                DEBUG((EFI_D_ERROR, "ParentPcieFunNum:%x\n\n",PackedPcieInformation->ParentPcieFunNum));
                DEBUG((EFI_D_ERROR, "PackedPcieInformation->ParentPcieBusNum:%x\n",PackedPcieInformation->ParentPcieBusNum));
                DEBUG((EFI_D_ERROR, "RootPortBusBase[NvmeList[iNvmeDevice].DieNumber:%x\n\n",RootPortBusBase[NvmeList[iNvmeDevice].DieNumber]));
              if(( NvmeList[iNvmeDevice].RootDevice == PackedPcieInformation->ParentPcieDevNum) &&\
                  (NvmeList[iNvmeDevice].RootFunction == PackedPcieInformation->ParentPcieFunNum)&&\
                  (RootPortBusBase[NvmeList[iNvmeDevice].DieNumber]==PackedPcieInformation->ParentPcieBusNum))
                  {               
                      DEBUG((EFI_D_ERROR, "[%a][%d]%a\n\n",__FUNCTION__,__LINE__,NvmeList[iNvmeDevice].RootLabel));
                      //Sprintf(SlotName,"OnBoard_%02x",OnBoardDeviceList[iOnBoardDevice].SlotNumber);//Mizl-20200506-distinct Onboard Devices by Index
                      MemCpy(SlotName,NvmeList[iNvmeDevice].RootLabel,sizeof(SlotName));
                      continue;
                  }
                  //<xuyj1-20210731- Add the GPIO to distinguish between the OCP and the basebord slot1 ->               
            }
      //Sprintf(SlotName,"OnBoard_%02x",iOnBoardDevice++);//Mizl-20200506-distinct Onboard Devices by Index
      //Mizl-20200525-Mantis 0039640:Redfish Can Get Only One oboard Network Devcice->
    }
  //  else
  //    Sprintf(SlotName,"Mainboard_Slot%02d",SlotNumber);
    
    MemCpy(PackedPcieInformation->PcieLocation,SlotName,sizeof(SlotName));
  }

//Mizl-20200509-mantis 0039084 Update Device Location->
  return EFI_SUCCESS;
}
//Mizl-20200406-Porting for X785 Slot Number->


/*//Mizl-20200515-Filter NVMe Disk From PCIeList+>
/**
    Got the device based on the Class,Sub-Class and interface.

    @param  Options       boot device
**/
/*BOOLEAN NVMePolicyFilter(
    IN PCIE_DEV_INFO  Device
)
{
    UINT8               FilterDevInterface;

    if((Device.BaseClassCode == OemNvme_PCI_CLASS_MASS_STORAGE) && \
        (Device.SubClassCode == PCI_CLASS_MASS_STORAGE_SOLID_STATE))
        {

            //Try to read the PCI Interface
            FilterDevInterface = PciRead8(PCI_LIB_ADDRESS(  Device.BusNum,\
                                                            Device.DevNum,\
                                                            Device.FuncNum,\
                                                            PCI_IFT));

            if(FilterDevInterface == PCI_IF_MASS_STORAGE_SOLID_STATE_ENTERPRISE_NVMHCI)
                return TRUE;
        }

    return FALSE;
}
//Mizl-20200515-Filter NVMe Disk From PCIeList->*/
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
  OEM_PCIE_DEV_DATA_PROTOCOL    *OemPcieDevProtocol;
  PCI_DEV_SETUP_FORM_DATA         *PcieDevData;


  PACKED_PCIE_INFO                OemPcieInfo;    
  PACKED_NIC_INFO                 OemNicInfo;
  UINT8                           NicCount,PcieCount;
  UINT8                           NicCategory;
  UINT8                           DevIdx,PortIdx;
  PCI_DEV_IDX_DATA                *IdxDev;
  PCIE_DEV_INFO                   PcieDevInfo;
  PCIE_NIC_INFO                   *PcieNicInfo;
  UINT8   			  			  ResponseDataSize=0;
  UINT8              Socket=0,DieNumber=0,FabricID=0,CountNumber=0;
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
  Status = GetPCIeTop(&pSlotDescription);
  if ( EFI_ERROR (Status) ) 
  {
      DEBUG((EFI_D_ERROR, "[OemDebug] not find the correct pcie slot description list ,please check pcie slot description list define\n")); 
  }
  for(CountNumber=0;CountNumber<MaxDieNumber;CountNumber++){
      if(CountNumber > 3)
      {
          Socket=1;
          DieNumber=CountNumber-4;
      }
      else
      {
          Socket=0;
          DieNumber=CountNumber; 
      }
          
      RootPortBusBase[CountNumber]=GetCPURbBusNumAndFabricId(Socket,DieNumber,&FabricID);
  }
  
  NicCount = 0;
  PcieCount = 0;
  
  for(DevIdx=0;DevIdx<PcieDevData->PciDevCount;DevIdx++){
      IdxDev = PcieDevData->PciDevIdxData[DevIdx];
    for(PortIdx=0;PortIdx<IdxDev->PortCount;PortIdx++){
      ZeroMem(&OemPcieInfo, sizeof(PACKED_PCIE_INFO)); //<Lizhq035-20170103 Variable init>
      ZeroMem(&PcieDevInfo, sizeof(PCIE_DEV_INFO));
      Status = OemPcieDevProtocol->GetPcieDevInfo(PcieDevData,IdxDev->PciDevIdx,PortIdx,&PcieDevInfo);
      //Mizl-20210228-Add Filter For 88080/88048 LSI Device+>
      if(!EFI_ERROR(FilterDeviceList(PcieDevInfo,&IdxDev->PortData[PortIdx])))
        continue;
      //Mizl-20210228-Add Filter For 88080/88048 LSI Device->

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
      AsciiStrCpy(OemPcieInfo.PcieBrandName,PcieDevInfo.BrandName);            
      
      /*//Mizl-20200515-Filter NVMe Disk From PCIeList+>
      if( NVMePolicyFilter(PcieDevInfo) == TRUE)
        continue;
      //Mizl-20200515-Filter NVMe Disk From PCIeList->*/


      //Mizl-20200408-Sync PCIe Location with BMC+>
      DEBUG((EFI_D_ERROR, "[%a][%d]LinkWidth:%x\n\n",__FUNCTION__,__LINE__,PcieDevInfo.LinkWidth));
      UpdatePcieSlotInfor(&OemPcieInfo);
      AsciiStrCpy(PcieDevInfo.PcieLocation,OemPcieInfo.PcieLocation);
      AsciiStrCpy(IdxDev->PcieLocation,OemPcieInfo.PcieLocation);
      DEBUG((EFI_D_ERROR, "[%a][%d]IdxDev->PortData[0].Prefix:%s OemPcieInfo.PcieLocation:%s \n\n",__FUNCTION__,__LINE__,IdxDev->PcieLocation,OemPcieInfo.PcieLocation));
      //Mizl-20200408-Sync PCIe Location with BMC->
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
        //Initialize OEM IPMI package header
        OemNicInfo.SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypeNic;
        OemNicInfo.SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
        OemNicInfo.DataValid = BIOSPackageValid;
        //Mizl-20200408-Sync PCIe Location with BMC+>
        CopyMem(OemNicInfo.NicLocation,PcieDevInfo.PcieLocation,16);
        //Mizl-20200408-Sync PCIe Location with BMC->
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

	//InitAmiLib(ImageHandle, SystemTable);
	
	Status = OemIpmiSetRBPcieInfo();
	if (EFI_ERROR (Status)) {
		return Status;
	}
	

	return  Status;
}

//***************************************************************
