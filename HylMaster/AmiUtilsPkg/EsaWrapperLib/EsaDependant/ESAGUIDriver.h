//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header:
//
// $Revision: 
//
// $Date: 
//**********************************************************************
#ifndef _ESA_GUI_DRIVER_H_
#define _ESA_GUI_DRIVER_H_

#include "GraphicsLibBin.h"
extern "C"
{
#include <Uefi.h>
}

typedef
EFI_STATUS
(EFIAPI *ESA_INIT_GUI_DRV) (
		VOID *GuiConfigData
	);

typedef
EFI_STATUS
(EFIAPI *ESA_TERMINATE_GUI_DRV) (
		EFI_HANDLE ImageHandle
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SHOWPAGE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* ,
		void* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_CREATE_GUI_VIEW) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);
/*
typedef
EFI_STATUS
(EFIAPI *ESA_CREATE_NODE_ATTRIBUTES) ( EFI_HANDLE ,
		GUI_ATTR** ,
		GUI_MAP_ATTR* 
	);*/

typedef
EFI_STATUS
(EFIAPI *ESA_GET_CURRENT_PAGE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE **
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SET_INVALIDATE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* ,
		UINT8 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SET_CTRL_HANDLE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SET_CTRL_ATTRIBUTE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SET_CALLBACK_PTR) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* ,void *
	);

typedef
EFI_STATUS
(EFIAPI *ESA_UPDATE_CTRL_DATA_SRC) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_UPDATE_CTRL_VALUE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_UPDATE_CTRL_FOCUSABLE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_UPDATE_CTRL_STATE) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_IDENTIFY_CTRL_NODE_BY_HANDLE) ( EFI_HANDLE ,
		void *, 
		GUI_CTRL_INFO_NODE*,
		GUI_CTRL_INFO_NODE **
	);

typedef
EFI_STATUS
(EFIAPI *ESA_UPDATE_ATTRIBUTE) ( EFI_HANDLE ,
		GUI_ATTR** , 
		UINT32 , 
		UINT16 , 
		UINTN *
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SET_FOCUS) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE* 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_GET_FOCUS) ( EFI_HANDLE ,UINT8,
		GUI_CTRL_INFO_NODE* ,GUI_CTRL_INFO_NODE**
	);

typedef
EFI_STATUS
(EFIAPI *ESA_RUN_EVENT_LOOP) ( EFI_HANDLE 
	);

typedef
EFI_STATUS
(EFIAPI *ESA_EXIT_EVENT_LOOP) ( EFI_HANDLE ,
		BOOLEAN IsExit
	);

typedef
EFI_STATUS
(EFIAPI *ESA_SET_FILE_XREF) ( EFI_HANDLE ,
		struct _filexref  *
	);

typedef
EFI_STATUS
(EFIAPI *ESA_LOAD_APP_FONT_FILE) ( EFI_HANDLE ,
		VOID *InFontList
	);

typedef
EFI_STATUS
(EFIAPI *ESA_UPDATE_PROGRESS_BAR) ( EFI_HANDLE ,
		GUI_CTRL_INFO_NODE *, 
		UINT32 

	);

typedef
		VOID
(EFIAPI *ESA_DISPLAY_CURRENT_PAGE) (EFI_HANDLE 
	);

typedef
		VOID
(EFIAPI *ESA_UPDATE_PAGENODE_ATTRIBUTE) (EFI_HANDLE ,GUI_CTRL_INFO_NODE*,GUI_MAP_ATTR*);

typedef bool (EFIAPI *ESA_REGISTER_CALLBACK) (EFI_HANDLE, void*, void *, bool );
typedef void (EFIAPI *ESA_STOPWATCH_SERVICE)(EFI_HANDLE, UINT8 ,CHAR8 *);
typedef EFI_STATUS (EFIAPI *ESA_GET_KEY_DATA)(EFI_HANDLE, UINT8, void**);
typedef EFI_STATUS (EFIAPI *ESA_UPDATE_GUI_VIEW) (EFI_HANDLE, GUI_CTRL_INFO_NODE* );
typedef EFI_STATUS (EFIAPI *ESA_GUI_VIEW_RELEASE_HANDLER)(EFI_HANDLE, UINT8);
typedef EFI_STATUS (EFIAPI *ESA_GET_SCROLL_POSSITION)(EFI_HANDLE, GUI_CTRL_INFO_NODE* ,UINT8, int* );
typedef EFI_STATUS (EFIAPI *ESA_SET_SCROLL_POSSITION)(EFI_HANDLE, GUI_CTRL_INFO_NODE* ,UINT8,int );

