//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file JsonHiiLibraryApi.h

**/

//---------------------------------------------------------------------------

#ifndef _JSON_HII_LIBRARY_H_
#define	_JSON_HII_LIBRARY_H_

extern BOOLEAN gUseCache;	//If set to TRUE, Library uses Cache. If set to FALSE, then Libray uses Variables.

#pragma once
#pragma pack(push, 1)

typedef struct
{
    UINT16 Handle;
    UINT8  Type;
    UINT16 NameHandle;  // prompt string
} HII_ATTRIBUTE;

typedef struct
{
    UINT16 Handle;
    UINT8  Type;
    // value follows
} HII_VALUE;

// attribute and value table types
struct Table6   // enumeration a.k.a. oneof
{
    HII_ATTRIBUTE  CommonHeader;
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
    HII_VALUE  CommonHeader;
    UINT8   NumberOfValues; // !!!!?????? maybe for ordered lists
    UINT8   ValueIndex[0]; // indices to possible values above
};

struct Table7   // Strings
{
    HII_ATTRIBUTE  CommonHeader;
    UINT8  Type;  // UTF-16le is 4
    UINT16 MinLength;
    UINT16 MaxLength;
    UINT16 DefLength;
    UINT16 DefString[0];    // default string
};

struct Table16  // string value
{
    HII_VALUE  CommonHeader;
    UINT16  Length; // length of string
    UINT16  String[0]; // the string
};

struct Table8   // password
{
    HII_ATTRIBUTE  CommonHeader;
    UINT8   EncodeType;
    UINT16  MinLength;
    UINT16  MaxLength;
    UINT16  DefLength;
    UINT8   Password[0];
};

struct Table17  // password value
{
    HII_VALUE  CommonHeader;
    UINT16  Length;
    UINT8   PassWord[0];
};

struct Table9   // numeric or checkbox
{
    HII_ATTRIBUTE  CommonHeader;
    UINT64 LowerBound;
    UINT64 UpperBound;
    UINT32 ScalarIncrement;
    UINT64 DefaultValue;
};

struct Table18 // numeric value
{
    HII_VALUE  CommonHeader;
    UINT8 Base;  
    UINT16 Datawidth;
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
    HII_ATTRIBUTE  CommonHeader;
    UINT8   Type;
    UINT8   Mode;
    UINT8   MinNumSettings;
    UINT8   MaxNumSettings;
    UINT8   NumPossible;    // number of device name string handles
    UINT16  PossibleDevices[0]; // string handles of devices
};

struct Table19 // Boot value
{
    HII_VALUE  CommonHeader;
    UINT8   Type;
    UINT8   Mode;
    UINT8   NumSettings;    // number of device name string handles in attribute
    UINT8   Devices[0];     // indices to list of string handles in attribute
};

struct Table11 // Collection
{
    HII_ATTRIBUTE  CommonHeader;
};

struct Table20 // Collection value
{
    HII_VALUE  CommonHeader;
};

typedef union
{
    HII_ATTRIBUTE *CommonHeader;
    struct Table6 *Enumeration;
    struct Table7 *String;
    struct Table8 *PassWord;
    struct Table9 *Numeric;
    struct Table10 *Boot;
} ATTRIBUTE_POINTER;

typedef union
{
    HII_VALUE *CommonHeader;
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
    UINT16 *String;
    UINT16 *MapString;
    HII_ATTRIBUTE    *Attribute;  // pointer to attribute table entry
    HII_VALUE        *Value; // offset to pointer to Value table entry
};

struct PackInfo
{
    UINT8 *Package; // the export package
    EFI_HII_HANDLE HiiHandle;            // index to package with exported database
    UINT16 FirstStringHandle;   // starting handle for strings in this pack
    UINT16 NumStrings;          // number of strings in pack
    // NOTE: The HII string number for strings[i] is i + 1;
    struct String *Strings;     // pointer to string within Hii string pack
    int     ExtraStrings;       // number of strings created at end of array
    struct String *MapStrings;  // pointer to mapping language string within Hii string pack
    UINT16 FirstControlHandle;
    UINT16 NumControls;
    struct Control *Controls;   // array
    void *VariableList;
};

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
//<kuaibai-20190218 Fix hang92 when execute OOB cycles in some cases +>
//#define MAX_PACKAGES 30
#define MAX_PACKAGES 100
//<kuaibai-20190218 Fix hang92 when execute OOB cycles in some cases ->
typedef struct InternalTables
{
    struct PackInfo Packs[MAX_PACKAGES];  
    int nPacks;
} HiiGlobals;

typedef struct
{
    UINT16  ControlType;
    UINT16  Length;
    UINT16  MapString[0];
} HII_MAPIDINFO;

