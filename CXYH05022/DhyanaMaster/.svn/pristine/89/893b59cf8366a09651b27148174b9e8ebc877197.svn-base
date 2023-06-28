#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/OemDebugAlwaysLib.h>
#include "OemDebugAlways_DebugLevel.h"
#include "OemLib.h"
#include "Library/OemDebugAlwaysLib.h"
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
  UINT32  PrintErrorLevel=MIN_ERROR_LEVEL_VALUE;
  
  //Check CMOS Setting
  if((GetDebugFlag() &SEC_DEBUG_ENABLED) ==SEC_DEBUG_ENABLED)
    return MAX_ERROR_LEVEL_VALUE;

  return PrintErrorLevel;
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
    return FALSE;
}

EFI_STATUS
SyncDebugModeWithCMOS(
    IN OEM_DEBUG_LEVEL_DATABASE *ODDB)
{
    UINT8 DebugModeMask = 0;
    EFI_STATUS  Status=EFI_SUCCESS;

    MODULE_START
    
    DumpDebugSetting(ODDB);
    
    if (ODDB->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE)
        DebugModeMask |= PEI_DEBUG_ENABLED|RTS_DEBUG_ENABLED|DXE_DEBUG_ENABLED;

    if (ODDB->DebugModeSupport & ODDB->PeiPrintSupport)
        DebugModeMask |= PEI_DEBUG_ENABLED;
        
    if (ODDB->DebugModeSupport & ODDB->RTSPrintSupport)
        DebugModeMask |= RTS_DEBUG_ENABLED;
    
    SetDebugFlag(DebugModeMask, DEBUG_MODE_ENABLED_MASK);

    MODULE_END(Status)
}