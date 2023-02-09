//**********************************************************************
// $Header:$
//
// $Revision:$
//
// $Date:$
//**********************************************************************
// Revision History
// ----------------
// $Log:$
// 
//**********************************************************************
//<Oem_FHDR_START>
//
// Name: OemStorageInfoProtocol.c
//
// Description:  Collect storage information, and stored these information into
//               gOemStorageInfoProtocolGuid
//
//<Oem_FHDR_END>
//**********************************************************************

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include <Efi.h>
#include <token.h>
#include <AmiDxeLib.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h> //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. >

#include <Protocol/AmiAhciBus.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/OemStorageInfoProtocol.h>
#include <PciBus.h>
#include "Nvme/NvmeBus.h"
#include "PCIEXPRESS.h"

#if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18 == 1)
#include <SataControllerInfoLib.h> //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
#endif

OEM_STORAGE_INFO    *mOemStorageInfo = NULL;

EFI_STATUS InitStorageInfo (
    IN OUT  OEM_STORAGE_INFO *StorageInfo
    );

EFI_STATUS AhciDriveInfoConstruction(
    IN  EFI_DISK_INFO_PROTOCOL *DiskInfo,
    OUT DRIVE_INFO *DriveInfo
    );
EFI_STATUS NvmeDriveInfoConstruction(
    IN  EFI_DISK_INFO_PROTOCOL *DiskInfo,
    OUT DRIVE_INFO *DriveInfo
    );

EFI_STATUS AppendStorageInfoList (
    IN OUT  OEM_STORAGE_INFO *StorageInfo,
    IN      DRIVE_INFO *DriveInfo
    );

EFI_STATUS GetOemStorageInfo(
  IN      OEM_STORAGE_INFO_PROTOCOL   *This,
  IN OUT  OEM_STORAGE_INFO  **OemStorageInfo
);

OEM_STORAGE_INFO_PROTOCOL mOemStorageInfoProtocol = {
    GetOemStorageInfo,
};

VOID
TrimSpace (
  IN  CHAR8  *Data,
  IN  UINT16 Size
  )
/*++
Routine Description:
  Trim space from CHAR8 data header and tail.

Arguments:
  Data  -  The data to swap.
  Size  -  Size of data to swap.

Returns:
  None.
--*/
{
  UINT16  Index;
  CHAR8   *TempBuffer = NULL;
  CHAR8   *TempBufferPtr = NULL;

  if (Data == NULL)
    return;

  //Remove the SPACE CHARACTER from the string header
  if (Data[0] == ' ') {
    TempBufferPtr = TempBuffer = AllocateZeroPool(Size);
    CopyMem(TempBuffer, Data, Size);
    Index = 0;
    while ((*TempBuffer == ' ') && (Index<Size)){
      Index++;
      TempBuffer++;
    }
    ZeroMem(Data, Size);
    CopyMem(Data, TempBuffer, Size-Index);

    if (TempBuffer != NULL) {
      FreePool(TempBufferPtr);
      TempBufferPtr = TempBuffer = NULL;
    }
  }

  //Remove the SPACE CHARACHER from the string tail end
  for (Index = Size; Index > 0; Index--) {
    if ( (Data[Index] != ' ')&&(Data[Index] != '\0') )
      break;

    if (Data[Index] == ' ')
      Data[Index] = '\0';
  }

  return ;
}

VOID
SwapEntries (
  IN  CHAR8  *Data,
  IN  UINT16 Size
  )
/*++
Routine Description:
  Swap order of nearby CHAR8 data.

Arguments:
  Data  -  The data to swap.
  Size  -  Size of data to swap.

Returns:
  None.
--*/
{
  UINT16  Index;
  CHAR8   Temp8;

  if ((Data == NULL) || (Size < 2))
    return;

  for (Index = 0; (Index + 1) < Size; Index += 2) {
    Temp8           = Data[Index];
    Data[Index]     = Data[Index + 1];
    Data[Index + 1] = Temp8;
  }

  return ;
}


