//***********************************************************************
//*                           *
//*   Copyright (c) 1985-2020, American Megatrends International LLC.   *
//*                           *
//*      All rights reserved. Subject to AMI licensing agreement.       *
//*                           *
//***********************************************************************

/** @file NbPciDxeInitLib.c
    Chipset NB PCI libraries

**/

#include <AmiDxeLib.h>
#include <Token.h>
#include <PciBus.h>
#include <PciHostBridge.h>
#include <Setup.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/AmiBoardInitPolicy.h>
#include <Library/PciAccessCspLib.h>
#include <Library/AmiPciBusLib.h>
#include <Library/PciLib.h>

#include <Library/AmdSocBaseLib.h>
#include <PciNumaMapping.h>
#include "Library/AmiSdlLib.h"
#include "Library/MemoryAllocationLib.h"
#include "AmiPciBusSetupOverrideLib.h"
#include "Library\BaseMemoryLib.h"
//-------------------------------------------------------------------------
//!!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!! PORTING REQUIRED !!!!!!!!!!!*
//!!!!!!!!!!!!!!!! must be maintained for PCI devices!!!!!!!!!!!!!!!!!!*
//-------------------------------------------------------------------------

LIST_ENTRY                      gVgaList;
EFI_PCI_PLATFORM_PROTOCOL       gPciPlatformProtocol;

extern EFI_S3_SAVE_STATE_PROTOCOL      *gS3SaveState;
extern EFI_STATUS LocateS3SaveProtocol();

extern VOID FindSlot (
PCI_DEV_INFO    *dev,
UINT32      *Slot
);

extern VOID VgaRoute(
IN PCI_DEV_INFO		*dev
);

typedef struct _VGA_INFO {
    EFI_LIST_ENTRY Link;
    PCI_DEV_INFO   *Dev;
} VGA_INFO;


// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: GraphicDevXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS 
GraphicDevXXX_Init(
  AMI_BOARD_INIT_PROTOCOL    *This,
  IN UINTN          *Function,
  IN OUT VOID          *ParameterBlock
)
{
    //Update Standard parameter block
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args = (AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_INIT_STEP     InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO      *dev = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS        Status = EFI_UNSUPPORTED;
//    PCI_CFG_ADDR      addr;
    SETUP_DATA  SetupData;
    UINTN             BufferSize;
    EFI_GUID          EfiSetupGuid = SETUP_GUID;
    UINT32            Slot;
    UINT32            LegacyVgaBDF;
    UINT8             OnBoard = 1, OffBoard = 0;

    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep >= isPciMaxStep) return EFI_INVALID_PARAMETER;
    switch (InitStep)
    {
        case isPciGetOptionRom:
            DEBUG((DEBUG_VERBOSE,"\n%a (isPciGetOptionRom); ", __FUNCTION__ ));
      // If CSM is enabled && CSM video is set to legacy then limit to 32bit MMIO.
      // Ideally only 1st gfx device found with legacy opt rom would have this done.

            BufferSize = sizeof(SETUP_DATA);
            pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &BufferSize, &SetupData);
            if ((dev->Class.BaseClassCode == 0x03 && dev->Class.SubClassCode == 0x00) ||
                (dev->Class.BaseClassCode == 0x03 && dev->Class.SubClassCode == 0x80) ||
                (dev->Class.BaseClassCode == 0x04 && dev->Class.SubClassCode == 0x00))
            {
                FindSlot(dev, &Slot);
                LegacyVgaBDF = PcdGet32 (PcdLegacyVgaBDF);
                DEBUG((DEBUG_VERBOSE,"\nLegacyVgaBDF %x, OnExtVgaSelect %x, RomImage %x RomSize %x\n", LegacyVgaBDF, SetupData.OnExtVgaSelect, dev->PciIo.RomImage, dev->PciIo.RomSize));

                if((LegacyVgaBDF & ~BIT00) != PCI_LIB_ADDRESS(dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function,OffBoard))
                {
                    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SKIP OPROM!!\r\n",__FUNCTION__,__LINE__));
                    dev->PciIo.RomImage = NULL;
                    dev->PciIo.RomSize = 0;
                }
                //Mizl-20210702- Just Need Compare BDF Information For Extern Setting When No VGA Installed->
                DEBUG((DEBUG_VERBOSE,"\nSetting the device B %x D %x F %x RomImage %x RomSize %x\n", dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function, dev->PciIo.RomImage, dev->PciIo.RomSize ));
            }
            break;
  //-------------------------------------------------------------------------
    case isPciSkipDevice:
        DEBUG((DEBUG_VERBOSE," (isPciSkipDevice); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciSetAttributes:
        DEBUG((DEBUG_VERBOSE," (isPciSetAttributes); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciProgramDevice:
        DEBUG((DEBUG_VERBOSE," (isPciProgramDevice); " ));

        break;
  //-------------------------------------------------------------------------
    case isPcieSetAspm:
        DEBUG((DEBUG_VERBOSE," (isPcieSetAspm); " ));

        break;
  //-------------------------------------------------------------------------
    case isPcieSetLinkSpeed:
        DEBUG((DEBUG_VERBOSE," (isPcieSetLinkSpeed); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciOutOfResourcesCheck:
        DEBUG((DEBUG_VERBOSE," (isPciOutOfResourcesCheck); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciReadyToBoot:
        DEBUG((DEBUG_VERBOSE," (isPciReadyToBoot); " ));

        break;
  //-------------------------------------------------------------------------
    }//switch

    return Status;
}

EFI_STATUS PciDevXXX_UpdateSlotNumber(
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)
{
    EFI_STATUS                      Status;
    EFI_PHYSICAL_ADDRESS            DsdtAddress;
	ACPI_HDR                        *Dsdt;
    
    UINTN                           SdlIndex;
    AMI_SDL_PCI_DEV_INFO            **SdlBuffer;
    AMI_SDL_PCI_DEV_INFO            *SdlDev;
    UINTN                           SdlCount;
    UINTN                           SdlChildIndex;

    PCI_CFG_ADDR                    addr;

    AMI_BOARD_INIT_PARAMETER_BLOCK	*Args=(AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_DEV_INFO   		            *dev=(PCI_DEV_INFO*)Args->Param1;

    Dsdt = NULL;
    Status = LibGetDsdt(&DsdtAddress, EFI_ACPI_TABLE_VERSION_ALL);
    if (!EFI_ERROR(Status)) {
        Dsdt = (ACPI_HDR*)DsdtAddress;
    }

    Status = LocateS3SaveProtocol();

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
    if (dev->Type != tPci2PciBrg || \
        dev->PciExpress->PcieCap.PortType != PCIE_TYPE_ROOTPORT || \
        !dev->PciExpress->PcieCap.SlotImpl) 
    {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Invalid Paramater\r\n",__FUNCTION__,__LINE__));
        return EFI_SUCCESS;
    }
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
    Status = AmiSdlFindRecordIndex(dev->AmiSdlPciDevData, &SdlIndex);
    if (EFI_ERROR(Status)) 
    {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Status:%r\r\n",__FUNCTION__,__LINE__,Status));
        return EFI_SUCCESS;
    }

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));

    SdlBuffer = NULL;
    Status = AmiSdlPciGetChildsOfParentIndex(&SdlBuffer, &SdlCount, SdlIndex);
    if (EFI_ERROR(Status)) 
    {
        if(SdlBuffer != NULL)
            pBS->FreePool(SdlBuffer);

        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Status:%r\r\n",__FUNCTION__,__LINE__,Status));
        return EFI_SUCCESS;
    }

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
    for (SdlChildIndex = 0; SdlChildIndex < SdlCount; SdlChildIndex++) 
    {
        SdlDev = SdlBuffer[SdlChildIndex];
        if (SdlDev->Slot == 0) 
            continue;
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
        // Update PhisSlotNum
        addr.ADDR = dev->Address.ADDR;
        addr.Addr.Register = dev->PciExpress->PcieOffs+PCIE_SLT_CAP_OFFSET;
        dev->PciExpress->SlotCap.PhisSlotNum = SdlDev->Slot;
        Status = PciCfg32(dev->RbIo, addr, TRUE, &dev->PciExpress->SlotCap.SLT_CAP);      
        if(gS3SaveState != NULL) 
        {
            BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                gS3SaveState,           //This
                EfiBootScriptWidthUint32,   //Width
                addr.ADDR,1,&dev->PciExpress->SlotCap.SLT_CAP);            //Address, Data
        } else 
        {
            PCI_TRACE((TRACE_PCI,"ERROR: gS3SaveState invalid \n"));
        }

        PCI_TRACE((TRACE_PCI," (Slot %d) ", SdlDev->Slot ));
        if (Dsdt == NULL) 
            continue;

        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
        Status = UpdateAslNameOfDevice(Dsdt, SdlDev->AslName, "_SUN", (UINT64)SdlDev->Slot);
        break;
    }
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
    if (SdlChildIndex == SdlCount) 
    {
      
        // Clear SlotImplement bit
        addr.ADDR = dev->Address.ADDR;
        addr.Addr.Register = dev->PciExpress->PcieOffs+PCIE_CAP_OFFSET;
        Status=PciCfg16(dev->RbIo,addr,FALSE,&dev->PciExpress->PcieCap.PCIE_CAP);          
        dev->PciExpress->PcieCap.SlotImpl = 0;
        Status=PciCfg16(dev->RbIo,addr,TRUE,&dev->PciExpress->PcieCap.PCIE_CAP);          
        PCI_TRACE((TRACE_PCI," (Onboard) "));
        if(gS3SaveState != NULL) 
        {
            BOOT_SCRIPT_S3_PCI_CONFIG_WRITE_MACRO(
                gS3SaveState,           //This
                EfiBootScriptWidthUint16,   //Width
                addr.ADDR,1,&dev->PciExpress->PcieCap.PCIE_CAP);            //Address, Data
        } else 
        {
            PCI_TRACE((TRACE_PCI,"ERROR: gS3SaveState invalid \n"));
        }
    }
    
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
    pBS->FreePool(SdlBuffer);

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Status:%r\r\n",__FUNCTION__,__LINE__,Status));
    return EFI_SUCCESS;         
}
// <AMI_PHDR_START>
//-------------------------------------------------------------------------
//
// Procedure: DeviceXXXX_Init
//
// Description:
//  This function provide each initial routine in genericsio.c
//
// Input:
//  IN  AMI_SIO_PROTOCOL    *AmiSio - Logical Device's information
//  IN  EFI_PCI_IO_PROTOCOL *PciIo - Read/Write PCI config space
//  IN  SIO_INIT_STEP       InitStep - Initial routine step
//
// Output:
//  EFI_SUCCESS - Initial step sucessfully
//  EFI_INVALID_PARAMETER - not find the initial step
//
// Modified:    Nothing
//
// Referrals:   None
//
// Notes:
//-------------------------------------------------------------------------
// <AMI_PHDR_END>
EFI_STATUS PciDevXXX_Init_Override(
AMI_BOARD_INIT_PROTOCOL		*This,
IN UINTN					*Function,
IN OUT VOID					*ParameterBlock
)
{
    AMI_BOARD_INIT_PARAMETER_BLOCK  *Args = (AMI_BOARD_INIT_PARAMETER_BLOCK*)ParameterBlock;
    PCI_INIT_STEP                   InitStep = (PCI_INIT_STEP)Args->InitStep;
    PCI_DEV_INFO                    *dev = (PCI_DEV_INFO*)Args->Param1;
    EFI_STATUS                      Status = EFI_UNSUPPORTED;
    PCI_CFG_ADDR                    addr;

    SETUP_DATA  SetupData;
    UINTN                           BufferSize;
    EFI_GUID                        EfiSetupGuid = SETUP_GUID;
 // UINT32                          Slot;
#if BoardPciRes_SUPPORT
    UINT32                          LegacyVgaBDF;
#endif
    VGA_INFO                        *VgaEntry;    

#if BoardPciRes_SUPPORT             
    EFI_STATUS                      MatchVgaStatus;
#endif

    //Check if parameters passed are VALID and
    if(Args->Signature != AMI_PCI_PARAM_SIG) return EFI_INVALID_PARAMETER;
    if(InitStep >= isPciMaxStep) return EFI_INVALID_PARAMETER;

    if (dev->ParentBrg->Type != tPci2PciBrg && dev->ParentBrg->Type != tPci2CrdBrg) 
    {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Invalid Parameter\r\n",__FUNCTION__,__LINE__));
        return EFI_SUCCESS;
    }
   
    if (dev->ParentBrg->PciExpress->PcieCap.SlotImpl && (dev->ParentBrg->PciExpress->SlotCap.PhisSlotNum != dev->AmiSdlPciDevData->Slot)) 
    {
        addr.ADDR = dev->ParentBrg->Address.ADDR;
        addr.Addr.Register = dev->ParentBrg->PciExpress->PcieOffs+PCIE_SLT_CAP_OFFSET;
        dev->ParentBrg->PciExpress->SlotCap.PhisSlotNum = dev->AmiSdlPciDevData->Slot;
        Status = PciCfg32(dev->RbIo,addr,TRUE,&dev->ParentBrg->PciExpress->SlotCap.SLT_CAP);
    }

    switch (InitStep)
    {
        case isPciGetSetupConfig:
            DEBUG((DEBUG_VERBOSE," (isPciGetSetupConfig); " ));
            // If CSM is enabled && CSM video is set to legacy then limit to 32bit MMIO.
            // Ideally only 1st gfx device found with legacy opt rom would have this done.
#if BoardPciRes_SUPPORT
            GetLastBootFailed();
            if(gLastBootFailedVar.DisableNextBoot){
                if ((dev->Class.BaseClassCode == PCI_CLASS_DISPLAY && dev->Class.SubClassCode == PCI_CLASS_DISPLAY_VGA) ||
                    (dev->Class.BaseClassCode == PCI_CLASS_DISPLAY && dev->Class.SubClassCode == PCI_CLASS_DISPLAY_OTHER) ||
                    (dev->Class.BaseClassCode == PCI_CLASS_MEDIA && dev->Class.SubClassCode == PCI_CLASS_DISPLAY_VGA)){
                    LegacyVgaBDF = PcdGet32(PcdLegacyVgaBDF);
                    if(!LegacyVgaBDF){
                        MatchVgaStatus = MatchVgaDevice(dev);
                        if(MatchVgaStatus != EFI_SUCCESS) VgaRoute(dev);
                    }
                }
            } else {
#endif
                BufferSize = sizeof(SETUP_DATA);
                pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &BufferSize, &SetupData);
                    if ((dev->Class.BaseClassCode == 0x03 && dev->Class.SubClassCode == 0x00) ||
                        (dev->Class.BaseClassCode == 0x03 && dev->Class.SubClassCode == 0x80) ||
                        (dev->Class.BaseClassCode == 0x04 && dev->Class.SubClassCode == 0x00))
                    {
                        VgaEntry = AllocateZeroPool( sizeof(VGA_INFO));
                        VgaEntry->Dev = dev;
                    
                        InsertTailList(&gVgaList, &VgaEntry->Link);
                    } //end if VGA device
#if BoardPciRes_SUPPORT
        }
#endif
    // APTIOV_SERVER_OVERRIDE_START : AmiPciBus Setup Override changes.
    if (dev->PciExpress != NULL) {
        if (dev->PciExpress->Pcie2 != NULL) {
            Status = AmiPciGetPcie2SetupDataOverrideHook (&dev->PciExpress->Pcie2->Pcie2Setup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
            DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]AmiPciGetPcie2SetupDataOverrideHook:%r\r\n",__FUNCTION__,__LINE__,Status));

        }
        Status = AmiPciGetPcie1SetupDataOverrideHook (dev, &dev->PciExpress->Pcie1Setup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]AmiPciGetPcie1SetupDataOverrideHook:%r\r\n",__FUNCTION__,__LINE__,Status));


    }
    Status = AmiPciGetPciDevSetupDataOverrideHook (&dev->DevSetup, dev->AmiSdlPciDevData, dev->SdlDevIndex, FALSE);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]AmiPciGetPciDevSetupDataOverrideHook:%r\r\n",__FUNCTION__,__LINE__,Status));
    // APTIOV_SERVER_OVERRIDE_END : AmiPciBus Setup Override changes.
    break;
  //-------------------------------------------------------------------------
    case isPciSkipDevice:
        DEBUG((DEBUG_VERBOSE," (isPciSkipDevice); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciSetAttributes:
        DEBUG((DEBUG_VERBOSE," (isPciSetAttributes); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciProgramDevice:
        DEBUG((DEBUG_VERBOSE," (isPciProgramDevice); " ));

        break;
  //-------------------------------------------------------------------------
    case isPcieSetAspm:
        DEBUG((DEBUG_VERBOSE," (isPcieSetAspm); " ));

        break;
  //-------------------------------------------------------------------------
    case isPcieSetLinkSpeed:
        DEBUG((DEBUG_VERBOSE," (isPcieSetLinkSpeed); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciGetOptionRom:
        DEBUG((DEBUG_VERBOSE," (isPciGetOptionRom); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciOutOfResourcesCheck:
        DEBUG((DEBUG_VERBOSE," (isPciOutOfResourcesCheck); " ));

        break;
  //-------------------------------------------------------------------------
    case isPciReadyToBoot:
        DEBUG((DEBUG_VERBOSE," (isPciReadyToBoot); " ));

        break;
  //-------------------------------------------------------------------------
    }//switch

    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PhaseNotify (
  IN EFI_PCI_PLATFORM_PROTOCOL                       *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE  Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{
    EFI_STATUS              Status;
    PCI_DEV_INFO            *dev;
    PCI_DEV_INFO            *SelectedVGA = NULL;
    PCI_DEV_INFO            *OnboardVGA = NULL;
    VGA_INFO                *VgaEntry;
    LIST_ENTRY              *NodeEntry;
    UINT32                  Slot;
    UINT32                  count = 0;
    SETUP_DATA              SetupData;
    UINTN                   BufferSize;
    EFI_GUID                EfiSetupGuid = SETUP_GUID;
    
    DEBUG((-1, "%a InitStep: %d\n",__FUNCTION__, Phase));
    
    if(ChipsetPhase == ChipsetExit) return EFI_SUCCESS;
    
    switch (Phase) {

      case EfiPciHostBridgeEndBusAllocation:
        NodeEntry = GetFirstNode (&gVgaList);
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  
        BufferSize = sizeof(SETUP_DATA);
        Status = pRS->GetVariable(L"Setup", &EfiSetupGuid, NULL, &BufferSize, &SetupData);
          
        if(EFI_ERROR(Status)) return Status;
         
        while(!IsNull (&gVgaList, NodeEntry)) {
            count++;
            DEBUG((DEBUG_ERROR, "count:%x\n",count));
            DEBUG((DEBUG_ERROR, "SetupData.OnExtVgaSelect:%x\n",SetupData.OnExtVgaSelect));
            VgaEntry = (VGA_INFO *) NodeEntry;
            dev = VgaEntry->Dev;
            DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function, dev->Attrib));
              
            FindSlot(dev, &Slot);
            DEBUG((DEBUG_ERROR, "Slot:%x\n",Slot));
            if(dev->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)
            {
            OnboardVGA = dev;
            }
            if(SelectedVGA == NULL){
                SelectedVGA = dev;
            } else 
            {
                //If current device is onboard
                if(dev->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE){
                    //Only situation where onboard gets priority over existing device if SETUP->Onboard and existing is EXT
                    if((SetupData.OnExtVgaSelect == 1) && !(SelectedVGA->Attrib & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)){
                        SelectedVGA = dev;
                    }
                } else {
                    //If current device is external.
                    //if AUTO and existing device is INT then replace
                    //'Onboard or External, External first'
                    #if defined(DISPLAY_WITH_FIXED_SLOT) && (DISPLAY_WITH_FIXED_SLOT == 1)
                      if(SetupData.OnExtVgaSelect == 0)
                      {
                          if(Slot == SetupData.VgaSlotNum)
                          {
                              SelectedVGA = dev;
                          }
                        #else
                      //if((SetupData.OnExtVgaSelect == 0) && (SelectedVGA->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)){
                          if(SetupData.OnExtVgaSelect == 0){
                          //SelectedVGA = dev;
                          DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function, dev->Attrib));
                          break;
                      }
                          
                    #endif
                      //'Onboard and External, External first'
                    #if defined(DISPLAY_WITH_FIXED_SLOT) && (DISPLAY_WITH_FIXED_SLOT == 1)
                      if(SetupData.OnExtVgaSelect == 2){
                          //if(((LegacyVgaBDF & 1) == 1) || (((LegacyVgaBDF & 1) == 0) && (Slot == SetupData.VgaSlotNum)))
                          //if(Slot == SetupData.VgaSlotNum){
                          if((count == 1) && (dev->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE))//just have Onboard
                          {
                              SelectedVGA = OnboardVGA;
                          }else{
                              SelectedVGA = dev;
                          }
                          DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function, dev->Attrib));
                          DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", SelectedVGA->Address.Addr.Bus, SelectedVGA->Address.Addr.Device, SelectedVGA->Address.Addr.Function, SelectedVGA->Attrib));
                          //}
                    #else
                      //if((SetupData.OnExtVgaSelect == 2) && (SelectedVGA->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE)){
                          //SelectedVGA = dev;
                          if(SetupData.OnExtVgaSelect == 2){                                                    
                             if((count == 1) && (dev->Attrib == EFI_PCI_IO_ATTRIBUTE_EMBEDDED_DEVICE))//just have Onboard
                             {
                                SelectedVGA = OnboardVGA;
                             }else{
                                SelectedVGA = dev;
                                DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function, dev->Attrib));
                                DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", SelectedVGA->Address.Addr.Bus, SelectedVGA->Address.Addr.Device, SelectedVGA->Address.Addr.Function, SelectedVGA->Attrib));
                                break;
                             }
                             DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", dev->Address.Addr.Bus, dev->Address.Addr.Device, dev->Address.Addr.Function, dev->Attrib));
                             DEBUG((DEBUG_ERROR, "B:%x|D:%d|F:%d Attribute %lx\n", SelectedVGA->Address.Addr.Bus, SelectedVGA->Address.Addr.Device, SelectedVGA->Address.Addr.Function, SelectedVGA->Attrib));
                             //break;
                    #endif
                      }
                  }
              } // else existing device
              NodeEntry = GetNextNode (&gVgaList, NodeEntry);
          }
          
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
          if (SelectedVGA != NULL ) VgaRoute(SelectedVGA);
            break;
      default:
          return EFI_UNSUPPORTED;
    }
    
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformPrepController (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_HANDLE                                     RootBridge,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{   
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetPlatformPolicy (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL         *This,
  OUT EFI_PCI_PLATFORM_POLICY           *PciPolicy
  )
{   
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__)); 
    return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
GetPciRom (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL   *This,
  IN  EFI_HANDLE                        PciHandle,
  OUT VOID                              **RomImage,
  OUT UINTN                             *RomSize
  )
{   
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__)); 
    return EFI_UNSUPPORTED;
}

EFI_STATUS
RegisterPciPlatformProtocol(
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                      Status;
    DEBUG((DEBUG_ERROR, "Installing PciPlatformProtocol\n"));
    
    ZeroMem (&gVgaList, sizeof(EFI_LIST_ENTRY) );
    InitializeListHead (&gVgaList);

    gPciPlatformProtocol.GetPciRom = GetPciRom;
    gPciPlatformProtocol.GetPlatformPolicy = GetPlatformPolicy;
    gPciPlatformProtocol.PlatformNotify = PhaseNotify;
    gPciPlatformProtocol.PlatformPrepController = PlatformPrepController;
    
    Status = gBS->InstallProtocolInterface(
                                    &ImageHandle,
                                    &gEfiPciPlatformProtocolGuid,
                                    EFI_NATIVE_INTERFACE,
                                    &gPciPlatformProtocol);
    
    return Status;
}
