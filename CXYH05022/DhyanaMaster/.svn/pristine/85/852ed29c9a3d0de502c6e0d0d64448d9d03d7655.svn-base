/** @file
 *  OemBds.c

  This file contains Oem Bds Hooks for dealing with boot options and their maintenance

  $Revision:0A
  $Date:  lvych 2018/09/21

  Copyright (C) 2018 Dawning Information Industry Co.,LTD.All rights reserved.<BR>

**/

#include "Token.h"
#include "OemBds.h"
#include "OemPCIeDev.h"
#include <Include\OemPCIeDevCodeId.h>

BOOT_OPTION_OEM_TAG BootDeviceTagPriorities[] = {
    BOOT_OPTION_DEVICE_TAG_PRIORITIES,
    UNASSIGNED_HIGHEST_TAG
};

//SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
typedef enum{
//    BoTagLegacyXxx
//    BoTagUefiXxx
//    BoTagXxx
    BoTagLegacyFloppy,
    BoTagLegacyHardDisk,
    BoTagLegacyCdrom,
    BoTagLegacyPcmcia,
    BoTagLegacyUsb,
    BoTagLegacyEmbedNetwork,
    BoTagLegacyBevDevice,
    BoTagUefi,
    BoTagEmbeddedShell,
    BoTagUefiHardDisk,
    BoTagUefiCdrom,
    BoTagUefiUsbHardDisk,
    BoTagUefiUsbKey,
    BoTagUefiUsbCdrom,
    BoTagUefiNetWork,
    BoTagUefiIP4NetWork,
    BoTagUefiIP6NetWork,
    BoTagUefiUriNetWork,
    BoTagUefiIP4UriNetWork,
    BoTagUefiIP6UriNetWork,
} BOOT_OPTION_TAG;

#if BOOT_OPTION_DEVICE_TAG_ASM1061_M2_SUPPORT == 1
PCI_BRIDGE_LIST gAsm1061RootPortList[] = {ARM1061_M2_SLOT_LIST };
#endif
//SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->


//<Wangyia001-20190921 High speed PXE first for UEFI and Legacy mode +>
#if defined(HIGH_SPEED_PXE_FIRST) && (HIGH_SPEED_PXE_FIRST == 1)
#if defined(PINGAN_PROJECT_SUPPORT) && (PINGAN_PROJECT_SUPPORT == 1)
//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern EFI_DEVICE_PATH_PROTOCOL* GetFirstDevicePathNodeOfType(EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type);
extern EFI_DEVICE_PATH_PROTOCOL* GetLastDevicePathNodeOfType(EFI_DEVICE_PATH_PROTOCOL *Dp, UINT8 Type);
extern EFI_HANDLE GetPhysicalBlockIoHandle(EFI_HANDLE BlockIoHandle);
extern EFI_HANDLE GetPhysicalNetworkCardHandle(EFI_HANDLE Handle, UINT8 *IpType, BOOLEAN *Uri);
#endif
#endif
//<Wangyia001-20190921 High speed PXE first for UEFI and Legacy mode ->

UINT32 FindDeviceTagPriority(UINT16 Tag){
    UINT32 i;
    for(i=0; BootDeviceTagPriorities[i]!=UNASSIGNED_HIGHEST_TAG; i++)
        if (Tag==BootDeviceTagPriorities[i]) return i;
    return UNASSIGNED_HIGHEST_TAG;
}

