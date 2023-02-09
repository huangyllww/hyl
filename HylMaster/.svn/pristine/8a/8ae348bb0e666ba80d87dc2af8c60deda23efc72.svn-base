#include <Efi.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <token.h>
#include <Library/FastVideo.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Ppi/AmiPeiPciEnumeration.h>
//<Kangmm-20210121 Code improve for Fast Video +>
#include <Library/BaseGetRbBusLib.h>

//Mizl-20210930- Add Patch For Hygon Dynamic Assign MMIO Resource Function+>
#include <Ppi/ReadOnlyVariable2.h>
#include <Setup.h>
typedef struct _MMIO_ADD_CTL
{
  /* data */
  union 
  {
    struct{
      UINT32  RE:1;
      UINT32  WE:1;
      UINT32  CpuDis:1;
      UINT32  Reserved_3:1;
      UINT32  DstFabricID:8;
      UINT32  NP:1;
      UINT32  Reserved_13_31:19;
    }Field;
    UINT32 AllBits;
    };
}MMIO_ADD_CTL;


typedef struct _CFG_ADD_MAP
{
  /* data */
  union 
  {
    struct{
      UINT32  RE:1;
      UINT32  WE:1;
      UINT32  Reserved_2_3:2;
      UINT32  DstFabricID:8;
      UINT32  Reserved_16_23:4;
      UINT32  BusNumBase:8;
      UINT32  BusNumLimit:8;
    }Field;
    UINT32 AllBits;
    };
}CFG_ADD_MAP;


typedef struct _IO_SPACE_BASE_ADD
{
  /* data */
  union 
  {
    struct{
      UINT32  RE:1;
      UINT32  WE:1;
      UINT32  Reserved_2_3:2;
      UINT32  VE:1;
      UINT32  IE:1;
      UINT32  Reserved_6_11:6;
      UINT32  IOBase:13;
      UINT32  Reserved_25_31:7;
    }Field;
    UINT32 AllBits;
    };
}IO_SPACE_BASE_ADD;


typedef struct _IO_SPACE_LIMT_ADD
{
  /* data */
  union 
  {
    struct{
      UINT32  DstFabricID:8;
      UINT32  Reserved_8_11:4;
      UINT32  IOLimit:13;
      UINT32  Reserved_25_31:7;
    }Field;
    UINT32 AllBits;
    };
}IO_SPACE_LIMT_ADD;
//Mizl-20210930- Add Patch For Hygon Dynamic Assign MMIO Resource Function->

#if (defined(FABRIC_RESOURCE_SUPPORT) && (FABRIC_RESOURCE_SUPPORT == 1))

#include <Ppi/FabricResourceManagerServicesPpi.h>

#endif
//<Kangmm-20210121 Code improve for Fast Video ->
#define AMD_VENDOR_ID                  0x1022
#define HIGON_VENDOR_ID                0x1D94
#define FCH_CFG_REG00                  0x000

//<Kangmm-20210121 Code improve for Fast Video +>
#define FABRIC_BUS_NUM_FOR_VGA 0
#define FABRIC_DEV_NUM_FOR_VGA 0x18
#define FABRIC_FUNC_NUM_FOR_VGA 0
//<Kangmm-20210121 Code improve for Fast Video ->


#define PCI_LIB_ADDRESS(Bus,Device,Function,Register)   \
  (((Register) & 0xfff) | (((Function) & 0x07) << 12) | (((Device) & 0x1f) << 15) | (((Bus) & 0xff) << 20))


typedef struct _VGA_ENABLE_REGISTER
{
    union {
        struct {
            UINT32  VgaEnable   :1;  // [0] VE: VGA enable
            UINT32  NonPosted   :1;  // [1] NP: non-posted
            UINT32  CpuDis      :1;  // [2] CpuDis: CPU Disable
            UINT32  Reserved    :1;  // [3] Reserved
            UINT32  DstFabricID :8;  // [11:4] Destination Fabric ID
            UINT32  Reserved2   :20;  // [31:11] Reserved
        } Field;
        UINT32  AllBits;
    };
} VGA_ENABLE_REGISTER;

