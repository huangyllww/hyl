
#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/OemDebugLevelSerivce.h>
#include "FileGuidNameErrorLevelTableHeader.h"
#include <Library/UefiRuntimeServicesTableLib.h>

#include "OemDebugAlways_DebugLevel.h"
#include "Library/OemDebugAlwaysLib.h"

#define  EFI_TPL_CALLBACK 8



static EFI_GUID gOEMDebugLevelDataBaseGuid = OEM_DEBUG_LEVEL_DATABASE_GUID;
UINT32 gAttributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS;

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

  DEBUG((DEBUG_INFO, "%a Entered..\n", __FUNCTION__));

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
//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   SyncIpmiSetupConfig
//
// Description: Check for the Ipmi OEM Setup Configuration and Set 
// certain variable bmc specified
//
// Input: None
//
// Output:  None
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
SyncDebugLevelSetingWithCMOS (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status=EFI_SUCCESS;
  OEM_DEBUG_LEVEL_DATABASE              *OemDebugLevelDataBase=GetOemDebugLevelDataBaseConfigPointer();
  UINT8                                 DebugModeMask=0;

  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));

  if(OemDebugLevelDataBase->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE) DebugModeMask=PEI_DEBUG_ENABLED|DXE_DEBUG_ENABLED;
  if(OemDebugLevelDataBase->DebugModeSupport & OemDebugLevelDataBase->PeiPrintSupport)  DebugModeMask|=PEI_DEBUG_ENABLED;
  if(OemDebugLevelDataBase->DebugModeSupport & OemDebugLevelDataBase->DxePrintSupport)  DebugModeMask|=DXE_DEBUG_ENABLED;
  if(OemDebugLevelDataBase->DebugModeSupport & OemDebugLevelDataBase->RTSPrintSupport)  DebugModeMask|=RTS_DEBUG_ENABLED;

  SetDebugFlag(DebugModeMask, DEBUG_MODE_ENABLED_MASK);

  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));

  return Status;
}


EFI_STATUS
SyncCmosSettingEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable )
{
  EFI_STATUS        Status=EFI_SUCCESS;

  DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]\r\n",__FUNCTION__,__LINE__));
  Status =  EfiNamedEventListen (
              &gOemNvramUpdateGuid,
              EFI_TPL_CALLBACK,
              SyncDebugLevelSetingWithCMOS,
              NULL,
              NULL
              );
    return Status;
}
