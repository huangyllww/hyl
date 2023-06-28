
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/OemDebugLevelSerivce.h>
#include "FileGuidNameErrorLevelTableHeader.h"
#include <Library/UefiRuntimeServicesTableLib.h>

#include "OemDebugLevel_DxeModule.c"
#include "OemDebugLevel_PeiModule.c"

#include "OemDebugAlways_DebugLevel.h"
#include "../CommonLib/CommonLib.h"
#include "Library/OemDebugLevelLib.h"
#include <Library/OemDebugAlwaysLib.h>

#include "OemLib.h"

#include "AmiDxeLib.h"

static EFI_GUID gOEMDebugLevelDataBaseGuid = OEM_DEBUG_LEVEL_DATABASE_GUID;
UINT32 gAttributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;

OEM_DEBUG_LEVEL_DATABASE *gOemDebugLevelDataBase = NULL;
OEM_DEBUG_LEVEL_DATABASE_PRIVATE *gOemDebugLevelDatabasePrivate = NULL;

/**
  Set the Dynamic Debug User configuration

  @retval EFI_STATUS Success for successful User configuration setting, else error.
 */
EFI_STATUS
SetOemDebugLevelDataBaseConfig(
    IN OEM_DEBUG_LEVEL_DATABASE *OemDebugLevelDataBase)
{

  DEBUG((DEBUG_INFO, "%a ..\n", __FUNCTION__));

  // Set DynamicDebug Nvram Variable
  return gRT->SetVariable(
      OEM_DEBUG_LEVEL_DATA_NAME,
      &gOEMDebugLevelDataBaseGuid,
      gAttributes,
      OemDebugLevelDataBase->Length,
      OemDebugLevelDataBase);
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

  // Get DynamicDebug Nvram Variable
  Size = 0;
  Status = gRT->GetVariable(
      OEM_DEBUG_LEVEL_DATA_NAME,
      &gOEMDebugLevelDataBaseGuid,
      NULL,
      &Size,
      NULL);
  DEBUG((DEBUG_INFO, " gRT->GetVariable status %r Size: %x \n", Status, Size));

  if (Status == EFI_BUFFER_TOO_SMALL)
  {
    // Allocate memory for Dynamic Debug Nvram Data
    OemDebugLevelConfigData = (OEM_DEBUG_LEVEL_DATABASE *)AllocateZeroPool(Size);
    if (OemDebugLevelConfigData == NULL)
    {
      DEBUG((DEBUG_ERROR, "Failed to allocate memory for OemDebugLevelConfigData! \n"));
      return NULL;
    }

    // Get DynamicDebug Nvram Variable
    Status = gRT->GetVariable(
        OEM_DEBUG_LEVEL_DATA_NAME,
        &gOEMDebugLevelDataBaseGuid,
        &gAttributes,
        &Size,
        OemDebugLevelConfigData);
    DEBUG((DEBUG_INFO, " gRT->GetVariable status %r Size: %x \n", Status, Size));
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
  // DXE Table Size
  return (UINT32)(NumberofElementsInBuildToolGeneratedDxeModuleTable * sizeof(FILE_GUID_PRINT_ERROR_LEVEL_TABLE));
}

/**
  Create Default File Guid Print Error Level DXE Table from Build generated File Guid Name Table

  @param  FileGuidPrintErrorLevelTable    Pointer to File Guid Print Error Level Table to be updated
**/
VOID CreateDefaultFileGuidPrintErrorLevelDxeTable(
    IN FILE_GUID_PRINT_ERROR_LEVEL_TABLE *FileGuidPrintErrorLevelTable)
{
  // Create DXE Table
  CreateDefaultFileGuidPrintErrorLevelTable(
      (FILE_GUID_NAME_TABLE *)BuildToolGeneratedDxeModuleTable,
      (UINT32)NumberofElementsInBuildToolGeneratedDxeModuleTable,
      FileGuidPrintErrorLevelTable);
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
    This is notification callback for multiple events to sync the User config with current settings

  @param  Event    Event whose notification function is being invoked.
  @param  Context  pointer to the notification function's context.
**/
VOID SyncOemDebugLevelSetting(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
  DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

  //Sync Setup Setting
  SetOemDebugLevelDataBaseConfig(gOemDebugLevelDataBase);

  DEBUG((DEBUG_INFO, "%a Exiting..\n", __FUNCTION__));

  return;
}

/**
  Set/Configure Debug Print Error Level for a module in Database

  @param  This          Pointer to Ami Debug Print Error Level Service Protocol
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Debug Print Error Level to be Set/Configured

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
DynamicDebugSetPrintErrorLevelToDatabase(
    IN OEM_DEBUG_LEVEL_SERVICE_PROTOCOL *This,
    IN EFI_GUID *ModuleGuid,
    IN OUT UINT32 ErrorLevel)
{

  // Set Debug Print Error Level to Database
  return SetDebugPrintErrorLevelToDatabase(
      OEM_DEBUG_LEVEL_DXE_TABLE(gOemDebugLevelDataBase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
      OEM_DEBUG_LEVEL_DXE_TABLE_ENTRIES(gOemDebugLevelDataBase),
      ModuleGuid, ErrorLevel);
}
/**
  Set/Configure Debug Print Error Level for a module

  @param  This          Pointer to Ami Debug Print Error Level Service Protocol
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Debug Print Error Level to be Set/Configured

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
DynamicDebugSetPrintErrorLevel(
    IN OEM_DEBUG_LEVEL_SERVICE_PROTOCOL *This,
    IN EFI_GUID *ModuleGuid,
    IN OUT UINT32 ErrorLevel)
{

  // Validate Input parameters
  if ((This == NULL) || (ModuleGuid == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  // Set Debug Print Error Level to Database
  return DynamicDebugSetPrintErrorLevelToDatabase(This, ModuleGuid, ErrorLevel);
}

/**
  Get configured Debug Print Error Level for a module from Database
  Checks user configuration:
      a) If Global error level is enabled, return Global Error Level
      b) Else if Phase error level is enabled, return Phase Error Level
      c) Else return Error level for module

  @param  This          Pointer to Ami Debug Print Error Level Service Protocol
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Pointer to return current Debug Print Error Level

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
DynamicDebugGetPrintErrorLevelFromDatabase(
    IN OEM_DEBUG_LEVEL_SERVICE_PROTOCOL *This,
    IN EFI_GUID *ModuleGuid,
    IN OUT UINT32 *ErrorLevel)
{
  EFI_STATUS Status;
  UINT32 ModuleErrorLevel = *ErrorLevel;

  *ErrorLevel = MIN_ERROR_LEVEL_VALUE;//SGEZT#29630:Improve Debug Level Module Logic+>
  if(!gOemDebugLevelDataBase->DebugModeSupport)
  {
    return EFI_SUCCESS;
  }
  
  // Check Global Level support first
  if (gOemDebugLevelDataBase->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE)
  {
    // Update Global Level value
    *ErrorLevel = gOemDebugLevelDataBase->GlobalPrintErrorLevel;
    return EFI_SUCCESS;
  }
  if (gOemDebugLevelDataBase->DxePrintSupport)
  {
    // Update Phase Level value
    if (gOemDebugLevelDataBase->DxePrintErrorLevel != MIN_ERROR_LEVEL_VALUE)
      *ErrorLevel = gOemDebugLevelDataBase->DxePrintErrorLevel;
    else//SGEZT#29630:Improve Debug Level Module Logic+>
    {
      *ErrorLevel = TransferModuleErrorLevel(gOemDebugLevelDataBase, Dxe);
      if(*ErrorLevel)
        *ErrorLevel=*ErrorLevel|BIT2;
    }//SGEZT#29630:Improve Debug Level Module Logic->
    return EFI_SUCCESS;
  }

  // Check DXE Database for Module level
  Status = GetDebugPrintErrorLevelFromDatabase(
      OEM_DEBUG_LEVEL_DXE_TABLE(gOemDebugLevelDataBase, FILE_GUID_PRINT_ERROR_LEVEL_TABLE),
      OEM_DEBUG_LEVEL_DXE_TABLE_ENTRIES(gOemDebugLevelDataBase),
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

  @param  This          Pointer to Ami Debug Print Error Level Service Protocol
  @param  ModuleGuid    Pointer to Module File Guid
  @param  ErrorLevel    Pointer to return current Debug Print Error Level

  @retval EFI_STATUS    Returns Success if module is found in Database, else Not Found
**/
EFI_STATUS
DynamicDebugGetPrintErrorLevel(
    IN OEM_DEBUG_LEVEL_SERVICE_PROTOCOL *This,
    IN EFI_GUID *ModuleGuid,
    IN OUT UINT32 *ErrorLevel)
{
  // Validate Input parameters
  if ((This == NULL) || (ModuleGuid == NULL) || (ErrorLevel == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  // Get Debug Print Error Level from Database
  return DynamicDebugGetPrintErrorLevelFromDatabase(This, ModuleGuid, ErrorLevel);
}

/**
GC_TODO:  CheckDebugModeSetting

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
CheckDebugModeSetting(
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  OEM_COMMON_SETUP_DATA OEMSetupData;
  UINTN VariableSize;
  EFI_STATUS Status;
  EFI_GUID OemSetupVariableGuid = OEM_COMMON_VAR_GUID;
  UINT32 VariableAttribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;

  MODULE_START

  VariableSize = sizeof(OEM_COMMON_SETUP_DATA);
  Status = pRS->GetVariable(
                    L"OEM_VAR",
                    &OemSetupVariableGuid,
                    &VariableAttribute,
                    &VariableSize,
                    &OEMSetupData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)
  
  if (!OEMSetupData.DebugModeMask)
    MODULE_END(Status = EFI_UNSUPPORTED)

  MODULE_END(Status = EFI_SUCCESS)
}

/**

    This is entry point of the DXE Driver that 
    1) Consumes Dynamic Debug Database HOB generated in PEI phase for PEI modules 
       and builds default database for DXE Phase from DXE auto generated files
    2) Retrieves User configuration settings and updates database
    3) Publishes AmiDebugPrintErrorLevel Protocol Service

    @param ImageHandle      Handle for the image of this driver
    @param SystemTable      Pointer to the EFI System Table

    @retval EFI_SUCCESS     Protocol successfully started and installed
    @retval Error           For non-successful case
**/
EFI_STATUS
BuildOemDebugLevelDataBaseDxeEntry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS Status;
  EFI_HANDLE NewHandle = NULL;
  OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevel_SetupConfig = NULL;
  VOID *DynamicDebugSyncEventRegId = NULL;
  EFI_EVENT DynamicDebugSyncEvent = NULL;
  EFI_GUID gAmiBdsConnectDriversProtocolGuid = {0x3aa83745, 0x9454, 0x4f7a, {0xa7, 0xc0, 0x90, 0xdb, 0xd0, 0x2f, 0xab, 0x8e}}; //yaosr debug
  OEM_DEBUG_LEVEL_OBJECT  *pDxeDedebugLevelObject=NULL;

  MODULE_START	
  
  InitAmiLib(ImageHandle, SystemTable);

  Status = gBS->AllocatePool (EfiBootServicesData,sizeof (OEM_DEBUG_LEVEL_OBJECT),&pDxeDedebugLevelObject);
  if(EFI_ERROR (Status))
    MODULE_END(Status)
  // Get Dynamic Debug User Configuration
  pOemDebugLevel_SetupConfig = GetOemDebugLevelDataBaseConfigPointer();

  // Build Dynamic Debug Database
  // In DXE, use IsPeiPhase flag as FALSE
  gOemDebugLevelDatabasePrivate = BuildOemDebugLevelDataBase(pOemDebugLevel_SetupConfig, FALSE);

  if (gOemDebugLevelDatabasePrivate == NULL)
    MODULE_END(Status = EFI_OUT_OF_RESOURCES)

  gOemDebugLevelDataBase = OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(gOemDebugLevelDatabasePrivate);

  pDxeDedebugLevelObject->Signature = DXE_DEBUG_LEVEL_SIGNATURE;
  pDxeDedebugLevelObject->DxePhaseDebugLevelDataBase=gOemDebugLevelDataBase;
  //Mizl-20220318-Check DebugMode Support+>
  Status = CheckDebugModeSetting(SystemTable);
  if (!EFI_ERROR(Status))
    gOemDebugLevelDataBase->DebugModeSupport=1;
  
  //SyncCMOSSetting();
  SyncDebugModeWithCMOS(gOemDebugLevelDataBase);
  //Mizl-20220318-Check DebugMode Support->
  // Update APIs
  pDxeDedebugLevelObject->DxeDedebugLevelProtocol.GetPrintErrorLevel = DynamicDebugGetPrintErrorLevel;
  pDxeDedebugLevelObject->DxeDedebugLevelProtocol.SetPrintErrorLevel = DynamicDebugSetPrintErrorLevel;

  // Install OemDebugLevelSerivce protocol
  Status = gBS->InstallProtocolInterface(
      &NewHandle,
      &gOemDebugLevelSerivceProtocolGuid,
      EFI_NATIVE_INTERFACE,
      &pDxeDedebugLevelObject->DxeDedebugLevelProtocol);
  DEBUG((DEBUG_INFO, "InstallProtocolInterface... Status: %r \n", Status));
  if (EFI_ERROR(Status))
    MODULE_END(Status)


  // Register Protocol notification for BdsConnectDrivers Protocol
  // First time it gets synced on registration and
  // then 2nd time after protocol is available
  EfiCreateProtocolNotifyEvent(
      &gAmiBdsConnectDriversProtocolGuid,
      TPL_CALLBACK,
      SyncOemDebugLevelSetting,
      NULL,
      &DynamicDebugSyncEventRegId);

  // Register Event for Setup enter GUID
  EfiNamedEventListen(
      &gAmiTseSetupEnterGuid,
      TPL_CALLBACK,
      SyncOemDebugLevelSetting,
      NULL,
      NULL);

  // Register event for Readytoboot
  EfiCreateEventReadyToBootEx(
      TPL_NOTIFY,
      SyncOemDebugLevelSetting,
      NULL,
      &DynamicDebugSyncEvent);

  MODULE_END(Status = EFI_SUCCESS)
}
