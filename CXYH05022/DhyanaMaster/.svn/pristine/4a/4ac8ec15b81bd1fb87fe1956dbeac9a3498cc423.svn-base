//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
/** @file CRBCspDxeLib.c

    This file contains code for CRB platform initialization in the Library stage
*/


#include <Token.h>
#include <Library/PciLib.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <PCI.h>
#include <Protocol/PciIo.h>
#include "BootOptions.h"
#include "BootOptioneLinks.h"
#include "Protocol\IdeControllerInit.h"
#include "Protocol\DiskInfo.h"
#include "Protocol\AmiAhciBus.h"
#include "Protocol\LegacyRegion2.h"
#include <Protocol/FabricTopologyServices.h> //<Yaosr001-20190628 fixed nvme name when install a CPU>


EFI_GUID  gOemEfiLegacyRegion2ProtocolGuid = EFI_LEGACY_REGION2_PROTOCOL_GUID;


typedef struct {
  UINT8   RbSeg;
  UINT8   RbBus;
  UINT8   RbDev;
  UINT8   RbFunc;
  UINT8   Socket;
} RB_DATA;

typedef struct {
  RB_DATA Rb;
  CHAR8   *Name;
} NVME_NAME_DATA;

NVME_NAME_DATA NvmeSlotNameMapTable[] = {
    //RbSeg     //RbBus   //RbDev   //RbFunc  //Socket    //Name
    {{0x00,     0x60,     0x03,     0x06,     0x00},     "NVMe001"}, 
    {{0x00,     0x60,     0x03,     0x07,     0x00},     "NVMe002"}, 

    {{0x00,     0x00,     0x01,     0x01,     0x00},     "NVMe01"}, 
    {{0x00,     0x00,     0x01,     0x02,     0x00},     "NVMe02"}, 
    {{0x00,     0x00,     0x01,     0x03,     0x00},     "NVMe03"}, 
    {{0x00,     0x00,     0x01,     0x04,     0x00},     "NVMe04"}, 

    {{0x00,     0x20,     0x03,     0x01,     0x00},     "NVMe05"}, 
    {{0x00,     0x20,     0x03,     0x02,     0x00},     "NVMe06"}, 
    {{0x00,     0x20,     0x03,     0x03,     0x00},     "NVMe07"}, 
    {{0x00,     0x20,     0x03,     0x04,     0x00},     "NVMe08"}, 

    {{0x00,     0x40,     0x01,     0x02,     0x00},     "NVMe10"}, 
    {{0x00,     0x40,     0x01,     0x03,     0x00},     "NVMe11"}, 

    {{0x00,     0x80,     0x01,     0x01,     0x01},     "NVMe15"}, 
    {{0x00,     0x80,     0x01,     0x02,     0x01},     "NVMe16"}, 
    {{0x00,     0x80,     0x01,     0x03,     0x01},     "NVMe17"}, 
    {{0x00,     0x80,     0x01,     0x04,     0x01},     "NVMe18"}, 

    {{0x00,     0xA0,     0x03,     0x01,     0x01},     "NVMe19"}, 
    {{0x00,     0xA0,     0x03,     0x02,     0x01},     "NVMe20"}, 
    {{0x00,     0xA0,     0x03,     0x03,     0x01},     "NVMe21"}, 
    {{0x00,     0xA0,     0x03,     0x04,     0x01},     "NVMe22"}, 

    {{0x00,     0xC0,     0x01,     0x01,     0x01},     "NVMe23"}, 
    {{0x00,     0xC0,     0x01,     0x02,     0x01},     "NVMe24"}, 
    {{0x00,     0xC0,     0x01,     0x03,     0x01},     "NVMe25"}, 
    {{0x00,     0xC0,     0x01,     0x04,     0x01},     "NVMe26"}, 

    {{0x00,     0xE0,     0x03,     0x01,     0x01},     "NVMe27"}, 
    {{0x00,     0xE0,     0x03,     0x02,     0x01},     "NVMe28"}, 
    {{0x00,     0xE0,     0x03,     0x03,     0x01},     "NVMe29"}, 
    {{0x00,     0xE0,     0x03,     0x04,     0x01},     "NVMe30"}, 

    {{0xFF,     0xFF,     0xFF,     0xFF,     0xFF},     "NVMeXX"}
};
//<Yaosr001-20190628 fixed nvme name when install a CPU +>
NVME_NAME_DATA NvmeSlotNameMapTable1[] = {
    //RbSeg     //RbBus   //RbDev   //RbFunc  //Socket    //Name
    {{0x00,     0xC0,     0x03,     0x06,     0x00},     "NVMe001"}, 
    {{0x00,     0xC0,     0x03,     0x07,     0x00},     "NVMe002"}, 

    {{0x00,     0x00,     0x01,     0x01,     0x00},     "NVMe01"}, 
    {{0x00,     0x00,     0x01,     0x02,     0x00},     "NVMe02"}, 
    {{0x00,     0x00,     0x01,     0x03,     0x00},     "NVMe03"}, 
    {{0x00,     0x00,     0x01,     0x04,     0x00},     "NVMe04"}, 

    {{0x00,     0x40,     0x03,     0x01,     0x00},     "NVMe05"}, 
    {{0x00,     0x40,     0x03,     0x02,     0x00},     "NVMe06"}, 
    {{0x00,     0x40,     0x03,     0x03,     0x00},     "NVMe07"}, 
    {{0x00,     0x40,     0x03,     0x04,     0x00},     "NVMe08"}, 

    {{0x00,     0x80,     0x01,     0x02,     0x00},     "NVMe10"}, 
    {{0x00,     0x80,     0x01,     0x03,     0x00},     "NVMe11"}, 

    {{0x00,     0xC0,     0x03,     0x01,     0x00},     "NVMe12"}, 
    {{0x00,     0xC0,     0x03,     0x02,     0x00},     "NVMe13"}, 
    {{0xFF,     0xFF,     0xFF,     0xFF,     0xFF},     "NVMeXX"}
};
//<Yaosr001-20190628 fixed nvme name when install a CPU ->

