/* $NoKeywords:$ */
/**
 * @file
 *
 * DXIO initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <GnbDxio.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/GnbLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/DxioLibV1.h>
#include <Library/OemGpioResetControlLib.h>
#include "DxioLibLocal.h"
#include <Library/TimerLib.h>
#include <AmdSoc.h>
#include <Library/IoLib.h>
#include <GnbRegistersZP.h>
#include <Ppi/EqTablePpi.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/BaseCoreLogicalIdLib.h>

#include "SkipRxCfgDevList.h" //SGMTB#148601:System Report Rx Error When Install DCU Z100 >
#define FILECODE LIBRARY_DXIOLIBV1_DXIOLIBV1_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define DXIO_PCS_SMN_BASE_ADDRESS  0x12000000
#define PCIE_RC_0_SMN_BASE_ADDRESS 0x11100000
#define PCIE_RC_1_SMN_BASE_ADDRESS 0x11200000
#define MAXLANE 15
#define GetCfgData(DieName,CfgData)  {\
    switch(DieName){\
      case 0:\
        CfgData=PcdGet64(PcdDie0PCIeCfgIndex);\
        break;\
      case 1:\
        CfgData=PcdGet64(PcdDie1PCIeCfgIndex);\
        break;\
      case 2:\
        CfgData=PcdGet64(PcdDie2PCIeCfgIndex);\
        break;\
      case 3:\
        CfgData=PcdGet64(PcdDie3PCIeCfgIndex);\
        break;\
      case 4:\
        CfgData=PcdGet64(PcdDie4PCIeCfgIndex);\
        break;\
      case 5:\
        CfgData=PcdGet64(PcdDie5PCIeCfgIndex);\
        break;\
      case 6:\
        CfgData=PcdGet64(PcdDie6PCIeCfgIndex);\
        break;\
      case 7:\
        CfgData=PcdGet64(PcdDie7PCIeCfgIndex);\
        break;\
      default:\
        CfgData=0xFFFFFFFFFFFFFFFF;\
    }\
}

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
enum{
     LANE_ANA_RX_VERG_CTRL2_INDEX=0,
     LANE_DIG_ANA_RX_AFE_CTLE_INDEX,
     LANE_ANA_RX_AFE_OVER_INDEX,
     LANE_ANA_RX_AFE_OVER_RTRIM_INDEX,
     LANE_ANA_RX_AFE_OVER_ICTRL_INDEX,
     LANE_ANA_RX_CAL_MUXB_INDEX,
     LANE_ANA_RX_CAL_MUXA_INDEX,
     LANE_DIG_RX_CDR_CTL_3_INDEX,
     LANE_DIG_RX_CDR_CTL_4_INDEX
};
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
//SGMTB#148601:System Report Rx Error When Install DCU Z100+>
#define InvalidDeviceID 0xFFFF
#define InvalidVendotID 0xFFFF


typedef struct 
{
  //Rx Param
  UINT16   ANA_RX_VERG_CTRL2;
  UINT16   DIG_ANA_RX_AFE_CTLE;
  UINT16   ANA_RX_AFE_OVER;
  UINT16   ANA_RX_AFE_OVER_RTRIM;
  UINT16   ANA_RX_AFE_OVER_ICTRL;
  UINT16   ANA_RX_CAL_MUXB;
  UINT16   ANA_RX_CAL_MUXA;
  UINT16   DIG_RX_CDR_CTL_3;
  UINT16   DIG_RX_CDR_CTL_4;
  
  //Cpu Type Define
  UINT16   CpuBaseModle;
  PKG_TYPE CpuPkg;
  //Device Specific
  UINT16   VendorID;
  UINT16   DeviceID;
}PHY_PARAM_TABLE;

PHY_PARAM_TABLE  PhyParameterTable[]={
  OEM_RX_PARAM_LIST
  //{0,0,0,0,0,0,0,0,0,HYGON_REV_F18_DN_Cx,PKG_DM1,InvalidDidVid},
  {0xac,0x18,0xa6,0x7d,0x07,0x48,0x0d,0x1,0x1,HYGON_REV_F18_DN_Cx,PKG_SL1,InvalidVendotID,InvalidDeviceID},
  {0xac,0x18,0xa2,0xff,0x07,0x48,0x1d,0x1,0x1,HYGON_REV_F18_DN_Cx,PKG_SL1R2,InvalidVendotID,InvalidDeviceID},
  {0,0,0,0,0,0,0,0x1,0x1,HYGON_REV_F18_DN_Dx,PKG_DM1,InvalidVendotID,InvalidDeviceID},
  {0,0x3c,0,0,0,0,0,0x1,0x1,HYGON_REV_F18_DN_Dx,PKG_SL1,InvalidVendotID,InvalidDeviceID},
  {0,0,0,0,0,0,0,0,0,0xFF,0xFF,InvalidVendotID,InvalidDeviceID},
};

#if 0
UINT16 PhyParameterTable_Hygon_Cx[3][9] = {
    //VERG_CTRL2,AFE_CTLE,AFE_OVER_RTRIM,AFE_OVER_ICTRL,CAL_MUXB,CAL_MUXA,CDR_CTL_3_3_5,CTL_4_0_2
    {0,0,0,0,0,0,0,0,0},//DM1
    {0xac,0x18,0xa6,0x7d,0x07,0x48,0x0d,0x1,0x1},//SL1
    {0xac,0x18,0xa2,0xff,0x07,0x48,0x1d,0x1,0x1},//SL1R2
};

UINT16 PhyParameterTable_Hygon_Dx[2][9] = {
    //VERG_CTRL2,AFE_CTLE,AFE_OVER_RTRIM,AFE_OVER_ICTRL,CAL_MUXB,CAL_MUXA,CDR_CTL_3_3_5,CTL_4_0_2
    {0,0,0,0,0,0,0,0x1,0x1},//DM1
    {0,0x3c,0,0,0,0,0,0x1,0x1},//SL1
}; 
#endif

//SGMTB#148601:System Report Rx Error When Install DCU Z100->

/**
 * Read PHY register
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  DxioType       0=GOP TYPE A /1=GOP TYPE B /2=PCIE TYPE A /3=PCIE TYPE B /4=SATA
 * @param[in]  Lane           lane number
 * @param[in]  RegAddress     PHY register address
 * @param[in]  Lane           Read value
 * @retval
 */
EFI_STATUS
PhyRegRead(
    GNB_HANDLE*               GnbHandle,
    UINT8                     DxioType,
    UINT8                     Lane,
    UINT32                    RegAddress,
    UINT16                    *Value
)
{
    UINT32   Data32;
    UINT32   SmnAddr;
    UINT8    Phy;
    UINT8    PhyLane;
    
    if(Lane > 3){
        Phy  = Lane/4+1;
        PhyLane = Lane%4;
    } else {
        Phy  = Lane/2;
        PhyLane = Lane%2;
    }
    
    SmnAddr = 0x12000000+(DxioType<<20)+((Phy*2)<<16)+(RegAddress+(PhyLane<<8))*2;
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddr&0xFFFFFFFC, &Data32, 0); 
    if ((SmnAddr & 3) == 2) {
        *Value = (UINT16)(Data32 >> 16);
    } else if ((SmnAddr & 3) == 0) {
        *Value = (UINT16)Data32;
    } else {
        return EFI_INVALID_PARAMETER;
    }
    
    return EFI_SUCCESS;
}

/**
 * Write PHY register
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  DxioType       0=GOP TYPE A /1=GOP TYPE B /2=PCIE TYPE A /3=PCIE TYPE B /4=SATA
 * @param[in]  Lane           lane number
 * @param[in]  RegAddress     PHY register address
 * @param[in]  Lane           Write value
 * @retval
 */
EFI_STATUS
PhyRegWrite(
    GNB_HANDLE*               GnbHandle,
    UINT8                     DxioType,
    UINT8                     Lane,
    UINT32                    RegAddress,
    UINT16                    Value
)
{
    UINT32   Data32;
    UINT32   SmnAddr;
    UINT8    Phy;
    UINT8    PhyLane;
    
    if(Lane>3){
        Phy     = Lane / 4 + 1;
        PhyLane = Lane % 4;
    } else {
        Phy     = Lane / 2;
        PhyLane = Lane % 2;
    }
    
    SmnAddr = 0x12000000 + (DxioType << 20) + ((Phy*2) << 16) + (RegAddress + (PhyLane <<8 )) * 2;
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddr & 0xFFFFFFFC, &Data32, 0); 
    if((SmnAddr&3)==2){
        Data32 &= 0xFFFF;
        Data32 |= (Value<<16);
    } else if ((SmnAddr&3)==0){
        Data32 &= 0xFFFF0000;
        Data32 |= Value;     
    } else {
        return EFI_INVALID_PARAMETER;
    }   
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddr & 0xFFFFFFFC, &Data32, 0);
    
    return EFI_SUCCESS;
}

//Do pci reset for GEN4 PCIE CARD
VOID AssertPciReset(UINT32    BusNumber)
{
    UINT32 GpioLow = 0x800000;

    SmnRegisterWrite8 (BusNumber, 0x2d01d1a, 1);        //IoMux
    SmnRegisterWrite (BusNumber, 0x2D02568, GpioLow);  //GPIO
}

VOID DeAssertPciReset(UINT32    BusNumber)
{
    UINT32 GpioHigh = 0xC00000;

    SmnRegisterWrite8 (BusNumber, 0x2d01d1a, 1);        //IoMux
    SmnRegisterWrite (BusNumber, 0x2D02568, GpioHigh);  //GPIO
    SmnRegisterWrite8 (BusNumber, 0x2d01d1a, 0);        //IoMux
}

VOID PcieGen4PciReset (UINT32    BusNumber)
{
    SOC_LOGICAL_ID       LogicalId;

    BaseGetLogicalIdOnExecutingCore (&LogicalId);
    IDS_HDT_CONSOLE (GNB_TRACE, "CPU LogicalId Revision: 0x%X\n", LogicalId.Revision);
    
    if (LogicalId.Revision & HYGON_REV_F18_DN_GTE_C0) { //CPU is Cx or Dx
        AssertPciReset (BusNumber);
        MicroSecondDelay (200000);
        DeAssertPciReset (BusNumber);
        MicroSecondDelay (20000);
    }
}

VOID
DxioCfgBeforeReconfig (
  IN       GNB_HANDLE       *GnbHandle
  );

VOID
DxioCfgAfterReconfig (
  IN       GNB_HANDLE       *GnbHandle
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
DxioEntryConfigDump (
  IN       DXIO_ENGINE_DESCRIPTOR    *EngineDescriptor
  )
{
  UINT8       *Buffer;

  Buffer = (UINT8*) EngineDescriptor;
  //GnbLibDebugDumpBuffer ((VOID*) EngineDescriptor, sizeof(DXIO_ENGINE_DESCRIPTOR), 1, 16);
  IDS_HDT_CONSOLE (PCIE_MISC, "\n");

  IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %a\n",
    (EngineDescriptor->EngineType == DxioPcieEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineType == DxioSATAEngine) ? "SATA Port" : (
    (EngineDescriptor->EngineType == DxioEthernetEngine) ? "Ethernet Port" : (
    (EngineDescriptor->EngineType == DxioUnusedEngine) ? "Unused" : "Invalid")))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->StartLane,
    EngineDescriptor->EndLane
  );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Hotplug - %d\n", EngineDescriptor->HotPluggable);
  IDS_HDT_CONSOLE (PCIE_MISC, "    GpioGroupId - %d\n", EngineDescriptor->GpioGroupID);
  if (EngineDescriptor->EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    PortPresent - %d\n    maxLinkSpeedCap - %d\n" ,
      EngineDescriptor->LinkDesc.Pcie.Capabilities.portPresent,
      EngineDescriptor->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap
    );
    //IDS_HDT_CONSOLE (PCIE_MISC, "    TrainingState - %d\n", EngineDescriptor->TrainingState);
    IDS_HDT_CONSOLE (PCIE_MISC, "    MacPortID - %d\n", EngineDescriptor->LinkDesc.Pcie.MacPortID);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump the data being sent to SMU
 *
 *
 * @param[in]  PlatformDescriptor   Pointer to platform descriptor
 * @param[in]  PlatformDescSize     Size of platform descriptor
 * @param[in]  AncillaryData        Pointer to platform descriptor
 * @param[in]  AncillaryDataSize    Size of platform descriptor
 */
VOID
DxioDebugDump (
  IN       DXIO_PLATFORM_DESCRIPTOR  *PlatformDescriptor,
  IN       UINT32                    PlatformDescSize,
  IN       UINT8                     *AncillaryData,
  IN       UINT32                    AncillaryDataSize
  )
{
  DXIO_PLATFORM_DESCRIPTOR  *PlatformPointer;
  DXIO_ENGINE_DESCRIPTOR    *EnginePointer;
  UINT32                    EngineCount;
  UINT32                    Index;


  PlatformPointer = (DXIO_PLATFORM_DESCRIPTOR *) PlatformDescriptor;
  EngineCount = PlatformPointer->NumEngineDesc;
  IDS_HDT_CONSOLE (GNB_TRACE, "Number of Engines is %d\n", EngineCount);
  PlatformPointer++;
  EnginePointer = (DXIO_ENGINE_DESCRIPTOR *)PlatformPointer;
  for (Index = 0;Index < EngineCount; Index++) {
    DxioEntryConfigDump (EnginePointer);
    EnginePointer++;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Platform Descriptor to SMU...\n");
  GnbLibDebugDumpBuffer ((VOID*) PlatformDescriptor, PlatformDescSize << 2, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  if (AncillaryDataSize != 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Ancillary Data to SMU...\n");
    GnbLibDebugDumpBuffer ((VOID*) AncillaryData, AncillaryDataSize << 2, 3, 8);
    IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "No ancillary data to send\n");
  }
  return;
}


/**
 * Get Downstream PCIe Device vendor ID and Device ID
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    Downstream vendor ID and Device ID
 */
UINT32
GetTargetVidDid (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
    PCIe_WRAPPER_CONFIG               *Wrapper;
    UINT32                            RpSubBusCfgAddress, TargetPciVidDidAddr;
    UINT32                            VidDid;
    UINT32                            OldValue, NewValue;
    UINT32                            MmCfg;
    UINT8                             PrimaryBus, SecondaryBus, SubBusNum;
    UINT8                             MappingPortID;

    Wrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;

    RpSubBusCfgAddress = (Wrapper->WrapId == 0) ? SMN_PCIE0_SUB_BUS_NUMBER_LATENCY : SMN_PCIE1_SUB_BUS_NUMBER_LATENCY;
    RpSubBusCfgAddress += (MappingPortID * 0x1000);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Get Target VidDid: ");
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIeCore%02x ", Wrapper->WrapId);
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIePort%02x\n", MappingPortID);
    //save old sub bus config
    NbioRegisterRead (GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    //allocate bus for target
    PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
    SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    NewValue = 0;
    NewValue |= PrimaryBus;
    NewValue |= (SecondaryBus << 8);
    NewValue |= (SubBusNum << 16);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &NewValue, 0);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Cfg RP: Address=%08x CfgValue=%x\n", RpSubBusCfgAddress, NewValue);
    //read target viddid
    MmCfg = (UINT32)(AsmReadMsr64(0xC0010058) & 0xfff00000);
    TargetPciVidDidAddr = PCIE_VIDDID_ADDR(MmCfg, SecondaryBus, 0, 0);
    VidDid = MmioRead32 (TargetPciVidDidAddr);
    IDS_HDT_CONSOLE (GNB_TRACE, "Target VidDid: %08x\n", VidDid);
    //restore RP sub bus config
    NbioRegisterWrite (GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    
    return VidDid;
}

/**
 * Get Downstream PCIe Device subsystem vendor ID and Device ID
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    Downstream subsystem vendor ID and Device ID
 */
UINT32
GetTargetSubSystemVidDid (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
    PCIe_WRAPPER_CONFIG               *Wrapper;
    UINT32                            RpSubBusCfgAddress; 
    UINT32                            TargetPciSubSystemVidDidAddr;
    UINT32                            SubSystemVidDid;
    UINT32                            OldValue,NewValue;
    UINT32                            MmCfg;
    UINT8                             PrimaryBus, SecondaryBus, SubBusNum;
    UINT8                             MappingPortID;

    Wrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;

    RpSubBusCfgAddress = (Wrapper->WrapId == 0) ? SMN_PCIE0_SUB_BUS_NUMBER_LATENCY : SMN_PCIE1_SUB_BUS_NUMBER_LATENCY;
    RpSubBusCfgAddress += (MappingPortID * 0x1000);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Get Target SubSystem VidDid: ");
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIeCore%02x ", Wrapper->WrapId);
    IDS_HDT_CONSOLE (GNB_TRACE, "PCIePort%02x\n", MappingPortID);
    //save old sub bus config
    NbioRegisterRead (GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    //allocate bus for target
    PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
    SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
    NewValue = 0;
    NewValue |= PrimaryBus;
    NewValue |= (SecondaryBus<<8);
    NewValue |= (SubBusNum<<16);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &NewValue, 0);
    
    IDS_HDT_CONSOLE (GNB_TRACE, "Cfg RP: Address=%08x CfgValue=%x\n", RpSubBusCfgAddress, NewValue);
    //read target viddid
    MmCfg = (UINT32)(AsmReadMsr64 (0xC0010058) & 0xfff00000);
    TargetPciSubSystemVidDidAddr = PCIE_SUBSYSTEM_VIDDID_ADDR (MmCfg, SecondaryBus, 0, 0);
    SubSystemVidDid = MmioRead32 (TargetPciSubSystemVidDidAddr);
    IDS_HDT_CONSOLE (GNB_TRACE, "Target SubSystem VidDid: %08x\n", SubSystemVidDid);
    //restore RP sub bus config
    NbioRegisterWrite (GnbHandle, TYPE_SMN, RpSubBusCfgAddress, &OldValue, 0);
    
    return SubSystemVidDid;
}

/**
 * Get EQ Configuration Table
 *
 *
 *
 * @param[out]  EqCfgTable     Pointer to EQ Configuration Table
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
GetEqCfgTable (
    UINT8   DieNumber,//SGEZT#25974-Improve-Code-For-Preset
    OUT HYGON_CPM_EQ_CONFIG_TABLE**  EqCfgTable
)
{
    EFI_PEI_SERVICES            **PeiServices;
    EFI_STATUS                  Status;
    HYGON_CPM_EQ_CONFIG_TABLE*  pEqCfgTable;
    EFI_PEI_EQ_TABLE_PPI        *EqTablePpi;

    PeiServices = (EFI_PEI_SERVICES**)GetPeiServicesTablePointer ();

    Status = (*PeiServices)->LocatePpi (
                                PeiServices,
                                &gEqTablePpiGuid,
                                0,
                                NULL,
                                (VOID**)&EqTablePpi
                                );
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    Status = EqTablePpi->GetCfgTable (PeiServices, DieNumber,&pEqCfgTable);
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    EqTablePpi->DumpEqCfgTable (PeiServices, pEqCfgTable);
    EqTablePpi->PreprocessEqCfgTable (PeiServices, pEqCfgTable);
    *EqCfgTable = pEqCfgTable;

    return EFI_SUCCESS;
}

/**
 * PCIe redo EQ
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    void
 */
VOID
PciePerformEqualization (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
    PCIE_LINK_CNTL_STRUCT             LinkCtl;
    PCIE_LINK_CNTL3_STRUCT            LinkCtl3;
    PCIe_WRAPPER_CONFIG               *PcieWrapper;
    UINT8                             MappingPortID;
    UINT32                            PcieRcCfgSmnBase;
    
    PcieWrapper      = PcieConfigGetParentWrapper (Engine);
    MappingPortID    = (Engine->Type.Port.PortId) % 8;
    PcieRcCfgSmnBase = (Engine->Type.Port.PortId < 8) ? PCIE_RC_0_SMN_BASE_ADDRESS : PCIE_RC_1_SMN_BASE_ADDRESS;
    
    //redo EQ
    IDS_HDT_CONSOLE (PCIE_MISC, "PCIe Perform Equalization\n");
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
    LinkCtl3.Field.PERFORM_EQUALIZATION = 1;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
    
    //force retrain link
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value, 0);
    LinkCtl.Field.RETRAIN_LINK =1;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value, 0);
    
    //delay 100ms
    MicroSecondDelay (100*1000);
    
    //set PERFORM_EQUALIZATION to 0
    //NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
    //LinkCtl3.Field.PERFORM_EQUALIZATION = 0;
    //NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
}

