//************************************************************************
//************************************************************************
//**                                                                  					    	**
//**        (C)Copyright 2014, American Megatrends, Inc.            	**
//**                                                                  					    	**
//**                       All Rights Reserved.                       				**
//**                                                                  						**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  **
//**                                                                  						**
//**                       Phone: (770)-246-8600                      				**
//**                                                                  						**
//************************************************************************
//************************************************************************

//************************************************************************
// $Archive: $
//
// $Author: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
// Revision History
// ----------------
// $Log: $
//
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AMIRescue.cpp
//
// Description:	Contains AmiResce Entry point
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************
#include "EsaDrvMgr.h"
#if (1==ESA_BINARY_SUPPORT)
#include "../AmiTsePkg/SkiaPkg/Protocol/ESAGUIDriver.h"
#else
#include "EsaDependant/ESAGUIDriver.h"
#endif

extern "C"
{
extern EFI_RUNTIME_SERVICES  	*gRT;
extern EFI_BOOT_SERVICES     	*gBS;
extern EFI_GUID gEsaGuiDrvGuid;
}

#if !(1==ESA_BINARY_SUPPORT)
EFI_GUID gEsaGuiDrvGuid = {0xA2DF5376, 0xC2ED, 0x49C0, {0x90, 0xFF, 0x8B, 0x17, 0x3B, 0x0F, 0xD0, 0x66}};
#endif
	
#define ESA_DRV_PROTOCOL ((ESA_GUI_DRIVER_PROTOCOL*)_ESADriverProtocol)
bool gDrvInitStatus = false;

VOID 						*_ESADriverProtocol; //(ESA_GUI_DRIVER_PROTOCOL*)
EFI_HANDLE 				    _AppHandle = (EFI_HANDLE)NULL;

void GraphicsLibUnInit()
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESATerminateGUIDriver(_AppHandle);
	else
		gDrvInitStatus = false;
	_AppHandle = NULL;
	_ESADriverProtocol = (ESA_GUI_DRIVER_PROTOCOL *)NULL;
}



EFI_STATUS InitEsaGuiDrv(APP_SPECIFIC_INFO *ApplicationConfig)
{
	EFI_STATUS	Status=EFI_SUCCESS;
	GUI_CONFIG_INFO GuiConfigData;
	gBS = ApplicationConfig->UefiSysTable->BootServices;
	
	Status = gBS->LocateProtocol((EFI_GUID*)&gEsaGuiDrvGuid,(VOID*)NULL,(VOID **)&_ESADriverProtocol);
	if(EFI_SUCCESS == Status)
	{
		GuiConfigData.ImageHandle 	= ApplicationConfig->ImageHandle;
		GuiConfigData.UefiSysTable 	= ApplicationConfig->UefiSysTable;
		GuiConfigData.HiiHandle 	= ApplicationConfig->HiiHandle;
		GuiConfigData.FileXrefList 	= ApplicationConfig->FileXrefList;
		GuiConfigData.FontList 		= ApplicationConfig->FontList;	
		Status = ESA_DRV_PROTOCOL->ESAInitializeGUIDriver(&GuiConfigData);
		
		_AppHandle = ApplicationConfig->ImageHandle;
		gDrvInitStatus = true;
	}
	return Status;
}

EFI_HANDLE GetCurAppHandle()
{
	if(true == gDrvInitStatus)
		return _AppHandle;
	else
		return NULL;
}

void ShowPage(GUI_CTRL_INFO_NODE* PageNode,void* Controlhandle)
{
	if(true == gDrvInitStatus)
	{
		ESA_DRV_PROTOCOL->ESAShowPage(_AppHandle,PageNode,Controlhandle);
		//the following is added to set focus to control.as ESA only support focus to Hii_View_Type_Value
		if(NULL != Controlhandle)
		{
			UINT32 OrgValueType = ((CONTROL_HANDLE*)Controlhandle)->ViewType;
				((CONTROL_HANDLE*)Controlhandle)->ViewType = Hii_View_Type_Value;
			ESA_DRV_PROTOCOL->ESAShowPage(_AppHandle,PageNode,Controlhandle);
			((CONTROL_HANDLE*)Controlhandle)->ViewType = OrgValueType;
		}
	}
}

void CreateGUIView(GUI_CTRL_INFO_NODE* ControlNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESACreateGUIView(_AppHandle,ControlNode);
}
/*
void CreateNodeAttributes(GUI_ATTR** Attrib,VOID *AttribTable)
{
	ESA_DRV_PROTOCOL->ESACreateNodeAttributes(_AppHandle,Attrib,(GUI_MAP_ATTR *)AttribTable);
}*/

void GetCurrentPage(GUI_CTRL_INFO_NODE ** PageNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAGetCurrentPage(_AppHandle,PageNode);
}

void SetInvalidate(GUI_CTRL_INFO_NODE* ControlNode,UINT8 InvalidFlag)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESASetInvalidate(_AppHandle,ControlNode,InvalidFlag);
}

void SetControlHandle(GUI_CTRL_INFO_NODE* ControlNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESASetControlHandle(_AppHandle,ControlNode);
}

void SetControlAttribute(GUI_CTRL_INFO_NODE* ControlNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESASetControlAttribute(_AppHandle,ControlNode);
}

void SetCallbackPtr(GUI_CTRL_INFO_NODE* ControlNode,void *FuncPtr)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESASetCallbackPtr(_AppHandle,ControlNode,FuncPtr);
}

