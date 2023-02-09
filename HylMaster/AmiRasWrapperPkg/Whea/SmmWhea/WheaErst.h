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


/** @file
  Contains structures,function definitions used by ERST
*/

#ifndef _EFI_ERST_H
#define _EFI_ERST_H

#include "SmmWheaInit.h"
#include <Protocol/FlashProtocol.h>

#define ERST_SIG 0x54535245

// Error Record Serailization Action Definition
//
#define ERST_BEGIN_WRITE_OPERATION        0x0
#define ERST_BEGIN_READ_OPERATION         0x1
#define ERST_BEGIN_CLEAR_OPERATION        0x2
#define ERST_END_OPERATION                0x3
#define ERST_SET_RECORD_OFFSET            0x4
#define ERST_EXECUTE_OPERATION            0x5
#define ERST_CHECK_BUSY_STATUS            0x6
#define ERST_GET_OPERATION_STATUS         0x7
#define ERST_GET_RECORD_IDENTIFIER        0x8
#define ERST_SET_RECORD_IDENTIFIER        0x9
#define ERST_GET_RECORD_COUNT             0xA
#define ERST_BEGIN_DUMMY_WRITE_OPERATION  0xB
#define ERST_RESERVED                     0xC
#define ERST_GET_ERROR_LOG_ADDRESS_RANGE  0xD
#define ERST_GET_ERROR_LOG_ADDRESS_LENGTH 0xE
#define ERST_GET_ERROR_LOG_ADDRESS_ATTR   0xF

// ERST Command Status Definition
//
#define ERST_SUCCESS                    0x0
#define ERST_NOT_ENOUGH_SPACE           0x1
#define ERST_HARDWARE_NOT_AVAILABLE     0x2
#define ERST_FAILED                     0x3
#define ERST_RECORD_STORE_EMPTY         0x4
#define ERST_RECORD_NOT_FOUND           0x5

//Serialization Instruction Definition
//
#define ERST_READ_REGISTER              0x0
#define ERST_READ_REGISTER_VALUE        0x1
#define ERST_WRITE_REGISTER             0x2
#define ERST_WRITE_REGISTER_VALUE       0x3
#define ERST_NOOP                       0x4

//ERST Serialization Action Register Region of memory location definition
//
#define ERST_END_OPER_OFFSET            1
#define ERST_SET_REC_OFFSET             2
#define ERST_CHK_BUSY_OFFSET            6
#define ERST_GET_OPER_STATUS_OFFSET     7
#define ERST_GET_REC_IDENT_OFFSET       8
#define ERST_SET_REC_IDENT_OFFSET       8
#define ERST_GET_REC_COUNT_OFFSET       16
#define ERST_GET_MEM_OFFSET             18
#define ERST_GET_MEM_SIZE_OFFSET        26
#define ERST_GET_MEM_ATTRIBUTES_OFFSET  30

#define ERST_PERSISTENT_STORE_GUID \
  { 0x193A3779, 0xA2E3, 0x4f08, 0xB4, 0xAB, 0x43, 0x99, 0x8E, 0x55, 0x02, 0xFF }

enum {
  WHEA_PREPEND_MAGIC_NUMBER = 0x44332211,
  WHEA_APPEND_MAGIC_NUMBER = 0x11223344
};

enum {
  FLASH_ERASE_VALUE = 0xFF,
  MARKED_AS_DELETE = 0x00
};

#pragma pack(1)

//Defined in UEFI spec
//
typedef struct _UEFI_ERROR_RECORD_HEADER {
  UINT32    SignatureStart;
  UINT16    Revision;
  UINT32    SignatureEnd;
  UINT16    SectionCount;
  UINT32    ErrSeverity;
  UINT32    ValidBits;
  UINT32    RecLength;
  UINT64    Timestamp;
  EFI_GUID  PlatformID;
  EFI_GUID  PartitionID;
  EFI_GUID  CreatorID;
  EFI_GUID  NotifiType;
  UINT64    RecordID;
  UINT32    Flags;
  UINT64    OSReserved;
  UINT8     Reserved[12];
} UEFI_ERROR_RECORD_HEADER;

//Header of Error Record in GPNV area
//
typedef struct _NV_PREPEND_DATA {
  UINT32      MagicNum;
  UINT8       RecordValidity;   //if status = ERASE_VALUE, then valid record
} NV_PREPEND_DATA;

//Footer of Error Record in GPNV area
//
typedef struct _NV_APPEND_DATA {
  UINT32      MagicNum;
} NV_APPEND_DATA;	

typedef struct {
  UINT16      Signature;
  UINT32      Reserved1;
  UINT8       Reserved2;
  UINT8       Attributes; 
} ERROR_RECORD_SERIALIZATION_INFO;

typedef struct _PERSISTENT_ERROR_RECORD {
  NV_PREPEND_DATA NvPrependData;
  UEFI_ERROR_RECORD_HEADER UefiErrorRecord;
} PERSISTENT_ERROR_RECORD;

#define PS_PREPEND_DATA_SIZE sizeof(NV_PREPEND_DATA)
#define PS_APPEND_DATA_SIZE sizeof(NV_APPEND_DATA)
 
#pragma pack()

//These are the registers that will be read/written by windows
//
typedef struct _WHEA_ERST_REGISTERS{
  UINT8         CmdReg;
  UINT8         EndOperation;
  UINT32        RecordOffset;
  UINT8         BusyStatus;
  UINT8         OperationStatus;
  UINT64        ValidRecordIdentifier; //firmware stores a record identifier which is available in persistent store
  UINT64        OSRecordIdentifier; //OS tells to firmware which record identifier to be read
  UINT16        RecordCount;
  UINT64        ElarAddress;
  UINT32        ElarLength;
  UINT8         ElarAttributes;
} WHEA_ERST_REGISTERS;

EFI_STATUS 
ErstSWSMIHandler(
  IN EFI_HANDLE                        DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT *RegisterContext,
  IN OUT EFI_SMM_SW_CONTEXT            *SwContext,
  IN OUT UINTN                         *CommBufferSize
);

EFI_STATUS 
WheaErstInit (
  VOID
);

VOID 
BiosWriteAccess(
  IN BOOLEAN  EnableAccess
);

#endif

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
