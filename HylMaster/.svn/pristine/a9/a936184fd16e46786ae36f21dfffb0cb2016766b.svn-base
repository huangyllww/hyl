#include <AmiDxeLib.h>
#include <AmiCspLib.h>

// Produced Protocols

// Consumed Protocols
#include <Protocol\PciIo.h>
#include <Uefi\UefiBaseType.h>
#include <Token.h>
#include <Library/UefiLib.h>
#include <OemPCIeDev.h>
//
#include <Protocol/AmiSmbios.h>
#include <Library/BaseGetRbBusLib.h>
//
#include <Library/PciLib.h>
#include "PCIeSlotDes.c"
#include "AutoDxioConfig.h"
#include "AutoPCIeSlotDescriptionList.h"
BOARD_SLOT_DESCRIPTION   *BoardSlotDescList[] = {AUTO_PCIE_SLOT_DESC_LIST NULL};
CHAR8         *BoardSlotDescNameList[] = {AUTO_PCIE_SLOT_DESC_LIST_NAMES NULL};
//#include "XMachine_PCIe_Top.c"
#define BLDCFG_DF_BUS_NUM_ASSIGN_STRATEGY      0x2020202020202020 //<yanshp1-20210201 Expand the bus number for Cambrian X1000 hang. >
//#include "PCIEXPRESS.h"
#define PCIE_LINK_CAPABILITY_OFFSET           0x0C
#define PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET 0x19
typedef union {
  struct{
    UINT32  MaxLinkSpeed:4;   // 0~3
    UINT32  MaximumLinkWidth :6; // 4~9
    UINT32  ASPMSupport:2;       // 10~11
    UINT32  L0sExitLatency:3;// 12~14
    UINT32  L1ExitLatency:3;  // 15~17
    UINT32  ClockPowerManagemen:1;             // 18
    UINT32  SurpriseDownErrorReportingCapable:1;             // 19
    UINT32  DLLLinkActiveReportingCapable:1;        // 20
    UINT32  LinkBandwidthNotificationCapability:1;        // 21
    UINT32  ASPMOptionalityCompliance:1;                   // 22
    UINT32  Reserved:1;                   // 23
    UINT32  PortNumber:8;           // 31:24 
  } Bits;
  UINT32    Dword;
} PCIE_LNK_CAP;