void UpdateControlDataSrc(GUI_CTRL_INFO_NODE* ControlNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAUpdateControlDataSrc(_AppHandle,ControlNode);
}

void UpdateControlvalue(GUI_CTRL_INFO_NODE* CtrlInfoNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAUpdateControlValue(_AppHandle,CtrlInfoNode);
}

void UpdateControlFocusable_Dummy(GUI_CTRL_INFO_NODE* ControlNode)
{
	if( (true == gDrvInitStatus) && (ESA_DRV_PROTOCOL->ESAUpdateControlFocusable))
		ESA_DRV_PROTOCOL->ESAUpdateControlFocusable(_AppHandle,ControlNode);
}

void UpdateControlstate(GUI_CTRL_INFO_NODE* CtrlInfoNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAUpdateControlState(_AppHandle,CtrlInfoNode);
}
void Updating_PageTree(GUI_CTRL_INFO_NODE* CtrlInfoNode)
{
	UpdateControlDataSrc(CtrlInfoNode);
	UpdateControlvalue(CtrlInfoNode);
	UpdateControlFocusable_Dummy(CtrlInfoNode);
	UpdateControlstate(CtrlInfoNode);
}
void IdentifyCtrlNodeByHandle(void *Controlhandle,GUI_CTRL_INFO_NODE* CurPage,GUI_CTRL_INFO_NODE **CurNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAIdentifyCtrlNodeByHandle(_AppHandle,Controlhandle,CurPage,CurNode);
}

void UpdateAttribute(GUI_ATTR** GUIAttributeList, UINT32 Attr_ID, UINT16 Attr_Size, UINTN *Attr_Value)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAUpdateAttribute(_AppHandle,GUIAttributeList,Attr_ID,Attr_Size,Attr_Value);
}

void Set_focus(GUI_CTRL_INFO_NODE* ControlNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESASet_focus(_AppHandle,ControlNode);
}

void Get_focus(GUI_CTRL_INFO_NODE* ControlNode)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAGet_focus(_AppHandle, 0, ControlNode, &ControlNode);
}

void RunAppLoop()
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESARunEventLoop(_AppHandle);
}

void ExitAppLoop(BOOLEAN IsExit)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAExitEventLoop(_AppHandle,IsExit);
}

void RunMsgLoop()
{
	RunAppLoop();
	ExitAppLoop(FALSE);
}

void ExitMsgLoop(BOOLEAN IsExit)
{
	ExitAppLoop(IsExit);
}

void setFileXREF(struct _filexref *InFileXref)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESASetFileXREF(_AppHandle,InFileXref);
}

void SetFontNode(void *InFontList)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->EsaLoadAppFontFile(_AppHandle,InFontList);
}

void UpdateProgressBar(GUI_CTRL_INFO_NODE *PageNode,UINT32 ProgressVal)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAUpdateProgressBar(_AppHandle,PageNode,ProgressVal);
}

void DisplayCurrentPage()
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESADisplayCurrentPage(_AppHandle);
}

void UpdatePageNodeAttributes(GUI_CTRL_INFO_NODE* PageTreeNode, GUI_MAP_ATTR* AttribMap)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAUpdatePageNodeAttributes(_AppHandle,PageTreeNode,(GUI_MAP_ATTR*)AttribMap);
}

void RegisterCallback(void *Callback,void *param,bool RegisterStatus)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESARegisterCallback(_AppHandle, Callback,param,RegisterStatus);
}

void StopWatchService(UINT8 ServiceValue,CHAR8 *Str)
{
	if(true == gDrvInitStatus)
		ESA_DRV_PROTOCOL->ESAStopwatchService(_AppHandle, ServiceValue,Str);
}

void GetKeyData(UINT8 ProtocolType,void** KeyData)
{
    if(true == gDrvInitStatus)
        ESA_DRV_PROTOCOL->ESAGetKeyData(_AppHandle,ProtocolType,KeyData);   
}

void UpdateGUIView(GUI_CTRL_INFO_NODE* PageTree)
{
    if(true == gDrvInitStatus)
        ESA_DRV_PROTOCOL->ESAUpdateGUIView(_AppHandle,PageTree);
}

void GUIViewReleaseHandler(UINT8 viewreleasetype)
{
    if(true == gDrvInitStatus)
        ESA_DRV_PROTOCOL->ESAGUIViewReleaseHandler(_AppHandle,viewreleasetype);
}

void GetScrollPosition(GUI_CTRL_INFO_NODE* PageTree,UINT8 xyType, int* Coordinate)
{
    if(true == gDrvInitStatus)
           ESA_DRV_PROTOCOL->ESAGetScrollPosition(_AppHandle,PageTree,xyType,Coordinate);
}

void SetScrollPosition(GUI_CTRL_INFO_NODE* PageTree,UINT8 xyType,UINT8 position)
{
    if(true == gDrvInitStatus)
        ESA_DRV_PROTOCOL->ESASetScrollPosition(_AppHandle,PageTree,xyType,position); 
}
//************************************************************************
//************************************************************************
//**                                                    		    	**
//**        (C)Copyright 2014, American Megatrends, Inc.            	**
//**                                                    		    	**
//**                       All Rights Reserved.            				**
//**                                                    				**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                    				**
//**                       Phone: (770)-246-8600           				**
//**                                                    				**
//************************************************************************
//************************************************************************
