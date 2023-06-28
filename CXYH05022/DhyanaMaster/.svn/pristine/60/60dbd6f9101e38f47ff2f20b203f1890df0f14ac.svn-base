//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file IpmiRedirFru.h
    IPMI Redir FRU functions.

**/

#ifndef _EFI_IPMI_REDIR_FRU_H_
#define _EFI_IPMI_REDIR_FRU_H_

//----------------------------------------------------------------------

#include "Token.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Include/IpmiNetFnAppDefinitions.h>
#include <Include/IpmiNetFnStorageDefinitions.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/GenericFruProtocol.h>
#include <Protocol/RedirFruProtocol.h>
#include <Include/Fru.h>
#include <Include/IpmiBmcCommon.h>

//----------------------------------------------------------------------

//
// Macro definitions
//

#define  CHASSIS_TYPE_LENGTH            1
#define  CHASSIS_TYPE_OFFSET            2 
#define  CHASSIS_PART_NUMBER            3 

#define  PRODUCT_MFG_OFFSET             3 
#define  BOARD_MFG_OFFSET               6 

#define  MANAGEMENT_ACCESS_RECORD       0x03
#define  SYSTEM_UUID_SUB_RECORD_TYPE    0x07
#define  RECORD_CHECKSUM_SIZE           sizeof(UINT8)

#define  FRU_DATA_LENGTH_MASK           0x3F
#define  FRU_DATA_TYPE_MASK             0xC0

#define  IPMI_FRU_COMMAND_RETRY_DELAY      30000
//<Kangmm001-20190828 fix Mantis 0032076 DMI fru information lost during AC Power cycle test +>
//#define  IPMI_FRU_COMMAND_RETRY_COUNTER    3
#define  IPMI_FRU_COMMAND_RETRY_COUNTER    32
//<Kangmm001-20190828 fix Mantis 0032076 DMI fru information lost during AC Power cycle test ->
//<lvych001-20160908 hardware monitor +>
#if defined(GET_SDR_DATA_STYLE) && (GET_SDR_DATA_STYLE == 2)
typedef struct _EFI_IPMI_SENSOR_DATABASE_STRUCT {
  IPMI_SENSOR_RECORD_STRUCT *Sdr;
  UINT8                     RecordType;        
} EFI_IPMI_SENSOR_DATABASE_STRUCT;

typedef struct _EFI_IPMI_SENSOR_PROTOCAL	EFI_IPMI_SENSOR_PROTOCAL;

typedef EFI_STATUS (*EFI_GET_IPMI_DATA_BASE)(void* Buffer,UINT16* SensorNum);

struct _EFI_IPMI_SENSOR_PROTOCAL {
  EFI_GET_IPMI_DATA_BASE    GetIpmiSensorData;
};

EFI_STATUS GetIpmiSensorData(void* Buffer,UINT16* SensorNum);
#endif
//<lvych001-20160908 hardware monitor ->

EFI_STATUS
EFIAPI
EfiGetFruRedirData (
  IN     EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN     UINTN                                FruSlotNumber,
  IN     UINTN                                FruDataOffset,
  IN OUT UINTN                                *FruDataSize,
  IN OUT UINT8                                *FruData
  );

VOID
GenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This,
  IN BOOLEAN                              GenerateDummySmbiosRecords
  );

EFI_STATUS
UpdateFruSmbiosTables (
  VOID
  );

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
