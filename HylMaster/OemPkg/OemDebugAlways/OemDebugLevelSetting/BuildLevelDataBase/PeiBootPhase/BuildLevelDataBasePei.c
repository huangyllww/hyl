
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Ppi/OemDebugLevelSerivce.h>
#include "uefi/UefiBaseType.h"
#include "Ppi/ReadOnlyVariable2.h"
#include "Library/PeiServicesLib.h"

#include "OemDebugAlways_DebugLevel.h"
#include "Library/OemDebugLevelLib.h"
#include "../CommonLib/CommonLib.h"

#include "OemDebugLevel_PeiModule.c"

#include <Library/OemDebugAlwaysLib.h>

#include "OemLib.h"

static EFI_GUID gOEMDebugLevelDataBaseGuid = OEM_DEBUG_LEVEL_DATABASE_GUID;

EFI_PEI_READ_ONLY_VARIABLE2_PPI *PeiVariable = NULL;

BOOLEAN gPeiDebugModeFlag=TRUE;
/**
  Set Debug Config Data

  @retval EFI_STATUS Success for successful User configuration setting, else error.
 */
EFI_STATUS
SetOemDebugLevelDataBaseConfig(
    IN OEM_DEBUG_LEVEL_DATABASE *OemDebugLevelConfigData)
{
  // Nvram variable cannot be set in Pei Phase
  // So return Unsupported
  return EFI_UNSUPPORTED;
}
/**
  Get/Retrieve Debug Data Base Pointer

  @retval Pointer to OEM_DEBUG_LEVEL_DATABASE configuration if sucessfully retried, else NULL
 */
