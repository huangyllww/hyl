/**
 * Copyright (c) 2011 - 2012 American Megatrends Inc.
 */

#ifndef __ESAGUIHEADER_H__
#define __ESAGUIHEADER_H__

#define HiiControl		0x1000
#define NonHiiControl	0x1001
#define MessageBox		0x1002

#define INVALIDATE_STATE 0x01  // new state will be updated 
#define INVALIDATE_VALUE 0x02  // value and state will be updated
#define INVALIDATE_DS    0x04  // if the whole DS was changed - combo box list changed for instance - everything will have to be updated.
                               // should be used only when necessary
#define INVALIDATE_TREE  0x08  // the whole subtree should be recreated - for instance if there's a new control list


//////////////////
//Attribute IDs  - 
// -> common to all the views
#define ATTR_COMMON_BASE				300
#define ATTR_X							ATTR_COMMON_BASE+1//int
#define ATTR_Y							ATTR_COMMON_BASE+2//int
#define ATTR_WIDTH						ATTR_COMMON_BASE+3//int
#define ATTR_HEIGHT						ATTR_COMMON_BASE+4//int
#define ATTR_FGCOLOR					ATTR_COMMON_BASE+5//uint32
#define ATTR_BGCOLOR					ATTR_COMMON_BASE+6//uint32
#define ATTR_RESOURCE_FILE				ATTR_COMMON_BASE+7//char* -> this is currently file name -> will have to support guided file names as well
                                                          //or char16*
#define ATTR_FONTTYPEFACEFILENAME		ATTR_COMMON_BASE+8// char* ->fontname. If not set, default will be used Helvetica - loads from file system
#define ATTR_FONTTYPEFACEFILEGUID		ATTR_COMMON_BASE+9// FFS GUID - loads from rom 
                                                          // possible combine in one attr that will accept either guid or filename
#define ATTR_FONTSIZE					ATTR_COMMON_BASE+10// uint32 - if not set - 18 (use SDL tokens for defaults, and for all other defaults)
#define ATTR_FONTSTYLE					ATTR_COMMON_BASE+11// uint32 (dbl check the type SkTypeface::style) least important - will assume normal if not set
#define ATTR_ALIGNMENT					ATTR_COMMON_BASE+12// controls the position of text in the view UINT8
#define ATTR_RESOURCE_GUID				ATTR_COMMON_BASE+13

// tbd : define attributes for x, y, w, & h - relative to the parent view, since we may not know the original resolution

// -> common for all the controls but may not be necessarily set for all of them
// currently customize the different colors and images for the major states - can be expanded on demand
// to include attr for different fonts/colors for different states. for now well have most used variations for images and colors
#define ATTR_FGCOLORFOCUS				ATTR_COMMON_BASE+14//uint32
#define ATTR_FGCOLORDISABLED			ATTR_COMMON_BASE+15//uint32	
#define ATTR_FGCOLORMOUSEOVER			ATTR_COMMON_BASE+16//uint32
#define ATTR_RESOURCEFOCUS				ATTR_COMMON_BASE+17//char* or char16* 
#define ATTR_RESOURCEDISABLED			ATTR_COMMON_BASE+18
#define ATTR_RESOURCEMOUSEOVER			ATTR_COMMON_BASE+19
#define ATTR_XINSET						ATTR_COMMON_BASE+20//int
#define ATTR_YINSET						ATTR_COMMON_BASE+21
#define ATTR_RESOURCEFOCUS_GUID			ATTR_COMMON_BASE+22
#define ATTR_RESOURCEDISABLED_GUID		ATTR_COMMON_BASE+24
#define ATTR_RESOURCEMOUSEOVER_GUID		ATTR_COMMON_BASE+25
#define ATTR_VALIGNMENT					ATTR_COMMON_BASE+26// controls the position of text in the view UINT8
#define ATTR_OPACITY					ATTR_COMMON_BASE+27// set opacity to view
#define ATTR_AUTOHEIGHT					ATTR_COMMON_BASE+28// bool 
#define ATTR_AUTOWIDTH					ATTR_COMMON_BASE+29// bool
#define ATTR_AUTORESIZE					ATTR_COMMON_BASE+30// bool
#define ATTR_LMARGIN					ATTR_COMMON_BASE+31
#define ATTR_RMARGIN					ATTR_COMMON_BASE+32
#define ATTR_CLICKABLE					ATTR_COMMON_BASE+33 //bool
#define ATTR_BGCOLORFOCUS				ATTR_COMMON_BASE+34//uint32
#define ATTR_BGCOLORMOUSEOVER			ATTR_COMMON_BASE+35//uint32
#define ATTR_BGCOLORDISABLED			ATTR_COMMON_BASE+36//uint32
#define ATTR_TOPMARGIN					ATTR_COMMON_BASE+46//uint32