//<Yaosr001-20181207 Import the OemBds module to support AMD Platform +>
UINT32 BootSataPortPrioritiesS0D0[] = {
    BOOT_SATA_PORT_PRIORITIES_S0D0,
    UNASSIGNED_HIGHEST_TAG
};
UINT32 BootSataPortPrioritiesS0D1[] = {
    BOOT_SATA_PORT_PRIORITIES_S0D1,
    UNASSIGNED_HIGHEST_TAG
};
UINT32 BootSataPortPrioritiesS1D0[] = {
    BOOT_SATA_PORT_PRIORITIES_S1D0,
    UNASSIGNED_HIGHEST_TAG
};
UINT32 BootSataPortPrioritiesS1D1[] = {
    BOOT_SATA_PORT_PRIORITIES_S1D1,
    UNASSIGNED_HIGHEST_TAG
};
//SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
#if BOOT_OPTION_DEVICE_TAG_ASM1061_M2_SUPPORT == 1
BOOLEAN IsAsm1061M2(UINT32 Bus)
{
    UINT8  i, j, Count, FabricId, RootBus;
    UINT32 CfgAddressMap, SecBus;
    
    Count = sizeof(gAsm1061RootPortList) / sizeof(PCI_BRIDGE_LIST);
    for(i = 0; i < Count; i++)
    {
        FabricId = (UINT8)(gAsm1061RootPortList[i].SocNum << 5) + gAsm1061RootPortList[i].NbioNum + 0x18;
        for(j = 0; j < 0x20; j += 4)
        {
            CfgAddressMap = PciRead32(PCI_LIB_ADDRESS(0,0x18,0,(0xA0 + j)));
            if (FabricId == (UINT8)(CfgAddressMap >> 4))
            {
                RootBus = (UINT8)(CfgAddressMap >> 16);
                SecBus = PciRead32(PCI_LIB_ADDRESS(RootBus, gAsm1061RootPortList[i].DevNum, gAsm1061RootPortList[i].FunNum, 0x18));
                if (Bus == ((SecBus >> 8) & 0xFF))
                    return TRUE;
                else
                    break;
            }
        }
    }
    return FALSE;
}
#endif
//SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->

UINT32 FindSataPortPriority(UINT32 Bus, UINT8 PortNumber){
  UINT32 i;
  UINT32 SataPortPriority;
  UINT32 DiePriority;
  UINT32 *ptr = NULL;
  
  if(Bus <= 0x0F){
    ptr = BootSataPortPrioritiesS0D0;
    DiePriority = 0;
  }else if(Bus >= 0x10 && Bus <= 0x1F){
    ptr = BootSataPortPrioritiesS0D1;
    DiePriority = 1;      
  }else if(Bus >= 0x40 && Bus <= 0x4F){
    ptr = BootSataPortPrioritiesS1D0;
    DiePriority = 2;       
  }else if(Bus >= 0x50 && Bus <= 0x5F){
    ptr = BootSataPortPrioritiesS1D1;
    DiePriority = 3;       
  }

  for(i=0; ptr[i] != UNASSIGNED_HIGHEST_TAG; i++)
      if(PortNumber == ptr[i]){
        SataPortPriority = 8*DiePriority+i;
        return SataPortPriority;
      }
  return UNASSIGNED_HIGHEST_TAG;
}
//<Yaosr001-20181207 Import the OemBds module to support AMD Platform ->

PCI_DEV_NINFO *GetDevBaseInfo(UINT16 VenId, UINT16 DevId, UINT16 SubSID ,UINT8 *dFound)
{
  PCI_DEV_NINFO  *PCIDevEntry;
  UINT8          Data;

  if(dFound == NULL) dFound =&Data;

  PCIDevEntry = &PCIDevDB[0];
  do {
    if( (PCIDevEntry->VendorId == VenId) && (PCIDevEntry->DeviceId == DevId) ){
      if((PCIDevEntry->SubsystemID == SubSID)||(PCIDevEntry->SubsystemID == 0xFFFF)){
          *dFound = 1;
          break;
      }
    }
    if((PCIDevEntry->VendorId == 0xFFFF) && (PCIDevEntry->DeviceId == 0xFFFF)){
        *dFound = 0;
        break;
    }
      PCIDevEntry++;
  } while(TRUE);

  return PCIDevEntry;
}

UINT32 FindNicPriority(UINT8 Seg,UINT8 Bus,UINT8 Dev,UINT8 Func)
{
  PCI_DEV_NINFO         *ptrdninfo;
  UINT8                 dfound=0;
  UINT16                VendorID;
  UINT16                DeviceID;
  UINT16                SubSystemId;

  VendorID = PciRead16(PCI_LIB_ADDRESS(Bus,Dev,Func,PCI_VID));
  DeviceID = PciRead16(PCI_LIB_ADDRESS(Bus,Dev,Func,PCI_DID));
  SubSystemId = PciRead16(PCI_LIB_ADDRESS(Bus,Dev,Func,PCI_SID));

  ptrdninfo = GetDevBaseInfo(VendorID, DeviceID, SubSystemId, &dfound);
  if(dfound){
    if(ptrdninfo->Speed)
      return ((UINT8)~ptrdninfo->Speed);//min low priority
  }

  return UNASSIGNED_HIGHEST_TAG;
}

