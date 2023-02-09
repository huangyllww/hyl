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
/** @file SbCspLib.h
    This file contains south bridge related defintions

**/
//**********************************************************************
#ifndef __SBLIB_H__
#define __SBLIB_H__

#include <efi.h>
#include <Protocol\PciRootBridgeIo.h>
#include <Protocol\BootScriptSave.h>
#include <Protocol\PciIo.h>
#include <Protocol\AmiSio.h>
#if     CSM_SUPPORT
#include <Protocol\LegacyInterrupt.h>
#endif
#include <Ppi/ReadOnlyVariable2.h>
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

/** CODECENTRY - Southbridge HD Audio OEM Codec structure */

/** CODECTBLLIST - Southbridge HD Audio Codec table list */

///
/// _AcpiRegWrite - ACPI MMIO register R/W structure
///
typedef struct _ACPI_REG_WRITE {
  UINT8        MmioBase;               /// MmioBase: Index of Fch block (For instance GPIO_BASE:0x01 SMI_BASE:0x02)
  UINT8        MmioReg;                /// MmioReg      : Register index
  UINT8        DataAndMask;            /// DataANDMask  : AND Register Data
  UINT8        DataOrMask;             /// DataOrMask   : Or Register Data
} ACPI_REG_WRITE;


typedef enum _SB_WAKEUP_TYPE {
	// first section 1~8 are compatible with "getWakeupTypeForSmbios" routine for SMBIOS.
	SbWakeupTypeOther = 1,
	SbWakeupTypeUnknown = 2,
	SbWakeupTypeApmTimer = 3,
	SbWakeupTypeModemRing = 4,
	SbWakeupTypeLanRemote = 5,
	SbWakeupTypePowerSwitch = 6,
	SbWakeupTypePciPme = 7,
	SbWakeupTypeAcPowerRestored = 8,
	SbWakeupTypeSmBiosMax,
	// the other section is chipset specific.
	SbWakeupTypeMax
} SB_WAKEUP_TYPE;

#pragma pack(pop)

#if     CSM_SUPPORT
EFI_STATUS
SBGen_InitializeRouterRegisters (
        EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRBIo);

UINT8
SBGen_GetPIRQIndex (
        UINT8       PIRQRegister);

EFI_STATUS
SBGen_ReadPirq (
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    OUT UINT8                           *PirqData );

EFI_STATUS
SBGen_WritePirq(
    IN EFI_LEGACY_INTERRUPT_PROTOCOL    *This,
    IN UINT8                            PirqNumber,
    IN UINT8                            PirqData);


EFI_STATUS SBGen_WriteBootScript(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *BootScriptSave
);
#endif

VOID
SBLib_ResetSystem (
    IN  EFI_RESET_TYPE      ResetType
);

EFI_STATUS
CountTime (
    IN  UINTN           DelayTime,
    IN  UINT16          BaseAddr // only needs to be 16 bit for I/O address
);
VOID SBLib_Shutdown(VOID);
VOID SB_Shutdown(VOID);
// Aptio Core 4.6.3.3 required
BOOLEAN SbLib_GetSmiState(VOID);
VOID SbLib_SmiDisable(VOID);
VOID SbLib_SmiEnable(VOID);
// SMM Related Porting Hooks
EFI_STATUS SbSmmSaveRestoreStates (IN BOOLEAN  Save);
// Latest SIO componend required
EFI_STATUS
SbLib_SetLpcDeviceDecoding(
    IN  EFI_PCI_IO_PROTOCOL *LpcPciIo,
    IN  UINT16              Base,
    IN  UINT8               DevUid,
    IN  SIO_DEV_TYPE       Type
    );
EFI_STATUS SbLib_SetLpcGenericDecoding (
    IN EFI_PCI_IO_PROTOCOL      *LpcPciIo,
    IN UINT16                   Base,
    IN UINT16                   Length,
    IN BOOLEAN                  Enable );

// Latest SBRun.c
VOID CspLibCheckPowerLoss(VOID);

#if CMOS_MANAGER_SUPPORT
#include <CmosAccess.h>
EFI_STATUS ReadWriteSecondBankTranslated (
    IN      EFI_PEI_SERVICES      **PeiServices,  // NULL in DXE phase
    IN      CMOS_ACCESS_TYPE      AccessType,
    IN      UINT16                CmosRegister,
    IN OUT  UINT8                 *CmosParameterValue
    );
#endif // CMOS_MANAGER_SUPPORT

VOID WaitOneSecRTC(VOID);

BOOLEAN
CheckSciMap(
  IN     UINT8  unavailableSciNum,
  IN OUT UINT8  *duplicateSci OPTIONAL,
  IN OUT UINT8  *duplicateSciReg OPTIONAL
  );

BOOLEAN
SbIsDefaultConfigMode(
    IN EFI_PEI_SERVICES                 **PeiServices,
   IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariablePpi
    );
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
//                                                                            //
//                  Below for access routine                              //
//                                                                            //
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//

#ifndef S3_SAVE
    #define S3_SAVE     0x80
#endif

//----------------------------------------------------------------------------
// Standard Pci routine Prototypes, No Porting Required.
//----------------------------------------------------------------------------

// the routine reference Intel Platform NorthBridge component.
// We should put the routine in to HYGON NorthBridge component.
// So I appended "SbCspLib" prior the name of routine.
// In future, we will not confilect with NorthBridge component
// If HYGON NorthBridge component implement the routine.

UINT8
SbCspLibReadPci8(
    IN UINT64   PciBusDevFunReg
);

UINT16
SbCspLibReadPci16(
    IN UINT64   PciBusDevFunReg
);

UINT32
SbCspLibReadPci32(
    IN UINT64   PciBusDevFunReg
);

VOID
SbCspLibWritePci8(
    IN UINT64   PciBusDevFunReg,
    IN UINT8    WriteValue8
);

VOID
SbCspLibWritePci16(
    IN UINT64   PciBusDevFunReg,
    IN UINT16   WriteValue16
);

VOID
SbCspLibWritePci32(
    IN UINT64   PciBusDevFunReg,
    IN UINT32   WriteValue32
);

UINT8
SbCspLibRwPci8(
    IN UINT64   PciBusDevFunReg,
    IN UINT8    SetBit8,
    IN UINT8    ResetBit8
);

UINT16
SbCspLibRwPci16(
    IN UINT64   PciBusDevFunReg,
    IN UINT16   SetBit16,
    IN UINT16   ResetBit16
);

UINT32
SbCspLibRwPci32(
    IN UINT64   PciBusDevFunReg,
    IN UINT32   SetBit32,
    IN UINT32   ResetBit32
);

VOID
SbCspLibWritePci8S3(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT8                            WriteValue8
);

VOID
SbCspLibWritePci16S3(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT16                           WriteValue16
);

VOID
SbCspLibWritePci32S3(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT32                           WriteValue32
);

UINT8
SbCspLibRwPci8S3(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT8                            SetBit8,
    IN UINT8                            ResetBit8
);

UINT16
SbCspLibRwPci16S3(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT16                           SetBit16,
    IN UINT16                           ResetBit16
);

UINT32
SbCspLibRwPci32S3(
    IN EFI_BOOT_SCRIPT_SAVE_PROTOCOL    *mBootScriptSave,
    IN UINT64                           BusDevFunReg,
    IN UINT32                           SetBit32,
    IN UINT32                           ResetBit32
);

//----------------------------------------------------------------------------
// Standard Memory routine Prototypes, No Porting Required.
//----------------------------------------------------------------------------

// the routine reference Intel Platform NorthBridge component.
// We should put the routine in to HYGON NorthBridge component.
// So I appended "SbCspLib" prior the name of routine.
// In future, we will not confilect with NorthBridge component
// If HYGON NorthBridge component implement the routine.