//Mizl-Update For Bridge Exist But No Device Connected +>
#define PCIE_SLOT_STATUS_OFFSET           0x1A
typedef union {
  struct{
    UINT16  AttentionButtonPressed:1;  
    UINT16  PowerFaultDetected:1; 
    UINT16  MRLSensorChanged:1;
    UINT16  PresenceDetectChanged:1;
    UINT16  CommandCompleted:1; 
    UINT16  MRLSensorState:1;    
    UINT16  PresenceDetectState:1;       
    UINT16  ElectromechanicalInterlockStatus:1;    
    UINT16  DataLinkLayerStateChanged:1;  
    UINT16  Reserved:6;
  } Bits;
  UINT16    Word;
} PCIE_SLOT_STATUS;
//Mizl-Update For Bridge Exist But No Device Connected ->
/**
Note:
**/
UINT8               gGpuType=0xff,gCPUBusWidth=0xff;
/**
GC_TODO: add routine description
@param         - GC_TODO: add arg description
@param         - GC_TODO: add arg description
@param         - GC_TODO: add arg description
@param         - GC_TODO: add arg description

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/

EFI_STATUS
GetPCIeTop(
  IN OUT  SLOT_DESCRIPTION    **SlotDescription,
  IN OUT  UINT8 *BoardId
 // IN OUT  UINT8               *SlotDescriptionSize
)
{
    UINT8       BoardGpioVaule=0xff;
    BOARD_SLOT_DESCRIPTION *BoardslotdescTemp=NULL;
    UINT8       CountNumber=0;
    BoardGpioVaule =PcdGet8(PcdBoardGpioVaule);
    DEBUG((-1,"[OemDbg] BoardGpioVaule=%0x\n",BoardGpioVaule));
    if(BoardGpioVaule!=0xff){
        for(CountNumber=0;BoardSlotDescList[CountNumber]!=NULL;CountNumber++)
        {
            BoardslotdescTemp=BoardSlotDescList[CountNumber];
            DEBUG((-1,"[OemDbg] BoardslotdescTemp->BoardGpioNumber=%0x\n",BoardslotdescTemp->BoardGpioNumber));
            if(BoardslotdescTemp->BoardGpioNumber==BoardGpioVaule)
            {
                *SlotDescription=BoardslotdescTemp->SlotDesc;
                *BoardId=BoardslotdescTemp->BoardGpioNumber;
                return EFI_SUCCESS;
            }
        }
    }
    
    
 /* switch(gGpuType)
  {
    case X785_BALANCED_MODE_Not_Auto_Detect: //Mizl-20200617-Add Auto Detect GPU Function
    case X785_BALANCED_MODE:
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]X785 Balanced Mode Detected!!!\n\n",__FUNCTION__,__LINE__));
      *SlotDescription = &X785_Balanced_Mode_Table[0];
      *SlotDescriptionSize = sizeof(X785_Balanced_Mode_Table)/sizeof(SLOT_DESCRIPTION);
      return EFI_SUCCESS;


    case X785_COMMON_MODE_Not_Auto_Detect: //Mizl-20200617-Add Auto Detect GPU Function
    case X785_COMMON_MODE:
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]X785 Common Mode Detected!!!\n\n",__FUNCTION__,__LINE__));
      *SlotDescription = &X785_Common_Mode_Table[0];
      *SlotDescriptionSize = sizeof(X785_Common_Mode_Table)/sizeof(SLOT_DESCRIPTION);
      return EFI_SUCCESS;


    case X745:
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]X745 Detected!!!\n\n",__FUNCTION__,__LINE__));
      *SlotDescription = &X745_Mode[0];
      *SlotDescriptionSize = sizeof(X745_Mode)/sizeof(SLOT_DESCRIPTION);
      return EFI_SUCCESS;


    default:
      DEBUG((EFI_D_ERROR, "[%a][%d]Unsupported GPUType:%x\n\n",__FUNCTION__,__LINE__,gGpuType));
      return EFI_INVALID_PARAMETER;
  }*/

  return EFI_INVALID_PARAMETER;
}
/**
    Returns the length of the structure pointed by BufferStart
    in bytes

    @param BufferStart  pointer to buffer

    @return Structure size

**/
UINT16
OEM_GetStructureLength(
    IN UINT8    *BufferStart
)
{
    UINT8       *BufferEnd = BufferStart;

    BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;

    while (ReadUnaligned16((UINT16*)BufferEnd) != 0) {
        BufferEnd++;
    }

    return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}


/**
    Find structure type starting from memory location pointed by
    Buffer

    @param Buffer
    @param StructureFoundPtr
    @param SearchType
    @param Instance


    @retval TRUE Structure found
    @retval FALSE Structure not found

    @remark
        @li If SearchType is found:\n
            UINT8   **Buffer - Points to the next structure\n
            UINT8   **StructureFoundPtr - Points to the structure that was found\n

        @li If SearchType is not found:\n
            UINT8   **Buffer - No change\n
            UINT8   **StructureFoundPtr = NULL

**/
BOOLEAN
FindStructureType(
    IN OUT UINT8    **Buffer,
    IN OUT UINT8    **StructureFoundPtr,
    IN     UINT8    SearchType,
    IN     UINT8    Instance        // 1-based
)
{
    UINT8       *BufferPtr = *Buffer;
    BOOLEAN     FindStatus = FALSE;

    *StructureFoundPtr = NULL;
    while (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type != 127) {
        if (((SMBIOS_STRUCTURE_HEADER*)BufferPtr)->Type == SearchType) {
            // If this instance, set the find status flag and update the Buffer pointer
            if (--Instance == 0) {
                DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]  Entry!!!\n\n",__FUNCTION__,__LINE__));
                FindStatus = TRUE;
                *StructureFoundPtr = BufferPtr;
                *Buffer = BufferPtr + OEM_GetStructureLength(BufferPtr);
                break;
            }
        }
        BufferPtr += OEM_GetStructureLength(BufferPtr);
    }
    if ((FindStatus == FALSE) && (SearchType == 127)) {
        FindStatus = TRUE;
        *StructureFoundPtr = BufferPtr;
        *Buffer = BufferPtr + OEM_GetStructureLength(BufferPtr);
    }
    return FindStatus;
}

