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
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************
// Revision History
// ----------------
// $Log: $
// 
// 
//**********************************************************************


//<AMI_FHDR_START>
//
// Name:  		PldmApi.h
//
// Description:	 Pldm tables for SCE environment
//
// <AMI_FHDR_END>

extern BOOLEAN gUseCache;	//If set to TRUE, Library uses Cache. If set to FALSE, then Libray uses Variables.
extern BOOLEAN gSkipVarbyHiiHandles;	//If set to TRUE, Variables are skipped if Mapping Table is present in the Package.

#pragma once
#pragma pack(push, 1)

typedef struct
{
    UINT16  ControlType;
    UINT16  Length;
} PLDM_MAPIDINFO;
#pragma pack(pop)

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreatePldmTables
// 
// Description:  This function constructs string, attribute, and value tables
//               Values come from NVRAM.
//
// Parameter:    UINT8 *HiiData
//					[in]  Pointer to HII export data base
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8  **StringTable
//					[out] return pointer to string table
//				 UINT32 *StringSize
//				    [out] return string table size
//               UINT8 **AttributeTable
//                  [out] return pointer to attribute table
//               UINT32 *AttributeSize
//                  [out] return attribute table size
//               UINT8 **ValueTable
//                  [out] return pointer to value table
//               UINT8  *ValueSize
//                  [out] return value table size
//				 UINT8  **MapStringTable
//					[out] return pointer to map string table
//				 UINT32 *MapStringSize
//				    [out] return map string table size
//				 BOOLEAN SaveVars
//				    [in] Save unreferenced variables
//
// Return value: EFI_STATUS
// 
// Modified:    
// 
// Referral(s): 
// 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CreatePldmTables(
    UINT8  *HiiData,       // pointer to HII export data
    UINT32  HiiSize,       // size of Hii Data
    UINT8 **StringTable,   // return string table pointer
    UINT32 *StringSize,    // return string table size
    UINT8 **AttributeTable,// return attribute table pointer
    UINT32 *AttributeSize, // return attribute table size
    UINT8 **ValueTable,    // return value table pointer
    UINT32 *ValueSize,     // return value size
    UINT8 **MapStringTable,// return map string table pointer
    UINT32 *MapStringSize, // return map string table size
    BOOLEAN SaveVars       // save unreferenced variables
    );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportPldmValues
// 
// Description:  This function imports a PLDM value table into NVRAM
//              
//
// Parameter:    UINT8 *HiiData
//					[in]  Pointer to HII export data base//				 
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8 *ValueTable
//				    [in]  Pointer to PLDM value table
//				 UINT32 TableSize
//				    [in]  Size of value table
// Return value: EFI_STATUS
// 
// Modified:    NVRAM values for questions
// 
// Referral(s): 
// 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ImportPldmValues(
    UINT8  *HiiData,       // pointer to HII export data
    UINT32  HiiSize,       // size of Hii Data
    UINT8  *ValueTable,    // value table pointer
    UINT32  TableSize
    );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportMappedPldmValues
// 
// Description:  This function imports a mapped PLDM value table into NVRAM
//              
//
// Parameter:    UINT8 *HiiData
//					[in]  Pointer to HII export data base				 
//				 UINT32 HiiSize
//					[in]  Size of export data base
//				 UINT8 *ValueTable
//				    [in]  Pointer to PLDM value table
//				 UINT32 ValueSize
//				    [in]  Size of value table
//				 UINT8  *AttributeTable
//				    [in] attribute table from foreign system
//				 UINT32 AttributeSize
//				    [in] Size of foreign attribute table
//				 UINT8 *MapStringTable
//				    [in] x-AMI string table from foreign system
//				 UINT32 MapStringSize
//				    [in] Size of foreign map string table
//				 UINT8 *EngStringTable
//				    [in] English string table from foreign system
//				 UINT32 EngStringSize
//				    [in] Size of English string table
//
// Return value: EFI_STATUS
// 
// Modified:    NVRAM values for questions
// 
// Referral(s): 
// 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ImportMappedPldmValues(
    UINT8  *HiiData,        // pointer to HII export data
    UINT32  HiiSize,        // size of Hii Data
    UINT8  *ValueTable,     // value table pointer
    UINT32  ValueSize,
    UINT8  *AttributeTable, // attribute table from foreign system
    UINT32  AttributeSize, 
    UINT8  *MapStringTable, // x-AMI string table from foreign system
    UINT32  MapStringSize,
    UINT8  *EngStringTable, // english string table from foreign system
    UINT32  EngStringSize
    );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    ImportVarValuesByHiiHandle