UINT8
SbCspLibReadMem8(
    IN UINT64   Address
);

UINT16
SbCspLibReadMem16(
    IN UINT64   Address
);

UINT32
SbCspLibReadMem32(
    IN UINT64   Address
);

VOID
SbCspLibWriteMem8(
    IN UINT64   Address,
    IN UINT8    Value
);

VOID
SbCspLibWriteMem16(
    IN UINT64   Address,
    IN UINT16   Value
);

VOID
SbCspLibWriteMem32(
    IN UINT64   Address,
    IN UINT32   Value
);

UINT8
SbCspLibRwMem8(
    IN UINT64   Address,
    IN UINT8    SetBit8,
    IN UINT8    ResetBit8
);

UINT16
SbCspLibRwMem16(
    IN UINT64   Address,
    IN UINT16   SetBit16,
    IN UINT16   ResetBit16
);

UINT32
SbCspLibRwMem32(
    IN UINT64   Address,
    IN UINT32   SetBit32,
    IN UINT32   ResetBit32
);

VOID
SbCspLibWriteMem8S3(
    IN UINT64                           Address,
    IN UINT8                            Value
);

VOID
SbCspLibWriteMem16S3(
    IN UINT64                           Address,
    IN UINT16                           Value
);

VOID
SbCspLibWriteMem32S3(
    IN UINT64                           Address,
    IN UINT32                           Value
);

UINT8
SbCspLibRwMem8S3(
    IN UINT64                           Address,
    IN UINT8                            SetBit8,
    IN UINT8                            ResetBit8
);

UINT16
SbCspLibRwMem16S3(
    IN UINT64                           Address,
    IN UINT16                           SetBit16,
    IN UINT16                           ResetBit16
);

UINT32
SbCspLibRwMem32S3(
    IN UINT64                           Address,
    IN UINT32                           SetBit32,
    IN UINT32                           ResetBit32
);

//----------------------------------------------------------------------------
// DXE PCI Access Routines, Redirect to Protocol, No Porting Required.
//----------------------------------------------------------------------------

UINTN
SbCspLibDxeReadPci(
  IN UINT8  Flag,
  IN UINT64 Address
  );

VOID
SbCspLibDxeWritePci(
  IN UINT8  Flag,
  IN UINT64 Address,
  IN UINTN  Value
  );

VOID
SbCspLibDxeRwPci(
  IN UINT8  Flag,
  IN UINT64 Address,
  IN UINTN  SetBit,
  IN UINTN  ResetBit
  );



//----------------------------------------------------------------------------
// HYGON specific MMIO , No porting required for HYGON.
//----------------------------------------------------------------------------

UINT8
ReadSbSmiIo8 (
    IN UINT8    Reg
);

VOID
WriteSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
);

UINT8
UEFI_ReadSbSmiIo8 (
    IN UINT8    Reg
);

VOID
UEFI_WriteSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
);

VOID
UEFI_RWSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
);

VOID
WriteSbSmiIo8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
);

VOID
RWSbSmiIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
);

VOID
RWSbSmiIo8S3 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
);


UINT16
ReadSbSmiIo16 (
    IN UINT8    Reg
);

VOID
WriteSbSmiIo16 (
    IN UINT8    Reg,
    IN UINT16    Data
);

VOID
WriteSbSmiIo16S3 (
    IN UINT8    Reg,
    IN UINT16    Data
);

VOID
RWSbSmiIo16 (
    IN UINT8    Reg,
    IN UINT16    ResetBit16,
    IN UINT16    SetBit16
);

VOID
RWSbSmiIo16S3 (
    IN UINT8    Reg,
    IN UINT16    ResetBit16,
    IN UINT16    SetBit16
);


UINT32
ReadSbSmiIo32 (
    IN UINT8    Reg
);

VOID
WriteSbSmiIo32 (
    IN UINT8    Reg,
    IN UINT32    Data
);

VOID
WriteSbSmiIo32S3 (
    IN UINT8    Reg,
    IN UINT32    Data
);

VOID
RWSbSmiIo32 (
    IN UINT8    Reg,
    IN UINT32    ResetBit32,
    IN UINT32    SetBit32
);
VOID
RWSbSmiIo32S3 (
    IN UINT8    Reg,
    IN UINT32    ResetBit32,
    IN UINT32    SetBit32
);
UINT8
ReadSbPmuIo8 (
    IN UINT8    Reg
);

VOID
WriteSbPmuIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
);

VOID
WriteSbPmuIo8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
);

VOID
RWSbPmuIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit8,
    IN UINT8    SetBit8
);

UINT8
ReadSbGpioIo8 (
    IN UINT8    Reg
);

VOID
WriteSbGpioIo8 (
    IN UINT8    Reg,
    IN UINT8    Data
);

VOID
RWSbGpioIo8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
);

VOID
WriteSbGpioIo8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
);

VOID
RWSbGpioIo8S3 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
);

UINT8
ReadSbMisc8 (
    IN UINT8    Reg
  );

UINT32
ReadSbMisc32 (
    IN UINT8    Reg
  );

VOID
WriteSbMisc8 (
    IN UINT8    Reg,
    IN UINT8    Data
  );

VOID
WriteSbMisc32 (
    IN UINT8    Reg,
    IN UINT32   Data
  );

VOID
RWSbMisc8 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
  );

VOID
RWSbMisc32 (
    IN UINT8    Reg,
    IN UINT32   ResetBit,
    IN UINT32   SetBit
  );

VOID
WriteSbMisc8S3 (
    IN UINT8    Reg,
    IN UINT8    Data
  );

VOID
WriteSbMisc32S3 (
    IN UINT8    Reg,
    IN UINT32   Data
  );

VOID
RWSbMisc8S3 (
    IN UINT8    Reg,
    IN UINT8    ResetBit,
    IN UINT8    SetBit
  );

VOID
RWSbMisc32S3 (
    IN UINT8    Reg,
    IN UINT32   ResetBit,
    IN UINT32   SetBit
  );




//  Index/Data I/O Access
#define READ_IO8_IDX_DATA(mIndex, bReg)         ReadIo8IdxData(mIndex, bReg)
#define WRITE_IO8_IDX_DATA(mIndex, bReg, bVal)  WriteIo8IdxData(mIndex, bReg, (UINT8)bVal)
#define RW_IO8_IDX_DATA(mIndex, bReg, Set, Rst) RwIo8IdxData(mIndex, bReg, (UINT8)Set, (UINT8)Rst)
#define SET_IO8_IDX_DATA(bReg, Set)             RwIo8IdxData(mIndex, bReg, (UINT8)Set, 0)
#define RESET_IO8_IDX_DATA(bReg, Reset)         RwIo8IdxData(mIndex, bReg, 0, (UINT8)Reset)

//  Index/Data I/O Access + S3 Boot Script
#define WRITE_IO8_IDX_DATA_S3(mBoot,mIndex, bReg, bVal) \
                            WriteIo8IdxDataS3(mBoot,mIndex, bReg, (UINT8)bVal)
#define RW_IO8_IDX_DATA_S3(mBoot,mIndex, bReg, Set, Rst) \
                            RwIo8IdxDataS3(mBoot,mIndex, bReg, (UINT8)Set, (UINT8)Rst)
#define SET_IO8_IDX_DATA_S3(mBoot,mIndex, bReg, Set) \
                            RwIo8IdxDataS3(mBoot,mIndex, bReg, (UINT8)Set, 0)
#define RESET_IO8_IDX_DATA_S3(mBoot,mIndex, bReg, Rst) \
                            RwIo8IdxDataS3(mBoot,mIndex, bReg, 0, (UINT8)Rst)

