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

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description:	
//
//<AMI_FHDR_END>
//**********************************************************************

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <Dxe.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/UefiLib.h>
#include "OemGraphicSwitchElink.h"
#include <Library/AmdSocBaseLib.h>

#include "Library/BaseMemoryLib.h"

#include "Protocol/PciPlatform.h"
#include "PciBus.h"
#include "Protocol\FabricResourceManagerServicesProtocol.h"

//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format+>
#include <Library/BaseGetRbBusLib.h>
//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format->

#define F17_SSP_RAW_ID     0x00830F00ul
#define SSP_SP3  4

//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format+>
#define MAX_DIE_COUNT_PER_SOCKET 0x4
#define DF_SYSTEM_CONFIG_REG 0x200
//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format->


//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern EFI_RUNTIME_SERVICES     *gRT;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_SYSTEM_TABLE         *gST;

BOOLEAN IsOffboardGfxDevice(
    UINT8 bus,
    UINT8 device,
    UINT8 function
);
BOOLEAN IsVgaSlotNumMatch(
    UINT8 bus,
    UINT8 device,
    UINT8 function,
    UINT8 VgaSlotNum
);
VOID LoopGraphicDevice();
VOID ClearBusResource();
void ShowString(UINT32 FrameBuffer,CHAR16 *String);//liutz001-20200310 Show string when switch to offboard vga

UINT8 OnExtVgaSelect = 0;
UINT8 VgaSlotNum = 3;
UINT8 RbIndex = 0;
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit +>
//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format+>
PCI_SLOT_BRIDGE_ENTRY          PciBridgeList[] = { PCI_SLOT_BRIDGE_LIST
                                            {0xff,0xff,0xff,0xff,0xff,0xff}
                                            };
//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format->
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit ->
UINT8 PciBridgeIndex = 0;//For bus resource clear
PCI_BRIDGE_ENTRY               PciBridgeInfo[12] = {
                                                    {0xff,0xff, 0xff}, {0xff, 0xff, 0xff}, {0xff, 0xff, 0xff},
                                                    {0xff,0xff, 0xff}, {0xff, 0xff, 0xff}, {0xff, 0xff, 0xff},
                                                    {0xff,0xff, 0xff}, {0xff, 0xff, 0xff}, {0xff, 0xff, 0xff},
                                                    {0xff,0xff, 0xff}, {0xff, 0xff, 0xff}, {0xff, 0xff, 0xff}
                                                    };//<xuyj1-20181130- release bus resource +>

#define NB_SMN_INDEX_EXT_3_BIOS  0x00C0
#define NB_SMN_INDEX_3_BIOS  0x00C4
#define NB_SMN_DATA_3_BIOS   0x00C8

typedef struct _NBIO_PICE_CONTROLLER_INFO
{
  UINT8                       Die;
  UINT8                       Nbio;
  UINT8                       Device;
  UINT8                       PcieController;
} NBIO_PICE_CONTROLLER_INFO;

NBIO_PICE_CONTROLLER_INFO   NbioInfo[] = {
        {0, 0, 1, 0},
        {0, 0, 3, 1}, //P0, Pcie1
        {1, 1, 1, 0}, //P1, Pcie0
        {1, 1, 3, 1},
        {2, 3, 1, 0}, //P2, Pcie0
        {2, 3, 3, 1},
        {3, 2, 1, 0}, //P3, Pcie1
        {3, 2, 3, 1}
        
};

BOOLEAN IsOffboardGfxDevice(
    UINT8 bus,
    UINT8 device,
    UINT8 function
)
{
    UINT32                              PciDeviceClass;
    UINT8                               BaseClassCode;
    UINT8                               SubClassCode;

    PciDeviceClass = PciRead32(PCI_LIB_ADDRESS(bus, device, function, PCI_REVISION_ID_OFFSET));
    BaseClassCode = (UINT8)(PciDeviceClass >> 24);
    SubClassCode = (UINT8)(PciDeviceClass >> 16);
    if ((BaseClassCode == 0x03 && SubClassCode == 0x00) ||
        (BaseClassCode == 0x03 && SubClassCode == 0x80) ||
        (BaseClassCode == 0x04 && SubClassCode == 0x00))
        return TRUE;
    else
        return FALSE;
}