CHAR8 *OemGetNvmePortName( 
    BBS_TABLE   *BbsTable,
    UINT8       *dFound
)
{
  NVME_NAME_DATA                   *pNvmeEntry = NULL;
  UINT8                            Bus;
  UINT8                            Device;
  UINT8                            Function;
  UINT8                            SecBus,SubBus;
  UINT32                           Data32;
  //<Yaosr001-20190628 fixed nvme name when install a CPU +>
  EFI_STATUS                       Status;
  UINTN                            NumberOfInstalledProcessors;
  UINTN                            NumberOfDie;
  UINTN                            IgnoredRootBridges;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  //<Yaosr001-20190628 fixed nvme name when install a CPU ->
  
  Bus = BbsTable->Bus & 0xFF;
  Device = BbsTable->Device & 0xFF;
  Function = BbsTable->Function & 0xFF;
  
  TRACE ((-1, "Bus:%x,Device:%x,Function:%x\n", Bus, Device, Function));

  //<Yaosr001-20190628 fixed nvme name when install a CPU +>
  // Locate FabricTopologyServices2Protocol
  Status = pBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
  FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, &IgnoredRootBridges);

  if(NumberOfInstalledProcessors == 2){
    pNvmeEntry = NvmeSlotNameMapTable;
  } else{
    pNvmeEntry = NvmeSlotNameMapTable1;
  }
  //<Yaosr001-20190628 fixed nvme name when install a CPU ->

  do {
    Data32 = PciRead32 ( PCI_LIB_ADDRESS(pNvmeEntry->Rb.RbBus, pNvmeEntry->Rb.RbDev, pNvmeEntry->Rb.RbFunc, 0x18));    
    SecBus = (Data32 >>8) & 0xFF;
    SubBus = (Data32 >>16) & 0xFF;

    if ((Bus >= SecBus) && (Bus <= SubBus)){
      *dFound = 1;
      break;
    }
    
    if ((pNvmeEntry->Rb.RbSeg == 0xFF) && (pNvmeEntry->Rb.Socket == 0xFF)){
      *dFound = 0;
      break;
    }
    
    pNvmeEntry++;
  } while(TRUE);
  
  TRACE ((-1, "OemGetNvmePortName:%a\n",pNvmeEntry->Name));

  return pNvmeEntry->Name;  

}

