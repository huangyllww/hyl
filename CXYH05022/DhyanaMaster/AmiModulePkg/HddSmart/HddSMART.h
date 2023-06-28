//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file HddSMART.h
    This file contains the Includes, Definitions, typedefs,
    Variable and External Declarations, Structure and
    function prototypes needed for the IdeSMART driver

**/

#ifndef _HDD_SMART_
#define _HDD_SMART_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------------

#include <Token.h>
#include <AmiDxeLib.h>
#include "Protocol/PciIo.h"
#include "Protocol/DevicePath.h"
#include "Protocol/DriverBinding.h"
#include "Protocol/BlockIo.h"
#include <Protocol/AMIPostMgr.h>
#include <Protocol/SimpleTextIn.h>
#include "HddSmartCommon.h"

//---------------------------------------------------------------------------

AMI_HDD_SMART_INIT_PROTOCOL *gHddSmartInitProtocol;

#define     ZeroMemory( Buffer, Size ) pBS->SetMem( Buffer, Size, 0 )

#define     FIS_REGISTER_D2H                0x34        // Device To Host
#define     NVME_IDENTIFY_COMMAND_SIZE      0x1000
#define     RaidStorageInterfaceNvme        0x04
#ifndef     IDENTIFY
#define     IDENTIFY                        0x06
#endif
#ifndef     GET_LOG_PAGE
#define     GET_LOG_PAGE                    0x02
#endif
#ifndef     NVME_COMMAND_TIMEOUT
#define     NVME_COMMAND_TIMEOUT            0x01
#endif
#define     MODEL_NUM_OFFSET                24

#ifndef EFI_SIGNATURE_16
#define EFI_SIGNATURE_16(A, B)             ((A) | (B << 8))
#endif
#ifndef EFI_SIGNATURE_32
#define EFI_SIGNATURE_32(A, B, C, D)  \
                  (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#endif

#define SMART_PROTOCOL_SIGNATURE        EFI_SIGNATURE_32 ('S','M','R','T')

#pragma pack(1)
typedef struct
{
    UINT8   Ahci_Dsfis[0x1C];               // DMA Setup Fis
    UINT8   Ahci_Dsfis_Rsvd[0x04];
    UINT8   Ahci_Psfis[0x14];               // PIO Setup Fis
    UINT8   Ahci_Psfis_Rsvd[0x0C];
    UINT8   Ahci_Rfis[0x14];                // D2H Register Fis
    UINT8   Ahci_Rfis_Rsvd[0x04];
    UINT64  Ahci_Sdbfis;                    // Set Device Bits Fis
    UINT8   Ahci_Ufis[0x40];                // Unknown FIS
    UINT8   Ahci_Ufis_Rsvd[0x60];
} AHCI_RECEIVED_FIS;
#pragma pack()

typedef struct
{
	AMI_STORAGE_BUS_INTERFACE     BusInterfaceType;
	void                          *BusInterface;
} SMART_ERROR_DEVINFO;

UINTN 
EfiValueToString (
    IN OUT CHAR16   *Buffer,
    IN INT64    Value,
    IN UINTN    Flags,
    IN UINTN    Width
);

EFI_STATUS 
GetHiiString (
    IN EFI_HII_HANDLE   HiiHandle,
    IN STRING_REF   Token,
    IN OUT UINTN    *pDataSize,
    OUT EFI_STRING  *ppData
);

EFI_STATUS 
CommonNonDataHook (
    IN VOID                         *BusInterface,
    IN COMMAND_STRUCTURE            CommandStructure,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
);

EFI_STATUS 
EFIAPI
InitSmartSupport (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
);

EFI_STATUS 
SmartReturnStatusWrapper (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType,
    IN OUT BOOLEAN                  *SmartFailure
);

EFI_STATUS 
EFIAPI
InstallSmartInterface (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
);

EFI_STATUS 
EFIAPI
UnInstallSmartInterface (
    IN VOID                         *BusInterface,
    IN AMI_STORAGE_BUS_INTERFACE    BusInterfaceType
);

//---------------------------------------------------------------------------
// SMART DIAGNOSTICS SUPPORT START
//---------------------------------------------------------------------------

EFI_STATUS 
EFIAPI
SmartSelfTest (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    IN UINT8                    SelfTestType,
    OUT UINT16                  *PollPeriod
);

EFI_STATUS 
EFIAPI
SmartSelfTestStatus (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    OUT UINT8                   *TestStatus
);

EFI_STATUS 
EFIAPI
SmartReturnStatus (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    IN OUT BOOLEAN              *SmartFailure
);

EFI_STATUS 
EFIAPI
SendSmartCommand (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    IN UINT8                    SubCommand,
    IN UINT8                    AutoSaveEnable,    //OPTIONAL
    IN UINT8                    SelfTestType       //OPTIONAL
);

EFI_STATUS 
EFIAPI
SmartReadData (
    IN  AMI_HDD_SMART_PROTOCOL  *This,
    IN  UINT8                   SubCommand,
    OUT VOID                    *Buffer,
    IN  UINT8                   LogAddress,   //OPTIONAL
    IN  UINT8                   SectorCount   //OPTIONAL
);

EFI_STATUS 
EFIAPI
SmartWriteData (
    IN AMI_HDD_SMART_PROTOCOL   *This,
    IN UINT8                    SubCommand,
    IN VOID                     *Buffer,
    IN UINT8                    LogAddress,
    IN UINT8                    SectorCount
);

EFI_STATUS 
EFIAPI
SmartAbortSelfTest (
    IN  AMI_HDD_SMART_PROTOCOL  *This
);

EFI_STATUS 
EFIAPI
SmartGetSmartData (
    IN  AMI_HDD_SMART_PROTOCOL  *This,
    OUT UINT8                   **SmartData
);

extern 
CHAR16 
*HiiGetString ( 
    VOID*  handle, 
    UINT16 token 
);

/****** DO NOT WRITE BELOW THIS LINE *******/
#ifdef __cplusplus
}
#endif
#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