OEM_DEBUG_LEVEL_DATABASE *
    GetOemDebugLevelDataBaseConfigPointer(VOID)
{
  EFI_STATUS Status;
  UINTN Size;
  OEM_DEBUG_LEVEL_DATABASE *OemDebugLevelConfigData = NULL;

  MODULE_START

  if (PeiVariable == NULL)
  {
    // Locate the Read only Variable Ppi to Get Nvram Variable
    Status = PeiServicesLocatePpi(
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0,
        NULL,
        (VOID **)&PeiVariable);
    DEBUG((DEBUG_INFO, " gEfiPeiReadOnlyVariable2PpiGuid status %r\n", Status));
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status))
      return NULL;
  }
  // Get DynamicDebug Nvram Variable
  Size = 0;

  Status = PeiVariable->GetVariable(
      PeiVariable,
      OEM_DEBUG_LEVEL_DATA_NAME,
      &gOEMDebugLevelDataBaseGuid,
      NULL,
      &Size,
      NULL);
  DEBUG((DEBUG_INFO, " PeiVariable->GetVariable status %r Size: %x \n", Status, Size));

  // Since the Nvram variable is used in Vfr/Sd files,
  // AMI BIOS creates default variable with default size of OEM_DEBUG_LEVEL_DATABASE.
  // The proper Nvram variable will contain module entries along with default size
  // If the size is equal to OEM_DEBUG_LEVEL_DATABASE, it's a false positive so ignore it.
  //Mizl-20220215- Fix Function Error+>
  // if ( (Status == EFI_BUFFER_TOO_SMALL) &&\
    //         (Size != sizeof(OEM_DEBUG_LEVEL_DATABASE)) ) {
  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    //Mizl-20220215- Fix Function Error->
    // Allocate memory for Dynamic Debug Nvram Data
    OemDebugLevelConfigData = (OEM_DEBUG_LEVEL_DATABASE *)AllocateZeroPool(Size);
    if (OemDebugLevelConfigData == NULL)
    {
      DEBUG((DEBUG_ERROR, "Failed to allocate memory for OemDebugLevelConfigData! \n"));
      return NULL;
    }

    // Get DynamicDebug Nvram Variable
    Status = PeiVariable->GetVariable(
        PeiVariable,
        OEM_DEBUG_LEVEL_DATA_NAME,
        &gOEMDebugLevelDataBaseGuid,
        NULL,
        &Size,
        OemDebugLevelConfigData);
    DEBUG((DEBUG_INFO, " PeiVariable->GetVariable status %r Size: %x \n", Status, Size));
    if (EFI_ERROR(Status))
    {
      // Free the allocated memory for Nvram data
      FreePool(OemDebugLevelConfigData);
      return NULL;
    }
  }

  DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

  return OemDebugLevelConfigData;
}
/**
  This function checks and fixes up the Dynamic Debug Database pointer after the services/data
  are migrated to permanent memory from Cache.

  @param This           Pointer to Ami Debug Print Error Level Service Ppi

  @retval EFI_STATUS    Returns error if database is corrupted after fixing up the pointer, else success
**/
EFI_STATUS
FixUpDynamicDatabasePointer(
    IN OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *This)
{
  // Get pointer to Dynamic Database Private Structure
  OEM_DEBUG_LEVEL_DATABASE_PRIVATE *pOemDebugLevelDataBasePrivate = OEM_DEBUG_LEVEL_DATABASE_PRIVATE_FROM_PPI(This);

  // Validate Signature
  if (pOemDebugLevelDataBasePrivate->Header.Signature != OEM_DEBUG_LEVEL_SIGNATURE)
  {
    // After memory is discovered, DynamicDebug database pointer needs fix up.
    // If signature does not match, fix it up.
    pOemDebugLevelDataBasePrivate = GetDynamicDebugDatabaseHobData();
    // Validate Signature with fix up pointer
    if ((pOemDebugLevelDataBasePrivate == NULL) ||
        (pOemDebugLevelDataBasePrivate->Header.Signature != OEM_DEBUG_LEVEL_SIGNATURE))
    {
      ASSERT(0);
      return EFI_VOLUME_CORRUPTED;
    }

    // Update fixed up Database pointer
    OEM_DEBUG_LEVEL_DATABASE_PRIVATE_TO_PPI(This) = (UINTN)pOemDebugLevelDataBasePrivate;
  }

  return EFI_SUCCESS;
}
/**
  Set/Configure Debug Print Error Level for a module in Database

  @param  This          Pointer to Ami Debug Print Error Level Service Ppi
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Debug Print Error Level to be Set/Configured

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
gOemDebugSetDebugLevelToDatabase(
    IN OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *This,
    IN EFI_GUID *ModuleGuid,
    IN UINT32 ErrorLevel)
{
  OEM_DEBUG_LEVEL_DATABASE_PRIVATE *pOemDebugLevelDataBasePrivate = NULL;
  OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevelDataBase = NULL;
  EFI_STATUS Status;

  // Perform Fix Up
  Status = FixUpDynamicDatabasePointer(This);
  ASSERT(!EFI_ERROR(Status));
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  // Get pointer to Dynamic Database Private Structure
  pOemDebugLevelDataBasePrivate = OEM_DEBUG_LEVEL_DATABASE_PRIVATE_FROM_PPI(This);

  // Get Pointer to Dynamic Debug Database
  pOemDebugLevelDataBase = OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(pOemDebugLevelDataBasePrivate);

  // Set Debug Print Error Level to Database
  return SetDebugPrintErrorLevelToDatabase(
      OEM_DEBUG_LEVEL_PEI_TABLE(pOemDebugLevelDataBase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
      OEM_DEBUG_LEVEL_PEI_TABLE_ENTRIES(pOemDebugLevelDataBase),
      ModuleGuid, ErrorLevel);
}
/**
  Set/Configure Debug Print Error Level for a module

  @param  This          Pointer to Ami Debug Print Error Level Service Ppi
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Debug Print Error Level to be Set/Configured

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
gOemDebugSetDebugLevel(
    IN OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *This,
    IN EFI_GUID *ModuleGuid,
    IN UINT32 ErrorLevel)
{

  // Validate Input parameters
  if ((This == NULL) || (ModuleGuid == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  // Set Debug Print Error Level to Database
  return gOemDebugSetDebugLevelToDatabase(This, ModuleGuid, ErrorLevel);
}
/**
  Get configured Debug Print Error Level for a module from Database
  Checks user configuration:
      a) If Global error level is enabled, return Global Error Level
      b) Else if Phase error level is enabled, return Phase Error Level
      c) Else return Error level for module

  @param  This          Pointer to Ami Debug Print Error Level Service Ppi
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Pointer to return current Debug Print Error Level

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
gOemDebugGetDebugLevelFromDatabase(
    IN OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *This,
    IN EFI_GUID *ModuleGuid,
    IN OUT UINT32 *ErrorLevel)
{
  OEM_DEBUG_LEVEL_DATABASE_PRIVATE *pOemDebugLevelDataBasePrivate = NULL;
  OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevelDataBase = NULL;
  EFI_STATUS Status;
  UINT32 ModuleErrorLevel = *ErrorLevel;

  // Perform Fix Up if needed
  Status = FixUpDynamicDatabasePointer(This);
  ASSERT(!EFI_ERROR(Status));
  if (EFI_ERROR(Status))
  {
    return Status;
  }

  // Get pointer to Dynamic Database Private Structure
  pOemDebugLevelDataBasePrivate = OEM_DEBUG_LEVEL_DATABASE_PRIVATE_FROM_PPI(This);

  // Get Pointer to Dynamic Debug Database
  pOemDebugLevelDataBase = OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(pOemDebugLevelDataBasePrivate);

  if(!pOemDebugLevelDataBase->DebugModeSupport)
  {
    *ErrorLevel = MIN_ERROR_LEVEL_VALUE;
    return EFI_SUCCESS;
  }
  
  if (pOemDebugLevelDataBase->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE)
  {
    *ErrorLevel = pOemDebugLevelDataBase->GlobalPrintErrorLevel;
    return EFI_SUCCESS;
  }

  if (pOemDebugLevelDataBase->PeiPrintSupport)
  {
    if (pOemDebugLevelDataBase->PeiPrintErrorLevel != MIN_ERROR_LEVEL_VALUE)
      *ErrorLevel = pOemDebugLevelDataBase->PeiPrintErrorLevel;
    else
      *ErrorLevel = TransferModuleErrorLevel(pOemDebugLevelDataBase, Pei);

    return EFI_SUCCESS;
  }

  // Check PEI Database for Module level
  Status = GetDebugPrintErrorLevelFromDatabase(
      OEM_DEBUG_LEVEL_PEI_TABLE(pOemDebugLevelDataBase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
      OEM_DEBUG_LEVEL_PEI_TABLE_ENTRIES(pOemDebugLevelDataBase),
      ModuleGuid, &ModuleErrorLevel);
  if (!EFI_ERROR(Status))
  {
    *ErrorLevel = ModuleErrorLevel;
    return Status;
  }
  return Status;
}
/**
  Get configured Debug Print Error Level for a module

  @param  This          Pointer to Ami Debug Print Error Level Service Ppi
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Pointer to return current Debug Print Error Level

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
gOemDebugGetDebugLevel(
    IN OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *This,
    IN EFI_GUID *ModuleGuid,
    IN OUT UINT32 *ErrorLevel)
{
  // Validate Input parameters
  if ((This == NULL) || (ModuleGuid == NULL) || (ErrorLevel == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  // Get Debug Print Error Level from Database
  return gOemDebugGetDebugLevelFromDatabase(This, ModuleGuid, ErrorLevel);
}

/**
  Calculate Size of the default File Guid Print Error Level PEI Table from Build generated File Guid Name Table Size

  @retval UINT32    Size of the default File Guid Print Error Level PEI Table
**/
UINT32
GetDebugModule_PeiTableSize(VOID)
{
  // PEI Table Size
  return (UINT32)(NumberofElementsInBuildToolGeneratedPeiModuleTable * sizeof(FILE_GUID_PRINT_ERROR_LEVEL_TABLE));
}