/**
    Returns the length of the structure pointed by BufferStart
    in bytes

    @param BufferStart  pointer to buffer

    @return Structure size

**/
UINT16
GetStructureLength(
    IN UINT8    *BufferStart
)
{
    UINT8       *BufferEnd = BufferStart;

    BufferEnd += ((SMBIOS_STRUCTURE_HEADER*)BufferStart)->Length;

    while (*(UINT16*)BufferEnd != 0) {
        BufferEnd++;
    }

    return (UINT16)(BufferEnd + 2 - BufferStart);   // +2 for double zero terminator
}

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
GC_TODO: Try to Update SMBIOS Type 9

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
EFIAPI
UpdateTable9 ()
{
  AMI_SMBIOS_PROTOCOL		          *SmbiosProtocol;
  
  //OEM_PCIE_DEV_DATA_PROTOCOL    *OemPcieDevProtocol;
  //PCI_DEV_SETUP_FORM_DATA         *PcieDevData;
  //PCI_DEV_IDX_DATA                *IdxDev;
  //PCIE_DEV_INFO                   PcieDevInfo;
  UINT8                           CapIdOffset;
  PCIE_LNK_CAP                    LinkCap;

  EFI_STATUS                      Status=EFI_SUCCESS;
  //UINT8                           DevIdx,PortIdx;


  UINT8                           *SrcBuffer;
  UINT8                           *StructureFoundPtr;

  SMBIOS_TABLE_ENTRY_POINT	      *SmbiosTableEntryPoint;

  //UINT8                           iSlotIndex;

  UINT8                           *TempBuffer;

  UINT16                          Size;
  UINT16                          Handle;

  /**
  Note:Define PCIe Top Related
  **/
  SLOT_DESCRIPTION              *pSlotDescription=NULL;
  UINT8                         iSlotDescription,RaidOcpIndex=0;
  UINT8                         NvmeIndex=0;
  UINT8                         RootBusNumber;
  UINT8                         SlotBus=0,SlotDevice=0,SlotFunction=0;
  UINT32                        SlotAddress;
  UINT16                        SlotVendorID=0xFFFF;
  UINT8                         Socket=0,DieNumber=0,FabricID=0;
  UINT8                         InstalledSocketNumber=0;
  UINT8                         DownStreamPortBusNumber,SecondaryBus;
  UINT8                         SubBus=0,RDvice=0,RFunctios=0,RBus=0;
  UINT8                         CountNumber=0;
  UINT8                         DeviceType=0;
  //PCIE_SLOT_STATUS              SlotStatus;//Mizl-Update For Bridge Exist But No Device Connected +>
  UINT8                         BoardId=0xff,SlotNUM=13;
  PCIE_SLOT_STATUS              SlotStatus;//Mizl-Update For Bridge Exist But No Device Connected +>
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]  Entry!!!\n\n",__FUNCTION__,__LINE__));

  // /**
  // Note: Get PCIe Device List
  // **/
  // Status = pBS->LocateProtocol (
  //         &gOemPcieDevProtocolGuid,
  //         NULL,
  //         &OemPcieDevProtocol
  //         );
  // if ( EFI_ERROR (Status) ) 
  //   return EFI_UNSUPPORTED;

  // DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));
  // PcieDevData = NULL;
  // Status = OemPcieDevProtocol->GetPcieDevList(&PcieDevData);
  // if ( EFI_ERROR (Status) ) 
  //   return EFI_UNSUPPORTED;
  // DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));


  /**
  Note:Locate SMBios Protocol
  **/
  Status = pBS->LocateProtocol (&gAmiSmbiosProtocolGuid, NULL, &SmbiosProtocol);
  if ( EFI_ERROR (Status) ) 
    return EFI_UNSUPPORTED;
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));

  /**
  Note:Scan SMBios Table
  **/
  SmbiosTableEntryPoint = SmbiosProtocol->SmbiosGetTableEntryPoint(2);
  if (SmbiosTableEntryPoint == NULL) {
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));
    return EFI_UNSUPPORTED;
  }

  /**
  Note:Get PCIe Top
  **/
  //gGpuType = (UINT8)PcdGet8(PcdGPUType);	
  //gCPUBusWidth = (PcdGet32(PcdNumberOfInstalledProcessors) == 1)? 0x40:0x20;
  InstalledSocketNumber=(UINT8)PcdGet32(PcdNumberOfInstalledProcessors);
  gCPUBusWidth = (InstalledSocketNumber == 1)? 0x20:0x10;
  if(BLDCFG_DF_BUS_NUM_ASSIGN_STRATEGY !=0 )
    gCPUBusWidth = BLDCFG_DF_BUS_NUM_ASSIGN_STRATEGY & 0xFF;
  Status = GetPCIeTop(&pSlotDescription,&BoardId);//,&SlotDescriptionSize);
  if ( EFI_ERROR (Status) ) 
    return EFI_UNSUPPORTED;
  
  if (pSlotDescription == NULL) {
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));
    return EFI_UNSUPPORTED;
  }
  /**
  Note:Dump All PCIe Device
  **/
  //DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotDescriptionSize:%x\n\n",__FUNCTION__,__LINE__,SlotDescriptionSize));

  for(iSlotDescription = 0;pSlotDescription[iSlotDescription].DieNumber!=INVALID_SETTING;iSlotDescription++)
  {
    RootBusNumber = pSlotDescription[iSlotDescription].DieNumber * gCPUBusWidth;

    /**
    Note: Now We Update Slot Basic Information Now
    **/
    /**
    Note:
      X745 Mode:  
                CPU <=====>Baseboard Slot   
                =====>DownStreamDevice == 0xff
      X785 Mode: 
                CPU <=======>Switch Up Stream Port <========>DownStream Port <============> EndPoint 
                ====>DownStreamDevice != 0xff
    **/
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]RootBusNumber:%x\n\n",__FUNCTION__,__LINE__,RootBusNumber));
    if(pSlotDescription[iSlotDescription].DownStreamDevice == 0xff)
    {
      SlotBus = RootBusNumber;
      SlotDevice = pSlotDescription[iSlotDescription].RootDevice ;
      SlotFunction = pSlotDescription[iSlotDescription].RootFunction;
    }else
    {
        DownStreamPortBusNumber = PciRead8(PCI_LIB_ADDRESS(RootBusNumber+2,pSlotDescription[iSlotDescription].DownStreamDevice,0,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
        if(pSlotDescription[iSlotDescription].DownStreamDevice1 != 0xff)
         {
            SlotBus =DownStreamPortBusNumber+1;// PciRead8(PCI_LIB_ADDRESS(DownStreamPortBusNumber+1,pSlotDescription[iSlotDescription].DownStreamDevice1,0,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
            SlotDevice = pSlotDescription[iSlotDescription].DownStreamDevice1;
         }
        else{
            SlotBus=RootBusNumber+2;
            SlotDevice = pSlotDescription[iSlotDescription].DownStreamDevice;
        }
      SlotFunction = 0;
    }

    SrcBuffer = (UINT8 *)(SmbiosTableEntryPoint->TableAddress); //We Use Slot Number to Get The Type 9 Table,Then Restore The Start Address!!
    if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 9, pSlotDescription[iSlotDescription].SlotNumber)) 
    {
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Get The Struct!!!Ready To Update...\n\n",__FUNCTION__,__LINE__));
      Size =GetStructureLength(StructureFoundPtr);
      pBS->AllocatePool(EfiBootServicesData,Size,(void**)&TempBuffer);
      MemCpy(TempBuffer, StructureFoundPtr, Size);
/*
      ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = SlotBus;
      ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (SlotDevice << 3) | SlotFunction;

      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Root BDF:%x %x %x\n\n",__FUNCTION__,__LINE__,SlotBus,SlotDevice,SlotFunction));
      SlotAddress = PCI_LIB_ADDRESS(SlotBus,SlotDevice,SlotFunction,0x00);
      SlotVendorID = PciRead16(SlotAddress);
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotVendorID %0x  SlotAddress %0x\n\n",__FUNCTION__,__LINE__,SlotVendorID,SlotAddress));
      if(SlotVendorID == 0xFFFF)
        continue;
*/
      //Get Slot bus dev func and detect the slot
      SecondaryBus=PciRead8(PCI_LIB_ADDRESS(SlotBus,SlotDevice,SlotFunction,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
      SubBus = PciRead8(PCI_LIB_ADDRESS(SlotBus,SlotDevice,SlotFunction,PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SecondaryBus %0x  SubBus %0x\n\n",__FUNCTION__,__LINE__,SecondaryBus,SubBus));
      if(SecondaryBus == SubBus)
      {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]No Mul-Function\n\n",__FUNCTION__,__LINE__));  
        RBus = SecondaryBus;          
      }
      else{
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function Or Bridge\n\n",__FUNCTION__,__LINE__));  
          DeviceType = PciRead8(PCI_LIB_ADDRESS(SecondaryBus,0,0,0x0E));
          if(DeviceType == 1)
          {
           DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]  Bridge\n\n",__FUNCTION__,__LINE__));  
           RBus = SubBus;
          }else{
             DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function\n\n",__FUNCTION__,__LINE__));  
            RBus = SecondaryBus;
          }
          /*DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]DeviceType %0x  \n\n",__FUNCTION__,__LINE__,DeviceType));
          DeviceType =  DeviceType<<1;
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]DeviceType1%0x  \n\n",__FUNCTION__,__LINE__,DeviceType));
          DeviceType = (DeviceType>>7)&1;
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]DeviceType2 %0x  \n\n",__FUNCTION__,__LINE__,DeviceType));
          if(DeviceType == 0)
          {
            DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function\n\n",__FUNCTION__,__LINE__));  
            RBus = SecondaryBus;
          }*/

      }
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotBus %0x  SlotDevice %0x SlotFunction %0x\n\n",__FUNCTION__,__LINE__,RBus,RDvice,RFunctios));
      SlotVendorID = PciRead16(PCI_LIB_ADDRESS(RBus,0,0,0x00));
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotVendorID %0x  SlotAddress %0x\n\n",__FUNCTION__,__LINE__,SlotVendorID,SlotAddress));
      if(SlotVendorID == 0xFFFF)
        continue;
      
      SlotAddress = PCI_LIB_ADDRESS(SlotBus,SlotDevice,SlotFunction,0x00);
      //update slot bus dev func Number; Bus Number and Device/Function Number See the endpoint in the slot, not the upstream switch
      ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = RBus;
      ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (RDvice << 3) | RFunctios;

      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotBus %0x  SlotDevice %0x SlotFunction %0x\n\n",__FUNCTION__,__LINE__,RBus,RDvice,RFunctios));
      GetLegCapStrucAddr(SlotAddress,0x10,&CapIdOffset);
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]CapIdOffset %0x\n\n",__FUNCTION__,__LINE__,CapIdOffset));
      if((CapIdOffset !=0) && (CapIdOffset !=0xFF))
      {
        LinkCap.Dword = PciRead32(SlotAddress+CapIdOffset + PCIE_LINK_CAPABILITY_OFFSET);
//Mizl-Update For Bridge Exist But No Device Connected +>
        SlotStatus.Word = PciRead16(SlotAddress+CapIdOffset + PCIE_SLOT_STATUS_OFFSET);
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]LinkWidth:%x Slot Presence:%x\n\n",__FUNCTION__,__LINE__,LinkCap.Bits.MaximumLinkWidth,SlotStatus.Bits.PresenceDetectState));
        if(SlotStatus.Bits.PresenceDetectState == 1)
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->CurrentUsage = 0x04;//In Use
//Mizl-Update For Bridge Exist But No Device Connected ->
        switch (LinkCap.Bits.MaximumLinkWidth)
        {
          case 1:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth1X;
            break;
          case 2:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth2X;
            break;
          case 4:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth4X;
            break;
          case 8:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth8X;
            break;
          case 16:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth16X;
            break;
          case 32:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth32X;
            break;
          default:
            ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidthUnknown;
            break;
        }
      }
    }
    /**
    Note:Update SMBios Type 9
    **/
    Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB8;
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth:%x \n\n",__FUNCTION__,__LINE__,((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]StructureFoundPtr:%x SlotID:%d\n\n",__FUNCTION__,__LINE__,TempBuffer,((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotID));
    Status = SmbiosProtocol->SmbiosWriteStructure(Handle, TempBuffer, GetStructureLength(TempBuffer));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Update Struct %r\n\n",__FUNCTION__,__LINE__,Status));
    if(TempBuffer!=NULL)
    pBS->FreePool(TempBuffer);
  }
