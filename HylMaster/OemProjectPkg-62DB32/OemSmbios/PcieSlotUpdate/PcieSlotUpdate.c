
//**********************************************************************
// $Header: PcieSlotUpdate.c
//
// $Function: SMBIOS table is updated here
//
// $Revision:01
//
// $Date:  Lizhq 2017/03/23
//**********************************************************************

#include <Token.h>
#include <AmiDxeLib.h>
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Smbios.h>
#include <Protocol/Smbios.h>
#include <Library\UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/AmiBoardInfo2.h>
#include <Library/AmiSdlLib.h>
#include <ProjectSmbiosConfig.h>

EFI_STATUS AmiSdlFindGPPBridgeLocation(IN UINTN ParentIndex, IN UINT8 Device, IN UINT8 Function, OUT AMI_SDL_PCI_DEV_INFO **Record, OUT UINTN *Index){
    UINTN                   i;
    AMI_SDL_PCI_DEV_INFO    *dev;
    EFI_STATUS              Status;
//-----------------
    if(Record==NULL || Index==NULL) return EFI_INVALID_PARAMETER;

    Status=AmiSdlInitBoardInfo();
    if(EFI_ERROR(Status)) return Status;

    for(i=0; i<gSdlPciData->PciDevCount; i++){
        dev=&gSdlPciData->PciDevices[i];
        if(dev->PciDevFlags.Bits.Pci2PciBridgeType) {
            if((dev->Device == Device) && (dev->Function == Function ) && (dev->ParentIndex == ParentIndex)){
                *Record=dev;
                *Index=i;
                return EFI_SUCCESS;
            }
        }
    }
    return EFI_NOT_FOUND;
}


/**
  This function is the entry point for this driver.  And initializes the notify event of update SMBIOS table

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS PcieSlotUpdateEntryPoint(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_DEVICE_PATH_PROTOCOL      *SlotDevs;
  ACPI_HID_DEVICE_PATH          *AcpiDp = NULL;
  AMI_SDL_PCI_DEV_INFO          *sltdata = NULL;
  AMI_SDL_PCI_DEV_INFO          **AmiSdlPciDevData = NULL;
  UINTN                         childcnt;
  UINTN                         DevIndex, DevBridgeIndex, DevRootIndex;
  UINT8                         CurSlot, BusNum, DevNum, FuncNum;

  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n"));
  InitAmiLib(ImageHandle,SystemTable);
  
  for (CurSlot = 0; SystemSlotsInfo[CurSlot].SlotType != 0xFF; CurSlot++)
  {
      Status = OemCheckPlatformSetting(&SystemSlotsInfo[CurSlot]);
      if (Status != EFI_SUCCESS)
          continue;
      SlotDevs = ConvertTextToDevicePath(SystemSlotsInfo[CurSlot].DevicePathText);
      AcpiDp = (ACPI_HID_DEVICE_PATH *)SlotDevs;

      while(!isEndNode (SlotDevs))   {
          if ((SlotDevs->Type == HARDWARE_DEVICE_PATH) &&
                  (SlotDevs->SubType == HW_PCI_DP)) {
              break;
          }
          SlotDevs = NEXT_NODE(SlotDevs);
      }
      if (!isEndNode(NEXT_NODE(SlotDevs))) //Switch case, update slot number not work
          continue;
      BusNum = AcpiDp->UID;
      DevNum = ((PCI_DEVICE_PATH *)SlotDevs)->Device;
      FuncNum = ((PCI_DEVICE_PATH *)SlotDevs)->Function;
      DEBUG ((DEBUG_INFO , "FUNCTION %a LINE %d BusNum %d DevNum %d FuncNum %d\n", __FUNCTION__, __LINE__, BusNum, DevNum, FuncNum));

      Status = AmiSdlFindRbBySegBus(0, AcpiDp->UID, &sltdata, &DevRootIndex);
      if (Status != EFI_SUCCESS)
          continue;
      Status = AmiSdlFindGPPBridgeLocation(DevRootIndex, DevNum, FuncNum, &sltdata, &DevBridgeIndex);
      if (Status != EFI_SUCCESS)
          continue;
      Status=AmiSdlPciGetChildsOfParentIndex(&AmiSdlPciDevData,&childcnt,DevBridgeIndex);
      if (Status != EFI_SUCCESS)
          continue;
      sltdata = AmiSdlPciDevData[0];
      AmiSdlFindRecordIndex(sltdata, &DevIndex);

      DEBUG((EFI_D_ERROR, "Update PCIDevice[%d] Slot %d with slot number %d\n", DevIndex, sltdata->Slot, SystemSlotsInfo[CurSlot].SlotID));
      sltdata->Slot = SystemSlotsInfo[CurSlot].SlotID;
  }

  DEBUG((EFI_D_ERROR, __FUNCTION__" End\n"));
  return Status;
}