EFI_STATUS EnumerateBus(UINT8 Bus,UINT8 BusLimit)
{
    EFI_STATUS                          Status;
    UINT8                               SecBusNum;
    UINT8                               Device;
    UINT8                               Function;
    BOOLEAN                             MultiFunc=FALSE;
    BOOLEAN                             GfxFound=FALSE;
    UINT8                               HeaderType;
    UINT16                              VendorId = 0;

    for( Device=0; Device <= PCI_MAX_DEVICE; Device++) {
        for( Function = 0; Function <= PCI_MAX_FUNC; Function++ ) {
            VendorId = PciRead16(PCI_LIB_ADDRESS(Bus, Device, Function, PCI_VENDOR_ID_OFFSET));
            if((Function == 0) && VendorId == DEVICE_ID_NOCARE) {
                break;
            } else if ( VendorId == DEVICE_ID_NOCARE ) {
                continue;
            }
            DEBUG((EFI_D_INFO, "Enumerating Bus @ B[%x] D[%x] F[%x] L[%x]PciBridgeIndex %d\n", Bus, Device, Function,BusLimit,PciBridgeIndex));
            HeaderType = PciRead8(PCI_LIB_ADDRESS(Bus, Device, Function, PCI_HEADER_TYPE_OFFSET));
            if( Function == 0) {
                if(HeaderType & HEADER_TYPE_MULTI_FUNCTION) {
                    MultiFunc = TRUE;
                }
            }
            HeaderType &= 0x03;
            switch(HeaderType) {
                case HEADER_TYPE_DEVICE :
                    GfxFound = IsOffboardGfxDevice(Bus, Device, Function);
                    DEBUG((EFI_D_INFO, "Bus %x Device %x Function %x\n", Bus, Device, Function));
                    if (!GfxFound)
                        continue;
                    if (OnExtVgaSelect == 0)
                        PcdSetBool(PcdGraphicSwitch, 1);
                    else if (OnExtVgaSelect == 2)
                    {//liutz001-20200310 Show string when switch to offboard vga++
                       // if (PciBridgeList[RbIndex].Slot == VgaSlotNum)
                       // {
                            PcdSetBool(PcdGraphicSwitch, 1);
                            DEBUG((EFI_D_INFO, "OnExtVgaSelect %x VgaSlotNum %x\n", OnExtVgaSelect, VgaSlotNum ));
                       // }
					   //liutz001-20200310 Show string when switch to offboard vga--
                    }
                    if (PcdGetBool(PcdGraphicSwitch))
                        return EFI_SUCCESS;
                    break;
                case HEADER_TYPE_PCI_TO_PCI_BRIDGE:
                    PciBridgeInfo[PciBridgeIndex].Bus = Bus;
                    PciBridgeInfo[PciBridgeIndex].Device = Device;
                    PciBridgeInfo[PciBridgeIndex].Function = Function;
                    PciBridgeIndex++;
                    if (PciBridgeIndex > 12)
                        return EFI_UNSUPPORTED;
                    SecBusNum = Bus + 1;
                    PciWrite8 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),\
                                SecBusNum);
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit +>								
                    //zanggy-202101119-Improve the process of scanning devices on the slot+>
                    PciWrite8 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                BusLimit);
                    //zanggy-202101119-Improve the process of scanning devices on the slot->
                    Status = EnumerateBus(SecBusNum,BusLimit);
                    //hujf1001-20210709 Clear Bus resource to fix GFX is under switch +>
                    ClearBusResource();
                    //hujf1001-20210709 Clear Bus resource to fix GFX is under switch +>
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit ->					
                    if ( EFI_ERROR( Status )) {
                        return Status;
                    }
                    if (PcdGetBool(PcdGraphicSwitch))
                        return EFI_SUCCESS;
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
    return EFI_SUCCESS;
}