/**
 * PCIe Perform EqualizationSetting 
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  PerformEq      TRUE = Perform EqualizationSetting , FALSE = Don't Perform EqualizationSetting 
 * 
 * @retval    Old Perform EqualizationSetting status
 */
BOOLEAN
PciePerformEqualizationSetting (
    IN     GNB_HANDLE               *GnbHandle,
    IN     PCIe_ENGINE_CONFIG       *Engine,
    IN     BOOLEAN                  PerformEq
)
{

  PCIE_LINK_CNTL3_STRUCT            LinkCtl3;
  PCIe_WRAPPER_CONFIG               *PcieWrapper;
  UINT8                             MappingPortID;
  UINT32                            PcieRcCfgSmnBase;
  UINT32                            DoEq;
  BOOLEAN                           OldPerformEqStatus;
  
  PcieWrapper      = PcieConfigGetParentWrapper (Engine);
  MappingPortID    = (Engine->Type.Port.PortId) % 8;
  PcieRcCfgSmnBase = (Engine->Type.Port.PortId < 8) ? PCIE_RC_0_SMN_BASE_ADDRESS : PCIE_RC_1_SMN_BASE_ADDRESS;
  
  if (PerformEq) {
    DoEq = 1;
  } else {
    DoEq = 0;
  }
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
  OldPerformEqStatus = (LinkCtl3.Field.PERFORM_EQUALIZATION == 1) ? TRUE : FALSE;
  LinkCtl3.Field.PERFORM_EQUALIZATION = DoEq;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET, &LinkCtl3.Value, 0);
  
  return OldPerformEqStatus;
}

/**
 * PCIe Retraining
 *
 * @param[in]  Die            Die number
 * @param[in]  Port           PCIe Port number
 * @param[in]  ForceEq        TRUE = Retraining with EQ, FALSE = Retraining without EQ
 * 
 * @retval    void
 */
VOID
PcieRetraining (
    IN     GNB_HANDLE               *GnbHandle,
    IN     PCIe_ENGINE_CONFIG       *Engine,
    IN     BOOLEAN                  ForceEq
)
{
  PCIE_LINK_CNTL_STRUCT             LinkCtl;
  PCIe_WRAPPER_CONFIG               *PcieWrapper;
  UINT8                             MappingPortID;
  UINT32                            PcieRcCfgSmnBase;
  BOOLEAN                           OldPerformEqStatus;
  
  PcieWrapper      = PcieConfigGetParentWrapper (Engine);
  MappingPortID    = (Engine->Type.Port.PortId) % 8;
  PcieRcCfgSmnBase = (Engine->Type.Port.PortId < 8) ? PCIE_RC_0_SMN_BASE_ADDRESS : PCIE_RC_1_SMN_BASE_ADDRESS;
    
  OldPerformEqStatus = PciePerformEqualizationSetting (GnbHandle, Engine, ForceEq);
  
  //force retrain link
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value, 0);
  LinkCtl.Field.RETRAIN_LINK =1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET, &LinkCtl.Value, 0);
  
  if (ForceEq)
    //delay 50ms
    MicroSecondDelay (50*1000);
  
  PciePerformEqualizationSetting (GnbHandle, Engine, OldPerformEqStatus);
}
/**
 * PCIe Wait to L0
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  MaxDelayMs     Max Delay Times to L0(ms)
 * 
 * @retval    TURE  PCIe wait L0 success
 *            FALSE PCIe wait L0 fail
 */
BOOLEAN
PcieWaitToL0 (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     UINT32                     MaxDelayMs
)
{
  UINT32                              i;
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  PCIE_LC_STATE0_STRUCT               LcState0;
  BOOLEAN                             WaitStatus = FALSE;
  
  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  for (i = 0; i < MaxDelayMs; i++) {
    MicroSecondDelay (1000);
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_STATE0_ADDRESS + (PcieWrapper->WrapId << 20) + (MappingPortID << 12), &LcState0.Value, 0);
    if ((UINT8)LcState0.Field.LC_CURRENT_STATE == 0x10) {
      WaitStatus = TRUE;
      break;
    }  
  }
  
  if (WaitStatus) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Wait to L0 Success, Wait %d ms\n", i);
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "Wait to L0 Fail, Time out\n", i);
  }
  
  return WaitStatus;
}

/**
 * PCIe Retry to L0
 *
 * @param[in]  GnbHandle            Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine               Pointer to engine config descriptor
 * @param[in]  RetryCount           Retry Count
 * @param[in]  MaxDelayMsPerTry     Max Delay Times to L0(ms) PreTry
 * 
 * @retval    TURE  PCIe Retry to L0 success
 *            FALSE PCIe Retry to L0 fail
 */
BOOLEAN
PcieRetryToL0 (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     UINT32                     RetryCount,
    IN     UINT32                     MaxDelayMsPerTry
)
{
  UINT32                              i;
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  BOOLEAN                             RetryStatus = FALSE;
  
  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  for (i = 0; i < RetryCount; i++) {
    //Reqo EQ
    PciePerformEqualization (GnbHandle, Engine);
    //wait to L0
    if (PcieWaitToL0 (GnbHandle, Engine, MaxDelayMsPerTry)) {
      RetryStatus = TRUE;
      break;
    }
  }
  
  if (RetryStatus) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Retry to L0 Success, Retry count = %d\n", i);
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "Retry to L0 Fail, Retry count = %d\n", RetryCount);
  }
  
  return RetryStatus;
}

/**
 * PCIe Clear All errors
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    VOID
 */
VOID
PcieClearAllErrors (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine
)
{
  PCIe_WRAPPER_CONFIG                 *PcieWrapper;
  UINT8                               MappingPortID;
  UINT32                              Data32;
  
  PcieWrapper   = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  IDS_HDT_CONSOLE (GNB_TRACE, "Pcie Clear All Errors for node%x - core%x - port%x\n", GnbHandle->InstanceId, PcieWrapper->WrapId , MappingPortID);
  // 1. clear CE/UCE/Root Errors
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_UNCORR_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: UCE status=%x\n", Data32);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_UNCORR_ERR_STATUS_OFFSET, &Data32, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_UNCORR_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "After Clear: UCE status=%x\n", Data32);
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_CORR_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: CE status=%x\n", Data32);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_CORR_ERR_STATUS_OFFSET, &Data32, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_CORR_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "After Clear: CE status=%x\n", Data32);
  
  
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_ROOT_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: Root Error status=%x\n", Data32);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_ROOT_ERR_STATUS_OFFSET, &Data32, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_ROOT_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "After Clear: Root Error status=%x\n", Data32);
  
  // 2. clear device status errors
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: device Error status=%x\n", Data32);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET, &Data32, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "After Clear: device Error status=%x\n", Data32);
  
  // 3. clear lane errors
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LANE_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "Before Clear: lane Error status=%x\n", Data32);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LANE_ERR_STATUS_OFFSET, &Data32, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LANE_ERR_STATUS_OFFSET, &Data32, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "After Clear: lane Error status=%x\n", Data32);
  
}

/**
 * PCIe Link Speed change
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  NewSpeed       Link Speed change value
 * @param[in]  OldSpeed       Old Link Speed value
 * 
 * @retval    VOID
 */
VOID
PcieSpeedChange (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     UINT8                      NewSpeed,
    OUT    UINT8                      *OldSpeed
)
{
    PCIE_LINK_CNTL2_STRUCT            LinkCtl2;
    PCIe_WRAPPER_CONFIG               *PcieWrapper;
    UINT8                             MappingPortID;
    PCIE_LINK_CNTL_STRUCT             LinkCtl;
    PCIE_LINK_STATUS_STRUCT           LinkStatus;
    UINT32                            SmnAddress;
    
    PcieWrapper   = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;
    
    //read current link speed
    if (OldSpeed != NULL) {
      SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET;
      NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LinkStatus.Value, 0);
      *OldSpeed = (UINT8)LinkStatus.Field.CURRENT_LINK_SPEED;
    }
    
    //speed change to NewSpeed
    SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL2_OFFSET;
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl2.Value, 0);
    LinkCtl2.Field.TARGET_LINK_SPEED = NewSpeed;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl2.Value, 0);
    
    //retraining link
    SmnAddress = PCIE_RC_REGS_SMN_BASE + (PcieWrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL_OFFSET;
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl.Value, 0);
    LinkCtl.Field.RETRAIN_LINK = 1;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl.Value, 0);
}

/**
 * PCIe Other Side Supported Speed
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Other Side Supported Speed
 */
UINT8
PcieOtherSideSupportedSpeed (    
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  PCIE_LC_CNTL_10           LcCntl10;
  PCIE_LC_SPEED_CNTL_STRUCT LcSpeed;

  //check target supported speed
  if (PcieEngine->Type.Port.PortId > 7) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_CNTL_10_ADDRESS + ((PcieEngine->Type.Port.PortId - 8) << 12), &LcCntl10.Value, 0);
  } else {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_CNTL_10_ADDRESS + (PcieEngine->Type.Port.PortId << 12), &LcCntl10.Value, 0);
  }
  if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 1)
    return 4;
  
  if (PcieEngine->Type.Port.PortId > 7) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((PcieEngine->Type.Port.PortId - 8) << 12), &LcSpeed.Value, 0);
  } else {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (PcieEngine->Type.Port.PortId << 12), &LcSpeed.Value, 0);
  }
  if (LcSpeed.Field.LC_OTHER_SIDE_SUPPORTS_GEN3 == 1)
    return 3;
  
  if (PcieEngine->Type.Port.PortId > 7) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_SPEED_CNTL_ADDRESS + ((PcieEngine->Type.Port.PortId - 8) << 12), &LcSpeed.Value, 0);
  } else {
    NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_SPEED_CNTL_ADDRESS + (PcieEngine->Type.Port.PortId << 12), &LcSpeed.Value, 0);
  }
  if (LcSpeed.Field.LC_OTHER_SIDE_SUPPORTS_GEN2 == 1)
    return 2;
  
  return 1;
  
}

/**
 * PCIe Current Link Speed
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Current Link Speed
 */
UINT32
PcieCurrentLinkSpeed (
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  UINT32                            PcieCore;
  UINT32                            MappingPortID;
  PCIE_LINK_STATUS_STRUCT           LinkStatus;
  
  PcieCore         = PcieEngine->Type.Port.PortId / 8;
  MappingPortID    = PcieEngine->Type.Port.PortId % 8;

  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET, &LinkStatus.Value, 0);
  
  return LinkStatus.Field.CURRENT_LINK_SPEED;
}

/**
 * PCIe Current Link Width
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Current Link Width
 */
UINT32
PcieCurrentLinkWidth (
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  UINT32                            PcieCore;
  UINT32                            MappingPortID;
  PCIE_LINK_STATUS_STRUCT           LinkStatus;
  
  PcieCore         = PcieEngine->Type.Port.PortId / 8;
  MappingPortID    = PcieEngine->Type.Port.PortId % 8;

  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET, &LinkStatus.Value, 0);
  
  return LinkStatus.Field.NEGOTIATED_LINK_WIDTH;
}

/**
 * PCIe Link Speed Cap
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Link Speed Cap
 */
UINT32
PcieCapLinkSpeed (
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  UINT32                            PcieCore;
  UINT32                            MappingPortID;
  PCIE_LINK_CAP_STRUCT              LinkCap;
  
  PcieCore         = PcieEngine->Type.Port.PortId / 8;
  MappingPortID    = PcieEngine->Type.Port.PortId % 8;

  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CAP_OFFSET    , &LinkCap.Value, 0);
  
  return LinkCap.Field.LINK_SPEED;
}

/**
 * PCIe Link Width Cap
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Link Width Cap
 */
UINT32
PcieCapLinkWidth (
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  UINT32                            PcieCore;
  UINT32                            MappingPortID;
  PCIE_LINK_CAP_STRUCT              LinkCap;
  
  PcieCore         = PcieEngine->Type.Port.PortId / 8;
  MappingPortID    = PcieEngine->Type.Port.PortId % 8;

  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CAP_OFFSET    , &LinkCap.Value, 0);
  
  return LinkCap.Field.LINK_WIDTH;
}

/**
 * PCIe DL Activate status
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe DL Activate status
 */
BOOLEAN
PcieDlActivate (
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  UINT32                            PcieCore;
  UINT32                            MappingPortID;
  PCIE_LINK_STATUS_STRUCT           LinkStatus;
  
  PcieCore         = PcieEngine->Type.Port.PortId / 8;
  MappingPortID    = PcieEngine->Type.Port.PortId % 8;

  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE_RC_REGS_SMN_BASE + (PcieCore << 20) + (MappingPortID << 12) + RC_PCIE_LINK_STATUS_OFFSET, &LinkStatus.Value, 0);
  
  return (LinkStatus.Field.DL_ACTIVE == 1) ? TRUE : FALSE;
}

/**
 * PCIe Link Speed Waring
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    PCIe Link Speed Waring status
 */
BOOLEAN
PcieLinkSpeedWaring(
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine
)
{
  BOOLEAN                     SpeedWaring = FALSE;
  UINT32                      CurrentSpeed; 
  UINT32                      CapSpeed;
  UINT32                      TargetSupportedSpeed;

  CurrentSpeed = PcieCurrentLinkSpeed (GnbHandle, PcieEngine);
  //GET TARGET SUPPORTED SPEED
  TargetSupportedSpeed = PcieOtherSideSupportedSpeed (GnbHandle, PcieEngine);
  //GET CAP SPEED
  CapSpeed = PcieCapLinkSpeed (GnbHandle, PcieEngine);
  if(CapSpeed > PcdGet16(PcdMaxCap))
    PcdSet16S (PcdMaxCap, CapSpeed);

  if (CapSpeed < TargetSupportedSpeed) {
    if (CapSpeed > CurrentSpeed) {
      SpeedWaring = TRUE;
    } else {
      SpeedWaring = FALSE;
    }
  } else {
    if (TargetSupportedSpeed > CurrentSpeed) {
      SpeedWaring = TRUE;
    } else {
      SpeedWaring = FALSE;
    }  
  }
  
  return SpeedWaring;
}

/**
GC_TODO: DoRiserCardCheck For Same Config But With Diff RiserCard

@retval EFI_SUCCESS   - GC_TODO: Match Success
@retval EFI_NOT_FOUND - GC_TODO: Continue Loop
**/
EFI_STATUS
DoRiserCardCheck(
  IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
  IN     UINT8                      EqCfgOffset
)
{
  UINT8    DieNumber=EqCfgTable->EqCfgList[EqCfgOffset].Die;
  UINT64    iPCIePort=0;

  UINT8     iCfg=0,CfgOffset=0;
  
  GetCfgData(DieNumber,iPCIePort);

  IDS_HDT_CONSOLE (GNB_TRACE, "%a %d  Die%d->iPCIePort=%lx\n", __FUNCTION__, __LINE__,EqCfgTable->EqCfgList[EqCfgOffset].Die,iPCIePort);

  if(EqCfgTable->EqCfgList[EqCfgOffset].CfgOffset == 0xFF)
    return EFI_SUCCESS;

  IDS_HDT_CONSOLE (GNB_TRACE, "%a %d  Start Check Riser Type!!!\n", __FUNCTION__, __LINE__);

  //@todo  loop Valid Cfg Port
  for (iCfg = 0; iCfg < 8; iCfg++)
  {
    /* code */
    CfgOffset=(UINT8)(iPCIePort>>(iCfg*8));
    IDS_HDT_CONSOLE (GNB_TRACE, "[OemDbg][%a][%d]CfgOffset:%x  DesCfgOffset:%x\r\n",__FUNCTION__,__LINE__,CfgOffset,EqCfgTable->EqCfgList[EqCfgOffset].CfgOffset);
    if(CfgOffset == 0xFF)
      return EFI_NOT_FOUND;
    
    if(CfgOffset == EqCfgTable->EqCfgList[EqCfgOffset].CfgOffset){
      IDS_HDT_CONSOLE(GNB_TRACE,"%a %d Riser[%x] Match Success!!!\n",__FUNCTION__,__LINE__,CfgOffset);
      return EFI_SUCCESS;
    }
  }
  
  return EFI_NOT_FOUND;
}
/**
 * PCIe HwInit Enable Write
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 */