UINT32 GetOptionHighPriority(BOOT_OPTION *Option,PRIORITY_ATTRIBUTE *PriorityAtr)
{
  BBS_TABLE             *BbsTable;
  UINT8                 Class;
  UINT8                 SubClass;
  UINT32                Bus;
  UINT32                Device;
  UINT32                Function;

  //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
  EFI_STATUS                Status;
  UINTN                     Seg;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath = NULL;
  EFI_HANDLE                Handle = NULL;
  EFI_PCI_IO_PROTOCOL       *PciIo = NULL;
  //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->

  if(NULL == PriorityAtr)
    return UNASSIGNED_HIGHEST_TAG;

  PriorityAtr->HighPriority = UNASSIGNED_HIGHEST_TAG;
  PriorityAtr->Tag = UNASSIGNED_HIGHEST_TAG;

  if (Option->BbsEntry != NULL)
  {
    BbsTable = Option->BbsEntry;
    Bus = BbsTable->Bus;
    Device = BbsTable->Device;
    Function = BbsTable->Function;
    Class = BbsTable->Class;
    SubClass = BbsTable->SubClass;
    //TRACE((-1,"Class:%x,SubClass:%x ",Class,SubClass));
    switch(BbsTable->DeviceType){
      case BBS_HARDDISK:
        if(Class == PCI_CLASS_MASS_STORAGE){
          if(SubClass == PCI_CLASS_MASS_STORAGE_SATADPA){//AHCI
            //<Yaosr001-20181207 Import the OemBds module to support AMD Platform +>
            /*if(Bus == SATA_BUS && Device == SATA_DEV && Function == SATA_FUNC){//sata
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagSata);
              PriorityAtr->Tag = BoTagSata;
            }else if(Bus == SSATA_BUS && Device == SSATA_DEV && Function == SSATA_FUNC){//ssata
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagsSata);
              PriorityAtr->Tag = BoTagsSata;
            }*/
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagSata);
            PriorityAtr->Tag = BoTagSata;
            PriorityAtr->BusNum = Bus; //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
            //<Yaosr001-20181207 Import the OemBds module to support AMD Platform ->
            //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
            #if BOOT_OPTION_DEVICE_TAG_ASM1061_M2_SUPPORT == 1
            if (IsAsm1061M2(Bus))
            {
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagASM1061M2);
              PriorityAtr->Tag = BoTagASM1061M2;
            }
            #endif
            //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->
          }else if(SubClass == PCI_CLASS_MASS_STORAGE_SOLID_STATE){//NVME
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagNvme);
            //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
            #if BOOT_OPTION_DEVICE_TAG_NVME_M2_SUPPORT == 1
            if (IsNvmeM2(Bus))
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagNvmeM2);
            #endif
            //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->

            PriorityAtr->Tag = BoTagNvme;
            PriorityAtr->BusNum = Bus; //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
          }else if(SubClass == PCI_CLASS_MASS_STORAGE_SAS){//SAS
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagSAS);
            PriorityAtr->Tag = BoTagSAS;
            PriorityAtr->BusNum = Bus; //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
          //<hanzhr001-20181107-Add RAID to Change Boot Option+>
          }else if(SubClass == PCI_CLASS_MASS_STORAGE_RAID){//RAID
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagRAID);
            PriorityAtr->Tag = BoTagRAID;
            //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
            #if BOOT_OPTION_DEVICE_TAG_ASM1061_M2_SUPPORT == 1
            if (IsAsm1061M2(Bus))
            {
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagASM1061M2);
              PriorityAtr->Tag = BoTagASM1061M2;
            }
            #endif
            PriorityAtr->BusNum = Bus;
            //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->
          //<hanzhr001-20181107-Add RAID to Change Boot Option->
          }
        }
        break;
      case BBS_EMBED_NETWORK:
#if defined(HIGH_SPEED_PXE_FIRST) && (HIGH_SPEED_PXE_FIRST == 1)
        PriorityAtr->HighPriority = FindNicPriority(0,Bus,Device,Function);
