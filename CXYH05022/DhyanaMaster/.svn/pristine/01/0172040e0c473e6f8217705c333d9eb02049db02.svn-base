/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
*/

/** @file StyxAcpiDebug2Table.c
    File contains function calls for StyxAcpiDebug2Table.c
**/

#ifndef _ACPI_UPDATE_CPU_INFO_H_
#define _ACPI_UPDATE_CPU_INFO_H_

// Include Files
#include "Token.h"
#include <Uefi.h>
#include <AmiDxeLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AcpiTable.h>
#include <IndustryStandard/Acpi.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
//#include <Guid/ArmMpCoreInfo.h>
#include <AmiHobs.h>

#define CREATOR_ID_AMI 0x20494D41  //" IMA""AMI "(AMI)
#define CREATOR_REV 0x00000000
#define ___INTERNAL_CONVERT_TO_STRING___(a) #a
#define CONVERT_TO_STRING(a) ___INTERNAL_CONVERT_TO_STRING___(a)

//#define EFI_ACPI_TABLE_VERSION_X EFI_ACPI_TABLE_VERSION_1_0B |EFI_ACPI_TABLE_VERSION_2_0
//#define EFI_ACPI_TABLE_VERSION_X1 EFI_ACPI_TABLE_VERSION_X|EFI_ACPI_TABLE_VERSION_3_0
//#define EFI_ACPI_TABLE_VERSION_ALL EFI_ACPI_TABLE_VERSION_X1|EFI_ACPI_TABLE_VERSION_4_0

static UINT8   OemId[6] = CONVERT_TO_STRING(T_ACPI_OEM_ID);
static UINT8   OemTableId[8] = CONVERT_TO_STRING(CPUSSDT);

#pragma pack(1)
typedef struct  {
    UINT16        ProcOpcode;//2
    UINT8         ProcPkgLen;//3
    UINT8         ProcNameStr[4];//7
    UINT8         ProcID;//8
    UINT8         PblkAddr[4];//12
    UINT8         PblkLen;//13
}PROC_TEMPLATE;
#pragma pack()

///SB Device Scope Head
#pragma pack(1)
typedef struct _SB_DEV_HEADER {
  UINT16  SbDevOpcode;              ///< Opcode
  UINT8   PkgLength;              ///< Package Length
  UINT8   SbDevName_C;              ///< Name Pointer
  UINT8   SbDevName_P;              ///< Name Pointer
  UINT8   SbDevName_U;              ///< Name Pointer
  UINT8   SbDevName_0;              ///< Name Pointer
  } SB_DEV_HEADER;

///SB Device Scope Body
typedef struct _SB_DEV_BODY {
  UINT8   HidNameOpcode;                   ///< Opcode
  UINT8   HidName_a__;                  ///< String "_"
  UINT8   HidName_a_H;                  ///< String "H"
  UINT8   HidName_a_I;                  ///< String "I"
  UINT8   HidName_a_D;                  ///< String "D"
  UINT8   StringPrefixOpcode;             ///< Prefix Opcode in String
  UINT8   ProcHid_a_A;                  ///< String "A"
  UINT8   ProcHid_a_C;                  ///< String "C"
  UINT8   ProcHid_a_P;                  ///< String "P"
  UINT8   ProcHid_a_I;                  ///< String "I"
  UINT8   ProcHid_a_0;                  ///< String "0"
  UINT8   ProcHid_b_0;                  ///< String "0"
  UINT8   ProcHid_c_0;                  ///< String "0"
  UINT8   ProcHid_a_7;                  ///< String "7"
  UINT8   NullChar;                     ///< NULL Char
  
  UINT8   UidNameOpcode;                   ///< Opcode
  UINT8   UidName_a__;                  ///< String "_"
  UINT8   UidName_a_U;                  ///< String "U"
  UINT8   UidName_a_I;                  ///< String "I"
  UINT8   UidName_a_D;                  ///< String "D"
  UINT8   BytePrefixOpcode;             ///< Prefix Opcode in Byte
  UINT8   ApicId;
  } SB_DEV_BODY;
#pragma pack()
  
#define   SBDEVH_STRUCT_SIZE sizeof (SB_DEV_HEADER)
#define   SBDEVB_STRUCT_SIZE sizeof (SB_DEV_BODY)
 
#define     SCOPE_NAME_0    		'0'
#define     SCOPE_NAME_A    		'A'
#define     HID_NAME__                  '_'
#define     HID_NAME_H                  'H'
#define     HID_NAME_I                  'I'
#define     HID_NAME_D                  'D'
#define     PROC_HID_A                  0x41
#define     PROC_HID_C                  0x43
#define     PROC_HID_P                  0x50
#define     PROC_HID_I                  0x49
#define     PROC_HID_0                  0x30
#define     PROC_HID_7                  0x37
#define     UID_NAME__                  '_'
#define     UID_NAME_U                  'U'
#define     UID_NAME_I                  'I'
#define     UID_NAME_D                  'D'

//Function Prototypes

/** 
* Fill the SSDT ACPI Table Header.
*
*   @param  pAcpiHdr    Pointer to ACPI Header
*   
*   @param  Signature   Signature of the ACPI Table
*   
*   @param TblLength     Length of the ACPI Table
* 
*/
VOID 
FillAcpiHeader(
        EFI_ACPI_DESCRIPTION_HEADER *pAcpiHdr, 
        UINT32 Signature, 
        UINTN TblLength
  );

/** 
* Set Processor name string
*
*   @param  ProcNameStr    Pointer to the String location
*   
*   @param  CpuNumber      Cpu id number
*   
* 
*/
VOID 
SetProcString(
        UINT8 *ProcNameStr, 
        UINT8 CpuNumber
  );

/** 
* calculates the checksum of a table starting at TblStart of length 
* BytesCount and returns the checksum value.
*
*   @param  TblStart    Starting address of the memory area to checksum
*   
*   @param  BytesCount  Length in bytes of the memory area to checksum
*   
*   @retval UINT8       value starting from TblStart and ending at 
*                       TblStart + BytesCount.
* 
*/
UINT8 
ChsumTbl(
        UINT8* TblStart, 
        UINT32 BytesCount
  );

#endif
/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
*/
