//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiPciEnumeration.c
    This file contains the code to implement
    AMI_PEI_PCI_ENUMERATION_PPI for doing PCI bridge Enumeration.

**/

#include "PeiPciEnumeration.h"
#if defined(PeiDevDefinition_SUPPORT) && (PeiDevDefinition_SUPPORT == 1)
#include "PeiPciEnumHook.h" //<Machong-20170722-add hook definition>
#endif
#define DEVICE_ID_NOCARE        0xFFFF

static
EFI_PEI_NOTIFY_DESCRIPTOR ANotifyList = {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    PeiPciNotifyOnEndOfPei
};

/**
 * Entry Point for PeiPciEnumeration PEIM, Allocates memory for the 
 * PEI_PCI_ENUMERATION_PRIVATE_DATA and Installs the AMI_PEI_PCI_ENUMERATION_PPI.
 * 
 * @param    FileHandle
 * @param    PeiServices
 * 
 * @return   EFI_STATUS
 */

EFI_STATUS
EFIAPI
PeiPciEnumerationEntry(
    IN  EFI_PEI_FILE_HANDLE     FileHandle,
    IN  CONST EFI_PEI_SERVICES  **PeiServices
)
{
    EFI_STATUS                          Status = EFI_SUCCESS;
    EFI_PEI_PPI_DESCRIPTOR              *PeiPciEnumerationDescriptor;
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData; 
    
    PeiPciEnumerationDescriptor = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
    PeiPciPrivateData = (PEI_PCI_ENUMERATION_PRIVATE_DATA *) AllocateZeroPool (sizeof (PEI_PCI_ENUMERATION_PRIVATE_DATA));
    
    PeiPciPrivateData->PeiPciEnumerationPpi.InitDevices = InitializePciDevices;
    PeiPciPrivateData->PeiPciEnumerationPpi.GetNextDeviceByClassCode = GetNextPciDeviceByClassCode;
    PeiPciPrivateData->PeiPciEnumerationPpi.ResetDeviceResources = ResetPciDeviceResources;
    
    PeiPciPrivateData->AssignMmioTopDown = FALSE;
    PeiPciPrivateData->IoBaseAddress = 0;
    PeiPciPrivateData->MmioBaseAddress = 0;
    PeiPciPrivateData->LastUsedBusNumber = 0;
    PeiPciPrivateData->SubBusNoForRootBridge = PEI_PCI_MAX_SUBORDINATE_NUMBER;
    
    PeiPciEnumerationDescriptor->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
    PeiPciEnumerationDescriptor->Guid = &gAmiPeiPciEnumerationPpiGuid;
    PeiPciEnumerationDescriptor->Ppi = &PeiPciPrivateData->PeiPciEnumerationPpi;
    Status = (*PeiServices)->InstallPpi(PeiServices, PeiPciEnumerationDescriptor);

    PEI_PCI_DEBUG((EFI_D_INFO, "PeiPciEnumeration Entry: Installation of AMI_PEI_PCI_ENUMERATION_PPI Status:%r\n", Status));
    return Status;
}

/**
    Routine to Get or Set the Root Bridge Resources from the PCD's
    AmiPcdPeiPciMmioTopAddress and AmiPcdPeiPciEnumerationLastBusNumber

    @param  Flag

    @retval NONE
**/

VOID
PcdGetSetPciBridgeResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA        *PeiPciPrivateData,
    BOOLEAN Flag
) 
{
    UINT64          PciMmioStartAddress = 0 ;
    if(Flag) {
        PcdSet8(AmiPcdPeiPciEnumerationLastBusNumber,PeiPciPrivateData->LastUsedBusNumber);
        PcdSet64(AmiPcdPeiPciMmioTopAddress,PeiPciPrivateData->MmioBaseAddress);
    } else {
        if(!PcdGet64(AmiPcdPeiPciMmioTopAddress) ) {
            FindPciMmioAddress(&PciMmioStartAddress);
            PcdSet64( AmiPcdPeiPciMmioTopAddress, PciMmioStartAddress);
        }
        PeiPciPrivateData->IoBaseAddress = PCI_IO_MAP_BASE_ADDRESS;
        PeiPciPrivateData->MmioBaseAddress = (UINT32) PcdGet64(AmiPcdPeiPciMmioTopAddress);
        PeiPciPrivateData->LastUsedBusNumber = PcdGet8(AmiPcdPeiPciEnumerationLastBusNumber);
        if( (PeiPciPrivateData->MmioBaseAddress) <=  PcdGet64(PcdPciExpressBaseAddress)) {
//<Kangmm-20190223-002 Fast Video not work on Rome+>
          PeiPciPrivateData->AssignMmioTopDown = TRUE;
//<Kangmm-20190223-002 Fast Video not work on Rome->
        }
    }
}

/**
    Routine to find the start of the MMIO address that can be assigned for the 
    PCI device in Pei Phase.

    @param  PciMmioStartAddress

    @retval NONE
        
    @notes 1) Initially consider the MMIO region below the PCIE_BASE_ADDRESS.
    This routine will check Resource descriptor HOB and finds 
    if the selected MMIO region falls in Resource descriptor Hob's memory range.
    2) If the selected region is in Hob's memory range, means its reserved memory
    and can't be configured used as MMIO so select MMIO from PCIE_BASE_ADDRESS + PCIE_LENGTH. 
**/

VOID
FindPciMmioAddress(
    UINT64              *PciMmioStartAddress
) 
{
    EFI_PEI_HOB_POINTERS                          Hob;
    EFI_HOB_RESOURCE_DESCRIPTOR                   *ResourceHob;
    EFI_PHYSICAL_ADDRESS                          MmioBaseAddress;
        
    *PciMmioStartAddress = PcdGet64(PcdPciExpressBaseAddress);
    // Try to find the Memory region that can be used as MMIO for PCI Devices. 
    // Check Resource descriptor HOB to find whether the MMIO selected
    // is being used as Reserved memory and System memory.
    MmioBaseAddress = *PciMmioStartAddress - SIZE_1MB;
    Hob.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);

    while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
        ResourceHob = Hob.ResourceDescriptor;
        // check whether the selected MMIO falls within Resource descriptor memory range.
        PEI_PCI_DEBUG((EFI_D_INFO,"Found Resource Hob Memory from Start:%lx to End:%lx \n",
                                               ResourceHob->PhysicalStart,
                                               ResourceHob->PhysicalStart + ResourceHob->ResourceLength));
        if ( ResourceHob->PhysicalStart <= MmioBaseAddress &&
                    ResourceHob->PhysicalStart + ResourceHob->ResourceLength >= MmioBaseAddress ) {

            PEI_PCI_DEBUG((EFI_D_INFO,"PcdPciExpressBaseAddress below region can't be used as MMIO, its reserved Memory!!\n"));
            PEI_PCI_DEBUG((EFI_D_INFO,"\n So using the memory above PciExpressBaseAddress + PCIE_LENGTH for PCI Mmio."));

            *PciMmioStartAddress = PcdGet64(PcdPciExpressBaseAddress) + PCIEX_LENGTH;
            break;
        }
        Hob.Raw = GET_NEXT_HOB (Hob);
        Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw);
    }
    return; 
}

/** 
  Performs Partial Enumeration and initializes the devices based on the Root Bridge and Class code
  specified through the ELink's PEI_PCI_ROOT_BRIDGE_LIST and PEI_PCI_CLASSCODE_LIST.
  
  Follows the Below Steps:
  ------------------------
  1. First Discovers the Devices under Root Bridge or any particular Pci to Pci bridge and 
     store the location information in the PEI_PCI_DEVICE_INFO Structure.
  2. Calculates the Resources required by each Bridge and stores the info in the PEI_PCI_BRIDGE_INFO_EXT
  3. Assign the resources to each Pci device.
  4. Updates the Subordinate Bus Number of the Pci to Pci Bridge.
  
  @param[in]  This                  AMI_PEI_PCI_ENUMERATION_PPI instance. 
  @retval EFI_SUCCESS               The operation succeeded. A list of Pci Devices enumerated
                                    is returned
  @retval EFI_INVALID_PARAMETER     The input parameter is illegal.  
  @retval EFI_OUT_OF_RESOURCES      There is insufficient memory to store the PCI Device Data.
**/

