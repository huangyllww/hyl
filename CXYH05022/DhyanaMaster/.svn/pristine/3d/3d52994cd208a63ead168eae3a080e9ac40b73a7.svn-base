//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.         **
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
/** @file Sb.h
    This file contains the definions for Sourthbridge

**/
//**********************************************************************

#ifndef  _SB_H   //To Avoid this header get compiled twice
#define  _SB_H

#include <Token.h>

#if SB_DEVICE_REGS_RC_DEFINITION_SUPPORT
#include SB_DEVICE_REGS_RC_DEFINITION_HEADER
#endif
#if defined WDT_SUPPORT && WDT_SUPPORT == 1
#include <Acpi30.h>
#endif

#ifdef USB1_EHCI_BUS_DEV_FUN 
#undef USB1_EHCI_BUS_DEV_FUN
#endif

#ifdef USB2_EHCI_BUS_DEV_FUN 
#undef USB2_EHCI_BUS_DEV_FUN
#endif

#ifdef USB3_EHCI_BUS_DEV_FUN 
#undef USB3_EHCI_BUS_DEV_FUN
#endif
#include <AgesaModulePkg\Fch\Taishan\FchTaishanCore\Fch.h>


#include <AmiHobs.h>

#define SB_PCI_CFG_ADDRESS(bus,dev,func,reg)    \
    (UINT64) ((((UINT8)bus << 24)+((UINT8)dev << 16)+((UINT8)func << 8)+((UINT8)(reg))) & 0xffffffff)

#define SB_PCIE_CFG_ADDRESS(bus,dev,func,reg) \
    ((UINTN)(PcdGet64 (PcdPciExpressBaseAddress) + (bus << 20) + (dev << 15) + (func << 12) + reg))

//## <AAV> The reason is this macro was removed from AmiCspLib.h in AptioV some code which from Aptio4 needs this definition.
//##		find the proper solution later.
#define CSP_PCI_CFG_ADDRESS(bus,dev,func,reg)   \
 ((UINT64) ( (((UINTN)bus) <<   24) +   (((UINTN)dev)   << 16) + (((UINTN)func) << 8)   +   ((UINTN)reg) ))& 0x00000000ffffffff


//
// 8259 Hardware definitions
//
#define LEGACY_MODE_BASE_VECTOR_MASTER                    0x08
#define LEGACY_MODE_BASE_VECTOR_SLAVE                     0x10
#define LEGACY_8259_CONTROL_REGISTER_MASTER               0x20
#define LEGACY_8259_MASK_REGISTER_MASTER                  0x21
#define LEGACY_8259_CONTROL_REGISTER_SLAVE                0xA0
#define LEGACY_8259_MASK_REGISTER_SLAVE                   0xA1
#define LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_MASTER  0x4D0
#define LEGACY_8259_EDGE_LEVEL_TRIGGERED_REGISTER_SLAVE   0x4D1
#define LEGACY_8259_EOI                                   0x20

#define RTC_IO_INDEX                0x70    // RTC I/O Index Port
#define RTC_IO_DATA                 0x71    // RTC I/O Data Port
#define PIRQ_IO_INDEX               0xC00   // PCI Interrupt I/O Index Port
#define PIRQ_IO_DATA                0xC01   // PCI Interrupt I/O Data Port
#define BIOSRAM_INDEX_PORT          0xCD4
#define BIOSRAM_DATA_PORT           0xCD5
#define PM_IO_INDEX                 0xCD6
#define PM_IO_DATA                  0xCD7
#define NUM_BITS_IN_ACPI_TIMER      32      // Programmed to 24 not 32
// This is the maximum possible bits in the timer.  Currently this is 32 according to the spec
#define MAX_ACPI_TIMER_BITS         32

// Cpu I/O space defines
#define TIMER_CTRL              0x43
#define TIMER_0_COUNT           0x40
#define TIMER_1_COUNT           0x41

#define AMI_CSP_RESET_GUID \
 {0x308DD02C, 0x092B, 0x4123, 0xA2, 0xAF, 0x3E, 0xF4, 0x44, 0x0A, 0x6B, 0x4A}