//Attributes for containers
#define ATTR_CONTAINER_ORIENTATION    	ATTR_COMMON_BASE+37//Orientation specified the axis of arrangement of the controls in a container
#define ATTR_CONTAINER_SCROLL_AREA_SIZE	ATTR_COMMON_BASE+38//Scroll Area Size is the width of the scrollable portion inside a container
#define ATTR_CONTAINER_X_INSET          ATTR_COMMON_BASE+39//x-inset specifies the no. of pixels from the left till the first child control
#define ATTR_CONTAINER_Y_INSET          ATTR_COMMON_BASE+40//y-inset specifies the no. of pixels from the top till the first child control
#define ATTR_CONTAINER_X_SPACING        ATTR_COMMON_BASE+41//x-spacing specifies the horizontal spacing between the controls
#define ATTR_CONTAINER_Y_SPACING        ATTR_COMMON_BASE+42//y-spacing specifies the vertical spacing between the controls
#define ATTR_CONTAINER_INSET            ATTR_COMMON_BASE+43 //Comes when the Orientation/axis comes into picture - Similar to x and y inset. But it is based on the orientation.
#define ATTR_CONTAINER_SPACING          ATTR_COMMON_BASE+44 //Comes when the Orientation/axis comes into picture - Similar to x and y spacing. But it is based on the orientation.

#define ATTR_MULTILINE                  ATTR_COMMON_BASE+45//uint32
//Attribute for Stroke_color
#define ATTR_STROKE_COLOR              			 ATTR_COMMON_BASE+55//BOOLEAN
//Attribute for Identifying OrderedList Control
#define ATTR_ORDEREDLIST_CONTROL			 ATTR_COMMON_BASE+56//BOOLEAN


#define ATTR_CONTROL_SPECIFIC_BASE		400
// for the individual controls
// checkbox
#define ATTR_RESOURCECHECKED_GUID				    ATTR_CONTROL_SPECIFIC_BASE+1//char* or char16*
#define ATTR_RESOURCEUNCHECKED_GUID				    ATTR_CONTROL_SPECIFIC_BASE+2//char* or char16*
#define ATTR_RESOURCECHECKED_DISABLED_GUID		    ATTR_CONTROL_SPECIFIC_BASE+3//char* or char16*
#define ATTR_RESOURCEUNCHECKED_DISABLED_GUID		ATTR_CONTROL_SPECIFIC_BASE+4//char* or char16*
#define ATTR_RESOURCECHECKED_MOUSEOVER_GUID		    ATTR_CONTROL_SPECIFIC_BASE+5//char* or char16*
#define ATTR_RESOURCEUNCHECKED_MOUSEOVER_GUID	    ATTR_CONTROL_SPECIFIC_BASE+6//char* or char16*
#define ATTR_RESOURCECHECKED_FOCUS_GUID				ATTR_CONTROL_SPECIFIC_BASE+7//char* or char16*
#define ATTR_RESOURCEUNCHECKED_FOCUS_GUID			ATTR_CONTROL_SPECIFIC_BASE+8//char* or char16*