/**
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param[in] pPciIo               EFI_PCI_IO_PROTOCOL
  @param[in] CapId                CAPID to search for

  @retval 0                       CAPID not found
  @retval Other                   CAPID found, Offset of desired CAPID
**/
UINT8
PcieFindCapId (
  IN EFI_PCI_IO_PROTOCOL        *pPciIo,
  IN UINT8                      CapId
  )
{
  UINT8 CapHeaderOffset;
  UINT8 CapHeaderId;
  UINT8 Data8;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  

  PciIo = pPciIo;
  //DEBUG((DEBUG_INFO,"PcieFindCapId () BDF %0x: %0x :%0x, CapId = %0x \n", Bus, Device, Function, CapId));
  PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x06,1,&Data8);
  if ((Data8 & 0x10) == 0x00) {
    ///
    /// Function has no capability pointer
    ///
    return 0;
  } else {
    ///
    /// Check the header layout to determine the Offset of Capabilities Pointer Register
    ///
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,0x0E,1,&Data8);
    if ((Data8 & 0x7F) == 0x02) {
      ///
      /// If CardBus bridge, start at Offset 0x14
      ///
      CapHeaderOffset = 0x14;
    } else {
      ///
      /// Otherwise, start at Offset 0x34
      ///
      CapHeaderOffset = 0x34;
    }
    ///
    /// Get Capability Header, A pointer value of 00h is used to indicate the last capability in the list.
    ///
    CapHeaderId     = 0;
    PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,CapHeaderOffset,1,&Data8);
    CapHeaderOffset = Data8 & ((UINT8) ~(BIT0 | BIT1));
    while (CapHeaderOffset != 0 && CapHeaderId != 0xFF) {
      PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,CapHeaderOffset,1,&CapHeaderId);
      if (CapHeaderId == CapId) {
        if (CapHeaderOffset > 0x3F) {
          ///
          /// Return valid capability offset
          ///
          return CapHeaderOffset;
        } else {
          ASSERT((FALSE));
          return 0;
        }
      }
      ///
      /// Each capability must be DWORD aligned.
      /// The bottom two bits of all pointers (including the initial pointer at 34h) are reserved
      /// and must be implemented as 00b although software must mask them to allow for future uses of these bits.
      ///
      PciIo->Pci.Read(PciIo,EfiPciIoWidthUint8,CapHeaderOffset+1,1,&Data8);
      CapHeaderOffset = Data8 & ((UINT8) ~(BIT0 | BIT1));
    }
    return 0;
  }
}


