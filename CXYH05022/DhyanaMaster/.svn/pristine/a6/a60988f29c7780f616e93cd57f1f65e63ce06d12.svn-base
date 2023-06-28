//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

/** @file PspCspLib.c

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------

#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Token.h>
#include <PspCspLib.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

#define GET_PCI_BUS(Reg) (((UINT32) Reg >> 16) & 0xFF)
#define GET_PCI_DEV(Reg) (((UINT32) Reg >> 11) & 0x1F)
#define GET_PCI_FUNC(Reg) (((UINT32) Reg >> 8) & 0x7)
#define GET_PCI_OFFSET(Reg) ((UINT32)Reg & 0xFF)

// Type Definition(s)

#define PCI_CONFIG_SMU_INDIRECT_INDEX   0xB8          ///<  Gnb Offset index for SMU mbox
#define PCI_CONFIG_SMU_INDIRECT_DATA    0xBC          ///<  Gnb Offset data for SMU mbox

#define SMU_CC_PSP_FUSES_STATUS         0xC0018000ul  ///< offset in GNB to find PSP fusing
#define SMU_CC_PSP_FUSES_SECURE         BIT0          ///< BIT0
#define SMU_CC_PSP_FUSES_FRA_ENABLE     BIT1          ///< BIT1
#define SMU_CC_PSP_FUSES_PROTO          BIT2          ///< BIT2
#define PLATFORM_SECURE_BOOT_EN         BIT4          ///< BIT4

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)
extern  WriteMsr();
extern  ReadMsr();
// Function Definition(s)

/**
 * Read memory/MMIO
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 *
 */
VOID
AmiLibHygonMemRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
     OUT   VOID *Value
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *(UINT8 *) Value = MmioRead8 ((UINTN)(UINT32)MemAddress);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *(UINT16 *) Value = MmioRead16 ((UINTN)(UINT32)MemAddress);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *(UINT32 *) Value = MmioRead32 ((UINTN)(UINT32)MemAddress);
    break;
  default:
    ASSERT (FALSE);
  }
}

/**
 * Write memory/MMIO
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 *
 */
VOID
AmiLibHygonMemWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Value
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    MmioWrite8 ((UINTN)(UINT32)MemAddress, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    MmioWrite16 ((UINTN)(UINT32)MemAddress, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    MmioWrite32 ((UINTN)(UINT32)MemAddress, *((UINT32 *) Value));
    break;
  default:
    ASSERT (FALSE);
  }
}

/**
 * Read IO port
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO port address
 * @param[in] Value         Pointer to save data
 *
 */
VOID
AmiLibHygonIoRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
     OUT   VOID *Value
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *(UINT8 *) Value = IoRead8 (IoAddress);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *(UINT16 *) Value = IoRead16 (IoAddress);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *(UINT32 *) Value = IoRead32 (IoAddress);
    break;
  default:
    ASSERT (FALSE);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Write IO port
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO port address
 * @param[in] Value         Pointer to data
 *
 */
VOID
AmiLibHygonIoWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Value
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    IoWrite8 (IoAddress, *(UINT8 *) Value);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    IoWrite16 (IoAddress, *(UINT16 *) Value);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    IoWrite32 (IoAddress, *(UINT32 *) Value);
    break;
  default:
    ASSERT (FALSE);
  }
}

/**
 * Get MMIO base address for PCI accesses
 *
 * @param[out] MmioAddress   PCI MMIO base address
 * @param[out] MmioSize      Size of region in bytes
 *
 * @retval    TRUE          MmioAddress/MmioSize are valid
 */
BOOLEAN
STATIC
GetPciMmioAddress (
     OUT   UINT64            *MmioAddress,
     OUT   UINT32            *MmioSize
  )
{
  BOOLEAN MmioIsEnabled;
  UINT32  EncodedSize;
  UINT64  LocalMsrRegister;

  MmioIsEnabled = FALSE;
  LocalMsrRegister = ReadMsr(MSR_MMIO_Cfg_Base);
  if ((LocalMsrRegister & BIT0) != 0) {
    *MmioAddress = LocalMsrRegister & 0xFFFFFFFFFFF00000;
    EncodedSize = (UINT32) ((LocalMsrRegister & 0x3C) >> 2);
    *MmioSize = ((1 << EncodedSize) * 0x100000);
    MmioIsEnabled = TRUE;
  }
  return MmioIsEnabled;
}

/**
 * Read PCI config space
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] PciAddress    Pci address
 * @param[in] Value         Pointer to data
 *
 */
