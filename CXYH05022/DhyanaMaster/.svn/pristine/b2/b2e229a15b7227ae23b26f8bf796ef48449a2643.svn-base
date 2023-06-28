//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file PeiBmcBoard.c
    This file contains code for Initialize LPC BMC base
    address and SIO KCS in the PEI stage.

**/

#include "Token.h"
#include "IpmiBmc.h"
#include <PiPei.h>
#include <Library/PciLib.h>

// LPC Bridge Bus Device Function Definitions
#define LPC_BUS                 0
#define LPC_DEVICE              31
#define LPC_FUNC                0
#define REG_GEN2_DEC            0x88
#define LPC_DECODE_ENABLE       0x01
#define KCS_BASE_ADDRESS_MASK   0xFFF0
#define ENABLE_16_BYTE          0x000C0000
//<Wangyia001-20170331 BMC Ready Function +> 
#define ACPI_MMIO_BASE          0xFED80000ul
#define GPIO_BANK0_BASE         0x1500
//<Wangyia001-20170331 BMC Ready Function -> 

//#define REG_LPC_ENABLE          0x82
//#define REG_LPC_PM_BASE         0x40
//#define CNF2_LPC_EN             0x2000
//#define SIO_INDEX_PORT          0x4E
//#define SIO_DATA_PORT           0x4F
//#define LDN_REGISTER            0x07
//#define KCS4_LDN                0x09
//#define ACTIVATE_REGISTER       0x30

//#define BIT_MC_LPC_EN       0x400

/**
    Initializes Lpc to decode BMC IO ranges. This is platform specific Hook
    need to initialize chipset to decode and enable BMC IO ranges.

    @param PeiServices Pointer to the PEI Core data Structure

    @return EFI_STATUS
    @retval EFI_SUCCESS returned

**/
EFI_STATUS
InitLpcBmc (
  IN  CONST EFI_PEI_SERVICES          **PeiServices )
{
    //Commenting the SIO initialization code as PILOT3 module is available
    /*{

        #define SIO_CONFIG_INDEX    0x4e
        #define SIO_CONFIG_DATA        0x4f
        #define PILOT3_LD_SEL_REGISTER    0x7
        #define PILOT3_ACTIVATE_REGISTER    0x30
        #define PILOT3_BASE1_HI_REGISTER    0x60
        #define PILOT3_BASE1_LO_REGISTER    0x61
        #define PILOT3_BASE2_HI_REGISTER    0x62
        #define PILOT3_BASE2_LO_REGISTER    0x63

        #define PILOT3_LDN_KCS4    0x9
        #define SIO_KCS4_DATA_BASE_ADDRESS    0xca2
        #define SIO_KCS4_CMD_BASE_ADDRESS    0xca3
        #define PILOT3_ACTIVATE_VALUE    0x1

        IoWrite8 (SIO_CONFIG_INDEX, 0x5A); //UnLock SIO space

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_LD_SEL_REGISTER); //Point to logical device selection register
        IoWrite8 (SIO_CONFIG_DATA, PILOT3_LDN_KCS4); //Set logical device KCS4

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE1_LO_REGISTER); //Point to Base Address LSB register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_DATA_BASE_ADDRESS & 0x00FF)); //Write Base Address LSB register

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE1_HI_REGISTER); //Point to Base Address MSB register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_DATA_BASE_ADDRESS >> 8)); //Write Base Address MSB register

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE2_LO_REGISTER); //Point to Activate Register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_CMD_BASE_ADDRESS & 0x00FF)); //Enable serial port with Activation bit

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_BASE2_HI_REGISTER); //Point to Base Address LSB register
        IoWrite8 (SIO_CONFIG_DATA, (UINT8)((UINT16)SIO_KCS4_CMD_BASE_ADDRESS >> 8)); //Write Base Address LSB register

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_ACTIVATE_REGISTER); //Point to Activate Register
        IoWrite8 (SIO_CONFIG_DATA, PILOT3_ACTIVATE_VALUE); //enable serial port with Activation bit

        IoWrite8 (SIO_CONFIG_INDEX, PILOT3_LD_SEL_REGISTER); //Point to logical device selection register
        IoWrite8 (SIO_CONFIG_DATA, PILOT3_LDN_KCS4); //Set logical device KCS4

        IoWrite8 (SIO_CONFIG_INDEX, 0xA5); //Lock SIO space
    }*/


//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
  //
  //Enable decoding Lpc BMC base address region.
  //

//###PORTING
//    *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
//            (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, REG_GEN2_DEC)) = \
//            ((IPMI_KCS_BASE_ADDRESS & KCS_BASE_ADDRESS_MASK) | LPC_DECODE_ENABLE) | ENABLE_16_BYTE;       // Enable 16 Byte
{
  UINT32  Val32;

// Used 3rd WideIo region in FCH, commented as PCIe base not programmed at this early moment
// and switch to IO PCI config space access(CF8/CFC)
/*
#define LPC_DEVICE_ZP 0x14
#define LPC_FUNC_ZP   0x03

  Val32 = *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
        (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE_ZP, LPC_FUNC_ZP, 0x90));
  *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
        (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE_ZP, LPC_FUNC_ZP, 0x90)) = (Val32 & 0xFFFF0000) | (IPMI_KCS_BASE_ADDRESS & 0xFFF0);    // align 16 bytes
  // Set 16 bytes wide region
  Val32 = *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
        (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE_ZP, LPC_FUNC_ZP, 0x74));
  *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
        (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE_ZP, LPC_FUNC_ZP, 0x74)) = Val32 | 0x8; // BIT3
  // Enable Region
  Val32 = *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
        (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE_ZP, LPC_FUNC_ZP, 0x48));
  *(volatile UINT32*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
        (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, 0x48)) = Val32 | 0x02000000; // BIT25
*/

  IoWrite32(0xCF8, 0x8000A390);
  Val32 = IoRead32(0xCFC);
  Val32 = (Val32 & 0xFFFF0000) | (IPMI_KCS_BASE_ADDRESS & 0xFFF0);
  IoWrite32(0xCFC, Val32);
  
  IoWrite32(0xCF8, 0x8000A374);
  Val32 = IoRead32(0xCFC);
  IoWrite32(0xCFC, (Val32 | 0x8));
  
  IoWrite32(0xCF8, 0x8000A348);
  Val32 = IoRead32(0xCFC);
  IoWrite32(0xCFC, (Val32 | 0x02000000));
 }       
        
#if IPMI_SEPARATE_DXE_SMM_INTERFACES == 1
/*
  //
  //Enable decoding LPC BMC base address region - KCS3.
  //Enable decoding LPC BMC base address region - KCS3.
  //
  *(UINT16 *)PCIE_CFG_ADDR (LPC_BUS, LPC_DEVICE, LPC_FUNC, REG_LPC_ENABLE) |= \
     BIT_MC_LPC_EN;
*/
#endif

/*
  *(UINT32 *)PCIE_CFG_ADDR (LPC_BUS, LPC_DEVICE, LPC_FUNC, REG_GEN2_DEC) = \
     ((BMC_ADDRESS_DECODE_MASK<<16) | (IPMI_KCS_BASE_ADDRESS & 0xFFFC) | LPC_DECODE_ENABLE);

  //
  //There are five KCS interfaces implemented on the Pilot. The KCS1 and KCS2 interfaces are 
  // disabled and will not be supported by the Pilot firmware. The remaining three KCS (
  // KCS3, 4, & 5) interfaces can be used for the communication between the host and the BMC.
  
  //
  //The BMC and the host processor communicate through KCS3, KCS4, KCS5, BT, SMIC or Mailboxes. 
  //Enable KCS4 in Pilot Sio to communicate with BMC.
  //By default base address is 0xCA2 and 0xCA3. just we need to enable in Activate Reg.
  //

  //
  // Enable decoding of SIO index ports 0x4E, 0x4F
  //

  *(UINT16 *)PCIE_CFG_ADDR (LPC_BUS, LPC_DEVICE, LPC_FUNC, REG_LPC_ENABLE) = \
     ((*(UINT16 *)PCIE_CFG_ADDR (LPC_BUS, LPC_DEVICE, LPC_FUNC, REG_LPC_ENABLE)) | CNF2_LPC_EN);
  
  //UnLock SIO space
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, 0x5A);

  //Enable KCS4 for Host to BMC interface.
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, LDN_REGISTER);
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_DATA_PORT, KCS4_LDN);
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, ACTIVATE_REGISTER);
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_DATA_PORT, 0x01);
  
  //Lock SIO space
  CpuIo->IoWrite8 (PeiServices, CpuIo, SIO_INDEX_PORT, 0xA5);
*/
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
//PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING////PORTING//
  return EFI_SUCCESS;
}

