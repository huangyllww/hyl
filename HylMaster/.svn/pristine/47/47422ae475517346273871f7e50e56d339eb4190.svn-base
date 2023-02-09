#include <AMIDxeLib.h>

#include "Setup.h"

#include <Protocol/AmiPostMgr.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/SimpleTextOut.h>
//Lizhq 20140815 First boot & boot w/ default configuration message prompt [+]{
#include <Library/HobLib.h>
#include <library/PcdLib.h>
//<lvych001-20161129 Add press hot key tips +>
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
#include "Protocol/AmiKeycode.h"
#else
#ifndef _AMI_EFI_KEY_DATA_
#define _AMI_EFI_KEY_DATA_
typedef struct {
  EFI_INPUT_KEY  Key;
} AMI_EFI_KEY_DATA;
#endif
#endif
#include "bootflow.h"
//<lvych001-20161129 Add press hot key tips ->

#define BOOT_WITH_FULL_CONFIGURATION                  0x00
#define BOOT_WITH_MINIMAL_CONFIGURATION               0x01
#define BOOT_ASSUMING_NO_CONFIGURATION_CHANGES        0x02
#define BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS 0x03
#define BOOT_WITH_DEFAULT_SETTINGS                    0x04
#define BOOT_ON_S4_RESUME                             0x05
#define BOOT_ON_S5_RESUME                             0x06
#define BOOT_WITH_MFG_MODE_SETTINGS                   0x07
#define BOOT_ON_S2_RESUME                             0x10
#define BOOT_ON_S3_RESUME                             0x11
#define BOOT_ON_FLASH_UPDATE                          0x12
#define BOOT_IN_RECOVERY_MODE                         0x20
//<lvych001-20161129 Add press hot key tips +>
#define SETUP_ENTRY_KEY_BIT                           0x00000001
#define POPUP_ENTRY_KEY_BIT                           0x00000002
#define NETWORK_ENTRY_KEY_BIT                         0x00000004
#define CALLBACK_ONCE_KEY_BIT                         0x00000080
#define TSE_CHECK_SHIFTSTATE  CheckAdvShiftState
UINT8 gPressHotKeys = 0;
//<lvych001-20161129 Add press hot key tips ->
//Lizhq 20140815 First boot & boot w/ default configuration message prompt [+]}
EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground = POST_FONT_FG; //<bieyb001-20170518  change font color for baidu>
EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background = POST_FONT_BG; //<bieyb001-20170518  change font color for baidu>
#define NULL_LINE L"                                                                                "//lenth=80
//<lvych002-20171116 Support sol display hot key messages+>
#ifndef GLYPH_HEIGHT
#define GLYPH_HEIGHT        17
#endif

#ifndef GLYPH_WIDTH
#define GLYPH_WIDTH         8
#endif

#ifndef FV_X
#define FV_X 1024
#define FV_Y 768
#endif

#define HOTKEY_LEFT         0
#define HOTKEY_TOP          2
#define HOTKEY_MAX_ROWS     2
#define HOTKEY_MAX_COLS     80
#define HOTKEYPX(x)         (HOTKEY_LEFT + x)
#define HOTKEYPY(y)         (HOTKEY_TOP - 1 - y)

extern EFI_BOOT_SERVICES *gBS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_RUNTIME_SERVICES *gRT;
//<lvych001-20161129 Add press hot key tips +>
extern BOOLEAN gQuietBoot;
extern BOOLEAN gEnterSetup;
extern UINT32  gBootFlow;
//<lvych002-20171116 Support sol display hot key messages+>
extern VOID*   gHiiHandle;
CHAR16 *HotKeyMsgText = NULL;
CHAR16 *gHotKeyMsgScreenBuffer = NULL;
UINTN gHotKeyStartRows = 0;
UINTN gHotKeyStartCols = 0;

EFI_STATUS GetScreenResolution(UINTN *ResX, UINTN *ResY);
CHAR16 *HiiGetString( VOID* handle, UINT16 token );
VOID   MemFreePointer( VOID **ptr );
VOID *
EfiLibAllocatePool (
  IN  UINTN   AllocationSize
  );
