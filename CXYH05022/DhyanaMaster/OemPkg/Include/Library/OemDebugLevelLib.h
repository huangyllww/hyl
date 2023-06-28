#ifndef __OEM_DEBUG_LEVEL_LIB_H__
#define __OEM_DEBUG_LEVEL_LIB_H__

#include "OemDebugAlways.h"
#include "Uefi/UefiBaseType.h"
#include "FileGuidNameErrorLevelTableHeader.h"
/**
  Set Debug Config Data

  @retval EFI_STATUS Success for successful User configuration setting, else error.
 */
EFI_STATUS
SetOemDebugLevelDataBaseConfig (
  IN  OEM_DEBUG_LEVEL_DATABASE    *OemDebugLevelConfigData
  );


/**
  Create Default File Guid Print Error Level PEI Table from Build generated File Guid Name Table

  @param  FileGuidPrintErrorLevelTable    Pointer to File Guid Print Error Level Table to be updated
**/
VOID
CreateDefaultFileGuidPrintErrorLevelPeiTable (
  IN  FILE_GUID_PRINT_ERROR_LEVEL_TABLE       *FileGuidPrintErrorLevelTable
  );



OEM_DEBUG_LEVEL_DATABASE *
GetOemDebugLevelDataBase();
/**
  Creates Default File Guid Print Error Level Table from File Guid Name Table
    1) Copy the File Guid in the Table
    2) Fill the default Print Error Level Mask for each File Guid

  @param FileNameGuidTable              Pointer to File Guid Name Table.
  @param NumberofTableEntries           Number of entries in FileNameGuidTable
  @param FileGuidPrintErrorLevelTable   Pointer to File Guid PrintErrorLevel Table.
**/
VOID
CreateDefaultFileGuidPrintErrorLevelTable (
  IN  CONST FILE_GUID_NAME_TABLE                *FileNameGuidTable,
  IN        UINT32                              NumberofTableEntries,
  IN        FILE_GUID_PRINT_ERROR_LEVEL_TABLE   *FileGuidPrintErrorLevelTable
  );

EFI_STATUS
SyncDebugModeWithCMOS(
    IN OEM_DEBUG_LEVEL_DATABASE *ODDB);


VOID CheckPointDebug(IN UINT16 Checkpoint);//SGEZT#29630:Improve Debug Level Module Logic->
#endif