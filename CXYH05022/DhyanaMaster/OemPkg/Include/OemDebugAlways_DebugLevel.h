#ifndef __OEM_DEBUG_LEVEL_H__
#define __OEM_DEBUG_LEVEL_H__

#include	<Token.h>

#pragma pack(push, 1)

#define OEM_DEBUG_LEVEL_DATABASE_TABLE_VERSION 1

// Debug Level Form set Guid
#define OEM_DEBUG_LEVEL_FORM_SET_GUID                                                  \
    {                                                                                  \
        0x8aa247bf, 0x0af9, 0x4035, { 0x82, 0x81, 0x19, 0xee, 0xa7, 0x9a, 0x34, 0xe5 } \
    }

// Debug Level Form set class and Var store ID
#define OEM_DEBUG_LEVEL_SET_CLASS 0xE1
#define OEM_DEBUG_LEVEL_DATABASE_VARSTORE_ID 0xFE00

#define OEM_DEBUG_LEVEL_DATA_NAME L"OemDebugLevel"

// Oem Debug DataBase Signature
#define OEM_DEBUG_LEVEL_SIGNATURE SIGNATURE_32('_', 'O', 'D', '_')

// Oem Debug Database Guid
#define OEM_DEBUG_LEVEL_DATABASE_GUID                                                  \
    {                                                                                  \
        0x8757d72b, 0x66b7, 0x4b76, { 0xac, 0xaa, 0xfe, 0x96, 0x3a, 0xd7, 0x3a, 0x34 } \
    }

#define OEM_DEBUG_LEVEL_VARSTORE \
    efivarstore OEM_DEBUG_LEVEL_DATABASE, varid = OEM_DEBUG_LEVEL_DATABASE_VARSTORE_ID, attribute = 0x03, name = OemDebugLevel, guid = OEM_DEBUG_LEVEL_DATABASE_GUID;

// Definitions for Minimum and Maximum Error level supported values
#define MIN_ERROR_LEVEL_VALUE 0x0
#define MAX_ERROR_LEVEL_VALUE 0xFFFFFFFF

// Question IDs for IA32 and X64 entries
#define OEM_DEBUG_LEVEL_PEI_START_QUESTION_ID 0x1000
#define OEM_DEBUG_LEVEL_PEI_END_QUESTION_ID 0x1FFF
#define OEM_DEBUG_LEVEL_DXE_START_QUESTION_ID 0x2000
#define OEM_DEBUG_LEVEL_DXE_END_QUESTION_ID 0x2FFF

// Identification to load/use module Error levels
#define LOAD_DEFAULT_IDENTIFICATION_VALUE MIN_ERROR_LEVEL_VALUE
//----------------------------------------------------------------------------------

// Macro definitions
#define OEM_DEBUG_LEVEL_DATABASE_SIZE_FROM_PRIVATE_LENGTH(PrivateLength) \
    (PrivateLength - sizeof(OEM_DEBUG_LEVEL_DATABASE_HEADER))

#define OEM_DEBUG_LEVEL_DATABASE_PRIVATE_LENGTH_FROM_DATABASE_SIZE(DatabaseSize) \
    (DatabaseSize + sizeof(OEM_DEBUG_LEVEL_DATABASE_HEADER))

#define OEM_DEBUG_LEVEL_DATABASE_HEADER_FROM_PRIVATE(DynamicDebugDatabasePrivate) \
    (OEM_DEBUG_LEVEL_DATABASE_HEADER *)(&(DynamicDebugDatabasePrivate->Header))

#define OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(DynamicDebugDatabasePrivate) \
    (OEM_DEBUG_LEVEL_DATABASE *)(&(DynamicDebugDatabasePrivate->Database))

#define OEM_DEBUG_LEVEL_DATABASE_PRIVATE_FROM_PPI(This) \
    (OEM_DEBUG_LEVEL_DATABASE_PRIVATE *)*(UINTN *)((UINT8 *)This + sizeof(OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI))

#define OEM_DEBUG_LEVEL_DATABASE_PRIVATE_TO_PPI(This) \
    *(UINTN *)((UINT8 *)This + sizeof(OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI))

#define OEM_DEBUG_LEVEL_PEI_TABLE(DatabasePtr, Type) \
    (Type *)((UINT8 *)DatabasePtr + DatabasePtr->PeiTableOffset)