VOID
EnHwInitWRLock(
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *Engine       
)
{
    PCIe_WRAPPER_CONFIG       *Wrapper;
    UINT32                    SmnAddress;
    UINT32                    Data32;
    
    Wrapper = PcieConfigGetParentWrapper (Engine);
    SmnAddress = (Wrapper->WrapId == 0) ? PCIE0_CNTL_ADDRESS : PCIE1_CNTL_ADDRESS;
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
    Data32 &= ~(BIT0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
}


/**
 * PCIe HwInit Write Lock
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 */

VOID
HwInitWRLock(
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *Engine       
)
{
    PCIe_WRAPPER_CONFIG       *Wrapper;
    UINT32                    SmnAddress;
    UINT32                    Data32;
    
    Wrapper = PcieConfigGetParentWrapper (Engine);
    SmnAddress = (Wrapper->WrapId == 0) ? PCIE0_CNTL_ADDRESS : PCIE1_CNTL_ADDRESS;
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
    Data32 |= BIT0;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
}

/**
 * PCIe LC CNTL Reset Link
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 */

VOID
ResetLink(
    GNB_HANDLE                *GnbHandle,
    PCIe_ENGINE_CONFIG        *Engine,
    UINT8                     MappingPortID
)
{
    PCIe_WRAPPER_CONFIG       *Wrapper;
    UINT32                    SmnAddress;
    
    Wrapper = PcieConfigGetParentWrapper (Engine);
    //Set PCIe_LC_CNTL.LC_RESET_LINK as 1
    SmnAddress = (Wrapper->WrapId == 0) ? SMN_0x11140280_ADDRESS : SMN_0x11240280_ADDRESS;
    NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress | (MappingPortID << 12),
            ~((UINT32) PCIE_LC_CNTL_LC_RESET_LINK_MASK),
            (0x1 << PCIE_LC_CNTL_LC_RESET_LINK_OFFSET),
            0
    );
    //delay 50ms
    MicroSecondDelay (50*1000);
    
    //Set PCIe_LC_CNTL.LC_RESET_LINK as 0
    SmnAddress = (Wrapper->WrapId == 0) ? SMN_0x11140280_ADDRESS : SMN_0x11240280_ADDRESS;
    NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress | (MappingPortID << 12),
            ~((UINT32) PCIE_LC_CNTL_LC_RESET_LINK_MASK),
            (0 << PCIE_LC_CNTL_LC_RESET_LINK_OFFSET),            
            0
    );
    
}

/**
 * Get The Index of EQ Configuration Table witch match current PCIe Engine
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  EqCfgTable     Pointer to EQ Configuration Table
 * @param[in]  StartIndex     EQ Configuration Table Start Index for Search
 * @param[in]  Phase          Before PCIe Training = 0, After PCIe Training = 0
 * @param[out] Index          Index of EQ Configuration Table witch match current PCIe Engine
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
PcieMatchEqCfg (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
    IN     UINT32                     StartIndex,
    IN     UINT8                      Phase,                    
    OUT    UINT32                     *Index
)
{
    CPU_GEN                    CpuGen;
    UINT32                     VidDid, SubSystemVidDid;
    UINT32                     i;
    UINT32                     StartPhyLane, EndPhyLane;
    UINT32                     ForceSearchIndex = 0xffffffff;
    UINT32                     SpecifiedSearchIndex = 0xffffffff;
    EFI_STATUS                Status;

    if(Index == NULL){
      return EFI_INVALID_PARAMETER;
    }
    CpuGen = GetModelId (); 
    StartPhyLane = Engine->EngineData.StartLane;
    EndPhyLane   = Engine->EngineData.EndLane;
    IDS_HDT_CONSOLE (GNB_TRACE, "StartPhyLane=%x EndPhyLane=%x\n", StartPhyLane, EndPhyLane);
    StartPhyLane %= 32;
    EndPhyLane   %= 32;
    IDS_HDT_CONSOLE (GNB_TRACE, "After StartPhyLane=%x EndPhyLane=%x\n", StartPhyLane, EndPhyLane);
    
    for (i = StartIndex; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
        if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) != pcie_eq_type)
            continue;
        if (((Phase == 0) && (EqCfgTable->EqCfgList[i].Type < pcie_txx_cfg_before_training)) ||
            ((Phase == 1) && (EqCfgTable->EqCfgList[i].Type > pcie_txx_cfg_before_training)))
            continue;
        if (EqCfgTable->EqCfgList[i].Flag & (FLAG_THROW | FLAG_USED))
            continue;
        if (CpuGen != EqCfgTable->EqCfgList[i].CpuGen)
            continue;
        if (GnbHandle->InstanceId != EqCfgTable->EqCfgList[i].Die)
            continue;
        if ((StartPhyLane == EqCfgTable->EqCfgList[i].StartLane) && (EndPhyLane == EqCfgTable->EqCfgList[i].EndLane)){
            
            //Mizl-20230216-Check Riser Card +>
            Status=DoRiserCardCheck(EqCfgTable,i);
            IDS_HDT_CONSOLE (GNB_TRACE, "DoRiserCardCheck%r\n", Status);

            if(EFI_ERROR (Status))
                continue;
            //Mizl-20230216-Check Riser Card ->


            if (EqCfgTable->EqCfgList[i].Flag & FLAG_SPECIFIED){
              {
                UINT16   iLoop = 0; //SGEZB#29635:CPU Dead Loop When No DID&VID Match
                for ( iLoop = 0; iLoop < 600; iLoop++)
                {
                  VidDid = GetTargetVidDid (GnbHandle, Engine);
                  if(VidDid != 0xFFFFFFFF)
                    break;
                  IDS_HDT_CONSOLE(GNB_TRACE,"No Device Found!!\rin VidDid:%x Target VidDid:%x\rn",VidDid,EqCfgTable->EqCfgList[i].VidDid);
                  MicroSecondDelay(1000);
                }
                
              }
                

                if ((VidDid == 0xffffffff) || (VidDid != EqCfgTable->EqCfgList[i].VidDid)) {
                    EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
                    continue;
                }
                if (EqCfgTable->EqCfgList[i].SubSysVidDid != 0xffffffff) {
                    SubSystemVidDid = GetTargetSubSystemVidDid (GnbHandle, Engine);
                    if (SubSystemVidDid != EqCfgTable->EqCfgList[i].SubSysVidDid)
                        continue;
                    EqCfgTable->EqCfgList[i].Flag |= FLAG_USED;
                    IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: SSID SpecifiedSearchIndex=%x\n", i);
                    *Index = i;
                    return EFI_SUCCESS;
                }
                if (SpecifiedSearchIndex != 0xffffffff)
                    continue;
                SpecifiedSearchIndex = i;
                EqCfgTable->EqCfgList[i].Flag |= FLAG_USED;
                IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: SpecifiedSearchIndex=%x\n", SpecifiedSearchIndex);
                break;
            } else if (EqCfgTable->EqCfgList[i].Flag & FLAG_FORCE) {
                if (ForceSearchIndex != 0xffffffff)
                    continue;              
                ForceSearchIndex = i;
                EqCfgTable->EqCfgList[i].Flag |= FLAG_USED;
                IDS_HDT_CONSOLE (GNB_TRACE, "Table Matched: ForceSearchIndex=%x\n", ForceSearchIndex);
                break;
            }
        } else {
            //EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
            continue;
        }
    }
    if (SpecifiedSearchIndex != 0xffffffff) {
        *Index = SpecifiedSearchIndex;
        return EFI_SUCCESS;
    }
    if (ForceSearchIndex != 0xffffffff) {
        *Index = ForceSearchIndex;
        return EFI_SUCCESS;
    }
    return EFI_NOT_FOUND;
}

/**
 * Configure EQ for current PCIe Engine 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * @param[in]  EqCfgTable     Pointer to EQ Configuration Table
 * @param[in]  Index          Index of EQ Configuration Table witch match current PCIe Engine
 * @param[in]  Phase          Before PCIe Training = 0, After PCIe Training = 0
 * @param[out] RequestRedoEq  Request RedoEq status
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
DoPcieEqCfg (
    IN     GNB_HANDLE                 *GnbHandle,
    IN     PCIe_ENGINE_CONFIG         *Engine,
    IN     HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable,
    IN     UINT32                     Index,
    IN     UINT8                      Phase,
    OUT    BOOLEAN                    *RequestRedoEq    
)
{
    PCIe_WRAPPER_CONFIG               *Wrapper;
    UINT8                             MappingPortID;
    UINT32                            SmnAddress;
    PCIE_LC_CNTL4                     PcieLcCntl4;
    PCIE_LC_CNTL8                     PcieLcCntl8;  
    PCIE_LINK_CNTL3_STRUCT            LinkCtl3;
    PCIE_LANE_EQ_CNTL_STRUCT          PcieEqCntl;
    PCIE_LANE_EQ_CNTL_16GT_STRUCT     PcieEqGen4Cntl;
    UINT8                             OldLinkSpeed;
    UINT8                             i;
    UINT16                            Data16;
    UINT32                            CfgValue;
    UINT16                            Cdr_Data;
    UINT8                             DxioType;
    UINT32                            Data32;
    UINT8                             Lane;
    
    if (RequestRedoEq == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    
    *RequestRedoEq = FALSE;
    Wrapper = PcieConfigGetParentWrapper (Engine);
    MappingPortID = (Engine->Type.Port.PortId) % 8;
    DxioType  = (Engine->Type.Port.PortId < 8) ? 2 : 3;
    
    if (Phase == 0) {
      switch (EqCfgTable->EqCfgList[Index].Type) {
        case pcie_gen3_ttx_cursor_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_cursor_before_training\n");
            SmnAddress = PCIE_PORT_REGS_SMN_BASE + PCIE_LC_FORCE_COEFF_OFFSET + (Wrapper->WrapId << 20) + (MappingPortID << 12);
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            Data32 &= 0xfff80000;
            Data32 |= (GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 1);
            Data32 |= (GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 7);
            Data32 |= (GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 13);
            Data32 |= 1;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            break;
        case pcie_gen4_ttx_cursor_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_cursor_before_training\n");
            SmnAddress = PCIE_PORT_REGS_SMN_BASE + PCIE_LC_FORCE_COEFF2_OFFSET + (Wrapper->WrapId << 20) + (MappingPortID << 12);
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            Data32 &= 0xfff80000;
            Data32 |= (GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 1);
            Data32 |= (GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 7);
            Data32 |= (GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 13);
            Data32 |= 1;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            break;
        case pcie_txx_force_iboost_lvl_before_training:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_txx_force_iboost_lvl\n");
          for (i = EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
              //set pre lane value
            if (EqCfgTable->EqCfgList[Index].PerPort) { 
              CfgValue = EqCfgTable->EqCfgList[Index].CfgValue[0];
            } else {
              CfgValue = EqCfgTable->EqCfgList[Index].CfgValue[i - EqCfgTable->EqCfgList[Index].StartLane];
            }
            PhyRegRead (GnbHandle, DxioType, (UINT8)(i % 16), LANE_DIG_PCS_XF_TX_OVER_IN_3, &Data16);
            Data16 &= (UINT32)(~(BIT4 | BIT5 | BIT6 | BIT7));
            Data16 |= (UINT32)(BIT2 | BIT3 | BIT8);
            Data16 |= (UINT32)(CfgValue << 4);
            PhyRegWrite (GnbHandle, DxioType, (UINT8)(i % 16), LANE_DIG_PCS_XF_TX_OVER_IN_3, Data16); 
          } 
          break;
        case pcie_gen3_ttx_force_otherside_preset_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_force_otherside_preset_before_training\n");
            if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_PORT_REGS_SMN_BASE + (Wrapper->WrapId << 20) + PCIE_LC_CNTL4_REG_OFFSET + (MappingPortID << 12);
                NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl4.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl4=%x\n", PcieLcCntl4.Value);
                PcieLcCntl4.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE = 1;
                PcieLcCntl4.Reg.LC_FORCE_PRESET_VALUE = EqCfgTable->EqCfgList[Index].CfgValue[0];
                PcieLcCntl4.Reg.LC_EQ_SEARCH_MODE = 3;
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl4=%x\n", PcieLcCntl4.Value);
                NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl4.Value, 0);
            } else {
                for (i=EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
                    //set pre lane value
                }
            }
            break;
        case pcie_gen4_ttx_force_otherside_preset_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_force_otherside_preset_before_training\n");
            if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_PORT_REGS_SMN_BASE + (Wrapper->WrapId << 20) + PCIE_LC_CNTL8_REG_OFFSET + (MappingPortID << 12);
                NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl8.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl8=%x\n", PcieLcCntl8.Value);
                PcieLcCntl8.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT = 1;
                PcieLcCntl8.Reg.LC_FORCE_PRESET_VALUE_16GT = EqCfgTable->EqCfgList[Index].CfgValue[0];
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieLcCntl8=%x\n", PcieLcCntl8.Value);
                NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl8.Value, 0);
            } else {
                for (i = EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
                    //set pre lane value
                }
            }
            break;
            
        case pcie_gen3_ttx_force_initial_preset_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_force_initial_preset_before_training\n");
            //Enable HWINIT_WR_LOCK writable
            EnHwInitWRLock(GnbHandle, Engine);
            //Set Gen3 PCIe_Lane_equalization Downstream_port_tx_preset
            for (i = 0; i <= MAXLANE; i++) {
              if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (PCIE_LANE_EQUALIZATION_CNTL_OFFSET+(i*2)) + (MappingPortID << 12);
                IDS_HDT_CONSOLE (GNB_TRACE, "PCIE_RC_Address_gen3=%x\n",SmnAddress);
                NbioSmnRegisterRead16 (GnbHandle, SmnAddress, &PcieEqCntl.Value);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieEqCntl=%x\n", PcieEqCntl.Value);
                PcieEqCntl.Field.DOWNSTREAM_PORT_TX_PRESET = (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[0]); 
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieEqCntl=%x\n", PcieEqCntl.Value);
                NbioSmnRegisterWrite16 (GnbHandle, SmnAddress, &PcieEqCntl.Value, 0);
              }
            } 
            //Unlock HWINIT_WR_LOCK
            HwInitWRLock(GnbHandle, Engine);
            //Reset PCIe_LC_CNTL.LC_RESET_LINK
            ResetLink(GnbHandle, Engine, MappingPortID);       
            break;
            
        case pcie_gen3_ttx_force_otherside_initial_preset_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_force_initial_preset_before_training\n");
            //Enable HWINIT_WR_LOCK writable
            EnHwInitWRLock(GnbHandle, Engine);
            //Set Gen3 PCIe_Lane_equalization UpStream_port_tx_preset
            for (i = 0; i <= MAXLANE; i++) {
              if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (PCIE_LANE_EQUALIZATION_CNTL_OFFSET+(i*2)) + (MappingPortID << 12);
                IDS_HDT_CONSOLE (GNB_TRACE, "PCIE_RC_Address_gen3=%x\n",SmnAddress);
                NbioSmnRegisterRead16 (GnbHandle, SmnAddress, &PcieEqCntl.Value);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieEqCntl=%x\n", PcieEqCntl.Value);
                PcieEqCntl.Field.UPSTREAM_PORT_TX_PRESET = (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[0]);
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieEqCntl=%x\n", PcieEqCntl.Value);
                NbioSmnRegisterWrite16 (GnbHandle, SmnAddress, &PcieEqCntl.Value, 0);
              }
            } 
            //Unlock HWINIT_WR_LOCK
            HwInitWRLock(GnbHandle, Engine);
            //Reset PCIe_LC_CNTL.LC_RESET_LINK
            ResetLink(GnbHandle, Engine, MappingPortID);       
            break;
            
        case pcie_gen4_ttx_force_initial_preset_before_training: 
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_DSUP_stream_preset_before_training\n");
            //Enable HWINIT_WR_LOCK writable
            EnHwInitWRLock(GnbHandle, Engine);
            //Set Gen4 PCIe_Lane_equalization Downstream_port_tx_preset
            for (i = 0; i <= MAXLANE; i++) {
              if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (PCIE_LANE_EQUALIZATION_CNTL_16GT_OFFSET + i) + (MappingPortID << 12);
                IDS_HDT_CONSOLE (GNB_TRACE, "PCIE_RC_Address_gen4=%x\n", SmnAddress);
                NbioSmnRegisterRead8 (GnbHandle, SmnAddress, &PcieEqGen4Cntl.Value);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieEqGen4Cntl=%x\n", PcieEqGen4Cntl.Value);
                PcieEqGen4Cntl.Field.DOWNSTREAM_PORT_TX_PRESET = (UINT8)(EqCfgTable->EqCfgList[Index].CfgValue[0]);
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieEqGen4Cntl=%x\n", PcieEqGen4Cntl.Value);
                NbioSmnRegisterWrite8 (GnbHandle, SmnAddress, &PcieEqGen4Cntl.Value, 0);
              }
            }
            //Unlock HWINIT_WR_LOCK
            HwInitWRLock(GnbHandle, Engine);
            //Reset PCIe_LC_CNTL.LC_RESET_LINK
            ResetLink(GnbHandle, Engine, MappingPortID); 
            break;
            
        case pcie_gen4_ttx_force_otherside_initial_preset_before_training:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_DSUP_stream_preset_before_training\n");
            //Enable HWINIT_WR_LOCK writable
            EnHwInitWRLock(GnbHandle, Engine);
            //Set Gen4 PCIe_Lane_equalization Upstream_port_tx_preset
            for (i = 0; i <= MAXLANE; i++) {
              if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (PCIE_LANE_EQUALIZATION_CNTL_16GT_OFFSET + i) + (MappingPortID << 12);
                IDS_HDT_CONSOLE (GNB_TRACE, "PCIE_RC_Address_gen4=%x\n", SmnAddress);
                NbioSmnRegisterRead8 (GnbHandle, SmnAddress, &PcieEqGen4Cntl.Value);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieEqGen4Cntl=%x\n", PcieEqGen4Cntl.Value);
                PcieEqGen4Cntl.Field.UPSTREAM_PORT_TX_PRESET = (UINT8)(EqCfgTable->EqCfgList[Index].CfgValue[0]);
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieEqGen4Cntl=%x\n", PcieEqGen4Cntl.Value);
                NbioSmnRegisterWrite8 (GnbHandle, SmnAddress, &PcieEqGen4Cntl.Value, 0);
              }
            }
            //Unlock HWINIT_WR_LOCK
            HwInitWRLock(GnbHandle, Engine);
            //Reset PCIe_LC_CNTL.LC_RESET_LINK
            ResetLink(GnbHandle, Engine, MappingPortID);
            break;
  
        default:
            return EFI_UNSUPPORTED;
      }
    } else if (Phase == 1) {
      switch (EqCfgTable->EqCfgList[Index].Type) {
        case pcie_gen3_ttx_cursor:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_cursor\n");
            SmnAddress = PCIE_PORT_REGS_SMN_BASE + PCIE_LC_FORCE_COEFF_OFFSET + (Wrapper->WrapId << 20) + (MappingPortID << 12);
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            Data32 &= 0xfff80000;
            Data32 |= (GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 1);
            Data32 |= (GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 7);
            Data32 |= (GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 13);
            Data32 |= 1;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            break;
        case pcie_gen4_ttx_cursor:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_cursor\n");
            SmnAddress = PCIE_PORT_REGS_SMN_BASE + PCIE_LC_FORCE_COEFF2_OFFSET + (Wrapper->WrapId << 20) + (MappingPortID << 12);
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            Data32 &= 0xfff80000;
            Data32 |= (GET_PRE_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 1);
            Data32 |= (GET_MAIN_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 7);
            Data32 |= (GET_POST_CURSOR(EqCfgTable->EqCfgList[Index].CfgValue[0]) << 13);
            Data32 |= 1;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &Data32, 0);
            break;
        case pcie_gen3_ttx_force_preset:
            break;
        case pcie_gen3_ttx_force_otherside_preset:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen3_ttx_force_otherside_preset\n");
            *RequestRedoEq = TRUE;
            if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_PORT_REGS_SMN_BASE + (Wrapper->WrapId << 20) + PCIE_LC_CNTL4_REG_OFFSET + (MappingPortID << 12);
                NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl4.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl4=%x\n", PcieLcCntl4.Value);
                PcieLcCntl4.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE = 1;
                PcieLcCntl4.Reg.LC_FORCE_PRESET_VALUE = EqCfgTable->EqCfgList[Index].CfgValue[0];
                PcieLcCntl4.Reg.LC_EQ_SEARCH_MODE = 3;
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl4=%x\n", PcieLcCntl4.Value);
                NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl4.Value, 0);
            } else {
                for (i=EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
                    //set pre lane value
                }
            }
            
            //clear PERFORM_EQUALIZATION and LINK_EQUALIZATION_REQ_INT_EN
            SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET;
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            LinkCtl3.Field.PERFORM_EQUALIZATION         = 0;
            LinkCtl3.Field.LINK_EQUALIZATION_REQ_INT_EN = 0;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            //speed change to Gen2
            PcieSpeedChange (GnbHandle, Engine, 2, &OldLinkSpeed);
            //delay 50ms
            MicroSecondDelay (50*1000);
            //set PERFORM_EQUALIZATION to 1
            SmnAddress = PCIE_RC_REGS_SMN_BASE + (Wrapper->WrapId << 20) + (MappingPortID << 12) + RC_PCIE_LINK_CNTL3_OFFSET;
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            LinkCtl3.Field.PERFORM_EQUALIZATION         = 1;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            //speed change to Gen3
            PcieSpeedChange (GnbHandle, Engine, 3, NULL);
            //speed change to old speed
            PcieSpeedChange (GnbHandle, Engine, OldLinkSpeed, NULL);
            //set PERFORM_EQUALIZATION to 0
            NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            LinkCtl3.Field.PERFORM_EQUALIZATION         = 0;
            NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LinkCtl3.Value, 0);
            break;
        case pcie_gen4_ttx_force_preset:
            break;
        case pcie_gen4_ttx_force_otherside_preset:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_gen4_ttx_force_otherside_preset\n");
            //SGEZT#24235-Skip-PHY-Conrfig-For-H3-Downgraded-To-H2 +>
            if(PcieCapLinkSpeed (GnbHandle, Engine) < DxioGen4)
              break;
            //SGEZT#24235-Skip-PHY-Conrfig-For-H3-Downgraded-To-H2 ->
            *RequestRedoEq = TRUE;
            if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (GNB_TRACE, "Configure EQ: Die%x PCIe%x Port%x\n", GnbHandle->InstanceId, Wrapper->WrapId, MappingPortID);
                SmnAddress = PCIE_PORT_REGS_SMN_BASE + (Wrapper->WrapId << 20) + PCIE_LC_CNTL8_REG_OFFSET + (MappingPortID << 12);
                NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl8.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before DoPcieEqCfg: PcieLcCntl8=%x\n", PcieLcCntl8.Value);
                PcieLcCntl8.Reg.LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT = 1;
                PcieLcCntl8.Reg.LC_FORCE_PRESET_VALUE_16GT = EqCfgTable->EqCfgList[Index].CfgValue[0];
                IDS_HDT_CONSOLE (GNB_TRACE, "After DoPcieEqCfg: PcieLcCntl8=%x\n", PcieLcCntl8.Value);
                NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &PcieLcCntl8.Value, 0);
            } else {
                for (i = EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
                    //set pre lane value
                }
            }
            //PciePerformEqualization (GnbHandle, Engine);
            break;
     
        case pcie_rxX_SSC_OFF_PHUG1:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_rxX_SSC_OFF_PHUG1\n"); 
            if(EqCfgTable->EqCfgList[Index].CpuGen == CPU_HYGON_DX){
              if (EqCfgTable->EqCfgList[Index].PerPort) { 
                IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_3_3_5 to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[0]);
                PhyRegRead (GnbHandle, DxioType, EqCfgTable->EqCfgList[Index].StartLane, LANE_DIG_RX_CDR_CTL_3, &Cdr_Data);
                Cdr_Data &= 0xffc7;
                Cdr_Data |= (EqCfgTable->EqCfgList[Index].CfgValue[0] << 3);
                PhyRegWrite (GnbHandle, DxioType, EqCfgTable->EqCfgList[Index].StartLane, LANE_DIG_RX_CDR_CTL_3, Cdr_Data); 
              }
            }
            break;
            
        case pcie_rxX_SSC_OFF_FRUG1:
            IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_rxX_SSC_OFF_FRUG1:\n"); 
            if(EqCfgTable->EqCfgList[Index].CpuGen == CPU_HYGON_DX){
              if (EqCfgTable->EqCfgList[Index].PerPort) {
                IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_4_0_2 to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[0]);
                PhyRegRead (GnbHandle, DxioType, EqCfgTable->EqCfgList[Index].StartLane,LANE_DIG_RX_CDR_CTL_4, &Cdr_Data);
                Cdr_Data &= 0xfff8;
                Cdr_Data |= EqCfgTable->EqCfgList[Index].CfgValue[0];
                PhyRegWrite (GnbHandle, DxioType, EqCfgTable->EqCfgList[Index].StartLane, LANE_DIG_RX_CDR_CTL_4, Cdr_Data); 
              }
            }
            break; 
        case pcie_txx_force_iboost_lvl:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_txx_force_iboost_lvl\n");
          for (i = EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++) {
              //set pre lane value
            if (EqCfgTable->EqCfgList[Index].PerPort) { 
              CfgValue = EqCfgTable->EqCfgList[Index].CfgValue[0];
            } else {
              CfgValue = EqCfgTable->EqCfgList[Index].CfgValue[i - EqCfgTable->EqCfgList[Index].StartLane];
            }
            PhyRegRead (GnbHandle, DxioType, (UINT8)(i % 16), LANE_DIG_PCS_XF_TX_OVER_IN_3, &Data16);
            Data16 &= (UINT32)(~(BIT4 | BIT5 | BIT6 | BIT7));
            Data16 |= (UINT32)(BIT2 | BIT3 | BIT8);
            Data16 |= (UINT32)(CfgValue << 4);
            PhyRegWrite (GnbHandle, DxioType, (UINT8)(i % 16), LANE_DIG_PCS_XF_TX_OVER_IN_3, Data16); 
          } 
          break; 
        case pcie_phy_para_override:
          IDS_HDT_CONSOLE (GNB_TRACE, "DoPcieEqCfg: pcie_phy_para_override\n");
          for (i = EqCfgTable->EqCfgList[Index].StartLane; i <= EqCfgTable->EqCfgList[Index].EndLane; i++){           
            Lane = (UINT8)(i % 16);
            IDS_HDT_CONSOLE (PCIE_MISC, "Cfg Lane%x ...\n",Lane);    
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_VERG_CTRL2 to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_VERG_CTRL2_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_VERG_CTRL2, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_VERG_CTRL2_INDEX]));
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_ANA_RX_AFE_CTLE to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_DIG_ANA_RX_AFE_CTLE_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_ANA_RX_AFE_CTLE, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_DIG_ANA_RX_AFE_CTLE_INDEX]));               
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_AFE_OVER_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_AFE_OVER_INDEX]));
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER_RTRIM to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_AFE_OVER_RTRIM_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER_RTRIM, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_AFE_OVER_RTRIM_INDEX]));
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER_ICTRL to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_AFE_OVER_ICTRL_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER_ICTRL, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_AFE_OVER_ICTRL_INDEX]));
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_CAL_MUXB to %x\n",EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_CAL_MUXB_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_CAL_MUXB, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_CAL_MUXB_INDEX]));
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_CAL_MUXA to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_CAL_MUXA_INDEX]);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_CAL_MUXA, (UINT16)(EqCfgTable->EqCfgList[Index].CfgValue[LANE_ANA_RX_CAL_MUXA_INDEX]));
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_3_3_5 to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_DIG_RX_CDR_CTL_3_INDEX]);
            PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, &Data16);
            Data16 &= 0xffc7;
            Data16 |= (EqCfgTable->EqCfgList[Index].CfgValue[LANE_DIG_RX_CDR_CTL_3_INDEX] << 3);
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, Data16);
            IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_4_0_2 to %x\n", EqCfgTable->EqCfgList[Index].CfgValue[LANE_DIG_RX_CDR_CTL_4_INDEX]);
            PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, &Data16);
            Data16 &= 0xfff8;
            Data16 |= EqCfgTable->EqCfgList[Index].CfgValue[LANE_DIG_RX_CDR_CTL_4_INDEX];
            PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, Data16);
          }
          break;  
        default:
            return EFI_UNSUPPORTED;
      }
    } else {
      return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
}

/**
 * Configure EQ for current PCIe Engine after PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */
