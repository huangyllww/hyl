//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /SandBox/Lenovo_AmiScePkg_Driver/Include/EDKStructures21.h 2     6/26/13 11:15a Arund $
//
// $Revision: 2 $
//
// $Date: 6/26/13 11:15a $
//
//*********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Lenovo_AmiScePkg_Driver/Include/EDKStructures21.h $
// 
// 2     6/26/13 11:15a Arund
// Updated code with PLDM integration and Mapping language
//
// 5     11/22/10 6:06p Sandhyar
// [TAG]  		EIP47074
// [Category]  	New Feature
// [Description]  	SCE password check enabled
// [Files]  		AMISCE.cpp
// Amisce.h
// ioacc.cpp
// oshook.h
// EDKStructures21.h
// FLASH.h
// c_pwdchk.cpp
// c_pwdchk.h
// ofbd_cfg.h
// ofbd_cfg.cpp
// amiscelnx2.mak
// amiscelnx2_64.mak
//
// 4     9/28/10 12:44p Timothym
// Restore changes lost during EFI merge.
//
// 3     9/27/10 5:22p Timothym
// Correct typo.
//
// 2     9/25/10 9:33p Timothym
// EFI shell.
//
//********************************************************************

//<AMI_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:          EDKStructures21.h
//
// Description:   Data structure definitions for UEFI 2.1
//
//----------------------------------------------------------------------------
//<AMI_FHDR_END>
#ifndef INC_EDKStructures21
#define INC_EDKStructures21
#define _EFI 1
#include "types.h"
#if defined(_EFI)
#include "os_hook.h"
#endif
///for UEFI 2.1
#define EFI_HII_PACKAGE_FORM_CONFIG 0x02
#ifndef EFI_IFR_EQ_ID_LIST_OP
#ifdef EFI_IFR_EQ_ID_VAL_LIST_OP
#define EFI_IFR_EQ_ID_LIST_OP EFI_IFR_EQ_ID_VAL_LIST_OP
#else
#define EFI_IFR_EQ_ID_LIST_OP          0x14
#endif
#endif

