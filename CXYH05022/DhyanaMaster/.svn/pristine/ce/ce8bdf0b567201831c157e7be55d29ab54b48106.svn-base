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
#include <AmiDxeLib.h>
extern "C"
{
#include "wchar.h"
#include "commonoem.h"
extern CHAR16 *HiiGetString( VOID* handle, UINT16 token );
void *memcpy (void* dest, void* src, int size);
static CHAR16* gEValue ;
extern UINTN	Wcslen(CHAR16 *Str);
extern CHAR16* Wcscpy(CHAR16 *,CHAR16 *);
UINTN StrLen ( CONST CHAR16              *String  );
CHAR16 *StrCpy (CHAR16 *Destination,CONST CHAR16  *Source);
VOID * AllocateZeroPool (  IN UINTN  AllocationSize  );
extern VOID* gHiiHandle;
}
GUI_CTRL_INFO_NODE *gCurrentPage = (GUI_CTRL_INFO_NODE *)NULL ;
extern EFI_HANDLE gImageHandle;
void AppPageUpdateforRefreshControls(GUI_CTRL_INFO_NODE *TreeNode);
void Delay();
void  *gCurFocusHandle = NULL;
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    NonHiiGetValue
//
// Description:   
//
// Parameter:     	
//			IN ESA_CONTROL_DATA_SOURCE_PROTOCOL *This,
//			IN UINT32 ControlID,
//			IN UINT8 Type,
//			IN OUT void** Value,
//			IN OUT UINT32* ByteLength
//
// Return Value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS NonHiiGetValue(
		IN ESA_DATA_SOURCE_PROTOCOL *This,
		IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT void** Value,
        IN OUT UINT32* ByteLength)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	CONTROL_HANDLE  *ControlData = (CONTROL_HANDLE  *)ControlHandle;	
	switch(ControlData->ViewType)
	{
		case Non_Hii_View_Type_Text:
			if(0 != ((TEXT_CONTROL *)(ControlData->CtrlHandle))->StringID)
			{
				*Value = HiiGetString (gHiiHandle, ( (FRONT_PAGE_CONTROL *)(ControlData->CtrlHandle))->StringID);	
				*ByteLength = (wcslen(((wchar_t *)*Value))+ 1) * sizeof(CHAR16) ;
			}
			else
			{
				CHAR16 *Str = ((TEXT_CONTROL *)(ControlData->CtrlHandle))->TextString;
				*Value = (CHAR16*)AllocateZeroPool((wcslen((wchar_t *)Str)+ 1) * sizeof(CHAR16));
				wcscpy((wchar_t *)*Value, (wchar_t *)Str);
				*ByteLength = (wcslen(((wchar_t *)*Value))+ 1) * sizeof(CHAR16) ;
			}
	
			break;
		case Non_Hii_View_Type_Button :	
			*Value = HiiGetString (gHiiHandle, ( (FRONT_PAGE_CONTROL *)(ControlData->CtrlHandle))->StringID);	
			*ByteLength = (wcslen((wchar_t *)*Value)+ 1) * sizeof(CHAR16) ;
			break;
		case Non_Hii_View_Type_Time_Control :	
		case Non_Hii_View_Type_Date_Control :
				EFI_TIME *Tm;	
				Tm = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
				Status = pRS->GetTime( Tm, (EFI_TIME_CAPABILITIES *)NULL );
				
				if(Status == 0)
				{
					 *Value = Tm;
					 *ByteLength = sizeof(EFI_TIME) ;
				}
			break;
		case Non_Hii_View_Type_ProgressBar:
			*Value = (UINTN *)AllocateZeroPool( sizeof(UINTN));
			
			if(!((CONTROL_HANDLE*)ControlData)->ControlValue)
				((CONTROL_HANDLE*)ControlData)->ControlValue= (UINTN *)AllocateZeroPool( sizeof(UINTN));
			
			if(((CONTROL_HANDLE*)ControlData)->ControlValue)
			{
				memcpy(*Value, ((CONTROL_HANDLE*)ControlData)->ControlValue, sizeof(UINT32) );
			}
			break;
		case Non_Hii_View_Type_Combo_box:
			{
				COMBOBOXOPTIONS *CBOptions = (COMBOBOXOPTIONS *)((CONTROL_HANDLE*)ControlData)->CtrlSpecificData;
				*Value = (UINTN *)AllocateZeroPool( sizeof(UINTN));
				
				if(!((CONTROL_HANDLE*)ControlData)->ControlValue)
					((CONTROL_HANDLE*)ControlData)->ControlValue= (UINTN *)AllocateZeroPool( sizeof(UINTN));
				
				if(((CONTROL_HANDLE*)ControlData)->ControlValue)
				{
					memcpy(*Value, ((CONTROL_HANDLE*)ControlData)->ControlValue, sizeof(UINTN) );
					*ByteLength = sizeof(UINTN);
				}
			}
			break;
		case Non_Hii_View_Type_List_box:
		{
			*Value = (UINTN *)AllocateZeroPool( sizeof(UINTN));
			COMBOBOXOPTIONS *CBOptions = (COMBOBOXOPTIONS *)NULL;
			CBOptions = (COMBOBOXOPTIONS *)((CONTROL_HANDLE*)ControlData)->CtrlSpecificData;
			if(((CONTROL_HANDLE*)ControlData)->ControlValue)
			{
				memcpy(*Value, ((CONTROL_HANDLE*)ControlData)->ControlValue, sizeof(UINT16) );
				*ByteLength = sizeof(UINTN);
			}
		}
			break;
		case Non_Hii_View_Type_Check_box :
			*Value = (UINT16 *)AllocateZeroPool(sizeof(UINT16)) ;
			if((ControlData->ControlValue) == NULL)
				ControlData->ControlValue = (UINT16 *)AllocateZeroPool(sizeof(UINT16)) ;
			memcpy(*Value ,(UINT16 *)ControlData->ControlValue , (sizeof(UINT16))) ;
			break ;
		case Non_Hii_View_Type_Edit_Box :
		if(ControlData->ControlValue || ControlData->Size)
		{
			if( (ControlData->Size == GUI_VALUE_TYPE_UNUM_8) || ( ControlData->Size == GUI_VALUE_TYPE_UNUM_16) || (ControlData->Size == GUI_VALUE_TYPE_UNUM_32) || (ControlData->Size == GUI_VALUE_TYPE_UNUM_64) || (ControlData->Size == GUI_VALUE_TYPE_NUM_8) || (ControlData->Size ==GUI_VALUE_TYPE_NUM_16) || (ControlData->Size == GUI_VALUE_TYPE_NUM_32) || (ControlData->Size == GUI_VALUE_TYPE_NUM_64) )
			{
				*Value = (UINT64 *)AllocateZeroPool(sizeof(UINT64));
				if(ControlData->ControlValue)
				{
					memcpy( *Value , ControlData->ControlValue, sizeof(UINT64) );
				}
				else
				{
					ControlData->ControlValue = (UINT64 *)AllocateZeroPool(sizeof(UINT64));
					memcpy( *Value , ControlData->ControlValue, sizeof(UINT64) );
				}
			}
			else
			{
				DS_STRING *EditBox_DS = (DS_STRING *)NULL;
				EditBox_DS = (DS_STRING *)ControlData->CtrlSpecificData;
				if(EditBox_DS != NULL)
				{
					*Value = (CHAR16 *)AllocateZeroPool ((EditBox_DS->MaxChar + 1)* sizeof(CHAR16)) ;
					memcpy( *Value , ControlData->ControlValue, EditBox_DS->MaxChar *sizeof(CHAR16) );
				}
				else
				{
					*Value = (CHAR16 *)AllocateZeroPool (MAX_CHAR_STRING_TYPE * sizeof(CHAR16)) ;
					memcpy( *Value , ControlData->ControlValue, MAX_CHAR_STRING_TYPE * sizeof(CHAR16) );
				}
			}

		}	
		else
		{
			DS_STRING *EditBox_DS = (DS_STRING *)NULL;
			EditBox_DS = (DS_STRING *)ControlData->CtrlSpecificData;

			if(EditBox_DS != NULL)
			{
				*Value = (CHAR16 *)AllocateZeroPool ((EditBox_DS->MaxChar + 1) * sizeof(CHAR16)) ;
				//gEValue = (CHAR16 *)AllocateZeroPool ((EditBox_DS->MaxChar + 1) * sizeof(CHAR16)) ;
				if((ControlData->ControlValue) == NULL) {
					ControlData->ControlValue = (UINT16 *)AllocateZeroPool((EditBox_DS->MaxChar + 1) * sizeof(CHAR16)) ;
				}
			}
			else
			{
				*Value = (CHAR16 *)AllocateZeroPool ((MAX_CHAR_STRING_TYPE + 1) * sizeof(CHAR16)) ;
				gEValue = (CHAR16 *)AllocateZeroPool ((MAX_CHAR_STRING_TYPE + 1) * sizeof(CHAR16)) ;
				Wcscpy((CHAR16*)*Value,gEValue);
				if((ControlData->ControlValue) == NULL) {
					ControlData->ControlValue = (UINT16 *)AllocateZeroPool(MAX_CHAR_STRING_TYPE * sizeof(CHAR16)) ;
				}
			}
			}
			break ;
		case NON_HII_VIEW_TYPE_ORDEREDLIST_CONTAINER:
			{
				UINT64 *CurrOrder= (UINT64 *)NULL;
				UINTN index = 0;
				COMBOBOXOPTIONS *CBOptions = (COMBOBOXOPTIONS *)((CONTROL_HANDLE*)ControlData)->CtrlSpecificData;
				
				CurrOrder = (UINT64 *)AllocateZeroPool (CBOptions->OptionCount * sizeof (UINT64));
				//Store the Current Order in the Array CurrOrder
				for (index =0 ; index < CBOptions->OptionCount; index++)
				{
					if (CBOptions->PtrTokens){
						//CurrOrder[index] = CBOptions->PtrTokens[index].Value;
						CurrOrder[index] = CBOptions->PtrTokens[index].Option;
					}
				}
				*Value = (void *)CurrOrder;
			}
			break;
		default:
			break;
	}
  return Status;
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    NonHiiGetState
//
// Description:   
//
// Parameter:     	
//		IN ESA_CONTROL_DATA_SOURCE_PROTOCOL * This,
//		IN UINT32 ControlID,
//		IN OUT UINT8* State
//
// Return Value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NonHiiGetState(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT UINT8* State)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	CONTROL_HANDLE  *ControlData = (CONTROL_HANDLE  *)ControlHandle;
	
	if ((ControlData->CtrlHiiType == NonHiiControl))
	{
		*State = ControlData->ControlState;
	}
  return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    NonHiiGetFocusable