VOID ClearBusResource()
{
    UINT8                               Bus;
    UINT8                               Device;
    UINT8                               Function;
    
//hujf1001-20210709 Clear Bus resource to fix GFX is under switch +>
//    while (PciBridgeIndex != 0)
    if (PciBridgeIndex >= 1)
    {
        PciBridgeIndex--;
        //<xuyj1-20181130- release bus resource +>
       // if (PciBridgeInfo[PciBridgeIndex].Bus == 0)
         //   return;
        //<xuyj1-20181130- release bus resource ->
        Bus = PciBridgeInfo[PciBridgeIndex].Bus;
        Device = PciBridgeInfo[PciBridgeIndex].Device;
        Function = PciBridgeInfo[PciBridgeIndex].Function;
        
        PciWrite8 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),\
                                        0);
        PciWrite8 (PCI_LIB_ADDRESS (Bus, Device, Function, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                                        0);
        DEBUG((EFI_D_INFO, "Clear Bus resource for B%x D%x F%x, PciBridgeIndex %d\n", Bus, Device, Function, PciBridgeIndex));
    }
//hujf1001-20210709 Clear Bus resource to fix GFX is under switch ->
}
//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format+>
UINT32
OemGetNumberOfProcessorsPresent ()
{
  UINT32  SysCfg;
  UINT32  OtherSocketPresent;
  BOOLEAN IsSspOrGn = FALSE;
    
  if (IsSOCSSP()||IsSOCGn()) {
    IsSspOrGn = TRUE;
  }
      
  SysCfg  = PciRead32 (PCI_LIB_ADDRESS (
              0,
              0x18, 
              1, 
              DF_SYSTEM_CONFIG_REG)
              );

  if (IsSspOrGn) {
    //bit 27 indicates OtherSocketPresent
    OtherSocketPresent = (SysCfg >> 27) & 0x1;
  } else {
    //bit 5 indicates OtherSocketPresent
    OtherSocketPresent = (SysCfg >> 5) & 0x1;
  }
  
  return (OtherSocketPresent + 1);
}

