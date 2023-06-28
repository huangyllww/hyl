
//**********************************************************************
// $Header: LocalHddBoot.c
//
// $Function: Local Hdd Boot
//
// $Revision:01
//
//**********************************************************************

#include <Token.h>
#include <Setup.h>
#include <IndustryStandard/Pci30.h>
#include <Protocol/PciIo.h>
#include "BootOptions.h"
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>


/**
    This function filters available boot options based on Setup
    control value
               
         
    @param Device Pointer to boot device

          
    @retval TRUE boot option will be removed from boot list
    @retval FALSE boot option won't be removed from boot list

**/
BOOLEAN LocalHddFilter( 
    IN BOOT_DEVICE *Device 
)
{
    EFI_STATUS              Status;
    SETUP_DATA              SetupData;
    UINTN                   VariableSize;
    UINT32                  VariableAttribute;
    EFI_GUID                SetupGuid = SETUP_GUID;
    BBS_TABLE               *BbsTable;
    UINT8                   Class;
    UINT8                   SubClass;
    EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath = NULL;
    EFI_HANDLE                Handle = NULL;
    EFI_PCI_IO_PROTOCOL       *PciIo = NULL;
    
    VariableSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable(
             L"Setup",
             &SetupGuid,
             &VariableAttribute,
             &VariableSize,
             &SetupData
             );
    if (EFI_ERROR(Status)) {
        DEBUG ((DEBUG_ERROR , "%a GetVariable with Status %r\n", __FUNCTION__, Status));
        return FALSE;
    }
    if (SetupData.LocalHddBoot)
        return FALSE;
    if (Device->BbsEntry != NULL)
    {
        BbsTable = Device->BbsEntry;
        Class = BbsTable->Class;
        SubClass = BbsTable->SubClass;
        DEBUG((DEBUG_INFO, "%a Class %x SubClass %x\n",__FUNCTION__, Class, SubClass));
        switch(BbsTable->DeviceType){
            case BBS_HARDDISK:
                if((Class == PCI_CLASS_MASS_STORAGE) && (SubClass == PCI_CLASS_MASS_STORAGE_SCSI))
                    return FALSE;
                else
                    return TRUE;
                break;
            default:
                return FALSE;
                break;
        }
    }
    else
    {
      Status = gBS->HandleProtocol(Device->DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID**)&TempDevicePath);
      if ( Status != EFI_SUCCESS )
      {
          DEBUG((DEBUG_ERROR, "%a HandleProtocol gEfiDevicePathProtocolGuid Status is %r\n",__FUNCTION__, Status));
          return FALSE;
      }
      Status = gBS->LocateDevicePath(
                      &gEfiPciIoProtocolGuid,
                      &TempDevicePath,
                      &Handle);
      if ( Status != EFI_SUCCESS )
      {
          DEBUG((DEBUG_ERROR, "%a LocateDevicePath gEfiPciIoProtocolGuid Status is %r\n",__FUNCTION__, Status));
          return FALSE;
      }
      Status = gBS->HandleProtocol ( Handle,
                                     &gEfiPciIoProtocolGuid,
                                     &PciIo );
      if ( Status != EFI_SUCCESS )
      {
          DEBUG((DEBUG_ERROR, "%a HandleProtocol gEfiPciIoProtocolGuid Status is %r\n",__FUNCTION__, Status));
          return FALSE;
      }
      
      Status=PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xB, 1, &Class);
      Status=PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0xA, 1, &SubClass);
      DEBUG((DEBUG_INFO, "%a Class %x SubClass %x\n",__FUNCTION__, Class, SubClass));
      
      if(Class == PCI_CLASS_MASS_STORAGE)
      {
        if(SubClass == PCI_CLASS_MASS_STORAGE_SCSI)
            return FALSE;
        else
            return TRUE;
      }
    }
    return FALSE;
}