EFI_STATUS AhciDriveInfoConstruction(
    IN  EFI_DISK_INFO_PROTOCOL *DiskInfo,
    OUT DRIVE_INFO *DriveInfo
    )
{
  EFI_STATUS                Status = EFI_OUT_OF_RESOURCES;
  UINTN                     SegNum = 0;
  UINTN                     BusNum = 0;
  UINTN                     DevNum = 0;
  UINTN                     FunNum= 0;
  PCI_DEVICE_INDEPENDENT_REGION    PciHeader;
  UINT32                    BufferSize;
  UINT64                    DriveSize;
  EFI_IDENTIFY_DATA         *IdentifyDriveInfo = NULL;
  SATA_DEVICE_INTERFACE     *SataDevInterface = NULL;
  EFI_PCI_IO_PROTOCOL       *PciIO;
  UINT8                     Index;
  UINT32                    SectorSize;
  #if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18ER_SUPPORT == 1)
  PCI_DEV_INFO              *Prt; //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
  UINTN                     SataControllerIndex; //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
  #endif

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));

  if (DiskInfo == NULL)
    return Status;
  if (DriveInfo == NULL)
    return Status;

  ZeroMem (&PciHeader, sizeof(PCI_DEVICE_INDEPENDENT_REGION));

  SataDevInterface = ((SATA_DISK_INFO *)DiskInfo)->SataDevInterface;
  //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. +>
  if (SataDevInterface == NULL)
    return Status;
  //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. ->
  PciIO = SataDevInterface->AhciBusInterface->PciIO;
  //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. +>
  if (PciIO == NULL)
    return Status;
  //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. ->
  #if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18 == 1)
  //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
  GetSataControllerIndex(PciIO, &Prt, &SataControllerIndex);
  DriveInfo->RootBus = Prt->Address.Addr.Bus;
  DriveInfo->RootDevice = Prt->Address.Addr.Device;
  DriveInfo->RootFunction = Prt->Address.Addr.Function;
  DriveInfo->HDDControllerIndex = (UINT8)SataControllerIndex;
  //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  ->
  #endif

  Status = PciIO->GetLocation(
      PciIO,
      &SegNum, &BusNum, &DevNum, &FunNum);

  Status = PciIO->Pci.Read(
      PciIO,
      EfiPciIoWidthUint8,
      0,
      sizeof (PCI_DEVICE_INDEPENDENT_REGION),
      &PciHeader
      );

  BufferSize = sizeof(EFI_IDENTIFY_DATA);
  IdentifyDriveInfo = AllocateZeroPool (BufferSize);

  if (IdentifyDriveInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "IdentifyDriveInfo: EFI_OUT_OF_RESOURCES\n"));
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  Status = DiskInfo->Identify (
      DiskInfo,
      IdentifyDriveInfo,
      &BufferSize
      );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  DriveInfo->PciIO  = PciIO;
  DriveInfo->HddSlot = SataDevInterface->PortNumber;
  DriveInfo->SegNum = (UINT8)SegNum;
  DriveInfo->BusNum = (UINT8)BusNum;
  DriveInfo->DevNum = (UINT8)DevNum;
  DriveInfo->FunNum = (UINT8)FunNum;
  CopyMem(&DriveInfo->PciHeader, &PciHeader, sizeof (PCI_DEVICE_INDEPENDENT_REGION));

  switch (PciHeader.ClassCode[1]) {
  case PCI_CLASS_MASS_STORAGE_SATADPA/*AMDSB_CLASS_CODE_AHCI*/:
    DriveInfo->HddMode = TypeAhci;
    break;
  case PCI_CLASS_MASS_STORAGE_RAID/*AMDSB_CLASS_CODE_RAID*/:
    DriveInfo->HddMode = TypeRaid;
    break;
  case PCI_CLASS_MASS_STORAGE_IDE:
    DriveInfo->HddMode = TypeIde;
    break;
  default:
    DriveInfo->HddMode = TypeOthers;
    break;
  }

  DEBUG((EFI_D_ERROR, "Seg:%d Bus:%d Dev:%d Fun:%d Slot:%d HddMode:%d ", SegNum, BusNum, DevNum, FunNum, DriveInfo->HddSlot, DriveInfo->HddMode));

  CopyMem(DriveInfo->HddSN, IdentifyDriveInfo->AtaData.SerialNo, OEM_IPMI_HDD_SN_LENGTH);
  SwapEntries ((CHAR8 *) DriveInfo->HddSN, OEM_IPMI_HDD_SN_LENGTH);
  TrimSpace ((CHAR8 *) DriveInfo->HddSN, OEM_IPMI_HDD_SN_LENGTH);
  DEBUG((EFI_D_ERROR, "SN:%a ", DriveInfo->HddSN));

  CopyMem(DriveInfo->HddFirmwareVersion, IdentifyDriveInfo->AtaData.FirmwareVer, OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH);
  SwapEntries ((CHAR8 *) DriveInfo->HddFirmwareVersion, OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH);
  TrimSpace ((CHAR8 *) DriveInfo->HddFirmwareVersion, OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH);
  DEBUG((EFI_D_ERROR, "FW:%a ", DriveInfo->HddFirmwareVersion));

  CopyMem(DriveInfo->HddModelName, IdentifyDriveInfo->AtaData.ModelName, OEM_IPMI_HDD_MODEL_NAME_LENGTH);
  SwapEntries ((CHAR8 *) DriveInfo->HddModelName, OEM_IPMI_HDD_MODEL_NAME_LENGTH);
  TrimSpace ((CHAR8 *) DriveInfo->HddModelName, OEM_IPMI_HDD_MODEL_NAME_LENGTH);
  DEBUG((EFI_D_ERROR, "Model:%a ", DriveInfo->HddModelName));

  // APTIOV_SERVER_OVERRIDE_RC_START : Changes to support 4K Sector Size.
  // Update sector size if necessary
  if ( (IdentifyDriveInfo->AtaData.phy_logic_sector_support & 0x4000) && (!(IdentifyDriveInfo->AtaData.phy_logic_sector_support & 0x8000)) && (IdentifyDriveInfo->AtaData.phy_logic_sector_support & 0x1000) ) {
      SectorSize = (UINT32)(IdentifyDriveInfo->AtaData.logic_sector_size_lo + (IdentifyDriveInfo->AtaData.logic_sector_size_hi << 16)) * 2;
  }
  else {
      SectorSize = 512;
  }
  //
  // For HardDisk append the size. Otherwise display atapi
  //
  if ((IdentifyDriveInfo->AtaData.config & 0x8000) == 00) {
    //
    // 48 bit address feature set is supported, get maximum capacity
    //
    if ((IdentifyDriveInfo->AtaData.command_set_supported_83 & 0x0400) == 0) {
      DriveSize =
        (
          (
            (
              (
                (IdentifyDriveInfo->AtaData.user_addressable_sectors_hi << 16) +
                IdentifyDriveInfo->AtaData.user_addressable_sectors_lo
              ) / 1000
            // APTIOV_SERVER_OVERRIDE_RC_START : Changes to support 4K Sector Size.
            ) * SectorSize
            // APTIOV_SERVER_OVERRIDE_RC_END : Changes to support 4K Sector Size.
          ) /
          1000
        );
    } else {
      DriveSize = IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing[0];
      for (Index = 1; Index < 4; Index++) {
        DriveSize |= LShiftU64 (
                      IdentifyDriveInfo->AtaData.maximum_lba_for_48bit_addressing[Index],
                      16 * Index
                      );
      }
      // APTIOV_SERVER_OVERRIDE_RC_START : Changes to support 4K Sector Size.
      DriveSize = (UINT32) DivU64x64Remainder (MultU64x32 (DivU64x64Remainder (DriveSize, 1000, 0), SectorSize), 1000, 0);
      // APTIOV_SERVER_OVERRIDE_RC_END : Changes to support 4K Sector Size.
    }

  }else{
    DEBUG((EFI_D_ERROR, "ATAPI device.\n"));
    return EFI_UNSUPPORTED;
  }

  DriveInfo->HddSize = DriveSize;
  DEBUG((EFI_D_ERROR, "DriveSize: %dGB\n", (DriveInfo->HddSize/1000)));

  DEBUG((EFI_D_ERROR, __FUNCTION__" End\n"));
  return Status;
}

