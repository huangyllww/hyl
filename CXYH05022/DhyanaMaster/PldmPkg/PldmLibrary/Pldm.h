//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
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

//**********************************************************************
// $Header: /SandBox/Pldm/Pldm.h 12    9/17/13 5:03p Timothym $
//
// $Revision: 12 $
//
// $Date: 9/17/13 5:03p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /SandBox/Pldm/Pldm.h $
//
// 12    9/17/13 5:03p Timothym
// Add FHDR
//
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  		Pldm.h
//
// Description:	Internal data structures
//
//<AMI_FHDR_END>
#pragma once
#pragma pack(push, 1)

#include "UEFI20HIIParser.h"
#include "types.h"

typedef struct
{
	UINT16 Handle;
	UINT8  Type;
	UINT16 NameHandle;  // prompt string
} PLDM_ATTRIBUTE;

// attribute types
#define BIOSEnumeration         0x0
#define BIOSString              0x1
#define BIOSPassword            0x2
#define BIOSInteger             0x3
#define BIOSBootConfigSetting   0x4
#define BIOSCollection          0x5
#define BIOSConfigSet           0x6
#define BIOSEnumerationReadOnly 0x80
#define BIOSStringReadOnly      0x81
#define BIOSPasswordReadOnly    0x82
#define BIOSIntegerReadOnly     0x83
#define BIOSBootConfigSettingReadOnly   0x84
#define BIOSCollectionReadOnly  0x85
#define BIOSConfigSetReadOnly   0x86

//AMI Specific type used for checkbox control
#define BiosCheckBox			0x7

typedef struct
{
	UINT16  Handle;
	UINT16  Length;
	UINT16  String[0];  // use ascii????
} PLDM_STRING;

typedef struct
{
	UINT16 Handle;
	UINT8  Type;
	// value follows
} PLDM_VALUE;

// attribute and value table types
struct Table6   // enumeration a.k.a. oneof
{
	PLDM_ATTRIBUTE  CommonHeader;
	UINT8 PossibleValues; // number of possible values
	UINT16 Handle[0];     // string handles
	// repeats for defaults
};

struct Table6Defaults
{
	UINT8  NumDefaults;   // number of default values
	UINT16 Handle[0];     // string handles
};

struct Table15 // enumeration value
{
	PLDM_VALUE  CommonHeader;
	UINT8   NumberOfValues; // !!!!?????? maybe for ordered lists
	UINT8   ValueIndex[0]; // indices to possible values above
};

struct Table7   // Strings
{
	PLDM_ATTRIBUTE  CommonHeader;
	UINT8  Type;  // UTF-16le is 4
	UINT16 MinLength;
	UINT16 MaxLength;
	UINT16 DefLength;
	UINT16 DefString[0];    // default string
};

struct Table16  // string value
{
	PLDM_VALUE  CommonHeader;
	UINT16  Length; // length of string
	UINT16  String[0]; // the string
};

struct Table8   // password
{
	PLDM_ATTRIBUTE  CommonHeader;
	UINT8   EncodeType;
	UINT16  MinLength;
	UINT16  MaxLength;
	UINT16  DefLength;
	UINT8   Password[0];
};

struct Table17  // password value
{
	PLDM_VALUE  CommonHeader;
	UINT16  Length;
	UINT8   PassWord[0];
};

struct Table9   // numeric or checkbox
{
	PLDM_ATTRIBUTE  CommonHeader;
	UINT64 LowerBound;
	UINT64 UpperBound;
	UINT32 ScalarIncrement;
	UINT64 DefaultValue;
};

struct Table18 // numeric value
{
	PLDM_VALUE  CommonHeader;
	UINT64 Value;
};

// Boot types
#define BOOTUnknown 0
#define BOOTDefault 1
#define BOOTNext 2
#define BOOTDefaultAndNext 3
#define BOOTOnetime 4
#define BOOTDefaultAndOnetime 5

struct Table10 // Boot
{
	PLDM_ATTRIBUTE  CommonHeader;
	UINT8   Type;
	UINT8   Mode;
	UINT8   MinNumSettings;
	UINT8   MaxNumSettings;
	UINT8   NumPossible;    // number of device name string handles
	UINT16  PossibleDevices[0]; // string handles of devices
};

struct Table19 // Boot value
{
	PLDM_VALUE  CommonHeader;
	UINT8   Type;
	UINT8   Mode;
	UINT8   NumSettings;    // number of device name string handles in attribute
	UINT8   Devices[0];     // indices to list of string handles in attribute
};

struct Table11 // Collection
{
	PLDM_ATTRIBUTE  CommonHeader;
};

struct Table20 // Collection value
{
	PLDM_VALUE  CommonHeader;
};

typedef union
{
	PLDM_ATTRIBUTE *CommonHeader;
	struct Table6 *Enumeration;
	struct Table7 *String;
	struct Table8 *PassWord;
	struct Table9 *Numeric;
	struct Table10 *Boot;
} ATTRIBUTE_POINTER;