/*typedef UINT16  EFI_HII_HANDLE;
typedef CHAR16  *EFI_STRING;
typedef UINT16  EFI_QUESTION_ID;
typedef UINT16  EFI_IMAGE_ID;
typedef UINT16  EFI_STRING_ID;
typedef UINT16  EFI_FORM_ID;
typedef UINT16  EFI_VARSTORE_ID;
typedef UINT16  EFI_DEFAULT_ID;
typedef UINT32  EFI_HII_FONT_STYLE;
#ifndef _EFI
typedef GUID	EFI_GUID;
#endif
typedef UINT64	EFI_PHYSICAL_ADDRESS;

//
//String package
//

//
// Type defined for backward compatible with Framework HII
//
typedef EFI_STRING_ID  STRING_REF;

#pragma pack(1)

#define EFI_HII_SIBT_END				0x00
#define EFI_HII_SIBT_STRING_SCSU		0x10
#define EFI_HII_SIBT_STRING_SCSU_FONT	0x11
#define EFI_HII_SIBT_STRINGS_SCSU		0x12
#define EFI_HII_SIBT_STRINGS_SCSU_FONT	0x13
#define EFI_HII_SIBT_STRING_UCS2		0x14
#define EFI_HII_SIBT_STRING_UCS2_FONT	0x15
#define EFI_HII_SIBT_STRINGS_UCS2		0x16
#define EFI_HII_SIBT_STRINGS_UCS2_FONT	0x17
#define EFI_HII_SIBT_DUPLICATE			0x20
#define EFI_HII_SIBT_SKIP2				0x21
#define EFI_HII_SIBT_SKIP1				0x22
#define EFI_HII_SIBT_EXT1				0x30
#define EFI_HII_SIBT_EXT2				0x31
#define EFI_HII_SIBT_EXT4				0x32
#define EFI_HII_SIBT_FONT				0x40

//Offset to lang info in 2.1 string pack header
#define LANG_OFFSET_UEFI21				0x2E

#define EFI_HII_PACKAGE_TYPE_ALL 0x00
#define EFI_HII_PACKAGE_TYPE_GUID 0x01
#define EFI_HII_PACKAGE_FORM_CONFIG 0x02
#define EFI_HII_PACKAGE_FORM_APP 0x03
#define EFI_HII_PACKAGE_STRINGS 0x04
#define EFI_HII_PACKAGE_FONTS 0x05
#define EFI_HII_PACKAGE_IMAGES 0x06
#define EFI_HII_PACKAGE_SIMPLE_FONTS 0x07
#define EFI_HII_PACKAGE_DEVICE_PATH 0x08
#define EFI_HII_PACKAGE_END 0x09
#define EFI_HII_PACKAGE_TYPE_SYSTEM_BEGIN 0xE0
#define EFI_HII_PACKAGE_TYPE_SYSTEM_END 0xFF

typedef struct {
  EFI_GUID               PackageListGuid;
  UINT32                 PackageLength;
} EFI_HII_PACKAGE_LIST_HEADER;

typedef struct {
UINT32 Length:24;
UINT32 Type:8;
//UINT8 *Data; // Changed as it is in EDK 1.05
} EFI_HII_PACKAGE_HEADER;

typedef struct _EFI_HII_STRING_PACKAGE_HDR {
EFI_HII_PACKAGE_HEADER Header;
UINT32 HdrSize;
UINT32 StringInfoOffset;
CHAR16 LanguageWindow[16];
EFI_STRING_ID LanguageName;
CHAR8 Language[6]; // Changed as it is in EDK
} EFI_HII_STRING_PACKAGE_HDR;

typedef struct {
UINT8 BlockType;
//UINT8 BlockBody[];  // Changed as it is in EDK
} EFI_HII_STRING_BLOCK;

typedef struct _EFI_HII_SIBT_DUPLICATE_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  EFI_STRING_ID           StringId;
} EFI_HII_SIBT_DUPLICATE_BLOCK;

typedef struct _EFI_HII_SIBT_END_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
} EFI_HII_SIBT_END_BLOCK;

typedef struct _EFI_HII_SIBT_EXT1_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   BlockType2;
  UINT8                   Length;
} EFI_HII_SIBT_EXT1_BLOCK;

typedef struct _EFI_HII_SIBT_EXT2_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   BlockType2;
  UINT16                  Length;
} EFI_HII_SIBT_EXT2_BLOCK;

typedef struct _EFI_HII_SIBT_EXT4_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   BlockType2;
  UINT32                  Length;
} EFI_HII_SIBT_EXT4_BLOCK;

typedef struct _EFI_HII_SIBT_FONT_BLOCK {
  EFI_HII_SIBT_EXT2_BLOCK Header;
  UINT8                   FontId;
  UINT16                  FontSize;
  EFI_HII_FONT_STYLE      FontStyle;
  CHAR16                  FontName[1];
} EFI_HII_SIBT_FONT_BLOCK;

typedef struct _EFI_HII_SIBT_SKIP1_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   SkipCount;
} EFI_HII_SIBT_SKIP1_BLOCK;

typedef struct _EFI_HII_SIBT_SKIP2_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT16                  SkipCount;
} EFI_HII_SIBT_SKIP2_BLOCK;

typedef struct _EFI_HII_SIBT_STRING_SCSU_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   StringText[1];
} EFI_HII_SIBT_STRING_SCSU_BLOCK;

typedef struct _EFI_HII_SIBT_STRING_SCSU_FONT_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   FontIdentifier;
  UINT8                   StringText[1];
} EFI_HII_SIBT_STRING_SCSU_FONT_BLOCK;

typedef struct _EFI_HII_SIBT_STRINGS_SCSU_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT16                  StringCount;
  UINT8                   StringText[1];
} EFI_HII_SIBT_STRINGS_SCSU_BLOCK;

typedef struct _EFI_HII_SIBT_STRINGS_SCSU_FONT_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   FontIdentifier;
  UINT16                  StringCount;
  UINT8                   StringText[1];
} EFI_HII_SIBT_STRINGS_SCSU_FONT_BLOCK;

typedef struct _EFI_HII_SIBT_STRING_UCS2_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  CHAR16                  StringText[1];
} EFI_HII_SIBT_STRING_UCS2_BLOCK;

typedef struct _EFI_HII_SIBT_STRING_UCS2_FONT_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   FontIdentifier;
  CHAR16                  StringText[1];
} EFI_HII_SIBT_STRING_UCS2_FONT_BLOCK;

typedef struct _EFI_HII_SIBT_STRINGS_UCS2_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT16                  StringCount;
  CHAR16                  StringText[1];
} EFI_HII_SIBT_STRINGS_UCS2_BLOCK;

typedef struct _EFI_HII_SIBT_STRINGS_UCS2_FONT_BLOCK {
  EFI_HII_STRING_BLOCK    Header;
  UINT8                   FontIdentifier;
  UINT16                  StringCount;
  CHAR16                  StringText[1];
} EFI_HII_SIBT_STRINGS_UCS2_FONT_BLOCK;


//
// Forms Package
//

typedef struct _EFI_HII_FORM_PACKAGE {
  EFI_HII_PACKAGE_HEADER       Header;
  // EFI_IFR_OP_HEADER         OpCodeHeader;
  // More op-codes follow
} EFI_HII_FORM_PACKAGE;

typedef struct {
  UINT8 Hour;
  UINT8 Minute;
  UINT8 Second;
} EFI_HII_TIME;

typedef struct {
  UINT16 Year;
  UINT8  Month;
  UINT8  Day;
} EFI_HII_DATE;

typedef union {
  UINT8           u8;
  UINT16          u16;
  UINT32          u32;
  UINT64          u64;
  BOOLEAN         b;
  EFI_HII_TIME    time;
  EFI_HII_DATE    date;
  EFI_STRING_ID   string;
} EFI_IFR_TYPE_VALUE;

#define EFI_SUCCESS	0
#define EFI_IFR_FORM_OP                0x01
#define EFI_IFR_SUBTITLE_OP            0x02
#define EFI_IFR_TEXT_OP                0x03
#define EFI_IFR_IMAGE_OP               0x04
#define EFI_IFR_ONE_OF_OP              0x05
#define EFI_IFR_CHECKBOX_OP            0x06
#define EFI_IFR_NUMERIC_OP             0x07
#define EFI_IFR_PASSWORD_OP            0x08
#define EFI_IFR_ONE_OF_OPTION_OP       0x09
#define EFI_IFR_SUPPRESS_IF_OP         0x0A
#define EFI_IFR_LOCKED_OP              0x0B
#define EFI_IFR_ACTION_OP              0x0C
#define EFI_IFR_RESET_BUTTON_OP        0x0D
#define EFI_IFR_FORM_SET_OP            0x0E
#define EFI_IFR_REF_OP                 0x0F
#define EFI_IFR_NO_SUBMIT_IF_OP        0x10
#define EFI_IFR_INCONSISTENT_IF_OP     0x11
#define EFI_IFR_EQ_ID_VAL_OP           0x12
#define EFI_IFR_EQ_ID_ID_OP            0x13
#define EFI_IFR_EQ_ID_LIST_OP          0x14
#define EFI_IFR_AND_OP                 0x15
#define EFI_IFR_OR_OP                  0x16
#define EFI_IFR_NOT_OP                 0x17
#define EFI_IFR_RULE_OP                0x18
#define EFI_IFR_GRAY_OUT_IF_OP         0x19
#define EFI_IFR_DATE_OP                0x1A
#define EFI_IFR_TIME_OP                0x1B
#define EFI_IFR_STRING_OP              0x1C
#define EFI_IFR_REFRESH_OP             0x1D
#define EFI_IFR_DISABLE_IF_OP          0x1E
#define EFI_IFR_TO_LOWER_OP            0x20
#define EFI_IFR_TO_UPPER_OP            0x21
#define EFI_IFR_ORDERED_LIST_OP        0x23
#define EFI_IFR_VARSTORE_OP            0x24
#define EFI_IFR_VARSTORE_NAME_VALUE_OP 0x25
#define EFI_IFR_VARSTORE_EFI_OP        0x26
#define EFI_IFR_VARSTORE_DEVICE_OP     0x27
#define EFI_IFR_VERSION_OP             0x28
#define EFI_IFR_END_OP                 0x29
#define EFI_IFR_MATCH_OP               0x2A
#define EFI_IFR_EQUAL_OP               0x2F
#define EFI_IFR_NOT_EQUAL_OP           0x30
#define EFI_IFR_GREATER_THAN_OP        0x31
#define EFI_IFR_GREATER_EQUAL_OP       0x32
#define EFI_IFR_LESS_THAN_OP           0x33
#define EFI_IFR_LESS_EQUAL_OP          0x34
#define EFI_IFR_BITWISE_AND_OP         0x35
#define EFI_IFR_BITWISE_OR_OP          0x36
#define EFI_IFR_BITWISE_NOT_OP         0x37
#define EFI_IFR_SHIFT_LEFT_OP          0x38
#define EFI_IFR_SHIFT_RIGHT_OP         0x39
#define EFI_IFR_ADD_OP                 0x3A
#define EFI_IFR_SUBTRACT_OP            0x3B
#define EFI_IFR_MULTIPLY_OP            0x3C
#define EFI_IFR_DIVIDE_OP              0x3D
#define EFI_IFR_MODULO_OP              0x3E
#define EFI_IFR_RULE_REF_OP            0x3F
#define EFI_IFR_QUESTION_REF1_OP       0x40
#define EFI_IFR_QUESTION_REF2_OP       0x41
#define EFI_IFR_UINT8_OP               0x42
#define EFI_IFR_UINT16_OP              0x43
#define EFI_IFR_UINT32_OP              0x44
#define EFI_IFR_UINT64_OP              0x45
#define EFI_IFR_TRUE_OP                0x46
#define EFI_IFR_FALSE_OP               0x47
#define EFI_IFR_TO_UINT_OP             0x48
#define EFI_IFR_TO_STRING_OP           0x49
#define EFI_IFR_TO_BOOLEAN_OP          0x4A
#define EFI_IFR_MID_OP                 0x4B
#define EFI_IFR_FIND_OP                0x4C
#define EFI_IFR_TOKEN_OP               0x4D
#define EFI_IFR_STRING_REF1_OP         0x4E
#define EFI_IFR_STRING_REF2_OP         0x4F
#define EFI_IFR_CONDITIONAL_OP         0x50
#define EFI_IFR_QUESTION_REF3_OP       0x51
#define EFI_IFR_ZERO_OP                0x52
#define EFI_IFR_ONE_OP                 0x53
#define EFI_IFR_ONES_OP                0x54
#define EFI_IFR_UNDEFINED_OP           0x55
#define EFI_IFR_LENGTH_OP              0x56
#define EFI_IFR_DUP_OP                 0x57
#define EFI_IFR_THIS_OP                0x58
#define EFI_IFR_SPAN_OP                0x59
#define EFI_IFR_VALUE_OP               0x5A
#define EFI_IFR_DEFAULT_OP             0x5B
#define EFI_IFR_DEFAULTSTORE_OP        0x5C
#define EFI_IFR_CATENATE_OP            0x5E
#define EFI_IFR_GUID_OP                0x5F

typedef struct _EFI_IFR_OP_HEADER {
  UINT8                    OpCode;
  UINT8                    Length:7;
  UINT8                    Scope:1;
} EFI_IFR_OP_HEADER;

typedef struct _EFI_IFR_STATEMENT_HEADER {
  EFI_STRING_ID            Prompt;
  EFI_STRING_ID            Help;
} EFI_IFR_STATEMENT_HEADER;

typedef struct _EFI_IFR_QUESTION_HEADER {
  EFI_IFR_STATEMENT_HEADER Header;
  EFI_QUESTION_ID          QuestionId;
  EFI_VARSTORE_ID          VarStoreId;
  union {
    EFI_STRING_ID          VarName;
    UINT16                 VarOffset;
  }                        VarStoreInfo;
  UINT8                    Flags;
} EFI_IFR_QUESTION_HEADER;

#define EFI_IFR_FLAG_READ_ONLY         0x01
#define EFI_IFR_FLAG_CALLBACK          0x04
#define EFI_IFR_FLAG_RESET_REQUIRED    0x10
#define EFI_IFR_FLAG_OPTIONS_ONLY      0x80

typedef struct _EFI_IFR_DEFAULTSTORE {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            DefaultName;
  UINT16                   DefaultId;
} EFI_IFR_DEFAULTSTORE;

#define EFI_HII_DEFAULT_CLASS_STANDARD       0x0000
#define EFI_HII_DEFAULT_CLASS_MANUFACTURING  0x0001
#define EFI_HII_DEFAULT_CLASS_SAFE           0x0002
#define EFI_HII_DEFAULT_CLASS_PLATFORM_BEGIN 0x4000
#define EFI_HII_DEFAULT_CLASS_PLATFORM_END   0x7fff
#define EFI_HII_DEFAULT_CLASS_HARDWARE_BEGIN 0x8000
#define EFI_HII_DEFAULT_CLASS_HARDWARE_END   0xbfff
#define EFI_HII_DEFAULT_CLASS_FIRMWARE_BEGIN 0xc000
#define EFI_HII_DEFAULT_CLASS_FIRMWARE_END   0xffff

typedef struct _EFI_IFR_VARSTORE {
  EFI_IFR_OP_HEADER        Header;
  EFI_GUID                 Guid;
  EFI_VARSTORE_ID          VarStoreId;
  UINT16                   Size;
  UINT8                    Name[1];
} EFI_IFR_VARSTORE;

typedef struct _EFI_IFR_VARSTORE_EFI {
  EFI_IFR_OP_HEADER        Header;
  UINT16                   VarStoreId;
  EFI_GUID                 Guid;
  UINT32                   Attributes;
} EFI_IFR_VARSTORE_EFI;

typedef struct _EFI_IFR_VARSTORE_NAME_VALUE {
  EFI_IFR_OP_HEADER        Header;
  UINT16                   VarStoreId;
  EFI_GUID                 Guid;
} EFI_IFR_VARSTORE_NAME_VALUE;

typedef struct _EFI_IFR_FORM_SET {
  EFI_IFR_OP_HEADER        Header;
  EFI_GUID                 Guid;
  EFI_STRING_ID            FormSetTitle;
  EFI_STRING_ID            Help;
} EFI_IFR_FORM_SET;

typedef struct _EFI_IFR_END {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_END;

typedef struct _EFI_IFR_FORM {
  EFI_IFR_OP_HEADER        Header;
  UINT16                   FormId;
  EFI_STRING_ID            FormTitle;
} EFI_IFR_FORM;

typedef struct _EFI_IFR_IMAGE {
  EFI_IFR_OP_HEADER        Header;
  EFI_IMAGE_ID             Id;
} EFI_IFR_IMAGE;

typedef struct _EFI_IFR_LOCKED {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_LOCKED;

typedef struct _EFI_IFR_RULE {
  EFI_IFR_OP_HEADER        Header;
  UINT8                    RuleId;
} EFI_IFR_RULE;

typedef struct _EFI_IFR_DEFAULT {
  EFI_IFR_OP_HEADER        Header;
  UINT16                   DefaultId;
  UINT8                    Type;
  EFI_IFR_TYPE_VALUE       Value;
} EFI_IFR_DEFAULT;

typedef struct _EFI_IFR_VALUE {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_VALUE;

typedef struct _EFI_IFR_SUBTITLE {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_STATEMENT_HEADER Statement;
  UINT8                    Flags;
} EFI_IFR_SUBTITLE;

#define EFI_IFR_FLAGS_HORIZONTAL       0x01

typedef struct _EFI_IFR_CHECKBOX {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    Flags;
} EFI_IFR_CHECKBOX;

#define EFI_IFR_CHECKBOX_DEFAULT       0x01
#define EFI_IFR_CHECKBOX_DEFAULT_MFG   0x02

typedef struct _EFI_IFR_TEXT {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_STATEMENT_HEADER Statement;
  EFI_STRING_ID            TextTwo;
} EFI_IFR_TEXT;

typedef struct _EFI_IFR_REF {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  EFI_FORM_ID              FormId;
} EFI_IFR_REF;

typedef struct _EFI_IFR_REF2 {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  EFI_FORM_ID              FormId;
  EFI_QUESTION_ID          QuestionId;
} EFI_IFR_REF2;

typedef struct _EFI_IFR_REF3 {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  EFI_FORM_ID              FormId;
  EFI_QUESTION_ID          QuestionId;
  EFI_GUID                 FormSetId;
} EFI_IFR_REF3;

typedef struct _EFI_IFR_REF4 {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  EFI_FORM_ID              FormId;
  EFI_QUESTION_ID          QuestionId;
  EFI_GUID                 FormSetId;
  EFI_STRING_ID            DevicePath;
} EFI_IFR_REF4;

typedef struct _EFI_IFR_RESET_BUTTON {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  EFI_DEFAULT_ID           DefaultId;
} EFI_IFR_RESET_BUTTON;

typedef struct _EFI_IFR_ACTION {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  EFI_STRING_ID            QuestionConfig;
} EFI_IFR_ACTION;

typedef struct _EFI_IFR_ACTION_1 {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
} EFI_IFR_ACTION_1;

typedef struct _EFI_IFR_DATE {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    Flags;
} EFI_IFR_DATE;

#define EFI_QF_DATE_YEAR_SUPPRESS      0x01
#define EFI_QF_DATE_MONTH_SUPPRESS     0x02
#define EFI_QF_DATE_DAY_SUPPRESS       0x04

#define EFI_QF_DATE_STORAGE            0x30
#define     QF_DATE_STORAGE_NORMAL     0x00
#define     QF_DATE_STORAGE_TIME       0x10
#define     QF_DATE_STORAGE_WAKEUP     0x20

typedef union {
  struct {
    UINT8 MinValue;
    UINT8 MaxValue;
    UINT8 Step;
  } u8;
  struct {
    UINT16 MinValue;
    UINT16 MaxValue;
    UINT16 Step;
  } u16;
  struct {
    UINT32 MinValue;
    UINT32 MaxValue;
    UINT32 Step;
  } u32;
  struct {
    UINT64 MinValue;
    UINT64 MaxValue;
    UINT64 Step;
  } u64;
} MINMAXSTEP_DATA;

typedef struct _EFI_IFR_NUMERIC {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    Flags;
  MINMAXSTEP_DATA          data;
} EFI_IFR_NUMERIC;

#define EFI_IFR_NUMERIC_SIZE           0x03
#define EFI_IFR_NUMERIC_SIZE_1         0x00
#define EFI_IFR_NUMERIC_SIZE_2         0x01
#define EFI_IFR_NUMERIC_SIZE_4         0x02
#define EFI_IFR_NUMERIC_SIZE_8         0x03

#define EFI_IFR_DISPLAY                0x30
#define EFI_IFR_DISPLAY_INT_DEC        0x00
#define EFI_IFR_DISPLAY_UINT_DEC       0x10
#define EFI_IFR_DISPLAY_UINT_HEX       0x20

typedef struct _EFI_IFR_ONE_OF {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    Flags;
  MINMAXSTEP_DATA          data;
} EFI_IFR_ONE_OF;

typedef struct _EFI_IFR_STRING {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    MinSize;
  UINT8                    MaxSize;
  UINT8                    Flags;
} EFI_IFR_STRING;

#define EFI_IFR_STRING_MULTI_LINE      0x01

typedef struct _EFI_IFR_PASSWORD {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT16                   MinSize;
  UINT16                   MaxSize;
} EFI_IFR_PASSWORD;

typedef struct _EFI_IFR_ORDERED_LIST {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    MaxContainers;
  UINT8                    Flags;
} EFI_IFR_ORDERED_LIST;

#define EFI_IFR_UNIQUE_SET             0x01
#define EFI_IFR_NO_EMPTY_SET           0x02

typedef struct _EFI_IFR_TIME {
  EFI_IFR_OP_HEADER        Header;
  EFI_IFR_QUESTION_HEADER  Question;
  UINT8                    Flags;
} EFI_IFR_TIME;

#define QF_TIME_HOUR_SUPPRESS          0x01
#define QF_TIME_MINUTE_SUPPRESS        0x02
#define QF_TIME_SECOND_SUPPRESS        0x04

#define QF_TIME_STORAGE                0x30
#define QF_TIME_STORAGE_NORMAL         0x00
#define QF_TIME_STORAGE_TIME           0x10
#define QF_TIME_STORAGE_WAKEUP         0x20

typedef struct _EFI_IFR_DISABLE_IF {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_DISABLE_IF;

typedef struct _EFI_IFR_SUPPRESS_IF {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_SUPPRESS_IF;

typedef struct _EFI_IFR_GRAY_OUT_IF {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_GRAY_OUT_IF;

typedef struct _EFI_IFR_INCONSISTENT_IF {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            Error;
} EFI_IFR_INCONSISTENT_IF;

typedef struct _EFI_IFR_NO_SUBMIT_IF {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            Error;
} EFI_IFR_NO_SUBMIT_IF;

typedef struct _EFI_IFR_REFRESH {
  EFI_IFR_OP_HEADER        Header;
  UINT8                    RefreshInterval;
} EFI_IFR_REFRESH;

typedef struct _EFI_IFR_VARSTORE_DEVICE {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            DevicePath;
} EFI_IFR_VARSTORE_DEVICE;

typedef struct _EFI_IFR_ONE_OF_OPTION {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            Option;
  UINT8                    Flags;
  UINT8                    Type;
  EFI_IFR_TYPE_VALUE       Value;
} EFI_IFR_ONE_OF_OPTION;

#define EFI_IFR_TYPE_NUM_SIZE_8        0x00
#define EFI_IFR_TYPE_NUM_SIZE_16       0x01
#define EFI_IFR_TYPE_NUM_SIZE_32       0x02
#define EFI_IFR_TYPE_NUM_SIZE_64       0x03
#define EFI_IFR_TYPE_BOOLEAN           0x04
#define EFI_IFR_TYPE_TIME              0x05
#define EFI_IFR_TYPE_DATE              0x06
#define EFI_IFR_TYPE_STRING            0x07
#define EFI_IFR_TYPE_OTHER             0x08

#define EFI_IFR_OPTION_DEFAULT         0x10
#define EFI_IFR_OPTION_DEFAULT_MFG     0x20

typedef struct _EFI_IFR_GUID {
  EFI_IFR_OP_HEADER        Header;
  EFI_GUID                 Guid;
  //Optional Data Follows
} EFI_IFR_GUID;

typedef struct _EFI_IFR_DUP {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_DUP;

typedef struct _EFI_IFR_EQ_ID_ID {
  EFI_IFR_OP_HEADER        Header;
  EFI_QUESTION_ID          QuestionId1;
  EFI_QUESTION_ID          QuestionId2;
} EFI_IFR_EQ_ID_ID;

typedef struct _EFI_IFR_EQ_ID_VAL {
  EFI_IFR_OP_HEADER        Header;
  EFI_QUESTION_ID          QuestionId;
  UINT16                   Value;
} EFI_IFR_EQ_ID_VAL;

typedef struct _EFI_IFR_EQ_ID_LIST {
  EFI_IFR_OP_HEADER        Header;
  EFI_QUESTION_ID          QuestionId;
  UINT16                   ListLength;
  UINT16                   ValueList[1];
} EFI_IFR_EQ_ID_LIST;

typedef struct _EFI_IFR_QUESTION_REF1 {
  EFI_IFR_OP_HEADER        Header;
  EFI_QUESTION_ID          QuestionId;
} EFI_IFR_QUESTION_REF1;

typedef struct _EFI_IFR_UINT8 {
  EFI_IFR_OP_HEADER        Header;
  UINT8 Value;
} EFI_IFR_UINT8;

typedef struct _EFI_IFR_UINT16 {
  EFI_IFR_OP_HEADER        Header;
  UINT16                   Value;
} EFI_IFR_UINT16;

typedef struct _EFI_IFR_QUESTION_REF2 {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_QUESTION_REF2;

typedef struct _EFI_IFR_UINT32 {
  EFI_IFR_OP_HEADER        Header;
  UINT32                   Value;
} EFI_IFR_UINT32;

typedef struct _EFI_IFR_UINT64 {
  EFI_IFR_OP_HEADER        Header;
  UINT64 Value;
} EFI_IFR_UINT64;

typedef struct _EFI_IFR_QUESTION_REF3 {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_QUESTION_REF3;

typedef struct _EFI_IFR_QUESTION_REF3_2 {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            DevicePath;
} EFI_IFR_QUESTION_REF3_2;

typedef struct _EFI_IFR_QUESTION_REF3_3 {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            DevicePath;
  EFI_GUID                 Guid;
} EFI_IFR_QUESTION_REF3_3;

typedef struct _EFI_IFR_RULE_REF {
  EFI_IFR_OP_HEADER        Header;
  UINT8                    RuleId;
} EFI_IFR_RULE_REF;

typedef struct _EFI_IFR_STRING_REF1 {
  EFI_IFR_OP_HEADER        Header;
  EFI_STRING_ID            StringId;
} EFI_IFR_STRING_REF1;

typedef struct _EFI_IFR_STRING_REF2 {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_STRING_REF2;

typedef struct _EFI_IFR_THIS {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_THIS;

typedef struct _EFI_IFR_TRUE {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_TRUE;

typedef struct _EFI_IFR_FALSE {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_FALSE;

typedef struct _EFI_IFR_ONE {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_ONE;

typedef struct _EFI_IFR_ONES {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_ONES;

typedef struct _EFI_IFR_ZERO {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_ZERO;

typedef struct _EFI_IFR_UNDEFINED {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_UNDEFINED;

typedef struct _EFI_IFR_VERSION {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_VERSION;

typedef struct _EFI_IFR_LENGTH {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_LENGTH;

typedef struct _EFI_IFR_NOT {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_NOT;

typedef struct _EFI_IFR_BITWISE_NOT {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_BITWISE_NOT;

typedef struct _EFI_IFR_TO_BOOLEAN {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_TO_BOOLEAN;

#define EFI_IFR_STRING_UNSIGNED_DEC      0
#define EFI_IFR_STRING_SIGNED_DEC        1
#define EFI_IFR_STRING_LOWERCASE_HEX     2
#define EFI_IFR_STRING_UPPERCASE_HEX     3

#define EFI_IFR_STRING_ASCII             0
#define EFI_IFR_STRING_UNICODE           8

typedef struct _EFI_IFR_TO_STRING {
  EFI_IFR_OP_HEADER        Header;
  UINT8                    Format;
} EFI_IFR_TO_STRING;

typedef struct _EFI_IFR_TO_UINT {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_TO_UINT;

typedef struct _EFI_IFR_TO_UPPER {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_TO_UPPER;

typedef struct _EFI_IFR_TO_LOWER {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_TO_LOWER;

typedef struct _EFI_IFR_ADD {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_ADD;

typedef struct _EFI_IFR_AND {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_AND;

typedef struct _EFI_IFR_BITWISE_AND {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_BITWISE_AND;

typedef struct _EFI_IFR_BITWISE_OR {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_BITWISE_OR;

typedef struct _EFI_IFR_CATENATE {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_CATENATE;

typedef struct _EFI_IFR_DIVIDE {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_DIVIDE;

typedef struct _EFI_IFR_EQUAL {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_EQUAL;

typedef struct _EFI_IFR_GREATER_EQUAL {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_GREATER_EQUAL;

typedef struct _EFI_IFR_GREATER_THAN {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_GREATER_THAN;

typedef struct _EFI_IFR_LESS_EQUAL {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_LESS_EQUAL;

typedef struct _EFI_IFR_LESS_THAN {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_LESS_THAN;

typedef struct _EFI_IFR_MATCH {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_MATCH;

typedef struct _EFI_IFR_MULTIPLY {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_MULTIPLY;

typedef struct _EFI_IFR_MODULO {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_MODULO;

typedef struct _EFI_IFR_NOT_EQUAL {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_NOT_EQUAL;

typedef struct _EFI_IFR_OR {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_OR;

typedef struct _EFI_IFR_SHIFT_LEFT {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_SHIFT_LEFT;

typedef struct _EFI_IFR_SHIFT_RIGHT {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_SHIFT_RIGHT;

typedef struct _EFI_IFR_SUBTRACT {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_SUBTRACT;

typedef struct _EFI_IFR_CONDITIONAL {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_CONDITIONAL;

#define EFI_IFR_FF_CASE_SENSITIVE    0x00
#define EFI_IFR_FF_CASE_INSENSITIVE  0x01

typedef struct _EFI_IFR_FIND {
  EFI_IFR_OP_HEADER        Header;
  UINT8                    Format;
} EFI_IFR_FIND;

typedef struct _EFI_IFR_MID {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_MID;

typedef struct _EFI_IFR_TOKEN {
  EFI_IFR_OP_HEADER        Header;
} EFI_IFR_TOKEN;

#define EFI_IFR_FLAGS_FIRST_MATCHING     0x00
#define EFI_IFR_FLAGS_FIRST_NON_MATCHING 0x01

typedef struct _EFI_IFR_SPAN {
  EFI_IFR_OP_HEADER        Header;
  UINT8                    Flags;
} EFI_IFR_SPAN;*/