//  Function to Create AHCI drive Boot Option
EFI_STATUS
OemCreateBootOptionNameForAhciDrive ( 
    BBS_TABLE *BbsTable,
    UINT8  *DriveString
)
{
    EFI_STATUS                          Status;
    UINT32                              Granularity;
    EFI_LEGACY_REGION2_PROTOCOL         *LegacyRegion2Protocol = NULL;
    UINT8                               Index;
    EFI_HANDLE                          *HandleBuffer = NULL;
    UINTN                               HandleCount = 0;
    UINT8                               TempStr[0x100] = {0};
    EFI_IDE_CONTROLLER_INIT_PROTOCOL    *IdeControllerInit;
    EFI_PCI_IO_PROTOCOL                 *PciIo;
    UINTN                               SegNum;
    UINTN                               BusNum;
    UINTN                               DevNum;
    UINTN                               FuncNum;     
    UINT8                               SataBusNum[4*NSOCKETS] = {0};
    UINT8                               j = 0;
    UINT8                               WhichDie = 0;
    UINT8                               SataIndex = 0; 
    UINT8                               PortNum = 0;

    TRACE ((-1, "DriveString:%a\n",(CHAR8*)DriveString)); 
    
    // Get the number of Sata Controllers
    Status = pBS->LocateHandleBuffer (
                      ByProtocol,
                      &gEfiIdeControllerInitProtocolGuid,
                      NULL,
                      &HandleCount,
                      &HandleBuffer
                      );
    if (EFI_ERROR(Status)) {
        return Status;
    }

    for (Index = 0; Index < HandleCount; Index++) {
        // Check if the IdeControllerInitProtocol is present on the Controller Handle
        Status = pBS->HandleProtocol(
                          HandleBuffer[Index],
                          &gEfiIdeControllerInitProtocolGuid,
                          (VOID**)&IdeControllerInit);
        if (EFI_ERROR(Status)) {
            continue;
        }
        Status = pBS->HandleProtocol(
                          HandleBuffer[Index],
                          &gEfiPciIoProtocolGuid,
                          (VOID**)&PciIo);
        if (EFI_ERROR(Status)) {
            continue;
        }
         
        // Retrieves Sata controller's PCI bus, device and function number.
        
        Status = PciIo->GetLocation(PciIo, 
                                   &SegNum, 
                                   &BusNum, 
                                   &DevNum, 
                                   &FuncNum);
        if(EFI_ERROR(Status)) {
            continue;
        }

        SataBusNum[j] = (UINT8)BusNum;
        DEBUG ((-1, "SataBusNum[%d] = 0x%x\n",j,SataBusNum[j]));
        j++;
        if (j >= 4*NSOCKETS) {
          break;
        }     
    }

    PortNum = DriveString[1] - 0x30;
    TRACE ((-1, "PortNum:%d\n", PortNum));
    TRACE ((-1, "BbsTable->Bus:%x\n",BbsTable->Bus)); 

    j = 0;
    do {
      if ((BbsTable->Bus & 0xFF) == SataBusNum[j] || (j > 4*NSOCKETS)) {
        break;
      }
      j++; 
    } while (1);

    switch (j) {
      case 0:
        WhichDie = 0;
        break;
      case 1:
        WhichDie = 1;
        break;
      case 2:
        WhichDie = 4;
        break;
      case 3:
        WhichDie = 5;
        break;
      default:
        WhichDie = 0;
        break;
    }

    SataIndex = WhichDie*8 + PortNum + 1; //base on 1, decimal
    Sprintf( TempStr, "(Disk%02d) D%d SATA %s", SataIndex ,WhichDie, DriveString);
    
    TempStr[31]=0;
    
    Status = pBS->LocateProtocol(&gOemEfiLegacyRegion2ProtocolGuid,NULL,&LegacyRegion2Protocol);

    if(EFI_ERROR(Status)) {
        return Status;
    }
          
    LegacyRegion2Protocol->UnLock(LegacyRegion2Protocol,(UINT32)DriveString,32,&Granularity);

    pBS->CopyMem(DriveString,TempStr,32);

    LegacyRegion2Protocol->Lock(LegacyRegion2Protocol,(UINT32)DriveString,32,&Granularity);

    return EFI_SUCCESS;

}

