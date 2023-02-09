//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**    (C)Copyright 2012-2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
#ifndef __GRAPHICSLIBBIN_H__
#define __GRAPHICSLIBBIN_H__

#include <Uefi/UefiBaseType.h>

//Value Types to be used in GUIValueType in GUI_CTRL_INFO_NODE structure
//Partially modelled after IFR question data and IFR value types
//Data Value Types - modelled after IFR value types

#define GUI_VALUE_TYPE_BASE					100
#define GUI_VALUE_TYPE_NONE					0
#define GUI_VALUE_TYPE_UNUM_8				GUI_VALUE_TYPE_BASE+1
#define GUI_VALUE_TYPE_UNUM_16				GUI_VALUE_TYPE_BASE+2
#define GUI_VALUE_TYPE_UNUM_32				GUI_VALUE_TYPE_BASE+3
#define GUI_VALUE_TYPE_UNUM_64				GUI_VALUE_TYPE_BASE+4
#define GUI_VALUE_TYPE_STRING				GUI_VALUE_TYPE_BASE+5
#define GUI_VALUE_TYPE_OPTION_SELECTION		GUI_VALUE_TYPE_BASE+6 //data type should be GUI_VALUE_TYPE_NUM_16
#define GUI_VALUE_TYPE_BOOLEAN				GUI_VALUE_TYPE_BASE+7
#define GUI_VALUE_TYPE_DATE					GUI_VALUE_TYPE_BASE+8
#define GUI_VALUE_TYPE_TIME					GUI_VALUE_TYPE_BASE+9
#define GUI_VALUE_TYPE_NUM_8				GUI_VALUE_TYPE_BASE+10
#define GUI_VALUE_TYPE_NUM_16				GUI_VALUE_TYPE_BASE+11
#define GUI_VALUE_TYPE_NUM_32				GUI_VALUE_TYPE_BASE+12
#define GUI_VALUE_TYPE_NUM_64				GUI_VALUE_TYPE_BASE+13
#define GUI_VALUE_TYPE_PWD					GUI_VALUE_TYPE_BASE+14

#define GUI_VALUE_TYPE_OPTION_ORDER         GUI_VALUE_TYPE_BASE+16 //Value Type for OrderedList Control
#define GUI_VALUE_TYPE_STRING_ID	        GUI_VALUE_TYPE_BASE+17 
#define GUI_VALUE_TYPE_GUID		    	    GUI_VALUE_TYPE_BASE+18 
// Data Sources are referring to the data that can be used by controls, but they are not values
// They include - the Minimum & max number of characters in the string
// min, max and step for a numeric controls

#define GUI_DATA_SRC_TYPE_BASE						200
#define GUI_DATA_SRC_TYPE_NONE						0
#define GUI_DATA_SRC_TYPE_STRING					GUI_DATA_SRC_TYPE_BASE+1
#define GUI_DATA_SRC_TYPE_OPTION_LIST		        GUI_DATA_SRC_TYPE_BASE+2
#define GUI_DATA_SRC_TYPE_OPTION    		        GUI_DATA_SRC_TYPE_BASE+3
#define GUI_DATA_SRC_TYPE_NUM_8						GUI_DATA_SRC_TYPE_BASE+4
#define GUI_DATA_SRC_TYPE_NUM_16					GUI_DATA_SRC_TYPE_BASE+5
#define GUI_DATA_SRC_TYPE_NUM_32					GUI_DATA_SRC_TYPE_BASE+6
#define GUI_DATA_SRC_TYPE_NUM_64					GUI_DATA_SRC_TYPE_BASE+7
#define GUI_DATA_SRC_TYPE_UNUM_8					GUI_DATA_SRC_TYPE_BASE+8
#define GUI_DATA_SRC_TYPE_UNUM_16					GUI_DATA_SRC_TYPE_BASE+9
#define GUI_DATA_SRC_TYPE_UNUM_32					GUI_DATA_SRC_TYPE_BASE+10
#define GUI_DATA_SRC_TYPE_UNUM_64					GUI_DATA_SRC_TYPE_BASE+11
#define GUI_DATA_SRC_TYPE_PROGRESSBAR					GUI_DATA_SRC_TYPE_BASE+12