//---------------------------------------------------------------------------
// OEM Generic Functions Support
//---------------------------------------------------------------------------
typedef enum {
  SB_DEV_FUNCTION_0,
  SB_DEV_ADSP,
  SB_DEV_SATA1,
  SB_DEV_SMBUS,
  SB_DEV_EHCI2,
  SB_DEV_LPC_BRIDGE,
  SB_DEV_EHCI1,
  SB_DEV_PCI_EX_PORT1,
  SB_DEV_PCI_EX_PORT2,
  SB_DEV_PCI_EX_PORT3,
  SB_DEV_PCI_EX_PORT4,
  SB_DEV_PCI_EX_PORT5,
  SB_DEV_PCI_EX_PORT6,
  SB_DEV_PCI_EX_PORT7,
  SB_DEV_PCI_EX_PORT8,
  SB_DEV_THERMAL,
  SB_DEV_SATA2,
  SB_DEV_XHCI,

  // The following is HYGON.
  SB_DEV_EHCI3,
  SB_DEV_LAN,
  SB_DEV_MAX,
} SB_DEV_TYPE;

typedef enum {
  SbComA,
  SbComB,
  SbLpt,
  SbFdd,
  SbGameL,
  SbGameH,
  SbKbc,
  SbMc,
  SbCnf1,	// 0x2E & 0x2F
  SbCnf2	// 0x4E & 0x4F
} SB_LPC_SIO_TYPE;

typedef enum {
  TYPE_HOT_PLUG,
  TYPE_SWGPE,
  TYPE_TCOSCI,
  TYPE_RI,
  TYPE_PCI_EXP,
  TYPE_BATLOW,
  TYPE_PME,
  TYPE_ME_SCI,
  TYPE_PME_B0,
  TYPE_GP27,
  TYPE_WADT,

  // The following is HYGON.
  // Reference :
  //	Preliminary BIOS and Kernel Developer's Guide (BKDG) for AMD Family 16h Models 30h-3Fh Processors
  //	52740 Rev 1.02 - Dec 11, 2013
  // Section :
  //    3.30.17 SMI Registers
  //    AMD can map 64 event source to GPE0 SCI event.
  TYPE_GPE0_HYGON_START,
  TYPE_GPE0_GEVENT0,
  TYPE_GPE0_GEVENT1,
  TYPE_GPE0_GEVENT2,
  TYPE_GPE0_GEVENT3,
  TYPE_GPE0_GEVENT4,
  TYPE_GPE0_GEVENT5,
  TYPE_GPE0_GEVENT6,
  TYPE_GPE0_GEVENT7,
  TYPE_GPE0_GEVENT8,
  TYPE_GPE0_GEVENT9,
  TYPE_GPE0_GEVENT10,
  TYPE_GPE0_GEVENT11,
  TYPE_GPE0_GEVENT12,
  TYPE_GPE0_GEVENT13,
  TYPE_GPE0_GEVENT14,
  TYPE_GPE0_GEVENT15,
  TYPE_GPE0_GEVENT16,
  TYPE_GPE0_GEVENT17,
  TYPE_GPE0_GEVENT18,
  TYPE_GPE0_GEVENT19,
  TYPE_GPE0_GEVENT20,
  TYPE_GPE0_GEVENT21,
  TYPE_GPE0_GEVENT22,
  TYPE_GPE0_GEVENT23,
  TYPE_GPE0_USB0_PME, // device 12h
  TYPE_GPE0_USB1_PME, // device 13h
  TYPE_GPE0_USB2_PME, // device 16h
  TYPE_GPE0_HD_AUDIO_PME,
  TYPE_GPE0_SATA0_PME,
  TYPE_GPE0_SATA1_PME,
  TYPE_GPE0_EC0_PME,
  TYPE_GPE0_EC1_PME,
  TYPE_GPE0_CIR_PME,
  TYPE_GPE0_WAKE_PIN,
  TYPE_GPE0_FAN_THERMAL,
  TYPE_GPE0_ASF_MASTER,
  TYPE_GPE0_ASF_SLAVE,
  TYPE_GPE0_SMBUS0,
  TYPE_GPE0_TWARN_PIN,
  TYPE_GPE0_INTERNAL_TRAFFIC_MONITOR,
  TYPE_GPE0_LLB_PIN,
  TYPE_GPE0_PWRBTN,
  TYPE_GPE0_PROHOT_PIN,
  TYPE_GPE0_HW_ASSERTION_MESSAGE,
  TYPE_GPE0_SCI_ASSERTION_MESSAGE,
  TYPE_GPE0_RAS_EVENT,
  TYPE_GPE0_XHC0,
  TYPE_GPE0_XHC1,
  TYPE_GPE0_AC_DC_TIMER_EVENT,
  TYPE_GPE0_TEMP_STATUS5,
  TYPE_GPE0_HYGON_END,

  TYPE_SB_GPE0_MAX
} SB_GPE0_TYPE;

typedef enum {
  GPE0_EN_OP_SAVE,
  GPE0_EN_OP_RESTORE,
  GPE0_EN_OP_CLEAR_ALL
} GPE0_EN_OP;

typedef enum {
  CPU_THERMAL_TRIP_STATUS,
  AFTERG3_EN,
  PWR_FLR
} SB_MISC_TYPE;

typedef enum {
  PCONF0,
  PCONF1,
  PADVAL,
  HYGON_GPIO,
  HYGON_IOMUX
} GPIO_REG_TYPE;

typedef enum {
  GPIO_NC,
  GPIO_SC,
  GPIO_SUS,
  GPIO_Normal,
  GPIO_HYGON_GEVENT
} AMI_GPIO_GROUP_TYPE;

typedef struct _AMI_OEM_GPIO {
  AMI_GPIO_GROUP_TYPE   Group;
  UINT8                 PinNum;
} AMI_OEM_GPIO;

VOID SbEnableDisableFunctions (
    IN SB_DEV_TYPE  SbDevType,
    IN BOOLEAN      Enable
);

VOID SbSaveRestoreAllHwSmi (
    IN BOOLEAN      Save
);

VOID SbDisableAllHwSmi (
    VOID
);

BOOLEAN SbCheckNmiStatus (
    VOID
);

BOOLEAN SbEnableDisableNmi (
    IN BOOLEAN      Enable
);

VOID SbSaveRestoreNmi (
    IN BOOLEAN      Save
);

VOID SbSwSmiTrigger (
    IN UINT8        SwSmi
);

VOID SbSwSmiIo (
    IN     UINT8    SwSmi,
    IN OUT UINT8    *Data
);

UINT16 SbGetSwSmi (
    IN OUT UINT32   *DataValue
);

UINT16 SbGetTco2StsAddress (
    IN UINT16       AcpiBaseAddr
);

UINT16 SbGetTco2CntAddress (
    IN UINT16       AcpiBaseAddr
);

VOID SetAfterG3Bit (
    IN BOOLEAN      Set
);

BOOLEAN SbCheckAfterG3 (
    VOID
);

VOID SbDisableLpcDevices (
    IN SB_LPC_SIO_TYPE  SioType
);

VOID SbEnableEhciSmi (
    VOID
);

VOID SbDisableEhciSmi (
    VOID
);

BOOLEAN IsPowerButtonPressed (
    VOID
);

VOID SbEnablePciPme (
    IN UINTN        PciAddress
);

VOID SbDisablePciPme (
    IN UINTN        PciAddress
);

VOID SbEnableWolPmConfg (
    VOID
);

VOID SbDisableWolPmConfg (
    VOID
);

