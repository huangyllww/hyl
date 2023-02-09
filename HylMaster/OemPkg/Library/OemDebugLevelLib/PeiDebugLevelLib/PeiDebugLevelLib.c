#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/OemDebugLevelSerivce.h>
#include "OemDebugAlways_DebugLevel.h"
#include "Library/OemDebugAlwaysLib.h"
#include "Uefi/UefiBaseType.h"
/**
  The function returns OemDebugLevelSerivce PPI instance

  @retval OemDebugLevelSerivce instance or NULL if service is not available
 */
OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI*
GetOemDebugLevelSerivcePpiInstance (VOID)
{
    EFI_STATUS  Status;
    OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *OemDebugLevelServicePpi = NULL;
    EFI_PEI_PPI_DESCRIPTOR *Dummy;

    Status = PeiServicesLocatePpi(&gOemDebugLevelServicePpiGuid, 0, &Dummy, (VOID**)&OemDebugLevelServicePpi);

    return OemDebugLevelServicePpi;
}

/**
  Returns the debug print error level mask for the current module.

  @return  Debug print error level mask for the current module.
**/
UINT32
EFIAPI
GetDebugPrintErrorLevel (
  VOID
  )
{
    OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI       *OemDebugLevelServicePpi = GetOemDebugLevelSerivcePpiInstance();
    UINT32                                  PrintErrorLevel = MIN_ERROR_LEVEL_VALUE;
    EFI_STATUS                              Status=EFI_NOT_READY;
    if (OemDebugLevelServicePpi) 
        Status = OemDebugLevelServicePpi->GetPrintErrorLevel (OemDebugLevelServicePpi, &gEfiCallerIdGuid, &PrintErrorLevel);
    
    if(!EFI_ERROR (Status) &&(PrintErrorLevel != MIN_ERROR_LEVEL_VALUE))
      return  PrintErrorLevel;

    //Check CMOS Setting
    if((GetDebugFlag() &PEI_DEBUG_ENABLED) ==PEI_DEBUG_ENABLED)
      return EFI_D_ERROR|EFI_D_LOAD|EFI_D_INFO;

    return PcdGet32 (PcdDebugPrintErrorLevel);
}

/**
  Sets/Configure the debug print error level mask for the current module.

  @param   ErrorLevel     Debug Print Error Level mask to be Set/Configured for the current module

  @retval  TRUE           The debug print error level mask was sucessfully set.
  @retval  FALSE          The debug print error level mask could not be set.
**/
BOOLEAN
EFIAPI
SetDebugPrintErrorLevel (
  UINT32  ErrorLevel
  )
{
    OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *OemDebugLevelServicePpi = GetOemDebugLevelSerivcePpiInstance();

    if (OemDebugLevelServicePpi) {
        EFI_STATUS      Status;
        // Set User Error Level for this module
        Status = OemDebugLevelServicePpi->SetPrintErrorLevel (OemDebugLevelServicePpi, &gEfiCallerIdGuid, ErrorLevel);
        if (!EFI_ERROR(Status)) {
            return TRUE;
        }
    } 

    return FALSE;
}

OEM_DEBUG_LEVEL_DATABASE *
GetOemDebugLevelDataBase()
{
  OEM_DEBUG_LEVEL_LEVEL_SERVICE_PPI *OemDebugLevelServicePpi = GetOemDebugLevelSerivcePpiInstance();
  OEM_DEBUG_LEVEL_DATABASE_PRIVATE *pOemDebugLevelDataBasePrivate = NULL;
  OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevelDataBase = NULL;
  pOemDebugLevelDataBasePrivate = OEM_DEBUG_LEVEL_DATABASE_PRIVATE_FROM_PPI(OemDebugLevelServicePpi);
  // Get Pointer to Dynamic Debug Database
  pOemDebugLevelDataBase = OEM_DEBUG_LEVEL_DATABASE_FROM_PRIVATE(pOemDebugLevelDataBasePrivate);

  return pOemDebugLevelDataBase;
}