VOID PrintHotKeyTips(UINT8 HotKeys);
//<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.+>
VOID UpdateGoPUgaDraw( VOID );
EFI_STATUS HiiInitializeProtocol( VOID );
//<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.->
//<lvych002-20171116 Support sol display hot key messages->

EFI_STATUS
PostManagerDisplayQuietBootMessage(
    CHAR16 *Message,
    INTN CoOrdX,
    INTN CoOrdY,
    CO_ORD_ATTRIBUTE Attribute,
    EFI_UGA_PIXEL Foreground,
    EFI_UGA_PIXEL Background
);
EFI_STATUS PostManagerDisplayPostMessage( CHAR16 *message );
BOOLEAN   IsWaitForKeyEventSupport (VOID);
void CheckandDeactivateSoftkbd(); 
BOOLEAN CheckAdvShiftState (UINT32 AmiKeyShiftState, UINT32 CondShiftState);
void PrintEnterSetupMessage();
//<lvych001-20161129 Add press hot key tips ->
//<lvych001-20170502 Improve code+>
EFI_STATUS SetGraphicsResolution (UINT32 HorResolution, UINT32 VerResolution);

VOID OemSetNativeResolution(VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
#if defined(FIX_POST_GRAPHICS_RESOLUTION_SUPPORT)&&(FIX_POST_GRAPHICS_RESOLUTION_SUPPORT == 1) //<lvych004-20161201 Mantis:0011796 +>
    Status = SetGraphicsResolution (FV_X, FV_Y);         //If EDID not found set 1024*768 mode
#endif//<lvych004-20161201 Mantis:0011796 ->
}
//<lvych001-20170502 Improve code->
//<lvych00120160602+>
/**
  Check whether the unicode represents a line break or not.

  This is a internal function. Please see Section 27.2.6 of the UEFI Specification
  for a description of the supported string format.

  @param  Char                    Unicode character

  @retval 0                       Yes, it forces a line break.
  @retval 1                       Yes, it presents a line break opportunity
  @retval 2                       Yes, it requires a line break happen before and after it.
  @retval -1                      No, it is not a link break.

**/
INT8
IsLineBreak (
  IN  CHAR16    Char
  )
{
  switch (Char) {
    //
    // Mandatory line break characters, which force a line-break
    //
    case 0x000A:
    case 0x000C:
    case 0x000D:
    case 0x2028:
    case 0x2029:
      return 0;
    //
    // Space characters, which is taken as a line-break opportunity
    //
    case 0x0020:
    case 0x1680:
    case 0x2000:
    case 0x2001:
    case 0x2002:
    case 0x2003:
    case 0x2004:
    case 0x2005:
    case 0x2006:
    case 0x2008:
    case 0x2009:
    case 0x200A:
    case 0x205F:
    //
    // In-Word Break Opportunities
    //
    case 0x200B:
      return 1;
    //
    // A space which is not a line-break opportunity
    //
    case 0x00A0:
    case 0x202F:
    //
    // A hyphen which is not a line-break opportunity
    //
    case 0x2011:
      return -1;
    //
    // Hyphen characters which describe line break opportunities after the character
    //
    case 0x058A:
    case 0x2010:
    case 0x2012:
    case 0x2013:
    case 0x0F0B:
    case 0x1361:
    case 0x17D5:
      return 1;
    //
    // A hyphen which describes line break opportunities before and after them, but not between a pair of them
    //
    case 0x2014:
      return 2;
  }
  return -1;
}

VOID ClearHotKeyMsgScreen()
{
	UINTN i;

	for(i=0;i<HOTKEY_MAX_ROWS;i++)
		PostManagerDisplayQuietBootMessage(NULL_LINE, GLYPH_WIDTH * HOTKEYPX(0), HOTKEYPY(i)* GLYPH_HEIGHT, CA_AttributeLeftBottom, Foreground, Background);
}