/**
 * 
 Update OCP and RAID Card SMBIOS Type 9 Information 
 */
  
  for(RaidOcpIndex = 0;SmbiosRAIDOCPList[RaidOcpIndex].DieNumber!=INVALID_SETTING;RaidOcpIndex++)
  {
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] pSlotDescription->DieNumber=%0x\n\n",__FUNCTION__,__LINE__, SmbiosRAIDOCPList[RaidOcpIndex].DieNumber));
      RootBusNumber = SmbiosRAIDOCPList[RaidOcpIndex].DieNumber * gCPUBusWidth;
     
  SrcBuffer = (UINT8 *)(SmbiosTableEntryPoint->TableAddress);
  if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 9, (NUMBER_OF_SYSTEM_SLOTS-SmbiosRAIDOCPList[RaidOcpIndex].SlotNumber))) 
  {
    Size =GetStructureLength(StructureFoundPtr);
    pBS->AllocatePool(EfiBootServicesData,Size,(void**)&TempBuffer);
    MemCpy(TempBuffer, StructureFoundPtr, Size);
    
    /**
    Note:Read Bridge Info To Judge Device Status
    **/
    //Get Slot bus dev func and detect the slot
    SecondaryBus=PciRead8(PCI_LIB_ADDRESS(RootBusNumber,SmbiosRAIDOCPList[RaidOcpIndex].RootDevice,SmbiosRAIDOCPList[RaidOcpIndex].RootFunction,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
    SubBus = PciRead8(PCI_LIB_ADDRESS(RootBusNumber,SmbiosRAIDOCPList[RaidOcpIndex].RootDevice,SmbiosRAIDOCPList[RaidOcpIndex].RootFunction,PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SecondaryBus %0x  SubBus %0x\n\n",__FUNCTION__,__LINE__,SecondaryBus,SubBus));
    if(SecondaryBus == SubBus)
      {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]No Mul-Function\n\n",__FUNCTION__,__LINE__));  
        RBus = SecondaryBus;          
      }
      else{
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function Or Bridge\n\n",__FUNCTION__,__LINE__));  
          DeviceType = PciRead8(PCI_LIB_ADDRESS(SecondaryBus,0,0,0x0E));
          if(DeviceType == 1)
          {
           DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]  Bridge\n\n",__FUNCTION__,__LINE__));  
           RBus = SubBus;
          }else{
             DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function\n\n",__FUNCTION__,__LINE__));  
            RBus = SecondaryBus;
          }

      }
    SlotVendorID = PciRead16(PCI_LIB_ADDRESS(RBus,0,0,0x00));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotVendorID %0x  SlotAddress %0x\n\n",__FUNCTION__,__LINE__,SlotVendorID));
    if(SlotVendorID == 0xFFFF)
      continue;
    
    SlotAddress = PCI_LIB_ADDRESS(RootBusNumber,SmbiosRAIDOCPList[RaidOcpIndex].RootDevice,SmbiosRAIDOCPList[RaidOcpIndex].RootFunction,0x00);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]RBus %0x  RDvice %0x RFunctios %0x\n\n",__FUNCTION__,__LINE__,RBus,RDvice,RFunctios));
    //update slot bus dev func Number
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = RBus;
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (RDvice << 3) | RFunctios;
    
    GetLegCapStrucAddr(SlotAddress,0x10,&CapIdOffset);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]CapIdOffset %0x\n\n",__FUNCTION__,__LINE__,CapIdOffset));
    if((CapIdOffset !=0) && (CapIdOffset !=0xFF))
    {
      LinkCap.Dword = PciRead32(SlotAddress+CapIdOffset + PCIE_LINK_CAPABILITY_OFFSET);
//Mizl-Update For Bridge Exist But No Device Connected +>
      SlotStatus.Word = PciRead16(SlotAddress+CapIdOffset + PCIE_SLOT_STATUS_OFFSET);
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]LinkWidth:%x Slot Presence:%x\n\n",__FUNCTION__,__LINE__,LinkCap.Bits.MaximumLinkWidth,SlotStatus.Bits.PresenceDetectState));
      if(SlotStatus.Bits.PresenceDetectState == 1)
        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->CurrentUsage = 0x04;//In Use
//Mizl-Update For Bridge Exist But No Device Connected ->
      switch (LinkCap.Bits.MaximumLinkWidth)
      {
        case 1:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth1X;
          break;
        case 2:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth2X;
          break;
        case 4:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth4X;
          break;
        case 8:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth8X;
          break;
        case 16:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth16X;
          break;
        case 32:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth32X;
          break;
        default:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidthUnknown;
          break;
      }
    }
    /**
    Note:Update SMBios Type 9
    **/
    Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB8;
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth:%x \n\n",__FUNCTION__,__LINE__,((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]StructureFoundPtr:%x SlotID:%d\n\n",__FUNCTION__,__LINE__,TempBuffer,((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotID));
    Status = SmbiosProtocol->SmbiosWriteStructure(Handle, TempBuffer, GetStructureLength(TempBuffer));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Update Struct %r\n\n",__FUNCTION__,__LINE__,Status));
    if(TempBuffer!=NULL)
    pBS->FreePool(TempBuffer);
  }
  }