EFI_STATUS NvmeDriveInfoConstruction(
    IN  EFI_DISK_INFO_PROTOCOL *DiskInfo,
    OUT DRIVE_INFO *DriveInfo
    )
{
  EFI_STATUS          Status = EFI_OUT_OF_RESOURCES;
  ACTIVE_NAMESPACE_DATA             *ActiveNameSpace;
  PCI_DEVICE_INDEPENDENT_REGION     PciHeader;
  EFI_PCI_IO_PROTOCOL               *PciIO;
  PCI_DEV_INFO                      *Prt;
  UINTN                             SegNum = 0;
  UINTN                             BusNum = 0;
  UINTN                             DevNum = 0;
  UINTN                             FunNum = 0;
  UINT64                            DriveSize;
  UINT32                            Data32;
  UINT8                             CapIdOffset;
  UINT8                             SlotNum =0;
  PCIE_SLOT_CAP                     SlotData;
  

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));
  if (DiskInfo == NULL)
    return Status;
  if (DriveInfo == NULL)
    return Status;

  ActiveNameSpace = BASE_CR(DiskInfo, ACTIVE_NAMESPACE_DATA, NvmeDiskInfo);
  
  if(ActiveNameSpace->Signature == ACTIVE_NAME_SPACE_SIG){
    
    PciIO = ActiveNameSpace->NvmeController->PciIO;
    
    //rootport
    Prt = (PCI_DEV_INFO *)PciIO;
    if(Prt->Signature == AMI_PCI_SIG){
      
      while(Prt->ParentBrg != NULL && Prt->Type!=tPciRootBrg) {
        
        //Get root port brg
        if((Prt->Type == tPci2PciBrg)&&(Prt->ParentBrg->Type == tPciRootBrg))
          break;
        
        Prt= Prt->ParentBrg;
      }
      #if defined(ADD_HARD_DISK_FOR_IPMICMDV18) && (ADD_HARD_DISK_FOR_IPMICMDV18 == 1)
      //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  +>
      DriveInfo->RootBus = Prt->Address.Addr.Bus;
      DriveInfo->RootDevice = Prt->Address.Addr.Device;
      DriveInfo->RootFunction = Prt->Address.Addr.Function;
      DEBUG((EFI_D_ERROR, "AIC Root bus dev func %x-%x-%x\n", DriveInfo->RootBus, DriveInfo->RootDevice, DriveInfo->RootFunction));
      //<hujf1001-20210107 Add more Hard Disk asset info for OemOEMIPMICmdV1.8  ->
      #endif
      CapIdOffset = PcieFindCapId(&Prt->PciIo,0x10);
      
      if(CapIdOffset !=0){

        Prt->RbIo->Pci.Read (Prt->RbIo,
                    EfiPciIoWidthUint32,
                    (Prt->Address.ADDR + CapIdOffset + PCIE_SLOT_CAPABILITY_OFFSET),
                    1,
                    &SlotData.Dword
                    );
        Data32 = SlotData.Bits.PhysicalSlotNum;
        if(Data32) SlotNum = Data32; 
        DEBUG((EFI_D_ERROR, "NVME SlotNum:%d \n", SlotNum));
      }
    }

    Status = PciIO->GetLocation(
        PciIO,
        &SegNum, &BusNum, &DevNum, &FunNum);

    Status = PciIO->Pci.Read(
        PciIO,
        EfiPciIoWidthUint8,
        0,
        sizeof (PCI_DEVICE_INDEPENDENT_REGION),
        &PciHeader
        );

    Status = PciIO->Pci.Read(
        PciIO,
        EfiPciIoWidthUint8,
        0,
        sizeof (PCI_DEVICE_INDEPENDENT_REGION),
        &PciHeader
        );

    DriveInfo->PciIO  = PciIO;
    DriveInfo->HddSlot = SlotNum;
    DriveInfo->SegNum = (UINT8)SegNum;
    DriveInfo->BusNum = (UINT8)BusNum;
    DriveInfo->DevNum = (UINT8)DevNum;
    DriveInfo->FunNum = (UINT8)FunNum;
    CopyMem(&DriveInfo->PciHeader, &PciHeader, sizeof (PCI_DEVICE_INDEPENDENT_REGION));
    DriveInfo->HddMode = TypeNvme;
    //Prepare data
    DEBUG((EFI_D_ERROR, "Seg:%d Bus:%d Dev:%d Fun:%d Slot:%d HddMode:%d ", SegNum, BusNum, DevNum, FunNum, DriveInfo->HddSlot, DriveInfo->HddMode));
    
    CopyMem(DriveInfo->HddSN, &(ActiveNameSpace->NvmeController->IdentifyControllerData->SerialNumber), OEM_IPMI_HDD_SN_LENGTH);
    TrimSpace ((CHAR8 *) DriveInfo->HddSN, OEM_IPMI_HDD_SN_LENGTH);
    DEBUG((EFI_D_ERROR, "SN:%a ", DriveInfo->HddSN));
    
    CopyMem(DriveInfo->HddFirmwareVersion, &(ActiveNameSpace->NvmeController->IdentifyControllerData->FirmwareRevision), OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH);
    TrimSpace ((CHAR8 *) DriveInfo->HddFirmwareVersion, OEM_IPMI_HDD_FIRMWARE_VERSION_LENGTH);
    DEBUG((EFI_D_ERROR, "FW:%a ", DriveInfo->HddFirmwareVersion));
    
    CopyMem(DriveInfo->HddModelName, &(ActiveNameSpace->NvmeController->IdentifyControllerData->ModelNumber), OEM_IPMI_HDD_MODEL_NAME_LENGTH);
    TrimSpace ((CHAR8 *) DriveInfo->HddModelName, OEM_IPMI_HDD_MODEL_NAME_LENGTH);
    DEBUG((EFI_D_ERROR, "Model:%a ", DriveInfo->HddModelName));
    
    //<LiuYB036-20180118 Remove NVMe Tenth of GB info. to sync with BMC web asset info.+>
    //DriveSize = DivU64x64Remainder (MultU64x32 (ActiveNameSpace->dMaxLBA,ActiveNameSpace->NvmeBlockIO.Media->BlockSize), 1000000, 0);//MB
    DriveSize = DivU64x64Remainder (MultU64x32 (ActiveNameSpace->IdentifyNamespaceData.NSIZE, ActiveNameSpace->NvmeBlockIO.Media->BlockSize), 1000000, 0);//MB
    //<LiuYB036-20180118 Remove NVMe Tenth of GB info. to sync with BMC web asset info.->

    DriveInfo->HddSize = DriveSize;
    DEBUG((EFI_D_ERROR, "DriveSize: %dGB\n", (DriveInfo->HddSize/1000)));
  
  }else{
    DEBUG((EFI_D_ERROR, "Nvme Signature error!\n"));
  }
  
  
  
  DEBUG((EFI_D_ERROR, __FUNCTION__" End\n"));

  return Status;
}