//
//GUIDed OpCodes
//

#define EFI_IFR_EXTEND_OP_LABEL       0x0
#define EFI_IFR_EXTEND_OP_BANNER      0x1
#define EFI_IFR_EXTEND_OP_TIMEOUT     0x2
#define EFI_IFR_EXTEND_OP_CLASS       0x3
#define EFI_IFR_EXTEND_OP_SUBCLASS    0x4

typedef struct _EFI_IFR_GUID_LABEL {
  EFI_IFR_OP_HEADER   Header;
  EFI_GUID            Guid;
  UINT8               ExtendOpCode;
  UINT16              Number;
} EFI_IFR_GUID_LABEL;

#define EFI_IFR_BANNER_ALIGN_LEFT     0
#define EFI_IFR_BANNER_ALIGN_CENTER   1
#define EFI_IFR_BANNER_ALIGN_RIGHT    2

typedef struct _EFI_IFR_GUID_BANNER {
  EFI_IFR_OP_HEADER   Header;
  EFI_GUID            Guid;
  UINT8               ExtendOpCode; // Extended opcode is EFI_IFR_EXTEND_OP_BANNER
  EFI_STRING_ID       Title;        // The string token for the banner title
  UINT16              LineNumber;   // 1-based line number
  UINT8               Alignment;    // left, center, or right-aligned
} EFI_IFR_GUID_BANNER;