EFI_STATUS
EFIAPI
InitializePciDevices (
    IN  AMI_PEI_PCI_ENUMERATION_PPI     *This
)
{
    EFI_STATUS                              Status;
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;
    //<Machong-20170719-try to make it work in CAR->
    PEI_PCI_ENUMERATION_PRIVATE_DATA        *PeiPciPrivateData= \
                                                (PEI_PCI_ENUMERATION_PRIVATE_DATA*)This;
    UINT8                                   Index, Index1;
    UINT16                                  HeaderType = 0;
    UINT16                                  VendorId = 0;
    PEI_PCI_DEV_INFO                        *PciRootBridgeInfo, *PciDeviceInfo;       
    PEI_PCI_BRIDGE_INFO_EXT                 *PciBridgeInfoExt, *PciRootBridgeInfoExt;
    PEI_PCI_BRIDGE_ENTRY                    PciBridgeList[] = { PEI_PCI_BRIDGE_LIST
                                                                {0xff,0xff,0xff}
                                                              };
    PCI_DEVICE_CLASSCODE_ENTRY              ClassCodeList[] = {PEI_PCI_CLASS_CODE_LIST
                                                                {0xff, 0xff, 0xff, 0xff}
                                                              };
    UINT8                                   TotalPciDevice = 0;
    BOOLEAN                                 BridgeEnumerated = FALSE;
    //<xuyj1-20210413 Mantis:134131 window os device manager is abnormal +>
    UINT8                                   VgaRbNumber=0;
    VgaRbNumber = (UINT8)PcdGet8(AmiPcdPeiPciEnumerationLastBusNumber);
    //<xuyj1-20210413 Mantis:134131 window os device manager is abnormal ->
    //<Machong-20170719-try to make it work in CAR+>
    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR->    
    
    // Check whether Enumeration is done already or not
    if(PeiPciPrivateData->HaveEnumeratedDevices ) {
        DEBUG((EFI_D_ERROR, "[OemDbg]: Init VGA Device Fail,Please Double Check It!!!\n\n"));
        PEI_PCI_DEBUG((EFI_D_INFO, "PCI Device Enumeration Done Already\n"));
        return EFI_SUCCESS;
    }
    if( PciBridgeList[0].Bus == 0xff && PciBridgeList[0].Device == 0xff &&
            PciBridgeList[0].Function == 0xff ) {

        PEI_PCI_DEBUG((EFI_D_INFO, "\n No PCI Bridge Entry added for device Enumeration in PEI phase\n"));
        return EFI_NOT_FOUND;
    }
    
    if(ClassCodeList[0].BaseClassCode == 0xff && ClassCodeList[0].ProgrammingInterface == 0xff && \
            ClassCodeList[0].SubClassCode == 0xff && ClassCodeList[0].BarIndex == 0xff ) {
        PEI_PCI_DEBUG((EFI_D_INFO, "\n No PCI Class Code Entry added for device Enumeration in PEI phase\n"));
        return EFI_NOT_FOUND;
    }
    // Obtain the PCI Bus and Mmio Resource from PCD's
    PcdGetSetPciBridgeResources(PeiPciPrivateData,FALSE);
   
    // Framing RootBridge Device Info Structure.
    Status = (**PeiServices).AllocatePool(\
                                (CONST EFI_PEI_SERVICES**)PeiServices,
                                (sizeof(PEI_PCI_DEV_INFO) + sizeof(PEI_PCI_BRIDGE_INFO_EXT)),
                                (VOID**)&PciRootBridgeInfo);
        
    if ( EFI_ERROR( Status )) {
        return EFI_OUT_OF_RESOURCES;
    }
    (**PeiServices).SetMem ((VOID*)PciRootBridgeInfo,
                              (sizeof(PEI_PCI_DEV_INFO) + sizeof(PEI_PCI_BRIDGE_INFO_EXT)),
                              0 );
        
    PciRootBridgeInfo->Type = tPciRootBrg;
    PciRootBridgeInfoExt = (PEI_PCI_BRIDGE_INFO_EXT *)(PciRootBridgeInfo+1);
    
    for( Index=0; PciBridgeList[Index].Function != 0xff ; Index++) {
        // First Check whether this bridge is already Enumerated.
        for( Index1 = 0, BridgeEnumerated = FALSE; Index1 < PeiPciPrivateData->PciDeviceCount; Index1++ ) {
            PciDeviceInfo = PeiPciPrivateData->PciDeviceInfo[Index1];
            if( PciDeviceInfo->Bus == PciBridgeList[Index].Bus &&\
                PciDeviceInfo->Device == PciBridgeList[Index].Device && \
                PciDeviceInfo->Function == PciBridgeList[Index].Function ) {
                BridgeEnumerated = TRUE;
                break;
            }
        }
        //<xuyj1-20210413 Mantis:134131 window os device manager is abnormal +>
       // if(BridgeEnumerated) continue;
        if(BridgeEnumerated ||(VgaRbNumber!=PciBridgeList[Index].Bus)) continue;
        //<xuyj1-20210413 Mantis:134131 window os device manager is abnormal ->
        if( PciBridgeList[Index].Device == 0 && \
               PciBridgeList[Index].Function == 0 ) {
            //
            // Add an entry of the PCI Bridge that is being Enumerated.
            //
            PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount] = PciRootBridgeInfo;
            PeiPciPrivateData->PciDeviceCount++;
            
            //Enumerate Root PCI Bus 0.
            Status = EnumerateBus(PeiPciPrivateData, PciRootBridgeInfo);
            PEI_PCI_DEBUG((EFI_D_INFO,"Enumerate Root Bus 0 Status:%r",Status)); 
            if ( EFI_ERROR( Status )) {
                goto EnumerationDone;
            }
        } else {
            
            PEI_PCI_DEBUG((EFI_D_INFO, "Enumerating Bridge @ B[%x] D[%x] F[%x] \n",   
                                                PciBridgeList[Index].Bus,
                                                PciBridgeList[Index].Device, 
                                                PciBridgeList[Index].Function ));

            VendorId = 0;
            //
            // Read PCI vendor ID to probe for the valid PCI device presence.
            //
            Status = PciCfg->Read(PeiServices, \
                                      PciCfg, \
                                      EfiPeiPciCfgWidthUint16,\
                                      PEI_PCI_CFG_ADDRESS(
                                          PciBridgeList[Index].Bus,
                                          PciBridgeList[Index].Device,
                                          PciBridgeList[Index].Function,
                                          PCI_VENDOR_ID_OFFSET),\
                                      &VendorId);

            if(VendorId == DEVICE_ID_NOCARE )  continue;
            
            //<Machong-2017-Try to fix Root Bridge Register+>
            CheckPreEnumerationInitFunctionHook(PciBridgeList[Index].Bus, PciBridgeList[Index].Device, PciBridgeList[Index].Function, VendorId);
            //<Machong-2017-Try to fix Root Bridge Register->
            
            // Check whether the Bridge provided through ELINK is a valid Pci2PciBridge
            // by reading the Header type from PciConfiguration Space.
            
            PciCfg->Read(PeiServices, \
                          PciCfg, \
                          EfiPeiPciCfgWidthUint8,\
                          PEI_PCI_CFG_ADDRESS(
                              PciBridgeList[Index].Bus,
                              PciBridgeList[Index].Device,
                              PciBridgeList[Index].Function,
                              PCI_HEADER_TYPE_OFFSET),\
                          &HeaderType);

            // HeaderType's Bit 0 and Bit 1 will indicate the type of the 
            // PCI Device
            HeaderType &= 0x03;
            if(HeaderType != HEADER_TYPE_PCI_TO_PCI_BRIDGE) continue;
            PeiPciPrivateData->LastUsedBusNumber++;
            //
            // Store the PCI to PCI Bridge Device info.
            // Since its a PCI bridge Allocate memory for PEI_PCI_BRIDGE_INFO_EXT
            // followed by PEI_PCI_DEV_INFO.
            //
            Status = (**PeiServices).AllocatePool( \
                         (CONST EFI_PEI_SERVICES**)PeiServices,
                         (sizeof(PEI_PCI_DEV_INFO)+ sizeof(PEI_PCI_BRIDGE_INFO_EXT)),
                         (VOID**)&PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount]);
            if ( EFI_ERROR( Status )) {
                goto EnumerationDone;
            }
            
            PciDeviceInfo = PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount];
            (**PeiServices).SetMem ((VOID*)PciDeviceInfo,
                                   (sizeof(PEI_PCI_DEV_INFO) + sizeof(PEI_PCI_BRIDGE_INFO_EXT)),
                                   0 );
            PciDeviceInfo->Type = tPci2PciBrg;
            PciDeviceInfo->Bus = PciBridgeList[Index].Bus;
            PciDeviceInfo->Device = PciBridgeList[Index].Device;
            PciDeviceInfo->Function = PciBridgeList[Index].Function;
            PciDeviceInfo->ParentBridge = PciRootBridgeInfo;
            // Framing Extended Info In case of  a PCI to PCI Bridge 
            PciBridgeInfoExt = (PEI_PCI_BRIDGE_INFO_EXT*)(PciDeviceInfo+1);
            PciBridgeInfoExt->PrimaryBusNumber = PciBridgeList[Index].Bus;
            PciBridgeInfoExt->SecondaryBusNumber = PeiPciPrivateData->LastUsedBusNumber;
            
            // Assign Subordinate Bus number with the max value,
            // Subordinate Bus Number of the bridge will be later updated
            // if any bridge is found under this P2P bridge.
            
           //<Kangmm-20190223-002 Fast Video not work on Rome+>
            PciBridgeInfoExt->SubordinateBusNumber = PciBridgeInfoExt->SecondaryBusNumber + 1;
           //<Kangmm-20190223-002 Fast Video not work on Rome->
            
            //
            // Program the Secondary & Subordinate Bus no. for Root Bridge.
            //
            // 
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciBridgeList[Index].Bus,
                                    PciBridgeList[Index].Device, 
                                    PciBridgeList[Index].Function, 
                                    PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),\
                                &PciBridgeInfoExt->SecondaryBusNumber);
            
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciBridgeList[Index].Bus,
                                    PciBridgeList[Index].Device,
                                    PciBridgeList[Index].Function, 
                                    PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                &PciBridgeInfoExt->SubordinateBusNumber);
            
            PeiPciPrivateData->PciDeviceCount++;
            //
            // Enumerate the device's under this Bridge.
            //
            Status = EnumerateBus(PeiPciPrivateData,PciDeviceInfo);
            PEI_PCI_DEBUG((EFI_D_INFO,"Enumeration of PCI/PCI Bridge @B[%X]|D[%X]|F[%X] Status:%r\n",
                                             PciBridgeList[Index].Bus, 
                                             PciBridgeList[Index].Device,
                                             PciBridgeList[Index].Function, Status)); 
            if ( EFI_ERROR( Status )) {
                goto EnumerationDone;
            }
            //
            // Current PCI/PCI bridge enumeration is done now Set the SubBus Number for this 
            // Bridge with the  highest numbered Pci bus segment which is behind (subordinate to)
            // the bridge.
            //
            
            PciBridgeInfoExt->SubordinateBusNumber = PeiPciPrivateData->LastUsedBusNumber;
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciBridgeList[Index].Bus,
                                    PciBridgeList[Index].Device,
                                    PciBridgeList[Index].Function,   
                                    PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                &PciBridgeInfoExt->SubordinateBusNumber);
        } // else
    }// for
    
    // Register a Notification function that frees Up the resources allocated for PCI to PCI Bridge
    // and the PCI Controllers at the End of PEI phase
    (**PeiServices).NotifyPpi (PeiServices, &ANotifyList);
    // Check whether any Pci Device is found
    for(Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
        PciDeviceInfo =  PeiPciPrivateData->PciDeviceInfo[Index];
        if(PciDeviceInfo->Type == tPciDevice) {
            TotalPciDevice++;
        }
    }
    PEI_PCI_DEBUG((EFI_D_INFO, "PEI PCI:Total PCI Controller Found:%d\n",TotalPciDevice));
    if ( !TotalPciDevice )  {
        // We didn't find any PCI Target device to proceed further so returning from here
        Status = EFI_NOT_FOUND;
        goto EnumerationDone;
    }
    // We discovered PCI devices, now start calculating Resource requirement and 
    // assign resources to the devices.
    PEI_PCI_DEBUG((EFI_D_INFO,"---------------RESOURCE CALCULATION STARTS----------------\n"));
    Status = CalculateBridgeResources( PeiPciPrivateData, PciRootBridgeInfo);
    PEI_PCI_DEBUG((EFI_D_INFO, "PEI PCI:CalculateBridgeResources Status:%r\n\n",Status));
    PEI_PCI_DEBUG((EFI_D_INFO,"---------------RESOURCE CALCULATION ENDS------------------\n\n"));
    
    if ( EFI_ERROR( Status )) {
        goto EnumerationDone;
    }
    PciRootBridgeInfoExt->IoBase = PeiPciPrivateData->IoBaseAddress;
    PciRootBridgeInfoExt->MmioBase = (UINT32) PeiPciPrivateData->MmioBaseAddress;
    PEI_PCI_DEBUG((EFI_D_INFO,"--------------RESOURCE ASSIGNMENT STARTS------------------\n"));
    Status = AssignBridgeResources( PeiPciPrivateData, PciRootBridgeInfo);
    PEI_PCI_DEBUG((EFI_D_INFO, "PEI PCI:AssignBridgeResources Status:%r\n",Status));
    PEI_PCI_DEBUG((EFI_D_INFO,"--------------RESOURCE ASSIGNMENT ENDS--------------------\n"));
    