/**
  Calculate Size of the default File Guid Print Error Level DXE Table from Build generated File Guid Name Table Size

  @retval UINT32    Size of the default File Guid Print Error Level DXE Table
**/
UINT32
GetDebugModule_DxeTableSize(VOID)
{
  // PEI does not have Dxe table included
  // So return size as ZERO
  return 0;
}

/**
  Create Default File Guid Print Error Level PEI Table from Build generated File Guid Name Table

  @param  FileGuidPrintErrorLevelTable    Pointer to File Guid Print Error Level Table to be updated
**/
VOID CreateDefaultFileGuidPrintErrorLevelPeiTable(
    IN FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable)
{
  // Create PEI Table
  CreateDefaultFileGuidPrintErrorLevelTable(
      (FILE_GUID_NAME_TABLE *)BuildToolGeneratedPeiModuleTable,
      (UINT32)NumberofElementsInBuildToolGeneratedPeiModuleTable,
      FileGuidPrintErrorLevelTable);
}

/**
  Create Default File Guid Print Error Level DXE Table from Build generated File Guid Name Table

  @param  FileGuidPrintErrorLevelTable    Pointer to File Guid Print Error Level Table to be updated
**/
VOID CreateDefaultFileGuidPrintErrorLevelDxeTable(
    IN FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable)
{
  // PEI does not have DXE table included
  // ASSERT if function is called
  ASSERT(0);
}