//  Function to Create NVME drive Boot Option
EFI_STATUS
OemCreateBootOptionNameForNVMEDrive ( 
    BBS_TABLE *BbsTable,
    UINT8  *DriveString
)
{
    EFI_STATUS                       Status;
    UINT32                           Granularity;
    EFI_LEGACY_REGION2_PROTOCOL      *LegacyRegion2Protocol;
    UINT8                            TempStr[0x100] = {0};
    UINT8                            dFound;
    CHAR8                            *NvmeNamePrefix = NULL;

    NvmeNamePrefix = OemGetNvmePortName(BbsTable, &dFound);
    Sprintf( TempStr, "(%s) B:%02X D:%02X F:%02X %s", NvmeNamePrefix, BbsTable->Bus & 0xFF, BbsTable->Device & 0xFF, BbsTable->Function & 0xFF, DriveString);
    
    TempStr[31]=0;
    
    Status = pBS->LocateProtocol(&gOemEfiLegacyRegion2ProtocolGuid,NULL,&LegacyRegion2Protocol);

    if(EFI_ERROR(Status)) {
        return Status;
    }
          
    LegacyRegion2Protocol->UnLock(LegacyRegion2Protocol,(UINT32)DriveString,32,&Granularity);

    pBS->CopyMem(DriveString,TempStr,32);

    LegacyRegion2Protocol->Lock(LegacyRegion2Protocol,(UINT32)DriveString,32,&Granularity);

    return EFI_SUCCESS;

}


///  Fucntion to create boot option string to all drives
EFI_STATUS OemCreateStringForAllDrives(BBS_TABLE *BbsTable)
{
    EFI_STATUS                        Status;
    static EFI_GUID                   DiskInfoGuid = EFI_DISK_INFO_PROTOCOL_GUID;
    UINT8                             Class = BbsTable->Class;
    UINT8                             SubClass = BbsTable->SubClass;
    EFI_HANDLE                        DeviceHandle = (EFI_HANDLE)*(UINTN*)&BbsTable->IBV1;
    EFI_DISK_INFO_PROTOCOL            *DiskInfo;
    

    // Get disk information protocol
    Status = pBS->HandleProtocol( DeviceHandle, &DiskInfoGuid, &DiskInfo );

    if (EFI_ERROR(Status)) return 0;

    // Based on SubClass Value calling AHCI drive Option
    if(SubClass == 6) {
      OemCreateBootOptionNameForAhciDrive(BbsTable,(UINT8*)(((UINT32)BbsTable->DescStringSegment << 4)+((UINT32)BbsTable->DescStringOffset & 0x0FFFF)) );
    }

    // Based on SubClass Value calling NVME drive Option
    if(SubClass == 8) {
      OemCreateBootOptionNameForNVMEDrive(BbsTable,(UINT8*)(((UINT32)BbsTable->DescStringSegment << 4)+((UINT32)BbsTable->DescStringOffset & 0x0FFFF)) );
    }

    return Status;
}



/// OEM hook function 
UINTN OEMConstructBootOptionNameByHWMap (
        BOOT_OPTION *Option, CHAR16 *Name, UINTN NameSize
)
{
    if (Option->BbsEntry == NULL) return 0;

    // Function to Change the Boot Option Name of the Drive
    OemCreateStringForAllDrives(Option->BbsEntry);

    return 0;
  
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
