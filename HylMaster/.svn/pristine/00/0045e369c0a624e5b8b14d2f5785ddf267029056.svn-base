/*****************************************************************************
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
 ******************************************************************************
 */


#ifndef __ASM1061R_DRIVER__H__
#define __ASM1061R_DRIVER__H__
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/ComponentName.h>
#include <Protocol/DevicePath.h>
#include <protocol/DriverBinding.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define MY_CR(record, TYPE, field, signature) _CR(record, TYPE, field)
#define ASM1061RIDE_CONTROLLER_PRIVATE_DATA_FROM_THIS(a) MY_CR(a, ASM1061R_IDE_CONTROLLER_PRIVATE_DATA, IdeInit, CONTROLLER_SIGNATURE)
#define CONTROLLER_MAX_CHANNELS         0x2
#define CONTROLLER_MAX_DEVICES          0x2
#define SATA_CONTROLLER_PROTOCOL_GUID \
    { 0x2acb6627, 0xdf02, 0x4e23, 0xb4, 0xf9, 0x6a, 0x93, 0xfa, 0x6e, 0x9d, 0xa6 }

#define CONTROLLER_DRIVER_GUID      SATA_CONTROLLER_PROTOCOL_GUID
#define IDE_ENUMER_ALL              FALSE
#define CONTROLLER_SIGNATURE        EFI_SIGNATURE_32('S','A','T','A')


#define ASM1061RVendorID  0x1B21
#define ASM1061RDeviceID  0x0621
#define ASM1061RRaidDeviceID 0x0622
#define ASM1061RDIDVID    ((ASM1061RDeviceID << 16) | ASM1061RVendorID) //0x06211B21

#define ASM1061R_MMIO_INDEX       0xD0      //PCI offset 0xD0 ~ 0xD3: Index
#define ASM1061R_MMIO_DATA        0xD4     //PCI offset 0xD4 ~ 0xD7: Data


typedef struct _ASM1061R_IDE_CONTROLLER_PRIVATE_DATA{
  // Standard signature used to identify Ide controller private data
  //
  UINT32                     Signature;

  //
  // Protocol instance of IDE_CONTROLLER_INIT produced by this driver
  //
  EFI_IDE_CONTROLLER_INIT_PROTOCOL    IdeInit;

  //
  // copy of protocol pointers used by this driver
  //
  EFI_PCI_IO_PROTOCOL        *PciIo;

  //
  // The highest disqulified mode for each attached Ide device.
  // Per ATA/ATAPI spec, if a mode is not supported, the modes higher than
  // it should not be supported
  //
  EFI_ATA_COLLECTIVE_MODE DisqulifiedModes[CONTROLLER_MAX_CHANNELS][CONTROLLER_MAX_DEVICES];

  //
  // A copy of IDENTIFY data for each attached Ide device and its flag
  //
  EFI_IDENTIFY_DATA IdentifyData[CONTROLLER_MAX_CHANNELS][CONTROLLER_MAX_DEVICES];
  BOOLEAN  IdentifyValid[CONTROLLER_MAX_CHANNELS][CONTROLLER_MAX_DEVICES];
}ASM1061R_IDE_CONTROLLER_PRIVATE_DATA;


EFI_STATUS
EFIAPI
ASM1061RIdeControllerSupported (
    IN EFI_DRIVER_BINDING_PROTOCOL *This,
    IN EFI_HANDLE                  Controller,
    IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
);

EFI_STATUS
EFIAPI
ASM1061RIdeControllerStart (
    IN EFI_DRIVER_BINDING_PROTOCOL    *This,
    IN EFI_HANDLE                     Controller,
    IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
);
EFI_STATUS
EFIAPI
ASM1061RIdeControllerStop (
    IN  EFI_DRIVER_BINDING_PROTOCOL     *This,
    IN  EFI_HANDLE                      Controller,
    IN  UINTN                           NumberOfChildren,
    IN  EFI_HANDLE                      *ChildHandleBuffer
);
EFI_STATUS
ASM1061RCalculateBestPioMode (
    IN  EFI_IDENTIFY_DATA    *IdentifyData,
    IN  UINT16               *DisPioMode OPTIONAL,
    OUT UINT16               *SelectedMode
);
EFI_STATUS
ASM1061RCalculateBestUdmaMode (
    IN  EFI_IDENTIFY_DATA    *IdentifyData,
    IN  UINT16               *DisUDmaMode OPTIONAL,
    OUT UINT16               *SelectedMode
);

EFI_STATUS
IdeControllerGetDriverName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **DriverName
);

EFI_STATUS
IdeControllerGetControllerName (
    IN  EFI_COMPONENT_NAME2_PROTOCOL *This,
    IN  EFI_HANDLE                   ControllerHandle,
    IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
    IN  CHAR8                        *Language,
    OUT CHAR16                       **ControllerName
);

VOID
ASM1061RSetAhciClass (
    EFI_PCI_IO_PROTOCOL   *PciIo
    );

VOID
ASM1061RSpinUp (
    IN EFI_PCI_IO_PROTOCOL      *PciIo
    );

VOID
ASM1061RDisableSafetyRemoval (
    IN EFI_PCI_IO_PROTOCOL      *PciIo
    );

VOID
ASM1061RDisableDeviceSleep (
    IN EFI_PCI_IO_PROTOCOL      *PciIo
    );

VOID
SetPciBridgerBusNum (
    IN EFI_PCI_IO_PROTOCOL      *PciIo,
    IN UINT8                    PciBusNum );

EFI_STATUS
GetLegCapStrucAddr (
    IN EFI_PCI_IO_PROTOCOL      *PciIo,
    IN UINT8                    FindCapNo,
    OUT UINT8                   *CapPtr8 );

EFI_STATUS
ASM1061RInit (
    EFI_PCI_IO_PROTOCOL   *PciIo
);
#endif