// 
// Description:  This function imports saved variables
//              
//
// Parameter:    EFI_HII_HANDLE Handle
//					[in]    Handle for device path substitution, if null 
//					no substitution			 
//				 UINT8 *ValueTable
//				    [in]  Pointer to PLDM value table
//				 UINT32 ValueSize
//				    [in]  Size of value table
//				 UINT8  *AttributeTable
//				    [in] attribute table from foreign system
//				 UINT32 AttributeSize
//				    [in] Size of foreign attribute table
//				 UINT8 *EngStringTable
//				    [in] English string table from foreign system
//				 UINT32 EngStringSize
//				    [in] Size of English string table
//
// Return value: EFI_STATUS
// 
// Modified:    NVRAM values for questions
// 
// Referral(s): 
// 
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
ImportVarValuesByHiiHandle(
    EFI_HII_HANDLE Handle,
    UINT8  *ValueTable,    
    UINT32  ValueSize,
    UINT8  *AttributeTable, 
    UINT32  AttributeSize,
    UINT8  *StringTable,
    UINT32  StringSize
);
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    CreatePldmTablesByHiiHandles
//
// Description:  This function constructs string, attribute, and value tables
//               Values come from NVRAM.
//
// Parameter:    EFI_HII_HANDLE *HiiHandles
//					[in]  Pointer to array of handles
//				 UINT32 HiiHandleCount
//					[in]  Size of export data base
//				 UINT8  **StringTable
//					[out] return pointer to string table
//				 UINT32 *StringSize
//				    [out] return string table size
//               UINT8 **AttributeTable
//                  [out] return pointer to attribute table
//               UINT32 *AttributeSize
//                  [out] return attribute table size
//               UINT8 **ValueTable
//                  [out] return pointer to value table
//               UINT8  *ValueSize
//                  [out] return value table size
//				 UINT8  **MapStringTable
//					[out] return pointer to map string table
//				 UINT32 *MapStringSize
//				    [out] return map string table size
//				 BOOLEAN SaveVars
//				    [in] Save unreferenced variables
//
// Return value: EFI_STATUS
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS
CreatePldmTablesByHiiHandles(
    EFI_HII_HANDLE  *HiiHandles,  // pointer to HII export data
    UINT32  HiiHandleCount,       // size of Hii Data
    UINT8 **StringTable,   // return string table pointer
    UINT32 *StringSize,    // return string table size
    UINT8 **AttributeTable,// return attribute table pointer
    UINT32 *AttributeSize, // return attribute table size
    UINT8 **ValueTable,    // return value table pointer
    UINT32 *ValueSize,     // return value size
    UINT8 **MapStringTable,// return map string table pointer
    UINT32 *MapStringSize, // return map string table size
    BOOLEAN SaveVars       // save unreferenced variables
    );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    GetMapStringList
//
// Description:  This function gets the Map string list available in Hii along with the specific control details 
//
// Parameter:    UINT8 **MapIdList
//               [out]  Pointer to the map string list
//               UINT32 *MapIdListSize
//               [out]  Map string list size
//
// Return value: EFI_STATUS
//
// Modified:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetMapStringList ( 
        OUT UINT8 **MapIdList,
        OUT UINT32 *MapIdListSize
        ) ;

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    GetValueUsingMapID
//
// Description:   Get the value for mapping string
//
// Parameter:    CHAR16 *MappingID
//               [in]  Mapping string
//               void **Buffer
//               [out]  Buffer to return
//               UINTN *BufferSize
//               [out]  Size of the buffer
//               UINT8 *ControlType
//               [out]  Type of the control
//
// Return value: EFI_STATUS
//
// Modified:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS  GetValueUsingMapID (
        IN CHAR16 *MappingID,
        OUT void **Buffer,
        OUT UINTN *BufferSize,
        OUT UINT8 *ControlType
        );

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    SetValueUsingMapID
//
// Description:   Set the value for mapping string
//
// Parameter:    CHAR16 *MappingID
//               [in]  Mapping string
//               void *Buffer
//               [out]  Buffer to set
//               UINTN BufferSize
//               [out]  Size of the buffer
//
// Return value: EFI_STATUS
//
// Modified:
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS  SetValueUsingMapID (
        IN CHAR16 *MappingID,
        IN void *Buffer,
        IN UINTN BufferSize
        );

#define REMOTE_BIOS_ENTRY_GUID 	  {0xa9026ce5, 0x9915, 0x4c36,0x86, 0xc7, 0x65, 0x28, 0xca, 0xa3, 0x36, 0x7a}
#define REMOTE_BIOS_VARIABLE_GUID {0xe5960b8f, 0xe551, 0x402c,0x80, 0x64, 0x2 , 0xb8, 0x66, 0xf0, 0x6b, 0xa0}
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
