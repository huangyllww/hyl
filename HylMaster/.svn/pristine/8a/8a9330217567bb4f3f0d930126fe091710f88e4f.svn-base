#include "FileGuidNameErrorLevelTableHeader.h"
#include "uefi/UefiBaseType.h"
#include "Library/DebugLib.h"

#include "OemDebugAlways_DebugLevel.h"
#include "Library/OemDebugAlwaysLib.h"
#include "CommonLib.h"

#include "Library/HobLib.h"
#include "Base.h"

#include "Library/BaseMemoryLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Token.h"

static EFI_GUID gOEMDebugLevelDataBaseGuid = OEM_DEBUG_LEVEL_DATABASE_GUID;

/**
  Get pointer to the Dynamic Debug Database HOB data

  @retval Pointer to OEM_DEBUG_LEVEL_DATABASE HOB data configuration if successfully retried, else NULL
 */

OEM_DEBUG_LEVEL_DATABASE_PRIVATE *
    GetDynamicDebugDatabaseHobData(VOID)
{
    EFI_HOB_GUID_TYPE *pOemDebugLevelDataBaseHob = GetFirstGuidHob(&gOEMDebugLevelDataBaseGuid);

    if (pOemDebugLevelDataBaseHob)
    {
        return GET_GUID_HOB_DATA(pOemDebugLevelDataBaseHob);
    }

    return NULL;
}


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
  )
{
    UINT8 i;

    if (Guid1->Data1 != Guid2->Data1) return (Guid1->Data1 < Guid2->Data1) ? -1 : 1;
    if (Guid1->Data2 != Guid2->Data2) return Guid1->Data2 - Guid2->Data2;
    if (Guid1->Data3 != Guid2->Data3) return Guid1->Data3 - Guid2->Data3;

    for (i = 0; i<8; i++) {
        if (Guid1->Data4[i] != Guid2->Data4[i]) return Guid1->Data4[i] - Guid2->Data4[i];
    }
    return 0;
}

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
  )
{
    INT32 Low = 0;
    INT32 High = (INT32)(NumberofTableEntries-1);

    while(Low<=High){
        INT32 i;
        INT32 CompareResult;

        i = (Low+High)/2;
        CompareResult = CompareFileGuid(&(FileGuidPrintErrorLevelTable[i].Guid),Guid);
        if ( CompareResult < 0 ) Low = i+1;
        else if ( CompareResult > 0 ) High = i-1;
        else {
            // Update the Table with Input Print Error Level
            FileGuidPrintErrorLevelTable[i].PrintErrorLevel = PrintErrorLevel;
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

/**
  This function Validates User config database, checks for compatibility and updates current Database with User configuration

  @param OemDebugLevelDatabasePrivate    Pointer to Dynamic Debug Database Private Structure
  @param OemDebugLevelDataBase         Pointer to Dynamic Debug User configuration Database

  @retval EFI_STATUS                    Returns Success
**/
EFI_STATUS
SyncDebugLevelConfigWithSetupConfig(
    IN OEM_DEBUG_LEVEL_DATABASE_PRIVATE *OemDebugLevelDatabasePrivate,
    IN OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevel_SetupConfig)
{
    EFI_STATUS Status;
    UINT32 TableIndex;
    OEM_DEBUG_LEVEL_DATABASE_HEADER *OemDebugLevelDataBaseHeader = OEM_DEBUG_LEVEL_DATABASE_HEADER_FROM_PRIVATE(OemDebugLevelDatabasePrivate);
    OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevelDataBase = OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(OemDebugLevelDatabasePrivate);
    FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable = NULL;

    DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

    // If the User Config data is not available, then return.
    if (pOemDebugLevel_SetupConfig == NULL)
    {
        DEBUG((DEBUG_INFO, "pOemDebugLevel_SetupConfig is NULL... return !\n"));
        return EFI_SUCCESS;
    }

    // Check for User Config compatibility by checking Version
    if (pOemDebugLevel_SetupConfig->Version != pOemDebugLevelDataBase->Version)
    {
        DEBUG((DEBUG_ERROR, "pOemDebugLevel_SetupConfig is not compatible... return !\n"));
           //Mizl-20220304-Sync CMOS Setting+>
        {
            UINT8                                 DebugModeMask=0;

            if(pOemDebugLevelDataBase->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE) DebugModeMask|=PEI_DEBUG_ENABLED|DXE_DEBUG_ENABLED;
            if(pOemDebugLevelDataBase->DebugModeSupport & pOemDebugLevelDataBase->PeiPrintSupport)  DebugModeMask|=PEI_DEBUG_ENABLED;
            if(pOemDebugLevelDataBase->DebugModeSupport & pOemDebugLevelDataBase->DxePrintSupport)  DebugModeMask|=DXE_DEBUG_ENABLED;
            if(pOemDebugLevelDataBase->DebugModeSupport & pOemDebugLevelDataBase->RTSPrintSupport)  DebugModeMask|=RTS_DEBUG_ENABLED;

            SetDebugFlag(DebugModeMask, DEBUG_MODE_ENABLED_MASK);

        }
        return EFI_UNSUPPORTED;
    }

 
    // Check for UserConfig Sync flag
    // If it is already Synced, no need to sync with User config again.
    // Check for both PEI and DXE sync flags
    if (OemDebugLevelDataBaseHeader->IsPeiUserConfigSynced &&
        OemDebugLevelDataBaseHeader->IsDxeUserConfigSynced)
    {
        DEBUG((DEBUG_INFO, "DynamicDebugData is already Synced with User configuration... return...!!!\n"));
        return EFI_SUCCESS;
    }

    // If PEI config is not already Synced, Sync it now.
    if (OemDebugLevelDataBaseHeader->IsPeiUserConfigSynced == FALSE)
    {
        DEBUG((DEBUG_INFO, "DynamicDebugData is NOT Synced with User configuration... Sync Pei Database...!!!\n"));

        // Update Sync flag
        OemDebugLevelDataBaseHeader->IsPeiUserConfigSynced = TRUE;

        // Fill Global and Phase level values
        pOemDebugLevelDataBase->GlobalPrintErrorLevel = pOemDebugLevel_SetupConfig->GlobalPrintErrorLevel;

        pOemDebugLevelDataBase->PeiPrintSupport = pOemDebugLevel_SetupConfig->PeiPrintSupport;
        pOemDebugLevelDataBase->PeiPrintErrorLevel = pOemDebugLevel_SetupConfig->PeiPrintErrorLevel;
        pOemDebugLevelDataBase->PeiWarnLevel =pOemDebugLevel_SetupConfig->PeiWarnLevel;
        pOemDebugLevelDataBase->PeiInfoLevel = pOemDebugLevel_SetupConfig->PeiInfoLevel;
        pOemDebugLevelDataBase->PeiErrorLevel = pOemDebugLevel_SetupConfig->PeiErrorLevel;

        pOemDebugLevelDataBase->DxePrintSupport = pOemDebugLevel_SetupConfig->DxePrintSupport;
        pOemDebugLevelDataBase->DxePrintErrorLevel = pOemDebugLevel_SetupConfig->DxePrintErrorLevel;

        pOemDebugLevelDataBase->RTSPrintSupport = pOemDebugLevel_SetupConfig->RTSPrintSupport;

        pOemDebugLevelDataBase->DxeWarnLevel = pOemDebugLevel_SetupConfig->DxeWarnLevel;
        pOemDebugLevelDataBase->DxeInfoLevel = pOemDebugLevel_SetupConfig->DxeInfoLevel;
        pOemDebugLevelDataBase->DxeErrorLevel = pOemDebugLevel_SetupConfig->DxeErrorLevel;
        // Update PEI table with User Config data
        TableIndex = 0;
        while (TableIndex < pOemDebugLevel_SetupConfig->PeiTableEntries)
        {
            FileGuidPrintErrorLevelTable = OEM_DEBUG_LEVEL_PEI_TABLE(pOemDebugLevel_SetupConfig, FILE_GUID_PRINT_ERROR_LEVEL_TABLE);
            Status = SetDebugPrintErrorLevelToDatabase(
                OEM_DEBUG_LEVEL_PEI_TABLE(pOemDebugLevelDataBase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
                OEM_DEBUG_LEVEL_PEI_TABLE_ENTRIES(pOemDebugLevelDataBase),
                &(FileGuidPrintErrorLevelTable[TableIndex].Guid),
                FileGuidPrintErrorLevelTable[TableIndex].PrintErrorLevel);

            //Increase Table Index
            TableIndex++;
            DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Module Guide:%x = %r\r\n",__FUNCTION__,__LINE__,FileGuidPrintErrorLevelTable[TableIndex].Guid,Status));
        }
    }
    else
    {
        DEBUG((DEBUG_INFO, "DynamicDebugData Pei Database is already Synced...!!!\n"));
    }

    // If DXE config is not already Synced, Sync it now.
    // DXE table is special case, check for DXE entries also to Sync it
    if ((OemDebugLevelDataBaseHeader->IsDxeUserConfigSynced == FALSE) &&
        OEM_DEBUG_LEVEL_DXE_TABLE_ENTRIES(pOemDebugLevelDataBase))
    {
        DEBUG((DEBUG_INFO, "DynamicDebugData DXE database is NOT Synced with User configuration... Sync DXE Database...!!!\n"));

        // Update Sync flag
        OemDebugLevelDataBaseHeader->IsDxeUserConfigSynced = TRUE;

        // Update DXE table with User Config data
        TableIndex = 0;
        while (TableIndex < pOemDebugLevel_SetupConfig->DxeTableEntries)
        {
            FileGuidPrintErrorLevelTable = OEM_DEBUG_LEVEL_DXE_TABLE(pOemDebugLevel_SetupConfig, FILE_GUID_PRINT_ERROR_LEVEL_TABLE);
            Status = SetDebugPrintErrorLevelToDatabase(
                OEM_DEBUG_LEVEL_DXE_TABLE(pOemDebugLevelDataBase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
                OEM_DEBUG_LEVEL_DXE_TABLE_ENTRIES(pOemDebugLevelDataBase),
                &(FileGuidPrintErrorLevelTable[TableIndex].Guid),
                FileGuidPrintErrorLevelTable[TableIndex].PrintErrorLevel);

            //Increase Table Index
            TableIndex++;
        }
    }

    //Mizl-20220304-Sync CMOS Setting+>
    {
        UINT8                                 DebugModeMask=0;

        if(pOemDebugLevelDataBase->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE) DebugModeMask|=PEI_DEBUG_ENABLED|DXE_DEBUG_ENABLED;
        if(pOemDebugLevelDataBase->DebugModeSupport & pOemDebugLevelDataBase->PeiPrintSupport)  DebugModeMask|=PEI_DEBUG_ENABLED;
        if(pOemDebugLevelDataBase->DebugModeSupport & pOemDebugLevelDataBase->DxePrintSupport)  DebugModeMask|=DXE_DEBUG_ENABLED;
        if(pOemDebugLevelDataBase->DebugModeSupport & pOemDebugLevelDataBase->RTSPrintSupport)  DebugModeMask|=RTS_DEBUG_ENABLED;

        SetDebugFlag(DebugModeMask, DEBUG_MODE_ENABLED_MASK);

    }
    DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

    return EFI_SUCCESS;
}

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
  )
{
    INT32 Low = 0;
    INT32 High = (INT32)(NumberofTableEntries-1);

    while(Low<=High){
        INT32 i;
        INT32 CompareResult;

        i = (Low+High)/2;
        CompareResult = CompareFileGuid(&(FileGuidPrintErrorLevelTable[i].Guid),Guid);
        if ( CompareResult < 0 ) Low = i+1;
        else if ( CompareResult > 0 ) High = i-1;
        else {
            // LOAD_DEFAULT_IDENTIFICATION_VALUE is magic number to check if table is configured.
            // If it is configured, value found in table is returned.
            if (FileGuidPrintErrorLevelTable[i].PrintErrorLevel != LOAD_DEFAULT_IDENTIFICATION_VALUE) {
                *PrintErrorLevel = FileGuidPrintErrorLevelTable[i].PrintErrorLevel;
            } else {
                // If not configured, input PrintErrorLevel is returned with out modification.
                // And input Print Error Level value will be updated in the database.
                FileGuidPrintErrorLevelTable[i].PrintErrorLevel = *PrintErrorLevel;
            }
            return EFI_SUCCESS;
        }
    }
    return EFI_NOT_FOUND;
}

/**
  Creates Default File Guid Print Error Level Table from File Guid Name Table
    1) Copy the File Guid in the Table
    2) Fill the default Print Error Level Mask for each File Guid

  @param FileNameGuidTable              Pointer to File Guid Name Table.
  @param NumberofTableEntries           Number of entries in FileNameGuidTable
  @param FileGuidPrintErrorLevelTable   Pointer to File Guid PrintErrorLevel Table.
**/
VOID CreateDefaultFileGuidPrintErrorLevelTable(
    IN CONST FILE_GUID_NAME_TABLE *FileNameGuidTable,
    IN UINT32 NumberofTableEntries,
    IN FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable)
{
    // Calculate File Guid Print Error Level Table Size
    UINT32 TableSize = NumberofTableEntries * sizeof(FILE_GUID_PRINT_ERROR_LEVEL_TABLE);

    // File Guid-Error Level table
    if (TableSize)
    {
        UINT32 Index = 0;
        // Go through Auto generated Module/File Guid Name table and create File Guid-Error Level table
        for (Index = 0; Index < NumberofTableEntries; Index++)
        {
            //Copy Guid
            CopyGuid(&(FileGuidPrintErrorLevelTable[Index].Guid), &(FileNameGuidTable[Index].Guid));
            // Assign default Print Error Level
            FileGuidPrintErrorLevelTable[Index].PrintErrorLevel = LOAD_DEFAULT_IDENTIFICATION_VALUE;
        }
    }
}

/**
  This function Builds Dynamic Debug database from Build generated tables and maps user configuration to created database

  @param pOemDebugLevel_SetupConfig         Pointer to Dynamic Debug User configuration Database
  @param IsPeiPhase                     Flag to determine if Phase of execution is PEI

  @retval OemDebugLevelDatabasePrivate    Pointer to Dynamic Debug Database Private Structure
**/
OEM_DEBUG_LEVEL_DATABASE_PRIVATE *
BuildOemDebugLevelDataBase(
    IN OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevel_SetupConfig,
    IN BOOLEAN IsPeiPhase)
{
    OEM_DEBUG_LEVEL_DATABASE_PRIVATE *OemDebugLevelDatabasePrivate = NULL;
    OEM_DEBUG_LEVEL_DATABASE_PRIVATE *OemDebugLevelDatabaseHobPrivate = NULL;
    OEM_DEBUG_LEVEL_DATABASE_HEADER *OemDebugLevelDebugDatabaseHeader = NULL;
    OEM_DEBUG_LEVEL_DATABASE *DynamicDebugHobDatabase = NULL;
    OEM_DEBUG_LEVEL_DATABASE *OemDebugLevelDebugDatabase = NULL;
    BOOLEAN IsValidHobFound = FALSE;
    UINT32 DatabaseSize = 0;
    UINT32 PrivateLength = 0;
    FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable = NULL;
    UINT32 SizeofPeiTable = 0;
    UINT32 SizeofDxeTable = 0;

    DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

    // Get PEI and DXE Tables with Size
    SizeofPeiTable = GetDebugModule_PeiTableSize();
    SizeofDxeTable = GetDebugModule_DxeTableSize();
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]SizeofPeiTable:%x SizeofDxeTable:%x\r\n",__FUNCTION__,__LINE__,SizeofPeiTable,SizeofDxeTable));


    // Private Length includes
    // 1. Default Private Structure Size
    // 2. Build Generated PEI Table Size
    // 3. Build Generated DXE Table Size
    PrivateLength = sizeof(OEM_DEBUG_LEVEL_DATABASE_PRIVATE) + SizeofPeiTable + SizeofDxeTable;

    // IsPeiPhase flag is TRUE for PEI and FALSE for DXE phase
    if (IsPeiPhase)
    {

        // Create Hob in PEI phase
        OemDebugLevelDatabasePrivate = BuildGuidHob(
            &gOEMDebugLevelDataBaseGuid,
            PrivateLength);
        if (OemDebugLevelDatabasePrivate == NULL)
        {
            DEBUG((DEBUG_ERROR, "Failed to allocate memory for OemDebugLevelDatabasePrivate! \n"));
            return NULL;
        }
        DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]OemDebugLevelDatabasePrivate:%x\r\n",__FUNCTION__,__LINE__,OemDebugLevelDatabasePrivate));
        // Clear the memory allocated
        ZeroMem(OemDebugLevelDatabasePrivate, PrivateLength);
    }
    else
    {
        // Locate Hob and use in DXE phase
        OemDebugLevelDatabaseHobPrivate = GetDynamicDebugDatabaseHobData();

        // Validate PEI Hob
        if ((OemDebugLevelDatabaseHobPrivate != NULL) &&
            (OemDebugLevelDatabaseHobPrivate->Header.Signature == OEM_DEBUG_LEVEL_SIGNATURE))
        {
            // Valid Hob is found
            IsValidHobFound = TRUE;

            // Update Private Length
            PrivateLength = OemDebugLevelDatabaseHobPrivate->Header.PrivateLength;

            // Hob Data contains only PEI Table size so add DXE size to it
            PrivateLength += SizeofDxeTable;
        }

        // We are in Dxe
        // Allocate memory for Dynamic Debug Database Private structure
        //
        OemDebugLevelDatabasePrivate = (OEM_DEBUG_LEVEL_DATABASE_PRIVATE *)AllocateZeroPool(PrivateLength);
        if (OemDebugLevelDatabasePrivate == NULL)
        {
            DEBUG((DEBUG_ERROR, "Failed to allocate memory for OemDebugLevelDatabasePrivate! \n"));
            return NULL;
        }
    }

    // Get Pointer to Dynamic Debug Header and Database
    OemDebugLevelDebugDatabaseHeader = OEM_DEBUG_LEVEL_DATABASE_HEADER_FROM_PRIVATE(OemDebugLevelDatabasePrivate);
    OemDebugLevelDebugDatabase = OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(OemDebugLevelDatabasePrivate);
    DatabaseSize = OEM_DEBUG_LEVEL_DATABASE_SIZE_FROM_PRIVATE_LENGTH(PrivateLength);
    
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]OemDebugLevelDebugDatabaseHeader:%x\r\n",__FUNCTION__,__LINE__,OemDebugLevelDebugDatabaseHeader));
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]OemDebugLevelDebugDatabase:%x\r\n",__FUNCTION__,__LINE__,OemDebugLevelDebugDatabase));
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]DatabaseSize:%x\r\n",__FUNCTION__,__LINE__,DatabaseSize));

    DEBUG((DEBUG_INFO, "IsValidHobFound: %x \n", IsValidHobFound));

    // Based on Hob, fill Database
    if (IsValidHobFound)
    {
        //Copy the HobData into Allocated memory
        DEBUG((DEBUG_INFO, "Create Database from Hob Data... \n"));
        CopyMem(OemDebugLevelDatabasePrivate, OemDebugLevelDatabaseHobPrivate, OemDebugLevelDatabaseHobPrivate->Header.PrivateLength);
    }
    else
    {
        //Construct Database from Build generated files for only PEI Table
        DEBUG((DEBUG_INFO, "Create Database from Build generated files... \n"));

        // Fill Data base Header
        OemDebugLevelDebugDatabaseHeader->Signature = OEM_DEBUG_LEVEL_SIGNATURE;
        OemDebugLevelDebugDatabaseHeader->IsPeiUserConfigSynced = FALSE;
        OemDebugLevelDebugDatabaseHeader->IsDxeUserConfigSynced = FALSE;

        // Fill Data base Structure
        OemDebugLevelDebugDatabase->Version = OEM_DEBUG_LEVEL_DATABASE_TABLE_VERSION;

        OemDebugLevelDebugDatabase->DebugModeSupport = DEFAULT_DEBUG_MODE_VALUE;
        // Fill Global and Phase level defaults
        OemDebugLevelDebugDatabase->GlobalPrintErrorLevel = DEFAULT_GLOBAL_ERROR_LEVEL_VALUE;

        OemDebugLevelDebugDatabase->PeiPrintSupport = DEFAULT_PEI_PRINT_SUPPORT;
        OemDebugLevelDebugDatabase->PeiPrintErrorLevel = DEFAULT_PEI_ERROR_LEVEL_VALUE;
        OemDebugLevelDebugDatabase->PeiErrorLevel = DEFAULT_PEI_ERROR_SUPPORT;
        OemDebugLevelDebugDatabase->PeiInfoLevel = DEFAULT_PEI_INFO_SUPPORT;
        OemDebugLevelDebugDatabase->PeiWarnLevel = DEFAULT_PEI_WARN_SUPPORT;

        OemDebugLevelDebugDatabase->DxePrintSupport = DEFAULT_DXE_PRINT_SUPPORT;
        OemDebugLevelDebugDatabase->DxePrintErrorLevel = DEFAULT_DXE_ERROR_LEVEL_VALUE;
        OemDebugLevelDebugDatabase->DxeErrorLevel = DEFAULT_DXE_ERROR_SUPPORT;
        OemDebugLevelDebugDatabase->DxeInfoLevel = DEFAULT_DXE_INFO_SUPPORT;
        OemDebugLevelDebugDatabase->DxeWarnLevel = DEFAULT_DXE_WARN_SUPPORT;

        OemDebugLevelDebugDatabase->RTSPrintSupport = DEFAULT_RTS_PRINT_SUPPORT;
        // Fill PEI Table
        OemDebugLevelDebugDatabase->PeiTableOffset = sizeof(OEM_DEBUG_LEVEL_DATABASE);
        OemDebugLevelDebugDatabase->PeiTableEntries = SizeofPeiTable / sizeof(FILE_GUID_PRINT_ERROR_LEVEL_TABLE);

        // Copy PEI table
        FileGuidPrintErrorLevelTable = OEM_DEBUG_LEVEL_PEI_TABLE(OemDebugLevelDebugDatabase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE);
        CreateDefaultFileGuidPrintErrorLevelPeiTable(FileGuidPrintErrorLevelTable);

        // Fill DXE Table Offset and make entries as ZERO, it will be updated later
        OemDebugLevelDebugDatabase->DxeTableOffset = OemDebugLevelDebugDatabase->PeiTableOffset + SizeofPeiTable;
        OemDebugLevelDebugDatabase->DxeTableEntries = 0;
    }

    //Update database DXE Table if available
    if (SizeofDxeTable)
    {
        DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]Update Dxe Table Entry\r\n",__FUNCTION__,__LINE__));
        // Fill DXE Table Entries
        OemDebugLevelDebugDatabase->DxeTableEntries = SizeofDxeTable / sizeof(FILE_GUID_PRINT_ERROR_LEVEL_TABLE);
        // Copy DXE table
        FileGuidPrintErrorLevelTable = OEM_DEBUG_LEVEL_DXE_TABLE(OemDebugLevelDebugDatabase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE);
        CreateDefaultFileGuidPrintErrorLevelDxeTable(FileGuidPrintErrorLevelTable);
    }

    // Update Lengths of the Database and Private structure
    OemDebugLevelDebugDatabaseHeader->PrivateLength = PrivateLength;
    OemDebugLevelDebugDatabase->Length = DatabaseSize;
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]--------------------------------------------------------------->\r\n",__FUNCTION__,__LINE__));
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->PeiPrintSupport: %x \r\nOemDebugLevelDebugDatabase->PeiPrintErrorLevel: %x \n", OemDebugLevelDebugDatabase->PeiPrintSupport, OemDebugLevelDebugDatabase->PeiPrintErrorLevel));
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->DxePrintSupport: %x \r\nOemDebugLevelDebugDatabase->DxePrintErrorLevel: %x \n", OemDebugLevelDebugDatabase->DxePrintSupport, OemDebugLevelDebugDatabase->DxePrintErrorLevel));

    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->PeiTableOffset: %x \r\nOemDebugLevelDebugDatabase->PeiTableEntries: %x\n", OemDebugLevelDebugDatabase->PeiTableOffset, OemDebugLevelDebugDatabase->PeiTableEntries));
    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->DxeTableOffset: %x \r\nOemDebugLevelDebugDatabase->DxeTableEntries: %x\n", OemDebugLevelDebugDatabase->DxeTableOffset, OemDebugLevelDebugDatabase->DxeTableEntries));

    DEBUG((DEBUG_INFO, "OemDebugLevelDebugDatabase->Length: %x \r\n", OemDebugLevelDebugDatabase->Length));
    DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]--------------------------------------------------------------->\r\n",__FUNCTION__,__LINE__));

    // Update Database with User Config data
    SyncDebugLevelConfigWithSetupConfig(OemDebugLevelDatabasePrivate, pOemDebugLevel_SetupConfig);

    DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

    return OemDebugLevelDatabasePrivate;
}