EnumerationDone:
    PcdGetSetPciBridgeResources(PeiPciPrivateData, TRUE);
    PeiPciPrivateData->HaveEnumeratedDevices = TRUE;
    return Status;
}
/**
    Assigns the Io and Mmio type Resources for all the devices under Parent bridge.And programs
    the Base and limit registers of the Bridge based on the Resource required for the devices
    under the bridge.This function will be called recursively to handle bridge under bridge 
    case.

    @param[in]  PeiPciPrivateData       Pointer to the PEI_PCI_ENUMERATION_PRIVATE_DATA
    @param[in]  ParentBridge            Pointer to the parent bridge PCI_DEVICE_INFO
    
    @retval EFI_SUCCESS - Assigned Resources to the given Input Bridge
**/
EFI_STATUS 
AssignBridgeResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge
)
{
    UINT8                                   Index;
    UINT16                                  BridgeBase,BridgeLimit = 0;
    UINT8                                   CommandRegister = 0;
    PEI_PCI_DEV_INFO                        *PciDeviceInfo = NULL;
    PEI_PCI_BRIDGE_INFO_EXT                 *ChildBridgeExtInfo = NULL;
    PEI_PCI_BRIDGE_INFO_EXT                 *ParentBridgeExtInfo =
                                                (PEI_PCI_BRIDGE_INFO_EXT*)(ParentBridge+1);
    UINT64                                  ChildBridgeMmioLength = 0;
    UINT32                                  ChildBridgeIoLength = 0;
    
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;
    //<Machong-20170719-try to make it work in CAR->
    
    //<Machong-20170719-try to make it work in CAR+>
    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR->  

    // First Program IO and Mmio Resource for the PCI to PCI Bridge
    for(Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
        PciDeviceInfo =  PeiPciPrivateData->PciDeviceInfo[Index];
        // Check whether this belongs to the ParentBridge
        if(PciDeviceInfo->ParentBridge != ParentBridge) continue;
        if(PciDeviceInfo->Type == tPci2PciBrg) {
            ChildBridgeExtInfo = (PEI_PCI_BRIDGE_INFO_EXT*)(PciDeviceInfo+1);
            if(ChildBridgeExtInfo->BridgeProgrammed) continue;
            
            // Read Command Register for Enabling Bridge I/O or Memory Decoding
            PciCfg->Read(PeiServices, \
                              PciCfg, \
                              EfiPeiPciCfgWidthUint8,\
                              PEI_PCI_CFG_ADDRESS (
                                  PciDeviceInfo->Bus,
                                  PciDeviceInfo->Device, 
                                  PciDeviceInfo->Function,
                                  PCI_COMMAND_OFFSET),\
                              &CommandRegister);
            if(ChildBridgeExtInfo->IoBaseLength) {
                ChildBridgeExtInfo->BridgeProgrammed = TRUE;
                ChildBridgeExtInfo->IoBase = PeiPciPrivateData->IoBaseAddress;
                BridgeBase = ChildBridgeExtInfo->IoBase >> 8;
                BridgeLimit = (ChildBridgeExtInfo->IoBase + \
                                    (ChildBridgeExtInfo->IoBaseLength - 1)) >> 8;
                
                //Enabling Bridge IO Decoding.
                CommandRegister |= (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_IO_SPACE);
                PciCfg->Write (PeiServices, \
                                    PciCfg, \
                                    EfiPeiPciCfgWidthUint8, \
                                    PEI_PCI_CFG_ADDRESS ( 
                                        PciDeviceInfo->Bus,
                                        PciDeviceInfo->Device,
                                        PciDeviceInfo->Function, 
                                        PCI_IOBASE),\
                                    (UINT8*)&BridgeBase);
                PciCfg->Write (PeiServices, \
                                    PciCfg, \
                                    EfiPeiPciCfgWidthUint8, \
                                    PEI_PCI_CFG_ADDRESS (
                                        PciDeviceInfo->Bus,
                                        PciDeviceInfo->Device, 
                                        PciDeviceInfo->Function, 
                                        PCI_IOLIMIT),\
                                    (UINT8 *)&BridgeLimit);
               PEI_PCI_DEBUG((EFI_D_INFO,"----------------------------------------------------\n"));
               PEI_PCI_DEBUG((EFI_D_INFO," Assigned Resource to the Bridge @B[%x]|D[%x]|F[%x]\n",
                                                      PciDeviceInfo->Bus,
                                                      PciDeviceInfo->Device,
                                                      PciDeviceInfo->Function));
               PEI_PCI_DEBUG((EFI_D_INFO, "Bridge IoBase:%x,IoLimit:%x \n", BridgeBase, BridgeLimit));
           }
           if(ChildBridgeExtInfo->MmioBaseLength) {
                ChildBridgeExtInfo->BridgeProgrammed = TRUE;
              
                ChildBridgeExtInfo->MmioBase = PeiPciPrivateData->MmioBaseAddress;
                
                if(PeiPciPrivateData->AssignMmioTopDown) {
                    ChildBridgeExtInfo->MmioBase = PeiPciPrivateData->MmioBaseAddress - ChildBridgeExtInfo->MmioBaseLength;
                    //<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
                    VgaMemoryAlignFuncHook(PciDeviceInfo->Bus,PciDeviceInfo->Device,PciDeviceInfo->Function, &ChildBridgeExtInfo->MmioBase);
                    //<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->
                } 
                BridgeBase = (UINT16)RShiftU64 (ChildBridgeExtInfo->MmioBase ,16);
                BridgeLimit = (UINT16)RShiftU64 (ChildBridgeExtInfo->MmioBase + \
                                      (ChildBridgeExtInfo->MmioBaseLength - 1),16);
                
                //Enabling Bridge Memory Decoding.
                CommandRegister |= (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE);
                PciCfg->Write (PeiServices, \
                                    PciCfg, \
                                    EfiPeiPciCfgWidthUint16, \
                                    PEI_PCI_CFG_ADDRESS (
                                        PciDeviceInfo->Bus,
                                        PciDeviceInfo->Device, 
                                        PciDeviceInfo->Function, 
                                        PCI_MEMBASE),\
                                    &BridgeBase);
                PciCfg->Write (PeiServices, \
                                    PciCfg, \
                                    EfiPeiPciCfgWidthUint16, \
                                    PEI_PCI_CFG_ADDRESS (
                                        PciDeviceInfo->Bus,
                                        PciDeviceInfo->Device, 
                                        PciDeviceInfo->Function, 
                                        PCI_MEMLIMIT),\
                                    &BridgeLimit);
                PEI_PCI_DEBUG((EFI_D_INFO,"---------------------------------------------------\n"));
                PEI_PCI_DEBUG((EFI_D_INFO, "Assigned Resource to the Bridge @B[%x]|D[%x]|F[%x]\n",PciDeviceInfo->Bus, PciDeviceInfo->Device, PciDeviceInfo->Function));
                PEI_PCI_DEBUG((EFI_D_INFO, "Bridge MemBase:%x,MemLimit:%x\n", BridgeBase, BridgeLimit));
            }
            PciCfg->Write(PeiServices, \
                              PciCfg, \
                              EfiPeiPciCfgWidthUint8,\
                              PEI_PCI_CFG_ADDRESS (
                                  PciDeviceInfo->Bus,
                                  PciDeviceInfo->Device, 
                                  PciDeviceInfo->Function,
                                  PCI_COMMAND_OFFSET),\
                              &CommandRegister);
            // Now assign Resources for the PCI to PCI Bridge behind this bridge.
            AssignBridgeResources(PeiPciPrivateData,PciDeviceInfo);
            FindChildBridgesLength(PeiPciPrivateData,
                                    PciDeviceInfo,
                                    &ChildBridgeIoLength, 
                                    &ChildBridgeMmioLength);
            //Align IoBase and MmioBase for the next devices under this bridge device.
            PeiPciPrivateData->IoBaseAddress += ChildBridgeIoLength;
            if(PeiPciPrivateData->AssignMmioTopDown) {
              PeiPciPrivateData->MmioBaseAddress -= (ChildBridgeExtInfo->MmioBaseLength - ChildBridgeMmioLength);
              //<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
              VgaMemoryAlignFuncHook(PciDeviceInfo->Bus,PciDeviceInfo->Device,PciDeviceInfo->Function, &PeiPciPrivateData->MmioBaseAddress);
              //<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->
            } else {
              PeiPciPrivateData->MmioBaseAddress += ChildBridgeMmioLength;
            }
            if(ChildBridgeExtInfo->IoBaseLength) {
                // Bridge Base and Limit registers are programmed 
                // Now start assigning resources to the devices under it
                PEI_PCI_DEBUG((EFI_D_INFO,"Assigning IO Resources "));
                AssignDeviceResources(PeiPciPrivateData, PciDeviceInfo, tBarIo);
                PeiPciPrivateData->IoBaseAddress += (ChildBridgeExtInfo->IoBaseLength - ChildBridgeIoLength);
            }
            if(ChildBridgeExtInfo->MmioBaseLength) {
                PEI_PCI_DEBUG((EFI_D_INFO, "Assigning MmiO Resources ")); 
                AssignDeviceResources(PeiPciPrivateData, PciDeviceInfo, tBarMem);
                if(!PeiPciPrivateData->AssignMmioTopDown) {
                  PeiPciPrivateData->MmioBaseAddress += (ChildBridgeExtInfo->MmioBaseLength - ChildBridgeMmioLength);
                }
            }
         } 
    } // For Loop
    
    // By this stage all the PCI to PCI bridge devices are assigned with resources
    // start handling the On board Devices
    if(ParentBridge->Type == tPciRootBrg && ParentBridgeExtInfo->ChildDeviceCount ) {
        ParentBridgeExtInfo->IoBase = PeiPciPrivateData->IoBaseAddress;
        ParentBridgeExtInfo->MmioBase = PeiPciPrivateData->MmioBaseAddress;
        // Since in PEI phase we have Memory resource limitation till 4GB
        // A PCI Device that needs 64 bit bar address also will be assigned 
        // and 32 bit address.
        PEI_PCI_DEBUG((EFI_D_INFO,"Assigning IO Resources "));
        AssignDeviceResources(PeiPciPrivateData, ParentBridge, tBarIo);
        
        PEI_PCI_DEBUG((EFI_D_INFO, "Assigning MMIO Resources ")); 
        AssignDeviceResources(PeiPciPrivateData, ParentBridge, tBarMem);
    } 
    return EFI_SUCCESS;
}

