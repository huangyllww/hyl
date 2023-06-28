/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 **/

#ifndef _AMD_PSP_BASELIB_V1_H_
#define _AMD_PSP_BASELIB_V1_H_

#include <AmdPspDirectory.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define PSP_PCI_SEG                     0x00          ///< PSP Seg address
#define PSP_PCI_BUS                     0x00          ///< PCI bus number for PSP
#define PSP_PCI_DEV                     0x08          ///< PCI device number for PSP
#define PSP_PCI_FUNC                    0x00          ///< PCI function number for PSP

#define PSP_PCI_DEVID_REG           0x00    ///< DevId
#define PSP_PCI_CMD_REG             0x04    ///< CmdReg
#define PSP_PCI_BAR1_REG            0x18    ///< Pci Bar1
#define PSP_PCI_BAR3_REG            0x20    ///< Pci Bar3
#define PSP_PCI_MIRRORCTRL1_REG     0x44    ///< PSP Mirror Reg Ctrl 1
#define PSP_PCI_EXTRAPCIHDR_REG     0x48    ///< Extra PCI Header Ctr
#define PSP_PCI_HTMSICAP_REG        0x5C    ///<  HT MSI Capability

#define PSP_MSR_PRIVATE_BLOCK_BAR   0xC00110A2ul ///< PSP Private Block Base Address (PSP_ADDR)

#define D8F0x44_PmNxtPtrW_MASK                                  0xff


#define PSP_MAILBOX_BASE            0x70    ///< Mailbox base offset on PCIe BAR
#define PSP_MAILBOX_STATUS_OFFSET   0x4     ///< Staus Offset


#define PSP_SMM_COMMUNICATION_TYPE_S3SCRIPT       0x1     ///< PspCommunicationType for S3 script
#pragma pack (push, 1)

/// PSP communication structure for S3SCRIPT
typedef struct {
  UINT64    PspBar3PciAddr;       /// PCI address for PSP BAR3
  UINT32    PspBar3Val;           /// PCI register value  for PSP BAR3
  UINT64    Pspx48PciAddr;        /// PCI address for PSP Register 48
  UINT32    Pspx48Val;            /// PCI register value  for PSP Register 48
} PSP_SMM_COMMUNICATION_S3SCRIPT;

/// Union for PSP_SMM_COMMUNICATE_DATA
typedef union _PSP_SMM_COMMUNICATE_DATA {
  PSP_SMM_COMMUNICATION_S3SCRIPT S3Script;      ///< S3Script
} PSP_SMM_COMMUNICATE_DATA;

/// PSP communication header
typedef struct {
  UINT32                       PspCommunicationType;         ///< Type of smm communication buffer
  PSP_SMM_COMMUNICATE_DATA     Data;                      ///< Communication buffer
} PSP_SMM_COMMUNICATION_BUFFER;

//======================================================================================
//
// Define Mailbox Status field
//
//======================================================================================
//
/// MBox Status MMIO space
///
typedef struct {
  UINT32 MboxInitialized:1;   ///< Target will set this to 1 to indicate it is initialized  (for ex. PSP/TPM ready)
  UINT32 Error:1;             ///< Target in adddtion to Done bit will also set this bit to indicate success/error on last command
  UINT32 Terminated:1;        ///< Target will set this bit if it aborted the command due to abort request
  UINT32 Halt:1;              ///< Target will set this error if there is critical error that require reset etc
  UINT32 Recovery:1;          ///< Target will set this error if some PSP entry point by PSP directory has been corrupted.
  UINT32 Reserved:27;          ///< Reserved
} MBOX_STATUS;

//
// Above defined as bitmap
#define MBOX_STATUS_INITIALIZED       0x00000001ul    ///< Mailbox Status: Initialized
#define MBOX_STATUS_ERROR             0x00000002ul    ///< Mailbox Status: Error
#define MBOX_STATUS_ABORT             0x00000004ul    ///< Mailbox Status: Abort
#define MBOX_STATUS_HALT              0x00000008ul    ///< Mailbox Status: Halt
#define MBOX_STATUS_RECOVERY          0x00000010ul    ///< Mailbox Status: Recovery required

//======================================================================================
//
// Define PSP Directory Structure
//
//======================================================================================

#define FIRMWARE_TABLE_SIGNATURE  0x55AA55AAul
/// Define the structure OEM signature table
typedef struct _FIRMWARE_ENTRY_TABLE {
  UINT32  Signature;    ///< Signature should be 0x55AA55AAul
  UINT32  ImcRomBase;   ///< Base Address for Imc Firmware
  UINT32  GecRomBase;   ///< Base Address for Gmc Firmware
  UINT32  XHCRomBase;   ///< Base Address for XHCI Firmware
  UINT32  PspDirBase;   ///< Base Address for PSP directory
  UINT32  NewPspDirBase;   ///< Base Address of PSP directory from program start from ST
} FIRMWARE_ENTRY_TABLE;



#define PSP_ENTRY_PROGRAM_ID_0             0                  ///< Program identifier 0, used when two programs share the same root key
#define PSP_ENTRY_PROGRAM_ID_1             1                  ///< Program identifier 1, used when two programs share the same root key

#define PSP_ENTRY_CZ_PROGRAM_ID            PSP_ENTRY_PROGRAM_ID_0       ///< CZ Program identifier
#define PSP_ENTRY_BR_PROGRAM_ID            PSP_ENTRY_PROGRAM_ID_1       ///< BR Program identifier



#pragma pack (pop)

//======================================================================================
//
// Declare the function prototype
//
//======================================================================================

UINT32
PspLibPciReadPspConfig (
  IN UINT16 Offset
  );

VOID
PspLibPciWritePspConfig (
  IN UINT16 Offset,
  IN UINT32 Value
  );

BOOLEAN
PSPEntryInfoV1 (
  IN      PSP_DIRECTORY_ENTRY_TYPE    EntryType,
  IN OUT  UINT64                      *EntryAddress,
  IN      UINT32                      *EntrySize
  );

BOOLEAN
GetPspEntryScsBinaryV1 (
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  );

BOOLEAN
GetPspEntryPspNvDataV1 (
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  );

BOOLEAN
CheckPspDevicePresentV1 (
  VOID
  );

BOOLEAN
CheckPspPlatformSecureEnableV1 (
  VOID
  );

BOOLEAN
CheckPspRecoveryFlagV1 (
  VOID
  );

BOOLEAN
GetPspBar3Addr (
  IN OUT   UINT32 *PspMmio
  );


BOOLEAN
GetPspMboxStatus (
  IN OUT   MBOX_STATUS **MboxStatus
  );


BOOLEAN
PspBarInitEarly ();


typedef
BOOLEAN
(* FP_CONDITIONER) (
  IN       VOID        *Context
  );

BOOLEAN
PspLibTimeOutV1 (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  );

BOOLEAN
GetFtpmControlAreaV1 (
  IN OUT   VOID **FtpmControlArea
  );

VOID
SwitchPspMmioDecodeV1 (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  );

VOID
AcquirePspSmiRegMutexV1 (
  VOID
  );

VOID
ReleasePspSmiRegMutexV1 (
  VOID
  );

#endif // _AMD_PSP_BASELIB_V1_H_