#define OEM_DEBUG_LEVEL_PEI_TABLE_ENTRIES(DatabasePtr) (DatabasePtr->PeiTableEntries)

#define OEM_DEBUG_LEVEL_DXE_TABLE(DatabasePtr, Type) \
    (Type *)((UINT8 *)DatabasePtr + DatabasePtr->DxeTableOffset)

#define OEM_DEBUG_LEVEL_DXE_TABLE_ENTRIES(DatabasePtr) (DatabasePtr->DxeTableEntries)

typedef struct
{
    // Version of the Database
    UINT32 Version;

    // Length includes default structure size and dynamically extended entries
    UINT32 Length;

    UINT8 DebugModeSupport;
    //Global Debug Level
    UINT32 GlobalPrintErrorLevel;

    //Phase Debug Level
    //PEI->
    UINT8 PeiPrintSupport;
    UINT8 PeiWarnLevel;
    UINT8 PeiInfoLevel;
    UINT8 PeiErrorLevel;
    UINT32 PeiPrintErrorLevel;

    //Dxe->
    UINT8 DxePrintSupport;
    UINT8 DxeWarnLevel;
    UINT8 DxeInfoLevel;
    UINT8 DxeErrorLevel;
    UINT32 DxePrintErrorLevel;

    //RTS
    UINT8 RTSPrintSupport;

    UINT8 CheckPointDebugSupport; //SGEZT#29630:Improve Debug Level Module Logic->
    UINT16 CheckPointData;//SGEZT#29630:Improve Debug Level Module Logic->
    // Offset and Number of entries of IA32 and X64 tables
    UINT32 PeiTableOffset;
    UINT32 PeiTableEntries;
    UINT32 DxeTableOffset;
    UINT32 DxeTableEntries;

    // Dynamically extended module entries go here
    //VOID Ia32Table[];            // IA32 Module file guide table
    //VOID X64Table[];             // X64 Module file guide table
} OEM_DEBUG_LEVEL_DATABASE;

// Dynamic Debug Database Header definition
typedef struct
{
    // Dynamic Debug Signature
    UINT32 Signature;
    // Length of Dynamic Debug Database Private Structure
    UINT32 PrivateLength;
    // Flag denotes whether PEI User configuration is already Synced or not
    BOOLEAN IsPeiUserConfigSynced;
    // Flag denotes whether DXE User configuration is already Synced or not
    BOOLEAN IsDxeUserConfigSynced;
    // Reserved flag
    UINT8 Reserved1[2];
} OEM_DEBUG_LEVEL_DATABASE_HEADER;

// Dynamic Debug Database Private Structure definition
// It contains Header and dynamically created Database
typedef struct
{
    OEM_DEBUG_LEVEL_DATABASE_HEADER Header;
    OEM_DEBUG_LEVEL_DATABASE Database;
} OEM_DEBUG_LEVEL_DATABASE_PRIVATE;



#define DumpDebugSetting(DataBase)\
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]--------------------------------------------------------------->\r\n", __FUNCTION__, __LINE__));\
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->PeiPrintSupport: %x \r\nOemDebugLevelDebugDatabase->PeiPrintErrorLevel: %x \n", DataBase->PeiPrintSupport, DataBase->PeiPrintErrorLevel));\
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->DxePrintSupport: %x \r\nOemDebugLevelDebugDatabase->DxePrintErrorLevel: %x \n", DataBase->DxePrintSupport, DataBase->DxePrintErrorLevel));\
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->RTSPrintSupport: %x \r\nOemDebugLevelDebugDatabase->DebugModeSupport: %x\n", DataBase->RTSPrintSupport,DataBase->DebugModeSupport));\
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->PeiTableOffset: %x \r\nOemDebugLevelDebugDatabase->PeiTableEntries: %x\n", DataBase->PeiTableOffset, DataBase->PeiTableEntries));\
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->DxeTableOffset: %x \r\nOemDebugLevelDebugDatabase->DxeTableEntries: %x\n", DataBase->DxeTableOffset, DataBase->DxeTableEntries));\
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->Length: %x \r\n", DataBase->Length));\
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]--------------------------------------------------------------->\r\n", __FUNCTION__, __LINE__));
#pragma pack(pop)

#endif