// combo
#define ATTR_RESOURCEEXPAND				ATTR_CONTROL_SPECIFIC_BASE+9//char* or char16*
#define ATTR_COMBOARROWHEIGHT			ATTR_CONTROL_SPECIFIC_BASE+10
#define ATTR_COMBOARROWIDTH			    ATTR_CONTROL_SPECIFIC_BASE+11
#define ATTR_LIST_VISIBLE_OPTIONS       ATTR_CONTROL_SPECIFIC_BASE+12//Attribute for overriding Max visible options in the list box
#define ATTR_DROP_DOWN_FOR_SINGLE_OPTION	ATTR_CONTROL_SPECIFIC_BASE+51

//dial
#define ATTR_RESOURCE_DIAL_PANEL_GUID    		    ATTR_CONTROL_SPECIFIC_BASE+13//char* or char16*
#define ATTR_RESOURCE_DIAL_HAND_GUID      		    ATTR_CONTROL_SPECIFIC_BASE+14//char* or char16*
#define ATTR_RESOURCE_DIAL_HAND_SHADOW_GUID    		ATTR_CONTROL_SPECIFIC_BASE+15//char* or char16*
#define ATTR_DIAL_START_ANGLE			            ATTR_CONTROL_SPECIFIC_BASE+16//uint32
#define ATTR_DIAL_END_ANGLE			                ATTR_CONTROL_SPECIFIC_BASE+17//uint32

//date and time
#define ATTR_RESOURCE_DATE_TIME_UP_ARROW_GUID              ATTR_CONTROL_SPECIFIC_BASE+18//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_UP_ARROW_DISABLED_GUID     ATTR_CONTROL_SPECIFIC_BASE+19//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_UP_ARROW_MOUSEOVER_GUID    ATTR_CONTROL_SPECIFIC_BASE+20//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_UP_ARROW_FOCUS_GUID        ATTR_CONTROL_SPECIFIC_BASE+21//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_DOWN_ARROW_GUID            ATTR_CONTROL_SPECIFIC_BASE+22//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_DOWN_ARROW_DISABLED_GUID   ATTR_CONTROL_SPECIFIC_BASE+23//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_DOWN_ARROW_MOUSEOVER_GUID  ATTR_CONTROL_SPECIFIC_BASE+24//char* or char16*
#define ATTR_RESOURCE_DATE_TIME_DOWN_ARROW_FOCUS_GUID      ATTR_CONTROL_SPECIFIC_BASE+25//char* or char16*
#define ATTR_DATE_TIME_ARROW_WIDTH                         ATTR_CONTROL_SPECIFIC_BASE+26
#define ATTR_DATE_TIME_ARROW_HEIGHT                        ATTR_CONTROL_SPECIFIC_BASE+27
#define ATTR_RESOURCE_DATE_TIME_OK_GUID			          ATTR_CONTROL_SPECIFIC_BASE+28
#define ATTR_RESOURCE_DATE_TIME_OK_DISABLED_GUID	          ATTR_CONTROL_SPECIFIC_BASE+29
#define ATTR_RESOURCE_DATE_TIME_OK_MOUSEOVER_GUID	      ATTR_CONTROL_SPECIFIC_BASE+30
#define ATTR_RESOURCE_DATE_TIME_OK_FOCUS_GUID	          ATTR_CONTROL_SPECIFIC_BASE+31
#define ATTR_RESOURCE_DATE_TIME_CANCEL_GUID			      ATTR_CONTROL_SPECIFIC_BASE+32
#define ATTR_RESOURCE_DATE_TIME_CANCEL_DISABLED_GUID	      ATTR_CONTROL_SPECIFIC_BASE+33
#define ATTR_RESOURCE_DATE_TIME_CANCEL_MOUSEOVER_GUID	  ATTR_CONTROL_SPECIFIC_BASE+34
#define ATTR_RESOURCE_DATE_TIME_CANCEL_FOCUS_GUID	      ATTR_CONTROL_SPECIFIC_BASE+35
#define ATTR_DATE_TIME_OK_CANCEL_WIDTH                     ATTR_CONTROL_SPECIFIC_BASE+36
#define ATTR_DATE_TIME_OK_CANCEL_HEIGHT                    ATTR_CONTROL_SPECIFIC_BASE+37