UINT16 SbGetIoTrapInfo (
    IN OUT UINT32   *TrappedIoData 
);

UINT16 SbGetAcpiBaseAddress (
    VOID
);

UINT16 SbGetPm1CntOffset (
    VOID
);

UINT32 SbGetRcrbAddress (
    VOID
);

BOOLEAN SbIsRtcPwrValid (
    VOID
);

BOOLEAN SbGetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbSetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbResetGpe0En (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbGpe0Operation (
    IN GPE0_EN_OP   Operation
);

BOOLEAN SbGetGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbClearGpe0Sts (
    IN SB_GPE0_TYPE Gpe0Type
);

VOID SbSetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

VOID SbResetGpe0GpinEn (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
);

VOID SbClearGpe0GpinSts (
    IN AMI_OEM_GPIO Gpio
);

BOOLEAN SbGetGpioUseSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbProgGpioUseSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      GpioMode,
    IN UINT8		MulFunc
);

BOOLEAN SbGetGpioIoSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbSetGpioIoSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      InputMode
);

BOOLEAN SbGetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio
);

VOID SbSetGpioLvlSel (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      High
);

UINT32 SbReadAltGpiSmiSts (
    VOID
);

VOID SbClearAltGpiSmiSts (
    IN AMI_OEM_GPIO Gpio
);

VOID SbProgramAltGpiSmi (
    IN AMI_OEM_GPIO Gpio,
    IN BOOLEAN      Set
);

VOID SbProgramGpioRout (
    IN AMI_OEM_GPIO Gpio,
    IN UINT8        Mode
);

BOOLEAN IsTopSwapOn(
    VOID
);

VOID  SetTopSwap(
    IN BOOLEAN                      On
);

#if SMBIOS_SUPPORT && SB_WAKEUP_TYPE_FN
UINT8 getWakeupTypeForSmbios(
    VOID
);
#endif

VOID SbProgramGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType,
    IN UINT32         AndData,
    IN UINT32         OrData
);

UINT32 SbReadGpioRegister (
    IN AMI_OEM_GPIO   Gpio,
    IN GPIO_REG_TYPE  RegType
);

EFI_STATUS SbGetMiscBitStatus(
    IN SB_MISC_TYPE         SbMiscType,
    IN OUT UINT8            *BitStatus
);

EFI_STATUS SbProgramMiscBit(
    IN SB_MISC_TYPE         SbMiscType,
    IN BOOLEAN              Set
);

BOOLEAN SbIsXhciRouting ( 
    VOID 
);

EFI_STATUS SbGetChipLanMacAddress ( 
    IN OUT UINT8            *MacAddress
);

EFI_STATUS ReadSpiDescriptor (
    IN UINT8                SectionSelect,
    IN UINT8                SectionIndex,
    OUT UINT32              *SectionData
);

#if SMBIOS_SUPPORT && SB_WAKEUP_TYPE_FN
UINT8 getWakeupTypeForSmbios(
    VOID
);
#endif

//----------------------------------------------------------------------------
// Chipset PCI Macros, Porting Required.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SMBus Module and ACPI Block (Device 20, Function 0)
#define READ_PCI8_SMBUS(bReg)           READ_PCI8(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg)
#define WRITE_PCI8_SMBUS(bReg, bVal)    WRITE_PCI8(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, bVal)
#define SET_PCI8_SMBUS(bReg, Set)       SET_PCI8(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set)
#define RESET_PCI8_SMBUS(bReg, Rst)     RESET_PCI8(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Rst)
#define RW_PCI8_SMBUS(bReg, Set, Rst)   RW_PCI8(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set, Rst)
#define READ_PCI16_SMBUS(bReg)          READ_PCI16(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg)
#define WRITE_PCI16_SMBUS(bReg, wVal)   WRITE_PCI16(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, wVal)
#define SET_PCI16_SMBUS(bReg, Set)      SET_PCI16(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set)
#define RESET_PCI16_SMBUS(bReg, Rst)    RESET_PCI16(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Rst)
#define RW_PCI16_SMBUS(bReg, Set, Rst)  RW_PCI16(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg,Set, Rst)
#define READ_PCI32_SMBUS(bReg)          READ_PCI32(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg)
#define WRITE_PCI32_SMBUS(bReg, dVal)   WRITE_PCI32(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, dVal)
#define SET_PCI32_SMBUS(bReg, Set)      SET_PCI32(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set)
#define RESET_PCI32_SMBUS(bReg, Rst)    RESET_PCI32(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Rst)
#define RW_PCI32_SMBUS(bReg, Set, Rst)  RW_PCI32(FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg,Set, Rst)

//----------------------------------------------------------------------------
//   LPC ISA Bridge (Device 20, Function 3)
#define READ_PCI8_SB(bReg)              READ_PCI8(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg)
#define WRITE_PCI8_SB(bReg, bVal)       WRITE_PCI8(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, bVal)
#define SET_PCI8_SB(bReg, Set)          SET_PCI8(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set)
#define RESET_PCI8_SB(bReg, Reset)      RESET_PCI8(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Reset)
#define RW_PCI8_SB(bReg, Set, Reset)    RW_PCI8(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set, Reset)
#define READ_PCI16_SB(bReg)             READ_PCI16(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg)
#define WRITE_PCI16_SB(bReg, wVal)      WRITE_PCI16(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, wVal)
#define SET_PCI16_SB(bReg, Set)         SET_PCI16(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set)
#define RESET_PCI16_SB(bReg, Reset)     RESET_PCI16(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Reset)
#define RW_PCI16_SB(bReg, Set, Reset)   RW_PCI16(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set, Reset)
#define READ_PCI32_SB(bReg)             READ_PCI32(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC, bReg)
#define WRITE_PCI32_SB(bReg, dVal)      WRITE_PCI32(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, dVal)
#define SET_PCI32_SB(bReg, Set)         SET_PCI32(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set)
#define RESET_PCI32_SB(bReg, Reset)     RESET_PCI32(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Reset)
#define RW_PCI32_SB(bReg, Set, Reset)   RW_PCI32(FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// IDE Controller (Device 20, Function 1)
#define READ_PCI8_IDE(bReg)             READ_PCI8(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg)
#define WRITE_PCI8_IDE(bReg, bVal)      WRITE_PCI8(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, bVal)
#define SET_PCI8_IDE(bReg, Set)         SET_PCI8(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set)
#define RESET_PCI8_IDE(bReg, Reset)     RESET_PCI8(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Reset)
#define RW_PCI8_IDE(bReg, Set, Reset)   RW_PCI8(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set, Reset)
#define READ_PCI16_IDE(bReg)            READ_PCI16(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg)
#define WRITE_PCI16_IDE(bReg, wVal)     WRITE_PCI16(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, wVal)
#define SET_PCI16_IDE(bReg, Set)        SET_PCI16(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set)
#define RESET_PCI16_IDE(bReg, Reset)    RESET_PCI16(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Reset)
#define RW_PCI16_IDE(bReg, Set, Reset)  RW_PCI16(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set, Reset)
#define READ_PCI32_IDE(bReg)            READ_PCI32(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg)
#define WRITE_PCI32_IDE(bReg, dVal)     WRITE_PCI32(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, dVal)
#define SET_PCI32_IDE(bReg, Set)        SET_PCI32(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set)
#define RESET_PCI32_IDE(bReg, Reset)    RESET_PCI32(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Reset)
#define RW_PCI32_IDE(bReg, Set, Reset)  RW_PCI32(FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// SATA Registers (Bus 0, Device 17, Function 0)
#define READ_PCI8_SATA(bReg)            READ_PCI8(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg)
#define WRITE_PCI8_SATA(bReg, bVal)     WRITE_PCI8(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, bVal)
#define SET_PCI8_SATA(bReg, Set)        SET_PCI8(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set)
#define RESET_PCI8_SATA(bReg, Reset)    RESET_PCI8(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Reset)
#define RW_PCI8_SATA(bReg, Set, Reset)  RW_PCI8(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set, Reset)
#define READ_PCI16_SATA(bReg)           READ_PCI16(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg)
#define WRITE_PCI16_SATA(bReg, wVal)    WRITE_PCI16(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, wVal)
#define SET_PCI16_SATA(bReg, Set)       SET_PCI16(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set)
#define RESET_PCI16_SATA(bReg, Reset)   RESET_PCI16(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Reset)
#define RW_PCI16_SATA(bReg, Set, Reset) RW_PCI16(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set, Reset)
#define READ_PCI32_SATA(bReg)           READ_PCI32(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg)
#define WRITE_PCI32_SATA(bReg, dVal)    WRITE_PCI32(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, dVal)
#define SET_PCI32_SATA(bReg, Set)       SET_PCI32(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set)
#define RESET_PCI32_SATA(bReg, Reset)   RESET_PCI32(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Reset)
#define RW_PCI32_SATA(bReg, Set, Reset) RW_PCI32(FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set, Reset)