VOID OemDisplayHotKeyPostMessage(CHAR16 *String,UINTN PX,UINTN PY)
{
	UINTN  hrow,hcol,xi,yi;
	CHAR16 *LineStart, *LineEnd;
	CHAR16 save;

	if(String == NULL)
	  return;

	hcol = PX;
	hrow = PY;
	xi = GLYPH_WIDTH * HOTKEYPX(hcol);
	//Find string
	for(LineStart = String; *LineStart!=0; )
	{
		if(hrow >= HOTKEY_MAX_ROWS)
			hrow = 0;
		yi = HOTKEYPY(hrow) * GLYPH_HEIGHT;
		//Find the end of string
		for(LineEnd = LineStart; (IsLineBreak (*LineEnd) != 0) && (*LineEnd!=0); LineEnd++);
		//Terminate current string with 0 to call GetString
		save = *LineEnd; // save the original char
		*LineEnd = 0;
		
		PostManagerDisplayQuietBootMessage(NULL_LINE, GLYPH_WIDTH * HOTKEYPX(0), yi, CA_AttributeLeftBottom, Foreground, Background);
		PostManagerDisplayQuietBootMessage(LineStart, xi, yi, CA_AttributeLeftBottom, Foreground, Background);
		if (save == 0) break;
		*LineEnd = save; //restore the original char

		//skip the line break
		for(LineEnd; (IsLineBreak (*LineEnd) == 0) && (*LineEnd!=0); LineEnd++)
		{
			switch(*LineEnd){
				case '\n':
					hrow ++;
				  break;
				case '\r':
					//hcol = PX;
				  break;
				default:
					break;
			}
		}
		LineStart = LineEnd; // continue with the next line
	}
}

VOID DisplayHotKeyTips()
{
  
  if(gQuietBoot && gPressHotKeys ){
      if(!(gPressHotKeys & CALLBACK_ONCE_KEY_BIT)){//Display once
        PrintHotKeyTips(gPressHotKeys);
        gPressHotKeys |= CALLBACK_ONCE_KEY_BIT;
      }
  }
  
}

VOID DisplayHotKeyTipsCallBack(IN EFI_EVENT Event, IN VOID *Context)
{
	DisplayHotKeyTips();
}

VOID HotKeyHelpMsg (VOID)
{
  EFI_STATUS Status;
  UINTN SizeofInfo;
  EFI_CONSOLE_CONTROL_SCREEN_MODE Mode;
  EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;
  AMI_POST_MANAGER_PROTOCOL *PostManager = NULL;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GopProtocol = NULL;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo =NULL;
  CHAR16 *HotKeyMsgText = NULL;
  //<Yaosr001-20190731 Support BIOS HotKey select. +>
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
  SETUP_DATA          SetupData;
  UINTN               SetupDataSize;
  EFI_GUID            gEfiSetupGuid = SETUP_GUID; 
#endif
  //<Yaosr001-20190731 Support BIOS HotKey select. ->
  
  TRACE((-1, "HotKeyHelpMsg\n"));
  
  Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid,NULL,&PostManager);    
  if (EFI_ERROR(Status)) PostManager=NULL;
  Status = pBS->LocateProtocol(&gEfiConsoleControlProtocolGuid,NULL,&ConsoleControl);
  if (EFI_ERROR(Status)) ConsoleControl=NULL;
  Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,&GopProtocol);
  if (EFI_ERROR(Status)) GopProtocol=NULL;
  Status = ConsoleControl->GetMode(ConsoleControl,&Mode,NULL,NULL);  
  TRACE((-1, "HotKey: Status=%r Mode=%d \n", Status, Mode));
	if (EFI_ERROR(Status)) return;

  GopProtocol->QueryMode(GopProtocol, GopProtocol->Mode->Mode, &SizeofInfo, &ModeInfo);
  HotKeyMsgText = NULL;
  //<Yaosr001-20190731 Support BIOS HotKey select. +>
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
  SetupDataSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
  TRACE ((-1, "gRT->GetVariable status %r, SetupData.BiosHotKeySupport: %d \n", Status, SetupData.BiosHotKeySupport));
  if(!EFI_ERROR(Status) && SetupData.BiosHotKeySupport == 0){
    HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_HOT_KEY_DISABLE_HELP) );
  }else{
    HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_SHOW_HOT_KEY_HELP) );
  }