EFI_STATUS
PcieEqConfigurationBeforeTraining (
    IN     GNB_HANDLE           *GnbHandle,
    IN     PCIe_ENGINE_CONFIG   *Engine
)
{
  EFI_STATUS                  Status;
  HYGON_CPM_EQ_CONFIG_TABLE*  gEqCfgTable;
  UINT32                      StartIndex = 0;
  UINT32                      SearchIndex;
  BOOLEAN                     RedoEq = FALSE;

  //get Eq Cfg Table
  Status = GetEqCfgTable (GnbHandle->InstanceId,&gEqCfgTable);//SGEZT#25974-Improve-Code-For-Preset

  if (EFI_ERROR(Status)) {
    return Status;
  }
  do{
    //match cfg table?
    Status = PcieMatchEqCfg (GnbHandle, Engine, gEqCfgTable, StartIndex, 0, &SearchIndex);
    if (!EFI_ERROR(Status)) {
      StartIndex = SearchIndex + 1;
      //setting Eq
      DoPcieEqCfg (GnbHandle, Engine, gEqCfgTable, SearchIndex, 0, &RedoEq);
    } 
  }while(!EFI_ERROR(Status));
  
  return EFI_SUCCESS;
}

/**
 * Configure EQ for current PCIe Engine after PCIe training 
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  Engine         Pointer to engine config descriptor
 * 
 * @retval    EFI_STATUS
 */

EFI_STATUS
PcieEqConfigurationAfterTraining (
    IN     GNB_HANDLE           *GnbHandle,
    IN     PCIe_ENGINE_CONFIG   *Engine,
    OUT    BOOLEAN              *RequestRedoEq
)
{
    EFI_STATUS                  Status;
    HYGON_CPM_EQ_CONFIG_TABLE*  gEqCfgTable;
    UINT32                      StartIndex = 0;
    UINT32                      SearchIndex;
    BOOLEAN                     RedoEq = FALSE;
    BOOLEAN                     RedoEqRecord = FALSE;
    
    if (RequestRedoEq == NULL) {
      return EFI_INVALID_PARAMETER;
    }
    
    *RequestRedoEq = FALSE;
    //get Eq Cfg Table
    Status = GetEqCfgTable (GnbHandle->InstanceId,&gEqCfgTable);//SGEZT#25974-Improve-Code-For-Preset
    if (EFI_ERROR(Status)) {
      return Status;
    }
    do{
      //match cfg table?
      Status = PcieMatchEqCfg (GnbHandle, Engine, gEqCfgTable, StartIndex, 1, &SearchIndex);
      if (!EFI_ERROR(Status)) {
        StartIndex = SearchIndex + 1;
        //setting Eq
        DoPcieEqCfg (GnbHandle, Engine, gEqCfgTable, SearchIndex, 1, &RedoEq);
        RedoEqRecord |= RedoEq;
      } 
    }while(!EFI_ERROR(Status));
    
    *RequestRedoEq = RedoEqRecord;
    
    return EFI_SUCCESS;
}
//SGMTB#148601:System Report Rx Error When Install DCU Z100+>
/**
 * PHY register reconfigure
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  PcieEngine     Pointer to the PcieEngine
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  RequestRedoEq  Pointer to the Request redo EQ
 * @retval
 */
EFI_STATUS
DxioPhyReCfg (
    GNB_HANDLE*               GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine,
    DXIO_PORT_DESCRIPTOR      *UserCfg,
    BOOLEAN                   *RequestRedoEq,
    UINT16                     CpuBaseModle
)
{
    UINT16                    Data16;
    UINT8                     DxioType;
    UINT8                     Lane,StartLane,EndLane;
    PKG_TYPE                  CpuPkg;
    PCIE_LC_CNTL_10           LcCntl10;
    UINT32                    DidVid;
    UINT8                     iPhyParameterTable;
    EFI_STATUS                Status=EFI_NOT_FOUND;
    UINT16                    VendorId,DeviceId;
    IDS_HDT_CONSOLE (PCIE_MISC,"%a Entry\n",__FUNCTION__);
    
    *RequestRedoEq = FALSE; 
    
    //get CPU Generation and Package info
    CpuPkg = GetPkgType ();
    
    DxioType   = (PcieEngine->Type.Port.PortId < 8) ? 2 : 3;
    DidVid = GetTargetVidDid (GnbHandle, PcieEngine);  

    VendorId = DidVid & 0xFFFF;
    DeviceId = (DidVid >> 16) & 0xFFFF;

    IDS_HDT_CONSOLE (PCIE_MISC,"[Input]CpuBaseModle=%x   CpuPkg=%x     VendorId:%x DeviceId:%X\n",CpuBaseModle,CpuPkg,VendorId,DeviceId);

    //@todo Loop PhyParameterTable Table
    for (iPhyParameterTable = 0; PhyParameterTable[iPhyParameterTable].CpuPkg != 0xFF; iPhyParameterTable++)
    {
      /* code */
      IDS_HDT_CONSOLE (PCIE_MISC,"[Loop][%x]CpuBaseModle=%x   CpuPkg=%x     VendorId:%04x DeviceId:%04X",\
                                          iPhyParameterTable,PhyParameterTable[iPhyParameterTable].CpuBaseModle,\
                                          PhyParameterTable[iPhyParameterTable].CpuPkg,\
                                          PhyParameterTable[iPhyParameterTable].VendorID,\
                                          PhyParameterTable[iPhyParameterTable].DeviceID);
      //@todo check CPU Model,Pkg Type
      if((!(CpuBaseModle & PhyParameterTable[iPhyParameterTable].CpuBaseModle)) || \
         (CpuPkg !=PhyParameterTable[iPhyParameterTable].CpuPkg) || \
         ((PhyParameterTable[iPhyParameterTable].VendorID != InvalidVendotID) && \
          (VendorId !=PhyParameterTable[iPhyParameterTable].VendorID)) ||\
          ((PhyParameterTable[iPhyParameterTable].DeviceID != InvalidVendotID) && \
          (DeviceId !=PhyParameterTable[iPhyParameterTable].DeviceID)))
          {
            IDS_HDT_CONSOLE (PCIE_MISC,"====Not Match\n");
            continue;
          }
      IDS_HDT_CONSOLE (PCIE_MISC,"====Matched Continue!!!\n");
      //@todo
      //check target supported speed
      if (PcieEngine->Type.Port.PortId > 7) {
        NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_CNTL_10_ADDRESS + ((PcieEngine->Type.Port.PortId - 8) << 12), &LcCntl10.Value, 0);
      } else {
        NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_CNTL_10_ADDRESS + (PcieEngine->Type.Port.PortId << 12), &LcCntl10.Value, 0);
      }

      if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 0) {
        IDS_HDT_CONSOLE (PCIE_MISC, "[LC_OTHER_SIDE_SUPPORTS_GEN4]Skip PHY ReCfg\n");
        Status=EFI_UNSUPPORTED;
        break;
      }

      IDS_HDT_CONSOLE (PCIE_MISC, "Start PHY ReCfg\n");
      StartLane = UserCfg->EngineData.StartLane%16;
      EndLane   = UserCfg->EngineData.EndLane%16;
      
      //Set Gen4 PHY Parameters 
      for (Lane = StartLane; Lane <= EndLane; Lane++)
      {    
        IDS_HDT_CONSOLE (PCIE_MISC, "Cfg Lane%x ...\n",Lane);    
        if(CpuBaseModle & HYGON_REV_F18_DN_Cx)
        {
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_VERG_CTRL2       to %x\n", PhyParameterTable[iPhyParameterTable].ANA_RX_VERG_CTRL2);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_VERG_CTRL2, PhyParameterTable[iPhyParameterTable].ANA_RX_VERG_CTRL2);
     
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_ANA_RX_AFE_CTLE     to %x\n", PhyParameterTable[iPhyParameterTable].DIG_ANA_RX_AFE_CTLE);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_ANA_RX_AFE_CTLE, PhyParameterTable[iPhyParameterTable].DIG_ANA_RX_AFE_CTLE);               
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER to %x\n", PhyParameterTable[iPhyParameterTable].ANA_RX_AFE_OVER);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER, PhyParameterTable[iPhyParameterTable].ANA_RX_AFE_OVER);
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER_RTRIM   to %x\n", PhyParameterTable[iPhyParameterTable].ANA_RX_AFE_OVER_RTRIM);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER_RTRIM, PhyParameterTable[iPhyParameterTable].ANA_RX_AFE_OVER_RTRIM);
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER_ICTRL   to %x\n", PhyParameterTable[iPhyParameterTable].ANA_RX_AFE_OVER_ICTRL);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER_ICTRL, PhyParameterTable[iPhyParameterTable].ANA_RX_AFE_OVER_ICTRL);
        
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_CAL_MUXB         to %x\n",PhyParameterTable[iPhyParameterTable].ANA_RX_CAL_MUXB);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_CAL_MUXB, PhyParameterTable[iPhyParameterTable].ANA_RX_CAL_MUXB);   
          IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_CAL_MUXA         to %x\n", PhyParameterTable[iPhyParameterTable].ANA_RX_CAL_MUXA);
          PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_CAL_MUXA, PhyParameterTable[iPhyParameterTable].ANA_RX_CAL_MUXA);
          
          *RequestRedoEq = TRUE;
        }

        IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_3_3_5      to %x\n", PhyParameterTable[iPhyParameterTable].DIG_RX_CDR_CTL_3);
        PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, &Data16);
        Data16 &= 0xffc7;
        Data16 |= (PhyParameterTable[iPhyParameterTable].DIG_RX_CDR_CTL_3 << 3);
        PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, Data16);
        IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_4_0_2      to %x\n", PhyParameterTable[iPhyParameterTable].DIG_RX_CDR_CTL_4);
        PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, &Data16);
        Data16 &= 0xfff8;
        Data16 |= PhyParameterTable[iPhyParameterTable].DIG_RX_CDR_CTL_4;
        PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, Data16);
      }
      Status=EFI_SUCCESS;

      break;
    }
    
    IDS_HDT_CONSOLE (PCIE_MISC,"%a Exit\n",__FUNCTION__);
    return Status;
}