VOID
AmiLibHygonPciRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
     OUT   VOID *Value
  )
{
  UINT32 LegacyPciAccess;
  UINT32 MMIOSize;
  UINT64 RMWrite;
  UINT64 RMWritePrevious;
  UINT64 MMIOAddress;


  ASSERT (PciAddress.AddressValue != ILLEGAL_SBDFO);
  if (!GetPciMmioAddress (&MMIOAddress, &MMIOSize)) {
    // We need to convert our "portable" PCI address into a "real" PCI access
    LegacyPciAccess = ((1 << 31) + (PciAddress.Address.Register & 0xFC) + (PciAddress.Address.Function << 8) + (PciAddress.Address.Device << 11) + (PciAddress.Address.Bus << 16) + ((PciAddress.Address.Register & 0xF00) << (24 - 8)));
    if (PciAddress.Address.Register <= 0xFF) {
      AmiLibHygonIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess);
      AmiLibHygonIoRead (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value);
    } else {
      RMWritePrevious = ReadMsr(NB_CFG);
      RMWrite = RMWritePrevious | 0x0000400000000000;
      WriteMsr(NB_CFG, &RMWrite);
      AmiLibHygonIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess);
      AmiLibHygonIoRead (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value);
      WriteMsr(NB_CFG, &RMWritePrevious);
    }
  } else {
    // Setup the MMIO address
    ASSERT ((MMIOAddress + MMIOSize) > (MMIOAddress + (PciAddress.AddressValue & 0x0FFFFFFF)));
    MMIOAddress += (PciAddress.AddressValue & 0x0FFFFFFF);
    AmiLibHygonMemRead (AccessWidth, MMIOAddress, Value);
  }
}

/**
 * Write PCI config space
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] PciAddress    Pci address
 * @param[in] Value         Pointer to data
 *
 */
VOID
AmiLibHygonPciWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
  IN       VOID *Value
  )
{
  UINT32 LegacyPciAccess;
  UINT32 MMIOSize;
  UINT64 RMWrite;
  UINT64 RMWritePrevious;
  UINT64 MMIOAddress;

  ASSERT (PciAddress.AddressValue != ILLEGAL_SBDFO);
  if (!GetPciMmioAddress (&MMIOAddress, &MMIOSize)) {
    // We need to convert our "portable" PCI address into a "real" PCI access
    LegacyPciAccess = ((1 << 31) + (PciAddress.Address.Register & 0xFC) + (PciAddress.Address.Function << 8) + (PciAddress.Address.Device << 11) + (PciAddress.Address.Bus << 16) + ((PciAddress.Address.Register & 0xF00) << (24 - 8)));
    if (PciAddress.Address.Register <= 0xFF) {
      AmiLibHygonIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess);
      AmiLibHygonIoWrite (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value);
    } else {
      RMWritePrevious = ReadMsr(NB_CFG);
      RMWrite = RMWritePrevious | 0x0000400000000000;
      WriteMsr(NB_CFG, &RMWrite);
      AmiLibHygonIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess);
      AmiLibHygonIoWrite (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value);
      WriteMsr(NB_CFG, &RMWritePrevious);
    }
  } else {
    // Setup the MMIO address
    ASSERT ((MMIOAddress + MMIOSize) > (MMIOAddress + (PciAddress.AddressValue & 0x0FFFFFFF)));
    MMIOAddress += (PciAddress.AddressValue & 0x0FFFFFFF);
    AmiLibHygonMemWrite (AccessWidth, MMIOAddress, Value);
  }
}

UINT32
AmiPspLibPciReadConfig (
  IN  UINT32 Register
  )
{
  UINT32 Value;
  PCI_ADDR PciAddr;

  Value = 0;
  PciAddr.AddressValue = MAKE_SBDFO (0, GET_PCI_BUS (Register), GET_PCI_DEV (Register), GET_PCI_FUNC (Register), GET_PCI_OFFSET (Register));

  AmiLibHygonPciRead (AccessWidth32, PciAddr, &Value);

  return Value;
}

VOID
AmiPspLibPciWriteConfig (
  IN  UINT32 Register,
  IN  UINT32 Value
  )
{
  PCI_ADDR PciAddr;
  PciAddr.AddressValue = MAKE_SBDFO (0, GET_PCI_BUS (Register), GET_PCI_DEV (Register), GET_PCI_FUNC (Register), GET_PCI_OFFSET (Register));

  AmiLibHygonPciWrite (AccessWidth32, PciAddr, &Value);

}

UINT32
AmiPspLibPciReadPspConfig (
  IN UINT16 Offset
  )
{
  return (AmiPspLibPciReadConfig  ((UINT32) (PSP_PCI_BDA  + Offset)));
}

VOID
AmiPspLibPciWritePspConfig (
  IN UINT16 Offset,
  IN  UINT32 Value
  )
{
  AmiPspLibPciWriteConfig  ((UINT32) (PSP_PCI_BDA  + Offset), Value);
}