//Scrollbar Specific
#define ATTR_SCROLLBAR_UP_NORMAL_IMAGE_GUID			    ATTR_CONTROL_SPECIFIC_BASE+41//uint32
#define ATTR_SCROLLBAR_UP_MOUSEOVER_IMAGE_GUID			ATTR_CONTROL_SPECIFIC_BASE+42//uint32
#define ATTR_SCROLLBAR_DOWN_NORMAL_IMAGE_GUID			ATTR_CONTROL_SPECIFIC_BASE+43//uint32
#define ATTR_SCROLLBAR_DOWN_MOUSEOVER_IMAGE_GUID		ATTR_CONTROL_SPECIFIC_BASE+44//uint32
#define ATTR_SCROLLBAR_THUMB_NORMAL_IMAGE_GUID			ATTR_CONTROL_SPECIFIC_BASE+45//uint32
#define ATTR_SCROLLBAR_THUMB_MOUSEOVER_IMAGE_GUID		ATTR_CONTROL_SPECIFIC_BASE+46//uint32
#define ATTR_SCROLLBAR_STRIP_NORMAL_IMAGE_GUID			ATTR_CONTROL_SPECIFIC_BASE+47//uint32
#define ATTR_SCROLLBAR			    			        ATTR_CONTROL_SPECIFIC_BASE+48//uint32
#define ATTR_CYCLICSCROLLBAR			    		    ATTR_CONTROL_SPECIFIC_BASE+49//uint32
#define ATTR_SCROLLBAR_WIDTH		         			ATTR_CONTROL_SPECIFIC_BASE+50//uint32




typedef
EFI_STATUS
(EFIAPI *FRONT_PAGE_CALLBACK) (
    VOID *
);

typedef UINT16 EFI_STRING_ID;

typedef union
{
    EFI_GUID        GUID;
    UINT8		u8;
    UINT16	u16;
    UINT32	u32;
    UINT64	 u64;
    INT8		i8;
    INT16          i16;
    INT32          i32;
    INT64          i64;
    BOOLEAN         b;
    EFI_STRING_ID   string;
} ATTR_VALUE;

typedef struct _FRONT_PAGE_CONTROL
{
    UINT16					StringID;			//Flags decides the property
    UINT32					MsgBoxCallbackIndex;
    FRONT_PAGE_CALLBACK		Callback;
} FRONT_PAGE_CONTROL;

typedef struct _CONTROL_HANDLE
{
    UINT32		CtrlHiiType;					// Decide Hii or non Hii contro
    UINT32		ViewType;
    void*		CtrlSpecificData;
    void*		CtrlHandle;				// Ptr to SDA control - multiple nodes can point to the same SDA control (ex, title, defaults button on every page)
    UINT8		ControlState;
    void*       ControlValue;
    UINT32      Size;
    UINT8  		 Interval;
    UINT32 		 CustomDataID;
    EFI_GUID	CustomDataGUID;
} CONTROL_HANDLE;

typedef struct _GUI_MAP_ATTR GUI_MAP_ATTR;

struct _GUI_MAP_ATTR
{
    UINT32				GUICtrlDisplayID;
    UINT32			Attr_ID;
    ATTR_VALUE		Value;
};

struct GUI_ATTR_TABLE
{
    UINT32			Attr_ID;
    ATTR_VALUE		Value;
};


struct GUI_ATTR_WRAP
{
    UINT32				GUICtrlDisplayID;
    GUI_ATTR_TABLE		*Attrb;
};