#endif
        break;
        default:
        break;
    }
  }
  
  //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support +>
  else {

      UINTN                     TempBus = 0xFF;
      UINTN                     TempDevice = 0xFF;
      UINTN                     TempFunction = 0xFF;

      Status = pBS->HandleProtocol(Option->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&TempDevicePath);
      if ( Status != EFI_SUCCESS )
      {
          DEBUG((DEBUG_ERROR, "HandleProtocol gEfiDevicePathProtocolGuid Status is %r\n",Status));
          return PriorityAtr->HighPriority;
      }
      Status = pBS->LocateDevicePath(
                      &gEfiPciIoProtocolGuid,
                      &TempDevicePath,
                      &Handle);
      if ( Status != EFI_SUCCESS )
      {
          DEBUG((DEBUG_ERROR, "LocateDevicePath gEfiPciIoProtocolGuid Status is %r\n",Status));
          return PriorityAtr->HighPriority;
      }
      Status = pBS->HandleProtocol ( Handle,
                                     &gEfiPciIoProtocolGuid,
                                     &PciIo );
      if ( Status != EFI_SUCCESS )
      {
          DEBUG((DEBUG_ERROR, "HandleProtocol gEfiPciIoProtocolGuid Status is %r\n",Status));
          return PriorityAtr->HighPriority;
      }
      
      Status=PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xB, 1, &Class);
      Status=PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xA, 1, &SubClass);
      Status = PciIo->GetLocation(PciIo, &Seg, &TempBus, &TempDevice, &TempFunction);

      Bus = (UINT32)TempBus;
      Device = (UINT32)TempDevice;
      Function = (UINT32)TempFunction;

#if defined(HIGH_SPEED_PXE_FIRST) && (HIGH_SPEED_PXE_FIRST == 1)      
      if (Class == PCI_CL_NETWORK) {

          PriorityAtr->HighPriority = FindNicPriority(0,(UINT8)Bus,(UINT8)Device,(UINT8)Function);
          return PriorityAtr->HighPriority;
      }
#endif
      if ((Option->Tag & 0xFF) == BoTagUefiHardDisk)
      {
         if(SubClass == PCI_CLASS_MASS_STORAGE_SATADPA){//AHCI
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagSata);
            PriorityAtr->Tag = BoTagSata;
            PriorityAtr->BusNum = Bus;
            #if BOOT_OPTION_DEVICE_TAG_ASM1061_M2_SUPPORT == 1
            if (IsAsm1061M2(Bus))
            {
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagASM1061M2);
              PriorityAtr->Tag = BoTagASM1061M2;
            }
            #endif
          }else if(SubClass == PCI_CLASS_MASS_STORAGE_SOLID_STATE){//NVME
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagNvme);
            #if BOOT_OPTION_DEVICE_TAG_NVME_M2_SUPPORT == 1
            if (IsNvmeM2(Bus))
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagNvmeM2);
            #endif
            PriorityAtr->Tag = BoTagNvme;
            PriorityAtr->BusNum = Bus;
          }else if(SubClass == PCI_CLASS_MASS_STORAGE_SAS){//SAS
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagSAS);
            PriorityAtr->Tag = BoTagSAS;
            PriorityAtr->BusNum = Bus;
          }else if(SubClass == PCI_CLASS_MASS_STORAGE_RAID){//RAID
            PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagRAID);
            PriorityAtr->Tag = BoTagRAID;
            #if BOOT_OPTION_DEVICE_TAG_ASM1061_M2_SUPPORT == 1
            if (IsAsm1061M2(Bus))
            {
              PriorityAtr->HighPriority = FindDeviceTagPriority(BoTagASM1061M2);
              PriorityAtr->Tag = BoTagASM1061M2;
            }
            #endif
            PriorityAtr->BusNum = Bus;
          }
      }
  }
  
  //SGMTB#164021:Add M.2 Boot Option Boot Sequence Support ->
  return PriorityAtr->HighPriority;
}



