
#ifndef __ESA_DRV_MGR_H
#define __ESA_DRV_MGR_H

#include "Token.h"

//extern "C"
//{
//#include "types.h"
//}
#include <Uefi.h>
#if (1==ESA_BINARY_SUPPORT)
#include "..\AmiTsePkg\SkiaPkg\GraphicsLibBin.h"
#include "..\AmiTsePkg\SkiaPkg\Protocol\EsaDatasrcProtocol.h"
#else
#include "EsaDependant/GraphicsLibBin.h"
#include "EsaDependant/EsaDatasrcProtocol.h"
#endif
#include "EsaGuiHeader.h"

#define STOPWATCH_START 0
#define STOPWATCH_STOP  1

typedef struct _APP_SPECIFIC_INFO
{
	EFI_HANDLE 				ImageHandle; 
	EFI_SYSTEM_TABLE 		*UefiSysTable;  //InitAmiLib(ImageHandle,ptrST);
	EFI_HII_HANDLE 			HiiHandle; 		//gHiiHandle = HiiHandle;
	VOID        			*FileXrefList;
	VOID 					*FontList;
}APP_SPECIFIC_INFO;


	EFI_STATUS InitEsaGuiDrv(APP_SPECIFIC_INFO *ApplicationConfig);
	void GraphicsLibUnInit();
	EFI_HANDLE GetCurAppHandle();
	void ShowPage(GUI_CTRL_INFO_NODE* ,void* );
	void CreateGUIView(GUI_CTRL_INFO_NODE*);
	void CreateNodeAttributes(GUI_ATTR** ,GUI_MAP_ATTR *);
	void GetCurrentPage(GUI_CTRL_INFO_NODE **);
	void SetInvalidate(GUI_CTRL_INFO_NODE* ,UINT8 );	
	void SetControlHandle(GUI_CTRL_INFO_NODE*);
	void SetControlAttribute(GUI_CTRL_INFO_NODE*);
	void SetCallbackPtr(GUI_CTRL_INFO_NODE* ,void *);
	void UpdateControlDataSrc(GUI_CTRL_INFO_NODE* );
	void UpdateControlvalue(GUI_CTRL_INFO_NODE* CtrlInfoNode);
	void UpdateControlFocusable(GUI_CTRL_INFO_NODE* );
	void UpdateControlstate(GUI_CTRL_INFO_NODE* CtrlInfoNode);
	void IdentifyCtrlNodeByHandle(void *, GUI_CTRL_INFO_NODE*,GUI_CTRL_INFO_NODE **);
	void UpdateAttribute(GUI_ATTR** , UINT32 ,UINT16 , UINTN *);
	void Set_focus(GUI_CTRL_INFO_NODE* );
	void Get_focus(GUI_CTRL_INFO_NODE* );
	void RunAppLoop();
	void ExitAppLoop(BOOLEAN IsExit);		
	void RunMsgLoop();
	void ExitMsgLoop(BOOLEAN IsExit);
	void setFileXREF(struct _filexref *);
	void SetFontNode(void *InFontList);
	void UpdateProgressBar(GUI_CTRL_INFO_NODE *,UINT32 );
	void DisplayCurrentPage();	
	void UpdatePageNodeAttributes(GUI_CTRL_INFO_NODE* PageTreeNode, GUI_MAP_ATTR* AttribMap);
	void RegisterCallback(void *Callback,void *param, BOOLEAN RegisterStatus);
	void StopWatchService(UINT8 ServiceValue,CHAR8 *Str);
	void Updating_PageTree(GUI_CTRL_INFO_NODE* CtrlInfoNode);
	void GetKeyData(UINT8 ProtocolType,void** KeyData);
	void UpdateGUIView(GUI_CTRL_INFO_NODE* PageTree);
	void GUIViewReleaseHandler(UINT8 viewreleasetype);
	void GetScrollPosition(GUI_CTRL_INFO_NODE* PageTree,UINT8 xyType, int* Coordinate);
	void SetScrollPosition(GUI_CTRL_INFO_NODE* PageTree,UINT8 xyType,UINT8 position);
	
	
EFI_STATUS NonHiiGetValue(
        IN ESA_DATA_SOURCE_PROTOCOL *This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT void** Value,
        IN OUT UINT32* ByteLength
);


EFI_STATUS NonHiiGetState (
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT UINT8* State
);


EFI_STATUS	NonHiiGetFocusable(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT BOOLEAN* Focusable
);

EFI_STATUS	NonHiiGetDataSrc(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT void** DataSrcStruct
);


EFI_STATUS	NonHiiControlCallback(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN void* Action, 
        IN OUT void *Value
);
#endif