typedef struct _EFI_IFR_GUID_TIMEOUT {
  EFI_IFR_OP_HEADER   Header;
  EFI_GUID            Guid;
  UINT8               ExtendOpCode;
  UINT16              TimeOut;
} EFI_IFR_GUID_TIMEOUT;

#define EFI_NON_DEVICE_CLASS              0x00
#define EFI_DISK_DEVICE_CLASS             0x01
#define EFI_VIDEO_DEVICE_CLASS            0x02
#define EFI_NETWORK_DEVICE_CLASS          0x04
#define EFI_INPUT_DEVICE_CLASS            0x08
#define EFI_ON_BOARD_DEVICE_CLASS         0x10
#define EFI_OTHER_DEVICE_CLASS            0x20

typedef struct _EFI_IFR_GUID_CLASS {
  EFI_IFR_OP_HEADER   Header;
  EFI_GUID            Guid;
  UINT8               ExtendOpCode;
  UINT16              Class;
} EFI_IFR_GUID_CLASS;

#define EFI_SETUP_APPLICATION_SUBCLASS    0x00
#define EFI_GENERAL_APPLICATION_SUBCLASS  0x01
#define EFI_FRONT_PAGE_SUBCLASS           0x02
#define EFI_SINGLE_USE_SUBCLASS           0x03