UINT32 GetOptionMidPriority(BOOT_OPTION *Option,PRIORITY_ATTRIBUTE *PriorityAtr)
{
  EFI_STATUS  Status;
  BBS_TABLE                         *BbsTable;
  static EFI_GUID DiskInfoGuid      = EFI_DISK_INFO_PROTOCOL_GUID;
  EFI_DISK_INFO_PROTOCOL            *DiskInfo;
  EFI_HANDLE                        DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL          *DevPath = NULL;
  VOID                              *Dev;
  UINT32                            Bus;
  UINT32                            Device;
  UINT32                            Function;
  UINT8                             PortNumber;
  CHAR16                            *Ptr;


  if(NULL == PriorityAtr)
    return UNASSIGNED_HIGHEST_TAG;

  PriorityAtr->MidPriority = UNASSIGNED_HIGHEST_TAG;

  switch (PriorityAtr->Tag) {
    case BoTagSata:
    //case BoTagsSata://<Yaosr001-20181207 Import the OemBds module to support AMD Platform>
      if (Option->BbsEntry != NULL)
      {
          BbsTable = Option->BbsEntry;
          Bus = BbsTable->Bus;
          Device = BbsTable->Device;
          Function = BbsTable->Function;
          DeviceHandle = (EFI_HANDLE)*(UINTN*)&BbsTable->IBV1;

          Status = pBS->HandleProtocol( DeviceHandle, &DiskInfoGuid, &DiskInfo );
          if (EFI_ERROR(Status))
            break;

          Dev = (SATA_DEVICE_INTERFACE *)((SATA_DISK_INFO*)DiskInfo)->SataDevInterface;
          PortNumber = ((SATA_DEVICE_INTERFACE *)Dev)->PortNumber;
          //PriorityAtr->MidPriority = FindSataPortPriority(PriorityAtr->Tag,PortNumber);
          PriorityAtr->MidPriority = FindSataPortPriority(Bus, PortNumber); //<Yaosr001-20181207 Import the OemBds module to support AMD Platform>
      }
      break;
    case BoTagSAS:
      if (Option->BbsEntry != NULL)
      {
        BbsTable = Option->BbsEntry;
        PriorityAtr->MidPriority = (BbsTable->Bus << 16);
        Ptr = Option->Description;
      }
      break;
    case BoTagNvme:
      break;
    //<hanzhr001-20181107-Add RAID to Change Boot Option+>
    case BoTagRAID:
      break;
    //<hanzhr001-20181107-Add RAID to Change Boot Option+>
    default:
      break;
  }

  return PriorityAtr->MidPriority;
}

UINT32 GetOptionLowPriority(BOOT_OPTION *Option,PRIORITY_ATTRIBUTE *PriorityAtr)
{

  if(NULL == PriorityAtr)
    return UNASSIGNED_HIGHEST_TAG;

  PriorityAtr->LowPriority = UNASSIGNED_HIGHEST_TAG;

  return PriorityAtr->LowPriority;
}

//<Wangyia001-20190921 High speed PXE first for UEFI and Legacy mode +>
#if defined(HIGH_SPEED_PXE_FIRST) && (HIGH_SPEED_PXE_FIRST == 1)
#if defined(PINGAN_PROJECT_SUPPORT) && (PINGAN_PROJECT_SUPPORT == 1)
UINT8 FindUefiNicSpeed(BOOT_OPTION *Option)
{
  EFI_HANDLE                Handle;
  UINT8                     IpType;
  BOOLEAN                   HttpBoot;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_STATUS                Status;
  EFI_PCI_IO_PROTOCOL       *PciIO;
  EFI_HANDLE                PciIoHandle;  
  UINTN                     SegNum = 0;
  UINTN                     BusNum = 0;
  UINTN                     DevNum = 0;
  UINTN                     FunNum= 0;
  UINT16                    VendorID;
  UINT16                    DeviceID;
  UINT16                    SubSystemId;
  PCI_DEV_NINFO             *ptrdninfo;
  UINT8                     dfound=0;

  if (Option->DeviceHandle == INVALID_HANDLE) {
    return 0;
  }
  //Name from Controller Handle
  Handle = GetPhysicalBlockIoHandle(Option->DeviceHandle);
  if (Handle == Option->DeviceHandle){
    EFI_HANDLE OldHandle = Handle;
    Handle = GetPhysicalNetworkCardHandle(Option->DeviceHandle, &IpType, &HttpBoot);
    if (Handle != OldHandle) {
      Status = pBS->HandleProtocol(Option->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&DevicePath);
      if (EFI_ERROR(Status)) return 0;
      Status = pBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevicePath, &PciIoHandle);
      if (EFI_ERROR(Status)) return 0;
      Status = pBS->HandleProtocol (PciIoHandle, &gEfiPciIoProtocolGuid, (VOID **) &PciIO);
      if (EFI_ERROR(Status)) return 0;        
      Status = PciIO->GetLocation(PciIO, &SegNum, &BusNum, &DevNum, &FunNum);  
      if (EFI_ERROR(Status)) return 0;
      VendorID = PciRead16(PCI_LIB_ADDRESS(BusNum,DevNum,FunNum,PCI_VID));
      DeviceID = PciRead16(PCI_LIB_ADDRESS(BusNum,DevNum,FunNum,PCI_DID));
      SubSystemId = PciRead16(PCI_LIB_ADDRESS(BusNum,DevNum,FunNum,PCI_SID));
      ptrdninfo = GetDevBaseInfo(VendorID, DeviceID, SubSystemId, &dfound);
      if (dfound) {
        if (ptrdninfo->Speed) {
          return ptrdninfo->Speed;
        }
      }
    }
  }
  
  return 0;
}