/**
 * 
 Update NVMe SMBIOS Type 9 Information 
 */
  
  for(NvmeIndex = 0;SmbiosNvmeList[NvmeIndex].DieNumber!=INVALID_SETTING;NvmeIndex++)
  {
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] pSlotDescription->DieNumber=%0x\n\n",__FUNCTION__,__LINE__, SmbiosNvmeList[NvmeIndex].DieNumber));
      RootBusNumber = SmbiosNvmeList[NvmeIndex].DieNumber * gCPUBusWidth;
     
  SrcBuffer = (UINT8 *)(SmbiosTableEntryPoint->TableAddress);
  if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 9, (NUMBER_OF_SYSTEM_SLOTS-SmbiosNvmeList[NvmeIndex].SlotNumber))) 
  {
    Size =GetStructureLength(StructureFoundPtr);
    pBS->AllocatePool(EfiBootServicesData,Size,(void**)&TempBuffer);
    MemCpy(TempBuffer, StructureFoundPtr, Size);
    
    /**
    Note:Read Bridge Info To Judge Device Status
    **/
    //Get Slot bus dev func and detect the slot
    SecondaryBus=PciRead8(PCI_LIB_ADDRESS(RootBusNumber,SmbiosNvmeList[NvmeIndex].RootDevice,SmbiosNvmeList[NvmeIndex].RootFunction,PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
    SubBus = PciRead8(PCI_LIB_ADDRESS(RootBusNumber,SmbiosNvmeList[NvmeIndex].RootDevice,SmbiosNvmeList[NvmeIndex].RootFunction,PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SecondaryBus %0x  SubBus %0x\n\n",__FUNCTION__,__LINE__,SecondaryBus,SubBus));
    if(SecondaryBus == SubBus)
      {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]No Mul-Function\n\n",__FUNCTION__,__LINE__));  
        RBus = SecondaryBus;          
      }
      else{
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function Or Bridge\n\n",__FUNCTION__,__LINE__));  
          DeviceType = PciRead8(PCI_LIB_ADDRESS(SecondaryBus,0,0,0x0E));
          if(DeviceType == 1)
          {
           DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]  Bridge\n\n",__FUNCTION__,__LINE__));  
           RBus = SubBus;
          }else{
             DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d] Mul-Function\n\n",__FUNCTION__,__LINE__));  
            RBus = SecondaryBus;
          }

      }
    SlotVendorID = PciRead16(PCI_LIB_ADDRESS(RBus,0,0,0x00));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]SlotVendorID %0x  SlotAddress %0x\n\n",__FUNCTION__,__LINE__,SlotVendorID));
    if(SlotVendorID == 0xFFFF)
      continue;
    
    SlotAddress = PCI_LIB_ADDRESS(RootBusNumber,SmbiosNvmeList[NvmeIndex].RootDevice,SmbiosNvmeList[NvmeIndex].RootFunction,0x00);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]RBus %0x  RDvice %0x RFunctios %0x\n\n",__FUNCTION__,__LINE__,RBus,RDvice,RFunctios));
    //update slot bus dev func Number
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->BusNumber = RBus;
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->DevFuncNumber = (RDvice << 3) | RFunctios;
    
    GetLegCapStrucAddr(SlotAddress,0x10,&CapIdOffset);
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]CapIdOffset %0x\n\n",__FUNCTION__,__LINE__,CapIdOffset));
    if((CapIdOffset !=0) && (CapIdOffset !=0xFF))
    {
      LinkCap.Dword = PciRead32(SlotAddress+CapIdOffset + PCIE_LINK_CAPABILITY_OFFSET);
//Mizl-Update For Bridge Exist But No Device Connected +>
      SlotStatus.Word = PciRead16(SlotAddress+CapIdOffset + PCIE_SLOT_STATUS_OFFSET);
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]LinkWidth:%x Slot Presence:%x\n\n",__FUNCTION__,__LINE__,LinkCap.Bits.MaximumLinkWidth,SlotStatus.Bits.PresenceDetectState));
      if(SlotStatus.Bits.PresenceDetectState == 1)
        ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->CurrentUsage = 0x04;//In Use