/**
    Calculates and returns the I/O or Mmio Resource length used by child Bridges.

    @param[in]  PeiPciPrivateData       Pointer to the PEI_PCI_ENUMERATION_PRIVATE_DATA
    @param[in]  ParentBridge            Pointer to the parent bridge PCI_DEVICE_INFO
    @param[in]  ChildBridgeIoLength     Pointer to the Memory location that holds Io Length.
    @param[in]  ChildBridgeMmioLength   Pointer to the Memory location that holds Mmio Length.
    
**/

VOID 
FindChildBridgesLength (
  PEI_PCI_ENUMERATION_PRIVATE_DATA     *PeiPciPrivateData,
  PEI_PCI_DEV_INFO                     *ParentBridge,
  UINT32                               *ChildBridgeIoLength, 
  UINT64                               *ChildBridgeMmioLength
)
{
    UINT8                                   Index;
    PEI_PCI_DEV_INFO                        *PciDeviceInfo = NULL;
    PEI_PCI_BRIDGE_INFO_EXT                 *ChildBridgeExtInfo = NULL;
    
    for(Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
         PciDeviceInfo =  PeiPciPrivateData->PciDeviceInfo[Index];
         // Check whether this device belongs to the ParentBridge
         if(PciDeviceInfo->ParentBridge != ParentBridge) continue;
         if(PciDeviceInfo->Type == tPci2PciBrg) {
             ChildBridgeExtInfo = (PEI_PCI_BRIDGE_INFO_EXT*)(PciDeviceInfo+1);
             if(ChildBridgeExtInfo->IoBaseLength) {
                 *ChildBridgeIoLength += ChildBridgeExtInfo->IoBaseLength;
             } else if (ChildBridgeExtInfo->MmioBaseLength ){
                 *ChildBridgeMmioLength += ChildBridgeExtInfo->MmioBaseLength;
             }
         }
    }
    return;
}
/**
    Assigns the Resources to the Pci Devices under the give Input Parent Bridge
    one by one based on the Granularity.The device with MaxGranularity will be 
    assigned Resource first.

    @param[in]  PeiPciPrivateData       Pointer to the PEI_PCI_ENUMERATION_PRIVATE_DATA
    @param[in]  ParentBridge            Pointer to the parent bridge PCI_DEVICE_INFO
    @param[in]  ResourceType            Resource Type either tBarIo or tBarMem
    
    @retval EFI_SUCCESS - Assigned Resources to the devices under the given Input Bridge.
**/

EFI_STATUS 
AssignDeviceResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge,
    PEI_PCI_BAR_TYPE                    ResourceType
)
{
    EFI_STATUS                              Status;
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;
    //<Machong-20170719-try to make it work in CAR->
    UINT8                                   CommandRegister = 0;
    PEI_PCI_DEV_INFO                        *PciDeviceInfo = NULL;
    PEI_PCI_BRIDGE_INFO_EXT                 *ParentBridgeExtInfo =
                                                        (PEI_PCI_BRIDGE_INFO_EXT*)(ParentBridge+1);
    UINT64                                  MaxGranularity = 0;
    UINT8                                   BridgeChildDevCount = 0;
    UINT64                                  BaseAddress = 0;
    UINT8                                   CmdRegMask;
    
    //<Machong-20170719-try to make it work in CAR+>
    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR-> 
    

    PEI_PCI_DEBUG((EFI_D_INFO, "for the Devices Under bridge @B[%x|D[%x|F[%x]\n", \
                                                  ParentBridge->Bus, ParentBridge->Device, ParentBridge->Function));
    PEI_PCI_DEBUG((EFI_D_INFO,"-----------------------------------------------------------------------\n"));
    BridgeChildDevCount = ParentBridgeExtInfo->ChildDeviceCount;
    if( ResourceType == tBarIo ) {
        BaseAddress = (UINT64) PeiPciPrivateData->IoBaseAddress;
        CmdRegMask = (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_IO_SPACE);
    }
    if(ResourceType == tBarMem) {
        BaseAddress = PeiPciPrivateData->MmioBaseAddress;
        CmdRegMask = (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE);
    }
    while(BridgeChildDevCount) {
        // Get the first maximum granularity device under this bridge and 
        // program the BAR address
        Status = GetNextMaxGranDevUnderBridge(PeiPciPrivateData,
                                               ParentBridge,
                                               ResourceType,
                                               &MaxGranularity,
                                               &PciDeviceInfo);
        if(EFI_ERROR(Status) || !PciDeviceInfo || !MaxGranularity) {
            return EFI_NOT_FOUND;
        }
        PEI_PCI_DEBUG((EFI_D_INFO, "    Assigned Bar for Device at B[%x]|D[%x]|F[%X] ",\
                    PciDeviceInfo->Bus, PciDeviceInfo->Device, PciDeviceInfo->Function ));

        // Program Bar for this Device and Enable Memory Decoding
        if( PeiPciPrivateData->AssignMmioTopDown && (PciDeviceInfo->Bus == 0)) {
            PciDeviceInfo->Bar.Base = BaseAddress - PciDeviceInfo->Bar.Length;
            BaseAddress -= PciDeviceInfo->Bar.Length;
        } else {
            PciDeviceInfo->Bar.Base = BaseAddress;
            BaseAddress += PciDeviceInfo->Bar.Length;
        }
        //Programming Bar of the PciDevice
        if(PciDeviceInfo->Bar.Type == tBarIo || PciDeviceInfo->Bar.Type == tBarMmio32 ) {
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint32, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciDeviceInfo->Bus,
                                    PciDeviceInfo->Device, 
                                    PciDeviceInfo->Function, 
                                    PciDeviceInfo->Bar.Offset),\
                                 (UINT32*)&PciDeviceInfo->Bar.Base);
                
         } else if (PciDeviceInfo->Bar.Type == tBarMmio64) {
             PciCfg->Write (PeiServices, \
                                    PciCfg, \
                                    EfiPeiPciCfgWidthUint64, \
                                    PEI_PCI_CFG_ADDRESS (
                                        PciDeviceInfo->Bus,
                                        PciDeviceInfo->Device, 
                                        PciDeviceInfo->Function, 
                                        PciDeviceInfo->Bar.Offset),\
                                    &PciDeviceInfo->Bar.Base);
         }
         PciDeviceInfo->IsBarProgrammed = TRUE;
         PEI_PCI_DEBUG((EFI_D_INFO,":%lx ,Length :%lx\n",PciDeviceInfo->Bar.Base, PciDeviceInfo->Bar.Length));
         BridgeChildDevCount--;
         //Enable Memory or IO Space Decoding for the device.
         PciCfg->Read (PeiServices, \
                             PciCfg, \
                             EfiPeiPciCfgWidthUint8,\
                             PEI_PCI_CFG_ADDRESS (
                                 PciDeviceInfo->Bus,
                                 PciDeviceInfo->Device, 
                                 PciDeviceInfo->Function,
                                 PCI_COMMAND_OFFSET),\
                             &CommandRegister);
         CommandRegister |= CmdRegMask;
         PciCfg->Write(PeiServices, \
                             PciCfg, \
                             EfiPeiPciCfgWidthUint8,\
                             PEI_PCI_CFG_ADDRESS (
                                   PciDeviceInfo->Bus,
                                   PciDeviceInfo->Device, 
                                   PciDeviceInfo->Function,
                                   PCI_COMMAND_OFFSET),\
                               &CommandRegister);
    } 
    PEI_PCI_DEBUG((EFI_D_INFO,"-----------------------------------------------------------------------\n"));
    return EFI_SUCCESS;
}

/**
    This will finds the UnProgrammed Pci Device with the next maximum granularity under
    the given Input Parent Bridge.

    @param[in]  PeiPciPrivateData       Pointer to the PEI_PCI_ENUMERATION_PRIVATE_DATA
    @param[in]  ParentBridge            Pointer to the parent bridge PCI_DEVICE_INFO
    @param[in]  ResourceType            Resource Type either tBarIo or tBarMem
    @param[in]  MaxGranularity          Maximum Granularity
    @param[in]  MaxGranPciDeviceInfo    Pci Device Info with Max Granularity
    
    @retval EFI_SUCCESS - Calculated Resources successfully.
**/