VOID CompareOemTag(BOOT_OPTION *Option1,BOOT_OPTION *Option2)
{
  UINT8                         NicSpeed1 = 0;
  UINT8                         NicSpeed2 = 0;
  UINT32                        TempTag = 0;

  NicSpeed1 = FindUefiNicSpeed(Option1);
  NicSpeed2 = FindUefiNicSpeed(Option2);  

  if ( (NicSpeed1 != 0) && (NicSpeed2 != 0) && (NicSpeed1 < NicSpeed2) && (Option1->Tag < Option2->Tag)) { //Both two NICs' speed is not undefined
    TempTag = Option1->Tag;
    Option1->Tag = Option2->Tag;
    Option2->Tag = TempTag;
  } 
  
  return;
}
#endif
#endif
//<Wangyia001-20190921 High speed PXE first for UEFI and Legacy mode ->

INT32 CompareOemPriority(BOOT_OPTION *Option1,BOOT_OPTION *Option2)
{
  PRIORITY_ATTRIBUTE Op1,Op2;

  //first/cmos clear/add new device to compare
  if (Option1->Priority != Option2->Priority)
    return 0;

  GetOptionHighPriority(Option1,&Op1);
  GetOptionMidPriority(Option1,&Op1);
  GetOptionLowPriority(Option1,&Op1);
  //TRACE((-1,"OP1:T:%x,H:%x,M:%x ,%S\n",Option1->Tag,Op1.HighPriority,Op1.MidPriority,Option1->Description));

  GetOptionHighPriority(Option2,&Op2);
  GetOptionMidPriority(Option2,&Op2);
  GetOptionLowPriority(Option2,&Op2);
  //TRACE((-1,"OP2:T:%x,H:%x,M:%x ,%S\n",Option2->Tag,Op2.HighPriority,Op2.MidPriority,Option2->Description));

  if (Op1.HighPriority < Op2.HighPriority) return -1;
  else if (Op1.HighPriority > Op2.HighPriority) return 1;

  if (Op1.MidPriority < Op2.MidPriority) return -1;
  else if (Op1.MidPriority > Op2.MidPriority) return 1;

  if (Op1.LowPriority < Op2.LowPriority) return -1;
  else if (Op1.LowPriority > Op2.LowPriority) return 1;

  return 0;
}

/**
 *  Compare the two boot options passed and determine the priority of
 *  the first parameter in relation to the second parameter
 *  Try to compare based on the following parameters in order,
 *  Tag, then Group Header, then Priority
 *
 *  @param Link1 Pointer to boot option 1
 *  @param Link2 Pointer to boot option 2
 *
 *  @retval INT32 the comparison result Less than zero - Boot option 1 is lower priority than boot option 2
 *  @retval zero boot option 1 is same priority as boot option 2 greater than zero - boot option 1 is a higher priority than boot option 2
 */