/// Define GUI Controls - List of possible Control Types => GUICtrlViewType in GUI_CTRL_INFO_NODE
#define GUI_VIEW_CONTROL_BASE			1000
#define GUI_VIEW_TEXT_CONTROL			GUI_VIEW_CONTROL_BASE+1
#define GUI_VIEW_BUTTON_CONTROL			GUI_VIEW_CONTROL_BASE+2
#define GUI_VIEW_CHECK_CONTROL			GUI_VIEW_CONTROL_BASE+3
#define GUI_VIEW_LISTBOX_CONTROL		GUI_VIEW_CONTROL_BASE+4
#define GUI_VIEW_IMAGE_CONTROL			GUI_VIEW_CONTROL_BASE+5
#define GUI_VIEW_EDITBOX_CONTROL		GUI_VIEW_CONTROL_BASE+6
#define GUI_VIEW_ROOT       			GUI_VIEW_CONTROL_BASE+7
#define GUI_VIEW_COMBO_CONTROL   		GUI_VIEW_CONTROL_BASE+8
#define GUI_VIEW_DIAL_CONTROL   		GUI_VIEW_CONTROL_BASE+9
#define GUI_VIEW_DATE_CONTROL   		GUI_VIEW_CONTROL_BASE+10
#define GUI_VIEW_TIME_CONTROL           GUI_VIEW_CONTROL_BASE+11
#define GUI_VIEW_PROGRESS_BAR_CONTROL           GUI_VIEW_CONTROL_BASE+12

//GUI_VIEW for containers
#define GUI_VIEW_CONTAINER_BASE								2000
#define GUI_VIEW_PAGE_CONTAINER								GUI_VIEW_CONTAINER_BASE+1
#define GUI_VIEW_VERTICAL_LIST_CONTROL_CONTAINER			GUI_VIEW_CONTAINER_BASE+2
#define GUI_VIEW_HORIZONTAL_LIST_CONTROL_CONTAINER		    GUI_VIEW_CONTAINER_BASE+3
#define GUI_VIEW_BESTFIT_CONTROL_CONTAINER					GUI_VIEW_CONTAINER_BASE+4
#define GUI_VIEW_MOVING_CONTROL_CONTAINER	                GUI_VIEW_CONTAINER_BASE+5
#define GUI_VIEW_DRAG_DROP_CONTROL_CONTAINER				GUI_VIEW_CONTAINER_BASE+6
#define GUI_VIEW_ORDERED_LIST_CONTROL_CONTAINER				GUI_VIEW_CONTAINER_BASE+7


typedef struct _GUI_ATTR GUI_ATTR;
struct _GUI_ATTR{
	UINT32		Attr_ID;
	UINT16		Attr_Size; //size of the value
	GUI_ATTR*	Next;
	UINT8		Attr_Value[];    
};

// GUI Tree node definition. 
// GUI Tree need to be constructed by the setup engine for each page.
// The engine can decide if the 

typedef struct _GUI_CTRL_INFO_NODE GUI_CTRL_INFO_NODE;
struct _GUI_CTRL_INFO_NODE{
	void*		CtrlHandle;			// Ptr to SDA control - multiple nodes can point to the same SDA control (ex, title, defaults button on every page)
	UINT32		GUIValueType;		// Type of the Value
	UINT32		GUIDataSrcType;		// Type of the DataSrc
	UINT32		GUICtrlViewType;    //
	void*		GUICtrlViewHandle;  // IViewBody* runtime ptr to a view
	GUI_ATTR*	GUIAttributeList;	// Will be passed down to the control and unpacked by the control
	UINT8		InvalidateFlag;
	BOOLEAN		GUICallback;		// Set callback handler. The actions will be control dependent, the value - depending on Value type
	UINT32		GUICtrlDisplayID;
	UINT32      CTRLnodeID;
	GUI_CTRL_INFO_NODE* Parent;
	GUI_CTRL_INFO_NODE* Child;
	GUI_CTRL_INFO_NODE* Next;
	GUI_CTRL_INFO_NODE* Prev;
};

#endif