#pragma pack(pop)

/**
  This function initializes the global structures data for the Controls by parsing the entire Hiidatabase.

  @param[IN] HiiData           Hiidatabase
  @param[IN] HiiSize           Hiidatabase size

  @retval 	 EFI_SUCCESS          initialization success
  @retval 	 Others               initialization failed
**/
EFI_STATUS 
InitializeHiiGlobals(
		IN UINT8  *HiiData,
		IN UINTN  HiiSize
		);

/**
  Gets the available MapString names of all controls in the Hii database.

  @param[OUT] 	MapStringList     The list of Mapstring names 
  @param[OUT] 	MapListSize       Mapstring list size

  @retval 		EFI_SUCCESS       Mapstring list get success
  @retval 		Others            Mapstring list get failed
**/
EFI_STATUS 
GetHiiMapStringList(
		OUT UINT8 **MapStringList,
		OUT UINT32 *MapListSize
		);

/**
  Gets the entire Hii database MapString controls name and value in Json Name:Value pair format

  @param[IN] 	HiiDatabase       Hiidatabase
  @param[IN] 	HiiSize           Hiidatabase size
  @param[OUT] 	MapControlInfo    Mapcontrols Json data buffer 
  @param[OUT] 	BufferSize        buffer size

  @retval 		EFI_SUCCESS       Control database information get success
  @retval 		Others            get failed
**/
EFI_STATUS
GetMapStringControlDatabase (
		IN UINT8 *HiiDatabase,
		IN UINTN  HiiSize,
		OUT UINT8 **MapControlInfo,
		OUT UINT32 *BufferSize
		);

/**
  Parses the provided MapString controls name and value in Json Name:Value pair format data and imports the values to NVRAM
  Returns the Set Failed MapId control names as a buffer  

  @param[IN] 	HiiDatabase       Hiidatabase
  @param[IN] 	HiiSize           Hiidatabase size
  @param[IN] 	MapControlInfo    Mapcontrols Json data buffer 
  @param[IN] 	BufferSize        buffer size
  @param[OUT] 	FailMapIdList     Set Failed Mapstring control names
  @param[OUT] 	FailMapListSize   Fail mapId buffer size

  @retval 		EFI_SUCCESS       Set Control Values success
  @retval 		Others            Set failed
**/
EFI_STATUS
SetMapStringControlDatabase (
		IN UINT8 *HiiDatabase,
		IN UINTN  HiiSize,
		IN UINT8 *MapControlInfo,
		IN UINT32 BufferSize,
		OUT void **FailMapIdList,
		OUT UINT32 *FailMapListSize
		);

/**
  This function will get the control value buffer for the provided Mapstring control name  

  @param[IN] 	MappingID         MapString name for the control
  @param[OUT] 	ValueBuffer       Control value buffer
  @param[OUT] 	ValueSize    	  Control value buffer size
  @param[OUT] 	ControlType       Control Type

  @retval 		EFI_SUCCESS       Get Control Value success
  @retval 		Others            Get failed
**/
EFI_STATUS
GetHiiMapStringValue(
		IN CHAR16 *MappingID,
		OUT void **ValueBuffer,
		OUT UINTN *ValueSize,
		OUT UINT16 *ControlType
		);
/**
  This function will set the control value buffer for the provided Mapstring control name control 

  @param[IN] 	MappingID       MapString name for the control
  @param[IN] 	Buffer       	Control value buffer
  @param[IN] 	BufferSize    	Control value buffer size

  @retval 		EFI_SUCCESS       Get Control Value success
  @retval 		Others            Get failed
**/
EFI_STATUS
SetHiiMapStringValue (
		IN CHAR16 *MappingID,
		IN void *Buffer,
		IN UINTN BufferSize
		);

/**
  This function will get the structure array holding the entire Hiidatabase controls information  

  @param[IN] 	HiiDatabase     HiiDatabase
  @param[IN] 	HiiSize       	HiiSize
  @param[OUT] 	HiiControlInfo  structure array holding the entire Hiidatabase controls information  

  @retval 		EFI_SUCCESS       Get success
  @retval 		Others            Get failed
**/
EFI_STATUS
GetHiiControlsDataArray (
		IN UINT8 *HiiDatabase,
		UINTN  HiiSize,
		OUT const struct InternalTables **HiiControlInfo
		);

/**
  Free the Allocated memory for all the array of global structures

  @param 	VOID 

  @retval 	VOID
**/
void
FreeHiiGlobals();

EFI_STATUS  ProcessBootOptions( void *Buffer, UINTN BufferSize );
#endif

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