#define RESET_PORT              0x0CF9
#define CLEAR_RESET_BITS        0x0F1
#define SB_COLD_RESET              0x02    // Set bit 1 for cold reset
#define SB_RST_CPU                 0x04    // Setting this bit triggers a reset of the CPU
#define SB_FULL_RESET              0x08    // Place system in S5 state for 3 to 5 seconds

#if defined WDT_SUPPORT && WDT_SUPPORT == 1
// WDRT Watchdog Resource Table
#define WDRT_SIG 0x54524457  //"WDRT" Watchdog Resource Table
#endif

typedef enum _SB_RESET_TYPE
{
    HardReset = 0,
    SoftReset,
    ShutDown,
    FullReset = 0x80,
} SB_RESET_TYPE;


//----------------------------------------------------------------------;
//   Hygon Taishan ACPI Power Management I/O Registers                      ;
//----------------------------------------------------------------------;
#define ACPI_IOREG_PM1_STS      0x00    // Power Management 1 Status Register
#define ACPI_IOREG_PM1_EN       0x02    // Power Management 1 Enables Register
#define ACPI_IOREG_PM1_CNTL     0x04    // Power Management 1 Control Register
#define ACPI_IOREG_PM1_TMR      0x08    // Power Management 1 Timer Register
#define ACPI_IOREG_PROC_CNTL    0x10    // Processor Control register
#define ACPI_IOREG_PLVL2        0x14    // Processor Level 2 Register
#define ACPI_IOREG_PLVL3        0x15    // Processor Level 3 Register
#define ACPI_IOREG_PLVL4        0x16    // Processor Level 4 Register
#define ACPI_IOREG_CPUPS        0x17    // Processor Performance Setting Register
#define ACPI_IOREG_PM2_CNTL     0xFF    // Power Management 2 Control Register
#define ACPI_IOREG_GPE0_STS     0x20    // General Purpose Event 0 Status Register
#define ACPI_IOREG_GPE0_EN      0x24    // General Purpose Event 0 Enable Register

//----------------------------------------------------------------------;
//   Hygon Taishan Power Management I/O Registers                           ;
//----------------------------------------------------------------------;
#define PM_IOREG_SMIWAKEN3      0x04    // SMI WakeUp Event Enable 3 Register
#define PM_IOREG_SMIWAKSTS3     0x07    // SMI WakeUp Event Status 3 Register
#define PM_IOREG_ACPICNTL       0x0E    // ACPI Control Register
#define PM_IOREG_ACPISTS        0x0F    // ACPI Status Register
#define PM_IOREG_ACPIEN         0x10    // ACPI Enable Register
#define PM_IOREG_SWPCIRST       0x55    // Software PCI Reset Register
#define PM_IOREG_MISCEN1        0x66    // Misc. Enable #1 Register
#define PM_IOREG_MISCEN2        0x67    // Misc. Enable #2 Register
#define PM_IOREG_MISCEN3        0x68    // Misc. Enable #3 Register
#define PM_IOREG_CF9RST         0x85    // CF9 Reset Register

//----------------------------------------------------------------------;
#define SB_REG_VID          0x00    // Vendor ID Register
#define SB_REG_DID          0x02    // Device ID Register
#define SB_REG_PCICMD       0x04    // PCI Command Register
#define SB_REG_PCISTS       0x06    // PCI Status Register
#define SB_REG_RID          0x08    // Revision ID Register
#define SB_REG_SCC          0x0A    // Sub Class Code Register
#define SB_REG_BCC          0x0B    // Base Class Code Register
#define SB_REG_MLT          0x0D    // Primary Master Latnecy Timer
#define SB_REG_HDR          0x0E    // Header Type Register
#define SB_REG_SVID         0x2C    // Sub-System Vendor ID Register