//Mizl-Update For Bridge Exist But No Device Connected ->
      switch (LinkCap.Bits.MaximumLinkWidth)
      {
        case 1:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth1X;
          break;
        case 2:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth2X;
          break;
        case 4:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth4X;
          break;
        case 8:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth8X;
          break;
        case 16:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth16X;
          break;
        case 32:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidth32X;
          break;
        default:
          ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth = SlotDataBusWidthUnknown;
          break;
      }
    }
    /**
    Note:Update SMBios Type 9
    **/
    Handle = ((SMBIOS_STRUCTURE_HEADER*)TempBuffer)->Handle;
    ((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotType = 0xB8;
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth:%x \n\n",__FUNCTION__,__LINE__,((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotDataBusWidth));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]StructureFoundPtr:%x SlotID:%d\n\n",__FUNCTION__,__LINE__,TempBuffer,((SMBIOS_SYSTEM_SLOTS_INFO*)TempBuffer)->SlotID));
    Status = SmbiosProtocol->SmbiosWriteStructure(Handle, TempBuffer, GetStructureLength(TempBuffer));
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Update Struct %r\n\n",__FUNCTION__,__LINE__,Status));
    if(TempBuffer!=NULL)
    pBS->FreePool(TempBuffer);
  }
  }

  /**
  Note:Remove Unuseful Slot Type -X745 Only have 11 Slots
  **/
  if((BoardId==0x07)||(BoardId==0x03))
      SlotNUM=12; //x7440 only have 11 Slots
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Remove Unuseful Slot!!!\n\n",__FUNCTION__,__LINE__));
    // SrcBuffer = (UINT8 *)(SmbiosTableEntryPoint->TableAddress); 
    // FindStructureType(&SrcBuffer, &StructureFoundPtr, 9, 12);
    // TempSrcBuffer = StructureFoundPtr;
    for (iSlotDescription = SlotNUM; iSlotDescription < NUMBER_OF_SYSTEM_SLOTS-19; iSlotDescription++)
    {
      SrcBuffer = (UINT8 *)(SmbiosTableEntryPoint->TableAddress); 
      /* code */
      if (FindStructureType(&SrcBuffer, &StructureFoundPtr, 9, SlotNUM)) //We need delete the 12th Type9 Table until the 38th 
      SmbiosProtocol->SmbiosDeleteStructure(((SMBIOS_STRUCTURE_HEADER*)StructureFoundPtr)->Handle);
    }
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]  Exit!!!\n\n",__FUNCTION__,__LINE__));
  return EFI_SUCCESS;
}

/**
GC_TODO: add routine description
@param         - GC_TODO: add arg description
@param         - GC_TODO: add arg description
@param         - GC_TODO: add arg description
@param         - GC_TODO: add arg description

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS UpdateSmbios_Entry(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS                  Status=EFI_SUCCESS;
  //VOID                        *Registration;

  InitAmiLib(ImageHandle, SystemTable);
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\n\n",__FUNCTION__,__LINE__));

  Status = UpdateTable9();

  return Status;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