typedef union
{
	PLDM_VALUE *CommonHeader;
	struct Table15 *Enumeration;
	struct Table16 *String;
	struct Table17 *PassWord;
	struct Table18 *Numeric;
	struct Table19 *Boot;
} VALUE_POINTER;

struct String
{
	UINT16 EfiHandle;
	char    Referenced; // 1 iff referenced by a control
	char    Allocated;   // 1 iff needs to freed
	UINT16 *String;     // pointer to string inside HII data
};

struct Control
{
	UINT16 EfiHandle;         // control occurence number within pack
	PLDM_ATTRIBUTE    *Attribute;  // pointer to attribute table entry
	PLDM_VALUE        *Value; // offset to pointer to Value table entry
};


struct PackInfo
{
	UINT8 *Package; // the export package
	UINT32 HiiIndex;            // index to package with exported database
	UINT16 FirstStringHandle;   // starting handle for strings in this pack
	UINT16 NumStrings;          // number of strings in pack
	// NOTE: The HII string number for strings[i] is i + 1;
	struct String *Strings;     // pointer to string within Hii string pack
	int     ExtraStrings;       // number of strings created at end of array
	struct String *MapStrings;  // pointer to mapping language string within Hii string pack
	UINT16 FirstControlHandle;
	UINT16 NumControls;
	UINT16 FirstBootString;     // handle of first added boot option device name
	struct Control *Controls;   // array
	int    ExtraControls;   // number of extra controls created at end of array (Legacy Device Orders)
};

// leave a gap in numbering so that we can introduce
// extra legacy controls and strings for devices
#define CONTROL_GAP 11
#define STRING_GAP 11

// for traversing string packs

typedef struct
{
	UINT8      *Buffer;         // start of string pack buffer
	UINT32      Length;         // length of string pack and buffer
	UINT32      BlockOffset;    // offset to current string block
	UINT32      StringOffset;   // offset to current string
	UINT32      StringIndex;    // conceptual index to string
	UINT32      SkipCount;      // number of string positions skipped since
	//  previous call
} STRING_CONTEXT;

#pragma pack(pop)

//<kuaibai-20190218 Fix hang92 when execute OOB cycles in some cases +>
//#define MAX_PACKAGES 30
#define MAX_PACKAGES 100
//<kuaibai-20190218 Fix hang92 when execute OOB cycles in some cases ->
static
struct InternalTables
{
	struct PackInfo Packs[MAX_PACKAGES];  // one for each export package containing controls
	int nPacks;
	UINT16 **LangStrings;   // array of pointers to language names
	int    nLangStrings;    // number of languages
	UINT8  LangSetting;     // Index to string of current lang setting
	UINT8 *HIIDBData;
	UINT8 *ImportValues;    // value table to import
	UINT32 ImportSize;
	// needed for import from foreign system with mapping strings
	UINT8 *ImportAttributes; // attribute table
	UINT32 ImportAttributeSize;    // table size
	UINT8 *ImportMapStrings;    // map string table
	UINT32 ImportMapStringSize;
	UINT8 *ImportEngStrings;    // english string table
	UINT32 ImportEngStringSize;
	// Legacy Boot info
	UINT8 *LegacyDevOrder;  // from variable
	int    NumLegacy;       // number of legacy ordered lists within variable
	UINT32 OrderOffset;     // offset to current list
} Globals;

// things not published in headers
#define RFC_3066_ENTRY_SIZE (42 + 1)
extern UINT32 UpdateLanguageCodes(UINT32 ConditionalVariable);

typedef struct
{
	UINT32	Active;
	UINT16	PathLength;
	CHAR16	Name[1];
	//EFI_DEVICE_PATH_PROTOCOL	DevicePath;
	//UINT8	LoadOptions[];
}
BOOT_OPTION;

extern
BOOT_OPTION **gBootOptions;
extern
UINT16 *gBootOrder;
extern
UINT64 gBootCount;

extern
VOID _PopulateGlobalBootOptions();

extern
char *gpStdBuf;
extern
char *gpMfgBuf;
extern
BOOLEAN gIsStdBuf;
extern
BOOLEAN gIsMfgBuf;
VOID _GetStdMfgDefaultsBuffer();
BOOLEAN _GetStdDefaults(CONTROL_INFO* Control);
BOOLEAN _GetMfgDefaults(CONTROL_INFO* Control);
void FreeResetControlInfo(void);
EFI_STATUS  LoadOptimalDefaultsOfAmiSce(void);
UINTN GetPLDMMappingCount();
CHAR8** GetPLDMMappingLanguageID();
void SetPreserveMode(UINTN Mode) ;

#define DEFAULT_BOOT_ORDER_VARIABLE_GUID  \
  { 0x45cf35f6, 0x0d6e, 0x4d04, 0x85, 0x6a, 0x03, 0x70, 0xa5, 0xb1, 0x6f, 0x53 }
#define DEFAULT_LEGACY_DEV_ORDER_VARIABLE_GUID  \
  { 0x3c4ead08, 0x45ae, 0x4315, 0x8d, 0x15, 0xa6, 0x0e, 0xaa, 0x8c, 0xaf, 0x69 }

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2013, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