typedef struct _X86_IO_BASE_ADDRESS
{
    union {
        struct {
            UINT32  ReadEnable   :1;  // [0] VE: VGA enable
            UINT32  WriteEnable   :1;  // [1] NP: non-posted
            UINT32  Reserved1    :2;  // [3:2] Reserved
            UINT32  VgaEnable      :1;  // [4] CpuDis: CPU Disable
            UINT32  IsaEnable    :1;  // [5] Reserved
            UINT32  Reserved2    :6;  // [11:6] Reserved
            UINT32  IoBase :13;  // [24:12] Destination Fabric ID
            UINT32  Reserved3   :7;  // [31:25] Reserved
        } Field;
        UINT32  AllBits;
    };
} X86_IO_BASE_ADDRESS;

typedef struct _VGA_MMIO_DECODE_CTRL
{
    union {
        struct {
            UINT32  ReadEnable   :1;  // [0] RE: Read Enable
            UINT32  WriteEnable  :1;  // [1] WE: Write Enable
            UINT32  CpuDis       :1;  // [2] Reserved
            UINT32  Reserved1    :1;  // [3] Reversed
            UINT32  DstFabricID  :8;  // [4:11] Reserved
            UINT32  Np    		 :1;  // [12] NP
            UINT32  Reserved2    :19; // [13:31] Reserved
        } Field;
        UINT32  AllBits;
    };
} VGA_MMIO_DECODE_CTRL;

typedef struct _X86_IO_LIMIT_ADDRESS
{
    union {
        struct {
            UINT32  DstFabricID   :8;  // [0] VE: VGA enable
            UINT32  Reserved   :4;  // [11:8] NP: non-posted
            UINT32  IOLimit      :13;  // [24:12] CpuDis: CPU Disable
            UINT32  Reserved2   :7;  // [31:25] Reserved
        } Field;
        UINT32  AllBits;
    };
} X86_IO_LIMIT_ADDRESS;

#if(FAST_VIDEO_ENABLE==1)
  extern 	INT32  InitVGA (UINT32 OemData, UINT8 *argv);
#endif


