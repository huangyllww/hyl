#ifndef __BUILD_OEM_DEBUG_LEVEL_DATA_BASE_SERVICE_H__
#define __BUILD_OEM_DEBUG_LEVEL_DATA_BASE_SERVICE_H__

#include "OemDebugAlways_DebugLevel.h"
#include "uefi/UefiBaseType.h"

static EFI_GUID gOEMDebugLevelFormSetGuid = OEM_DEBUG_LEVEL_FORM_SET_GUID;
//SGEZT#29630:Improve Debug Level Module Logic+>
#define TransferModuleErrorLevel(DebugLevelBase,x)\
    (DebugLevelBase->##x##WarnLevel?BIT1:0)|(DebugLevelBase->##x##InfoLevel?BIT6:0)|(DebugLevelBase->##x##ErrorLevel?BIT31:0)
//SGEZT#29630:Improve Debug Level Module Logic->
/**
  This function Builds Dynamic Debug database from Build generated tables and maps user configuration to created database

  @param DynamicDebugUserConfig         Pointer to Dynamic Debug User configuration Database
  @param IsPeiPhase                     Flag to determine if Phase of execution is PEI

  @retval DynamicDebugDatabasePrivate    Pointer to Dynamic Debug Database Private Structure
**/
OEM_DEBUG_LEVEL_DATABASE_PRIVATE*
BuildOemDebugLevelDataBase (
  IN  OEM_DEBUG_LEVEL_DATABASE    *pOemDebugLevel_SetupConfig,
  IN  BOOLEAN                     IsPeiPhase
  );


/**
  Calculate Size of the default File Guid Print Error Level PEI Table from Build generated File Guid Name Table Size

  @retval UINT32    Size of the default File Guid Print Error Level PEI Table
**/
UINT32
GetDebugModule_PeiTableSize (VOID);

/**
  Calculate Size of the default File Guid Print Error Level DXE Table from Build generated File Guid Name Table Size

  @retval UINT32    Size of the default File Guid Print Error Level DXE Table
**/
UINT32
GetDebugModule_DxeTableSize (VOID);


/**
  Get pointer to the Dynamic Debug Database HOB data

  @retval Pointer to DYNAMIC_DEBUG_DATABASE HOB data configuration if successfully retried, else NULL
 */

OEM_DEBUG_LEVEL_DATABASE_PRIVATE*
GetDynamicDebugDatabaseHobData (VOID);


/**
  Create Default File Guid Print Error Level PEI Table from Build generated File Guid Name Table

  @param  FileGuidPrintErrorLevelTable    Pointer to File Guid Print Error Level Table to be updated
**/
VOID
CreateDefaultFileGuidPrintErrorLevelPeiTable (
  IN  FILE_GUID_PRINT_ERROR_LEVEL_TABLE       *FileGuidPrintErrorLevelTable
  );


/**
  Create Default File Guid Print Error Level DXE Table from Build generated File Guid Name Table

  @param  FileGuidPrintErrorLevelTable    Pointer to File Guid Print Error Level Table to be updated
**/
VOID
CreateDefaultFileGuidPrintErrorLevelDxeTable (
  IN  FILE_GUID_PRINT_ERROR_LEVEL_TABLE       *FileGuidPrintErrorLevelTable
  );


/**
  This function searches file Guid in the table provided and sets Error Level if Guid is found in the table.

  @param FileGuidPrintErrorLevelTable   Pointer to File Guid PrintErrorLevel Table.
  @param NumberofTableEntries           Number of entries in FileGuidPrintErrorLevelTable
  @param Guid                           File Guid to be searched in the table provided
  @param PrintErrorLevel                Debug Print Error Level to be Set/Configured

  @retval EFI_STATUS                    Returns Success if Guid is found in Database, else Not Found
**/
EFI_STATUS
SetDebugPrintErrorLevelToDatabase (
  IN        FILE_GUID_PRINT_ERROR_LEVEL_TABLE   *FileGuidPrintErrorLevelTable, 
  IN        UINT32                              NumberofTableEntries, 
  IN CONST  EFI_GUID                            *Guid, 
  IN        UINT32                              PrintErrorLevel
  );

/**
  This function compares both input Guids and returns key to use in Search algorithm

  @param Guid1      Pointer to Guid1.
  @param Guid2      Pointer to Guid2.

  @retval INT32     Returns ZERO, if both the Guids are same, else difference of first different member of EFI_GUID structure
**/
INT32
CompareFileGuid (
  IN  CONST EFI_GUID *Guid1,
  IN  CONST EFI_GUID *Guid2
  );


/**
  This function searches file Guid in the table provided and returns Error Level if Guid is found in the table.

  @param FileGuidPrintErrorLevelTable   Pointer to File Guid PrintErrorLevel Table.
  @param NumberofTableEntries           Number of entries in FileGuidPrintErrorLevelTable
  @param Guid                           File Guid to be searched in the table provided
  @param PrintErrorLevel                Pointer to update current Debug Print Error Level

  @retval EFI_STATUS                    Returns Success if Guid is found in Database, else Not Found
**/
EFI_STATUS
GetDebugPrintErrorLevelFromDatabase (
  IN        FILE_GUID_PRINT_ERROR_LEVEL_TABLE   *FileGuidPrintErrorLevelTable, 
  IN        UINT32                              NumberofTableEntries, 
  IN CONST  EFI_GUID                            *Guid, 
  IN OUT    UINT32                              *PrintErrorLevel
  );

#endif