//----------------------------------------------------------------------------
// Host PCI Bridge Registers (Device 20, Function 4)
#define READ_PCI8_PCIBR(bReg)            READ_PCI8(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg)
#define WRITE_PCI8_PCIBR(bReg, bVal)     WRITE_PCI8(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, bVal)
#define SET_PCI8_PCIBR(bReg, Set)        SET_PCI8(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set)
#define RESET_PCI8_PCIBR(bReg, Rst)      RESET_PCI8(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Rst)
#define RW_PCI8_PCIBR(bReg, Set, Rst)    RW_PCI8(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set, Rst)
#define READ_PCI16_PCIBR(bReg)           READ_PCI16(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg)
#define WRITE_PCI16_PCIBR(bReg, wVal)    WRITE_PCI16(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, wVal)
#define SET_PCI16_PCIBR(bReg, Set)       SET_PCI16(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set)
#define RESET_PCI16_PCIBR(bReg, Rst)     RESET_PCI16(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Rst)
#define RW_PCI16_PCIBR(bReg, Set, Rst)   RW_PCI16(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg,Set, Rst)
#define READ_PCI32_PCIBR(bReg)           READ_PCI32(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg)
#define WRITE_PCI32_PCIBR(bReg, dVal)    WRITE_PCI32(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, dVal)
#define SET_PCI32_PCIBR(bReg, Set)       SET_PCI32(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set)
#define RESET_PCI32_PCIBR(bReg, Rst)     RESET_PCI32(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Rst)
#define RW_PCI32_PCIBR(bReg, Set, Rst)   RW_PCI32(FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg,Set, Rst)

//----------------------------------------------------------------------------
// USB Controller #1 (EHCI)
#define READ_PCI8_EUSB(bReg)            READ_PCI8(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg)
#define WRITE_PCI8_EUSB(bReg, bVal)     WRITE_PCI8(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, bVal)
#define SET_PCI8_EUSB(bReg, Set)        SET_PCI8(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set)
#define RESET_PCI8_EUSB(bReg, Reset)    RESET_PCI8(FCH_EHCI1_BUS,FCH_EHCI1_DEV,NC, bReg, Reset)
#define RW_PCI8_EUSB(bReg, Set, Reset)  RW_PCI8(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set, Reset)
#define READ_PCI16_EUSB(bReg)           READ_PCI16(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg)
#define WRITE_PCI16_EUSB(bReg, wVal)    WRITE_PCI16(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, wVal)
#define SET_PCI16_EUSB(bReg, Set)       SET_PCI16(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set)
#define RESET_PCI16_EUSB(bReg, Reset)   RESET_PCI16(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Reset)
#define RW_PCI16_EUSB(bReg, Set, Reset) RW_PCI16(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set, Reset)
#define READ_PCI32_EUSB(bReg)           READ_PCI32(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg)
#define WRITE_PCI32_EUSB(bReg, dVal)    WRITE_PCI32(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, dVal)
#define SET_PCI32_EUSB(bReg, Set)       SET_PCI32(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set)
#define RESET_PCI32_EUSB(bReg, Reset)   RESET_PCI32(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Reset)
#define RW_PCI32_EUSB(bReg, Set, Reset) RW_PCI32(FCH_EHCI1_BUS,FCH_EHCI1_DEV,0,bReg, Set, Reset)


//----------------------------------------------------------------------------
// USB XHCI 0 Controller  (XHCI)
#define READ_PCI8_XHCI0(bReg)               READ_PCI8(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg)
#define WRITE_PCI8_XHCI0(bReg, bVal)        WRITE_PCI8(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, bVal)
#define SET_PCI8_XHCI0(bReg, Set)           SET_PCI8(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set)
#define RESET_PCI8_XHCI0(bReg, Reset)       RESET_PCI8(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Reset)
#define RW_PCI8_XHCI0(bReg, Set, Reset)     RW_PCI8(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set, Reset)
#define READ_PCI16_XHCI0(bReg)              READ_PCI16(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg)
#define WRITE_PCI16_XHCI0(bReg, wVal)       WRITE_PCI16(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, wVal)
#define SET_PCI16_XHCI0(bReg, Set)          SET_PCI16(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set)
#define RESET_PCI16_XHCI0(bReg, Reset)      RESET_PCI16(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Reset)
#define RW_PCI16_XHCI0(bReg, Set, Reset)    RW_PCI16(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set, Reset)
#define READ_PCI32_XHCI0(bReg)              READ_PCI32(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg)
#define WRITE_PCI32_XHCI0(bReg, dVal)       WRITE_PCI32(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, dVal)
#define SET_PCI32_XHCI0(bReg, Set)          SET_PCI32(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set)
#define RESET_PCI32_XHCI0(bReg, Reset)      RESET_PCI32(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Reset)
#define RW_PCI32_XHCI0(bReg, Set, Reset)    RW_PCI32(FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// USB XHCI 0 Controller  (XHCI)
#define READ_PCI8_XHCI1(bReg)               READ_PCI8(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg)
#define WRITE_PCI8_XHCI1(bReg, bVal)        WRITE_PCI8(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, bVal)
#define SET_PCI8_XHCI1(bReg, Set)           SET_PCI8(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Set)
#define RESET_PCI8_XHCI1(bReg, Reset)       RESET_PCI8(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Reset)
#define RW_PCI8_XHCI1(bReg, Set, Reset)     RW_PCI8(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Set, Reset)
#define READ_PCI16_XHCI1(bReg)              READ_PCI16(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg)
#define WRITE_PCI16_XHCI1(bReg, wVal)       WRITE_PCI16(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, wVal)
#define SET_PCI16_XHCI1(bReg, Set)          SET_PCI16(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Set)
#define RESET_PCI16_XHCI1(bReg, Reset)      RESET_PCI16(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Reset)
#define RW_PCI16_XHCI1(bReg, Set, Reset)    RW_PCI16(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Set, Reset)
#define READ_PCI32_XHCI1(bReg)              READ_PCI32(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg)
#define WRITE_PCI32_XHCI1(bReg, dVal)       WRITE_PCI32(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, dVal)
#define SET_PCI32_XHCI1(bReg, Set)          SET_PCI32(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Set)
#define RESET_PCI32_XHCI1(bReg, Reset)      RESET_PCI32(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Reset)
#define RW_PCI32_XHCI1(bReg, Set, Reset)    RW_PCI32(FCH_XHCI1_BUS,FCH_XHCI1_DEV,FCH_XHCI1_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// SMBus Module and ACPI Block (Device 20, Function 0)
#define WRITE_PCI8_SMBUS_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, bVal)
#define SET_PCI8_SMBUS_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set)
#define RESET_PCI8_SMBUS_S3(mBoot, bReg, Rst) \
                                RESET_PCI8_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Rst)
