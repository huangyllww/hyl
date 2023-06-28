#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Protocol/OemDebugLevelSerivce.h>
#include "Library/OemDebugAlwaysLib.h"
#include "OemDebugAlways_DebugLevel.h"
#include "OemLib.h"

OEM_DEBUG_LEVEL_SERVICE_PROTOCOL *gOemDebugLevelServiceProtocol = NULL;

static EFI_SYSTEM_TABLE *gThisSystemTable = NULL;

EFI_EVENT gProtocolNotificationEvent = NULL;

BOOLEAN fCurrentDebugMode = FALSE; //SGEZT#29630:Improve Debug Level Module Logic+>
OEM_DEBUG_LEVEL_DATABASE *pDebugLevelDataBase = NULL; //SGEZT#29630:Improve Debug Level Module Logic+>

/**
  The function locates OemDebugLevelSerivce Protocol instance in global variable
 */
VOID
GetOemDebugLevelSerivceProtocolInstance (VOID)
{
    EFI_STATUS  Status;

    if (gOemDebugLevelServiceProtocol == NULL) {
        Status = gThisSystemTable->BootServices->LocateProtocol(
                    &gOemDebugLevelSerivceProtocolGuid, 
                    NULL, 
                    (VOID**)&gOemDebugLevelServiceProtocol);
    }
}

OEM_DEBUG_LEVEL_DATABASE *
GetOemDebugLevelDataBase()
{
  OEM_DEBUG_LEVEL_OBJECT  *OemDebugLevelObject=NULL;
  OEM_DEBUG_LEVEL_DATABASE *pOemDebugLevelDataBase = NULL;

  GetOemDebugLevelSerivceProtocolInstance ();
  if (gOemDebugLevelServiceProtocol == NULL)
  return NULL;
  
  OemDebugLevelObject = DXE_DEBUG_LEVEL_FROM_PROTOCOL_THIS(gOemDebugLevelServiceProtocol);

  return OemDebugLevelObject->DxePhaseDebugLevelDataBase;
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
  
  // Retrieve the current debug print error level mask for the module from PcdDebugPrintErrorLevel.
  if( PcdGet32 (PcdDebugPrintErrorLevel) != MIN_ERROR_LEVEL_VALUE)
    return PcdGet32 (PcdDebugPrintErrorLevel);

  //Check CMOS Setting
//SGEZT#29630:Improve Debug Level Module Logic+>
  if((GetDebugFlagEx() & DXE_DEBUG_ENABLED) == DXE_DEBUG_ENABLED)
  {
    return DEFAULT_CMOS_DEBUG_LEVEL_SUPPORT; //SGEZT#29630:Improve Debug Level Module Logic  
  }
//SGEZT#29630:Improve Debug Level Module Logic->

  return MIN_ERROR_LEVEL_VALUE;
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

    // Set/Configure the current debug print error level mask for the module in PcdDebugPrintErrorLevel
    PcdSet32 (PcdDebugPrintErrorLevel, ErrorLevel);

    // Get Ami debug Print Error Level protocol instance
    GetOemDebugLevelSerivceProtocolInstance ();

    // Set/Configure the current debug print error level mask for the module in User configuration
    if (gOemDebugLevelServiceProtocol == NULL) {
        EFI_STATUS      Status;
        // Set User Error Level for this module
        Status = gOemDebugLevelServiceProtocol->SetPrintErrorLevel (gOemDebugLevelServiceProtocol, &gEfiCallerIdGuid, ErrorLevel);
        if (!EFI_ERROR(Status)) {
            return TRUE;
        }
    } 

    return FALSE;
}

/**
  The function retrieves the current debug print error level mask for the module 
  from User configuration and sets the mask in PcdDebugPrintErrorLevel
 */

VOID
EFIAPI
SetModuleDebugPrintErrorLevel (
  VOID
  )
{
    EFI_STATUS                              Status;
    UINT32                                  PrintErrorLevel;


    // Get the current debug print error level mask for the module from PcdDebugPrintErrorLevel
    PrintErrorLevel = PcdGet32 (PcdDebugPrintErrorLevel);

    // Get the current debug print error level mask for the module from User configuration
    Status = gOemDebugLevelServiceProtocol->GetPrintErrorLevel (gOemDebugLevelServiceProtocol, &gEfiCallerIdGuid, &PrintErrorLevel);
    if (!EFI_ERROR(Status)) {
        // Set/Configure the user debug print error level mask for the module in PcdDebugPrintErrorLevel
        PrintErrorLevel = PcdSet32 (PcdDebugPrintErrorLevel, PrintErrorLevel);
    }

    return;
}

/**
  Notification callback for OemDebugLevelSerivce Protocol

  @param   Event        The Event this notify function registered to.
  @param   Context      Context passed in while registering notification

**/
VOID
EFIAPI
OemDebugLevelSerivceProtocolNotification (
  IN EFI_EVENT        Event,
  IN VOID             *Context )
{

    // Get Ami debug Print Error Level protocol instance
    GetOemDebugLevelSerivceProtocolInstance ();
    if (gOemDebugLevelServiceProtocol == NULL) {
        return;
    }

    // Close Event
    if (Event) {
        gThisSystemTable->BootServices->CloseEvent (Event);
    }

    // Change Debug Print Error Level for Module
    SetModuleDebugPrintErrorLevel ();

    return;
}

