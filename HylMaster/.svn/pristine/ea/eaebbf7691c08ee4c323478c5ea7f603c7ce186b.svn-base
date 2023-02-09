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
#include <IndustryStandard/Pci22.h>
#include <Pci.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <PCIEXPRESS.h>
#include <OemSmbios.h>
#include <ProjectSmbiosConfig.h>

EFI_SMBIOS_PROTOCOL         static  *EfiSmbios = NULL;
SMBIOS_SYSTEM_SLOTS_INFO    SystemSlotsTemp = {{9, 0x11, 0}, 1,     SYS_SLOT_PCIE_4,    DBW_8X, 3, 3, 0xFF, 0x0C, 1, 0, 0xFF, 0xFF};
SMBIOS_SYSTEM_SLOTS_INFO    *Type9;
UINT8                       SecondaryBus, SubordinateBus;
BOOLEAN                     isSpecial = FALSE;

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


/**
  Callback function executed when gEfiSmbiosProtocolGuid install.

  @param[in] Event      Event whose notification function is being invoked.
  @param[in] Context    The pointer to the notification function's context, which
                        is implementation-dependent.
**/
VOID
EFIAPI
UpdateSmbiosType9(
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

#if OEM_SLOT_INFO == 1
    UpdateSmbiosType9Table();
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
OemSmbiosType9EntryPoint(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS    Status = EFI_SUCCESS;
    EFI_EVENT     EndOfDxeEvent;
    
    DEBUG ((DEBUG_INFO , "FUNCTION %a LINE %d\n", __FUNCTION__, __LINE__));

    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UpdateSmbiosType9,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR (Status);
    return Status;

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