#define RW_PCI8_SMBUS_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI8_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set,Rst)
#define WRITE_PCI16_SMBUS_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg,wVal)
#define SET_PCI16_SMBUS_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set)
#define RESET_PCI16_SMBUS_S3(mBoot, bReg, Rst) \
                                RESET_PCI16_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Rst)
#define RW_PCI16_SMBUS_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI16_S3(mBoot,FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set,Rst)
#define WRITE_PCI32_SMBUS_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg,dVal)
#define SET_PCI32_SMBUS_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set)
#define RESET_PCI32_SMBUS_S3(mBoot, bReg, Rst) \
                                RESET_PCI32_S3(mBoot, FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Rst)
#define RW_PCI32_SMBUS_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI32_S3(mBoot,FCH_ISA_BUS,FCH_ISA_DEV,FCH_ISA_FUNC,bReg, Set,Rst)

//----------------------------------------------------------------------------
//   LPC ISA Bridge (Device 20, Function 3)
#define WRITE_PCI8_SB_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, bVal)
#define SET_PCI8_SB_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set)
#define RESET_PCI8_SB_S3(mBoot, bReg, Reset) \
                                RESET_PCI8_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Reset)
#define RW_PCI8_SB_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI8_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set, Reset)
#define WRITE_PCI16_SB_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, wVal)
#define SET_PCI16_SB_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set)
#define RESET_PCI16_SB_S3(mBoot, bReg, Reset) \
                                RESET_PCI16_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Reset)
#define RW_PCI16_SB_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI16_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set, Reset)
#define WRITE_PCI32_SB_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, dVal)
#define SET_PCI32_SB_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set)
#define RESET_PCI32_SB_S3(mBoot, bReg, Reset) \
                                RESET_PCI32_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Reset)
#define RW_PCI32_SB_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI32_S3(mBoot, FCH_LPC_BUS,FCH_LPC_DEV,FCH_LPC_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// IDE Controller (Device 20, Function 1)
#define WRITE_PCI8_IDE_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, bVal)
#define SET_PCI8_IDE_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set)
#define RESET_PCI8_IDE_S3(mBoot, bReg, Reset) \
                                RESET_PCI8_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Reset)
#define RW_PCI8_IDE_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI8_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set, Reset)
#define WRITE_PCI16_IDE_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, wVal)
#define SET_PCI16_IDE_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set)
#define RESET_PCI16_IDE_S3(mBoot, bReg, Reset) \
                                RESET_PCI16_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Reset)
#define RW_PCI16_IDE_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI16_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set, Reset)
#define WRITE_PCI32_IDE_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, dVal)
#define SET_PCI32_IDE_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set)
#define RESET_PCI32_IDE_S3(mBoot, bReg, Reset) \
                                RESET_PCI32_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Reset)
#define RW_PCI32_IDE_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI32_S3(mBoot, FCH_IDE_BUS,FCH_IDE_DEV,FCH_IDE_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// SATA Registers (Bus 0, Device 17, Function 0)
#define WRITE_PCI8_SATA_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, bVal)
#define SET_PCI8_SATA_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set)
#define RESET_PCI8_SATA_S3(mBoot, bReg, Reset) \
                                RESET_PCI8_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Reset)
#define RW_PCI8_SATA_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI8_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set, Rst)
#define WRITE_PCI16_SATA_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, wVal)
#define SET_PCI16_SATA_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set)
#define RESET_PCI16_SATA_S3(mBoot, bReg, Reset) \
                                RESET_PCI16_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Reset)
#define RW_PCI16_SATA_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI16_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set, Reset)
#define WRITE_PCI32_SATA_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, dVal)
#define SET_PCI32_SATA_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set)
#define RESET_PCI32_SATA_S3(mBoot, bReg, Reset) \
                                RESET_PCI32_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Reset)
#define RW_PCI32_SATA_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI32_S3(mBoot, FCH_SATA1_BUS,FCH_SATA1_DEV,FCH_SATA1_FUNC,bReg, Set, Reset)

//----------------------------------------------------------------------------
// Host PCI Bridge Registers (Device 20, Function 4)
#define WRITE_PCI8_PCIBR_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, bVal)
#define SET_PCI8_PCIBR_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set)
#define RESET_PCI8_PCIBR_S3(mBoot, bReg, Rst) \
                                RESET_PCI8_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Rst)
#define RW_PCI8_PCIBR_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI8_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set,Rst)
#define WRITE_PCI16_PCIBR_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg,wVal)
#define SET_PCI16_PCIBR_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set)
#define RESET_PCI16_PCIBR_S3(mBoot, bReg, Rst) \
                                RESET_PCI16_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Rst)
#define RW_PCI16_PCIBR_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI16_S3(mBoot,FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set,Rst)
#define WRITE_PCI32_PCIBR_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg,dVal)
#define SET_PCI32_PCIBR_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set)
#define RESET_PCI32_PCIBR_S3(mBoot, bReg, Rst) \
                                RESET_PCI32_S3(mBoot, FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Rst)
#define RW_PCI32_PCIBR_S3(mBoot, bReg, Set, Rst) \
                                RW_PCI32_S3(mBoot,FCH_PCI_BUS,FCH_PCI_DEV,FCH_PCI_FUNC,bReg, Set,Rst)

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// USB Controller #1 (EHCI)
#define WRITE_PCI8_EUSB_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, bVal)
#define SET_PCI8_EUSB_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set)
#define RESET_PCI8_EUSB_S3(mBoot, bReg, Reset) \
                                RESET_PCI8_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Reset)
#define RW_PCI8_EUSB_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI8_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set, Reset)
#define WRITE_PCI16_EUSB_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, wVal)
#define SET_PCI16_EUSB_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set)
#define RESET_PCI16_EUSB_S3(mBoot, bReg, Reset) \
                                RESET_PCI16_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Reset)
#define RW_PCI16_EUSB_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI16_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set, Reset)
#define WRITE_PCI32_EUSB_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, dVal)
#define SET_PCI32_EUSB_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set)
#define RESET_PCI32_EUSB_S3(mBoot, bReg, Reset) \
                                RESET_PCI32_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Reset)
#define RW_PCI32_EUSB_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI32_S3(mBoot, FCH_EHCI1_BUS,FCH_EHCI1_DEV,0, bReg, Set, Reset)

//----------------------------------------------------------------------------
// USB XHCI0 Controller #0 (XHCI)
#define WRITE_PCI8_XHCI0_S3(mBoot, bReg, bVal) \
                                WRITE_PCI8_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, bVal)
#define SET_PCI8_XHCI0_S3(mBoot, bReg, Set) \
                                SET_PCI8_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set)
#define RESET_PCI8_XHCI0_S3(mBoot, bReg, Reset) \
                                RESET_PCI8_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Reset)
#define RW_PCI8_XHCI0_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI8_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set, Reset)
#define WRITE_PCI16_XHCI0_S3(mBoot, bReg, wVal) \
                                WRITE_PCI16_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, wVal)
#define SET_PCI16_XHCI0_S3(mBoot, bReg, Set) \
                                SET_PCI16_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set)
#define RESET_PCI16_XHCI0_S3(mBoot, bReg, Reset) \
                                RESET_PCI16_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Reset)