EFI_STATUS 
GetNextMaxGranDevUnderBridge(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge,
    PEI_PCI_BAR_TYPE                    ResourceType,
    UINT64                              *MaxGranularity,
    PEI_PCI_DEV_INFO                    **MaxGranPciDeviceInfo
) 
{
    UINT8                               Index;
    PEI_PCI_DEV_INFO                    *PciDeviceInfo = NULL;
    
    // Set Initial value to zero
    *MaxGranularity = 0;
    // Calculate maximum granularity for the devices under this  Bridge..
    for(Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
        PciDeviceInfo =  PeiPciPrivateData->PciDeviceInfo[Index];
        if(PciDeviceInfo->ParentBridge != ParentBridge) continue;
        if(PciDeviceInfo->Type == tPciDevice && !PciDeviceInfo->IsBarProgrammed) {
            if(ResourceType == tBarMem) {
                if( PciDeviceInfo->Bar.Type != tBarMmio32 && PciDeviceInfo->Bar.Type != tBarMmio64)
                    continue;
            } else if (ResourceType == tBarIo) {
                if( PciDeviceInfo->Bar.Type != tBarIo)
                    continue;
            }
            if(!(*MaxGranularity)) {
                *MaxGranularity = PciDeviceInfo->Bar.Gran;
                *MaxGranPciDeviceInfo = PciDeviceInfo;
            } else if ( PciDeviceInfo->Bar.Gran >  *MaxGranularity ) {
                *MaxGranularity = PciDeviceInfo->Bar.Gran;
                *MaxGranPciDeviceInfo = PciDeviceInfo;
            }
            
        } 
    } // for loop
    if(*MaxGranularity) {
        return EFI_SUCCESS;
    }
    return EFI_NOT_FOUND;
}

/**
    Calculates the resources required for each bridge,This function will be called recursively
    in case of Pci to Pci Bridge under Bridge is found. 

    @param[in]  PeiPciPrivateData       Pointer to the PEI_PCI_ENUMERATION_PRIVATE_DATA
    @param[in]  ParentBridge            Pointer to the parent bridge PCI_DEVICE_INFO
    
    @retval EFI_SUCCESS - Calculated Resources successfully.
**/

EFI_STATUS 
CalculateBridgeResources(
    PEI_PCI_ENUMERATION_PRIVATE_DATA    *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                    *ParentBridge
) 
{
    UINT8                                   Index = 0;
    PEI_PCI_DEV_INFO                        *PciDeviceInfo = NULL;
    PEI_PCI_BRIDGE_INFO_EXT                 *ChildBridgeExtInfo = NULL;
    PEI_PCI_BRIDGE_INFO_EXT                 *ParentBridgeExtInfo =
                                                (PEI_PCI_BRIDGE_INFO_EXT*)(ParentBridge+1);
    UINT32                                  GranRemainderLen = 0;

    PEI_PCI_DEBUG((EFI_D_INFO,"-----------------------------------------------------------\n"));
    PEI_PCI_DEBUG((EFI_D_INFO,"Calculate Resource for the Bridge @B[%X]|D[%X]|F[%X] Start \n", 
                                         ParentBridge->Bus, ParentBridge->Device, ParentBridge->Function ));
    PEI_PCI_DEBUG((EFI_D_INFO,"-----------------------------------------------------------\n"));

    for(Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
       
       PciDeviceInfo =  PeiPciPrivateData->PciDeviceInfo[Index];
       if(PciDeviceInfo->ParentBridge != ParentBridge) continue;
       if(PciDeviceInfo->Type == tPci2PciBrg) {
           ChildBridgeExtInfo = (PEI_PCI_BRIDGE_INFO_EXT*)(PciDeviceInfo+1);
           // Now start calculating the resources for the child bridge
           CalculateBridgeResources(PeiPciPrivateData,PciDeviceInfo);
           ParentBridgeExtInfo->IoBaseLength += ChildBridgeExtInfo->IoBaseLength;
           ParentBridgeExtInfo->MmioBaseLength += ChildBridgeExtInfo->MmioBaseLength;
       }
       if(PciDeviceInfo->Type == tPciDevice) {
           PEI_PCI_DEBUG((EFI_D_INFO, "  --Required Resource for the PCI Device @B[%X]|D[%X]|F[%X] \n", PciDeviceInfo->Bus, PciDeviceInfo->Device,PciDeviceInfo->Function));
           PEI_PCI_DEBUG((EFI_D_INFO, "    Bar Length :%lx, Bar Granularity:%lx, Bar Type :%x\n",PciDeviceInfo->Bar.Length, PciDeviceInfo->Bar.Gran, PciDeviceInfo->Bar.Type));
           // Append the Bar length and  Update the Granularity.
           if(PciDeviceInfo->Bar.Type == tBarIo ) {
               ParentBridgeExtInfo->IoBaseLength += (UINT32)PciDeviceInfo->Bar.Length;
               if(!ParentBridgeExtInfo->IoGranularity ) {
                   ParentBridgeExtInfo->IoGranularity = (UINT32)PciDeviceInfo->Bar.Gran;
               } else if (ParentBridgeExtInfo->IoGranularity < PciDeviceInfo->Bar.Gran ) {
                   ParentBridgeExtInfo->IoGranularity = (UINT32)PciDeviceInfo->Bar.Gran;
               }
           } else if ( PciDeviceInfo->Bar.Type == tBarMmio32 || PciDeviceInfo->Bar.Type == tBarMmio64) {
               ParentBridgeExtInfo->MmioBaseLength += PciDeviceInfo->Bar.Length;
               if(!ParentBridgeExtInfo->MmioGranularity ) {
                   ParentBridgeExtInfo->MmioGranularity = PciDeviceInfo->Bar.Gran;
               } else if (ParentBridgeExtInfo->MmioGranularity < PciDeviceInfo->Bar.Gran ) {
                   ParentBridgeExtInfo->MmioGranularity = PciDeviceInfo->Bar.Gran;
               }
           }
        }
    }
    // Check and update the Io Granularity, make sure minimum IO granularity for a bridge is 0xFFF.
    if( ParentBridgeExtInfo->IoBaseLength) {
        GranRemainderLen = ParentBridgeExtInfo->IoBaseLength % (P2P_BRG_IO_GRA +1);
        if( GranRemainderLen) {
            ParentBridgeExtInfo->IoBaseLength += (UINT32)(P2P_BRG_IO_GRA + 1) - GranRemainderLen;
        }
    }
    // Check and update the IO Granularity, make sure minimum IO granularity for a bridge is 0xFFFFF.
    if(ParentBridgeExtInfo->MmioBaseLength) {
        DivU64x32Remainder(ParentBridgeExtInfo->MmioBaseLength,(UINT32)(P2P_BRG_MEM_GRA +1),&GranRemainderLen);
        if (GranRemainderLen) {
            ParentBridgeExtInfo->MmioBaseLength += (P2P_BRG_MEM_GRA + 1) - GranRemainderLen;
        }
    }
    PEI_PCI_DEBUG((EFI_D_INFO,"Total Resource Required for the Bridge @B[%X]|D[%X]|F[%X] \n   --MMIO Length:%lx,IO Length:%x\n",
                      ParentBridge->Bus, ParentBridge->Device, ParentBridge->Function,
                      ParentBridgeExtInfo->MmioBaseLength,ParentBridgeExtInfo->IoBaseLength));
    PEI_PCI_DEBUG((EFI_D_INFO,"--------------------------------------------------------- \n\n"));

    return EFI_SUCCESS;
    
} 
/**
    Enumerates the Bridge and finds the Devices under the bridge, if a bridge is found
    under parent bridge again call this EnumerateBus() function in recursive manner. 

    @param[in]  PeiPciPrivateData       Pointer to the PEI_PCI_ENUMERATION_PRIVATE_DATA
    @param[in]  ParentBridge            Pointer to the parent bridge PCI_DEVICE_INFO
    
    @retval EFI_OUT_OF_RESOURCES - Resource is not enough for storing
                                   Device Info
    @retval EFI_SUCCESS - Bus has been enumerated successfully.

**/
EFI_STATUS 
EnumerateBus(
    PEI_PCI_ENUMERATION_PRIVATE_DATA      *PeiPciPrivateData,
    PEI_PCI_DEV_INFO                      *ParentBridge
)
{
    EFI_STATUS                          Status;
    UINT8                               Bus;
    UINT8                               Device;
    UINT8                               Function;
    UINT16                              VendorId;
    UINT8                               HeaderType;
    UINT8                               BarOffset;
    UINT8                               RevisionId[4];
    BOOLEAN                             MultiFunc=FALSE;
    AMI_PEI_PCI_CLASSCODE               PciDeviceClass;
    PEI_PCI_DEV_INFO                    *PciDeviceInfo;
    PEI_PCI_BRIDGE_INFO_EXT             *ParentBridgeExtInfo;
    PEI_PCI_BRIDGE_INFO_EXT             *ChildBridgeExtInfo;
    
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;
    //<Machong-20170719-try to make it work in CAR->
    
    //<Machong-20170719-try to make it work in CAR+>
    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR-> 
    
    ParentBridgeExtInfo = (PEI_PCI_BRIDGE_INFO_EXT*)(ParentBridge+1);
    Bus =  ParentBridgeExtInfo->SecondaryBusNumber;
    
    for( Device = 0; Device <= PCI_MAX_DEVICE; Device++ ) {
        
        for( Function = 0;Function <= PCI_MAX_FUNC; Function++ ) {
           
            PciCfg->Read(PeiServices, \
                            PciCfg, \
                            EfiPeiPciCfgWidthUint16,\
                            PEI_PCI_CFG_ADDRESS(Bus,Device,
                            Function,PCI_VENDOR_ID_OFFSET),\
                            &VendorId);

            PEI_PCI_DEBUG((EFI_D_INFO,"Enumerating B[%x]|D[%x]|F[%x] Vendor ID:%X \n",Bus, Device,Function,VendorId ));
            //
            // Check for valid Device/Bridge
            //
            if((Function == 0) && VendorId == DEVICE_ID_NOCARE) {
                break;
            } else if ( VendorId == DEVICE_ID_NOCARE ) {
                continue;
            }

            PciCfg->Read(PeiServices, \
                          PciCfg, \
                          EfiPeiPciCfgWidthUint8,\
                          PEI_PCI_CFG_ADDRESS(Bus,Device,
                          Function,PCI_HEADER_TYPE_OFFSET),\
                          &HeaderType);

            PEI_PCI_DEBUG((EFI_D_INFO, "HeaderType:%x\n",HeaderType));
            //
            // Check whether it is a multi function Device.
            //
            if( Function == 0) {
                if(HeaderType & HEADER_TYPE_MULTI_FUNCTION) {
                    MultiFunc = TRUE;
                }
            }
            // HeaderType's Bit 0 and Bit 1 will indicate the type of the 
            // PCI Device
            
            HeaderType &= 0x03;

            switch(HeaderType) {
                case HEADER_TYPE_DEVICE :
                    PEI_PCI_DEBUG((EFI_D_INFO, "Found PCI Target Device @B[%x]|D[%x]|F[%x]\n",Bus, Device, Function ));
                    CheckPreEnumerationInitFunctionHook(Bus, Device, Function, VendorId);
                    //
                    // PCI Target device Found.
                    //
                    PciCfg->Read(PeiServices, \
                                  PciCfg, \
                                  EfiPeiPciCfgWidthUint32,\
                                  PEI_PCI_CFG_ADDRESS(Bus,Device,
                                  Function,PCI_REVISION_ID_OFFSET),\
                                  &RevisionId);
                    PciDeviceClass.BaseClassCode = RevisionId[3];
                    PciDeviceClass.SubClassCode = RevisionId[2];
                    PciDeviceClass.ProgrammingInterface = RevisionId[1];
                    
                    //
                    // Verify whether this device is to be handled or not
                    // based on the class code list.
                    BarOffset = 0xFF; //<Machong-20170719-Make it more iteration for full scan+>
                    while( IsAllowedClassCodeDevice(PciDeviceClass,&BarOffset)) {
                        //
                        // Store the PCI device's info.
                        //
                       if( PeiPciPrivateData->PciDeviceCount >= PEI_MAXIMUM_PCI_DEVICE) {
                           //Resource is not enough for storing Device Info 
                           return EFI_OUT_OF_RESOURCES;
                       }
                       Status = (**PeiServices).AllocatePool( \
                                           (CONST EFI_PEI_SERVICES**)PeiServices,
                                            sizeof(PEI_PCI_DEV_INFO),
                                            (VOID**)&PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount]);

                       if ( EFI_ERROR( Status )) {
                           return EFI_OUT_OF_RESOURCES;
                       }
                       PciDeviceInfo = PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount];
                       (**PeiServices).SetMem((VOID*)PciDeviceInfo,
                                               sizeof(PEI_PCI_DEV_INFO),
                                               0);
                       PeiPciPrivateData->PciDeviceCount++;
                       PciDeviceInfo->Bus = Bus;
                       PciDeviceInfo->Device = Device;
                       PciDeviceInfo->Function = Function;
                       PciDeviceInfo->PciDeviceClass = PciDeviceClass;
                       PciDeviceInfo->Bar.Offset = BarOffset;
                       PciDeviceInfo->Type = tPciDevice;
                       PciDeviceInfo->ParentBridge = ParentBridge;
                       ParentBridgeExtInfo->ChildDeviceCount++;
                       // Finds the PCI Target device resource requirement
                       Status = QueryPciDeviceResources(PciDeviceInfo);
                       if(EFI_ERROR(Status)) {
                           continue;
                       }
                    }
                    break;

                case HEADER_TYPE_PCI_TO_PCI_BRIDGE:
                    if(Bus == 0) continue;
                    PEI_PCI_DEBUG((EFI_D_INFO, "Discovered PCI 2 PCI Bridge Device at B[%X]|D[%X]|F[%X]\n",\
                                                                 Bus,Device,Function));
                    //<Machong-20170720-Add Hook For Bridge+>
                    CheckPreEnumerationInitFunctionHook(Bus, Device, Function, VendorId);
                    //<Machong-20170720-Add Hook For Bridge->
                    
                    if( PeiPciPrivateData->PciDeviceCount >= PEI_MAXIMUM_PCI_DEVICE) {
                         //Resource is not enough for storing Device Info 
                         return EFI_OUT_OF_RESOURCES;
                    }
                    //
                    // New PCI to PCI Bridge found.
                    // Store the PCI to PCI Bridge Device info.
                    //
                    PeiPciPrivateData->LastUsedBusNumber++;
                    
                    Status = (**PeiServices).AllocatePool( \
                                  (CONST EFI_PEI_SERVICES**)PeiServices,
                                  (sizeof(PEI_PCI_DEV_INFO) + sizeof(PEI_PCI_BRIDGE_INFO_EXT)),
                                  (VOID**)&PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount]);
                    
                    if ( EFI_ERROR( Status )) {
                        return EFI_OUT_OF_RESOURCES;
                    }
                    PciDeviceInfo = PeiPciPrivateData->PciDeviceInfo[PeiPciPrivateData->PciDeviceCount];
                    // Set the Memory Allocated to Zero 
                    (**PeiServices).SetMem ((VOID*)PciDeviceInfo,
                                             (sizeof(PEI_PCI_DEV_INFO) + sizeof(PEI_PCI_BRIDGE_INFO_EXT)),
                                              0 );
                    PeiPciPrivateData->PciDeviceCount++;
                    PciDeviceInfo->Type = tPci2PciBrg;
                    PciDeviceInfo->Bus = Bus;
                    PciDeviceInfo->Device = Device;
                    PciDeviceInfo->Function = Function;
                    PciDeviceInfo->ParentBridge = ParentBridge;
                    ChildBridgeExtInfo = (PEI_PCI_BRIDGE_INFO_EXT*)(PciDeviceInfo+1);
                    ChildBridgeExtInfo->PrimaryBusNumber = Bus;
                    ChildBridgeExtInfo->SecondaryBusNumber = PeiPciPrivateData->LastUsedBusNumber;
                    // Assign Subordinate Bus number with the max value,
                    // Subordinate Bus Number of the bridge will be later updated
                    // if any bridge is found under this P2P bridge.
                    //<Kangmm-20190223-002 Fast Video not work on Rome+>
                    ChildBridgeExtInfo->SubordinateBusNumber = ChildBridgeExtInfo->SecondaryBusNumber + 1;
                    //<Kangmm-20190223-002 Fast Video not work on Rome->
                    //
                    // Program the Primary,Secondary & Subordinate Bus no. for Root Bridge.
                    //
                    //
                    PciCfg->Write ( PeiServices, \
                                          PciCfg, \
                                          EfiPeiPciCfgWidthUint8, \
                                          PEI_PCI_CFG_ADDRESS ( Bus, 
                                             Device, 
                                             Function, 
                                             PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET ),\
                                          &ChildBridgeExtInfo->PrimaryBusNumber );

                    PciCfg->Write ( PeiServices, \
                                         PciCfg, \
                                         EfiPeiPciCfgWidthUint8, \
                                         PEI_PCI_CFG_ADDRESS ( Bus, 
                                             Device, 
                                             Function, 
                                             PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET ),\
                                         &ChildBridgeExtInfo->SecondaryBusNumber );
                    PciCfg->Write ( PeiServices, \
                                          PciCfg, \
                                          EfiPeiPciCfgWidthUint8, \
                                          PEI_PCI_CFG_ADDRESS (Bus,
                                             Device, 
                                             Function, 
                                             PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                          &ChildBridgeExtInfo->SubordinateBusNumber);

                    //
                    // Enumerate the Device's/Bridge's under this Bridge.
                    //
                    Status = EnumerateBus(PeiPciPrivateData, PciDeviceInfo);
                    if ( EFI_ERROR( Status )) {
                        return Status;
                    }
                    //
                    // Current PCI/PCI bridge enumeration is done now Set the SubBus Number for this 
                    // Bridge with the  highest numbered Pci bus segment which is behind (subordinate to)
                    // the bridge.
                    //
                    ChildBridgeExtInfo->SubordinateBusNumber = PeiPciPrivateData->LastUsedBusNumber;
                    PciCfg->Write (PeiServices, \
                                      PciCfg, \
                                      EfiPeiPciCfgWidthUint8,\
                                      PEI_PCI_CFG_ADDRESS (
                                          PciDeviceInfo->Bus,
                                          PciDeviceInfo->Device,
                                          PciDeviceInfo->Function, 
                                          PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                      &ChildBridgeExtInfo->SubordinateBusNumber);
                    break;
                default:
                    break;
            } // Switch
            if((Function == 0) && !MultiFunc) {
                break;
            } else if(MultiFunc) {
                MultiFunc=FALSE;
            }
        } // Function loop
    } // Device Loop
    return EFI_SUCCESS;
}