#else
  HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_SHOW_HOT_KEY_HELP) );
#endif
  //<Yaosr001-20190731 Support BIOS HotKey select. -> 
  if ( HotKeyMsgText != NULL ){
    OemDisplayHotKeyPostMessage(HotKeyMsgText,0,0);
  }
  MemFreePointer( (VOID **)&HotKeyMsgText );

	return;
}
//Machong DEBUG HotKey Help Message }
BOOLEAN HotKeyHelpMsgEX (VOID)
{
  EFI_STATUS Status;
  UINTN SizeofInfo;
  EFI_CONSOLE_CONTROL_SCREEN_MODE Mode;
  
  UINTN	  Index;
  EFI_INPUT_KEY Key;
  //Lizhq 20140815 First boot & boot w/ default configuration message prompt [-]{
  //	UINTN                                 			      BufferSize;
	
  //	UINT32 		BootFlow = BOOT_FLOW_CONDITION_FIRST_BOOT;
  //	EFI_GUID 	BootFlowGuid = BOOT_FLOW_VARIABLE_GUID;
  //Lizhq 20140815 First boot & boot w/ default configuration message prompt [-]}
  UINT32    BootMode = BOOT_WITH_FULL_CONFIGURATION;
  
  EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;
  AMI_POST_MANAGER_PROTOCOL *PostManager = NULL;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GopProtocol = NULL;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo =NULL;
  AMITSESETUP TSEVar;
  UINTN Size;
  EFI_GUID  AmiTseSetupguid = AMITSESETUP_GUID;
 
  BOOLEAN bScreenUsed = FALSE;
//<lvych00320160810+> 
  TRACE((-1, "HotKeyHelpMsgEX\n")); 
  Size = sizeof (AMITSESETUP);        
  Status = gRT->GetVariable(L"AMITSESetup", \
                             &AmiTseSetupguid, \
                             NULL, \
                             &Size, \
                             &TSEVar );
	if (TSEVar.AMISilentBoot == 0) return bScreenUsed;
  
  Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid,NULL,&PostManager);    
  if (EFI_ERROR(Status)) PostManager=NULL;
  Status = pBS->LocateProtocol(&gEfiConsoleControlProtocolGuid,NULL,&ConsoleControl);
  if (EFI_ERROR(Status)) ConsoleControl=NULL;
  Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,&GopProtocol);
  if (EFI_ERROR(Status)) {
//temp <bieyb20160927>	  ASSERT_EFI_ERROR (Status);
	  return bScreenUsed;
  }
  

  Status = ConsoleControl->GetMode(ConsoleControl,&Mode,NULL,NULL);  
  TRACE((-1, "HotKeyEX: Status=%r Mode=%d \n", Status, Mode));
  if (EFI_ERROR(Status)) return bScreenUsed;
  if (1){//(Mode==EfiConsoleControlScreenGraphics){
	TRACE((-1, "HotKey: QueryMode 1. \n"));
    GopProtocol->QueryMode(GopProtocol, GopProtocol->Mode->Mode, &SizeofInfo, &ModeInfo); 
    //Lizhq 20140815 First boot & boot w/ default configuration message prompt [-]{   
    /*
    BufferSize       = sizeof (BootFlow);
    
    Status = pRS->GetVariable (L"BootFlow", 
    &BootFlowGuid, 
    NULL,
    &BufferSize,
    &BootFlow);
    */
    //Lizhq 20140815 First boot & boot w/ default configuration message prompt [-]}  
    
//<lvych00320160810->
    //Lizhq 20140815 First boot & boot w/ default configuration message prompt [m]{ 
    BootMode = GetBootModeHob ();
    TRACE((-1, "HotKey: BootMode=%d \n", BootMode));
    if(PcdGetBool(PcdFirstBoot) || (BootMode  == BOOT_WITH_DEFAULT_SETTINGS))
    {
	    //Init Screen
	    bScreenUsed = TRUE;  
	    PostManager->DisplayQuietBootMessage(
	          NULL_LINE, 0, ModeInfo->VerticalResolution-32, CA_AttributeCustomised,
	          Foreground, Background);
	    PostManager->DisplayQuietBootMessage(
	          NULL_LINE, 0, ModeInfo->VerticalResolution-16, CA_AttributeCustomised,
	          Foreground, Background);
	    
    }
    
    if(PcdGetBool(PcdFirstBoot))
    {
      PcdSetBool(PcdFirstBoot, FALSE);
      TRACE((-1, "HotKey: BOOT_FLOW_CONDITION_FIRST_BOOT\n"));
    
      if(BootMode  == BOOT_WITH_DEFAULT_SETTINGS) 
      {
        PostManager->DisplayQuietBootMessage(
          L"Press F1 to continue, BIOS is boot from default configuration...", 0, ModeInfo->VerticalResolution-16, CA_AttributeCustomised,
          Foreground, Background);       
        do {
          gBS->WaitForEvent( 1, gST->ConIn->WaitForKey, &Index );
          gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
        } while(Key.ScanCode != EFI_SCAN_F1);  
      }else{
        PostManager->DisplayQuietBootMessage(
          L"Press F1 to continue, BIOS is updated...", 0, ModeInfo->VerticalResolution-16, CA_AttributeCustomised,
          Foreground, Background);       
        do {
          gBS->WaitForEvent( 1, gST->ConIn->WaitForKey, &Index );
          gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
        } while(Key.ScanCode != EFI_SCAN_F1); 
      } //End if(BootMode  == BOOT_WITH_DEFAULT_SETTINGS) 
    }//End if(PcdGetBool(PcdFirstBoot)) 
    else{
      if(BootMode  == BOOT_WITH_DEFAULT_SETTINGS){
        PostManager->DisplayQuietBootMessage(
          L"Press F1 to continue, BIOS is boot from default configuration...", 0, ModeInfo->VerticalResolution-16, CA_AttributeCustomised,
          Foreground, Background);       
        do {
          gBS->WaitForEvent( 1, gST->ConIn->WaitForKey, &Index );
          gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
        } while(Key.ScanCode != EFI_SCAN_F1);  
      }
    }
    //Lizhq 20140815 First boot & boot w/ default configuration message prompt [m]}
 	}//End if (Mode==EfiConsoleControlScreenGraphics)
	return bScreenUsed;
}
//<lvych003-20161201 Add clear cmos message+>
BOOLEAN ShowCmosClearMsg (VOID)
{
  EFI_STATUS                                Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL              *GopProtocol = NULL;
  EFI_GUID                                  AmiTseSetupguid = AMITSESETUP_GUID;
  AMITSESETUP                               TSEVar;  
  EFI_INPUT_KEY                             Key;
  UINTN                                     Index;
  UINTN                                     Size;
  BOOLEAN                                   bScreenUsed = FALSE;
//<lvych003-20170408 Improve show clear cmos message behavior+>
  UINTN                                     TimeCount = 600; //unit 100ms//<lvych003-20170408 mantis:0013240 >
  EFI_TPL                                   CurrentTpl = 0;  //<lvych001-20171024 Revised keyboard maybe NG when cmos was cleared.>

  TRACE((-1, "ShowCmosClearMsg\n")); 
  if(!PcdGetBool(PcdCmosClear)) return bScreenUsed;
  Size = sizeof (AMITSESETUP);        
  Status = gRT->GetVariable(L"AMITSESetup", \
                             &AmiTseSetupguid, \
                             NULL, \
                             &Size, \
                             &TSEVar );
  if (TSEVar.AMISilentBoot == 0) return bScreenUsed;
  Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,&GopProtocol);
  if (EFI_ERROR(Status)) return bScreenUsed;

  //Init Screen
  bScreenUsed = TRUE;
  HotKeyMsgText = NULL;
  HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_CLEAR_CMOS_HELP) );
  
  if ( HotKeyMsgText != NULL ){
	ClearHotKeyMsgScreen();
    OemDisplayHotKeyPostMessage(HotKeyMsgText,0,1);
  }
  MemFreePointer( (VOID **)&HotKeyMsgText );