#if 0
/**
 * PHY register reconfigure
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  PcieEngine     Pointer to the PcieEngine
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  RequestRedoEq  Pointer to the Request redo EQ
 * @retval
 */
VOID
DxioPhyReCfgHygonCx (
    GNB_HANDLE*               GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine,
    DXIO_PORT_DESCRIPTOR      *UserCfg,
    BOOLEAN                   *RequestRedoEq
)
{
    UINT16                    Data16;
    UINT8                     DxioType;
    UINT8                     Lane,StartLane,EndLane;
    UINT8                     PkgIndex;
    PKG_TYPE                  CpuPkg;
    PCIE_LC_CNTL_10           LcCntl10;
    
    IDS_HDT_CONSOLE (PCIE_MISC, "DxioPhyReCfgHygonCx Entry\n");
    
    *RequestRedoEq = FALSE; 
    
    //get CPU Generation and Package info
    CpuPkg = GetPkgType ();
    if(CpuPkg == PKG_DM1) {
      IDS_HDT_CONSOLE (PCIE_MISC, "Skip DM1 Pkg\n");
      goto EXIT;
    }
    PkgIndex = 0;
    switch (CpuPkg) {
        case PKG_DM1:
            PkgIndex = 0;
            break;

        case PKG_SL1:
            PkgIndex = 1;
            break;

        case PKG_SL1R2:
            PkgIndex =2;
            break;

        default:
            break;
    }
    
    DxioType   = (PcieEngine->Type.Port.PortId < 8) ? 2 : 3;

    //check target supported speed
    if (PcieEngine->Type.Port.PortId > 7) {
      NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_CNTL_10_ADDRESS + ((PcieEngine->Type.Port.PortId - 8) << 12), &LcCntl10.Value, 0);
    } else {
      NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_CNTL_10_ADDRESS + (PcieEngine->Type.Port.PortId << 12), &LcCntl10.Value, 0);
    }
    if(PcdGetBool(PcdPcieSITestMode) == FALSE) {
      if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 0) {
        IDS_HDT_CONSOLE (PCIE_MISC, "Skip PHY ReCfg\n");
        goto EXIT;
      }
    }    
    IDS_HDT_CONSOLE (PCIE_MISC, "Start PHY ReCfg\n");
    StartLane = UserCfg->EngineData.StartLane%16;
    EndLane   = UserCfg->EngineData.EndLane%16;

    //Set HYGON_CX Gen4 PHY Parameters 
    for (Lane = StartLane; Lane <= EndLane; Lane++){    
      IDS_HDT_CONSOLE (PCIE_MISC, "Cfg Lane%x ...\n",Lane);    
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_VERG_CTRL2 to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_VERG_CTRL2_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_VERG_CTRL2, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_VERG_CTRL2_INDEX]);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_ANA_RX_AFE_CTLE to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_ANA_RX_AFE_CTLE_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_ANA_RX_AFE_CTLE, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_ANA_RX_AFE_CTLE_INDEX]);               
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_INDEX]);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER_RTRIM to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_RTRIM_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER_RTRIM, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_RTRIM_INDEX]);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_AFE_OVER_ICTRL to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_ICTRL_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_AFE_OVER_ICTRL, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_AFE_OVER_ICTRL_INDEX]);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_CAL_MUXB to %x\n",PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_CAL_MUXB_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_CAL_MUXB, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_CAL_MUXB_INDEX]);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_ANA_RX_CAL_MUXA to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_CAL_MUXA_INDEX]);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_ANA_RX_CAL_MUXA, PhyParameterTable_Hygon_Cx[PkgIndex][LANE_ANA_RX_CAL_MUXA_INDEX]);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_3_3_5 to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_RX_CDR_CTL_3_INDEX]);
      PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, &Data16);
      Data16 &= 0xffc7;
      Data16 |= (PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_RX_CDR_CTL_3_INDEX] << 3);
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, Data16);
      IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_4_0_2 to %x\n", PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_RX_CDR_CTL_4_INDEX]);
      PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, &Data16);
      Data16 &= 0xfff8;
      Data16 |= PhyParameterTable_Hygon_Cx[PkgIndex][LANE_DIG_RX_CDR_CTL_4_INDEX];
      PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, Data16);
    } 
    
    *RequestRedoEq = TRUE;
EXIT:
    IDS_HDT_CONSOLE (PCIE_MISC, "DxioPhyReCfgHygonCx Exit\n");
    return;
}
/*----------------------------------------------------------------------------------------*/
/**
 * PHY register reconfigure for HYGON_DX
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  PcieEngine     Pointer to the PcieEngine
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @retval
 */
VOID
DxioPhyReCfgHygonDx(
    GNB_HANDLE*               GnbHandle,
    PCIe_ENGINE_CONFIG        *PcieEngine,
    DXIO_PORT_DESCRIPTOR      *UserCfg,
    BOOLEAN                   *RequestRedoEq
)
{
    UINT16                    Data16;
    UINT8                     DxioType;
    UINT8                     Lane,StartLane,EndLane;
    UINT8                     PkgIndex;
    PKG_TYPE                  CpuPkg;
    PCIE_LC_CNTL_10           LcCntl10;
    
    IDS_HDT_CONSOLE (PCIE_MISC, "DxioPhyReCfgHygonDx Entry\n");    
    
    *RequestRedoEq = FALSE;
    //get CPU Generation and Package info
    CpuPkg = GetPkgType ();
    PkgIndex = 0;
    switch (CpuPkg) {
        case PKG_DM1:
            PkgIndex = 0;
            break;

        case PKG_SL1:
            PkgIndex = 1;
            break;

        case PKG_SL1R2:
            PkgIndex =2;
            break;

        default:
            break;
    }       
    
    DxioType   = (PcieEngine->Type.Port.PortId < 8) ? 2 : 3;
  
    //check target supported speed
    if (PcieEngine->Type.Port.PortId > 7) {
      NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_CNTL_10_ADDRESS + ((PcieEngine->Type.Port.PortId - 8) << 12), &LcCntl10.Value, 0);
    } else {
      NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_CNTL_10_ADDRESS + (PcieEngine->Type.Port.PortId << 12), &LcCntl10.Value, 0);
    }
    //Check target if support gen4
    if (LcCntl10.Field.LC_OTHER_SIDE_SUPPORTS_GEN4 == 0) {
      IDS_HDT_CONSOLE (PCIE_MISC, "Skip PHY ReCfg\n");
      goto EXIT;
    }    
    IDS_HDT_CONSOLE (PCIE_MISC, "Start PHY ReCfg\n");
    StartLane = UserCfg->EngineData.StartLane%16;
    EndLane   = UserCfg->EngineData.EndLane%16;

    //HYGON_DX Set Ctl3/Ctl4 config for Gen4
    if ((CpuPkg == PKG_DM1) || (CpuPkg == PKG_SL1)) {
      for (Lane=StartLane; Lane <= EndLane; Lane++) {
        IDS_HDT_CONSOLE (PCIE_MISC, "Cfg Lane%x ...\n", Lane);      
        IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_3_3_5 to %x\n", PhyParameterTable_Hygon_Dx[PkgIndex][LANE_DIG_RX_CDR_CTL_3_INDEX]);
        PhyRegRead (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, &Data16);
        Data16 &= 0xffc7;
        Data16 |= (PhyParameterTable_Hygon_Dx[PkgIndex][LANE_DIG_RX_CDR_CTL_3_INDEX] << 3);
        PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_3, Data16);
        IDS_HDT_CONSOLE (PCIE_MISC, "LANE_DIG_RX_CDR_CTL_4_0_2 to %x\n", PhyParameterTable_Hygon_Dx[PkgIndex][LANE_DIG_RX_CDR_CTL_4_INDEX]);
        PhyRegRead (GnbHandle, DxioType, Lane,LANE_DIG_RX_CDR_CTL_4, &Data16);
        Data16 &= 0xfff8;
        Data16 |= PhyParameterTable_Hygon_Dx[PkgIndex][LANE_DIG_RX_CDR_CTL_4_INDEX];
        PhyRegWrite (GnbHandle, DxioType, Lane, LANE_DIG_RX_CDR_CTL_4, Data16);  
      }   
    }

EXIT:
    IDS_HDT_CONSOLE (PCIE_MISC, "DxioPhyReCfgHygonDx Exit\n");
    return;
}
#endif
//SGMTB#148601:System Report Rx Error When Install DCU Z100->

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Update Port Training Status
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     AGESA_STATUS
 */
VOID
DxioUpdatePortTrainingStatus (
  IN      GNB_HANDLE                      *GnbHandle,
  IN      DXIO_COMPLEX_DESCRIPTOR         *UserConfig,
  IN OUT  DXIO_PLATFORM_DESCRIPTOR        *ThisNode,
  IN      PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi
  )
{
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR  *DxioNodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR    *DxioEnginePointer;
  UINT32                    DxioNodeCount;
  //User Config Pointers
  DXIO_COMPLEX_DESCRIPTOR   *UserCfg;
  DXIO_PORT_DESCRIPTOR      *UserCfgPortList;              // Pointer to start of User Config for this socket
  DXIO_PORT_DESCRIPTOR      *UserCfgPortPointer;           // Updatable pointer for UserCfgPortList processing
  SOC_LOGICAL_ID            LogicalId;
  // PCIe Structures
  PCIe_ENGINE_CONFIG        *PcieEngineList;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;
  PCIE_LC_STATE0_STRUCT     LcState0;
  UINT8                     SocketNumber;
  BOOLEAN                   EndOfList;
  UINT32                    Index;
  UINT8                     EngineId;
  UINT8                     PortDevMap[16];
  BOOLEAN                   TrainingSuccess;
  PCIE_LINK_CNTL2_STRUCT    LinkCntl2;
  PCIE_LINK_STATUS_STRUCT   LinkStatus;
  BOOLEAN                   RedoEq = FALSE;
  BOOLEAN                   RequestRedoEq = FALSE;
  UINT32                    TryCount = 0;
  EFI_STATUS                Status; //<hety-20230609 >
  //IDS_HDT_CONSOLE (PCIE_MISC, "DxioUpdatePortTrainingStatus enter\n");

  LibAmdMemFill (PortDevMap, 0, sizeof (PortDevMap), (AMD_CONFIG_PARAMS *) NULL);

  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;
  UserCfg = UserConfig;
  EndOfList = FALSE;
  UserCfgPortList = NULL;
  while (EndOfList != TRUE) {
    //IDS_HDT_CONSOLE (PCIE_MISC, " This list is for socket %d\n", UserCfg->SocketId);
    if (UserCfg->SocketId == SocketNumber) {
      UserCfgPortList = UserCfg->PciePortList;
    }
    if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      UserCfg++;
    } else {
      EndOfList = TRUE;
    }
  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  EngineId = 0;
//  for (Index = 0;Index < DxioNodeCount;Index++) {
//    GnbLibDebugDumpBuffer ((VOID*) DxioEnginePointer, sizeof(DXIO_ENGINE_DESCRIPTOR), 1, 16);
//    IDS_HDT_CONSOLE (PCIE_MISC, "\n");
//    DxioEnginePointer++;
//  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  for (Index = 0;Index < DxioNodeCount;Index++) {
    if (DxioEnginePointer->EngineType == DxioPcieEngine) {
      if (DxioEnginePointer->StartLane >= 16) {
        DxioEnginePointer->StartLane -= 4;
        DxioEnginePointer->EndLane -= 4;     ///@todo this value should be an offset defined in the wrapper
      }
      DxioEnginePointer->StartLane += GnbHandle->StartLane;
      DxioEnginePointer->EndLane += GnbHandle->StartLane;
      //DxioEntryConfigDump (DxioEnginePointer);
      // Find the UserConfig entry for this DXIO engine
      UserCfgPortPointer = UserCfgPortList;       // Start of the port list for this socket
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        //IDS_HDT_CONSOLE (PCIE_MISC, "This port starts at lane %d and ends at lane %d\n", UserCfgPortPointer->EngineData.StartLane,  UserCfgPortPointer->EngineData.EndLane);
        if ((UserCfgPortPointer->EngineData.StartLane >= DxioEnginePointer->StartLane) &&
            (UserCfgPortPointer->EngineData.EndLane <= DxioEnginePointer->EndLane) &&
            (UserCfgPortPointer->EngineData.EngineType == DxioPcieEngine)) {
          EndOfList = TRUE;
        } else {
          if ((UserCfgPortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
            UserCfgPortPointer++;
          } else {
            EndOfList = TRUE;
            ASSERT (FALSE);           // Something bad happened here @todo - handle error
          }
        }
      }
      // Find the PCIe wrapper for this DXIO engine
      PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        //IDS_HDT_CONSOLE (PCIE_MISC, "Wrapper StartLane = %d, EndLane = %d\n",PcieWrapper->StartPhyLane, PcieWrapper->EndPhyLane);
        if ((UserCfgPortPointer->EngineData.StartLane >= (PcieWrapper->StartPhyLane + GnbHandle->StartLane)) &&
            (UserCfgPortPointer->EngineData.EndLane <= (PcieWrapper->EndPhyLane + GnbHandle->StartLane))) {
          EndOfList = TRUE;
        } else {
          PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
          DxioEnginePointer->LinkDesc.Pcie.MacPortID += 8;      ///@todo this should be port offset defined in the wrapper
          if (PcieWrapper == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // Find the PCIe structure entry for this DXIO engine
      PcieEngineList = PcieConfigGetChildEngine (PcieWrapper);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
//        IDS_HDT_CONSOLE (PCIE_MISC, "This PortId = %d\n", PcieEngineList->Type.Port.PortId);
        if (PcieEngineList->Type.Port.PortId == DxioEnginePointer->LinkDesc.Pcie.MacPortID) {
            EndOfList = TRUE;
        } else {
          PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList);
          if (PcieEngineList == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // At this point we have aligned structures...
      // PcieEngineList points to the engine in the Pcie platform structure.
      // UserCfgPortPointer points to the port in UserConfig
      // DxioEnginePointer points to the engine structure in the DXIO structure
      // We can use this data to update the status of the engine in the Pcie platform
      if (PcieEngineList != NULL) {
          TrainingSuccess = FALSE;
          LcState0.Value = 0;
          if(PcdGetBool(PcdPcieSITestMode) == FALSE) {
              UINT32 timeout;
              for(timeout=0; timeout<20; timeout++){
                  MicroSecondDelay(1000);
                  if (PcieEngineList->Type.Port.PortId > 7) {
                      NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_STATE0_ADDRESS+((PcieEngineList->Type.Port.PortId - 8) << 12), &LcState0.Value, 0);
                      IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for Port %d is 0x%x\n", PcieEngineList->Type.Port.PortId, LcState0.Value);
                      if ((LcState0.Field.LC_CURRENT_STATE > 4)) {               
                        TrainingSuccess = TRUE;
                        break;
                      }
                  } else {
                      NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_STATE0_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LcState0.Value, 0);
                      IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for Port %d is 0x%x\n", PcieEngineList->Type.Port.PortId, LcState0.Value);
                      if ((LcState0.Field.LC_CURRENT_STATE > 4)) {
                        TrainingSuccess = TRUE;
                        break;
                      }
                  }
              }
          } else {
	          if (PcieEngineList->Type.Port.PortId > 7) {
	              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_STATE0_ADDRESS+((PcieEngineList->Type.Port.PortId - 8) << 12), &LcState0.Value, 0);
	              IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for Port %d is 0x%x\n", PcieEngineList->Type.Port.PortId, LcState0.Value);
	              if ((LcState0.Field.LC_CURRENT_STATE > 4)) {               
	                TrainingSuccess = TRUE;
	              }
	          } else {
	              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_STATE0_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LcState0.Value, 0);
	              IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for Port %d is 0x%x\n", PcieEngineList->Type.Port.PortId, LcState0.Value);
	              if ((LcState0.Field.LC_CURRENT_STATE > 4)) {
	                TrainingSuccess = TRUE;
	              }
	          }
          }
        if ((TrainingSuccess != TRUE) || (DxioEnginePointer->LinkDesc.Pcie.Capabilities.portPresent == 0)) {
          PcieEngineList->InitStatus = 0;
          IDS_HDT_CONSOLE (PCIE_MISC, "PCIe Detect failed on Device %x Function %x\n",
                           PcieEngineList->Type.Port.PortData.DeviceNumber,
                           PcieEngineList->Type.Port.PortData.FunctionNumber
                           );
        } else {
          PcieEngineList->InitStatus = 1;
          IDS_HDT_CONSOLE (PCIE_MISC, "PCIe Detect SUCCESS on Device %x Function %x\n",
                           PcieEngineList->Type.Port.PortData.DeviceNumber,
                           PcieEngineList->Type.Port.PortData.FunctionNumber
                           );
          PcieEngineList->InitStatus = INIT_STATUS_PCIE_TRAINING_SUCCESS;
          if (PcdGet8 (PcdPcieLinkTrainingType) == 1) {
            DxioSetOperatingSpeed (GnbHandle, PcieEngineList, DxioEnginePointer, UserCfgPortPointer, SmuServicesPpi);
          } else {
            if (PcieEngineList->Type.Port.PortId > 7) {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LINK_STATUS_ADDRESS + ((PcieEngineList->Type.Port.PortId - 8) << 12), &LinkStatus.Value, 0);
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LINK_CNTL2_ADDRESS + ((PcieEngineList->Type.Port.PortId - 8) << 12), &LinkCntl2.Value, 0);
            } else {
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LINK_STATUS_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LinkStatus.Value, 0);
              NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LINK_CNTL2_ADDRESS + ((PcieEngineList->Type.Port.PortId) << 12), &LinkCntl2.Value, 0);
            }
          }
          
          //wait PHY ready
          MicroSecondDelay (200000);
          
          while (!PcieDlActivate(GnbHandle, PcieEngineList) || PcieLinkSpeedWaring(GnbHandle, PcieEngineList)) {
            PcieRetraining (GnbHandle, PcieEngineList, FALSE);
            TryCount++;
            MicroSecondDelay (100000);
            if (TryCount > 5)
              break;
          }
          
          //Wait PCIe Link status to L0
          if ((PcdGetBool (PcdPcieSITestMode) == TRUE) || (PcieWaitToL0 (GnbHandle, PcieEngineList, 200))) {
            
            IDS_HDT_CONSOLE (
              PCIE_MISC, "PCIe Training SUCCESS on Device %x Function %x\n",
              PcieEngineList->Type.Port.PortData.DeviceNumber,
              PcieEngineList->Type.Port.PortData.FunctionNumber
              );
            
            //Get Logical Id
            BaseGetLogicalIdOnExecutingCore (&LogicalId); 
            
            //SGEZT#24235-Skip-PHY-Conrfig-For-H3-Downgraded-To-H2 +>
            if(PcieCapLinkSpeed (GnbHandle, PcieEngineList) > DxioGen3)
            {
            //SGMTB#148601:System Report Rx Error When Install DCU Z100+>
			  // //Check if HYGON_CX Platform
              // if(LogicalId.Revision & (HYGON_REV_F18_DN_Cx)){
              //   DxioPhyReCfgHygonCx (GnbHandle, PcieEngineList, UserCfgPortPointer, &RequestRedoEq);
              //   RedoEq |= RequestRedoEq;
              // }
            
              // //Check if HYGON_DX Platform
              // if(LogicalId.Revision & (HYGON_REV_F18_DN_Dx)){               
              //   DxioPhyReCfgHygonDx (GnbHandle, PcieEngineList, UserCfgPortPointer, &RequestRedoEq);
              //   RedoEq |= RequestRedoEq;
              // }
              Status = DxioPhyReCfg(GnbHandle, PcieEngineList, UserCfgPortPointer,&RequestRedoEq,LogicalId.Revision);
              if(!EFI_ERROR (Status))
                RedoEq |= RequestRedoEq;
            //SGMTB#148601:System Report Rx Error When Install DCU Z100->
            }
            //SGEZT#24235-Skip-PHY-Conrfig-For-H3-Downgraded-To-H2->
            
            if (PcdGetBool (PcdPcieSITestMode) == FALSE) {
              //EQ Configuration
              PcieEqConfigurationAfterTraining (GnbHandle, PcieEngineList, &RequestRedoEq);
              RedoEq |= RequestRedoEq;
            }
            
            if (PcdGetBool (PcdPcieSITestMode) == FALSE) {
              //Redo EQ
              if (RedoEq) {
                PciePerformEqualization (GnbHandle, PcieEngineList);
                if (PcieWaitToL0 (GnbHandle, PcieEngineList, 200)) {
                  
                  IDS_HDT_CONSOLE (
                    PCIE_MISC, "Reconfig EQ SUCCESS on Device %x Function %x\n",
                    PcieEngineList->Type.Port.PortData.DeviceNumber,
                    PcieEngineList->Type.Port.PortData.FunctionNumber
                    );
                  
                } else {
                  
                  IDS_HDT_CONSOLE (
                    PCIE_MISC, "Reconfig EQ Failed on Device %x Function %x\n",
                    PcieEngineList->Type.Port.PortData.DeviceNumber,
                    PcieEngineList->Type.Port.PortData.FunctionNumber
                    );
                  
                  //retry 10 times to L0
                  PcieRetryToL0 (GnbHandle, PcieEngineList, 10, 200);
                  
                } 
              }
            }
          } else {
            
            IDS_HDT_CONSOLE (
              PCIE_MISC, "PCIe Training Failed on Device %x Function %x\n",
              PcieEngineList->Type.Port.PortData.DeviceNumber,
              PcieEngineList->Type.Port.PortData.FunctionNumber
              );
            
          }
        }
      }
    }
    DxioEnginePointer++;
    EngineId++;
  }
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Update Pcie Data Structure
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @retval     AGESA_STATUS
 */