/**
   Certain PCI controllers requires additional Initialization before
   Enumerating the device.This routine will check for the presence of
   Hook function added from the ELink "PeiPciPreEnumerationHookFuncList"
   
   @param Bus
   @param Device
   @param Function
   @param VendorId
   
   @return
 */
VOID
CheckPreEnumerationInitFunctionHook(
    UINT8     Bus, 
    UINT8     Device, 
    UINT8     Function,
    UINT16    VendorId
)
{
    UINT8                      Index = 0;
    UINT16                     DeviceId = 0;
    
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;
    //<Machong-20170719-try to make it work in CAR->

    PCI_CONTROLLER_INFO        PciControllerInfo[] = { PRE_ENUMERATION_INIT_FUNC_LIST
                                                       {0xFFFF,0xFFFF,NULL}
                                                     };
    //<Machong-20170719-try to make it work in CAR+>
    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR-> 
    
    PciCfg->Read(PeiServices, \
                   PciCfg, \
                   EfiPeiPciCfgWidthUint16,\
                   PEI_PCI_CFG_ADDRESS(Bus,Device,
                   Function,PCI_DEVICE_ID_OFFSET),\
                   &DeviceId);
    
    // Check for the presence of any ELink Hook Function related to this
    // PCI Device
    for(Index=0; PciControllerInfo[Index].VendorID != 0xFFFF; Index++) {
        if(VendorId == PciControllerInfo[Index].VendorID &&
            DeviceId == PciControllerInfo[Index].DeviceID) {  
                   PciControllerInfo[Index].InitFunction (
                           PCIE_CFG_ADDRESS (Bus,
                                             Device,
                                             Function,
                                             0 ));
                   break;
        }
    }
    return;
}

//<Chendl001-20170925 VGA FrameBuffer Align on 16MB +>
/**
   This Function used for ASPEED VGA FrameBuffer Align and this
   Hook function added from the ELink "MemoryAlignHookFuncList"
   
   @param Bus
   @param Device
   @param Function
   @param VendorId
   
   @return
 */