/**
GC_TODO:  CheckDebugModeSetting

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
CheckDebugModeSetting(
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN VariableSize = sizeof(OEM_COMMON_SETUP_DATA);
  OEM_COMMON_SETUP_DATA OemSetupData;
  EFI_GUID OemSetupVariableGuid = OEM_COMMON_VAR_GUID;

  MODULE_START

  if (PeiVariable == NULL)
  {
    // Locate the Read only Variable Ppi to Get Nvram Variable
    Status = PeiServicesLocatePpi(
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0,
        NULL,
        (VOID **)&PeiVariable);
    if (EFI_ERROR(Status))
      MODULE_END(Status)
  }

  Status = PeiVariable->GetVariable(PeiVariable,
                                    L"OEM_VAR",
                                    &OemSetupVariableGuid,
                                    NULL,
                                    &VariableSize,
                                    &OemSetupData);

  if (EFI_ERROR(Status))
    MODULE_END(Status)

  if (!OemSetupData.DebugModeMask)
    MODULE_END(Status = EFI_UNSUPPORTED)

  MODULE_END(Status = EFI_SUCCESS)
}
/**

    This is entry point of the PEI Driver that 
    1) Builds default Dynamic Debug database for PEI Phase as HOB from PEI auto generated file
    2) Retrieves User configuration settings and updates database
    3) Publishes AmiDebugPrintErrorLevel PPI Service

    @param FileHandle    Pointer to image file handle.
    @param PeiServices   Pointer to the PEI Core data Structure

    @return EFI_STATUS  Status of Driver execution
    @retval EFI_SUCCESS Successful driver initialization
**/
EFI_STATUS
BuildOemDebugLevelDataBasePeiEntry(
    IN EFI_PEI_FILE_HANDLE FileHandle,
    IN CONST EFI_PEI_SERVICES **PeiServices)
{
  EFI_STATUS Status = EFI_SUCCESS;
  OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevel_SetupConfig = NULL;
  OEM_DEBUG_LEVEL_DATABASE_PRIVATE *pOemDebugLevelDataBasePrivate = NULL;
  UINTN Size = 0;
  OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *gOemDebugLevelServicePPI = NULL;
  EFI_PEI_PPI_DESCRIPTOR *PpiDesc = NULL;

  MODULE_START

  // Get Dynamic Debug User Configuration Via NVRAM
  pOemDebugLevel_SetupConfig = GetOemDebugLevelDataBaseConfigPointer();
  DEBUG((EFI_D_INFO, "[OemDbg][%a][%d]pOemDebugLevel_SetupConfig:%x\r\n", __FUNCTION__, __LINE__, pOemDebugLevel_SetupConfig));

  // Build Dynamic Debug Database
  // In PEI, use IsPeiPhase flag as TRUE
  pOemDebugLevelDataBasePrivate = BuildOemDebugLevelDataBase(pOemDebugLevel_SetupConfig, TRUE);
  if (pOemDebugLevelDataBasePrivate == NULL)
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)

  DEBUG((DEBUG_INFO, "pOemDebugLevelDataBasePrivate: %p ! \n", pOemDebugLevelDataBasePrivate));
  DEBUG((DEBUG_INFO, "pOemDebugLevelDataBase: %p ! \n", OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(pOemDebugLevelDataBasePrivate)));

  //Mizl-20220318-Check DebugMode Support+>
  Status = CheckDebugModeSetting(PeiServices);
  if (!EFI_ERROR(Status))
    pOemDebugLevelDataBasePrivate->Database.DebugModeSupport=1;
  //Mizl-20220318-Check DebugMode Support->

  // Allocate descriptor and PPI structures as these are dynamically updated
  // Size contains PPI size + Pointer to Database(size of UINTN)
  Size = sizeof(OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI) + sizeof(UINTN);
  gOemDebugLevelServicePPI = (OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *)AllocateZeroPool(Size);
  if (gOemDebugLevelServicePPI == NULL)
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)

  // Update APIs
  gOemDebugLevelServicePPI->GetPrintErrorLevel = gOemDebugGetDebugLevel;
  gOemDebugLevelServicePPI->SetPrintErrorLevel = gOemDebugSetDebugLevel;

  // Update Database pointer
  OEM_DEBUG_LEVEL_DATABASE_PRIVATE_TO_PPI(gOemDebugLevelServicePPI) = (UINTN)pOemDebugLevelDataBasePrivate;
  DEBUG((DEBUG_INFO, "Pointer from Instance: %p ! \n", OEM_DEBUG_LEVEL_DATABASE_PRIVATE_FROM_PPI(gOemDebugLevelServicePPI)));

  // PPI Descriptor
  PpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool(sizeof(EFI_PEI_PPI_DESCRIPTOR));
  if (PpiDesc == NULL)
  {
    FreePool(gOemDebugLevelServicePPI);
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)
  }

  // Update Ppi descriptor
  PpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PpiDesc->Guid = &gOemDebugLevelServicePpiGuid;
  PpiDesc->Ppi = gOemDebugLevelServicePPI;

  // Install the OemDebugLevelSerivce PPI
  Status = (*PeiServices)->InstallPpi(PeiServices, PpiDesc);
  DEBUG((DEBUG_INFO, " (*PeiServices)->InstallPpi Status: %r \n", Status));
  if (EFI_ERROR(Status))
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)

  MODULE_END(Status = EFI_SUCCESS)
}