VOID GetExactBusNumber()
{
  UINT8     SlotIndex;
  UINT8     RbNumber;
  UINT8     SocketNumber;
  UINT8     DieNumber;
  UINT8     DstFabricId;
  UINT32    SocketPresentCount;

  SocketPresentCount = OemGetNumberOfProcessorsPresent();

  for( SlotIndex=0; PciBridgeList[SlotIndex].Function != 0xff; SlotIndex++) {
    RbNumber = PciBridgeList[SlotIndex].RbNumber;
    //Ignore invalid RootBridge number
    if (RbNumber > 7) {
      continue;
    }
    //If only one socket present, ignore the RootBridge 4-7
    if (RbNumber >= 4 && SocketPresentCount <= 1) {
      continue;
    }
    
    if (RbNumber < MAX_DIE_COUNT_PER_SOCKET) {
      SocketNumber = 0;
      DieNumber = RbNumber;
    } else {
      SocketNumber = 1;
      DieNumber = RbNumber - MAX_DIE_COUNT_PER_SOCKET;
    }
  
    PciBridgeList[SlotIndex].Bus = GetCPURbBusNumAndFabricId (SocketNumber, DieNumber, &DstFabricId);
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit +>
    PciBridgeList[SlotIndex].BusLimit = GetCPURbBusNumLimitAndFabricId (SocketNumber, DieNumber, &DstFabricId);
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit ->
  }
}
//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format->
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   LoopGraphicDevice
//
// Description: Loop for Graphic Device
//
// Input: None
//
// Output:  None
//
//<AMI_PHDR_END>
//**********************************************************************
VOID LoopGraphicDevice()
{
    UINT8                                   Index;
    UINT8                                   HeaderType = 0;
    //zanggy-202101119-Improve the process of scanning devices on the slot+>
    UINT32                                  SocketPresentCount;
    
    
    SocketPresentCount = OemGetNumberOfProcessorsPresent();
    //zanggy-202101119-Improve the process of scanning devices on the slot->
    
    for( Index=0; PciBridgeList[Index].Function != 0xff ; Index++) {
        //zanggy-202101119-Improve the process of scanning devices on the slot+>
        //Ignore invalid RootBridge number
        if (PciBridgeList[Index].RbNumber > 7) {
          continue;
        }
        //If only one socket present, ignore the RootBridge 4-7
        if (PciBridgeList[Index].RbNumber >= 4 && SocketPresentCount <= 1) {
          continue;
        }
        //zanggy-202101119-Improve the process of scanning devices on the slot->
        if( PciBridgeList[Index].Bus == 0xff && PciBridgeList[Index].Device == 0xff &&
                PciBridgeList[Index].Function == 0xff ) {
            DEBUG((EFI_D_INFO, "\n No PCI Bridge Entry added for device Enumeration in PEI phase\n"));
            return;
        }
        DEBUG((EFI_D_INFO, "Enumerating Bridge @ B[%x] D[%x] F[%x] L[%x]\n",   
                                                PciBridgeList[Index].Bus,
                                                PciBridgeList[Index].Device, 
                                                PciBridgeList[Index].Function,
                                                PciBridgeList[Index].BusLimit));
        PciBridgeIndex = 0;
        RbIndex = Index;
        HeaderType = PciRead8(PCI_LIB_ADDRESS(
                                          PciBridgeList[Index].Bus,
                                          PciBridgeList[Index].Device,
                                          PciBridgeList[Index].Function,
                                          PCI_HEADER_TYPE_OFFSET));
        HeaderType &= 0x03;
        if(HeaderType != HEADER_TYPE_PCI_TO_PCI_BRIDGE) continue;
        //<xuyj1-20181130- release bus resource +>
        PciBridgeInfo[PciBridgeIndex].Bus = PciBridgeList[Index].Bus;
        PciBridgeInfo[PciBridgeIndex].Device = PciBridgeList[Index].Device;
        PciBridgeInfo[PciBridgeIndex].Function = PciBridgeList[Index].Function;
        PciBridgeIndex++;
        //<xuyj1-20181130- release bus resource ->
        PciWrite8 (PCI_LIB_ADDRESS (
                                    PciBridgeList[Index].Bus,
                                    PciBridgeList[Index].Device, 
                                    PciBridgeList[Index].Function, 
                                    PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),\
                PciBridgeList[Index].Bus +PCI_BUS_NUMBER_START);
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit +>            
        PciWrite8 (PCI_LIB_ADDRESS (
                                    PciBridgeList[Index].Bus,
                                    PciBridgeList[Index].Device,
                                    PciBridgeList[Index].Function, 
                                    PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET),\
                PciBridgeList[Index].BusLimit);
        EnumerateBus(PciBridgeList[Index].Bus +PCI_BUS_NUMBER_START,PciBridgeList[Index].BusLimit);
//<libj-20220215 Improve PCI SLOT BRIDGE LIST definition format add limit ->		
        ClearBusResource();
        if (PcdGetBool(PcdGraphicSwitch))
        {
            DEBUG((EFI_D_INFO, "Found External GFX and system will switch to offboard GFX."));
            break;
        }
    }
}

VOID
NbSmnRead (
  IN    UINT32  DieNum,
  IN    UINT32  Address,
  IN    UINT32  *Value
  )
{
    UINT32 read_value=0;

    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    read_value=PciRead32(NB_SMN_DATA_3_BIOS);
    *Value=read_value;

    //clear in case other functions don't pay attention to this reg
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);

}

