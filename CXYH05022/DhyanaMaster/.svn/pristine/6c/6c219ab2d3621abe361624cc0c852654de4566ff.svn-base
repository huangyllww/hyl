/** @file OemPort80.c
    Function file for Tse hook functionalities.

**/

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include "Token.h"
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/AMIPostMgr.h>
#include "OemConOut.h"

#include "Library/OemDebugLevelLib.h" ////SGEZT#29630:Improve Debug Level Module Logic+>
//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern    EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl;
#if CON_OUT_CHECKPOINTS_IN_QUIET_MODE
extern    AMI_POST_MANAGER_PROTOCOL *PostManager;
extern    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground ;
extern    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background ;
EFI_GUID gAmiPostManagerProtocolGuidLocal = AMI_POST_MANAGER_PROTOCOL_GUID;
#endif
//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------
//**********************************************************************
// Status Code Library (functions used throughout the module)
//**********************************************************************
VOID OemConOutCheckpoint (
    IN VOID *Dummy,
    IN UINT16 Checkpoint
)
{
    EFI_STATUS Status;
    EFI_CONSOLE_CONTROL_SCREEN_MODE Mode;
    CHAR16 strmsg[10];
    UINT8  DataValue;
    
    if (ConsoleControl==NULL || pST->ConOut==NULL
#if CON_OUT_CHECKPOINTS_IN_QUIET_MODE
            || PostManager == NULL
#endif
    ) {
        return;
    }
    DataValue = PcdGet8(PcdPostCodeDebug);
    if(DataValue == 1) {
        Swprintf(strmsg, L"%02X(%02X)",Checkpoint&0xFF,Checkpoint>>8);
    }else{
        Swprintf(strmsg, L"%02X",Checkpoint&0xFF);
    }
      DEBUG ((DEBUG_INFO, "[OemDbg]CheckPoint = %s\n", strmsg));
    
    Status = ConsoleControl->GetMode(ConsoleControl,&Mode,NULL,NULL);
    if (EFI_ERROR(Status)) {
      return;
    }

    if (Mode==EfiConsoleControlScreenText)
    {
        UINTN X,Y, MaxX, MaxY;
        pST->ConOut->QueryMode(pST->ConOut,pST->ConOut->Mode->Mode, &MaxX, &MaxY);
        //Get cursor position
        X = pST->ConOut->Mode->CursorColumn;
        Y = pST->ConOut->Mode->CursorRow;
        //Set cursor and print string
        pST->ConOut->SetCursorPosition(pST->ConOut, MaxX-8, MaxY-1);
        pST->ConOut->OutputString(pST->ConOut, strmsg);
        //Restore cursor position
        pST->ConOut->SetCursorPosition(pST->ConOut, X, Y);
    }
    
#if CON_OUT_CHECKPOINTS_IN_QUIET_MODE
    else  // EfiConsoleControlScreenGraphics
    {
        PostManager->DisplayQuietBootMessage(
            strmsg, 0, 0, CA_AttributeRightBottom,
            Foreground, Background
        );
    }
#endif
    CheckPointDebug(Checkpoint); //SGEZT#29630:Improve Debug Level Module Logic+>

    return;
}