typedef struct _IFR_MSGBOX
{
    // this structure is, in some way, following EFI_IFR structures
    UINT8	Opcode, Length;
    UINT16	Title;
    /*EFI_HII_HANDLE*/VOID * 	TextHandle;
    UINT16	Text;
} AMI_IFR_MSGBOX;

typedef struct _TEXT_CONTROL
{
    UINT16	StringID;
    CHAR16	*TextString;
} TEXT_CONTROL;

typedef struct _PTRTOKENS
{
    UINT16 Option;
    UINT64 Value;
} PTRTOKENS;

typedef struct _COMBOBOXOPTIONS
{
    UINT16 OptionCount;
    PTRTOKENS *PtrTokens;
} COMBOBOXOPTIONS;

typedef enum
{
    left = 0,
    center,
    right,
    constraint

} EAlign;

typedef enum
{
    top = 0,
    middle,
    bottom,
    v_constraint

} EVAlign;
//Color defines
#define COLOR_WHITE        0xffffffff
#define COLOR_DARKGRAY     0xff404040
#define COLOR_ORANGE       0xfff08040
#define COLOR_DARKORANGE   0xffd06515
#define COLOR_LIGHTGRAY    0xffa7a7a7
#define COLOR_LIGHTBLUE    0xff00a2e8
#define COLOR_DARKBLUE     0xff007aae
#define COLOR_UNKNOWN      0xff004040
#define COLOR_SHADOW       0xfff2f2f2
#define COLOR_SKY_BLUE     0xffbbebff
#define COLOR_DARKBLACK    0xff000000
#define COLOR_SMOOTH_BLACK 0xff222222

#ifndef OPENSANS_BOLD_WEBFONT_GUID
#define OPENSANS_BOLD_WEBFONT_GUID \
{0x10131007, 0x9edd, 0x469f, 0xa0, 0x7a, 0x6d, 0x87, 0x5c, 0x1f, 0xf8, 0x5f}
#endif

#define Non_Hii_View_Type_Button               	0x10040
#define Non_Hii_View_Type_Text               	0x10041
#define Non_Hii_View_Type_Combo_box		    	0x4000
#define Non_Hii_View_Type_Check_box				0x4001
#define Non_Hii_View_Type_List_box		    	0x4002
#define Non_Hii_View_Type_Edit_Box			0x40003
#define Non_Hii_View_Type_Time_Control				0x4004
#define Non_Hii_View_Type_Date_Control				0x4005
#define Non_Hii_View_Type_ProgressBar				0x4006
#define NON_HII_VIEW_TYPE_ORDEREDLIST_CONTAINER		0x4007
#define Non_Hii_View_Type_Button               		0x10040
#define Non_Hii_View_Type_Text               		0x10041

#ifndef INVALID_HANDLE
#define	INVALID_HANDLE	((VOID*)(UINTN)-1)
#endif

typedef struct _DS_STRING {
	UINT32	DSType;
	UINT16	MinChar;
	UINT16	MaxChar;  // if 0,0 for min/max -> no limit
}DS_STRING;

typedef struct _DS_NUM_8 {
	UINT32	DSType;
	INT8	MinValue;
	INT8	MaxValue;
	INT8	Step;
	UINT8	Display; //(dec, hex)
}DS_NUM_8;
typedef struct _DS_NUM_16 {
	UINT32	DSType;
	INT16	MinValue;
	INT16	MaxValue;
	INT8	Step;
	UINT8	Display; //(dec, hex)
}DS_NUM_16;
typedef struct _DS_NUM_32 {
	UINT32	DSType;
	INT32	MinValue;
	INT32	MaxValue;
	INT16	Step;
	UINT8	Display; //(dec, hex)
}DS_NUM_32;
typedef struct _DS_NUM_64 {
	UINT32	DSType;
	INT64	MinValue;
	INT64	MaxValue;
	INT32	Step;
	UINT8	Display; //(dec, hex)
}DS_NUM_64;
typedef struct _DS_UNUM_8 {
	UINT32	DSType;
	UINT8	MinValue;
	UINT8	MaxValue;
	UINT8	Step;
	UINT8	Display; //(dec, hex)
}DS_UNUM_8;
typedef struct _DS_UNUM_16 {
	UINT32	DSType;
	UINT16	MinValue;
	UINT16	MaxValue;
	UINT8	Step;
	UINT8	Display; //(dec, hex)
}DS_UNUM_16;
typedef struct _DS_UNUM_32 {
	UINT32	DSType;
	UINT32	MinValue;
	UINT32	MaxValue;
	UINT16	Step;
	UINT8	Display; //(dec, hex)
}DS_UNUM_32;
typedef struct _DS_UNUM_64 {
	UINT32	DSType;
	UINT64	MinValue;
	UINT64	MaxValue;
	UINT32	Step;
	UINT8	Display; //(dec, hex)
}DS_UNUM_64;