VOID
DxioUpdatePcieStatus (
  IN      GNB_HANDLE                *GnbHandle,
  IN      DXIO_COMPLEX_DESCRIPTOR   *UserConfig,
  IN      DXIO_PLATFORM_DESCRIPTOR  *ThisNode,
  IN OUT  DXIO_ENGINE_DESCRIPTOR    **EarlyTrain,
  IN OUT  UINT32                    *EarlyTrainAddress,
  IN OUT  PCIe_ENGINE_CONFIG        **EarlyTrainEngine

  )
{
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR  *DxioNodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR    *DxioEnginePointer;
  UINT32                    DxioNodeCount;
  //User Config Pointers
  DXIO_COMPLEX_DESCRIPTOR   *UserCfg;
  DXIO_PORT_DESCRIPTOR      *UserCfgPortList;              // Pointer to start of User Config for this socket
  DXIO_PORT_DESCRIPTOR      *UserCfgPortPointer;           // Updatable pointer for UserCfgPortList processing
  // PCIe Structures
  PCIe_ENGINE_CONFIG        *PcieEngineList;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;

  UINT8                     SocketNumber;
  BOOLEAN                   EndOfList;
  UINT32                    Index;
  UINT8                     EngineId;
  UINT8                     PortDevMap[16];

  IDS_HDT_CONSOLE (PCIE_MISC, "DxioUpdatePcieStatus enter\n");

  LibAmdMemFill (PortDevMap, 0, sizeof (PortDevMap), (AMD_CONFIG_PARAMS *) NULL);

  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;
  UserCfg = UserConfig;
  EndOfList = FALSE;
  UserCfgPortList = NULL;
  while (EndOfList != TRUE) {
    if (UserCfg->SocketId == SocketNumber) {
      UserCfgPortList = UserCfg->PciePortList;
    }
    if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      UserCfg++;
    } else {
      EndOfList = TRUE;
    }
  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  EngineId = 0;
  for (Index = 0;Index < DxioNodeCount;Index++) {
    //GnbLibDebugDumpBuffer ((VOID*) DxioEnginePointer, sizeof(DXIO_ENGINE_DESCRIPTOR), 1, 16);
    //IDS_HDT_CONSOLE (PCIE_MISC, "\n");
    DxioEnginePointer++;
  }

  DxioNodePlatformDesc = ThisNode;
  DxioNodePlatformDesc ++;                                              // Point to first engine descriptor
  DxioEnginePointer = (DXIO_ENGINE_DESCRIPTOR *)DxioNodePlatformDesc;       // Initialize engine pointer to first engine
  DxioNodePlatformDesc --;                                              // Restore DxioNodePlatformDesc address

  DxioNodeCount = DxioNodePlatformDesc->NumEngineDesc;

  for (Index = 0;Index < DxioNodeCount;Index++) {
    if (DxioEnginePointer->EngineType == DxioPcieEngine) {
      if (DxioEnginePointer->StartLane >= 16) {
        DxioEnginePointer->StartLane -= 4;
        DxioEnginePointer->EndLane -= 4;     ///@todo this value should be an offset defined in the wrapper
      }
      DxioEnginePointer->StartLane += GnbHandle->StartLane;
      DxioEnginePointer->EndLane += GnbHandle->StartLane;
      //DxioEntryConfigDump (DxioEnginePointer);
      // Find the UserConfig entry for this DXIO engine
      UserCfgPortPointer = UserCfgPortList;       // Start of the port list for this socket
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if ((UserCfgPortPointer->EngineData.StartLane >= DxioEnginePointer->StartLane) &&
            (UserCfgPortPointer->EngineData.EndLane <= DxioEnginePointer->EndLane) &&
            (UserCfgPortPointer->EngineData.EngineType == DxioPcieEngine)) {
          EndOfList = TRUE;
        } else {
          if ((UserCfgPortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
            UserCfgPortPointer++;
          } else {
            EndOfList = TRUE;
            ASSERT (FALSE);           // Something bad happened here @todo - handle error
          }
        }
      }
      // Find the PCIe wrapper for this DXIO engine
      PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if ((UserCfgPortPointer->EngineData.StartLane >= (PcieWrapper->StartPhyLane + GnbHandle->StartLane)) &&
            (UserCfgPortPointer->EngineData.EndLane <= (PcieWrapper->EndPhyLane + GnbHandle->StartLane))) {
          EndOfList = TRUE;
        } else {
          PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
          DxioEnginePointer->LinkDesc.Pcie.MacPortID += 8;      ///@todo this should be port offset defined in the wrapper
          if (PcieWrapper == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // Find the PCIe structure entry for this DXIO engine
      PcieEngineList = PcieConfigGetChildEngine (PcieWrapper);
      EndOfList = FALSE;
      while (EndOfList != TRUE) {
        if (PcieEngineList->Type.Port.PortId == DxioEnginePointer->LinkDesc.Pcie.MacPortID) {
            EndOfList = TRUE;
        } else {
          PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList);
          if (PcieEngineList == NULL) {
            ASSERT (FALSE);       // Something bad happened here @todo - handle error
          }
        }
      }

      // At this point we have aligned structures...
      // PcieEngineList points to the engine in the Pcie platform structure.
      // UserCfgPortPointer points to the port in UserConfig
      // DxioEnginePointer points to the engine structure in the DXIO structure
      // We can use this data to update the status of the engine in the Pcie platform
      if (PcieEngineList != NULL) {
        PcieEngineList->EngineData.EngineType = (UINT8) DxioEnginePointer->EngineType;
        PcieEngineList->EngineData.StartLane = (UINT16) DxioEnginePointer->StartLane;
        PcieEngineList->EngineData.EndLane = (UINT16) DxioEnginePointer->EndLane;
        if (DxioEnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
          *EarlyTrain = DxioEnginePointer;
          *EarlyTrainAddress = DxioEnginePointer->LinkDesc.Pcie.DevFunNumber;
          *EarlyTrainEngine = PcieEngineList;
        }
        PcieEngineList->Type.Port.PortData.PortPresent = TRUE;
        PcieEngineList->Type.Port.PortData.DeviceNumber = UserCfgPortPointer->Port.DeviceNumber;
        PcieEngineList->Type.Port.PortData.FunctionNumber = UserCfgPortPointer->Port.FunctionNumber;
        PcieEngineList->Type.Port.PortData.LinkSpeedCapability = UserCfgPortPointer->Port.LinkSpeedCapability;
        PcieEngineList->Type.Port.PortData.LinkAspm = UserCfgPortPointer->Port.LinkAspm;
        if(PcdGetBool(PcdCfgPcieHotplugSupport) == FALSE) {
            UserCfgPortPointer->Port.LinkHotplug = DxioHotplugDisabled;
        }
        PcieEngineList->Type.Port.PortData.LinkHotplug = UserCfgPortPointer->Port.LinkHotplug;
        //IDS_HDT_CONSOLE (PCIE_MISC, "DxioEnginePointer->LinkHotplug = %d\n", PcieEngineList->Type.Port.PortData.LinkHotplug);
        PcieEngineList->Type.Port.PcieBridgeId = EngineId;                    ///@todo Workaround for speed set
        PcieEngineList->Type.Port.LinkAspmL1_1 = UserCfgPortPointer->Port.LinkAspmL1_1;
        PcieEngineList->Type.Port.LinkAspmL1_2 = UserCfgPortPointer->Port.LinkAspmL1_2;
        PcieEngineList->Type.Port.ClkReq = UserCfgPortPointer->Port.ClkReq;
        PcieEngineList->Type.Port.EqPreset = UserCfgPortPointer->Port.EqPreset;
        PcieEngineList->Type.Port.PortData.MiscControls.ClkPmSupport = UserCfgPortPointer->Port.MiscControls.ClkPmSupport;
        //IDS_HDT_CONSOLE (PCIE_MISC, "Device 0:%d:%d ClkPmSupport = %d\n", PcieEngineList->Type.Port.PortData.DeviceNumber,
        //                 PcieEngineList->Type.Port.PortData.FunctionNumber,
        //                 PcieEngineList->Type.Port.PortData.MiscControls.ClkPmSupport);
        PcieConfigSetDescriptorFlags (PcieEngineList, DESCRIPTOR_ALLOCATED);
        PcieMapPortPciAddress (PcieEngineList, PortDevMap);
        PcieEngineList->Type.Port.Address.AddressValue = MAKE_SBDFO (
                                             0,
                                             GnbHandle->Address.Address.Bus,
                                             PcieEngineList->Type.Port.PortData.DeviceNumber,
                                             PcieEngineList->Type.Port.PortData.FunctionNumber,
                                             0
                                             );
        //PcieConfigEngineDebugDump (PcieEngineList);
      }
    }
    DxioEnginePointer++;
    EngineId++;
  }
  PcieSetPortPciAddressMap (GnbHandle, PortDevMap);
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * DXIO User Config Parser
 *
 *
 *
 * @param[in]  PeiServices    Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[out] ThisNode       Pointer to a location to store the address of the platform descriptor for this node
 * @param[out] NodeSize       Pointer to a location to store the size (in dwords) of the platforrm desciptor for this node
 * @param[out] AncData        Pointer to a location to store the address of the ancillary port data
 * @param[out] AncDataSize    Pointer to a location to store the size (in dwords) of the ancillary port data
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioParseUserConfig (
  IN       CONST EFI_PEI_SERVICES    **PeiServices,
  IN       GNB_HANDLE                *GnbHandle,
  IN       DXIO_COMPLEX_DESCRIPTOR   *UserConfig,
  OUT      DXIO_PLATFORM_DESCRIPTOR  **ThisNode,
  OUT      UINT32                    *NodeSize,
  OUT      UINT8                     **AncData,
  OUT      UINT32                    *AncDataSize
  )
{
  AGESA_STATUS               AgesaStatus;
  UINT8                      SocketNumber;
  UINT8                      InstanceId;
  UINT16                     NodeMinLane;
  UINT16                     NodeMaxLane;
  // DXIO Firmware Structures
  DXIO_PLATFORM_DESCRIPTOR   *NodePlatformDesc;
  DXIO_ENGINE_DESCRIPTOR     *EnginePointer;
  UINT32                     ThisNodeCount;
  UINT32                     ThisNodeSize;
  // Ancillary Data Structures
  UINT32                     DataSize;
  UINT8                      *AncillaryData;
  UINT32                     AncillaryDataSize;
  UINT32                     AncillaryDataOffset;
  ETHERNET_PORT_DATA         *EtherNetPortHeap;
  UINT32                     EtherNetPortOffset;
  DXIO_ANCILLARYDATA         *DxioAncData;
  DXIO_ANCILLARYDATA_SUBHEAP *DxioSubHeapData;

  UINT32                     EtherNetPortCount;
  //User Config Pointers
  DXIO_COMPLEX_DESCRIPTOR    *UserCfg;
  DXIO_PORT_DESCRIPTOR       *PortList;              // Pointer to start of User Config for this socket
  DXIO_PORT_DESCRIPTOR       *PortPointer;           // Updatable pointer for PortList processing
  BOOLEAN                    EndOfList;
  BOOLEAN                    SataEnabled;

  // Get Socket Number
  SocketNumber = GnbHandle->SocketId;
  InstanceId = GnbHandle->InstanceId;
  // Get Min/Max lane number for this node
  NodeMinLane = GnbHandle->StartLane;
  NodeMaxLane = GnbHandle->EndLane;

  SataEnabled = FALSE;
  AgesaStatus = AGESA_SUCCESS;
  //IDS_HDT_CONSOLE (PCIE_MISC, "Evaluating ports for Lane %d through %d\n", NodeMinLane, NodeMaxLane);
  // Locate socket-specific structure
  PortList = NULL;
  //IDS_HDT_CONSOLE (PCIE_MISC, "Looking for SocketNumber %d \n", SocketNumber);
  EndOfList = FALSE;
  UserCfg = UserConfig;
  while (EndOfList != TRUE) {
    //IDS_HDT_CONSOLE (PCIE_MISC, " This list is for socket %d\n", UserCfg->SocketId);
    if (UserCfg->SocketId == SocketNumber) {
      PortList = UserCfg->PciePortList;
    }
    if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      UserCfg++;
    } else {
      EndOfList = TRUE;
    }
  }
  // Parse structure and count entries for this node
  if (PortList != NULL) {
    PortPointer = PortList;
    ThisNodeSize = sizeof (DXIO_PLATFORM_DESCRIPTOR) + 8;  // Include 2 reserved UINT32 at the end
    ThisNodeCount = 0;
    EtherNetPortCount = 0;
    EtherNetPortOffset = 0;
    EtherNetPortHeap = NULL;
    AncillaryDataSize = 0;
    AncillaryDataOffset = NULL;
    EndOfList = FALSE;
    EnginePointer = NULL;

    // Calculate size of Platform Structure and Ancillary Data for this node
    do {
      if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
          (PortPointer->EngineData.EngineType != DxioUnusedEngine)) {
        ThisNodeSize += sizeof (DXIO_ENGINE_DESCRIPTOR);
        ThisNodeCount++;
        if (PortPointer->EngineData.EngineType == DxioEthernetEngine) {
          EtherNetPortCount++;
        } else {
          AncillaryDataSize += DxioAncDataCalcSize (PortPointer);
          //IDS_HDT_CONSOLE (PCIE_MISC, "This port is EtherNet, new count is %d and new AncillaryDataSize is 0x%x\n", ThisNodeCount, AncillaryDataSize);
        }
      }
      if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
        PortPointer++;
      } else {
        EndOfList = TRUE;
      }
    } while (EndOfList != TRUE);
    if (EtherNetPortCount != 0) {
      AncillaryDataSize += (4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
    }
    if (AncillaryDataSize != 0) {
      AncillaryDataSize += sizeof (DXIO_HEAPDATA_CONTAINER) + sizeof (DXIO_ANCILLARYDATA);
    }

    // Build data structures for this node if there is any configuration required
    if (ThisNodeCount != 0) {
      // Allocate memory for this node platform structure
      AgesaStatus = (*PeiServices)->AllocatePool (
                                      PeiServices,
                                      ThisNodeSize,
                                      (VOID **)&NodePlatformDesc
                               );
      //ASSERT_EFI_ERROR (AgesaStatus);
      *ThisNode = NodePlatformDesc;             // Save pointer to allocated memory
      *NodeSize = (ThisNodeSize + 3) >> 2;      // Platfom Descriptor size in dwords


      // Allocate memory for this node ancillary data
      AncillaryData = NULL;
      if (AncillaryDataSize != 0) {
        AgesaStatus = (*PeiServices)->AllocatePool (
                                        PeiServices,
                                        AncillaryDataSize,
                                        (VOID **)&AncillaryData
                                 );
        if (AgesaStatus == AGESA_SUCCESS) {
          *AncData = AncillaryData;                          // Save pointer to allocated memory
          *AncDataSize = AncillaryDataSize >> 2;             // Ancillary Data size in dwords

          LibAmdMemFill (AncillaryData, 0, AncillaryDataSize, (AMD_CONFIG_PARAMS *) NULL);

          DxioAncData = (DXIO_ANCILLARYDATA *) AncillaryData;
          DxioAncData->descriptorType = 0;   ///@todo
          DxioAncData->version = DXIO_ANCILLARYDATA_VERSION;
          DxioAncData->numDWords = (AncillaryDataSize >> 2) -1;
          AncillaryData += sizeof (DXIO_ANCILLARYDATA);

          AncillaryDataOffset = sizeof (DXIO_ANCILLARYDATA);

          if (EtherNetPortCount != 0) {
            DxioSubHeapData = (DXIO_ANCILLARYDATA_SUBHEAP *) AncillaryData;
            DxioSubHeapData->descriptorType = DXIO_ANCDATA_SUBHEAPTYPE_XGBE;
            DxioSubHeapData->version = DXIO_ANCILLARYDATA_SUBHEAP_VERSION;
            DxioSubHeapData->numDWORDs = ((4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (DXIO_ANCILLARYDATA_SUBHEAP)) >> 2;
            AncillaryData += sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
            EtherNetPortHeap = (ETHERNET_PORT_DATA *) AncillaryData;
            EtherNetPortOffset = AncillaryDataOffset;
            AncillaryData += (4 * sizeof (ETHERNET_PORT_DATA));
            AncillaryDataOffset += (4 * sizeof (ETHERNET_PORT_DATA)) + sizeof (DXIO_ANCILLARYDATA_SUBHEAP);
          }
        } else {
          *AncData = (UINT8 *) NULL;
          *AncDataSize = 0;
        }
      } else {
        *AncData = (UINT8 *) NULL;
        *AncDataSize = 0;
      }

      // Initialize platform level entries for this structure
      LibAmdMemFill (NodePlatformDesc, 0, ThisNodeSize, (AMD_CONFIG_PARAMS *) NULL);
      NodePlatformDesc->PlatformType = ZP_PLATFORM_TYPE;
      NodePlatformDesc->NumEngineDesc = ThisNodeCount;
      // Parse structure and copy entries for this node to this node platform structure
      PortPointer = PortList;
      NodePlatformDesc ++;                    // Point to first engine descriptor
      EnginePointer = (DXIO_ENGINE_DESCRIPTOR *)NodePlatformDesc;       // Initialize engine pointer to first engine
      NodePlatformDesc --;                    // Restore NodePlatformDesc address
      AncillaryDataSize = 0;
      EndOfList = FALSE;
      do {
        //IDS_HDT_CONSOLE (PCIE_MISC, "This port starts at lane %d and ends at lane %d\n", PortPointer->EngineData.StartLane,  PortPointer->EngineData.EndLane);
        if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
          (PortPointer->EngineData.EngineType != DxioUnusedEngine)) {
          if ((PortPointer->EngineData.StartLane < NodeMinLane) || (PortPointer->EngineData.StartLane > NodeMaxLane)) {  ///egh fix this
            //Invalid configuration
            ASSERT (FALSE);
          }
          if (PortPointer->EngineData.StartLane <= PortPointer->EngineData.EndLane) {
            EnginePointer->StartLane = PortPointer->EngineData.StartLane - (NodeMinLane);
            EnginePointer->EndLane = PortPointer->EngineData.EndLane - (NodeMinLane);
          } else {
            EnginePointer->EndLane = PortPointer->EngineData.StartLane - (NodeMinLane);
            EnginePointer->StartLane = PortPointer->EngineData.EndLane - (NodeMinLane);
          }
          ///@todo DXIO expects lanes 20-35 for PCIe1, but we are using 16-31.  Fix offset here
          if (EnginePointer->StartLane >= 16) {
            EnginePointer->StartLane +=4;
            EnginePointer->EndLane +=4;
          }
          EnginePointer->GpioGroupID = 1; //Unused
          EnginePointer->engineResetID = PortPointer->EngineData.GpioGroupId;
          EnginePointer->EngineType = PortPointer->EngineData.EngineType;
          EnginePointer->HotPluggable = PortPointer->EngineData.HotPluggable;
          switch (PortPointer->EngineData.EngineType) {
          case DxioPcieEngine:
            EnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink = PortPointer->Port.MiscControls.SbLink;

            if (PcdGet8 (PcdPcieLinkTrainingType) == 0) {
              IDS_HDT_CONSOLE (GNB_TRACE, "Train single step\n");
              if (EnginePointer->LinkDesc.Pcie.Capabilities.earlyTrainLink == 1) {
                EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = DxioGen1;
              } else {
                EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = PortPointer->Port.LinkSpeedCapability;
              }
            } else {
              IDS_HDT_CONSOLE (GNB_TRACE, "Train two step\n");
              EnginePointer->LinkDesc.Pcie.Capabilities.maxLinkSpeedCap = DxioGen1;
            }
            
            //Mask ASPM L0S
            PortPointer->Port.LinkAspm = PortPointer->Port.LinkAspm & 0x02;
            
            EnginePointer->LinkDesc.Pcie.DevFunNumber = (PortPointer->Port.DeviceNumber << 3) | PortPointer->Port.FunctionNumber;
            EnginePointer->LinkDesc.Pcie.Capabilities.enableTurnOffSettings = 1;
            if(PcdGetBool(PcdPcieSITestMode) == FALSE) {
              EnginePointer->LinkDesc.Pcie.Capabilities.turnOffUnusedLanes = PortPointer->Port.MiscControls.TurnOffUnusedLanes;
            } else {
              EnginePointer->LinkDesc.Pcie.Capabilities.turnOffUnusedLanes = 0; 
            }
            EnginePointer->LinkDesc.Pcie.Capabilities.portPresent = PortPointer->Port.PortPresent;
            if(PcdGetBool(PcdCfgPcieHotplugSupport) == FALSE) {
                PortPointer->Port.LinkHotplug = DxioHotplugDisabled;
            }
            EnginePointer->LinkDesc.Pcie.Capabilities.linkHotplug = PortPointer->Port.LinkHotplug;
            EnginePointer->LinkDesc.Pcie.Capabilities.autoSpeedChangeCntrl = PortPointer->Port.AutoSpdChng;

            if (PcdGet8(PcdPcieEqSearchMode) != 1) {
              EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchModeOride = 1;
              EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchMode =
                  PcdGet8 (PcdPcieEqSearchMode);
              IDS_HDT_CONSOLE (GNB_TRACE, "EqSearchMode: %d\n",
                  EnginePointer->LinkDesc.Pcie.Capabilities.eqSearchMode);
            }

            if (PortPointer->Port.LinkHotplug == DxioHotplugServerEntSSD) {
              EnginePointer->LinkDesc.Pcie.Capabilities.linkHotplug = DxioHotplugServerExpress;
            }
            if (PortPointer->EngineData.StartLane > PortPointer->EngineData.EndLane) {
              EnginePointer->LinkDesc.Pcie.Capabilities.linkReversal = 1;
            }
            DataSize = DxioAncDataPopulate (EnginePointer, PortPointer, AncillaryData);
            if (0 != DataSize) {
              EnginePointer->LinkDesc.Pcie.AncillaryDataOffset = AncillaryDataOffset >> 2;
              AncillaryData += DataSize;
              AncillaryDataOffset += DataSize;
            }
            break;
          case DxioSATAEngine:
            EnginePointer->LinkDesc.Sata.ChannelType = PortPointer->Port.MiscControls.ChannelType;
            SataEnabled = TRUE;
            DataSize = DxioAncDataPopulate (EnginePointer, PortPointer, AncillaryData);
            if (0 != DataSize) {
              EnginePointer->LinkDesc.Sata.AncillaryDataOffset = AncillaryDataOffset >> 2;
              AncillaryData += DataSize;
              AncillaryDataOffset += DataSize;
            }
            break;
          case DxioDPEngine:
            break;
          case DxioEthernetEngine:
            NbioEnableNbifDevice (GnbHandle, DxioEthernetEngine, (UINT8) EnginePointer->StartLane);
            EnginePointer->LinkDesc.Eth.AncillaryDataOffset = EtherNetPortOffset >> 2;
            EtherNetPortHeap += PortPointer->EtherNet.EthPortProp0.PortNum;
            LibAmdMemCopy ((VOID *) EtherNetPortHeap,
                           (VOID *) &PortPointer->EtherNet,
                           sizeof (ETHERNET_PORT_DATA),
                           (AMD_CONFIG_PARAMS *)NULL
                           );
            EtherNetPortHeap -= PortPointer->EtherNet.EthPortProp0.PortNum;
            break;
          case DxioGOPEngine:
            break;
          default:
            break;
          }
          //DxioEntryConfigDump (EnginePointer);
          EnginePointer++;
        }
        if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
          PortPointer++;
        } else {
          EndOfList = TRUE;
        }
      } while (EndOfList != TRUE);
    } else {
      // Nothing found for this node, skip DXIO call
      *ThisNode = NULL;             // Save pointer to allocated memory
      *NodeSize = 0;      // Platfom Descriptor size in dwords
    }
    if ((EnginePointer != NULL) && (SataEnabled == TRUE)) {
      NbioEnableNbifDevice (GnbHandle, DxioSATAEngine, (UINT8) EnginePointer->StartLane);
    } else {
      if (!PcdGetBool (PcdAmdSataEnable)) {
        NbioDisableNbifDevice (GnbHandle, DxioSATAEngine, 0);
      }
    }
  }
  // Return pointer to this node platform structure
  return (AgesaStatus);
}
/*----------------------------------------------------------------------------------------*/
/**
 * SET HWDEBUG REG
 * @param[in]  PeiServices    Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
SetHwdbgReg(
    IN       CONST EFI_PEI_SERVICES    **PeiServices,
    IN       GNB_HANDLE                *GnbHandle,
    IN       DXIO_COMPLEX_DESCRIPTOR   *UserConfig
 )
{
    AGESA_STATUS               AgesaStatus;
    UINT32                     Data32;
    UINT8                      SocketNumber;
    UINT8                      InstanceId;
    UINT16                     NodeMinLane;
    UINT16                     NodeMaxLane;
    BOOLEAN                    EndOfList;
    UINT32                     AncillaryDataSize=0;
    DXIO_COMPLEX_DESCRIPTOR    *UserCfg;
    DXIO_PORT_DESCRIPTOR       *PortList;
    DXIO_PORT_DESCRIPTOR       *PortPointer; 
    UINT16                     EngineStartLan,EngineEndLan;
    UINT16                     i;
    UINT32                     HwDbgSmnAddress;                        
    Data32 = 0;
    
    IDS_HDT_CONSOLE (PCIE_MISC, "SetHwdbgReg start\n");
    
    NbioRegisterWrite (GnbHandle,TYPE_SMN,0x1220B7DC,&Data32,0);
    NbioRegisterWrite (GnbHandle,TYPE_SMN,0x1230B7DC,&Data32,0);
    
    SocketNumber = GnbHandle->SocketId;
    InstanceId = GnbHandle->InstanceId;
    // Get Min/Max lane number for this node
    NodeMinLane = GnbHandle->StartLane;
    NodeMaxLane = GnbHandle->EndLane;

    AgesaStatus = AGESA_SUCCESS;
    PortList = NULL;
    EndOfList = FALSE;
    UserCfg = UserConfig;
  
    while (EndOfList != TRUE) {
      if (UserCfg->SocketId == SocketNumber) {  
        PortList = UserCfg->PciePortList;
      }
      if ((UserCfg->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
        UserCfg++;
      } else {
        EndOfList = TRUE;
      }
     }
    
    EndOfList = FALSE;
     
    if (PortList != NULL) {
        PortPointer = PortList;
        do {
             if (((PortPointer->EngineData.StartLane >= NodeMinLane) && (PortPointer->EngineData.StartLane <= NodeMaxLane)) &&
                 (PortPointer->EngineData.EngineType != DxioUnusedEngine)) {
                 if (PortPointer->EngineData.EngineType == DxioPcieEngine) {
                        AncillaryDataSize = DxioAncDataCalcSize (PortPointer);
                        if(AncillaryDataSize!=0){
                            AncillaryDataSize = 0;
                            if(PortPointer->EngineData.StartLane <= PortPointer->EngineData.EndLane){
                                EngineStartLan = PortPointer->EngineData.StartLane - NodeMinLane;
                                EngineEndLan = PortPointer->EngineData.EndLane - NodeMinLane;
                            } else {
                                EngineStartLan = PortPointer->EngineData.EndLane - NodeMinLane;
                                EngineEndLan = PortPointer->EngineData.StartLane - NodeMinLane;
                            }
                            HwDbgSmnAddress = (EngineStartLan<16)? 0x1220B7DC : 0x1230B7DC;                           
                            for(i=EngineStartLan;i<=EngineEndLan;i++)
                            {
                                //write HwDbg Reg
                                NbioRegisterRMW(GnbHandle,TYPE_SMN,HwDbgSmnAddress,0xFFFFFFFF,1<<(i%16),0);  
                            }
                        }
                }
             }
            if ((PortPointer->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
                      PortPointer++;
                    } else {
                      EndOfList = TRUE;
                    }
         }while(EndOfList != TRUE);
     }
     IDS_HDT_CONSOLE (PCIE_MISC, "SetHwdbgReg end\n");
     return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Ignore some useless errors, such as blk_err, os_wait_too_long, os_without_eds,
 * and wrong_os_after_eds_flag
 * 
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  GnbHandle       Pointer to the Silicon Descriptor for this node
 **/

