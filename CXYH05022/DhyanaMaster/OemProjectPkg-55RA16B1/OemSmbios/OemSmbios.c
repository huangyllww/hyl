//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Uefi/UefiSpec.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Pci.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Library/IpmiInterfaceLib.h>


#include <OemIpmiNetFnDefinitions.h>
#include <Library/AmdSocBaseLib.h>

#include <OemSmbios.h>
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 +>
#include <PCIEXPRESS.h> 
#include <Protocol/AmdCpmTableProtocol/AmdCpmTableProtocol.h>
#include <IndustryStandard/Pci22.h>
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9->

EFI_SMBIOS_PROTOCOL         static  *EfiSmbios = NULL;
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 +>
AMD_CPM_TABLE_PROTOCOL      *AmdCpmTableProtocolPtr = NULL;
SMBIOS_SYSTEM_SLOTS_INFO    SystemSlotsTemp = {{9, 0x11, 0}, 1,     SYS_SLOT_PCIE_4,    DBW_8X, 3, 3, 0xFF, 0x0C, 1, 0, 0xFF, 0xFF};
SMBIOS_SYSTEM_SLOTS_INFO    *Type9;
UINT8                       SecondaryBus, SubordinateBus;
BOOLEAN                     isSpecial = FALSE;


//CPU0 AGPIO23_3 PRSNT_HDDBP_N
#define PRSNT_HDDBP_N(ValidSeeting) 23,DIE3,SOCKET0,ValidSeeting,"PRSNT_HDDBP_N" 
#define PRSNT_REAR_PCIE_0_N(ValidSeeting) 10,DIE2,SOCKET1,ValidSeeting,"PRSNT_REAR_PCIE_0_N" 
#define PRSNT_REAR_PCIE_1_N(ValidSeeting) 40,DIE2,SOCKET1,ValidSeeting,"PRSNT_REAR_PCIE_1_N" 
#define INVALID_SETTING_ARRARY {INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,INVALID_SETTING,"INVALID_SETTING"}
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 ->

SMBIOS_TYPE_8 PortConnectorInfo[] = {
        {{{8, 0x09, 0}, 1, 0x12, 2, 0x12, 0x10}, "J1 USB1", "USB3-L"},
        {{{8, 0x09, 0}, 1, 0x12, 2, 0x12, 0x10}, "J1 USB2", "USB3-R"},
        {{{8, 0x09, 0}, 1, 0x00, 2, 0x07, 0x1C}, "J1 VGA", "VGA"}, 
        {{{8, 0x09, 0}, 1, 0x0B, 2, 0x0B, 0x1F}, "J20 LAN1", "LAN"},
        {{{8, 0x09, 0}, 1, 0x09, 2, 0x12, 0x10}, "J2 DEBUG_PORT", "USB"},
        
        {{{8, 0x09, 0}, 1, 0xFF, 0, 0, 0xFF}, "J53 M2_0"},
        {{{8, 0x09, 0}, 1, 0xFF, 0, 0, 0xFF}, "J175 M2_1"},
        {{{8, 0x09, 0}, 1, 0x12, 0, 0, 0x10}, "TypeA_USB (J45)"},
        {{{8, 0x09, 0}, 1, 0x22, 0, 0, 0x20}, "SATA4~7 (J52)"},
};
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 +>
//riser type 0x11
GLOBAL_REMOVE_IF_UNREFERENCED SMBIOS_TYPE_9 SystemSlotsInfo[] = {
    {    SYS_SLOT_PCIE_4,    DBW_16X, LONG_LENGTH, 0x1, 0x1,  "CPU0_PCIe_Riser2_Slot1",    L"PciRoot(0x3)/Pci(0x3,0x1)",    FALSE, INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_4,    DBW_8X, SHORT_LENGTH, 0x2, 0x2,   "CPU0_PCIe_Riser1_Slot2",    L"PciRoot(0x2)/Pci(0x3,0x2)",    FALSE, PRSNT_HDDBP_N(InputHigh), INVALID_SETTING_ARRARY},
    {    SYS_SLOT_PCIE_3,    DBW_8X, SHORT_LENGTH, 0x1, 0x1,   "CPU0_PCIe_Riser3_Slot1",    L"PciRoot(0x1)/Pci(0x1,0x1)",    FALSE, INVALID_SETTING_ARRARY },
    
//for rear slot
    {    SYS_SLOT_PCIE_3,    DBW_16X, LONG_LENGTH, 0x1, 0x1,  "CPU1_PCIe_Slot_1",    L"PciRoot(0x7)/Pci(0x3,0x1)",    FALSE, PRSNT_REAR_PCIE_0_N(InputHigh),PRSNT_REAR_PCIE_1_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_3,    DBW_16X, LONG_LENGTH, 0x1, 0x1,  "CPU1_PCIe_Slot_1",    L"PciRoot(0x7)/Pci(0x3,0x1)",    FALSE, PRSNT_REAR_PCIE_0_N(InputHigh),PRSNT_REAR_PCIE_1_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_3,    DBW_16X, LONG_LENGTH, 0x1, 0x1,  "CPU1_PCIe_Slot_1",    L"PciRoot(0x7)/Pci(0x3,0x1)",    FALSE, PRSNT_REAR_PCIE_0_N(InputLow),PRSNT_REAR_PCIE_1_N(InputHigh), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_3,    DBW_8X, LONG_LENGTH, 0x1, 0x1,   "CPU1_PCIe_Slot_1",    L"PciRoot(0x7)/Pci(0x3,0x1)",    FALSE, PRSNT_REAR_PCIE_0_N(InputLow),PRSNT_REAR_PCIE_1_N(InputLow), INVALID_SETTING_ARRARY },
    {    SYS_SLOT_PCIE_3,    DBW_8X, LONG_LENGTH, 0x2, 0x2,   "CPU1_PCIe_Slot_2",    L"PciRoot(0x7)/Pci(0x3,0x2)",    FALSE, PRSNT_REAR_PCIE_0_N(InputLow),PRSNT_REAR_PCIE_1_N(InputLow), INVALID_SETTING_ARRARY },

    {    0xFF, 0xFF,0xFF, 0xFF,0xFF, NULL, NULL, FALSE, INVALID_SETTING_ARRARY },

};

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