typedef struct _EFI_IFR_GUID_SUBCLASS {
  EFI_IFR_OP_HEADER   Header;
  EFI_GUID            Guid;
  UINT8               ExtendOpCode;
  UINT16              SubClass;
} EFI_IFR_GUID_SUBCLASS;

#pragma pack()

//
// UEFI control data width calculation
//

#define	TSE_COMMON_QUESTION_ID				Question.VarStoreInfo.VarOffset
#define TSE_GET_IFR_INCONSISTENT_POPUP(a)	(((EFI_IFR_INCONSISTENT_IF*)a)->Error)
#define	TSE_COMMON_PROMPT					Question.Header.Prompt
#define	TSE_COMMON_HELP						Question.Header.Help
#define TSE_GET_DATE_HELP(a)				(((EFI_IFR_DATE *)a)->TSE_COMMON_HELP)
#define TSE_GET_DATE_PROMPT(a)				(((EFI_IFR_DATE *)a)->TSE_COMMON_PROMPT)
#define TSE_GET_TIME_HELP(a)				(((EFI_IFR_TIME *)a)->TSE_COMMON_HELP)
#define TSE_GET_TIME_PROMPT(a)				(((EFI_IFR_TIME *)a)->TSE_COMMON_PROMPT)
#define	TSE_SUBTITLE_PROMPT					Statement.Prompt
#define TSE_GET_NUMERIC_WIDTH(a)			(1<<(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE))
#define _GET_MINMAXSTEP_DATA(b,c,d)			( \
				((c)==EFI_IFR_NUMERIC_SIZE_1)?((b).u8.d):( 	\
				((c)==EFI_IFR_NUMERIC_SIZE_2)?((b).u16.d):( \
				((c)==EFI_IFR_NUMERIC_SIZE_4)?((b).u32.d): 	\
				((b).u64.d) )))