VOID
NbSmnWrite (
  IN    UINT32  DieNum,
  IN    UINT32  Address,
  IN    UINT32  *Value
  )
{
    UINT32 write_value=*Value;

    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
    PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
    PciWrite32(NB_SMN_DATA_3_BIOS, write_value);

    //clear in case other functions don't pay attention to this reg
    PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);

}

VOID
NbSmnRW (
  IN    UINT32  DieNum,
  IN    UINT32  Address,
  IN    UINT32  AndMask,
  IN    UINT32  OrMask
  )
{
    UINT32  RegValue;

    NbSmnRead (DieNum, Address, &RegValue);
    RegValue &= AndMask;
    RegValue |= OrMask;
    NbSmnWrite (DieNum, Address, &RegValue);
}

BOOLEAN
IsSocSsp(
 VOID
 )
{
    SOC_ID_STRUCT   SocId;
    
    SocId.SocFamilyID = F17_SSP_RAW_ID;
    SocId.PackageType = SSP_SP3;
    if (SocHardwareIdentificationCheck(&SocId)){
        return TRUE;        // SSP
    } else {
        return FALSE;       // ZP
    }
}

VOID DisableOnBoardVideo (
    IN EFI_EVENT  Event,
    IN VOID       *Context
    )
{
    UINT8   Index, LogicalBridgeId, AMD_Bus_Num, AMD_Dev_Num = AST_SB_DEV_NUM, AMD_Fuc_Num = AST_SB_FUNC_NUM;
    UINT32  Addr;
    
    if((AMD_Dev_Num == 2) || (AMD_Dev_Num == 4))
    {
        AMD_Dev_Num--;
        AMD_Fuc_Num++;
    }
    if (IsSocSsp())
    {
        Index = (DIE_NUM_FOR_VGA_CONTROLLER % 4) * 2 + ((AMD_Dev_Num == 1) ? 0 : 1);
        LogicalBridgeId = NbioInfo[Index].PcieController * 8 + AMD_Fuc_Num - 1;
        Addr = 0x13B31004 + (NbioInfo[Index].Nbio << 20) + (LogicalBridgeId << 10);
    } else 
    {
        LogicalBridgeId = ((AMD_Dev_Num == 1) ? 0 : 1) * 8 + AMD_Fuc_Num - 1;
        Addr = 0x13B31004 + (LogicalBridgeId << 10);
    }
    DEBUG((-1, "AMD_Dev_Num %x AMD_Fuc_Num %x Index %x LogicalBridgeId %x\n", AMD_Dev_Num, AMD_Fuc_Num, Index, LogicalBridgeId));

    AMD_Bus_Num = PcdGet8(PcdFastVideoBusNum);
    PciOr8(PCI_LIB_ADDRESS(AMD_Bus_Num, AST_SB_DEV_NUM, AST_SB_FUNC_NUM, 0x68), BIT4);
    //HIDE ROOT PORT
    NbSmnRW (DIE_NUM_FOR_VGA_CONTROLLER, Addr, ~(BIT0 | BIT2 | BIT18), (BIT0 | BIT2 | BIT18));
    DEBUG((-1, "HIDE ROOT PORT with Address %x\n", Addr));
    if (Event != NULL) {
      gBS->CloseEvent (Event);
    }
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   OemGraphicSwitch_Init
//
// Description: This function is the entry point for this PEI. This function
//              initializes the chipset Project
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
// Notes:       This function could initialize Project for anything.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI OemGraphicSwitch_Init (
        IN EFI_HANDLE       ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS                      Status;
    SETUP_DATA                      SetupData;
    UINT32                          VarAttr;
    UINTN                           VariableSize = sizeof(SETUP_DATA);
    EFI_GUID                        gEfiSetupGuid = SETUP_GUID;
    UINT32                          FrameBuffer = 0;
    CHAR16                          TextString[60];
    #if ONBOARD_VIDEO_DISABLE_BY_DXIO == 0
    EFI_EVENT                       Event = NULL;
    VOID                            *Registration;
    #endif

    Status = gRT->GetVariable( L"Setup", \
                                   &gEfiSetupGuid, \
                                   &VarAttr, \
                                   &VariableSize, \
                                   &SetupData );
    if (EFI_ERROR(Status)) {
        return Status;
    }
    if (SetupData.OnExtVgaSelect == 1) { //Onboard and External, Onboard first
        SetupData.OnBoardVideoEn = 1;
        gRT->SetVariable(L"Setup", &gEfiSetupGuid, VarAttr, sizeof(SETUP_DATA), &SetupData);
        return EFI_SUCCESS;
    }
    
    OnExtVgaSelect = SetupData.OnExtVgaSelect;
    VgaSlotNum = SetupData.VgaSlotNum;
    
    GetExactBusNumber();//zanggy-202101119-Improve PCI_SLOT_BRIDGE_LIST define format>
    
    LoopGraphicDevice();

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]OnExtVgaSelect:%x PcdGraphicSwitch:%x OnBoardVideoEn:%x\r\n",__FUNCTION__,__LINE__,\
                        SetupData.OnExtVgaSelect,PcdGetBool(PcdGraphicSwitch),SetupData.OnBoardVideoEn));