//<lvych001-20171024 Revised keyboard maybe NG when cmos was cleared.+>
  gST->ConIn->Reset( gST->ConIn, FALSE );
  CurrentTpl = gBS->RaiseTPL( TPL_HIGH_LEVEL ); //we store the current TPL
  gBS->RestoreTPL( TPL_APPLICATION); //setting the TPL level as application Level TPL
  do {
    gBS->WaitForEvent( 1, gST->ConIn->WaitForKey, &Index );
    gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
    gBS->Stall(100000);//delay 100ms
    TimeCount--;
  } while(Key.ScanCode != EFI_SCAN_ESC && TimeCount);
  gBS->RaiseTPL( TPL_HIGH_LEVEL ); // Raising the TPL before reverting back as TPL may be changes in between
  gBS->RestoreTPL( CurrentTpl );
  Status = gST->ConIn->Reset( gST->ConIn, FALSE );
//<lvych001-20171024 Revised keyboard maybe NG when cmos was cleared.->

  PcdSetBool(PcdCmosClear, FALSE);

  return bScreenUsed;
}
//<lvych003-20161201 Add clear cmos message->
//<lvych00120160602->
//<lvych001-20170421 Fix it always shows hotkey messages when enter to EFI OS +>
VOID CleanUpLogo();
VOID OemPreEfiBootProcess(VOID)
{
  CleanUpLogo();
}
//<lvych001-20170421 Fix it always shows hotkey messages when enter to EFI OS ->
//<lvych001-20161129 Add press hot key tips +>
VOID PrintHotKeyTips(UINT8 HotKeys)
{
//<lvych004-20161210 Revised show press hot key tips location+>
  EFI_STATUS                                Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL              *GopProtocol = NULL;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION      *ModeInfo =NULL;
  UINTN                                     SizeofInfo;
  CHAR16                                    *HotKeyMsgText = NULL;

  if(HotKeys){
    
    if ( gQuietBoot )
    {
        Status = pBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,NULL,&GopProtocol);
        if (!EFI_ERROR(Status)){
            GopProtocol->QueryMode(GopProtocol, GopProtocol->Mode->Mode, &SizeofInfo, &ModeInfo);
            HotKeyMsgText = NULL;
            if(HotKeys & SETUP_ENTRY_KEY_BIT){
              HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ENTER_SETUP) );
            }else if(HotKeys & POPUP_ENTRY_KEY_BIT){
              HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ENTER_BBS_POPUP) );
            }else if(HotKeys & NETWORK_ENTRY_KEY_BIT){
              HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_BOOTING_NETWORK) );
            }
            
            if ( HotKeyMsgText != NULL ){
              ClearHotKeyMsgScreen();
              OemDisplayHotKeyPostMessage(HotKeyMsgText,0,1);
            }
            MemFreePointer( (VOID **)&HotKeyMsgText );
        }
    }else{
//<lvych001-20170502 Improve code+>
      
      if(HotKeys & SETUP_ENTRY_KEY_BIT){
        HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ENTER_SETUP) );
      }else if(HotKeys & POPUP_ENTRY_KEY_BIT){
        HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_ENTER_BBS_POPUP) );
      }else if(HotKeys & NETWORK_ENTRY_KEY_BIT){
        HotKeyMsgText = HiiGetString( gHiiHandle, STRING_TOKEN(STR_BOOTING_NETWORK) );
      }
      
      if ( HotKeyMsgText != NULL ){
        PostManagerDisplayPostMessage(HotKeyMsgText);
      }
      MemFreePointer( (VOID **)&HotKeyMsgText );