void
VgaMemoryAlignFuncHook(
    UINT8     Bus, 
    UINT8     Device, 
    UINT8     Function,
    UINT64    *MMIOBaseAddress
)
{

    UINT8                      Index = 0;
    UINT16                     VendorId;
    UINT16                     DeviceId;
    PCI_CONTROLLER_ALIGN_INFO  PciControllerInfo[] = { MEMORY_ALIGN_FUNC_LIST 
                                                           {0xFFFF,0xFFFF,NULL}
                                                         };
                                                         
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;

    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR-> 
        
    PciCfg->Read(PeiServices, \
                       PciCfg, \
                       EfiPeiPciCfgWidthUint16,\
                       PEI_PCI_CFG_ADDRESS(Bus,Device,
                       Function,PCI_VENDOR_ID_OFFSET),\
                       &VendorId);
    
    PciCfg->Read(PeiServices, \
                       PciCfg, \
                       EfiPeiPciCfgWidthUint16,\
                       PEI_PCI_CFG_ADDRESS(Bus,Device,
                       Function,PCI_DEVICE_ID_OFFSET),\
                       &DeviceId);
    
    // Check for the presence of any ELink Hook Function related to this PCI Device
    for(Index=0; PciControllerInfo[Index].VendorID != 0xFFFF; Index++) {
      if(VendorId == PciControllerInfo[Index].VendorID && DeviceId == PciControllerInfo[Index].DeviceID) { 
        PEI_PCI_DEBUG((EFI_D_INFO, "\n Come Here, VendorId = 0x%x, DeviceId = 0x%x",VendorId,DeviceId));
        PciControllerInfo[Index].InitFunction (MMIOBaseAddress);            
        break;
      }
    }
}
//<Chendl001-20170925 VGA FrameBuffer Align on 16MB ->

/** 
  Based on the ClassCode Input verified whether this device has to skipped or handled
  by checking through the list of class codes provided through the PEI_PCI_CLASS_CODE_LIST. 
  
  @param[in]      PciClassCode        Structure that Contains the BCC, SCC and PI values.
  
  @retval EFI_SUCCESS                 The operation succeeded.
**/

BOOLEAN
IsAllowedClassCodeDevice (
    IN  AMI_PEI_PCI_CLASSCODE               PciClassCode, 
    OUT UINT8                               *BarOffset
)
{
    PCI_DEVICE_CLASSCODE_ENTRY          PciDeviceClassCodeList[] = {PEI_PCI_CLASS_CODE_LIST
                                                                    {0xff, 0xff, 0xff}};
    UINT8                               PciDeviceClassCodeListSize = 
                                            sizeof(PciDeviceClassCodeList)/ sizeof(PCI_DEVICE_CLASSCODE_ENTRY);
    UINT8                               Index = 0;
    
    //<Machong-20170719-Make it more iteration for full scan+>
    UINT8                               OldBaroffset;
    
    OldBaroffset = *BarOffset;
    //<Machong-20170719-Make it more iteration for full scan->
    
    for (Index = 0; Index <= PciDeviceClassCodeListSize; Index++) {
        if(PciDeviceClassCodeList[Index].BaseClassCode ==  PciClassCode.BaseClassCode && 
                PciDeviceClassCodeList[Index].SubClassCode ==  PciClassCode.SubClassCode) {
            // Check for programming Interface value if its value is not 0xFF
            if(PciDeviceClassCodeList[Index].ProgrammingInterface != 0xFF) {
                if(PciDeviceClassCodeList[Index].ProgrammingInterface != PciClassCode.ProgrammingInterface) { 
                    return FALSE;
                }
            }
            // Check for Valid BAR offset.
            if((PciDeviceClassCodeList[Index].BarIndex) > PCI_MAX_BAR-1) {
                // Invalid Bar offset
                return FALSE;
            } else {
                *BarOffset = PCI_BASE_ADDRESSREG_OFFSET + (PciDeviceClassCodeList[Index].BarIndex)*4;
                
                if( (*BarOffset > OldBaroffset) || (OldBaroffset == 0xFF) ) //<Machong-20170720-For Bar Full Scan>
                  return TRUE;
                else
                  continue;
            }
        }
    }
    return FALSE;
}

/** 
  Queries the Resources Required for the Pci Device provided as input.
  
  @param[in]      PciDeviceInfo       Specified the pointer to the PCI_DEVICE_INFO structure
                                      of the device whose Resources needs to be queried.
  
  @retval EFI_SUCCESS                 The operation succeeded.PciDevice resource is found. 
 **/

EFI_STATUS
QueryPciDeviceResources(
    PEI_PCI_DEV_INFO         *PciDeviceInfo
) 
{
    UINT64                      Buffer64;
    UINT32                      Buffer32;
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;
    //<Machong-20170719-try to make it work in CAR->
    PCI_DEVICE_CLASSCODE_ENTRY  PciDeviceClassCodeList[] = {PEI_PCI_CLASS_CODE_LIST
                                                            {0xff, 0xff, 0xff}};
    UINT8                       PciDeviceClassCodeListSize = 
                                    sizeof(PciDeviceClassCodeList)/ sizeof(PCI_DEVICE_CLASSCODE_ENTRY);
    UINT8                       Index = 0;
    
    //<Machong-20170719-try to make it work in CAR+>
    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR-> 
    
    // currently handling PciDevice resource checking
    if(PciDeviceInfo->Type != tPciDevice) return EFI_UNSUPPORTED;
    PciCfg->Read (PeiServices, \
                       PciCfg, \
                       EfiPeiPciCfgWidthUint32, \
                       PEI_PCI_CFG_ADDRESS (
                           PciDeviceInfo->Bus,
                           PciDeviceInfo->Device, 
                           PciDeviceInfo->Function, 
                           PciDeviceInfo->Bar.Offset),\
                       &Buffer32);
    //check what type of resource the Pci device requires
    if(Buffer32 & 1) {
        PciDeviceInfo->Bar.Type=tBarIo;
    } else {
        switch ((Buffer32) & 0x0F) {
            case 0x0 : 
                PciDeviceInfo->Bar.Type=tBarMmio32;
                break;
            case 0x4 : 
                PciDeviceInfo->Bar.Type=tBarMmio64;
                break;
            default : return EFI_UNSUPPORTED;
        } //switch          
    } // Bar Type Check
    //Now try to find the Device Decode Range.
    Buffer64 = (~0ULL);
    switch (PciDeviceInfo->Bar.Type) {
        case    tBarMmio64:
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint64, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciDeviceInfo->Bus,
                                    PciDeviceInfo->Device, 
                                    PciDeviceInfo->Function, 
                                    PciDeviceInfo->Bar.Offset),\
                             &Buffer64);
                        
            PciCfg->Read(PeiServices, \
                            PciCfg, \
                            EfiPeiPciCfgWidthUint64,\
                            PEI_PCI_CFG_ADDRESS(
                                PciDeviceInfo->Bus,
                                PciDeviceInfo->Device,
                                PciDeviceInfo->Function,
                                PciDeviceInfo->Bar.Offset),\
                            &Buffer64);
            Buffer64&=(~0x0F); //Mask don't care bits for MMIO
            if(Buffer64) {
                PciDeviceInfo->Bar.Gran=(UINTN)(~Buffer64);
                PciDeviceInfo->Bar.Length = PciDeviceInfo->Bar.Gran+1;
            } else {
                PciDeviceInfo->Bar.Type= tBarUnused;
            }
            break;
        case tBarMmio32     :
        case tBarIo         :
            Buffer32 = DWORD_BAR_MASK;
            PciCfg->Write (PeiServices, \
                            PciCfg, \
                            EfiPeiPciCfgWidthUint32, \
                            PEI_PCI_CFG_ADDRESS (
                                PciDeviceInfo->Bus,
                                PciDeviceInfo->Device, 
                                PciDeviceInfo->Function, 
                                PciDeviceInfo->Bar.Offset),\
                             &Buffer32);
            PciCfg->Read (PeiServices, \
                            PciCfg, \
                            EfiPeiPciCfgWidthUint32, \
                            PEI_PCI_CFG_ADDRESS (
                                PciDeviceInfo->Bus,
                                PciDeviceInfo->Device, 
                                PciDeviceInfo->Function, 
                                PciDeviceInfo->Bar.Offset),\
                            &Buffer32);
            if(PciDeviceInfo->Bar.Type == tBarIo) {
                Buffer32 &=~0x03; // Mask Don't care bits for IO
            } else {
                Buffer32 &=~0x0F; // Mask Don't care bits for MMIO
            }

            if(Buffer32){
                PciDeviceInfo->Bar.Gran=(UINT64)(~Buffer32);
                PciDeviceInfo->Bar.Length = PciDeviceInfo->Bar.Gran+1;
            } else {
                PciDeviceInfo->Bar.Type = tBarUnused;
            }
            break;
         default : PciDeviceInfo->Bar.Type=tBarUnused;
    }//switch  
    PEI_PCI_DEBUG((EFI_D_INFO, "Bar Type:%x,Bar.Length:%lx, Bar.Gran:%lx \n", \
                         PciDeviceInfo->Bar.Type, PciDeviceInfo->Bar.Length,PciDeviceInfo->Bar.Gran));

    return EFI_SUCCESS;    
}

/** 
  Based on the specified Class Code input and DevInstance Number,Finds the PCI devices. 
  
  @param[in]      This                AMI_PEI_PCI_ENUMERATION_PPI instance. 
  @param[in]      ClassCode           Specifies the Class Code of the PCI Device to fetch.
  @param[in]      DevInstance         Instance number to discover.
  @param[in, out] PciDevice           Pointer to reference the found PciDevice.
  
  @retval EFI_SUCCESS                 The operation succeeded.PciDevice data is found. 
  @retval EFI_NOT_FOUND               No PciDevice is found at that instance.
 **/