//----------------------------------------------------------------------;
//  Hygon Taishan PATA PCI Register Equates (D14h:F1)                       ;
//----------------------------------------------------------------------;
#define IDE_REG_PIOTIM      0x40    // PIO Timing Register
#define IDE_REG_MDMATMD     0x44    // Multi-Words DMA Timing Modes Register
#define IDE_REG_PIOCNTL     0x48    // PIO Control Register
#define IDE_REG_PIOMODE     0x4A    // PIO Mode Register
#define IDE_REG_UDMACNTL    0x54    // Ultra DMA Control Register
#define IDE_REG_UDMAMODE    0x56    // Ultra DMA Mode Register

//----------------------------------------------------------------------;
// Hygon Taishan PCI Bus/Device/Function/Register Number Macros             ;
//----------------------------------------------------------------------;
#define SB_REG(Reg)         SB_PCI_CFG_ADDRESS(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,Reg)
#define SMBUS_REG(Reg)      SB_PCI_CFG_ADDRESS(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,Reg)
#define IDE_REG(Reg)        SB_PCI_CFG_ADDRESS(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,Reg)
#define SATA_REG(Reg)       SB_PCI_CFG_ADDRESS(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,Reg)

#define USB1_EHCI_REG(Reg)   SB_PCI_CFG_ADDRESS(FCH_EHCI1_BUS,FCH_EHCI1_DEV,FCH_EHCI1_FUNC,Reg)
#define USB2_EHCI_REG(Reg)   SB_PCI_CFG_ADDRESS(FCH_EHCI2_BUS,FCH_EHCI2_DEV,FCH_EHCI2_FUNC,Reg)
#define USB3_EHCI_REG(Reg)   SB_PCI_CFG_ADDRESS(FCH_EHCI3_BUS,FCH_EHCI3_DEV,FCH_EHCI3_FUNC,Reg)

#define USB_XHCI0_REG(Reg)   SB_PCI_CFG_ADDRESS(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,Reg)
#define USB_XHCI1_REG(Reg)   SB_PCI_CFG_ADDRESS(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,Reg)
#define PCIBR_REG(Reg)      SB_PCI_CFG_ADDRESS(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,Reg)

#define SD_REG(Reg)   SB_PCI_CFG_ADDRESS(SD_PCI_BUS,SD_PCI_DEV,SD_PCI_FUNC,Reg)


#define MAX_AHCI_CHANNEL            8


#define RAID_FC_DEVID       0x4394  // SATA + FC enable mode Device ID
#define USB20_REG_LEGEXTCAP 0xA0    // USB Legacy Support Extended Capability Register
#define USB20_REG_LEGCTLSTS 0xA4    // USB Legacy Support Control/Status Register


//----------------------------------------------------------------------;
// AMD FCH PCI Bus/Device/Function Number Macros                      	;
//----------------------------------------------------------------------;
#define LPC_BUS FCH_LPC_BUS
#define LPC_DEVICE FCH_LPC_DEV
#define LPC_FUNC FCH_LPC_FUNC

#if defined WDT_SUPPORT && WDT_SUPPORT == 1
// WDRT Watchdog Resource Table
#pragma pack (push, 1)
//======================================================
//  WDRT Watchdog Resource Table
//======================================================
typedef struct {
	ACPI_HDR 	Header; 
	GAS_30		ControlRegAddr;
	GAS_30		CountRegAddr;
	UINT16      PciDeviceID;            
	UINT16      PciVendorID;        
	UINT8       PciBusNumber;       
	UINT8       PciDeviceNumber;      
	UINT8       PciFunctionNumber;
	UINT8       PciSegment;
	UINT16		MaxCount;
	UINT8		Units;
}WDRT;
#pragma pack (pop)
#endif

#define SB_ACPI_PM1_STS               ACPI_IOREG_PM1_STS
#define SB_ACPI_PM1_EN                ACPI_IOREG_PM1_EN
#define SB_ACPI_PM1_CNTL              ACPI_IOREG_PM1_CNTL
#define SB_ACPI_GPE0_STS              ACPI_IOREG_GPE0_STS
#define SB_ACPI_GPE0_EN               ACPI_IOREG_GPE0_EN
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