#if BMC_INIT_DELAY
/**
    Checks the Power Failure PWR_FLR bit in Power Management Control register and Proper delay 
    is provided to get response from BMC when PWR_FLR bit is set and Wait for BMC Setup option 
    is enabled.

    @param PeiServices Pointer to the PEI Core data Structure

    @return BOOLEAN
    @retval TRUE  - If PWR_FLR bit is set.
    @retval FALSE - If PWR_FLR bit is not set.

**/
BOOLEAN
BmcInitDelay (
  IN CONST EFI_PEI_SERVICES          **PeiServices )
{
    //<Wangyia001-20170331 BMC Ready Function +>
    /*SERVER_IPMI_DEBUG ((EFI_D_LOAD, "Check for Power Failure PWR_FLR bit: %x \n",\
      ( *(volatile UINT8*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
      (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, LPC_GEN_PMCON_3)) & PWR_FLR_BIT )));    

    //
    // Check Power Failure PWR_FLR bit for AC power loss.
    //
    if ( *(volatile UINT8*) ((UINTN) (PcdGet64(PcdPciExpressBaseAddress)) + \
            (UINTN) PCI_LIB_ADDRESS(LPC_BUS, LPC_DEVICE, LPC_FUNC, LPC_GEN_PMCON_3)) & PWR_FLR_BIT ) {
            SERVER_IPMI_DEBUG ((EFI_D_LOAD, "Power Failure PWR_FLR bit for AC power loss. \n"));
            return TRUE;
    }
    */
    
    UINT8 BmcReady = 0;
    BmcReady = MmioRead8 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + (BMC_READY_PIN << 2) + 2) & 0x01;
	SERVER_IPMI_DEBUG ((EFI_D_LOAD, "BmcReadyPin=0x%x \n",BmcReady));
    if (1 == BmcReady) {
      return TRUE;
    } 
   
    return FALSE;
    //<Wangyia001-20170331 BMC Ready Function -> 
}
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