typedef struct _DS_OPTION {
	UINT32	DSType; //GUI_DATA_SRC_TYPE_OPTION
	UINT8	State;
    UINT32  ValueType; //GUI_VALUE_TYPE_STRING or GUI_DATA_SRC_TYPE_NUM_ 
	void*   Value;
}DS_OPTION;

typedef struct _DS_OPTION_LIST {
	UINT32		DSType; //GUI_DATA_SRC_TYPE_OPTION_LIST
	UINTN		NumOptions;
	DS_OPTION*	Option;
}DS_OPTION_LIST;


#define MIN_CHAR_STRING_TYPE			0
#define MAX_CHAR_STRING_TYPE			21

#define ACTION_VALIDATE     0x10001
#define ACTION_INVALID_VALUE     0x10002

#define ATTR_NAVIGATION_ACTION_UP					ATTR_COMMON_BASE+57//BOOLEAN
#define ATTR_NAVIGATION_ACTION_UP_FIRST				ATTR_COMMON_BASE+58//BOOLEAN
#define ATTR_NAVIGATION_ACTION_DOWN					ATTR_COMMON_BASE+59//BOOLEAN
#define ATTR_NAVIGATION_ACTION_DOWN_LAST			ATTR_COMMON_BASE+60//BOOLEAN
#define ATTR_NAVIGATION_ACTION_LEFT					ATTR_COMMON_BASE+61//BOOLEAN
#define ATTR_NAVIGATION_ACTION_LEFT_FIRST			ATTR_COMMON_BASE+62//BOOLEAN
#define ATTR_NAVIGATION_ACTION_RIGHT				ATTR_COMMON_BASE+63//BOOLEAN
#define ATTR_NAVIGATION_ACTION_RIGHT_LAST			ATTR_COMMON_BASE+64//BOOLEAN
#define ATTR_NAVIGATION_ACTION_TAB					ATTR_COMMON_BASE+65//BOOLEAN
#define ATTR_NAVIGATION_ACTION_SHIFT_TAB			ATTR_COMMON_BASE+66//BOOLEAN
#define ATTR_NAVIGATION_ACTION_HOME					ATTR_COMMON_BASE+67//BOOLEAN
#define ATTR_NAVIGATION_ACTION_END					ATTR_COMMON_BASE+68//BOOLEAN
#define ATTR_NAVIGATION_ACTION_PAGE_UP				ATTR_COMMON_BASE+69//BOOLEAN
#define ATTR_NAVIGATION_ACTION_PAGE_DOWN			ATTR_COMMON_BASE+70//BOOLEAN
#define ATTR_NAVIGATION_INITIAL_FOCUS				ATTR_COMMON_BASE+71//BOOLEAN

#define FOCUS_NO_ACTION 	0
#define FOCUS_PREV 			1
#define FOCUS_NEXT 			2
#define FOCUS_FIRST			3
#define FOCUS_LAST 			4
#define FOCUS_SAME 			7
#define FOCUS_LAST_FOCUSED 	8

#define Hii_View_Type_Value	                    0x10002
#endif //end of file



