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
// $Header: /Alaska/Tools/template.h 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.h $
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
#ifndef __ASM1061R_DRIVER__H__
#define __ASM1061R_DRIVER__H__
#ifdef __cplusplus
extern "C" {
#endif
#include <Token.h>
#include <AmiDxeLib.h>
#include <Setup.h>
#include <pci.h>
#include <Protocol\PciIo.h>
#include <Protocol\IdeControllerInit.h>
#include <Protocol\ComponentName.h>
#include <Protocol\DevicePath.h>
#include <protocol\DriverBinding.h>
#define EFI_SIGNATURE_16(A, B)        ((A) | (B << 8))
#define EFI_SIGNATURE_32(A, B, C, D)  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define MY_CR(record, TYPE, field, signature) _CR(record, TYPE, field)
#define ASM1061RIDE_CONTROLLER_PRIVATE_DATA_FROM_THIS(a) MY_CR(a, ASM1061R_IDE_CONTROLLER_PRIVATE_DATA, IdeInit, CONTROLLER_SIGNATURE)
#define CONTROLLER_MAX_CHANNELS         0x2
//#define CONTROLLER_MAX_CHANNELS_AHCI    MAX_AHCI_CHANNEL
#define CONTROLLER_MAX_DEVICES          0x2
#define SATA_CONTROLLER_PROTOCOL_GUID \
        { 0x2acb6627, 0xdf02, 0x4e23, 0xb4, 0xf9, 0x6a, 0x93, 0xfa, 0x6e, 0x9d, 0xa6 }

#define CONTROLLER_DRIVER_GUID      SATA_CONTROLLER_PROTOCOL_GUID
#define IDE_ENUMER_ALL              FALSE
#define CONTROLLER_SIGNATURE        EFI_SIGNATURE_32('S','A','T','A')


#define ASM1061RVendorID 0x1B21
#define ASM1061RDeviceID 0x0622

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
  IN  EFI_IDENTIFY_DATA    * IdentifyData,
  IN  UINT16               *DisPioMode OPTIONAL,
  OUT UINT16               *SelectedMode
  );
EFI_STATUS
ASM1061RCalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA    * IdentifyData,
  IN  UINT16               *DisUDmaMode OPTIONAL,
  OUT UINT16               *SelectedMode
  );

EFI_STATUS
IdeControllerGetDriverName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **DriverName
);

EFI_STATUS
IdeControllerGetControllerName (
  IN  EFI_COMPONENT_NAME2_PROTOCOL  *This,
  IN  EFI_HANDLE                   ControllerHandle,
  IN  EFI_HANDLE                   ChildHandle        OPTIONAL,
  IN  CHAR8                        *Language,
  OUT CHAR16                       **ControllerName
  );
/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif
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