#define TSE_GET_NUMERIC_MINVALUE(a)			_GET_MINMAXSTEP_DATA((((EFI_IFR_NUMERIC*)(a))->data),(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE),MinValue)
#define TSE_GET_NUMERIC_MAXVALUE(a)			_GET_MINMAXSTEP_DATA((((EFI_IFR_NUMERIC*)(a))->data),(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE),MaxValue)
#define TSE_GET_NUMERIC_STEP(a)				_GET_MINMAXSTEP_DATA((((EFI_IFR_NUMERIC*)(a))->data),(((EFI_IFR_NUMERIC*)(a))->Flags&EFI_IFR_NUMERIC_SIZE),Step)
#define TSE_GET_STRING_WIDTH(a)				((((EFI_IFR_OP_HEADER*)a)->OpCode==EFI_IFR_STRING_OP))?(((EFI_IFR_STRING*)(a))->MaxSize * sizeof(CHAR16)):(((EFI_IFR_PASSWORD*)(a))->MaxSize * sizeof(CHAR16))
#define TSE_GET_STRING_MAXSIZE(a)			((((EFI_IFR_OP_HEADER*)a)->OpCode==EFI_IFR_STRING_OP))?(((EFI_IFR_STRING*)(a))->MaxSize):(((EFI_IFR_PASSWORD*)(a))->MaxSize)
#define TSE_GET_STRING_MINSIZE(a)			((((EFI_IFR_OP_HEADER*)a)->OpCode==EFI_IFR_STRING_OP))?(((EFI_IFR_STRING*)(a))->MinSize):(((EFI_IFR_PASSWORD*)(a))->MinSize)
#define TSE_INVALID_TOKEN					((EFI_STRING_ID)-1)
#define	TSE_TEXT_TEXT						Statement.Prompt
#define	TSE_TEXT_HELP						Statement.Help
#define TSE_GET_ONE_OF_OPTION_WIDTH(a) ( \
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_8 ? sizeof(UINT8) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_16 ? sizeof(UINT16) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_32 ? sizeof(UINT32) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_64 ? sizeof(UINT64) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_BOOLEAN ? sizeof(BOOLEAN) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_TIME ? sizeof(EFI_HII_TIME) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_DATE ? sizeof(EFI_HII_DATE) : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_STRING ? sizeof(EFI_STRING_ID) : (\
	  0)))))))))