/**
GC_TODO: CheckGPIOSetting
@TableIndex   - The Offset of Current Table

@retval EFI_SUCCESS   - This Item is Valid,Enable this Item
@retval EFI_NOT_FOUND - This Item is Invalid,Jusr Ignore it
**/
EFI_STATUS
OemCheckPlatformSetting(
    IN    SMBIOS_TYPE_9 *SlotsInfo
)
{
    UINT8           iGpioIndex;
    UINT8           GPIOSetting;
    GPIOCONFIG      *GPIOConfig;
    EFI_STATUS      Status;
      
    if (AmdCpmTableProtocolPtr == NULL)
    {
        Status = gBS->LocateProtocol ( &gAmdCpmTableProtocolGuid,
                                          NULL,
                                          (VOID**)&AmdCpmTableProtocolPtr
                                          );
        if (Status != EFI_SUCCESS) {
              DEBUG ((DEBUG_ERROR , "gBS->LocateProtocol(gAmdCpmTableProtocolGuid)... ERROR\n"));
              return Status;
        }
    }

      //Get All Gpio's Setting
    for (iGpioIndex = 0; iGpioIndex < MaxGpioSetting; iGpioIndex++)
    {
        GPIOConfig = &SlotsInfo->GpioMap[iGpioIndex];
        DEBUG ((DEBUG_INFO , "GPIOConfig SocketNumber %d DieNumber %d GpioNumber %d\n", GPIOConfig->SocketNumber, GPIOConfig->DieNumber, GPIOConfig->GpioNumber));
        if(GPIOConfig->GpioNumber != INVALID_SETTING) 
            GPIOSetting = AmdCpmTableProtocolPtr->CommonFunction.GetGpio(AmdCpmTableProtocolPtr, (UINT16)((GPIOConfig->SocketNumber << 14) + (GPIOConfig->DieNumber << 10) + GPIOConfig->GpioNumber));
        else
            GPIOSetting = INVALID_SETTING;
        DEBUG ((DEBUG_INFO , "GPIOSetting %x\n", GPIOSetting));
        if ((iGpioIndex == 0) && (GPIOSetting == INVALID_SETTING)) //If first is invalid setting, it will follow last judge status
            return EFI_SUCCESS;
        if ((GPIOSetting != INVALID_SETTING) && (GPIOSetting  != GPIOConfig->Validvalue))
        {
            return EFI_INVALID_PARAMETER;
        }
        if (GPIOSetting == INVALID_SETTING)
        {
            return EFI_SUCCESS;
        }
    }
      
    return EFI_SUCCESS;
}
EFI_STATUS SetForDeviceWithSwitch(BOOLEAN ConfigSlot, UINT8 StartBus, UINT8 EndBus, IN OUT UINT8 *DevBusNum, IN OUT UINT8 *SlotNum)
{
    EFI_STATUS                          Status;
    UINT8                               SecBusNum, Bus, Device, Function;
    BOOLEAN                             MultiFunc=FALSE;
    BOOLEAN                             DeviceFound=FALSE;
    UINT8                               HeaderType, CapPtr8;
    UINT16                              VendorId = 0;
    PCIE_SLOT_CAP                       SlotData;

    for( Bus = StartBus; Bus <= EndBus; Bus++) {
    for( Device=0; Device <= PCI_MAX_DEVICE; Device++) {
        for( Function = 0; Function <= PCI_MAX_FUNC; Function++ ) {
            VendorId = PciRead16(PCI_LIB_ADDRESS(Bus, Device, Function, PCI_VENDOR_ID_OFFSET));
            if((Function == 0) && VendorId == DEVICE_ID_NOCARE) {
                break;
            } else if ( VendorId == DEVICE_ID_NOCARE ) {
                continue;
            }
            DEBUG((EFI_D_INFO, "Enumerating Bus @ B[%x] D[%x] F[%x]\n", Bus, Device, Function));
            HeaderType = PciRead8(PCI_LIB_ADDRESS(Bus, Device, Function, PCI_HEADER_TYPE_OFFSET));
            if( Function == 0) {
                if(HeaderType & HEADER_TYPE_MULTI_FUNCTION) {
                    MultiFunc = TRUE;
                }
            }
            HeaderType &= 0x03;
            switch(HeaderType) {
                case HEADER_TYPE_DEVICE :
                    if ((ConfigSlot == FALSE) && (Device == 0) && (Function == 0))
                    {
                        DEBUG ((DEBUG_INFO , "Slot implement with Bus %x Dev %x Func %x\n", Bus, Device, Function));
                        Type9->StructureType.Handle = 0xFFFE;
                        Type9->BusNumber = Bus;
                        Type9->DevFuncNumber = (UINT8)((Device << 3) + Function);
                        Type9->CurrentUsage = 0x4;
                        Status = EfiSmbios->Add(EfiSmbios, NULL, &Type9->StructureType.Handle, (EFI_SMBIOS_TABLE_HEADER *) Type9);
                        DEBUG ((DEBUG_INFO , "LINE %d Add SMBIOS type 9, Status %r\n", __LINE__, Status));
                        *DevBusNum = Bus;
                        DeviceFound = TRUE;
                        //SetForDeviceWithSwitch(TRUE, StartBus, Bus-1, DevBusNum, SlotNum);
                    }
                    break;
                case HEADER_TYPE_PCI_TO_PCI_BRIDGE:
                    if (ConfigSlot == TRUE)
                    {
                        SecBusNum = PciRead8(PCI_LIB_ADDRESS(Bus, Device, Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
                        if (SecBusNum == *DevBusNum)
                        {
                            if ((SecondaryBus != *DevBusNum) || ((SecondaryBus == *DevBusNum) && (isSpecial == TRUE)))
                            {
                                GetLegCapStrucAddr(PCI_LIB_ADDRESS(Bus, Device, Function, 0x00), 0x10,&CapPtr8);
                                SlotData.Dword = PciRead32(PCI_LIB_ADDRESS(Bus, Device, Function, CapPtr8 + PCIE_SLOT_CAPABILITY_OFFSET));
                                SlotData.Bits.PhysicalSlotNum = *SlotNum;
                                PciWrite32(PCI_LIB_ADDRESS(Bus, Device, Function, CapPtr8 + PCIE_SLOT_CAPABILITY_OFFSET), SlotData.Dword);
                                DEBUG((EFI_D_INFO, "Updating Slot number %x to @ B[%x] D[%x] F[%x]\n", *SlotNum, Bus, Device, Function));
                                SlotData.Dword = PciRead32(PCI_LIB_ADDRESS(Bus, Device, Function, CapPtr8 + PCIE_SLOT_CAPABILITY_OFFSET));
                                *SlotNum = (*SlotNum) + 1;
                                DEBUG((EFI_D_INFO, "Update result @ B[%x] D[%x] F[%x]: %x\n", Bus, Device, Function, SlotData.Bits.PhysicalSlotNum));
                            }
                        }
                    }

                    break;
                default:
                    break;
            }
            if((Function == 0) && !MultiFunc) {
                break;
            } else if(MultiFunc) {
                MultiFunc=FALSE;
            }
        }
    }
    }
    if (DeviceFound == TRUE)
        return EFI_SUCCESS;
    else
        return EFI_NOT_FOUND;
}
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 ->
/**
  Function to update SMBIOS type 8 for Port Connector Information

  @param None
  @return EFI_STATUS
  
**/
VOID
EFIAPI
UpdateSmbiosType8Table (VOID)
{
    EFI_STATUS                          Status;
    SMBIOS_TYPE_8                       *Type8;
    UINT8                               Index = 0;
    UINT8                               *p;
    UINT32                              VendorID;
    UINT8                               LanBusNumber = 0;

    
    LanBusNumber = PciRead8(PCI_LIB_ADDRESS(LAN_BUS, LAN_DEVICE, LAN_FUNCTION, 0x19));
    
    DEBUG ((DEBUG_INFO , "UpdateSmbiosType8Table LanBusNumber = %x\n", LanBusNumber));
    
    if (LanBusNumber != 0xff)
    {
        VendorID = PciRead32(PCI_LIB_ADDRESS(LanBusNumber, 0, 0, 0x0));
    }
    
    DEBUG ((DEBUG_INFO , "UpdateSmbiosType8Table VendorID = %x\n", VendorID));
    if (VendorID != 0x80861521 && VendorID != 0x15218086)
    {
        SMBIOS_TYPE_8 PortConnectorInfo[] = {
                {{{8, 0x09, 0}, 1, 0x12, 2, 0x12, 0x10}, "J1 USB1", "USB3-L"},
                {{{8, 0x09, 0}, 1, 0x12, 2, 0x12, 0x10}, "J1 USB2", "USB3-R"},
                {{{8, 0x09, 0}, 1, 0x00, 2, 0x07, 0x1C}, "J1 VGA", "VGA"}, 
                {{{8, 0x09, 0}, 1, 0x0B, 2, 0x0B, 0x1F}, "J3 LAN1", "LAN"},
                {{{8, 0x09, 0}, 1, 0x0B, 2, 0x0B, 0x1F}, "J24 LAN2", "LAN"},
                {{{8, 0x09, 0}, 1, 0x09, 2, 0x12, 0x10}, "J2 DEBUG_PORT", "USB"},
                
                {{{8, 0x09, 0}, 1, 0xFF, 0, 0, 0xFF}, "J53 M2_0"},
                {{{8, 0x09, 0}, 1, 0xFF, 0, 0, 0xFF}, "J175 M2_1"},
                {{{8, 0x09, 0}, 1, 0x12, 0, 0, 0x10}, "TypeA_USB (J45)"},
                {{{8, 0x09, 0}, 1, 0x22, 0, 0, 0x20}, "SATA4~7 (J52)"},
        };
    }
    
    for (Index = 0; Index < (sizeof(PortConnectorInfo)/sizeof(SMBIOS_TYPE_8)); Index++)
    {
        if(PortConnectorInfo[Index].PortInfo.ExtRefDesig == 2)
            Type8 = AllocateZeroPool (sizeof(SMBIOS_PORT_CONN_INFO) + AsciiStrLen(PortConnectorInfo[Index].sIntRefDesig) + AsciiStrLen(PortConnectorInfo[Index].sExtRefDesig) + 3);
        else
            Type8 = AllocateZeroPool (sizeof(SMBIOS_PORT_CONN_INFO) + AsciiStrLen(PortConnectorInfo[Index].sIntRefDesig) + 2);
        
        MemCpy(Type8, &PortConnectorInfo[Index], sizeof(SMBIOS_PORT_CONN_INFO));
        Type8->PortInfo.StructureType.Handle = 0xFFFE;
        p = (UINT8*)Type8 + sizeof(SMBIOS_PORT_CONN_INFO);
        MemCpy(p, PortConnectorInfo[Index].sIntRefDesig, AsciiStrLen(PortConnectorInfo[Index].sIntRefDesig));
        if(PortConnectorInfo[Index].PortInfo.ExtRefDesig == 2)
        {
            p = p + AsciiStrLen(PortConnectorInfo[Index].sIntRefDesig) + 1;
            MemCpy(p, PortConnectorInfo[Index].sExtRefDesig, AsciiStrLen(PortConnectorInfo[Index].sExtRefDesig));
        }
        Status = EfiSmbios->Add(EfiSmbios, NULL, &Type8->PortInfo.StructureType.Handle, (EFI_SMBIOS_TABLE_HEADER *) Type8);
        DEBUG ((DEBUG_INFO , "Add SMBIOS type 8, Index %x, Status %r\n", Index, Status));
    }
    return;
}
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 +>
/**
      Function to update SMBIOS type 9 for system slot information

      @param None
      @return EFI_STATUS
**/
VOID
EFIAPI
UpdateSmbiosType9Table (VOID)
{
    EFI_STATUS                          Status;

    UINT8                               CurSlot, SlotIndex = 0, StrLength;
    UINTN                               SegNum, BusNum, DevNum, FuncNum;
    EFI_HANDLE                          Handle;
    EFI_DEVICE_PATH_PROTOCOL            *SlotDevs;
    EFI_DEVICE_PATH_PROTOCOL            *DevPathTpm;
    EFI_PCI_IO_PROTOCOL                 *PciIo;
    INTN                                err;
    UINT8                               CapIdOffset;
    PCIE_SLOT_CAP                       SlotData;
    UINT8                               SpecialSlotNum = SPECIAL_SLOT_NUMBER_START + 5;
    UINT8                               Location, SwitchLocation[10], Index = 0, i;

    
    DEBUG ((DEBUG_INFO , "FUNCTION %a LINE %d\n", __FUNCTION__, __LINE__));

    for (CurSlot = 0; SystemSlotsInfo[CurSlot].SlotType != 0xFF; CurSlot++)
    {
      DEBUG ((DEBUG_INFO , "FUNCTION %a LINE %d, Current Slot %d\n", __FUNCTION__, __LINE__, CurSlot));
      Status = OemCheckPlatformSetting(&SystemSlotsInfo[CurSlot]);
      if (Status != EFI_SUCCESS)
          continue;
      StrLength = (UINT8)StrLen(SystemSlotsInfo[CurSlot].DevicePathText);

      isSpecial = SystemSlotsInfo[CurSlot].MatchDevPath;
      SlotDevs = ConvertTextToDevicePath(SystemSlotsInfo[CurSlot].DevicePathText);
      DevPathTpm = SlotDevs;
      gBS->LocateDevicePath (
                          &gEfiDevicePathProtocolGuid,
                          &DevPathTpm,
                          &Handle);
      gBS->HandleProtocol(
                         Handle,
                         &gEfiDevicePathProtocolGuid,
                         (VOID *)&DevPathTpm);
      err = MemCmp (SlotDevs, DevPathTpm, GetDevicePathSize(SlotDevs));
      Status = (err == 0) ? EFI_SUCCESS : EFI_NOT_FOUND;
      DEBUG ((DEBUG_INFO , "LINE %d LocateDevicePath Status %r DevicePathText %s StrLength %d\n", __LINE__, Status, SystemSlotsInfo[CurSlot].DevicePathText, StrLength));
      DEBUG ((DEBUG_INFO , "LINE %d SlotDevs DevicePathText %d, DevPathTpm DevicePathText %d\n", __LINE__, GetDevicePathSize(SlotDevs), GetDevicePathSize(DevPathTpm)));
      if ((EFI_ERROR(Status)) && (isSpecial == TRUE)) //Situation for switch Riser which has same Riser Type with common Riser, or some slimline no GPIO dependency. In such case, device path should be identical
            continue;
      if (Status == EFI_SUCCESS) //Skip some riser has same GPIO setting with Switch Riser
      {
                for (i = 0; i < Index; i++)
                {
                    Location = SwitchLocation[i];
                    err = MemCmp (SystemSlotsInfo[CurSlot].DevicePathText, SystemSlotsInfo[Location].DevicePathText, 50);
                    if (err == 0)
                        break;
                    
                }
                if ((i == Index) && (StrLength > 26))
                {
                    SwitchLocation[Index] = CurSlot;
                    Index++;
                }
                if ((i < Index) && (StrLength < 26))
                {
                    continue;
                }
      }
            
      Type9 = AllocateZeroPool (sizeof(SMBIOS_SYSTEM_SLOTS_INFO) + AsciiStrLen(SystemSlotsInfo[CurSlot].sSlotDes) + 2);
      MemCpy(Type9, &SystemSlotsTemp, sizeof(SMBIOS_SYSTEM_SLOTS_INFO));
      Type9->StructureType.Handle = 0xFFFE;
      MemCpy(((UINT8*)Type9 + sizeof(SMBIOS_SYSTEM_SLOTS_INFO)), SystemSlotsInfo[CurSlot].sSlotDes, AsciiStrLen(SystemSlotsInfo[CurSlot].sSlotDes));
      Type9->SlotType = SystemSlotsInfo[CurSlot].SlotType;
      Type9->SlotDataBusWidth = SystemSlotsInfo[CurSlot].SlotDataBusWidth;
      Type9->SlotLength = SystemSlotsInfo[CurSlot].SlotLength;
      //dengyh-20220117 change the physical slot ID to the slot ID at rear +
    #if defined(Show_rearslotID)&&(Show_rearslotID!=0)
            if(SystemSlotsInfo[CurSlot].RearSlotID!=0xFF)
                Type9->SlotID = SystemSlotsInfo[CurSlot].RearSlotID;
            else
                Type9->SlotID = SystemSlotsInfo[CurSlot].SlotID;
    #else
            Type9->SlotID = SystemSlotsInfo[CurSlot].SlotID;
    #endif
            //dengyh-20220117 change the physical slot ID to the slot ID at rear -
            if (EFI_ERROR(Status) || (SlotDevs == NULL))
            {
                Type9->CurrentUsage = 0x3;
                Status = EfiSmbios->Add(EfiSmbios, NULL, &Type9->StructureType.Handle, (EFI_SMBIOS_TABLE_HEADER *) Type9);
                DEBUG ((DEBUG_INFO , "LINE %d Add SMBIOS type 9, DevicePathText %s, Status %r\n", __LINE__, SystemSlotsInfo[CurSlot].DevicePathText, Status));
            }
            else
            {
                Status = gBS->OpenProtocol(
                            Handle,
                            &gEfiPciIoProtocolGuid,
                            &PciIo,
                            NULL,
                            NULL,
                            EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
                if (!EFI_ERROR(Status)) {
                    PciIo->Pci.Read(
                            PciIo,
                            EfiPciIoWidthUint8,
                            PCI_SBUS,
                            1,
                            &SecondaryBus);
                    PciIo->Pci.Read(
                            PciIo,
                            EfiPciIoWidthUint8,
                            PCI_SUBUS,
                            1,
                            &SubordinateBus);
                    CapIdOffset = PcieFindCapId(PciIo, 0x10);
                    PciIo->GetLocation(PciIo, &SegNum, &BusNum, &DevNum, &FuncNum);
                    if(CapIdOffset !=0){
                        PciIo->Pci.Read (PciIo,
                                  EfiPciIoWidthUint32,
                                  (CapIdOffset + PCIE_SLOT_CAPABILITY_OFFSET),
                                  1,
                                  &SlotData.Dword
                                  );
                      DEBUG ((DEBUG_INFO , "LINE %d Original [B%x|D%x|F%x] slot number %x\n", __LINE__, BusNum, DevNum, FuncNum, SlotData.Bits.PhysicalSlotNum));
                    }
                    DEBUG ((DEBUG_INFO , "LINE %d SecondaryBus %x SubordinateBus %x\n", __LINE__, SecondaryBus, SubordinateBus));

                    Status = SetForDeviceWithSwitch(FALSE, SecondaryBus, SubordinateBus, (UINT8 *)&BusNum, &SpecialSlotNum);
                    if (Status == EFI_NOT_FOUND)
                    {
                        //For Hotplug port, root port present but no device present
                        Type9->StructureType.Handle = 0xFFFE;
                        Type9->CurrentUsage = 0x3;
                        Status = EfiSmbios->Add(EfiSmbios, NULL, &Type9->StructureType.Handle, (EFI_SMBIOS_TABLE_HEADER *) Type9);
                        DEBUG ((DEBUG_INFO , "LINE %d Add SMBIOS type 9, DevicePathText %s, Status %r\n", __LINE__, SystemSlotsInfo[CurSlot].DevicePathText, Status));
                    }
                }
            }
        }
        return;
    }
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 ->

/**
  Update the SMBIOS type 39 by IPMI PSU command.

**/
#if SYSTEM_POWER_SUPPLY_INFO == 1
EFI_STATUS
UpdateSmbiosType39Table(
  VOID
)
{
    EFI_STATUS                          Status;
    SMBIOS_TABLE_TYPE39                 *Type39Structure;
    EFI_SMBIOS_HANDLE                   SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    EFI_SMBIOS_TYPE                     SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
    UINT8                               PsuIndex, StrIndex;
    UINT8                               DataBuffer[4];
    UINT8                               ResponseData[0xFF];
    UINT8                               ResponseDataSize;
    PACKED_OEM_IPMI_PSU_INFO          *PsuInfo;
    CHAR8                               IpmiPsuStr[7][MAX_SMBIOS_STRING_SIZE] = {0};
    PACKED_OEM_IPMI_PSU_MODULE_STATE  *PsuModuleState;
    UINTN                               StrNumber;
    EFI_IPMI_TRANSPORT                  *mIpmiTransportProtocol = NULL;
  
    DEBUG ((EFI_D_INFO, "%a Entry...\n", __FUNCTION__));
    Status = pBS->LocateProtocol (&gEfiDxeIpmiTransportProtocolGuid, NULL, &mIpmiTransportProtocol);
          if (EFI_ERROR(Status)) {
              DEBUG((EFI_D_ERROR, "Locate IPMI protocol Status:%r\n", Status));
              return Status;
               }
          Status = pBS->LocateProtocol (
                      &gEfiSmbiosProtocolGuid,
                      NULL,
                      (VOID **) &EfiSmbios);
          if (EFI_ERROR(Status)) {
                    DEBUG((EFI_D_ERROR, "Locate smbios protocol Status:%r\n", Status));
                    return Status;
                     }
          for(PsuIndex = 0; PsuIndex < NUMBER_OF_POWER_SUPPLY; PsuIndex++){
            // Read Type 39 structure
            Status = EfiSmbios->GetNext( EfiSmbios, &SmbiosHandle, &SmbiosType, (EFI_SMBIOS_TABLE_HEADER **) &Type39Structure, NULL);
            DEBUG ((EFI_D_INFO, "SmbiosProtocol Get Next Status: %r \n", Status));
            DEBUG ((EFI_D_INFO, "SmbiosProtocol Get Next SmbiosHandle: 0x%X \n", SmbiosHandle));
            DEBUG ((EFI_D_INFO, "  Type39Structure: 0x%p \n", Type39Structure));
            if (EFI_ERROR(Status))
              return Status;

            if (SmbiosHandle == SMBIOS_HANDLE_PI_RESERVED)
              break;

            //Use IPMI CMD get PSU Info
            ZeroMem(DataBuffer, sizeof(DataBuffer));
            ResponseDataSize = sizeof(ResponseData);
            ZeroMem(ResponseData, ResponseDataSize);
            DataBuffer[0] = SreverRespositorySubTypePsu;
            DataBuffer[1] = PsuIndex;
            DEBUG ((EFI_D_INFO, "PSU Index : %d\n", PsuIndex));

            Status=mIpmiTransportProtocol->SendIpmiCommand(
                    mIpmiTransportProtocol,
                    NETFN_OEM,
                    BMC_LUN,
                    CMD_OEM_GET_SERVER_REPOSITORY_INFO,
                    (UINT8*)DataBuffer,
                    sizeof(DataBuffer),
                    ResponseData,
                    &ResponseDataSize );
            if(EFI_ERROR(Status))
              return Status;
    PsuInfo = (PACKED_OEM_IPMI_PSU_INFO *)(ResponseData + 1);
    DEBUG ((EFI_D_INFO, "\n PsuPresent: %d \n", PsuInfo->PsuPresent));
    DEBUG ((EFI_D_INFO, "PsuRatedPower: %d \n", PsuInfo->PsuRatedPower));
    DEBUG ((EFI_D_INFO, "MaxPowerCapacity 0x%X \n", Type39Structure->MaxPowerCapacity));
  //  DEBUG ((EFI_D_INFO, "MaxPowerCapacity 0x%X %0x%X\n", ResponseData[131],ResponseData[130]));
    ZeroMem(IpmiPsuStr, sizeof(IpmiPsuStr));
    if(PsuInfo->PsuPresent == 1){
      AsciiStrnCpyS(IpmiPsuStr[0], sizeof(IpmiPsuStr[0]), PsuInfo->PsuLocation, sizeof(PsuInfo->PsuLocation));        //Location
      AsciiStrnCpyS(IpmiPsuStr[1], sizeof(IpmiPsuStr[1]), PsuInfo->PsuModel, sizeof(PsuInfo->PsuModel));              //DeviceName
      AsciiStrnCpyS(IpmiPsuStr[2], sizeof(IpmiPsuStr[2]), PsuInfo->PsuVendor, sizeof(PsuInfo->PsuVendor));            //Manufacturer
      AsciiStrnCpyS(IpmiPsuStr[3], sizeof(IpmiPsuStr[3]), PsuInfo->PsuSn, sizeof(PsuInfo->PsuSn));                    //SerialNumber
      //To update by DMI edit tool
//    AsciiStrnCpyS(IpmiPsuStr[4], sizeof(IpmiPsuStr[4]), SmBiosDefaultString, AsciiStrLen(SmBiosDefaultString));     //AssetTabNumber
//    AsciiStrnCpyS(IpmiPsuStr[5], sizeof(IpmiPsuStr[5]), SmBiosDefaultString, AsciiStrLen(SmBiosDefaultString));     //ModelPartNumber
      AsciiStrnCpyS(IpmiPsuStr[6], sizeof(IpmiPsuStr[6]), PsuInfo->PsuRevision, sizeof(PsuInfo->PsuRevision));        //RevisionLevel
      Type39Structure->MaxPowerCapacity = PsuInfo->PsuRatedPower;
    }

    //Use IPMI CMD get PSU State
    ZeroMem(DataBuffer, sizeof(DataBuffer));
    ResponseDataSize = sizeof(ResponseData);
    ZeroMem(ResponseData, ResponseDataSize);
    DataBuffer[0] = SreverRespositorySubTypePsu;
    DataBuffer[1] = PsuIndex;
    DEBUG ((EFI_D_INFO, "PSU Index : %d\n", PsuIndex));
            
            Status=mIpmiTransportProtocol->SendIpmiCommand(
                                                mIpmiTransportProtocol,
                                                NETFN_OEM,
                                                BMC_LUN,
                                                CMD_OEM_GET_MODULE_STATE,
                                                (UINT8*)DataBuffer,
                                                sizeof(DataBuffer),
                                                ResponseData,
                                                &ResponseDataSize );

    if(EFI_ERROR(Status))
      return Status;
    PsuModuleState = (PACKED_OEM_IPMI_PSU_MODULE_STATE *)ResponseData;
    DEBUG ((EFI_D_INFO, "Present 0x%X \n", PsuModuleState->Present));
    DEBUG ((EFI_D_INFO, "CriticalLevel 0x%X \n", PsuModuleState->CriticalLevel));
   *((UINT16 *)&Type39Structure->PowerSupplyCharacteristics) = 0;
    if(PsuModuleState->Present == 1){
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyHotReplaceable = 1;
      Type39Structure->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 4;    //Auto-switch
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyType = 4;            //Switching
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyPresent = 1;
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyUnplugged = 0;

      switch(PsuModuleState->CriticalLevel){
        case 0: //Normal
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 3;  //OK
          break;
        case 1: //Not critical
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 4;  //Non-critical
          break;
        case 2: //Critical
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 5;  //critical
          break;
        case 3: //Urgency
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 5;  //critical
          break;
        default:
          Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 2;  //unknown
      }

    }else{
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyHotReplaceable = 1;
      Type39Structure->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 2;    //unknown
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyType = 2;            //unknown
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyPresent = 0;
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyUnplugged = 1;
      Type39Structure->PowerSupplyCharacteristics.PowerSupplyStatus = 2;          //unknown
    }
    DEBUG ((EFI_D_INFO, "PowerSupplyCharacteristics: 0x%X \n", *((UINT16 *)&Type39Structure->PowerSupplyCharacteristics)));

            //
            //Update the strings
            //
            for(StrIndex = 0; StrIndex < 7; StrIndex++){
              if(IpmiPsuStr[StrIndex][0] == 0) continue;
              StrNumber = StrIndex + 1;
              DEBUG ((EFI_D_INFO, "Str[%d]: %a \n", StrNumber, IpmiPsuStr[StrIndex]));
              Status = EfiSmbios->UpdateString ( EfiSmbios, &SmbiosHandle, &StrNumber, IpmiPsuStr[StrIndex]);
            }
          }
          DEBUG ((EFI_D_INFO, "%a End...\n", __FUNCTION__));
          return Status;    
}
#endif

/**
  Callback function executed when the EndOfDxe event group is signaled.

  @param[in] Event      Event whose notification function is being invoked.
  @param[in] Context    The pointer to the notification function's context, which
                        is implementation-dependent.
**/
VOID
EFIAPI
UpdateSmbiosOnEndOfDxe (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
    EFI_STATUS          Status;
    
    Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &EfiSmbios);
    if (EFI_ERROR (Status))
    {
        DEBUG ((DEBUG_ERROR , "gBS->LocateProtocol(gEfiSmbiosProtocolGuid)... ERROR\n"));
        return;
    }
#if TYPE8_STRUCTURE == 0
    UpdateSmbiosType8Table();
#endif
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 +>
#if OEM_SLOT_INFO == 1
    DEBUG ((DEBUG_ERROR , "UpdateSmbiosType9Table\n"));
    UpdateSmbiosType9Table();
#endif
//<Kangmm-20220227-SGEZT22437 Update SMBIOS Type 9 ->    
#if SYSTEM_POWER_SUPPLY_INFO == 1
    Status = UpdateSmbiosType39Table();
#endif
 
    
    Status = gBS->CloseEvent (Event);
    ASSERT_EFI_ERROR (Status);
    return;
}
/**
   The entry function of the OemSmbios driver.

   This function updates SMBIOS for different board design.

   @param[in] ImageHandle  The firmware allocated handle for the EFI image.
   @param[in] SystemTable  A pointer to the EFI System Table.

   @retval EFI_SUCCESS     The entry point is executed successfully.
   @retval EFI_UNSUPPORTED Do not support ACPI S3.
   @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
OemSmbiosInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS    Status;
    EFI_EVENT     EndOfDxeEvent;
    
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UpdateSmbiosOnEndOfDxe,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR (Status);
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