//<lvych001-20170502 Improve code->
    }
  }
//<lvych004-20161210 Revised show press hot key tips location->
}

BOOLEAN OemCheckKeyStateCond(
        IN AMI_EFI_KEY_DATA AmiKey,
        IN UINT16           ScanCode,
        IN CHAR16           UnicodeChar,
        IN UINT32           CondShiftState
        )
{
    
    if ( (AmiKey.Key.UnicodeChar == UnicodeChar) 
          && (AmiKey.Key.ScanCode == ScanCode)
#if TSE_USE_AMI_EFI_KEYCODE_PROTOCOL
          && (TSE_CHECK_SHIFTSTATE (AmiKey.KeyState.KeyShiftState, CondShiftState))
#endif
        ){
            return TRUE;
        }
    
    return FALSE;
    
}

//<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.+>
EFI_STATUS InitUgaStr()
{

	if (NULL == gST->ConOut)
		return EFI_NOT_READY;

	UpdateGoPUgaDraw();//In legacy option rom launch GOP will be removed so updating here else it will crash
	if(EFI_SUCCESS != HiiInitializeProtocol())
		return EFI_UNSUPPORTED;

	return EFI_SUCCESS;
}
//<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.->
VOID OemCheckForKey (AMI_EFI_KEY_DATA AmiKey)
{
	EFI_STATUS       Status; //<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.>
    //<Yaosr001-20190731 Support BIOS HotKey select. +>
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
	EFI_STATUS          SetupStatus;
	SETUP_DATA          SetupData;
    UINTN               SetupDataSize;
    EFI_GUID            gEfiSetupGuid = SETUP_GUID; 

    TRACE((-1, "OemCheckForKey\n"));
    SetupDataSize = sizeof(SETUP_DATA);
    SetupStatus = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
    TRACE ((-1, "gRT->GetVariable status %r, SetupData.BiosHotKeySupport: %d \n", SetupStatus, SetupData.BiosHotKeySupport));
#endif
    //<Yaosr001-20190731 Support BIOS HotKey select. ->

    if(gPressHotKeys)
        return;
//<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.+>
    Status = InitUgaStr();
    if (EFI_ERROR(Status))
    	return;
//<lvych001-20171214 Revised system maybe hang postcode B2 when press hotkey.->

    if( OemCheckKeyStateCond(AmiKey,SETUP_ENTRY_SCAN,SETUP_ENTRY_UNICODE,SETUP_ENTRY_SHIFT_STATE)
       || OemCheckKeyStateCond(AmiKey,SETUP_DEBUG_ENTRY_SCAN,SETUP_DEBUG_ENTRY_UNICODE,SETUP_DEBUG_ENTRY_SHIFT_STATE)
      )
    {
        //<Yaosr001-20190731 Support BIOS HotKey select. +>
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
        if(!EFI_ERROR(SetupStatus) && SetupData.BiosHotKeySupport == 0){
          return;
        }
#endif
        //<Yaosr001-20190731 Support BIOS HotKey select. ->
        gPressHotKeys |= SETUP_ENTRY_KEY_BIT;
        gEnterSetup = TRUE;//<lvych001-20170502 Improve code>
    }
    else if(OemCheckKeyStateCond(AmiKey,0,L'.',0))
    {
        gPressHotKeys |= SETUP_ENTRY_KEY_BIT;
        CheckandDeactivateSoftkbd();
        gEnterSetup = TRUE;//<lvych001-20170502 Improve code>
    }
#if SETUP_BBS_POPUP_ENABLE
    else if (OemCheckKeyStateCond(AmiKey,POPUP_MENU_ENTRY_SCAN,POPUP_MENU_ENTRY_UNICODE,POPUP_MENU_ENTRY_SHIFT_STATE))
    {
        gPressHotKeys |= POPUP_ENTRY_KEY_BIT;
    }
#endif
#if SETUP_OEM_KEY1_ENABLE            
    else if (OemCheckKeyStateCond(AmiKey,SETUP_OEM_KEY1_SCAN,SETUP_OEM_KEY1_UNICODE,SETUP_OEM_KEY1_SHIFT))
    {
        gPressHotKeys |= NETWORK_ENTRY_KEY_BIT;
    }
#endif
    TRACE((-1, "QuietBoot:%x,PressHotKeys:%x\n",gQuietBoot,gPressHotKeys));
    PrintHotKeyTips(gPressHotKeys);

}
//<lvych001-20161129 Add press hot key tips ->