/**
  Check if PSP device is present

  @retval BOOLEAN  0: PSP Disabled, 1: PSP Enabled

**/
BOOLEAN
CheckPspDevicePresent (
  VOID
  )
{
  UINT32 SecureFuseReg;
  AmiPspLibPciWriteConfig ( (UINT32)PCI_CONFIG_SMU_INDIRECT_INDEX, (UINT32)SMU_CC_PSP_FUSES_STATUS);
  SecureFuseReg = AmiPspLibPciReadConfig ( (UINT32)PCI_CONFIG_SMU_INDIRECT_DATA);

  if (SecureFuseReg &= SMU_CC_PSP_FUSES_PROTO) {
    return (TRUE);
  }
  return (FALSE);
}

/**
  Return the PspMMIO MMIO location

  @param[in] PspMmio Pointer to Psp MMIO address

  @retval BOOLEAN  0: Error, 1 Success
**/
BOOLEAN
GetPspBar1Addr (
  IN OUT   UINT32 *PspMmio
  )
{

  if (CheckPspDevicePresent () == FALSE) {
    return (FALSE);
  }

  *PspMmio = AmiPspLibPciReadPspConfig  (PSP_PCI_BAR1_REG);

  if ((*PspMmio) == 0xffffffff) {
    return (FALSE);
  }

  return (TRUE);
}

/**
  Return the PspMbox MMIO location


  @retval EFI_STATUS  0: Success, NonZero Error

**/
EFI_STATUS
GetPspMboxLocation (
  IN OUT   PSP_MBOX **PspMbox
  )
{
  UINT32 PspMmio;

  if (GetPspBar1Addr (&PspMmio) == FALSE) {
    return (EFI_UNSUPPORTED);
  }

  *PspMbox = (PSP_MBOX *)( (UINTN)PspMmio + PSP_MAILBOX_BASE);   // PSPMbox base is at offset CP2MSG_28 ie. offset 28*4 = 0x70

  return (EFI_SUCCESS);
}

/**
    Send PSP command to PSP

        
    @param  VOID            MboxBuffer
            MBOX_COMMAND    Cmd

         
    @retval BOOLEAN TRUE    Command success
                    FALSE   Command failed

**/

BOOLEAN
AmiSendPspCommand (
  IN       VOID       *MboxBuffer,
  IN       MBOX_COMMAND       Cmd
  )
{
    UINT32     Command;
    BOOLEAN    Status;
    PSP_MBOX   *PspMbox;
    Status = TRUE;

    // Get PspMbox location. fail if not found
    if (EFI_ERROR (GetPspMboxLocation (&PspMbox))) {
      ASSERT (FALSE); // Assertion in the debug build
      return (FALSE);
    }

    // The Command register may be disable by PSP driver when entering D3.
    // Save Command register
    Command = AmiPspLibPciReadPspConfig (PSP_PCI_CMD_REG);
     //Enable BusMaster & MemAccess
    AmiPspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command | 0x6);

    if (PspMbox->MboxSts.Halt) {
      ASSERT (FALSE); // Assertion in the debug build
      //Restore Command register
      AmiPspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command);
      return (FALSE);
    }

    if (PspMbox->MboxSts.Recovery) {
      //Restore Command register
      AmiPspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command);
      return (FALSE);
    }

    // Wait till mailbox is initialized or done processing command
    /// Wait for PSP to be ready. @todo add timeout
    while ( (!PspMbox->MboxSts.MboxInitialized) ||  (PspMbox->MboxCmd)) {
      ;
    }

    // Now send the command
    PspMbox->Buffer = (MBOX_BUFFER *)MboxBuffer;
    PspMbox->MboxCmd = Cmd;

    /// Wait for PSP to be done or reflect error @todo add timeout
    while (PspMbox->MboxCmd) {
      ;
    }

    // error vs. terminated
    if (PspMbox->MboxSts.Error || PspMbox->MboxSts.Terminated) {
      Status = FALSE;
    }
    //Restore Command register
    AmiPspLibPciWritePspConfig (PSP_PCI_CMD_REG, Command);
    return Status;
}

/**
    This elink is to provide a call for sending Psp Mbox message
    for a Sx notification.

    @param VOID

    @retval VOID

**/

VOID
Psp_S3WarmResetCallback(VOID)
{
    MBOX_SX_BUFFER    *Buffer;
    UNALIGNED_MBOX_BUFFER   UBuff;    // Unaligned buffer
    UINT8               Value8;

    // Set a flag here for the indication of capsule
    IoWrite8(0x72, PcdGet8(PcdCmosAgesaSpecified));
    Value8 = IoRead8(0x73);
    Value8 |= BIT6;
    IoWrite8(0x73, Value8);

    Buffer = BALIGN32 (&UBuff);

    Buffer->Header.TotalSize = sizeof (MBOX_SX_BUFFER);

    Buffer->Req.SleepType = 3;

    AmiSendPspCommand (Buffer, MboxBiosCmdSxInfo);
}


//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