EFI_STATUS
EFIAPI
GetNextPciDeviceByClassCode (
  IN  AMI_PEI_PCI_ENUMERATION_PPI             *This,
  IN  AMI_PEI_PCI_CLASSCODE                   PciClassCode,
  IN  UINT8                                   DevInstance,
  IN  OUT AMI_PEI_PCI_DEVICE_LOCATION         *PciDevice
)
{
    EFI_STATUS                                Status;
    AMI_PEI_PCI_CLASSCODE                     PciDeviceClassCode;
    PEI_PCI_ENUMERATION_PRIVATE_DATA          *PeiPciPrivateData = \
                                                (PEI_PCI_ENUMERATION_PRIVATE_DATA*)This;
    UINT8                                     Index;
    
    if( PciDevice == NULL ) return EFI_INVALID_PARAMETER;
    if(!PeiPciPrivateData->HaveEnumeratedDevices) {
        // PCI Bridge Enumeration is not done before, so doing now
        Status = InitializePciDevices(This);
        if(EFI_ERROR(Status)) {
            return Status;
        }
    }

    for( Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
        if(PeiPciPrivateData->PciDeviceInfo[Index]->Type != tPciDevice) continue;
        PciDeviceClassCode = PeiPciPrivateData->PciDeviceInfo[Index]->PciDeviceClass;
        if((PciDeviceClassCode.BaseClassCode == PciClassCode.BaseClassCode) &&  \
               (PciDeviceClassCode.SubClassCode == PciClassCode.SubClassCode)) {
            
            // When PI value is 0xFF, the PI value is not needed to be considered, skip 
            // checking PI.
            if(PciClassCode.ProgrammingInterface != 0xFF) {
                if(PciDeviceClassCode.ProgrammingInterface != PciClassCode.ProgrammingInterface) {  
                    continue;
                }
            }
            if(DevInstance == 0) {
                PciDevice->BusNumber = PeiPciPrivateData->PciDeviceInfo[Index]->Bus;
                PciDevice->DeviceNumber = PeiPciPrivateData->PciDeviceInfo[Index]->Device;
                PciDevice->FunctionNumber = PeiPciPrivateData->PciDeviceInfo[Index]->Function;
                PciDevice->SegmentNumber = 0;

                PEI_PCI_DEBUG((EFI_D_INFO, "\n PeiPciEnumeration GetNextPciDeviceByClassCode: EFI_SUCCESS"));
                return EFI_SUCCESS;
            }
            DevInstance--;
        }
    }
    return EFI_NOT_FOUND;
}

/** 
  Resets the resources allocated to the PCI Devices,like Secondary Bus Number,
  Subordinate Bus Number, MEM/IO Base Address and Limit Register values.
  This is to avoid the conflict with DXE drivers. 
  
  @param[in]  PeiServices       Specifies pointer to the PEI Services data structure.
  @param[in]  This              Indicates the AMI_PEI_PCI_ENUMERATION_PPI instance. 
  @retval EFI_SUCCESS           The operation succeeded. Root Bridge Resources are cleared
  @retval EFI_INVALID_PARAMETER The input parameter is illegal.   
**/

EFI_STATUS
EFIAPI
ResetPciDeviceResources (
  IN  AMI_PEI_PCI_ENUMERATION_PPI     *This
)
{
    UINT8                                   Index = 0;
    UINT8                                   CmdReg = 0;
    UINT8                                   BusNumber = 0;
    UINT32                                  ResetBuffer = 0;
    PEI_PCI_DEV_INFO                        *PciDeviceInfo = NULL;
    PEI_PCI_ENUMERATION_PRIVATE_DATA        *PeiPciPrivateData = \
                                                    (PEI_PCI_ENUMERATION_PRIVATE_DATA*)This;
    PEI_PCI_BRIDGE_INFO_EXT                 *PciBridgeInfo = NULL;
    
    
    //<Machong-20170719-try to make it work in CAR+>
    CONST EFI_PEI_SERVICES                  **PeiServices;
    EFI_PEI_PCI_CFG2_PPI                    *PciCfg;

    PeiServices = GetPeiServicesTablePointer ();
    PciCfg = (**PeiServices).PciCfg;
    //<Machong-20170719-try to make it work in CAR-> 
    
    // Check whether Device reset is already done
    if(PeiPciPrivateData->DeviceResetDone) {
        // Reset done already so returning from here
        return EFI_SUCCESS;
    }
    for( Index = 0; Index < PeiPciPrivateData->PciDeviceCount; Index++) {
        PciDeviceInfo = PeiPciPrivateData->PciDeviceInfo[Index];
        // Reset the Resources Allocated for Target Pci Device
        if(PciDeviceInfo->Type == tPciDevice) {
            // Reset the Base Address Assigned.
            if(PciDeviceInfo->Bar.Length) {
                PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint32, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciDeviceInfo->Bus,
                                    PciDeviceInfo->Device, 
                                    PciDeviceInfo->Function, 
                                    PciDeviceInfo->Bar.Offset),\
                                &ResetBuffer);
                if(PciDeviceInfo->Bar.Type == tBarMmio64) {
                    PciCfg->Write (PeiServices, \
                                 PciCfg, \
                                 EfiPeiPciCfgWidthUint32, \
                                 PEI_PCI_CFG_ADDRESS (
                                     PciDeviceInfo->Bus,
                                     PciDeviceInfo->Device, 
                                     PciDeviceInfo->Function, 
                                     (PciDeviceInfo->Bar.Offset+1)),\
                                 &ResetBuffer);
               }
            }
        }
        // Reset the Resources Allocated for Pci2PciBridge Device
        if(PciDeviceInfo->Type == tPci2PciBrg) {
            PciBridgeInfo = (PEI_PCI_BRIDGE_INFO_EXT*)(PciDeviceInfo+1);
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciDeviceInfo->Bus,
                                    PciDeviceInfo->Device, 
                                    PciDeviceInfo->Function, 
                                    PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),\
                                &BusNumber);
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciDeviceInfo->Bus,
                                    PciDeviceInfo->Device, 
                                    PciDeviceInfo->Function, 
                                    PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),\
                                &BusNumber);
            PciCfg->Write (PeiServices, \
                                PciCfg, \
                                EfiPeiPciCfgWidthUint8, \
                                PEI_PCI_CFG_ADDRESS (
                                    PciDeviceInfo->Bus,
                                    PciDeviceInfo->Device, 
                                    PciDeviceInfo->Function, 
                                    PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                &BusNumber);
            if(PciBridgeInfo->BridgeProgrammed) {
                // Now reset MemBase and Limit registers
                if (PciBridgeInfo->MmioBaseLength) {
                    PciCfg->Write (PeiServices, \
                                        PciCfg, \
                                        EfiPeiPciCfgWidthUint16, \
                                        PEI_PCI_CFG_ADDRESS (
                                            PciDeviceInfo->Bus,
                                            PciDeviceInfo->Device, 
                                            PciDeviceInfo->Function, 
                                            PCI_MEMBASE),\
                                        (UINT16*)&ResetBuffer);
                    PciCfg->Write (PeiServices, \
                                        PciCfg, \
                                        EfiPeiPciCfgWidthUint16, \
                                        PEI_PCI_CFG_ADDRESS (
                                            PciDeviceInfo->Bus,
                                            PciDeviceInfo->Device, 
                                            PciDeviceInfo->Function, 
                                            PCI_MEMLIMIT),\
                                        (UINT16*)&ResetBuffer);
                }
                if( PciBridgeInfo->IoBaseLength ) {
                    PciCfg->Write (PeiServices, \
                                        PciCfg, \
                                        EfiPeiPciCfgWidthUint8, \
                                        PEI_PCI_CFG_ADDRESS (
                                            PciDeviceInfo->Bus,
                                            PciDeviceInfo->Device, 
                                            PciDeviceInfo->Function, 
                                            PCI_IOBASE),\
                                        (UINT8*)&ResetBuffer);
                    PciCfg->Write (PeiServices, \
                                        PciCfg, \
                                        EfiPeiPciCfgWidthUint8, \
                                        PEI_PCI_CFG_ADDRESS (
                                            PciDeviceInfo->Bus,
                                            PciDeviceInfo->Device, 
                                            PciDeviceInfo->Function, 
                                            PCI_IOLIMIT),\
                                        (UINT8*)&ResetBuffer);
                }
            }
        } // Type Pci2PciBridge
        // Disable Memory and IO Space Decoding for this device.
        PciCfg->Read (PeiServices, \
                            PciCfg, \
                            EfiPeiPciCfgWidthUint8, \
                            PEI_PCI_CFG_ADDRESS (
                                PciDeviceInfo->Bus,
                                PciDeviceInfo->Device, 
                                PciDeviceInfo->Function, 
                                PCI_COMMAND_OFFSET),\
                            &CmdReg);
        CmdReg &= ~(EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER );
        PciCfg->Write (PeiServices, \
                            PciCfg, \
                            EfiPeiPciCfgWidthUint8, \
                            PEI_PCI_CFG_ADDRESS (
                                PciDeviceInfo->Bus,
                                PciDeviceInfo->Device, 
                                PciDeviceInfo->Function, 
                                PCI_COMMAND_OFFSET),
                            &CmdReg);
    } // For loop
    PeiPciPrivateData->DeviceResetDone = TRUE;
    PEI_PCI_DEBUG((EFI_D_INFO,"\n PeiPciEnumeration: Assigned PCI Device Resources reset is Successful"));
    return EFI_SUCCESS;
}

/**
    This routine resets the resources allocated for the available controllers 
    and the PCI to PCI Bridge at end of PEI.

    @param PeiServices 
    @param NotifyDesc 
    @param InvokePpi 

         
    @retval EFI_STATUS this function returns EFI_SUCCESS if the
        PCI Device is reset successfully. Otherwise, returns
        any type of error it encountered during the reset operation.
              
**/

EFI_STATUS
EFIAPI 
PeiPciNotifyOnEndOfPei (
    IN  EFI_PEI_SERVICES             **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
    IN  VOID                         *InvokePpi
)
{
    AMI_PEI_PCI_ENUMERATION_PPI       *PeiPciEnumerationPpi;
    EFI_STATUS                        Status;
    PEI_PCI_ENUMERATION_PRIVATE_DATA  *PeiPciPrivateData;
    
    Status = (**PeiServices).LocatePpi (
                                 PeiServices,
                                 &gAmiPeiPciEnumerationPpiGuid,  // GUID
                                 0,                     // INSTANCE
                                 NULL,      // PEI_PPI_DESCRIPTOR
                                 (VOID**)&PeiPciEnumerationPpi              // PPI
                                 ); 
    if(EFI_ERROR(Status)) {
        return Status;
    }
    PeiPciPrivateData = (PEI_PCI_ENUMERATION_PRIVATE_DATA*)PeiPciEnumerationPpi;
    if( PeiPciPrivateData->DeviceResetDone ) {
        return EFI_SUCCESS;
    } else {
        //<Chendl001-20171020 Not Reset PCIE Device Resources +>
        //Status = PeiPciEnumerationPpi->ResetDeviceResources(PeiPciEnumerationPpi);
        //<Chendl001-20171020 Not Reset PCIE Device Resources ->
    }
    return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