/**
    The constructor function locates OemDebugLevelSerivce protocol.
    If the protocol is available, set debug print error level PCD based on user configuration.
    Else, register notification for the protocol

    @param ImageHandle      The firmware allocated handle for the EFI image.
    @param SystemTable      A pointer to the EFI System Table.

    @return EFI_STATUS
    @retval EFI_SUCCESS     The constructor always returns EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
OemDebugLevelLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable )
{

    EFI_STATUS Status;

    // Update global variable
    gThisSystemTable = SystemTable;

    // Locate the AmiDebugPrintErrorLevel Service protocol.
    GetOemDebugLevelSerivceProtocolInstance ();
    // If the Protocol is not available, register protocol notification
    if (gOemDebugLevelServiceProtocol == NULL) {

        VOID        *ProtocolNotifyId = NULL;

        // Create the event
        Status = SystemTable->BootServices->CreateEvent (
                        EVT_NOTIFY_SIGNAL,
                        TPL_CALLBACK,
                        OemDebugLevelSerivceProtocolNotification,
                        NULL,
                        &gProtocolNotificationEvent
                        );
        ASSERT_EFI_ERROR (Status);

        // Register for protocol notifications on this event
        Status = SystemTable->BootServices->RegisterProtocolNotify (
                        &gOemDebugLevelSerivceProtocolGuid,
                        gProtocolNotificationEvent,
                        &ProtocolNotifyId
                        );
        ASSERT_EFI_ERROR (Status);
    } else {
        // Change Debug Print Error Level for Module if the protocol is available
        SetModuleDebugPrintErrorLevel ();
    }

    return EFI_SUCCESS;
}

/**
  The destructor function closes the event created by constructor if any.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The destructor completed successfully.
  @retval Other value   The destructor did not complete successfully.

**/
EFI_STATUS
EFIAPI
OemDebugLevelLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable )
{
    EFI_STATUS      Status = EFI_SUCCESS;

    // Close Event if created in constructor
    if (gProtocolNotificationEvent) {
        Status = SystemTable->BootServices->CloseEvent (gProtocolNotificationEvent);
    }

    return Status;
}

//SGEZT#29630:Improve Debug Level Module Logic+>
EFI_STATUS
SyncDebugModeWithCMOS(
    IN OEM_DEBUG_LEVEL_DATABASE *ODDB)
{
  UINT8 DebugModeMask = 0;
  EFI_STATUS Status = EFI_SUCCESS;

  MODULE_START

  DumpDebugSetting(ODDB);

  if (ODDB->DebugModeSupport & (ODDB->GlobalPrintErrorLevel != MIN_ERROR_LEVEL_VALUE))
    DebugModeMask |= PEI_PHASE_FLAG | DXE_PHASE_FLAG | PEI_DEBUG_ENABLED | RTS_DEBUG_ENABLED | DXE_DEBUG_ENABLED;

  if (ODDB->DebugModeSupport & ODDB->PeiPrintSupport)
    DebugModeMask |= PEI_PHASE_FLAG | PEI_DEBUG_ENABLED;

  if (ODDB->DebugModeSupport & ODDB->DxePrintSupport)
    DebugModeMask |= DXE_PHASE_FLAG | DXE_DEBUG_ENABLED;

  if (ODDB->DebugModeSupport & ODDB->RTSPrintSupport)
    DebugModeMask |= PEI_PHASE_FLAG | DXE_PHASE_FLAG | RTS_DEBUG_ENABLED;

  SetDebugFlag(DebugModeMask, DEBUG_MODE_ENABLED_MASK);

  MODULE_END(Status)
}


VOID CheckPointDebug(IN UINT16 Checkpoint)
{
  UINT16 DebugCheckPointData = 0xFFFF;
  UINT8 CurrentDebugMode = GetDebugFlagEx();
  UINT8 DXE_Debug = DXE_PHASE_FLAG |DXE_DEBUG_ENABLED|RTS_DEBUG_ENABLED;
  BOOLEAN fSyncCMOSSetting = FALSE;

  DEBUG ((-1, "%a %d GetDebugFlagEx:%x\n", __FUNCTION__, __LINE__,CurrentDebugMode));

  if (pDebugLevelDataBase == NULL)
    pDebugLevelDataBase = GetOemDebugLevelDataBase();

  if (pDebugLevelDataBase == NULL) return;
  
  if (pDebugLevelDataBase->CheckPointDebugSupport == 0 || (pDebugLevelDataBase->CheckPointData== 0xFFFF))
    return;

  DebugCheckPointData = pDebugLevelDataBase->CheckPointData;

  if ((Checkpoint != DebugCheckPointData) && (fCurrentDebugMode == TRUE))
  {
    fCurrentDebugMode = FALSE; //Disable Debug Mode Later
    CurrentDebugMode = CurrentDebugMode & (~DXE_Debug);
    fSyncCMOSSetting = TRUE;
  }

  if ((Checkpoint == DebugCheckPointData) && (fCurrentDebugMode == FALSE))
  {
    fCurrentDebugMode = TRUE; //Enable Debug Mode Later
    CurrentDebugMode = CurrentDebugMode | DXE_Debug;
    fSyncCMOSSetting = TRUE;
  }

  if (fSyncCMOSSetting == TRUE)
    SetDebugFlag(CurrentDebugMode, DEBUG_MODE_ENABLED_MASK);

  return;
}
//SGEZT#29630:Improve Debug Level Module Logic->