//
// Description:   
//
// Parameter:     	
//		IN ESA_CONTROL_DATA_SOURCE_PROTOCOL * This,
//		IN UINT32 ControlID,
//		IN OUT BOOLEAN* Focusable
//
// Return Value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS NonHiiGetFocusable(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN OUT BOOLEAN* Focusable)
{
	EFI_STATUS	Status = EFI_SUCCESS;
	CONTROL_HANDLE  *ControlData = (CONTROL_HANDLE  *)ControlHandle;
	
	*Focusable = 0;
	if(ControlData->CtrlHiiType == NonHiiControl)
	{
		switch(ControlID)
		{  
			case Non_Hii_View_Type_Text: 
					*Focusable = 0;
				break;
			case Non_Hii_View_Type_Button:  
			case Non_Hii_View_Type_Combo_box:
			case Non_Hii_View_Type_Check_box:
			case Non_Hii_View_Type_Edit_Box:
			case Non_Hii_View_Type_Time_Control:
			case Non_Hii_View_Type_Date_Control:
			case Non_Hii_View_Type_List_box:
			case NON_HII_VIEW_TYPE_ORDEREDLIST_CONTAINER:
				*Focusable = 1;
				break;
			default:        	    
				break;
		}
	}
  return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    NonHiiGetDataSrc
//
// Description:   
//
// Parameter:     	
//		IN ESA_CONTROL_DATA_SOURCE_PROTOCOL * This,
//		IN UINT32 ControlID,	
//		IN UINT8  Type,
//		IN OUT void** DataSrcStruct
//
// Return Value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>


EFI_STATUS NonHiiGetDataSrc(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,	
        IN OUT void** DataSrcStruct )
{
	EFI_STATUS	Status = EFI_SUCCESS;
	CONTROL_HANDLE  *ControlData = (CONTROL_HANDLE  *)ControlHandle;
	
	if(ControlData->ViewType == Non_Hii_View_Type_Combo_box || (ControlData->ViewType == Non_Hii_View_Type_List_box))
	{
		UINT32 OptionCount = 0;
		UINT32 Index = 0;
		DS_OPTION * DataSrcStructOptionList = (DS_OPTION*)NULL;
		COMBOBOXOPTIONS* CBOptions = (COMBOBOXOPTIONS *)ControlData->CtrlSpecificData;

		if(CBOptions)
		{
			*DataSrcStruct = AllocateZeroPool(sizeof(DS_OPTION_LIST));
			((DS_OPTION_LIST*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_OPTION_LIST;
			((DS_OPTION_LIST*)(*DataSrcStruct))->NumOptions = CBOptions->OptionCount;
			DataSrcStructOptionList = (DS_OPTION*) AllocateZeroPool( CBOptions->OptionCount * sizeof(DS_OPTION));
		
			for( Index = 0; Index <CBOptions->OptionCount; Index++ )
			{
				CHAR16 *text=( CHAR16 *)NULL;
				DataSrcStructOptionList[Index].DSType = GUI_DATA_SRC_TYPE_OPTION;
				DataSrcStructOptionList[Index].State = 0;
				DataSrcStructOptionList[Index].ValueType = GUI_VALUE_TYPE_STRING;
				
				if(CBOptions->PtrTokens)
				{
					if(CBOptions->PtrTokens[Index].Option != 0)
						text = HiiGetString(gHiiHandle, (UINT16)CBOptions->PtrTokens[Index].Option);
					else
					{
						// Allocate memory for static Non-HII strings
						if(CBOptions->PtrTokens[Index].Value)
						{
							text = (CHAR16 *) AllocateZeroPool((StrLen((CHAR16 *) CBOptions->PtrTokens[Index].Value) + 1) * sizeof(CHAR16));
							if(text)
								StrCpy((CHAR16 *)text,(CHAR16 *) CBOptions->PtrTokens[Index].Value);
						}
						else
							text = (CHAR16*)NULL;
					}
				}
				else
					text = (CHAR16*)NULL;
				DataSrcStructOptionList[Index].Value = text;
			}
			((DS_OPTION_LIST*)(*DataSrcStruct))->Option = DataSrcStructOptionList;
		}
	}
	else if( ControlData->ViewType == Non_Hii_View_Type_Edit_Box )
		{
	                UINT32 CtrlDataSrcType = 0;
						CtrlDataSrcType = ((DS_STRING *)(ControlData->CtrlSpecificData))->DSType ;

	                switch(CtrlDataSrcType)
	                {
	                    case GUI_DATA_SRC_TYPE_STRING:
	                        {
	                            DS_STRING *EditBox_DS = (DS_STRING *)NULL;
	                            EditBox_DS = (DS_STRING *)ControlData->CtrlSpecificData;
					            *DataSrcStruct = AllocateZeroPool(sizeof(DS_STRING));
					            ((DS_STRING*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_STRING;
					            if(EditBox_DS != NULL)
								{								
									((DS_STRING*)(*DataSrcStruct))->MinChar = EditBox_DS->MinChar;
									((DS_STRING*)(*DataSrcStruct))->MaxChar = EditBox_DS->MaxChar;
					            }
					            else
								{
									((DS_STRING*)(*DataSrcStruct))->MinChar = MIN_CHAR_STRING_TYPE;
									((DS_STRING*)(*DataSrcStruct))->MaxChar = MAX_CHAR_STRING_TYPE;
					            }
	                        }
	                        break;
	                    case GUI_DATA_SRC_TYPE_NUM_8:
	                        {
	                            DS_NUM_8 *EditBox_DS = (DS_NUM_8 *)NULL;
	                            EditBox_DS = (DS_NUM_8 *)ControlData->CtrlSpecificData;
					            *DataSrcStruct = AllocateZeroPool(sizeof(DS_NUM_8));
					            ((DS_NUM_8*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_8;
					            if(EditBox_DS != NULL){
						            ((DS_NUM_8*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
						            ((DS_NUM_8*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
						            ((DS_NUM_8*)(*DataSrcStruct))->Display = EditBox_DS->Display;
					            }
					            else{
						            ((DS_NUM_8*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
						            ((DS_NUM_8*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
					            }
					            ((DS_NUM_8*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_INT_DEC;
	                        }
	                        break;
	                    case GUI_DATA_SRC_TYPE_UNUM_8:
							{
								DS_UNUM_8 *EditBox_DS = (DS_UNUM_8 *)NULL;
								EditBox_DS = (DS_UNUM_8 *)ControlData->CtrlSpecificData;
								*DataSrcStruct = AllocateZeroPool(sizeof(DS_UNUM_8));
								((DS_UNUM_8*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_8;
								if(EditBox_DS != NULL){
									((DS_UNUM_8*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
									((DS_UNUM_8*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
									((DS_UNUM_8*)(*DataSrcStruct))->Display = EditBox_DS->Display;
								}
								else{
									((DS_UNUM_8*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
									((DS_UNUM_8*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
								}
								((DS_UNUM_8*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_UINT_DEC;
							}
	                        break;
	                    case GUI_DATA_SRC_TYPE_NUM_16:
	                        {
	                            DS_NUM_16 *EditBox_DS = (DS_NUM_16 *)NULL;
	                            EditBox_DS = (DS_NUM_16 *)ControlData->CtrlSpecificData;
					            *DataSrcStruct = AllocateZeroPool(sizeof(DS_NUM_16));
					            ((DS_NUM_16*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_16;
					            if(EditBox_DS != NULL){
						            ((DS_NUM_16*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
						            ((DS_NUM_16*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
						            ((DS_NUM_16*)(*DataSrcStruct))->Display = EditBox_DS->Display;
					            }
					            else{
						            ((DS_NUM_16*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
						            ((DS_NUM_16*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
					            }
					            ((DS_NUM_16*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_INT_DEC;
	                        }
	                        break;
	                    case GUI_DATA_SRC_TYPE_UNUM_16:
							{
								DS_UNUM_16 *EditBox_DS = (DS_UNUM_16 *)NULL;
								EditBox_DS = (DS_UNUM_16 *)ControlData->CtrlSpecificData;
								*DataSrcStruct = AllocateZeroPool(sizeof(DS_UNUM_16));
								((DS_UNUM_16*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_16;
								if(EditBox_DS != NULL){
									((DS_UNUM_16*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
									((DS_UNUM_16*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
									((DS_UNUM_16*)(*DataSrcStruct))->Display = EditBox_DS->Display;
								}
								else{
									((DS_UNUM_16*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
									((DS_UNUM_16*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
								}
								((DS_UNUM_16*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_UINT_DEC;
							}
	                        break;
	                    case GUI_DATA_SRC_TYPE_NUM_32:
	                        {
	                            DS_NUM_32 *EditBox_DS = (DS_NUM_32 *)NULL;
	                            EditBox_DS = (DS_NUM_32 *)ControlData->CtrlSpecificData;
					            *DataSrcStruct = AllocateZeroPool(sizeof(DS_NUM_32));
					            ((DS_NUM_32*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_32;
					            if(EditBox_DS != NULL){
						            ((DS_NUM_32*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
						            ((DS_NUM_32*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
						            ((DS_NUM_32*)(*DataSrcStruct))->Display = EditBox_DS->Display;
					            }
					            else{
						            ((DS_NUM_32*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
						            ((DS_NUM_32*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
					            }
					            ((DS_NUM_32*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_INT_DEC;
	                        }
	                        break;
	                    case GUI_DATA_SRC_TYPE_UNUM_32:
							{
								DS_UNUM_32 *EditBox_DS = (DS_UNUM_32 *)NULL;
								EditBox_DS = (DS_UNUM_32 *)ControlData->CtrlSpecificData;
								*DataSrcStruct = AllocateZeroPool(sizeof(DS_UNUM_32));
								((DS_UNUM_32*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_32;
								if(EditBox_DS != NULL){
									((DS_UNUM_32*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
									((DS_UNUM_32*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
									((DS_UNUM_32*)(*DataSrcStruct))->Display = EditBox_DS->Display;
								}
								else{
									((DS_UNUM_32*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
									((DS_UNUM_32*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
								}
								((DS_UNUM_32*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_UINT_DEC;
							}
	                        break;
	                    case GUI_DATA_SRC_TYPE_NUM_64:
	                        {
	                            DS_NUM_64 *EditBox_DS = (DS_NUM_64 *)NULL;
	                            EditBox_DS = (DS_NUM_64 *)ControlData->CtrlSpecificData;
					            *DataSrcStruct = AllocateZeroPool(sizeof(DS_NUM_64));
					            ((DS_NUM_64*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_NUM_64;
					            if(EditBox_DS != NULL){
						            ((DS_NUM_64*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
						            ((DS_NUM_64*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
						            ((DS_NUM_64*)(*DataSrcStruct))->Display = EditBox_DS->Display;
					            }
					            else{
						            ((DS_NUM_64*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
						            ((DS_NUM_64*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
					            }
					            ((DS_NUM_32*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_INT_DEC;
	                        }
	                        break;
	                    case GUI_DATA_SRC_TYPE_UNUM_64:
	                        {
	                            DS_UNUM_64 *EditBox_DS = (DS_UNUM_64 *)NULL;
	                            EditBox_DS = (DS_UNUM_64 *)ControlData->CtrlSpecificData;
					            *DataSrcStruct = AllocateZeroPool(sizeof(DS_UNUM_64));
					            ((DS_UNUM_64*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_UNUM_64;
					            if(EditBox_DS != NULL){
						            ((DS_UNUM_64*)(*DataSrcStruct))->MinValue = EditBox_DS->MinValue;
						            ((DS_UNUM_64*)(*DataSrcStruct))->MaxValue = EditBox_DS->MaxValue;
						            ((DS_UNUM_64*)(*DataSrcStruct))->Display = EditBox_DS->Display;
					            }
					            else{
						            ((DS_UNUM_64*)(*DataSrcStruct))->MinValue = MIN_CHAR_STRING_TYPE;
						            ((DS_UNUM_64*)(*DataSrcStruct))->MaxValue = MAX_CHAR_STRING_TYPE;
					            }
					            ((DS_UNUM_64*)(*DataSrcStruct))->Display = EFI_IFR_DISPLAY_UINT_DEC;
	                        }
	                        break;
	                }

		}
	else if(ControlData->ViewType == NON_HII_VIEW_TYPE_ORDEREDLIST_CONTAINER ){
		UINTN Index=0;
		COMBOBOXOPTIONS *CBOptions = (COMBOBOXOPTIONS *)NULL;
							
		/*PTRTOKENS *PtrTokens;*/
		DS_OPTION * DataSrcStructOptionList = (DS_OPTION*)NULL;
		*DataSrcStruct = AllocateZeroPool(sizeof(DS_OPTION_LIST));
		 
		CBOptions = (COMBOBOXOPTIONS *)ControlData->CtrlSpecificData;

		((DS_OPTION_LIST*)(*DataSrcStruct))->DSType = GUI_DATA_SRC_TYPE_OPTION_LIST;
		((DS_OPTION_LIST*)(*DataSrcStruct))->NumOptions = CBOptions->OptionCount;
		
		DataSrcStructOptionList = (DS_OPTION*) AllocateZeroPool( CBOptions->OptionCount * sizeof(DS_OPTION));
		
		for( Index = 0; Index <CBOptions->OptionCount; Index++ )
		{
			CHAR16 *text=( CHAR16 *)NULL;
			DataSrcStructOptionList[Index].DSType = GUI_DATA_SRC_TYPE_OPTION;
			DataSrcStructOptionList[Index].State = 0;
			DataSrcStructOptionList[Index].ValueType =GUI_VALUE_TYPE_STRING;
			
			if(CBOptions->PtrTokens)
			{
				text = (CHAR16 *) AllocateZeroPool((StrLen((CHAR16 *) CBOptions->PtrTokens[Index].Value) + 1) * sizeof(CHAR16));
				if(text)
					StrCpy((CHAR16 *)text,(CHAR16 *) CBOptions->PtrTokens[Index].Value);
				else{
					text = (CHAR16 *)NULL;
				}
			}
			DataSrcStructOptionList[Index].Value = text;
		}
		((DS_OPTION_LIST*)(*DataSrcStruct))->Option = DataSrcStructOptionList;
	}
  return Status;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:    NonHiiControlCallback
//
// Description:   
//
// Parameter:     	
//		IN ESA_CONTROL_DATA_SOURCE_PROTOCOL * This,
//		IN void* ControlHandle,
//		IN UINT32 ControlID,
//		IN void* Action, 
//		IN UINT8  Type,
//		IN OUT void *Value
//
// Return Value:  EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>


EFI_STATUS	NonHiiControlCallback(
        IN ESA_DATA_SOURCE_PROTOCOL * This,
        IN void* ControlHandle,
        IN UINT32 ControlID,
        IN void* Action, 
        IN OUT void *Value )
{
	EFI_STATUS	Status = EFI_SUCCESS;
	int action = *(int*)Action;
	//action = ControlActionSelect ;

	switch (action)
	{
/*	    case GUI_ACTION_LCLICK:
	    	Status = EFI_UNSUPPORTED;
	        break;
	    case ControlActionTimer :
	        SetupRefreshControl();
	        break;*/
	    case ACTION_VALIDATE:
	    case ACTION_INVALID_VALUE:
	        return EFI_SUCCESS;        
	        break;
	    case ControlActionTimer :
			if(ControlHandle != INVALID_HANDLE)
			{
				GetCurrentPage( &gCurrentPage);
				AppPageUpdateforRefreshControls(gCurrentPage) ;
			}
	        break;
	    case ControlActionSelect : 
		if(ControlHandle != INVALID_HANDLE)
		{
			if( (((CONTROL_HANDLE*) ControlHandle)->CtrlHiiType ) == NonHiiControl )
			{
				GUI_CTRL_INFO_NODE *tCtrlnode = (GUI_CTRL_INFO_NODE *)NULL ;
					GetCurrentPage(&gCurrentPage);
				if( (((CONTROL_HANDLE*) ControlHandle)->ViewType ) == Non_Hii_View_Type_Check_box )
				{
				//gCheckValue = *((UINT8 *)(((CONTROL_HANDLE*) ControlHandle)->ControlValue)) ;
				memcpy(((CONTROL_HANDLE*) ControlHandle)->ControlValue ,Value , (sizeof(UINT16))) ;
				
				if( NULL != ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle ) ){
					if( NULL != ((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback ){
						((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback(ControlHandle);
					}else{
						IdentifyCtrlNodeByHandle( ((CONTROL_HANDLE*)ControlHandle),gCurrentPage,&tCtrlnode);
						if(tCtrlnode)
							tCtrlnode->InvalidateFlag = INVALIDATE_VALUE;
					}
				}else{
					IdentifyCtrlNodeByHandle(((CONTROL_HANDLE*)ControlHandle),gCurrentPage,&tCtrlnode);
				       if(tCtrlnode)
					   tCtrlnode->InvalidateFlag = INVALIDATE_VALUE;
				}
				//ShowPage(gCurrentPage, NULL);
				GetCurrentPage(&gCurrentPage);
				//gCurFocusHandle is used by the library user to set the focus after the action event
				if(NULL != gCurFocusHandle)
					ShowPage(gCurrentPage, gCurFocusHandle);
				else
					ShowPage(gCurrentPage, ControlHandle);
				gCurFocusHandle = NULL;
					
				}
				else if( (((CONTROL_HANDLE*) ControlHandle)->ViewType ) == Non_Hii_View_Type_Button)
				{
					    if (NULL != ((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback)
					    {
						((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback(ControlHandle);								
					}
				}
				else if( ((CONTROL_HANDLE*)ControlHandle )->ViewType == Non_Hii_View_Type_Edit_Box )
				{

				UINT16 StrLen = 0;
				CHAR16* EditboxValue = (CHAR16*)NULL;
				StrLen = Wcslen((CHAR16 *)Value);
				EditboxValue = (CHAR16*) AllocateZeroPool((StrLen + 1) * sizeof(CHAR16));
				
				if(EditboxValue){
					memcpy(EditboxValue, Value, (StrLen+1) * sizeof(CHAR16));
				}
				if( ((CONTROL_HANDLE*)ControlHandle)->ControlValue )
				{
					memcpy(((CONTROL_HANDLE*)ControlHandle)->ControlValue, EditboxValue, (StrLen+1) * sizeof(CHAR16));
				}


	                    if( NULL != ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle ) ){
					if( NULL != ((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback ){
						((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback(ControlHandle);
	                        }
	                    }
	                }
				else if((((CONTROL_HANDLE*)ControlHandle)->ViewType == Non_Hii_View_Type_List_box)||(((CONTROL_HANDLE*)ControlHandle)->ViewType == Non_Hii_View_Type_Combo_box))
				{
					UINTN Index = 0 ;
                	GUI_CTRL_INFO_NODE * Ctrlnode = (GUI_CTRL_INFO_NODE*)NULL;
					GetCurrentPage(&gCurrentPage);
                	Index = *((UINTN *)Value);
					
					if(((CONTROL_HANDLE*)ControlHandle)->ControlValue == NULL)
						((CONTROL_HANDLE*)ControlHandle)->ControlValue = (UINTN *)AllocateZeroPool(sizeof(UINTN)) ;
					memcpy(((CONTROL_HANDLE*)ControlHandle)->ControlValue, &Index, (sizeof(UINTN))) ;
					
					IdentifyCtrlNodeByHandle( ((CONTROL_HANDLE*)ControlHandle),gCurrentPage,&Ctrlnode);
					
					if(Ctrlnode != NULL)
					{
						UpdateControlDataSrc(Ctrlnode);
						UpdateControlvalue(Ctrlnode);
						DisplayCurrentPage();
					}
					
					if(((CONTROL_HANDLE*) ControlHandle)->CtrlHandle != NULL)
					{
						if( NULL != ((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback )
							((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback(ControlHandle);
					}

				}
				else if( ((CONTROL_HANDLE*)ControlHandle)->ViewType == Non_Hii_View_Type_Time_Control )
				{
					if( Value )
					{
						EFI_TIME* Tm;
						EFI_TIME  TempTm;	

						Tm = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
						if(Tm)
						{
							Status = pRS->GetTime( Tm, (EFI_TIME_CAPABILITIES *)NULL );
							if( Status == EFI_SUCCESS )
							{
								TempTm = *((EFI_TIME*) Value);
								Tm->Hour = TempTm.Hour;
								Tm->Minute = TempTm.Minute;
								Tm->Second = TempTm.Second;
			
								Status = pRS->SetTime( Tm );
								if( Status == EFI_SUCCESS )
								{
									GUI_CTRL_INFO_NODE *CtrlNode = (GUI_CTRL_INFO_NODE *)NULL ;
									
									GetCurrentPage(&gCurrentPage);
									IdentifyCtrlNodeByHandle(((CONTROL_HANDLE*)ControlHandle),gCurrentPage,&CtrlNode);
									
									if(CtrlNode)
									{
										if(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue == NULL)
										{
											 ((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
										}
												 
										 GUI_CTRL_INFO_NODE *tCtrlnode = (GUI_CTRL_INFO_NODE *)NULL ;
										 memcpy(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue, &Value, (sizeof(EFI_TIME))) ;
	
										 UpdateControlvalue(CtrlNode);
										 DisplayCurrentPage();
									}
									
									// **** Need to free please un-comment it if not ****
/*									MemFreePointer((void**)&Tm);	
									
									if(EFI_ERROR(Status))
										return Status;*/
								}
							}
						}
					}
				}
				else if( ((CONTROL_HANDLE*)ControlHandle)->ViewType == Non_Hii_View_Type_Date_Control )
				{
					if( Value )
					{
						EFI_TIME* Tm;
						EFI_TIME  TempTm;	

						Tm = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
						if(Tm)
						{
							Status = pRS->GetTime( Tm, (EFI_TIME_CAPABILITIES *)NULL );
							if( Status == EFI_SUCCESS )
							{
								TempTm = *((EFI_TIME*) Value);
								Tm->Day = TempTm.Day;
								Tm->Month = TempTm.Month;
								Tm->Year = TempTm.Year;
			
								Status = pRS->SetTime( Tm );
								if( Status == EFI_SUCCESS )
								{
									GUI_CTRL_INFO_NODE *CtrlNode = (GUI_CTRL_INFO_NODE *)NULL ;
									
									GetCurrentPage( &gCurrentPage);
									IdentifyCtrlNodeByHandle(((CONTROL_HANDLE*)ControlHandle),gCurrentPage,&CtrlNode);
									
									if(CtrlNode)
									{
										if(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue == NULL)
										{
											 ((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
										}
												 
										 GUI_CTRL_INFO_NODE *tCtrlnode = (GUI_CTRL_INFO_NODE *)NULL ;
										 memcpy(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue, &Value, (sizeof(EFI_TIME))) ;
	
										 UpdateControlvalue(CtrlNode);
										 DisplayCurrentPage();
									}
									
									// **** Need to free please un-comment it if not ****
/*									MemFreePointer((void**)&Tm);	
									
									if(EFI_ERROR(Status))
										return Status;*/
								}
							}
						}
					}
				}
				else if((((CONTROL_HANDLE*)ControlHandle)->ViewType) == NON_HII_VIEW_TYPE_ORDEREDLIST_CONTAINER){
					COMBOBOXOPTIONS *CBOptions = (COMBOBOXOPTIONS *) NULL;
					UINT16 TempOption = 0;
					UINT64 TempValue = 0;
					GUI_CTRL_INFO_NODE * Ctrlnode = (GUI_CTRL_INFO_NODE*)NULL;
					int k = 0;
					
					CBOptions = (COMBOBOXOPTIONS *)((CONTROL_HANDLE*)ControlHandle)->CtrlSpecificData;
					for(int i =0, j =0;i < CBOptions->OptionCount; i++ , j++)
					{
						if(  (CBOptions->PtrTokens[j].Option) == (UINT16)(((UINT64*)Value)[i]) ){
							continue;
						}
						else{
							for( k = j  ;(CBOptions->PtrTokens[k].Option) != (UINT16)(((UINT64*)Value)[i]) ;  k++ ) ;
								TempOption = CBOptions->PtrTokens[j].Option;
								CBOptions->PtrTokens[j].Option = CBOptions->PtrTokens[k].Option;
								CBOptions->PtrTokens[k].Option =  TempOption;	
								TempValue = CBOptions->PtrTokens[j].Value;
								CBOptions->PtrTokens[j].Value = CBOptions->PtrTokens[k].Value;
								CBOptions->PtrTokens[k].Value =  TempValue;
						}
					}
					IdentifyCtrlNodeByHandle(((CONTROL_HANDLE*)ControlHandle),gCurrentPage,&Ctrlnode);
					if(Ctrlnode != NULL)
						Ctrlnode->InvalidateFlag = INVALIDATE_DS;
					//ShowPage(gCurrPageTree, NULL);
					GetCurrentPage(&gCurrentPage);
					ShowPage(gCurrentPage, NULL);
					
					if(((CONTROL_HANDLE*) ControlHandle)->CtrlHandle != NULL)
					{
						if( NULL != ((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback )
							((FRONT_PAGE_CONTROL *) ( ((CONTROL_HANDLE*) ControlHandle)->CtrlHandle))-> Callback(ControlHandle);
					}
				}
			}
		}
		break;
	}
  return Status;
}
void Delay()
{
	UINT32 Outer ;
	UINT32 Inner ;
	   for ( Outer = 1 ; Outer <= 32767 ; Outer++ )
	       for ( Inner = 1 ; Inner <= 100 ; Inner++ );
}

void UpdateProgressbar( GUI_CTRL_INFO_NODE *ProgressNode,UINT32 Percentage )
{
	 GUI_CTRL_INFO_NODE *CtrlNode = ProgressNode;
	 UINT32 ProgressPercent = Percentage ;
	 
		if(ProgressPercent <= 100)
			memcpy(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue, &ProgressPercent, (sizeof(UINT32))) ;

		UpdateControlvalue(CtrlNode);
	 DisplayCurrentPage();
	
/*    CProgressBarControl *ProgressCntrl  =(CProgressBarControl *)NULL ;
    ProgressCntrl = (CProgressBarControl *)(ProgressControlNode->GUICtrlViewHandle);
    if(NULL != ProgressCntrl) 
    {	
	if(Percentage <= 100)
		ProgressCntrl->Set_Progress(Percentage);
	//ProgressCntrl->Set_CtrlValue(&Percentage,sizeof(UINT32));
	ProgressControlNode->InvalidateFlag = INVALIDATE_VALUE;
    }*/
}

void AppPageUpdateforRefreshControls(GUI_CTRL_INFO_NODE *TreeNode)
{
	EFI_STATUS Status = EFI_SUCCESS ;
	GUI_CTRL_INFO_NODE *CtrlNode = TreeNode;
    void *Value = NULL ;

	if ( CtrlNode && (CtrlNode->CtrlHandle != INVALID_HANDLE) ) {
		if( ( (((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ViewType ) == Non_Hii_View_Type_Time_Control ) || ( (((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ViewType ) == Non_Hii_View_Type_Date_Control  ) )
		{
			EFI_TIME *Tm;	
			Tm = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
			Status = pRS->GetTime( Tm, (EFI_TIME_CAPABILITIES *)NULL );
			
			if(Status == 0)
			{
				 Value = Tm;
				 if(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue == NULL)
				 {
					 ((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue = (EFI_TIME *)AllocateZeroPool(sizeof(EFI_TIME));
				 }
					 
					 GUI_CTRL_INFO_NODE *tCtrlnode = (GUI_CTRL_INFO_NODE *)NULL ;
					 memcpy(((CONTROL_HANDLE*)(((GUI_CTRL_INFO_NODE *)CtrlNode)->CtrlHandle))->ControlValue, &Value, (sizeof(EFI_TIME))) ;

					 UpdateControlvalue(CtrlNode);
					 DisplayCurrentPage();
				 }

			}
		}


    CtrlNode = CtrlNode->Child;

    while(CtrlNode)
    {
    	AppPageUpdateforRefreshControls( CtrlNode);
        CtrlNode = CtrlNode->Next;
    }
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