INT32 OemCompareTagThenPriority(DLINK *Link1, DLINK *Link2){
	BOOT_OPTION *Option1 = (BOOT_OPTION*)Link1;
	BOOT_OPTION *Option2 = (BOOT_OPTION*)Link2;
	INT32       Result=0;
	
    //<Wangyia001-20190921 High speed PXE first for UEFI and Legacy mode +>
#if defined(HIGH_SPEED_PXE_FIRST) && (HIGH_SPEED_PXE_FIRST == 1)
#if defined(PINGAN_PROJECT_SUPPORT) && (PINGAN_PROJECT_SUPPORT == 1)
    CompareOemTag(Option1,Option2);
#endif
#endif
    //<Wangyia001-20190921 High speed PXE first for UEFI and Legacy mode ->
	
	if (Option1->Tag < Option2->Tag) return -1;
	else if (Option1->Tag > Option2->Tag) return 1;
	if (Option1->GroupHeader != Option2->GroupHeader)
		return (Option1->GroupHeader) ? -1 : 1;

	//DUMP_BOOT_OPTION_LIST(BootOptionList,"Before CompareOemPriority:");
	Result = CompareOemPriority(Option1,Option2);
	if(Result !=0)
		return Result;

	if (Option1->Priority < Option2->Priority) return -1;
	else if (Option1->Priority > Option2->Priority) return 1;
	return 0;
}

/**
 * Go through the master boot option list and apply orphan boot option policy to the boot option groups
 */
VOID OemPostProcessBootOptions(){
    DLINK *Link;
    BOOT_OPTION *Option;

    SortList(BootOptionList, OemCompareTagThenPriority);
    //Now we are ready for the processing of orphan group headers.
    //process empty groups ( groups with just the header)
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
        if (!Option->GroupHeader) continue;
        if (   Link==NULL
            || OUTTER(Link,Link,BOOT_OPTION)->Tag != Option->Tag
        ) ApplyOrphanBootOptionPolicy(
            BootOptionList, Option, ORPHAN_GROUP_HEADERS_POLICY
          );
    }

#if GROUP_BOOT_OPTIONS_BY_TAG
    CreateGroupHeaders(BootOptionList);
#endif
}

/**
 * Go through the boot option list and set the priorities of each group of devices
 *
 * @param BootOptionList the master list of boot options
 */
VOID OemSetBootOptionPriorities(){
	DLINK *Link;
#if GROUP_BOOT_OPTIONS_BY_TAG
    UINT16 PreviousBootOptionNumber = INVALID_BOOT_OPTION_NUMBER;
    UINT32 PreviousTag = UNASSIGNED_HIGHEST_TAG;
#endif
    UINT32 PreviousPriority=0;
    BOOT_OPTION *Option;
    UINT16 NextOptionNumber;

    //Detect first unused boot option number
    NextOptionNumber = 0;
    if (!DListEmpty(BootOptionList)){
        FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
            if (   Option->BootOptionNumber != INVALID_BOOT_OPTION_NUMBER
                && Option->BootOptionNumber > NextOptionNumber
            ) NextOptionNumber = Option->BootOptionNumber;
        }
        NextOptionNumber++;
    }

	SortList(BootOptionList, OemCompareTagThenPriority);
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option){
#if GROUP_BOOT_OPTIONS_BY_TAG
        //<xuyj1001-20200331 Fixed the issue of there is only one UEFI boot option number for the same type of devices +>
        if(IsLegacyBootOption(Option)){
        if (Option->BootOptionNumber==INVALID_BOOT_OPTION_NUMBER){
            if (   PreviousTag != Option->Tag
                || Option->Tag==UNASSIGNED_LOWEST_TAG
                || Option->Tag==UNASSIGNED_HIGHEST_TAG
            ) PreviousBootOptionNumber=NextOptionNumber++;
            Option->BootOptionNumber=PreviousBootOptionNumber;
            Option->Priority=++PreviousPriority;
        }else{
            PreviousBootOptionNumber = Option->BootOptionNumber;
            PreviousPriority = Option->Priority;
        }
        PreviousTag = Option->Tag;
        }
        else{
            if (Option->BootOptionNumber==INVALID_BOOT_OPTION_NUMBER){
                Option->BootOptionNumber=(NextOptionNumber)++;
                Option->Priority=++PreviousPriority;
            }else{
                PreviousPriority = Option->Priority;
            }
        }
        //<xuyj1001-20200331 Fixed the issue of there is only one UEFI boot option number for the same type of devices ->

#else
        if (Option->BootOptionNumber==INVALID_BOOT_OPTION_NUMBER){
            Option->BootOptionNumber=(NextOptionNumber)++;
            Option->Priority=++PreviousPriority;
        }else{
            PreviousPriority = Option->Priority;
        }
#endif
	}
    DUMP_BOOT_OPTION_LIST(BootOptionList,"After Setting Priorities");
}
//<Kangmm-20200811-Fix mantis 0042028 Harddisk default boot order doesn't not follow BBS Priorities +>
/**
    This function sets variables: DefaultLegacyDevOrder and DefaultBootOrder.
    
    This function takes the current boot order and then it creates two NVRAM 
    variables, DefaultLegacyDevOrder and DefaultBootOrder, by sorting the order 
    based on rules defined at build time. The function then calls SetVariable 
    for each of those variables with attributes of BOOT_VARIABLE_ATTRIBUTES.
    Once created, these variables are available for the boot process to use if
    the user chooses to load the default boot order.

    @return VOID
    
    @note In the case where CSM is disabled or not present, the code to set variable
    DefaultLegacyDevOrder is disabled.
 */