#define TSE_GET_ONE_OF_OPTION_VALUE(a) ( \
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_8 ? ((EFI_IFR_ONE_OF_OPTION*)a)->Value.u8 : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_16 ? ((EFI_IFR_ONE_OF_OPTION*)a)->Value.u16 : (\
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Type == EFI_IFR_TYPE_NUM_SIZE_32 ? ((EFI_IFR_ONE_OF_OPTION*)a)->Value.u32 : \
	  ((EFI_IFR_ONE_OF_OPTION*)a)->Value.u64)))

#define TSE_GET_ONE_OF_WIDTH(a)				((((EFI_IFR_OP_HEADER*)a)->OpCode==EFI_IFR_ONE_OF_OP)?(1<<(((EFI_IFR_ONE_OF*)(a))->Flags&EFI_IFR_NUMERIC_SIZE)):1)
#define TSE_GET_CONTROL_DATA_WIDTH(a) ( \
	  ((EFI_IFR_OP_HEADER*)a)->OpCode == EFI_IFR_NUMERIC_OP ? (UINT32)TSE_GET_NUMERIC_WIDTH(a) : ( \
	  ((EFI_IFR_OP_HEADER*)a)->OpCode == EFI_IFR_ONE_OF_OP  ? (UINT32)TSE_GET_ONE_OF_WIDTH(a) : ( \
	  ((EFI_IFR_OP_HEADER*)a)->OpCode == EFI_IFR_CHECKBOX_OP  ? 1 : \
	  (UINT32)TSE_GET_STRING_WIDTH(a))))

#define UefiGetWidth(a) TSE_GET_CONTROL_DATA_WIDTH(a)

#define LANG_OFFSET_UEFI21				0x2E
#endif  // include guard
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2010, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