#define RW_PCI16_XHCI0_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI16_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set, Reset)
#define WRITE_PCI32_XHCI0_S3(mBoot, bReg, dVal) \
                                WRITE_PCI32_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, dVal)
#define SET_PCI32_XHCI0_S3(mBoot, bReg, Set) \
                                SET_PCI32_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set)
#define RESET_PCI32_XHCI0_S3(mBoot, bReg, Reset) \
                                RESET_PCI32_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Reset)
#define RW_PCI32_XHCI0_S3(mBoot, bReg, Set, Reset) \
                                RW_PCI32_S3(mBoot, FCH_XHCI_BUS,FCH_XHCI_DEV,FCH_XHCI_FUNC,bReg, Set, Reset)

//------------------------------------------------------------------------;
// Chipset specific IO Macros, need porting for each SB controller. ;
//------------------------------------------------------------------------;

//----------------------------------------------------------------------------
// ACPI Access
#define READ_IO8_PM(IoReg)                READ_IO8(PM_BASE_ADDRESS+IoReg)
#define WRITE_IO8_PM(IoReg, mVal)         WRITE_IO8(PM_BASE_ADDRESS+IoReg, mVal)
#define SET_IO8_PM(IoReg, mSet)           RW_IO8(PM_BASE_ADDRESS+IoReg, mSet, 0)
#define RESET_IO8_PM(IoReg, mReset)       RW_IO8(PM_BASE_ADDRESS+IoReg, 0, mReset)
#define RW_IO8_PM(IoReg, mSet, mReset)    RW_IO8(PM_BASE_ADDRESS+IoReg, mSet, mReset)
#define READ_IO16_PM(IoReg)               READ_IO16(PM_BASE_ADDRESS+IoReg)
#define WRITE_IO16_PM(IoReg, mVal)        WRITE_IO16(PM_BASE_ADDRESS+IoReg, mVal)
#define SET_IO16_PM(IoReg, mSet)          RW_IO16(PM_BASE_ADDRESS+IoReg, mSet, 0)
#define RESET_IO16_PM(IoReg, mReset)      RW_IO16(PM_BASE_ADDRESS+IoReg, 0, mReset)
#define RW_IO16_PM(IoReg, mSet, mReset)   RW_IO16(PM_BASE_ADDRESS+IoReg, mSet, mReset)
#define READ_IO32_PM(IoReg)               READ_IO32(PM_BASE_ADDRESS+IoReg)
#define WRITE_IO32_PM(IoReg, mVal)        WRITE_IO32(PM_BASE_ADDRESS+IoReg, mVal)
#define SET_IO32_PM(IoReg, mSet)          RW_IO32(PM_BASE_ADDRESS+IoReg, mSet, 0)
#define RESET_IO32_PM(IoReg, mReset)      RW_IO32(PM_BASE_ADDRESS+IoReg, 0, mReset)
#define RW_IO32_PM(IoReg, mSet, mReset)   RW_IO32(PM_BASE_ADDRESS+IoReg, mSet, mReset)

//----------------------------------------------------------------------------
//  ACPI Access + S3 Boot Script
#define WRITE_IO8_PM_S3(mBoot, IoReg, mVal) \
                WRITE_IO8_S3(mBoot, PM_BASE_ADDRESS+IoReg, mVal)
#define SET_IO8_PM_S3(mBoot, IoReg, mSet) \
                RW_IO8_S3(mBoot, PM_BASE_ADDRESS+IoReg, mSet, 0)
#define RESET_IO8_PM_S3(mBoot, IoReg, mReset) \
                RW_IO8_S3(mBoot, PM_BASE_ADDRESS+IoReg, 0, mReset)
#define RW_IO8_PM_S3(mBoot, IoReg, mSet, mReset) \
                RW_IO8_S3(mBoot, PM_BASE_ADDRESS+IoReg, mSet, mReset)
#define WRITE_IO16_PM_S3(mBoot, IoReg, mVal) \
                WRITE_IO16_S3(mBoot, PM_BASE_ADDRESS+IoReg, mVal)
#define SET_IO16_PM_S3(mBoot, IoReg, mSet) \
                RW_IO16_S3(mBoot, PM_BASE_ADDRESS+IoReg, mSet, 0)
#define RESET_IO16_PM_S3(mBoot, IoReg, mReset) \
                RW_IO16_S3(mBoot, PM_BASE_ADDRESS+IoReg, 0, mReset)
#define RW_IO16_PM_S3(mBoot, IoReg, mSet, mReset) \
                RW_IO16_S3(mBoot, PM_BASE_ADDRESS+IoReg, mSet, mReset)
#define WRITE_IO32_PM_S3(mBoot, IoReg, mVal) \
                WRITE_IO32_S3(mBoot, PM_BASE_ADDRESS+IoReg, mVal)
#define SET_IO32_PM_S3(mBoot, IoReg, mSet) \
                RW_IO32_S3(mBoot, PM_BASE_ADDRESS+IoReg, mSet, 0)
#define RESET_IO32_PM_S3(mBoot, IoReg, mReset) \
                RW_IO32_S3(mBoot, PM_BASE_ADDRESS+IoReg, 0, mReset)
#define RW_IO32_PM_S3(mBoot, IoReg, mSet, mReset) \
                RW_IO32_S3(mBoot, PM_BASE_ADDRESS+IoReg, mSet, mReset)

//----------------------------------------------------------------------------
//  Power Management (PM) Access
#define READ_IO8_PMIO(bReg)               READ_IO8_IDX_DATA(PM_IO_INDEX, bReg)
#define WRITE_IO8_PMIO(bReg, mVal)        WRITE_IO8_IDX_DATA(PM_IO_INDEX, bReg, mVal)
#define RW_IO8_PMIO(bReg, mSet, mReset)   RW_IO8_IDX_DATA(PM_IO_INDEX, bReg, mSet, mReset)
#define SET_IO8_PMIO(bReg, mSet)          RW_IO8_IDX_DATA(PM_IO_INDEX, bReg, mSet, 0)
#define RESET_IO8_PMIO(bReg, mReset)      RW_IO8_IDX_DATA(PM_IO_INDEX, bReg, 0, mReset)

//----------------------------------------------------------------------------
//  Power Management (PM) Access + S3 Boot Script
#define WRITE_IO8_PMIO_S3(mBoot,bReg, mVal) \
                WRITE_IO8_IDX_DATA_S3(mBoot,PM_IO_INDEX, bReg, mVal)
#define RW_IO8_PMIO_S3(mBoot,bReg, mSet, mReset) \
                RW_IO8_IDX_DATA_S3(mBoot,PM_IO_INDEX, bReg, mSet, mReset)
#define SET_IO8_PMIO_S3(mBoot,bReg, mSet) \
                RW_IO8_IDX_DATA_S3(mBoot,PM_IO_INDEX, bReg, mSet, 0)
#define RESET_IO8_PMIO_S3(mBoot,bReg, mReset) \
                RW_IO8_IDX_DATA_S3(mBoot,PM_IO_INDEX, bReg, 0, mReset)

//----------------------------------------------------------------------------
//  Power Management (PM2) Block2 Registers Access
#define READ_IO8_PMIO2(bReg)              READ_IO8_IDX_DATA(PM2_IO_INDEX, bReg)
#define WRITE_IO8_PMIO2(bReg, mVal)       WRITE_IO8_IDX_DATA(PM2_IO_INDEX, bReg, mVal)
#define RW_IO8_PMIO2(bReg, mSet, mReset)  RW_IO8_IDX_DATA(PM2_IO_INDEX, bReg, mSet, mReset)
#define SET_IO8_PMIO2(bReg, mSet)         RW_IO8_IDX_DATA(PM2_IO_INDEX, bReg, mSet, 0)
#define RESET_IO8_PMIO2(bReg, mReset)     RW_IO8_IDX_DATA(PM2_IO_INDEX, bReg, 0, mReset)