#if ONBOARD_VIDEO_DISABLE_BY_DXIO == 0
    if (SetupData.OnExtVgaSelect == 0) { //Onboard or External, External first
        if (PcdGetBool(PcdGraphicSwitch) != 0)
        {
            FrameBuffer = PcdGet32(PcdFastVideoMmioBaseAddress);
            if (FrameBuffer == 0)
              return Status;
            //show string
            Swprintf(TextString, L"VGA is redirected to offboard VGA.");
            ShowString(FrameBuffer,TextString);
    
            Event = EfiCreateProtocolNotifyEvent (
                                &gEfiPciEnumerationCompleteProtocolGuid,
                                TPL_NOTIFY,
                                DisableOnBoardVideo,
                                NULL,
                                &Registration
                                );
            ASSERT (Event != NULL);
        }
    }
    if (SetupData.OnExtVgaSelect == 2) { //Onboard and External, External first
        if (PcdGetBool(PcdGraphicSwitch) != 0) // GFX installed
        {
            Swprintf(TextString, L"VGA is redirected to offboard VGA during POST.");
            ShowString(FrameBuffer,TextString);
        }
    }
#else
    if (SetupData.OnExtVgaSelect == 0) { //Onboard or External, External first
        if (PcdGetBool(PcdGraphicSwitch) != 0) // GFX installed
        {
            if (SetupData.OnBoardVideoEn == 1)
            {
                SetupData.OnBoardVideoEn = 0;
                gRT->SetVariable(L"Setup", &gEfiSetupGuid, VarAttr, sizeof(SETUP_DATA), &SetupData);
                gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
            }
        } else // no GFX installed
        {
            if (SetupData.OnBoardVideoEn == 0) //OnBoard video not enabled
            {
                SetupData.OnBoardVideoEn = 1;
                gRT->SetVariable(L"Setup", &gEfiSetupGuid, VarAttr, sizeof(SETUP_DATA), &SetupData);
                gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
            }
        }
    }
    if (SetupData.OnExtVgaSelect == 2) { //Onboard and External, External first
        if (PcdGetBool(PcdGraphicSwitch) != 0) // GFX installed
        {
            FrameBuffer = PcdGet32(PcdFastVideoMmioBaseAddress);
            if (FrameBuffer == 0)
              return Status;
            Swprintf(TextString, L"VGA is redirected to offboard VGA during POST.");
            ShowString(FrameBuffer,TextString);
        }
    }
#endif
    return EFI_SUCCESS;
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