VOID OemSetDefaultTseBootOrder(VOID)
{
    #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    UINTN DevOrderSize;
    LEGACY_DEVICE_ORDER *DevOrder;
    EFI_GUID DefaultLegacyDevOrderGuid = DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID;
    LEGACY_DEVICE_ORDER *Order;
    UINTN  i;
    #endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    UINTN BootOrderSize;
    UINT16 *BootOrder;
    UINT16 PreviousBootOptionNumber;
    DLINK *Link;
    UINTN BootOrderIndex;
    BOOT_OPTION *Option;

    EFI_GUID DefaultBootOrderGuid = DEFAULT_BOOT_ORDER_VARIABLE_GUID;


    // PRECONDITION: All Boot Option Numbers are set
    BootOrderSize = BootOptionList->Size * sizeof(UINT16);
    if (BootOrderSize == 0) 
        return;
    
    SortList(BootOptionList, OemCompareTagThenPriority);
    #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    // Build DefaultLegacyDevOrder variable
    BuildLegacyDevOrderBuffer(&DevOrder, &DevOrderSize);

    // Remove disable mask from buffer DevOrder
    for( Order = DevOrder; 
         (UINT8*)Order < (UINT8*)DevOrder + DevOrderSize; 
         Order = (LEGACY_DEVICE_ORDER*)((UINT8*)Order + Order->Length + sizeof(Order->Type)) )
        for( i = 0; i < (Order->Length / sizeof(UINT16) - 1); i++ )
            Order->Device[i] &= 0x00FF;

    // Store "DefaultLegacyDevOrder" EFI variable
    pRS->SetVariable(
        L"DefaultLegacyDevOrder", 
        &DefaultLegacyDevOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES,
        DevOrderSize,
        DevOrder
    );
    pBS->FreePool(DevOrder);
    #endif  // #if defined(CSM_SUPPORT) && CSM_SUPPORT == 1
    // Build DefaultBootOrder variable
    BootOrder = Malloc(BootOrderSize);
    PreviousBootOptionNumber = INVALID_BOOT_OPTION_NUMBER;
    BootOrderIndex = 0;

    // Set boot option number for each boot option
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        if ( (PreviousBootOptionNumber == Option->BootOptionNumber) ||
             (Option->Attributes & LOAD_OPTION_HIDDEN) )
            continue;
        BootOrder[BootOrderIndex++] = Option->BootOptionNumber;
        PreviousBootOptionNumber = Option->BootOptionNumber;
    }
    
    // Put hidden boot option with lowest priority.
    FOR_EACH_BOOT_OPTION(BootOptionList,Link,Option)
    {
        if ( (PreviousBootOptionNumber == Option->BootOptionNumber) ||
             !(Option->Attributes & LOAD_OPTION_HIDDEN) )
            continue;
        BootOrder[BootOrderIndex++] = Option->BootOptionNumber;
        PreviousBootOptionNumber = Option->BootOptionNumber;        
    }
    
    // Store "DefaultBootOrder" EFI variable
    pRS->SetVariable(
        L"DefaultBootOrder", 
        &DefaultBootOrderGuid,
        BOOT_VARIABLE_ATTRIBUTES, 
        BootOrderIndex * sizeof(UINT16), 
        BootOrder
    );
    pBS->FreePool(BootOrder);
}
//<Kangmm-20200811-Fix mantis 0042028 Harddisk default boot order doesn't not follow BBS Priorities ->