//----------------------------------------------------------------------------
//  Power Management (PM2) Block2 Registers Access + S3 Boot Script
#define WRITE_IO8_PMIO2_S3(mBoot,mIdx, mVal) \
                WRITE_IO8_IDX_DATA_S3(mBoot,PM2_IO_INDEX, mIdx, mVal)
#define RW_IO8_PMIO2_S3(mBoot,mIdx, mSet, mReset) \
                RW_IO8_IDX_DATA_S3(mBoot,PM2_IO_INDEX, mIdx, mSet, mReset)
#define SET_IO8_PMIO2_S3(mBoot,mIdx, mSet) \
                RW_IO8_IDX_DATA_S3(mBoot,PM2_IO_INDEX, mIdx, mSet, 0)
#define RESET_IO8_PMIO2_S3(mBoot,mIdx, mReset) \
                RW_IO8_IDX_DATA_S3(mBoot,PM2_IO_INDEX, mIdx, 0, mReset)

//----------------------------------------------------------------------------
//  PCI IRQ Routing (Pci_Intr_Index/Pci_Intr_Data)
#define READ_IO8_PIRQ(mIdx) \
                READ_IO8_IDX_DATA(PIRQ_IO_INDEX, mIdx)
#define WRITE_IO8_PIRQ(mIdx, mVal) \
                WRITE_IO8_IDX_DATA(PIRQ_IO_INDEX, mIdx, mVal)

//----------------------------------------------------------------------------
//  PCI IRQ Routing (Pci_Intr_Index/Pci_Intr_Data) + S3 BootScript
#define WRITE_IO8_PIRQ_S3(mBoot,mIdx, mVal) \
                WRITE_IO8_IDX_DATA_S3(mBoot,PIRQ_IO_INDEX, mIdx, mVal)

//----------------------------------------------------------------------------
// HYGON (AlternatRtcAddrPort/AlternatRtcDataPort)
#define READ_IO8_HYGON_RTC(mIdx)          READ_IO8_IDX_DATA(0x72, mIdx)
#define WRITE_IO8_HYGON_RTC(mIdx, mVal)   WRITE_IO8_IDX_DATA(0x72, mIdx, mVal)

//------------------------------------------------------------------------;
// Chipset specific MMIO Macros, need porting for each SB controller.;
//------------------------------------------------------------------------;

//------------------------------------------------------------------------;
// GPIO MMIO
#define READ_MMIO8_GPIO(MarcoReg) ReadSbGpioIo8(MarcoReg)

#define WRITE_MMIO8_GPIO(MarcoReg, MarcoData) WriteSbGpioIo8(MarcoReg, MarcoData)

#define RW_MMIO8_GPIO(MarcoReg, MarcoResetBits, MarcoSetBits) \
			RWSbGpioIo8(MarcoReg, MarcoResetBits, MarcoSetBits)

//------------------------------------------------------------------------;
// MISC MMIO
#define READ_MMIO8_MISC(MarcoReg) ReadSbMisc8(MarcoReg)
#define READ_MMIO32_MISC(MarcoReg) ReadSbMisc32(MarcoReg)

#define WRITE_MMIO8_MISC(MarcoReg, MarcoData) WriteSbMisc8(MarcoReg, MarcoData)

#define RW_MMIO8_MISC(MarcoReg, MarcoResetBits, MarcoSetBits) \
            RWSbMisc8(MarcoReg, MarcoResetBits, MarcoSetBits)

#define SET_MMIO8_MISC(MarcoReg, MarcoSetBits) \
            RWSbMisc8(MarcoReg, 0, MarcoSetBits)

#define RESET_MMIO8_MISC(MarcoReg, MarcoResetBits) \
            RWSbMisc8(MarcoReg, MarcoResetBits, 0)

#define WRITE_MMIO32_MISC(MarcoReg, MarcoData) WriteSbMisc32(MarcoReg, MarcoData)

#define RW_MMIO32_MISC(MarcoReg, MarcoResetBits, MarcoSetBits) \
            RWSbMisc32(MarcoReg, MarcoResetBits, MarcoSetBits)

#define SET_MMIO32_MISC(MarcoReg, MarcoSetBits) \
            RWSbMisc32(MarcoReg, 0, MarcoSetBits)

#define RESET_MMIO32_MISC(MarcoReg, MarcoResetBits) \
            RWSbMisc32(MarcoReg, MarcoResetBits, 0)

//------------------------------------------------------------------------;
// MISC MMIO + S3 boot script
#define WRITE_MMIO8_MISC_S3(MarcoReg, MarcoData) WriteSbMisc8S3(MarcoReg, MarcoData)

#define RW_MMIO8_MISC_S3(MarcoReg, MarcoResetBits, MarcoSetBits) \
            RWSbMisc8S3(MarcoReg, MarcoResetBits, MarcoSetBits)

#define SET_MMIO8_MISC_S3(MarcoReg, MarcoSetBits) \
            RWSbMisc8S3(MarcoReg, 0, MarcoSetBits)

#define RESET_MMIO8_MISC_S3(MarcoReg, MarcoResetBits) \
            RWSbMisc8S3(MarcoReg, MarcoResetBits, 0)

#define WRITE_MMIO32_MISC_S3(MarcoReg, MarcoData) WriteSbMisc32S3(MarcoReg, MarcoData)

#define RW_MMIO32_MISC_S3(MarcoReg, MarcoResetBits, MarcoSetBits) \
            RWSbMisc32S3(MarcoReg, MarcoResetBits, MarcoSetBits)

#define SET_MMIO32_MISC_S3(MarcoReg, MarcoSetBits) \
            RWSbMisc32S3(MarcoReg, 0, MarcoSetBits)

#define RESET_MMIO32_MISC_S3(MarcoReg, MarcoResetBits) \
            RWSbMisc32S3(MarcoReg, MarcoResetBits, 0)

//------------------------------------------------------------------------;
// SMI MMIO
#define READ_MMIO8_SMI(MarcoReg) ReadSbSmiIo8(MarcoReg)
#define READ_MMIO32_SMI(MarcoReg) ReadSbSmiIo32(MarcoReg)

#define WRITE_MMIO8_SMI(MarcoReg, MarcoData) WriteSbSmiIo8(MarcoReg, MarcoData)

#define RW_MMIO8_SMI(MarcoReg, MarcoResetBits, MarcoSetBits) \
            RWSbSmiIo8(MarcoReg, MarcoResetBits, MarcoSetBits)

#define SET_MMIO8_SMI(MarcoReg, MarcoSetBits) \
            RWSbSmiIo8(MarcoReg, 0, MarcoSetBits)

#define RESET_MMIO8_SMI(MarcoReg, MarcoResetBits) \
            RWSbSmiIo8(MarcoReg, MarcoResetBits, 0)

#define WRITE_MMIO32_SMI(MarcoReg, MarcoData) WriteSbSmiIo32(MarcoReg, MarcoData)

#define RW_MMIO32_SMI(MarcoReg, MarcoResetBits, MarcoSetBits) \
            RWSbSmiIo32(MarcoReg, MarcoResetBits, MarcoSetBits)

#define SET_MMIO32_SMI(MarcoReg, MarcoSetBits) \
            RWSbSmiIo32(MarcoReg, 0, MarcoSetBits)

#define RESET_MMIO32_SMI(MarcoReg, MarcoResetBits) \
            RWSbSmiIo32(MarcoReg, MarcoResetBits, 0)



#ifdef __cplusplus
}
#endif
#endif

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