EFI_STATUS AppendStorageInfoList (
    IN OUT  OEM_STORAGE_INFO *StorageInfo,
    IN      DRIVE_INFO *DriveInfo
    )
{
  EFI_STATUS          Status = EFI_OUT_OF_RESOURCES;
  DRIVE_INFO          *PtrDriveInfo = NULL;
  DRIVE_INFO          *PtrNewDrive  = NULL;
  UINTN               StorageNumber;

  if (StorageInfo == NULL)
    return Status;
  if (DriveInfo == NULL)
    return Status;

  StorageNumber = StorageInfo->ActualNumber;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start Number:%d\n", StorageNumber));

  PtrNewDrive =  AllocateZeroPool(sizeof(DRIVE_INFO));
  if(PtrNewDrive == NULL)
    return Status;
  CopyMem(PtrNewDrive, DriveInfo, sizeof(DRIVE_INFO));

  Status=AppendItemLst((T_ITEM_LIST*)&StorageInfo->InitCounter,PtrNewDrive);

  DEBUG((EFI_D_ERROR, __FUNCTION__" End Number:%d\n", StorageInfo->ActualNumber));
  return Status;
}

EFI_STATUS InitStorageInfo (
    IN OUT  OEM_STORAGE_INFO *StorageInfo
    )
{
  EFI_STATUS          Status = EFI_OUT_OF_RESOURCES;

  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  UINTN                           Index;
  DRIVE_INFO                      HddInfo;
  //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. +>
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath = NULL;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePathNode = NULL;
  PCI_DEVICE_PATH                 *PciDevicePath = NULL;
  //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. ->
  //Mizl-20201226-Add ASMedia AHCI Controller In Support List+>
  EFI_PCI_IO_PROTOCOL             *PciIO = NULL;
  SATA_DEVICE_INTERFACE           *SataDevInterface = NULL;
  UINT16                          AsmediaVendID;
  //Mizl-20201226-Add ASMedia AHCI Controller In Support List->
  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));

  if (StorageInfo == NULL)
    return Status;

  Status = gBS->LocateHandleBuffer (
    ByProtocol,
    &gEfiDiskInfoProtocolGuid,
    NULL,
    &HandleCount,
    &HandleBuffer
    );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Get disk information protocol status: %r\n", Status));
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = pBS->HandleProtocol(
        HandleBuffer[Index],
        &gEfiDiskInfoProtocolGuid,
        &DiskInfo
        );
    if (!EFI_ERROR(Status)) {
      ZeroMem(&HddInfo, sizeof(DRIVE_INFO));
      if ( CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid) ) {
          //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. +>
          Status = pBS->HandleProtocol(
                        HandleBuffer[Index],
                        &gEfiDevicePathProtocolGuid,
                        (VOID **)&DevicePath
                        );
          if (EFI_ERROR(Status)) {
              continue;
          } else {
            DevicePathNode = DevicePath;
              
            while (!IsDevicePathEnd (DevicePathNode)) {
              if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
                  (DevicePathSubType (DevicePathNode) == HW_PCI_DP) ) {
                
                PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
                //break; //<sunhp001-20190730-should use last pci device path>
              }
              
              DevicePathNode = NextDevicePathNode (DevicePathNode);
            }
            
            if (PciDevicePath == NULL) {
              continue;
            }
            //Mizl-20201226-Add ASMedia AHCI Controller In Support List+>
            AsmediaVendID = 0;
            SataDevInterface = ((SATA_DISK_INFO *)DiskInfo)->SataDevInterface;
            if (SataDevInterface != NULL)
            {
                
              PciIO = SataDevInterface->AhciBusInterface->PciIO;
              if(PciIO != NULL)
                Status = PciIO->Pci.Read(PciIO, EfiPciIoWidthUint16, PCI_VID, 1, &AsmediaVendID);
            }
            //Mizl-20201226-Add ASMedia AHCI Controller In Support List->
            // 3rd party AHCI controller did not use AMI AHCI driver, AhciDriveInfoConstruction is not a general method to detect SATA drive.
            // For 3rd party AHCI controller, skip it as w/a. A general method need to be developed
            if ((PciDevicePath->Function == 0) && (AsmediaVendID != 0x1b21)){//<zhaoyf1-20191223 Judge board Sata Controller by Function for AMD and Hygon platform 2:board Sata controller 0:not board Sata controller>
              continue;
            }
          } //End if (EFI_ERROR(Status)) {
          //<Lizhq-20190619 Optimization for 3rd party AHCI controller compatibility issue. ->

        Status = AhciDriveInfoConstruction(DiskInfo, &HddInfo);
      } else if ( CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoNvmeInterfaceGuid)) {
        Status = NvmeDriveInfoConstruction(DiskInfo, &HddInfo);
      } else {
        Status = EFI_NOT_FOUND;
      }

      if (!EFI_ERROR(Status)) {
        Status = AppendStorageInfoList (StorageInfo, &HddInfo);
      }
    }

  }

  DEBUG((EFI_D_ERROR, __FUNCTION__" End Status:%r\n", Status));
  return Status;
}