typedef struct _ESA_GUI_DRIVER_PROTOCOL {
		UINT8								ProtocolVersion;
		BOOLEAN						ESAIntializeGUIDriverStatus;
		ESA_INIT_GUI_DRV 					ESAInitializeGUIDriver;
		ESA_TERMINATE_GUI_DRV 				ESATerminateGUIDriver;
		ESA_SHOWPAGE 						ESAShowPage;
		ESA_CREATE_GUI_VIEW 				ESACreateGUIView;
		//ESA_CREATE_NODE_ATTRIBUTES			ESACreateNodeAttributes;
		ESA_GET_CURRENT_PAGE				ESAGetCurrentPage;
		ESA_SET_INVALIDATE					ESASetInvalidate;
		ESA_SET_CTRL_HANDLE					ESASetControlHandle;//NOT SUPPORTED
		ESA_SET_CTRL_ATTRIBUTE				ESASetControlAttribute;
		ESA_SET_CALLBACK_PTR				ESASetCallbackPtr;
		ESA_UPDATE_CTRL_DATA_SRC			ESAUpdateControlDataSrc;
		ESA_UPDATE_CTRL_VALUE 				ESAUpdateControlValue;
		ESA_UPDATE_CTRL_FOCUSABLE 			ESAUpdateControlFocusable;//NOT SUPPORTED
		ESA_UPDATE_CTRL_STATE				ESAUpdateControlState;
		ESA_IDENTIFY_CTRL_NODE_BY_HANDLE 	ESAIdentifyCtrlNodeByHandle;
		ESA_UPDATE_ATTRIBUTE 				ESAUpdateAttribute;
		ESA_SET_FOCUS 						ESASet_focus; //NOT SUPPORTED
		ESA_GET_FOCUS 						ESAGet_focus; //NOT SUPPORTED
		ESA_RUN_EVENT_LOOP 					ESARunEventLoop;
		ESA_EXIT_EVENT_LOOP 				ESAExitEventLoop;		
		ESA_SET_FILE_XREF					ESASetFileXREF;
		ESA_LOAD_APP_FONT_FILE				EsaLoadAppFontFile;
		ESA_UPDATE_PROGRESS_BAR				ESAUpdateProgressBar; //NOT SUPPORTED
		ESA_DISPLAY_CURRENT_PAGE			ESADisplayCurrentPage;	
		ESA_UPDATE_PAGENODE_ATTRIBUTE		ESAUpdatePageNodeAttributes;
		ESA_REGISTER_CALLBACK				ESARegisterCallback;
		ESA_STOPWATCH_SERVICE				ESAStopwatchService;
		ESA_GET_KEY_DATA					ESAGetKeyData;
		ESA_UPDATE_GUI_VIEW   				ESAUpdateGUIView;
		ESA_GUI_VIEW_RELEASE_HANDLER        ESAGUIViewReleaseHandler;
		ESA_GET_SCROLL_POSSITION            ESAGetScrollPosition;
		ESA_SET_SCROLL_POSSITION            ESASetScrollPosition;
		
} ESA_GUI_DRIVER_PROTOCOL;

typedef struct _GUI_CONFIG_INFO
{
	EFI_HANDLE 				ImageHandle; 
	EFI_SYSTEM_TABLE 			*UefiSysTable;  //InitAmiLib(ImageHandle,ptrST);
	EFI_HII_HANDLE 				HiiHandle; 		//gHiiHandle = HiiHandle;
	VOID        				*FileXrefList;
	VOID 					*FontList;
	GUI_CTRL_INFO_NODE 			*CurrentPage;   //*gCurrentPage;
	VOID* 					CurrentCtrlFocusHnd;   //gCurrentCtrlFocusHnd;
	VOID*					CurPageHandle;         //gCurPageHandle;
	GUI_CTRL_INFO_NODE 			*CurrPageTree;   //*gCurrPageTree;
	bool                    		ExitAppStatus;
	bool                    		IsEsaInitOwner;
}GUI_CONFIG_INFO;
extern ESA_GUI_DRIVER_PROTOCOL gESADriverProtocol;
/****** DO NOT WRITE BELOW THIS LINE *******/

#endif
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