VOID
IgnoreUselessErrorFlag (
    IN       CONST EFI_PEI_SERVICES   **PeiServices,
    IN       GNB_HANDLE               *GnbHandle
 )
{
  UINT32               Data32;
  UINT32               EdsFlagBit;
  UINT32               BlkErrFlagBit;
  UINT32               PcieCore0ErrorFlagSmnAddress;
  UINT32               PcieCore1ErrorFlagSmnAddress;
  UINT32               Mask;
  SOC_LOGICAL_ID       LogicalId;

  PcieCore0ErrorFlagSmnAddress  = 0x1118000c;
  PcieCore1ErrorFlagSmnAddress  = 0x1128000c;
  EdsFlagBit                    = BIT13;  //It is used to ignore blk_err
  BlkErrFlagBit                 = BIT14;  //It is used to ignore os_wait_too_long, os_without_eds, wrong_os_after_eds_flag.
  Data32                        = (UINT32) (EdsFlagBit | BlkErrFlagBit);
  Mask                          = (UINT32) (~Data32);

  BaseGetLogicalIdOnExecutingCore (&LogicalId);

  if (LogicalId.Revision & HYGON_REV_F18_DN_GTE_C0) {
    NbioRegisterRMW (GnbHandle, TYPE_SMN, PcieCore0ErrorFlagSmnAddress, Mask, Data32, 0);
    NbioRegisterRMW (GnbHandle, TYPE_SMN, PcieCore1ErrorFlagSmnAddress, Mask, Data32, 0);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * DXIO Process Results
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]  UserConfig     Pointer to the platform BIOS supplied platform configuration
 * @param[in]  ThisNode       Pointer to the platform descriptor for this node
 * @param[in]  ThisNodeSize   Size of the platform descriptor for this node
 * @param[in]  SmuServicesPpi Pointer to SMU Services PPI or similar entry point structure
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioProcessResultsV1 (
  IN       GNB_HANDLE                      *GnbHandle,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig,
  IN       DXIO_PLATFORM_DESCRIPTOR        *ThisNode,
  IN       UINT32                          ThisNodeSize,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi
  )
{
  AGESA_STATUS              AgesaStatus;
  UINT32                    SmuArg[6];
  UINT8                     DxioRetval;
  BOOLEAN                   DxioComplete;
  DXIO_RETURN_PARAMS        *ReturnArgs;
  LISM_RETURN_PARAMS        *LismRetParams;
  RESET_REQUEST_PARAMS      *ResetReqParams;
  GPIO_RESET_INFO           GpioResetInfo;
  PCI_ADDR                  EarlyTrainAddress;
  PCI_ADDR                  NbioAddress;
  DXIO_ENGINE_DESCRIPTOR    *EarlyTrainDesc;
  PCIe_ENGINE_CONFIG        *EarlyTrainEngine;
  BOOLEAN                   PcieStatusUpdated;
  PCIE_LC_STATE0_STRUCT     LcState0;
  GNB_HANDLE                *ResetHandle;

  // Process results

  AgesaStatus = AGESA_SUCCESS;
  ReturnArgs = (DXIO_RETURN_PARAMS*) &SmuArg[0];
  PcieStatusUpdated = FALSE;
  DxioComplete = FALSE;
  EarlyTrainAddress.AddressValue = 0;
  EarlyTrainDesc = NULL;
  EarlyTrainEngine = NULL;
  do {
    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_LISM_COLLECTRESULTS, SmuArg, SmuArg);
    DxioRetval = (UINT8) (SmuArg[0] & 0xFF);
    switch (DxioRetval) {
    case DXIO_MBOX_RETVAL_OK:
      switch (ReturnArgs->RetParams_Type) {
      case DXIO_MBOX_RETPARAMS_LISM:
        // Process return parameters
        LismRetParams = (LISM_RETURN_PARAMS*) &SmuArg[2];
        IDS_HDT_CONSOLE (GNB_TRACE, "Current State = 0x%x\n", LismRetParams->CurrentState);
        switch (LismRetParams->CurrentState) {
        case DXIO_LinkInitState_done:
            DxioComplete = TRUE;
            break;
        case DXIO_LinkInitState_mapped:
          // Ports are mapped.  Allow pre-configuration options to be programmed.
          IDS_HDT_CONSOLE (GNB_TRACE, "PORTS ARE MAPPED\n");
          LibAmdMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (AMD_CONFIG_PARAMS *) NULL);
          LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
          SmuArg[2] = (UINT32) ThisNode;
          SmuArg[3] = ThisNodeSize;
          SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);
//          DxioEngineMapping (GnbHandle, UserConfig, ThisNode);
          EarlyTrainDesc = NULL;
          DxioUpdatePcieStatus (GnbHandle,
                                UserConfig,
                                ThisNode,
                                (DXIO_ENGINE_DESCRIPTOR**)&EarlyTrainDesc,
                                &EarlyTrainAddress.AddressValue,
                                (PCIe_ENGINE_CONFIG **) &EarlyTrainEngine
                                );
          DxioCfgBeforeReconfig (GnbHandle);
          break;
        case DXIO_LinkInitState_configured:
          // Configuration is done.  Allow post-configuration, pre-training options to be programmed.
          IDS_HDT_CONSOLE (GNB_TRACE, "RECONFIG IS COMPLETE\n");
          DxioCfgAfterReconfig (GnbHandle);
          break;
        case DXIO_LinkInitState_vetting:
          if (PcdGetBool (PcdPcieSyncReset)) {
            // Set DxioComplete for syncing all dies in multi-die config
            DxioComplete = TRUE;
          }
        case DXIO_LinkInitState_earlyTrained:
          // Read back training results
          IDS_HDT_CONSOLE (GNB_TRACE, "Vetting or earlyTrained state detected...\n");
          if (PcieStatusUpdated != TRUE) {
            if (EarlyTrainDesc == NULL) {
              IDS_HDT_CONSOLE (GNB_TRACE, "EarlyTrainLink NOT FOUND\n");
            } else {
              IDS_HDT_CONSOLE (GNB_TRACE, "\nEarlyTrainLink found at 0x%x\n", EarlyTrainAddress);
              PcieStatusUpdated = TRUE;
              LibAmdMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (AMD_CONFIG_PARAMS *) NULL);
              LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
              SmuArg[2] = (UINT32) ThisNode;
              SmuArg[3] = ThisNodeSize;
              SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);
              EarlyTrainAddress.AddressValue = EarlyTrainAddress.AddressValue << 12;
              NbioAddress = NbioGetHostPciAddress (GnbHandle);
              if (EarlyTrainEngine != NULL) {
                if (EarlyTrainEngine->Type.Port.PortId > 7) {
                  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE1_LC_STATE0_ADDRESS + ((EarlyTrainEngine->Type.Port.PortId - 8) << 12), &LcState0.Value, 0);
                } else {
                  NbioRegisterRead (GnbHandle, TYPE_SMN, PCIE0_LC_STATE0_ADDRESS + (EarlyTrainEngine->Type.Port.PortId << 12), &LcState0.Value, 0);
                }
                IDS_HDT_CONSOLE (PCIE_MISC, "LcState0 for earlyTrainLink is 0x%x\n", LcState0.Value);
                if ((LcState0.Field.LC_CURRENT_STATE >= 0x10) ||
                    (LcState0.Field.LC_PREV_STATE1 >= 0x10) ||
                    (LcState0.Field.LC_PREV_STATE2 >= 0x10) ||
                    (LcState0.Field.LC_PREV_STATE3 >= 0x10)) {
                  // earlyTrainLink is trained - report to PSP
                  EarlyTrainAddress.AddressValue |= NbioAddress.AddressValue;
                  GnbLibPciRMW (EarlyTrainAddress.AddressValue | 0x18, AccessWidth32, 0xFF000FF, 0x020200, (AMD_CONFIG_PARAMS *) NULL);

                  LibAmdMemFill ((VOID *)ThisNode, 0x00, ThisNodeSize << 2, (AMD_CONFIG_PARAMS *) NULL);
                  LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
                  SmuArg[2] = (UINT32) ThisNode;
                  SmuArg[3] = ThisNodeSize;
                  SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);
                  EarlyTrainDesc->LinkDesc.Pcie.DevFunNumber = 0x200;
                  IDS_HDT_CONSOLE (GNB_TRACE, "Sending this Platform Descriptor back to SMU...\n");
                  GnbLibDebugDumpBuffer ((VOID*) ThisNode, ThisNodeSize << 2, 1, 16);
                  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
                  LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
                  SmuArg[1] = 0;
                  SmuArg[2] = (UINT32) ThisNode;
                  SmuArg[3] = ThisNodeSize;
                  SmuArg[5] = DXIO_HEAPTYPE_CFGPTR;
                  SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);
                }
              }
            }
          }
          break;
        default:
          break;
        }
        break;
      case DXIO_MBOX_RETPARAMS_REQ_RESET:
        // Issue reset request for specific PCIe Engine
        ResetReqParams = (RESET_REQUEST_PARAMS*) &SmuArg[2];
        GpioResetInfo.ResetId = ResetReqParams->GpioId;
        GpioResetInfo.ResetControl = ResetReqParams->resetAssertDeassert;
        if (PcdGetBool (PcdPcieSyncReset)) {
	        if (ResetReqParams->resetAssertDeassert == 0) {
	          if (GnbHandle->Address.Address.Bus == 0) {
	            ResetHandle = GnbHandle;
	            while (ResetHandle != NULL) {
	              AgesaGpioSlotResetControl((UINTN)ResetHandle->Address.Address.Bus, &GpioResetInfo);
	              IDS_HDT_CONSOLE (GNB_TRACE, "Reset Assert Request for GpioId 0x%x on Bus 0x%x\n", ResetReqParams->GpioId, ResetHandle->Address.Address.Bus);
	              ResetHandle = GnbGetNextHandle (ResetHandle);
	            }
	          }
	        } else {
	          if (GnbHandle->Address.Address.Bus == 0) {
	            ResetHandle = GnbHandle;
	            while (ResetHandle != NULL) {
	              AgesaGpioSlotResetControl((UINTN)ResetHandle->Address.Address.Bus, &GpioResetInfo);
	              IDS_HDT_CONSOLE (GNB_TRACE, "Reset Deassert Request for GpioId 0x%x on Bus 0x%x\n", ResetReqParams->GpioId, ResetHandle->Address.Address.Bus);
	              ResetHandle = GnbGetNextHandle (ResetHandle);
	            }
	          }
	        }
        } else {
	        if (ResetReqParams->resetAssertDeassert == 0) {
	          AgesaGpioSlotResetControl ((UINTN) GnbHandle->Address.Address.Bus, &GpioResetInfo);
	          IDS_HDT_CONSOLE (GNB_TRACE, "Reset Assert Request for GpioId 0x%x\n", ResetReqParams->GpioId);
	        } else {
	          AgesaGpioSlotResetControl ((UINTN) GnbHandle->Address.Address.Bus, &GpioResetInfo);
	          IDS_HDT_CONSOLE (GNB_TRACE, "Reset Deassert Request for GpioId 0x%x\n", ResetReqParams->GpioId);
	        }
        }
		
        break;
      case DXIO_MBOX_RETPARAMS_NONE:
        DxioComplete = TRUE;
        break;
      case DXIO_MBOX_RETPARAMS_GENERIC:
        // Process return parameters
        break;
      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "Invalid return parameter [0x%x]\n", DxioRetval);
        ASSERT (FALSE);
        break;
      }
      break;
    case DXIO_MBOX_RETVAL_ERROR:
      switch (ReturnArgs->RetParams_Type) {
      case DXIO_MBOX_RETPARAMS_LISM:
        LismRetParams = (LISM_RETURN_PARAMS*) &SmuArg[2];
        IDS_HDT_CONSOLE (GNB_TRACE, "Current State = 0x%x", LismRetParams->CurrentState);
        AgesaStatus = AGESA_CRITICAL;
        DxioComplete = TRUE;
        break;
      default:
        ASSERT (FALSE);
        break;
      }
      break;
    default:
      IDS_HDT_CONSOLE (GNB_TRACE, "DXIO Mailbox Request did not return OK [0x%x]\n", DxioRetval);
      ASSERT (FALSE);
      DxioComplete = TRUE;
      break;
    }
    if (FALSE == DxioComplete) {
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_LISM_RESUME, SmuArg, SmuArg);
    }
  } while (FALSE == DxioComplete);
  if ((PcieStatusUpdated != TRUE) && (EarlyTrainDesc != NULL)) {
    GnbLibPciRMW (EarlyTrainAddress.AddressValue | 0x18, AccessWidth32, 0xFF000FF, 0x000000, (AMD_CONFIG_PARAMS *) NULL);
  }
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Concurrent Training
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  Pcie            Pointer to the AGESA platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioConcurrentTraining (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  AGESA_STATUS              AgesaStatus;
  AGESA_STATUS              Status;
  GNB_HANDLE                *GnbHandle;
  UINT32                    SmuArg[6];
  UINT8                     InstanceId;
  UINT8                     DxioRetval;
  DXIO_PLATFORM_DESCRIPTOR  *PlatformDesc[MAX_INSTANCE_ID];
  UINT32                    PlatformDescSize[MAX_INSTANCE_ID];
  UINT8                     *AncillaryData[MAX_INSTANCE_ID];
  UINT32                    AncillaryDataSize[MAX_INSTANCE_ID];

  AgesaStatus = AGESA_SUCCESS;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    DxioParseUserConfig (PeiServices,
                         GnbHandle,
                         UserConfig,
                         &PlatformDesc[InstanceId],
                         &PlatformDescSize[InstanceId],
                         &AncillaryData[InstanceId],
                         &AncillaryDataSize[InstanceId]
                         );
    
    SetHwdbgReg(PeiServices,GnbHandle,UserConfig);
    IgnoreUselessErrorFlag (PeiServices, GnbHandle);
    
    if (PlatformDesc[InstanceId] != NULL) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Platform Descriptor for Die %d\n", InstanceId);
      DxioDebugDump (PlatformDesc[InstanceId],
                     PlatformDescSize[InstanceId],
                     AncillaryData[InstanceId],
                     AncillaryDataSize[InstanceId]);
    }

    if (InstanceId == UserConfig->BmcLinkLocation) {
      LibAmdMemFill(SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *)NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_RELOAD, SmuArg, SmuArg);
    }

    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_INIT, SmuArg, SmuArg);
    DxioRetval = (UINT8) (SmuArg[0] & 0xFF);
    if (DxioRetval != DXIO_MBOX_RETVAL_OK) {
      PlatformDesc[InstanceId] = NULL;
    }

    if (PlatformDesc[InstanceId] != NULL) {

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_GETVERSION, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERMAPPING;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERCONFIG;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = DBUG_SETSCRATCHFLAG_V1ANCILLARYDATA;
      SmuArg[2] = 1;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADCAPABILITIES, SmuArg, SmuArg);

      if (AncillaryDataSize[InstanceId] != 0) {
        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = 0;
        SmuArg[2] = (UINT32) AncillaryData[InstanceId];
        SmuArg[3] = AncillaryDataSize[InstanceId];
        SmuArg[5] = DXIO_HEAPTYPE_ANCDATA;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);
      }

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_MACPCS;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_GPIOPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = 0;
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuArg[5] = DXIO_HEAPTYPE_CFGPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      // Inititiate configuration
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_GO, SmuArg, SmuArg);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  //release all die PCIe reset
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    PcieGen4PciReset(GnbHandle->Address.Address.Bus);
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    if (PlatformDesc[InstanceId] != NULL) {
      // Process results
      Status = DxioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId], SmuServicesPpi);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    if (PlatformDesc[InstanceId] != NULL) {
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, GnbHandle->InstanceId, DXIO_MSG_LISM_RESUME, SmuArg, SmuArg);
      // Process results
      Status = DxioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId], SmuServicesPpi);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);

      // Read back training results
      LibAmdMemFill ((VOID *)(PlatformDesc[InstanceId]), 0x00, PlatformDescSize[InstanceId] << 2, (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);

      DxioUpdatePortTrainingStatus (GnbHandle, UserConfig, PlatformDesc[InstanceId], SmuServicesPpi);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Non Concurrent Training
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  Pcie            Pointer to the AGESA platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioNonConcurrentTraining (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  AGESA_STATUS              AgesaStatus;
  AGESA_STATUS              Status;
  GNB_HANDLE                *GnbHandle;
  UINT32                    SmuArg[6];
  UINT8                     InstanceId;
  UINT8                     DxioRetval;
  DXIO_PLATFORM_DESCRIPTOR  *PlatformDesc[MAX_INSTANCE_ID];
  UINT32                    PlatformDescSize[MAX_INSTANCE_ID];
  UINT8                     *AncillaryData[MAX_INSTANCE_ID];
  UINT32                    AncillaryDataSize[MAX_INSTANCE_ID];

  AgesaStatus = AGESA_SUCCESS;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InstanceId = GnbHandle->InstanceId;
    DxioParseUserConfig (PeiServices,
                         GnbHandle,
                         UserConfig,
                         &PlatformDesc[InstanceId],
                         &PlatformDescSize[InstanceId],
                         &AncillaryData[InstanceId],
                         &AncillaryDataSize[InstanceId]
                         );
    
    SetHwdbgReg(PeiServices,GnbHandle,UserConfig);
    IgnoreUselessErrorFlag (PeiServices, GnbHandle);
    
    if (PlatformDesc[InstanceId] != NULL) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Platform Descriptor for Die %d\n", InstanceId);
      DxioDebugDump (PlatformDesc[InstanceId],
                     PlatformDescSize[InstanceId],
                     AncillaryData[InstanceId],
                     AncillaryDataSize[InstanceId]);
    }

    if (InstanceId == UserConfig->BmcLinkLocation) {
      LibAmdMemFill(SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *)NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_RELOAD, SmuArg, SmuArg);
    }

    LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
    SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_INIT, SmuArg, SmuArg);
    DxioRetval = (UINT8) (SmuArg[0] & 0xFF);


    if ((PlatformDesc[InstanceId] != NULL) && (DxioRetval == DXIO_MBOX_RETVAL_OK)) {

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_GETVERSION, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERMAPPING;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = DBUG_SETSCRATCHFLAG_RETURNAFTERCONFIG;
        SmuArg[2] = 1;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = DBUG_SETSCRATCHFLAG_V1ANCILLARYDATA;
      SmuArg[2] = 1;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_DBUG_SETSCRATCH, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADCAPABILITIES, SmuArg, SmuArg);

      if (AncillaryDataSize[InstanceId] != 0) {
        LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
        SmuArg[1] = 0;
        SmuArg[2] = (UINT32) AncillaryData[InstanceId];
        SmuArg[3] = AncillaryDataSize[InstanceId];
        SmuArg[5] = DXIO_HEAPTYPE_ANCDATA;
        SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);
      }

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_MACPCS;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[4] = 1;
      SmuArg[5] = DXIO_HEAPTYPE_GPIOPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[1] = 0;
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuArg[5] = DXIO_HEAPTYPE_CFGPTR;
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_LOADHEAP, SmuArg, SmuArg);

      // Inititiate configuration
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_LISM_GO, SmuArg, SmuArg);
      
      PcieGen4PciReset(GnbHandle->Address.Address.Bus);
      
      // Process results
      Status = DxioProcessResultsV1 (GnbHandle, UserConfig, PlatformDesc[InstanceId], PlatformDescSize[InstanceId], SmuServicesPpi);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);

      // Read back training results
      LibAmdMemFill ((VOID *)(PlatformDesc[InstanceId]), 0x00, PlatformDescSize[InstanceId] << 2, (AMD_CONFIG_PARAMS *) NULL);
      LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
      SmuArg[2] = (UINT32) PlatformDesc[InstanceId];
      SmuArg[3] = PlatformDescSize[InstanceId];
      SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DXIO_MSG_INIT_DMAENGINECONIFGTODRAM, SmuArg, SmuArg);

      DxioUpdatePortTrainingStatus (GnbHandle, UserConfig, PlatformDesc[InstanceId], SmuServicesPpi);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * SMU Early Init
 *
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  Pcie            Pointer to the AGESA platfom complex
 * @param[in]  UserConfig      Pointer to the platform BIOS supplied platform configuration
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
DxioEarlyInitV1 (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       DXIO_COMPLEX_DESCRIPTOR         *UserConfig
  )
{
  AGESA_STATUS              AgesaStatus;

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioEarlyInitV1 Enter\n");

  AgesaStatus = AGESA_SUCCESS;

  if (PcdGetBool (PcdPcieSyncReset)) {
    AgesaStatus = DxioConcurrentTraining (PeiServices, SmuServicesPpi, Pcie, UserConfig);
  } else {
    AgesaStatus = DxioNonConcurrentTraining (PeiServices, SmuServicesPpi, Pcie, UserConfig);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioEarlyInitV1 Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Port Speed Update Request
 *
 *
 *
 * @param[in]  PeiServices     Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  SmuServicesPpi  Pointer to SMU Services PPI or similar entry point structure
 * @param[in]  InstanceId      The unique identifier of the NBIO instance associated with this socket/die
 * @param[in]  EngineId        PCIe bridge ID
 * @param[in]  TargetSpeed     Desired speed of the target port (Gen1, Gen2, Gen3)
 */

VOID
DxioSetLinkSpeedV1 (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PEI_AMD_NBIO_SMU_SERVICES_PPI   *SmuServicesPpi,
  IN       UINT32                          InstanceId,
  IN       UINT32                          EngineId,
  IN       UINT32                          TargetSpeed
  )
{
  UINT32                    SmuArg[6];
  UINT32                    DxioMboxMessage;


  LibAmdMemFill (SmuArg, 0x00, 24, (AMD_CONFIG_PARAMS *) NULL);
  SmuArg[1] = DxioPcieEngine;
  SmuArg[2] = TargetSpeed;
  SmuArg[3] = EngineId;

  DxioMboxMessage = DXIO_MSG_PHYS_TARGETFREQUPDATE | (DXIO_freqUpdType_single << 10);
  SmuServicesPpi->SmuDxioServiceRequest (SmuServicesPpi, InstanceId, DxioMboxMessage, SmuArg, SmuArg);
}