//Mizl-20210930- Add Patch For Hygon Dynamic Assign MMIO Resource Function+>
/**
GC_TODO: AdjustMMIOResource
@param         - PeiServices: PEI Phase Service Pointer
@param         - Base: PCIe MMIO Base Address
@param         - Limit: PCIe MMIO Limit Address

@retval EFI_SUCCESS   - Assgin MMIO Base Success
**/
EFI_STATUS
AdjustMMIOResource
(
  IN CONST  EFI_PEI_SERVICES     **PeiServices,
  IN        UINT64              Base,
  IN        UINT64              Limit
)
{
  EFI_STATUS                        Status=EFI_SUCCESS;

  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *ReadOnlyVariable;
  UINT64                            AspeedVgaBufferSize=0x1000000; //Set 16MB In Default
  SETUP_DATA                        mSetupData;
  static EFI_GUID                   gSetupGuid = SETUP_GUID;
  UINTN                             Size = sizeof(SETUP_DATA);

  Status = (**PeiServices).LocatePpi (PeiServices,&gEfiPeiReadOnlyVariable2PpiGuid,0,NULL,&ReadOnlyVariable);
  if (!EFI_ERROR(Status)) 
  {
    Status = ReadOnlyVariable->GetVariable( ReadOnlyVariable,
              L"Setup",
              &gSetupGuid,
              NULL,
              &Size,
              &mSetupData );
    if(!EFI_ERROR(Status))
    {
      switch(mSetupData.BmcVgaMemSize)
      {
        case 3:
          AspeedVgaBufferSize = 0x4000000;//64MB
          break;
        case 2:
          AspeedVgaBufferSize = 0x2000000;//32MB
          break;
        default:
          DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Use 16MB In Default\r\n",__FUNCTION__,__LINE__));//16MB and 8MB
          break;
      }
    }
  }

  AspeedVgaBufferSize = AspeedVgaBufferSize +0x1000000;//Reserved 1MB for Address Align

  if((Limit - Base) < AspeedVgaBufferSize)
  {
    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Can Not Get Enough MMIO Resource For AST2500!!!\r\n",__FUNCTION__,__LINE__));
    return  EFI_BUFFER_TOO_SMALL;
  }
  /*
    MMIOLimit < PcdGet64(PcdPciExpressBaseAddress) ==> AssignMmioTopDown Flag Will Be Set,We Should Pass MMIOLimit Address
    MMIOBase > PcdGet64(PcdPciExpressBaseAddress) ==> AssignMmioTopDown Flag Will Not Be Set,We Should Pass MMIOBase Address
    MMIOBase < PcdGet64(PcdPciExpressBaseAddress) && MMIOLimit > PcdGet64(PcdPciExpressBaseAddress)
  */
  if(Limit <= PcdGet64(PcdPciExpressBaseAddress))
  {
    PcdSet64(AmiPcdPeiPciMmioTopAddress,Limit);
    return  EFI_SUCCESS;
  }

  if(Base > PcdGet64(PcdPciExpressBaseAddress))
  {
    PcdSet64(AmiPcdPeiPciMmioTopAddress,Base);
    return EFI_SUCCESS;
  }      

  if(Base < PcdGet64(PcdPciExpressBaseAddress))
    if(Base + AspeedVgaBufferSize <= PcdGet64(PcdPciExpressBaseAddress))
    {
      PcdSet64(AmiPcdPeiPciMmioTopAddress,Base + AspeedVgaBufferSize);
      return EFI_SUCCESS;
    }
    else if((Limit - AspeedVgaBufferSize) > PcdGet64(PcdPciExpressBaseAddress))
    {
      PcdSet64(AmiPcdPeiPciMmioTopAddress,Limit);
      return EFI_SUCCESS;
    }else
    {
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Can Not Get Enough MMIO Resource For AST2500!!!\r\n",__FUNCTION__,__LINE__));
      return  EFI_BUFFER_TOO_SMALL;
    }
  
  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Should Not Be Here!!!!\r\n",__FUNCTION__,__LINE__));

  return EFI_UNSUPPORTED;
}
//Mizl-20210930- Add Patch For Hygon Dynamic Assign MMIO Resource Function->
/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the Oem Fast Video Pei driver
 *
 * This function Used To Initialize AST2500 VGA Controller During PEI Phase
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*----------------------------------------------------------------------------------------*/
//<Kangmm-20210121 Code improve for Fast Video +>
EFI_STATUS
EFIAPI
FastVideoPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{

  EFI_STATUS						Status;
  AMI_PEI_PCI_ENUMERATION_PPI       *PeiPciEnumerationPpi;
  UINTN       						PciAddress;
  UINT16      						RegValue;
  UINT16      						VendorID;
  VGA_ENABLE_REGISTER          	    VgaEnableReg;
  X86_IO_BASE_ADDRESS               IoBaseReg;
  X86_IO_LIMIT_ADDRESS              IoLimitReg;
  VGA_MMIO_DECODE_CTRL              VgaMMIOCtrl;
  UINT8 							Data_Fabric_Dev;
  UINT32                            Reg32;
  EFI_BOOT_MODE             		BootMode;
//<Kangmm-20210121 Code improve for Fast Video +>
#if (defined(FABRIC_RESOURCE_SUPPORT) && (FABRIC_RESOURCE_SUPPORT == 1))

  FABRIC_RESOURCE_MANAGER_PPI       *FabricResourceManagerServicesPpi;
  FABRIC_RESOURCE_FOR_EACH_RB       *FabricResource;
  UINT64                            PciMmioStartAddress;

#endif
  UINT8                             SocketNum = SOCKET_NUM_FOR_VGA_CONTROLLER;
  UINT8                             DieNum = DIE_NUM_FOR_VGA_CONTROLLER;
  UINT8                             RbBusNum = 0;
  UINT8                             FabricId = 0;

  //Mizl-20210930- Add Patch For Hygon Dynamic Assign MMIO Resource Function+>
  UINTN                             DataFabricAddress=PCI_LIB_ADDRESS(0, 0x18, 0, 0x00);
  UINT8                             idx=0;
  MMIO_ADD_CTL                      MMIOAddCtl;
  CFG_ADD_MAP                       PCIeBusConfig;
  IO_SPACE_BASE_ADD                 IoSpaceAddress;
  IO_SPACE_LIMT_ADD                 IoLimitAddress;

  UINT32                            MMIOBase;
  UINT32                            MMIOLimit;

  BOOLEAN                           SyncedDataFabricResource=FALSE;
  //Mizl-20210930- Add Patch For Hygon Dynamic Assign MMIO Resource Function->

//<Kangmm-20210121 Code improve for Fast Video ->  
  Status = (*PeiServices)->GetBootMode (
                             PeiServices,
                             &BootMode
                             );

  if (BootMode == BOOT_IN_RECOVERY_MODE){ 

    PcdSet64(AmiPcdPeiPciMmioTopAddress,0x00);
    PcdSet8(AmiPcdPeiPciEnumerationLastBusNumber,0x00);

    return EFI_SUCCESS;
  } else {

    // Get Bus Number For VGA Die
    PciAddress = PCI_LIB_ADDRESS(FABRIC_BUS_NUM_FOR_VGA,FABRIC_DEV_NUM_FOR_VGA,FABRIC_FUNC_NUM_FOR_VGA,FCH_CFG_REG00);
    VendorID = MmioRead16 ((UINTN) PcdGet64(PcdPciExpressBaseAddress) + PciAddress);

    if (VendorID != AMD_VENDOR_ID && VendorID != HIGON_VENDOR_ID) {  
	    DEBUG((DEBUG_INFO, "[Oemdbg]: Not Found the root bridge...\n"));
	    return EFI_SUCCESS;
    }
    
    
    RbBusNum = GetCPURbBusNumAndFabricId(SocketNum,DieNum,&FabricId);
    DEBUG((DEBUG_INFO, "RbBusNum %x\n",RbBusNum));
    DEBUG((DEBUG_INFO, "FabricId %x\n",FabricId));
    //Set VGA Bus Num to PCD
    PcdSet8(PcdFastVideoBusNum,RbBusNum);
    PcdSet8(AmiPcdPeiPciEnumerationLastBusNumber,RbBusNum);
    DEBUG((DEBUG_INFO, "[Oemdbg]: RbBusNum %x\n",RbBusNum));
    
    //Set VGA Resource  
#if (defined(FABRIC_RESOURCE_SUPPORT) && (FABRIC_RESOURCE_SUPPORT == 1))
    Status = (*PeiServices)->LocatePpi(
                               PeiServices,
                               &gAmdFabricResourceManagerServicesPpiGuid,
                               0,
                               NULL,
                               &FabricResourceManagerServicesPpi
                               );
    if(!EFI_ERROR(Status)){ 
        Status = (*PeiServices)->AllocatePool (
                               PeiServices,
                               sizeof (FABRIC_RESOURCE_FOR_EACH_RB),
                               &FabricResource
                               );
    
  
        (*PeiServices)->SetMem (FabricResource, sizeof (FABRIC_RESOURCE_FOR_EACH_RB), 0);
           
        Status = FabricResourceManagerServicesPpi->FabricGetAvailableResource (FabricResource);
        

        PciMmioStartAddress = (UINT64 )FabricResource->PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Base;
        DEBUG((DEBUG_INFO, "PciMmioStartAddress %lx\n", FabricResource->PrefetchableMmioSizeBelow4G[SocketNum][DieNum].Base));
        PcdSet64(AmiPcdPeiPciMmioTopAddress,PciMmioStartAddress);
	}
#endif

  //Mizl-20210929- Enumrate DataFabric MMIO Regs +>
  if((!IsSOCSSP()) && (!IsSOCGn()))
  {

    for (idx = 0; idx < 8; idx++)
    {
      /* code */
      //Dump Memory Config
      MMIOAddCtl.AllBits=PciRead32(DataFabricAddress|0x208|(idx*0x10));
      MMIOLimit=PciRead32(DataFabricAddress|0x204|(idx*0x10));
      MMIOBase=PciRead32(DataFabricAddress|0x200|(idx*0x10));

      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Offset:%x-%x-%x,DstFabricID:%x MMIOBase:%x MMIOLimit:%x RE:%x WE:%x\r\n",__FUNCTION__,__LINE__,0x200|(idx*0x10),0x204|(idx*0x10),0x208|(idx*0x10),\
                              MMIOAddCtl.Field.DstFabricID,MMIOBase,MMIOLimit,MMIOAddCtl.Field.RE,MMIOAddCtl.Field.WE));

      //Dump PCI Bus Config
      PCIeBusConfig.AllBits=PciRead32(DataFabricAddress|0xA0|(idx*0x4));
      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Offset:%x,DstFabricID:%x BusNumBase:%x BusNumLimit:%x RE:%x WE:%x\r\n",__FUNCTION__,__LINE__,0xA0|(idx*0x4),\
                            PCIeBusConfig.Field.DstFabricID,PCIeBusConfig.Field.BusNumBase,PCIeBusConfig.Field.BusNumLimit,PCIeBusConfig.Field.RE,PCIeBusConfig.Field.WE));
        
      IoSpaceAddress.AllBits=PciRead32(DataFabricAddress|0xC0|(idx*0x8));
      IoLimitAddress.AllBits=PciRead32(DataFabricAddress|0xC4|(idx*0x8));

      DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Offset:%x-%x DstFabricID:%x IOBase:%x IOLimit:%x RE:%x WE:%x\r\n",__FUNCTION__,__LINE__,0xC0|(idx*0x8),0xC4|(idx*0x8),\
                            IoLimitAddress.Field.DstFabricID,IoSpaceAddress.Field.IOBase,IoLimitAddress.Field.IOLimit,IoSpaceAddress.Field.RE,IoSpaceAddress.Field.WE));

      if((MMIOAddCtl.Field.DstFabricID == FabricId) && (MMIOBase!= 0) && (MMIOBase < 0xFFFF))
      {
        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]DstFabricID:%x MMIOBase:%x MMIOLimit:%x\r\n",__FUNCTION__,__LINE__,MMIOAddCtl.Field.DstFabricID,MMIOBase,MMIOLimit));

        Status = AdjustMMIOResource(PeiServices,MMIOBase<<16,MMIOLimit<<16);
        if(EFI_ERROR (Status))
          return Status;

        DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]IoBaseAddress:%x\r\n",__FUNCTION__,__LINE__,DataFabricAddress|0xc0|(idx*0x08)));
        IoBaseReg.AllBits = PciRead32(DataFabricAddress|0xc0|(idx*0x08));
        IoBaseReg.Field.IoBase = 0x3c0;
        IoBaseReg.Field.ReadEnable = 1;
        IoBaseReg.Field.WriteEnable = 1;
        IoBaseReg.Field.VgaEnable = 1;
        PciWrite32(DataFabricAddress|0xc0|(idx*0x08),IoBaseReg.AllBits);
      
        // Set IO Limit Address
        IoLimitReg.AllBits = PciRead32(DataFabricAddress|0xc4|(idx*0x08));
        IoLimitReg.Field.IOLimit = 0x3df;
        IoLimitReg.Field.DstFabricID = FabricId;
        PciWrite32(DataFabricAddress|0xc4|(idx*0x08),IoLimitReg.AllBits);

        MMIOAddCtl.Field.RE = 1;
        MMIOAddCtl.Field.WE = 1;

        PciWrite32(DataFabricAddress|0x208|(idx*0x10), MMIOAddCtl.AllBits);
        
        SyncedDataFabricResource = TRUE;
        break;
      }
    }
  }
  //Mizl-20210929- Enumrate DataFabric MMIO Regs ->
	Status = (**PeiServices).LocatePpi
                              (
                              PeiServices,
                              &gAmiPeiPciEnumerationPpiGuid,
                              0,
                              NULL,
                              &PeiPciEnumerationPpi
                              );
  
    // Assign Root Bridge && VGA device Bus Number and Memory/IO Resource
    Status = PeiPciEnumerationPpi->InitDevices(PeiPciEnumerationPpi);
    //Mizl-20200403-Syatem hang after Disable vga+>
    if(EFI_ERROR(Status))
    {
      DEBUG((EFI_D_ERROR, "[OemDbg]: Init VGA Device Fail,Please Double Check It!!!\n\n"));
      return Status;
    }
    //Mizl-20200403-Syatem hang after Disable vga->


	Data_Fabric_Dev = 0x18;
  // Enable  VGA-compatible IO Address Space
  VgaEnableReg.AllBits = PciRead32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0x80));

  VgaEnableReg.Field.DstFabricID = FabricId;
  VgaEnableReg.Field.VgaEnable = TRUE;
  PciWrite32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0x80),VgaEnableReg.AllBits);
  
  //Mizl-20200403-Syatem hang after Disable vga+>
  //if((!IsSOCSSP()) && (!IsSOCGn()))
  if((!IsSOCSSP()) && (!IsSOCGn()) && (SyncedDataFabricResource == FALSE))
  //Mizl-20200403-Syatem hang after Disable vga->
  {
    // Set IO Base Address
    IoBaseReg.AllBits = PciRead32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0xC0));
    IoBaseReg.Field.IoBase = 0x3c0;
    IoBaseReg.Field.ReadEnable = 1;
    IoBaseReg.Field.WriteEnable = 1;
    IoBaseReg.Field.VgaEnable = 1;
    PciWrite32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0xC0),IoBaseReg.AllBits);

    // Set IO Limit Address
    IoLimitReg.AllBits = PciRead32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0xC4));
    IoLimitReg.Field.IOLimit = 0x3df;
    IoLimitReg.Field.DstFabricID = FabricId;
    PciWrite32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0xC4),IoLimitReg.AllBits);
    

      

    // Set VGA Bar1 MMIO Decode Base Address
    Reg32 = PciRead32(PCI_LIB_ADDRESS((RbBusNum + 2), 0, 0, 0x10));
    Reg32 = Reg32 >> 16;
    PciWrite32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0x200),Reg32);

    // Set VGA Bar1 MMIO Decode Limit Address
    Reg32 = PciRead32(PCI_LIB_ADDRESS((RbBusNum + 2), 0, 0, 0x10));
    Reg32 += 0x01FF0000;
    Reg32 = Reg32 >> 16;
    PciWrite32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0x204),Reg32);


    // Set  MMIO CTRL
    VgaMMIOCtrl.AllBits = PciRead32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0x208));
    VgaMMIOCtrl.Field.ReadEnable = 1;
    VgaMMIOCtrl.Field.WriteEnable = 1;
    VgaMMIOCtrl.Field.DstFabricID = FabricId;


    PciWrite32(PCI_LIB_ADDRESS(0, Data_Fabric_Dev, 0, 0x208), VgaMMIOCtrl.AllBits);
  }
  // Enable PCIE Root Bridge IO Decode To VGA-compatible Space
  RegValue = PciRead16(PCI_LIB_ADDRESS(RbBusNum, AST_SB_DEV_NUM, AST_SB_FUNC_NUM, 0x3e));
  RegValue |= 0x8;
  PciWrite16(PCI_LIB_ADDRESS(RbBusNum, AST_SB_DEV_NUM, AST_SB_FUNC_NUM, 0x3e),RegValue);
  RegValue = PciRead16(PCI_LIB_ADDRESS(RbBusNum, AST_SB_DEV_NUM, AST_SB_FUNC_NUM, 0x3e));

  //////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////// - Graphic Mode -  ////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////
  #if FAST_VIDEO_ENABLE == 1

      Status = InitVGA((UINT32)PeiServices, "1024x768x32");
      if (Status != EFI_SUCCESS) {
        DEBUG((DEBUG_INFO, "[OemDbg]: Fast Video Graphic Mode Fail ...\nStatus = %r\n", Status));
      }
  
  #endif
  
  //////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////// - Text Mode -  //////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////////////
  #if FAST_VIDEO_TEXT_MODE == 1

      Status = InitVGA2(NULL,NULL);
      if (Status != EFI_SUCCESS) {
        DEBUG((DEBUG_INFO, "[OemDbg]: Fast Video Text Mode Fail ...\nStatus = %r\n", Status));
      }
  
  #endif
  }
  
  return EFI_SUCCESS;
}
//<Kangmm-20210121 Code improve for Fast Video ->