EFI_STATUS GetOemStorageInfo(
  IN      OEM_STORAGE_INFO_PROTOCOL   *This,
  IN OUT  OEM_STORAGE_INFO  **OemStorageInfo
)
{
  EFI_STATUS          Status = EFI_SUCCESS;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));
  if(This==NULL || OemStorageInfo==NULL || mOemStorageInfo==NULL)
    return EFI_INVALID_PARAMETER;

  *OemStorageInfo = mOemStorageInfo;

  DEBUG((EFI_D_ERROR, __FUNCTION__" End Status: %r\n", Status));
  return Status;
}

VOID EFIAPI InstallStorageInfoProtocol (
  IN EFI_EVENT Event,
  IN VOID *Context)
{
  EFI_HANDLE          Handle;
  EFI_STATUS          Status = EFI_SUCCESS;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));

  mOemStorageInfo = AllocateZeroPool(sizeof(OEM_STORAGE_INFO));
  if (NULL == mOemStorageInfo) {
    DEBUG((EFI_D_ERROR, "mOemStorageInfo = 0x%x\n", mOemStorageInfo));
    return;
  }
  Handle = NULL;
  Status = InitStorageInfo (mOemStorageInfo);
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gOemStorageInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mOemStorageInfoProtocol
                  );

  if(Event) {
    pBS->CloseEvent(Event);
    Event=NULL;
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__" End Status:%r\n", Status));

  return ;
}

EFI_STATUS OemStorageInfoProtocolEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  
  EFI_EVENT   Event;
  VOID        *Registration;
  
  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));
  InitAmiLib(ImageHandle, SystemTable);
  
  Status = RegisterProtocolCallback(
            &gBdsAllDriversConnectedProtocolGuid ,
            InstallStorageInfoProtocol,
            NULL, &Event, &Registration
            );
  DEBUG((EFI_D_ERROR, __FUNCTION__" End Status:%r\n", Status));
  
  
  return Status;
}

