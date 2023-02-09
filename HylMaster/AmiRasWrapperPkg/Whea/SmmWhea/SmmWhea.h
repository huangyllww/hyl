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
  Contains structures for Whea related AcpiTables in SMM

*/

#ifndef _EFI_SMM_WHEA_H
#define _EFI_SMM_WHEA_H

#include <Include/WheaInclude.h>

#define  ACPI_REV1       0x01

//Whea Instruction definition

#define WHEA_READ_REG           0x0
#define WHEA_READ_REG_VALUE     0x1
#define WHEA_WRITE_REG          0x2
#define WHEA_WRITE_REG_VALUE    0x3
#define WHEA_NOOP               0x4

#define WHEA_ACCESS_BYTE        0x1
#define WHEA_ACCESS_WORD        0x2
#define WHEA_ACCESS_DWORD       0x3
#define WHEA_ACCESS_QWORD       0x4

#define WHEA_ENTRY_ADDRID_SYSMEM    0x0
#define WHEA_ENTRY_ADDRID_SYSIO     0x1
#define WHEA_ENTRY_ADDRID_RES       0x0

//Register bit width

#define REG_BIT_WIDTH_BYTE      8
#define REG_BIT_WIDTH_WORD      16
#define REG_BIT_WIDTH_DWORD     32
#define REG_BIT_WIDTH_QWORD     64

//Register bit offset

#define REG_BIT_OFFSET0         0
#define SOURCEID_OFFSET         0x02

//FLAGS

#define WHEA_CLEAR_FLAG         0
#define WHEA_SET_FLAG           1

//Reserved

#define WHEA_RESERVED           0

//Mask

#define WHEA_BYTE_MASK           0xff
#define WHEA_WORD_MASK           0xffff
#define WHEA_DWORD_MASK          0xffffffff
#define WHEA_QWORD_MASK          0xffffffffffffffff
#define WHEA_CHK_BUSY_STS_MASK   0x01
#define WHEA_GET_CMD_STS_MASK    0xfffe

#pragma pack(1)

// ACPI 3.0 Generic Address Space definition

typedef struct _EFI_ACPI_3_0_GAS{
  UINT8   AddressSpaceId;
  UINT8   RegisterBitWidth;
  UINT8   RegisterBitOffset;
  UINT8   AccessSize;
  UINT64  Address;
} EFI_ACPI_3_0_GAS;

// WHEA ERST ACPI TAble

typedef struct _WHEA_SERIALIZATION_HEADER {
  UINT32   InjectionHeaderSize;
  UINT32   Reserved;
  UINT32   InjectionInstEntryCount;
} WHEA_SERIALIZATION_HEADER;

typedef struct _WHEA_SERIALIZATION_INST_ENTRY {
  UINT8             SerializationAction;
  UINT8             Instruction;
  UINT8             Flags;
  UINT8             Reserved;
  EFI_ACPI_3_0_GAS  RegisterRegion;
  UINT64            Value;
  UINT64            Mask;
} WHEA_SERIALIZATION_INST_ENTRY;

#define WHEA_ERST_INSTR_ENTRIES 16

typedef struct _WHEA_ERST_ACPI_TABLE{
   EFI_ACPI_DESCRIPTION_HEADER   AcpiHeader;
  WHEA_SERIALIZATION_HEADER      SerializationHeader;
  WHEA_SERIALIZATION_INST_ENTRY  SerializationInstEntry[WHEA_ERST_INSTR_ENTRIES];
} WHEA_ERST_ACPI_TABLE;

// WHEA Private Data

typedef struct _WHEA_MODULE_PRIVATE_DATA{
  VOID	                          *ErrorLogAddressRange;
  VOID                             *ErrorStatusBlock;
  VOID                            *PersistentStoreBuffer;
  WHEA_ERST_ACPI_TABLE            *Erst;
  BOOLEAN                         ErstEnabled;
} WHEA_MODULE_PRIVATE_DATA;

typedef struct _WHEA_INTERFACE {
      UINTN                       Signature;
      WHEA_MODULE_PRIVATE_DATA    *WheaData;
      EFI_WHEA_PROTOCOL           EfiWheaProtocol;
} WHEA_INTERFACE;

#pragma pack ()

VOID
WheaLogErrInfo(
  IN UINT32 ErrorType,
  IN UINT32 ErrorSeverity,
  IN VOID   *ErrData
);

VOID
WheaZeroMemory (
  IN  VOID  *Buffer,
  IN  UINTN Size
 );

#ifndef _CR
#define _CR(Record, TYPE, Field)           ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#endif
#ifndef CR
#define CR(record, TYPE, field, signature) _CR(record, TYPE, field)
#endif
#ifndef EFI_SIGNATURE_16
#define EFI_SIGNATURE_16(A, B)             ((A) | (B << 8))
#endif
#ifndef EFI_SIGNATURE_32
#define EFI_SIGNATURE_32(A, B, C, D)       (EFI_SIGNATURE_16 (A, B) | (EFI_SIGNATURE_16 (C, D) << 16))
#endif

#define EFI_WHEA_INTERFACE_SIGNATURE EFI_SIGNATURE_32 ('W', 'H', 'E', 'A')

#define WHEA_INTERFACE_INSTANCE_FROM_THIS(a) \
  CR (a, WHEA_INTERFACE, EfiWheaProtocol, EFI_WHEA_INTERFACE_SIGNATURE)

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
