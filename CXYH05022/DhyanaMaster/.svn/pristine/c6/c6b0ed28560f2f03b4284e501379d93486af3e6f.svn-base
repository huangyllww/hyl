//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

#include <SetupDataXmlCreation.h>
#include <SetupDataXmlInterface.h>
#include <Token.h>
#include "Library/BaseMemoryLib.h"
#include "ConditionalExpressionApi.h"

#if AttribRegistryLib_SUPPORT
#include "BiosAttribConfigElinks.h"
#include "JsonHiiLibElinks.h"
#include "AutoId.h"
#else
#define SETUPDATA_XML_UNSUPPORTED_CONTROL_TYPE    {_CONTROL_TYPE_LABEL},{_CONTROL_TYPE_MSGBOX},{_CONTROL_TYPE_EMPTY},{_CONTROL_TYPE_MENU},
#define SETUPDATA_XML_ALLOW_DUPLICATE_MAP_ID 0
#define SUPPORTED_MAPPING_ID_LIST     {"x-AMI"},{"x-UEFI-AMI"},
#define HOOK_SUPPRESS_MAPID_LIST     {L"SETUP000"},
#endif

#define swprintf    SPrint
#define INT64_MAX_LIMIT 0x7FFFFFFFFFFFFFFF
#define INT64_MAX_MASK_BIT 0x8000000000000000
#ifndef REMOTE_SUPPORTED_LANGUAGES
#define REMOTE_SUPPORTED_LANGUAGES L"en-US"
#endif

MAP_ID_LIST 	    *gXmlMapIdList = (MAP_ID_LIST*)NULL;
SETUP_INFO_PROTOCOL gSetupInfoProtocol = { (GET_SETUP_DATA_XML) CreateSetupData};
CHAR8               *gXmlSupprtedMapLangArr[] = {SUPPORTED_MAPPING_ID_LIST ""};
UINTN               gXmlSupportedMapLangCount = sizeof(gXmlSupprtedMapLangArr)/sizeof(CHAR8*) - 1;


extern PAGE_LIST		    *gPages;
extern UINT8			    *gApplicationData;
extern CONTROL_INFO		    *gControlInfo;
extern UINTN		        gLangCount;
extern LANGUAGE_DATA	    *gLanguages;
extern BOOT_DATA		    *gDriverData;
extern UINTN			    gDriverOptionCount;
extern EFI_BOOT_SERVICES 	*gBS;
extern NVRAM_VARIABLE		*gVariableList;
extern EFI_HII_HANDLE 		gHiiHandle;
extern NVRAM_VARIABLE		*gOptimalDefaults;
extern EFI_GUID             gSetupInfoProtocolGuid;
extern MAP_ID_LIST          *gXmlAttrRegMapIdList;
/**
  Initializes all the Tse methods to access gPageInfo.

  @param VOID 

  @retval VOID

**/
VOID XmlCreationInitializeTseMethods ()
{
    extern UINT32 AllocatedFirstPageSize;       //TSE variable
#if (0==REMOTE_CONFIG_TSE_DEPENDENCY)
    EFI_STATUS Status = EFI_SUCCESS;
    
    Status = Uefi21HiiInitializeProtocol ();
    if ( EFI_ERROR ( Status ) )
       return;
    InitApplicationData ();   
#else   
    if (0 == AllocatedFirstPageSize)
    {
        HiiFixupData ();
        ProcessUIInitHook ();
        BootGetLanguages ();
        BootGetBootOptions();
        VarLoadVariables ((VOID **)&gVariableList, NULL);
        VarBuildDefaults();
    }
#endif
}

BOOLEAN XmlIsStringControl (CONTROL_TYPE  ControlType)
{
	BOOLEAN IsStrControl = FALSE;
	if(_CONTROL_TYPE_POPUP_STRING == ControlType)
	{
		IsStrControl = TRUE;
	}
	return IsStrControl;
}

void XmlInit_AttributeData(ATTRIBUTES      *DestAttributes)
{
		DestAttributes->_AttributeName = (CHAR16*)NULL;
		DestAttributes->_DisplayName = (CHAR16*)NULL;
		DestAttributes->_HelpText = (CHAR16*)NULL;
		DestAttributes->_Value = (VALUE*)NULL;		
		DestAttributes->_WarningText = (CHAR16*)NULL;	
		DestAttributes->_UpperBoundStr = (CHAR16*)NULL;
		DestAttributes->_LowerBoundStr = (CHAR16*)NULL;
		DestAttributes->_ScalarIncrementStr = (CHAR16*)NULL;
		DestAttributes->_MinLengthStr = (CHAR16*)NULL;
		DestAttributes->_MaxLengthStr = (CHAR16*)NULL;
		DestAttributes->_DefaultValue = (CHAR16*)NULL;
}

UINT8 XmlIsUnSignedInteger(CONTROL_INFO *In_ControlInfo)
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT64 Min = (UINT64)UefiGetMinValue(In_ControlInfo->ControlPtr);
	UINT64 Max = (UINT64)UefiGetMaxValue(In_ControlInfo->ControlPtr);
	UINT8 UnsignedStatus = 1;
	UINT32 size = 0;
	
	size = UefiGetWidth(In_ControlInfo->ControlPtr);

	switch(size)
	{
		case 1:
			UnsignedStatus = (UINT8)(IS_UNSIGNED(UINT8,Min,Max));
			break;
		case 2:
			UnsignedStatus = (UINT8)IS_UNSIGNED(UINT16,Min,Max);
			break;
		case 4:
			UnsignedStatus = (UINT8)IS_UNSIGNED(UINT32,Min,Max);
			break;
		case 8:
			UnsignedStatus = (UINT8)IS_UNSIGNED(UINT64,Min,Max);
			break;
		default:
			UnsignedStatus = (UINT8)(IS_UNSIGNED(UINT8,Min,Max));
			break;
	}
	return UnsignedStatus;
}

void Xmlget_ConfigData(ATTRIBUTES      *Attributes,CONTROL_INFO 	*In_CtrlInfo)
{
	CHAR16 *MinLengthStr = (CHAR16*)NULL;
	CHAR16 *MaxLengthStr = (CHAR16*)NULL;
	CHAR16 *UpperBoundStr = (CHAR16*)NULL;
	CHAR16 *LowerBoundStr = (CHAR16*)NULL;
	CHAR16 *ScalarIncrementStr = (CHAR16*)NULL;
	UINTN AllocStrlen = 128;
	UINT8 Base= 0; 
	UINT32 size = 0;
	
	if( (CONTROL_TYPE_POPUP_STRING == In_CtrlInfo->ControlType ) || (CONTROL_TYPE_PASSWORD == In_CtrlInfo->ControlType) )
	{
		MinLengthStr =EfiLibAllocateZeroPool( AllocStrlen *sizeof(CHAR16));
		MaxLengthStr =EfiLibAllocateZeroPool( AllocStrlen *sizeof(CHAR16));
		swprintf( MinLengthStr, AllocStrlen , L"%ld",(UINT64)UefiGetMinValue(In_CtrlInfo->ControlPtr));
		swprintf( MaxLengthStr, AllocStrlen , L"%ld",(UINT64)UefiGetMaxValue(In_CtrlInfo->ControlPtr));
		Attributes->_MinLengthStr   = MinLengthStr;
		Attributes->_MaxLengthStr   = MaxLengthStr;
	}
	else if(CONTROL_TYPE_NUMERIC == In_CtrlInfo->ControlType) 
	{
		UpperBoundStr =EfiLibAllocateZeroPool( AllocStrlen *sizeof(CHAR16));

		ScalarIncrementStr = EfiLibAllocateZeroPool( AllocStrlen *sizeof(CHAR16));
		swprintf( ScalarIncrementStr, AllocStrlen , L"%ld",(UINT64)UefiGetStepValue(In_CtrlInfo->ControlPtr));
		//swprintf( UpperBoundStr, AllocStrlen , L"%ld",(UINT64)UefiGetMaxValue(In_CtrlInfo->ControlPtr));

		size = UefiGetWidth(In_CtrlInfo->ControlPtr);
		Base = XmlIsUnSignedInteger(In_CtrlInfo);
		UpperBoundStr = XmlConvVfrNumToString((UINT64)UefiGetMaxValue(In_CtrlInfo->ControlPtr),size,Base);
		LowerBoundStr = XmlConvVfrNumToString((UINT64)UefiGetMinValue(In_CtrlInfo->ControlPtr),size,Base);
		Attributes->_UpperBoundStr   = UpperBoundStr;
		Attributes->_LowerBoundStr   = LowerBoundStr;
		Attributes->_ScalarIncrementStr = ScalarIncrementStr;	
	}
}

void XmlFreeMapIdList()
{
	MAP_ID_LIST *Visit = (MAP_ID_LIST*)NULL;
	MAP_ID_LIST *Temp = (MAP_ID_LIST*)NULL;
	Visit = gXmlMapIdList;
	
	while(Visit!= NULL)
	{
		Temp = Visit;
		if(Temp->MapID)
		MemFreePointer(&Temp->MapID);
		Visit=Visit->Next;
		MemFreePointer(&Temp);
	}
	gXmlMapIdList = (MAP_ID_LIST*)NULL;
}

/**
  Checks the mapping ID present in attribute registry entry or not
    
  @param  MapID             Mapping

  @return BOOLEAN
  @retval TRUE      If mapping ID found in the attribute registry Attributes entry
  @retval FALSE     If mapping ID not found in the attribute registry Attributes entry
**/

BOOLEAN CheckMappingidInAttribRegList (CHAR16 *MapID)
{
    MAP_ID_LIST *Temp = (MAP_ID_LIST*)NULL;
    MAP_ID_LIST *Visit = (MAP_ID_LIST*)NULL;
    
    if (NULL == gXmlAttrRegMapIdList)
    {
        return TRUE;
    }
    for(Visit = gXmlAttrRegMapIdList;Visit!= NULL;Visit=Visit->Next)
    {
        if( !StrCmp(Visit->MapID,MapID) )
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOLEAN XmlIsDuplicateMapId(CHAR16 *MapID)
{
	
	MAP_ID_LIST *Temp = (MAP_ID_LIST*)NULL;
	MAP_ID_LIST *Visit = (MAP_ID_LIST*)NULL;
	MAP_ID_LIST *Prev = (MAP_ID_LIST*)NULL;
	BOOLEAN DuplicateMapIdStatus = FALSE;
	
	if( NULL == MapID )
		return TRUE;
	if(NULL == gXmlMapIdList)
	{
	    gXmlMapIdList = EfiLibAllocateZeroPool(sizeof(MAP_ID_LIST));
		if(gXmlMapIdList)
		{
		    gXmlMapIdList->Next =(MAP_ID_LIST*)NULL;
		    gXmlMapIdList->MapID = EfiLibAllocateZeroPool( (StrLen(MapID)+1)*2);
			StrCpy(gXmlMapIdList->MapID,MapID);
		}
	}
	else
	{
		for(Visit = gXmlMapIdList;Visit!= NULL;Visit=Visit->Next)
		{
			Prev = Visit;
			if( !StrCmp(Visit->MapID,MapID) )
			{
				DuplicateMapIdStatus = TRUE;
				break;
			}
		}
		if(FALSE == DuplicateMapIdStatus)
		{
			Temp = EfiLibAllocateZeroPool(sizeof(MAP_ID_LIST));
			if(Temp)
			{
				Temp->Next =(MAP_ID_LIST*)NULL;
				Temp->MapID = EfiLibAllocateZeroPool( (StrLen(MapID)+1)*2);
				StrCpy(Temp->MapID,MapID);
			}
			Prev->Next = Temp;
		}
	}
	return DuplicateMapIdStatus;
}

/**
    Checks for space in input string

    @param CHAR16 * -> Mapping id string

    @retval BOOLEAN
**/
BOOLEAN XmlCheckForSpaceInMappingId (CHAR16 *AttributeName)
{
    UINTN   iIndex = 0;
    UINTN   StringLength = 0;
    BOOLEAN RetStatus = FALSE;
    
    if (NULL == AttributeName)
    {
        return RetStatus;
    }
    StringLength = StrLen (AttributeName);
    
    for (iIndex = 0; iIndex < StringLength; iIndex ++)
    {
        if (' ' == AttributeName [iIndex])
        {
            RetStatus = TRUE;
            break;
        }
    }
    return RetStatus;
}

/**
  Validates and returns the mapping ID
    
  @param  In_CtrlInfo       Retrieves the mapping ID for this control info
  @param  In_PageHandle     Page handle
  @param  In_DisplayName    Output Prompt for the mapping ID
  @param  In_AttributeName  Output mapping ID for the control

  @return BOOLEAN
  @retval TRUE      If valid control
  @retval FALSE     Not an valid control
**/

BOOLEAN XmlValidateMapId(CONTROL_INFO 	*In_CtrlInfo,EFI_HANDLE In_PageHandle,CHAR16  **In_DisplayName,CHAR16          **In_AttributeName)
{
	CHAR16          *DisplayName   = (CHAR16*)NULL;
	CHAR16          *AttributeName   = (CHAR16*)NULL;
	BOOLEAN 		ValidateStatus = FALSE;
	
	if( TRUE == IsXmlControlTypeSupported((UINT8)In_CtrlInfo->ControlType) )
	{
		DisplayName = Uefi21HiiGetStringLanguage(In_PageHandle,In_CtrlInfo->QuestionId,(CHAR16*)NULL);
		AttributeName = XmlGetActualHiiString(In_PageHandle,In_CtrlInfo->QuestionId);
		
        if (NULL != AttributeName)
        {
            if (TRUE == XmlCheckForSpaceInMappingId (AttributeName))
            {
                MemFreePointer (&DisplayName);
                MemFreePointer (&AttributeName);
                return FALSE;
            }
            if (FALSE == CheckMappingidInAttribRegList (AttributeName))
            {
                return FALSE;
            }
        }
        
		if( (XmlAllowDuplicateMapID()) || (FALSE == XmlIsDuplicateMapId(AttributeName)) )					
		{
			if(XmlAllowMappingStringOnly ())					
			{
				if( (NULL ==AttributeName) ||(!StrLen(AttributeName)))
				{
					MemFreePointer(&DisplayName);
					MemFreePointer(&AttributeName);	
				}
				else
					ValidateStatus = TRUE;
			}
			else
			{
				if( (FALSE == In_CtrlInfo->ControlFlags.ControlVisible) || (NULL ==DisplayName) ||(!StrLen(DisplayName)))
				{
					MemFreePointer(&DisplayName);
					MemFreePointer(&AttributeName);
				}
				else
					ValidateStatus = TRUE;
			}
			*In_DisplayName = DisplayName;
			*In_AttributeName = AttributeName;
		}
	}
	return ValidateStatus;
}

/**
    Removes the /n and /r for the incoming string

    @param CHAR16 ** -> String to whcih /n and /r will be removed and returned in same.
    
    @retval EFI_STATUS
**/
EFI_STATUS XmlRemoveSpecialChars (CHAR16 **String)
{
	UINTN iIndex = 0;
	
	while ((*String) [iIndex] != L'\0')
	{
		if (((*String) [iIndex] == L'\r' ) || ((*String) [iIndex] == L'\n'))
			(*String) [iIndex] = L' ';
		
		iIndex ++;
	}
	return EFI_SUCCESS;
}

/**
    Fills the Attribute registry fields

    @retval void
**/
void XmlGet_BiosAttributes()
{
	UINTN 			PageIndex = 0;
	UINTN 			ControlIndex = 0;
	UINTN 			AttributesIndex = 0;
	PAGE_INFO 		*PageInfo = (PAGE_INFO*)NULL;
	CONTROL_INFO 	*CtrlInfo = (CONTROL_INFO *)NULL;
	ATTRIBUTES      Attributes;
	CHAR16          *DisplayName   = (CHAR16*)NULL;
	CHAR16          *AttributeName   = (CHAR16*)NULL;
	CHAR16          *FormattedDisplayName   = (CHAR16*)NULL;
	CHAR16          *FormattedHelpText   = (CHAR16*)NULL;
	CHAR16          *TempFormattedHelpText   = (CHAR16*)NULL;
	void            *PrevHandle = 0;
	CHAR16          *GUIDString;
	EFI_GUID        FormGuid;
	EFI_GUID        DestFormGuid;
	UINTN           Size = 1024;
	PAGE_INFO       *NextPageInfo = (PAGE_INFO*)NULL; 
	CHAR16          *PageTemplateStr = L"\n\n\t\t\t<Page PageID=\"0x%x\" PageParentID=\"0x%x\" %s  PageFlags=\"0x%x\">";
	CHAR16          *text = NULL;
	CHAR16          *text2 = NULL;
	BOOLEAN         CtrlSaved = FALSE;
	GUID_INFO       *guidInfo = (GUID_INFO*)NULL;
	EFI_GUID ZeroGuid = {0};
	UINTN Flag = 0 ;
    BOOLEAN SkipFlag = 0 ;
    UINT16 value = 0 ;
	
	WriteToBuffer (L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<SetupData>");  

	for ( PageIndex = 1; PageIndex < (UINTN)gPages->PageCount; PageIndex++ ) 
	{
	    CHAR16          tempBuffer[1024] = {L""};

        text = NULL;
        text2 = NULL;
        Flag = 0 ;
        PageInfo = (PAGE_INFO*)( (UINTN)gApplicationData + gPages->PageList[PageIndex] );
		if( NULL == PageInfo->PageHandle )   //ignoring setup
			continue;

		if( PrevHandle != PageInfo->PageHandle )
        {       
            //Get GUID of formset to which the page belongs
            GetPageGuidFromPgIndex(PageInfo->PageID,&FormGuid);
            
            GUIDString=GetGUIDAsString(&FormGuid);
            
            swprintf(tempBuffer, Size , L"\n\t<Platform Guid=\"%s\" Handle=\"0x%x\">", GUIDString, PageInfo->PageHandle);
            
            WriteToBuffer(tempBuffer);
            
            WriteToBuffer(L"\n\t\t<Pages>");
            
            if(GUIDString != NULL)
                FreePool(GUIDString);
        }
		text2 = GetMultiLanguageXmlString (PageInfo->PageHandle, PageInfo->PageSubTitle, L"PageTitle", L"");
		for ( ControlIndex = 0; ControlIndex < PageInfo->PageControls.ControlCount; ControlIndex++ )
		{
		    CtrlSaved = FALSE;
			SkipFlag = FALSE;
		    
			CtrlInfo = (CONTROL_INFO *)((UINT8 *)(gControlInfo) + PageInfo->PageControls.ControlList[ControlIndex]);
	
            if(CONTROL_TYPE_SUBMENU == CtrlInfo->ControlType)
            {
                value = UefiGetSpecialEqIDValue (CtrlInfo, &guidInfo);
               
                if (value != (UINT16)-2)
                {
                    if(guidInfo->GuidFlags.GuidLaunchCallback)
                    {    
                        if( GUID_KEY_AMI_CALLBACK == guidInfo->GuidKey )
                        {
                            //Allow save changes and discard changes only
                            if( (SAVE_VALUE != value) && (DISCARD_VALUE != value) )
                            {    
                                SkipFlag = FALSE ;
                                continue;
                            }
                            else
                                SkipFlag = TRUE ;
                        }
                    }
                }
            }
#if SUPPRESS_COND_EXPRESSION_CONTROLS			
            if(( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr) != NULL )
            {
               if((SkipFlag != TRUE) && ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_SUPPRESS_IF_OP
                         || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_NO_SUBMIT_IF_OP
                         || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_INCONSISTENT_IF_OP
                         || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_GRAY_OUT_IF_OP
                         || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_DISABLE_IF_OP
                         || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_WARNING_IF_OP )
                   continue;
               else
                   SkipFlag = FALSE ;
            }
#endif
#if REMOTE_COND_EXPRESSION_SUPPORT			
            if (
				(CONTROL_TYPE_SUBMENU == CtrlInfo->ControlType ||
				CONTROL_TYPE_TEXT == CtrlInfo->ControlType ||
				CONTROL_TYPE_MEMO == CtrlInfo->ControlType) &&
				(NULL != CtrlInfo->ControlConditionalPtr) &&
				(TRUE != SkipFlag)
				)
            {
                if(FALSE == XmlValidateMapId(CtrlInfo,PageInfo->PageHandle,&DisplayName,&AttributeName))
                    continue;
            }
#endif            
			if(CONTROL_TYPE_SUBMENU == CtrlInfo->ControlType)
			{
				if(Flag == 0)
				{
					swprintf(tempBuffer, Size, PageTemplateStr, PageInfo->PageID, PageInfo->PageParentID, text2, PageInfo->PageFlags);
					WriteToBuffer(tempBuffer);
				}
			    GetPageGuidFromPgIndex(CtrlInfo->ControlDestPageID,&DestFormGuid);
			    if( CompareGuid (&FormGuid, &DestFormGuid) == 0 )
			    {
    			    WriteControlInfoToBuffer(CtrlInfo,NULL,DestFormGuid);    
			    }  
			    else
					WriteControlInfoToBuffer(CtrlInfo,NULL,ZeroGuid);  
			    Flag++ ;
			    CtrlSaved = TRUE;
			    continue;
			}
			else if(CONTROL_TYPE_TEXT == CtrlInfo->ControlType)
			{
				if(Flag == 0)
				{
					swprintf(tempBuffer, Size, PageTemplateStr, PageInfo->PageID, PageInfo->PageParentID, text2, PageInfo->PageFlags);
					WriteToBuffer(tempBuffer);
				    Flag++ ;
				}
				WriteControlInfoToBuffer(CtrlInfo, &Attributes,ZeroGuid);
				continue;
			}
            else if(CONTROL_TYPE_MEMO == CtrlInfo->ControlType)
            {
#if(!SUPPORT_SUBTITLE_IN_SAVE_EXIT_PAGE)
                if(PageInfo->PageFormID != EXIT_MAIN) 
                {
#endif
                    if(Flag == 0)
                    {
                        swprintf(tempBuffer, Size, PageTemplateStr, PageInfo->PageID, PageInfo->PageParentID, text2, PageInfo->PageFlags);
                        WriteToBuffer(tempBuffer);
                        Flag++ ;
                    }
                    WriteControlInfoToBuffer(CtrlInfo, &Attributes,ZeroGuid);
#if(!SUPPORT_SUBTITLE_IN_SAVE_EXIT_PAGE)
                }
#endif
                continue;
            }
			
			if(FALSE == XmlValidateMapId(CtrlInfo,PageInfo->PageHandle,&DisplayName,&AttributeName))
				continue;

			//Check to export the MapId or not
			 if( FALSE ==  IsXmlMapIdSupported (AttributeName ))
			     continue;
			 
			XmlInit_AttributeData(&Attributes);
			Attributes._AttributeName = AttributeName;

			if (NULL != DisplayName)
			{
				FormattedDisplayName = SkipEscCode (DisplayName);
				FreePool (DisplayName);
				XmlRemoveSpecialChars (&FormattedDisplayName);
				Attributes._DisplayName = FormattedDisplayName;
			}
			TempFormattedHelpText = Uefi21HiiGetStringLanguage (PageInfo->PageHandle,CtrlInfo->ControlHelp,(CHAR16*)NULL);
			if (NULL != TempFormattedHelpText)
			{
				FormattedHelpText = SkipEscCode (TempFormattedHelpText);
				FreePool (TempFormattedHelpText);
				XmlRemoveSpecialChars (&FormattedHelpText);
				Attributes._HelpText = FormattedHelpText; 
			}
			Attributes._ReadOnly     = CtrlInfo->ControlFlags.ControlReadOnly;
			Attributes._Type         = (CONTROL_TYPE)CtrlInfo->ControlType;
			Attributes._WarningText = NULL;
			Attributes._DefaultValue = NULL;		
			Xmlget_ConfigData(&Attributes,CtrlInfo);
			
         if( FALSE == CtrlSaved )
         {
			if(Flag == 0)
			{
				swprintf(tempBuffer, Size, PageTemplateStr, PageInfo->PageID, PageInfo->PageParentID, text2, PageInfo->PageFlags);
				WriteToBuffer(tempBuffer);
			    Flag++ ;
			}
            WriteControlInfoToBuffer(CtrlInfo, &Attributes,ZeroGuid);    
         }
            XmlFreeAttributeData(&Attributes);			
			DisplayName = NULL;
			AttributeName = NULL;
		}
		
		if(Flag != 0)
			WriteToBuffer(L"\n\t\t\t</Page>");
		
		if( (PageInfo->PageParentID == 0x0) && (Flag == 0) ) 
		{
			swprintf(tempBuffer, Size, PageTemplateStr, PageInfo->PageID, PageInfo->PageParentID, text2, PageInfo->PageFlags);
			WriteToBuffer(tempBuffer);
			WriteToBuffer(L"\n\t\t\t</Page>");
		}
		        
		if ( (PageIndex+1) < (UINTN)gPages->PageCount )
        {
            NextPageInfo = (PAGE_INFO*)( (UINTN)gApplicationData + gPages->PageList[PageIndex+1] );
        }
            
        if( ( gPages->PageCount-1 == PageIndex ) || ( NextPageInfo->PageHandle != PageInfo->PageHandle ) )
        {
            WriteToBuffer(L"\n\t\t</Pages>\n\t</Platform>");
        }
		
        PrevHandle = PageInfo->PageHandle;
        
        if(text != NULL)
            FreePool(text);
        if(text2 != NULL)
            FreePool(text2);
	}
    WriteToBuffer (L"\n</SetupData>");
    XmlFreeMapIdList();
}

void XmlFreeAttributeData(ATTRIBUTES      *Attributes)
{
	VALUE		*Value = (VALUE*)NULL;
	UINTN       Index = 0;
	
	if(NULL != Attributes)
	{
		MemFreePointer(&(Attributes->_DisplayName));
		MemFreePointer(&(Attributes->_HelpText));
		MemFreePointer(&(Attributes->_AttributeName));
		
		if(NULL != Attributes->_Value)
		{
			for(Index = 0;Index<Attributes->_ValueCount;Index++)
			{
				Value = &(Attributes->_Value[Index]);
				MemFreePointer( (VOID**)&(Value->_ValueName) );
				MemFreePointer( (VOID**)&(Value->_ValueDisplayName) );
			}
		}
		MemFreePointer( (VOID**)&(Attributes->_Value) );
		MemFreePointer(&(Attributes->_WarningText));
		MemFreePointer(&(Attributes->_UpperBoundStr));
		MemFreePointer(&(Attributes->_LowerBoundStr));
		MemFreePointer(&(Attributes->_ScalarIncrementStr));
		MemFreePointer(&(Attributes->_MinLengthStr));
		MemFreePointer(&(Attributes->_MaxLengthStr));
		MemFreePointer(&(Attributes->_DefaultValue));
	}
}

VOID InstallSetupInfoProtocol(VOID)
{
	gBS->InstallProtocolInterface(&gImageHandle, &gSetupInfoProtocolGuid, EFI_NATIVE_INTERFACE, &gSetupInfoProtocol);
}

UINT32 XmlGetControlGuidType(CONTROL_INFO *In_ControlInfo)
{
	UINT16 value = 0;
	GUID_INFO *guidInfo = (GUID_INFO*)NULL;
	UINT32 GuidKey = 0;
	
	value = UefiGetSpecialEqIDValue (In_ControlInfo, &guidInfo);
	
	if (value != (UINT16)-2)
		GuidKey =  guidInfo->GuidKey;
	
	return GuidKey;
			
}

CHAR16 *XmlGetActualHiiString(EFI_HII_HANDLE handle, UINT16 token)
{
   EFI_STATUS status = EFI_SUCCESS;
   CHAR16 *buffer;
   UINTN bufferSize = 16;
   EFI_HII_STRING_PROTOCOL *HiiString=(EFI_HII_STRING_PROTOCOL *)NULL;
   CHAR8 *language; 
   UINTN MaplangIndex = 0;

   for (MaplangIndex=0; MaplangIndex < gXmlSupportedMapLangCount; MaplangIndex++)
   {
      buffer = EfiLibAllocateZeroPool(bufferSize);
      if(NULL == buffer)
         return buffer;
      
      language = gXmlSupprtedMapLangArr[MaplangIndex];
      status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID**)&HiiString);
      if( EFI_ERROR( status ) )
         return buffer;
   
      status = HiiString->GetString(HiiString, language, handle,token, buffer,&bufferSize, NULL);
      if (EFI_BUFFER_TOO_SMALL == status)
      {
         MemFreePointer( (VOID **)&buffer );
         buffer = EfiLibAllocateZeroPool(bufferSize);
         if(NULL == buffer)
            return buffer;       
         status = HiiString->GetString(HiiString, language, handle,token, buffer,&bufferSize, NULL);
      }
      if ( EFI_SUCCESS == status )
         break;
      MemFreePointer( (VOID **)&buffer );
   }
   return buffer;
}

/**
    Convert unsigned number to string

    @param UINT64 InputVal,CHAR16 *String

    @retval void
**/

void XmlConvertIntToString(UINT64 InputVal,CHAR16 *String)
{
	UINT64 Val = 0;
	UINT8 Digit = 0;
	CHAR8 RevString[128];
	//CHAR16 *String = NULL;
	INT32 Index =0,ValidIndex =0;
	INT32 Count =0;
	BOOLEAN ValidDigitFound = FALSE;
	Val = InputVal;
	
	if(Val & INT64_MAX_MASK_BIT)
	{
		Val = INT64_MAX_LIMIT; 
	}
	while(Val!= 0)
	{
		Digit = Val%10;
		RevString[Count] = Digit+48;
		Val = Val/10;
		Count++;
	}
	
	//String = AllocateZeroPool( 512 *sizeof(CHAR16));
	for(Index=Count-1;Index >= 0;--Index)
	{
		if(FALSE == ValidDigitFound)
		{
			if(RevString[Index] == 0)
				continue;
		}
		ValidDigitFound = TRUE;
		String[ValidIndex] = (CHAR16)(RevString[Index]);
		ValidIndex++;
	}
	if(FALSE == ValidDigitFound)
	{
		String[ValidIndex] = (CHAR16)(48);
		ValidIndex++;
	}
	
	String[ValidIndex] = L'\0';
}

CHAR16* XmlConvVfrNumToString(UINT64 Num,UINTN ControlDataWidth,UINT8 IsUnSigned)
{
	CHAR16 *Text = NULL;
	Text = (CHAR16*)AllocateZeroPool(128 * sizeof(CHAR16));

	if (1 ==IsUnSigned )
		XmlConvertIntToString((UINT64)Num,Text);
	else
	{
		switch(ControlDataWidth)
		{
			case 1:
					Swprintf( Text, L"%d", (INT8)Num );	
				break;
			case 2:
					Swprintf( Text,L"%d", (INT16)Num);	
				break;
			case 4:
					Swprintf( Text, L"%d", (INT32)Num);		
				break;
			case 8:
					Swprintf( Text, L"%ld", (INT64)Num );		
				break;
			default:
					Swprintf( Text, L"%d", (UINT8)Num);
				break;
		}
	}
	

	return Text;
}

BOOLEAN XmlAllowDuplicateMapID ()
{
#if SETUPDATA_XML_ALLOW_DUPLICATE_MAP_ID
		return SETUPDATA_XML_ALLOW_DUPLICATE_MAP_ID ;
#endif			
	return 0;
}

BOOLEAN XmlAllowMappingStringOnly  ()
{
#if LIST_MAPPING_STRING_ONLY
		return LIST_MAPPING_STRING_ONLY ;
#endif			
	return 0;
}

/**
    Returns TRUE of the control type is supported
     
    @param UINT8 In_ControlType

    @retval BOOLEAN
     
**/
BOOLEAN IsXmlControlTypeSupported(UINT8 In_ControlType)
{
	BOOLEAN IsSupported = TRUE;
	UINTN Count = 0;
	UINTN Index = 0;
	CONTROL_TYPE SupportedControlArr[]= { SETUPDATA_XML_UNSUPPORTED_CONTROL_TYPE };

	if(In_ControlType < MAX_SUPPORTED_CONTROL)
	{
		Count = sizeof(SupportedControlArr)/sizeof(CONTROL_TYPE);
		for(Index = 0;Index<Count;Index++)
		{
			if(In_ControlType == (UINT8)SupportedControlArr[Index] )
			{
				IsSupported = FALSE;
				break;
			}
		}
	}
	else
		IsSupported = FALSE;
	
	return IsSupported;
}

/**
    Returns TRUE if the MapString is not in Suppress MapIdList
     
    @param CHAR16 *MapString

    @retval BOOLEAN
     
**/

BOOLEAN IsXmlMapIdSupported(CHAR16 *MapString)
{
    UINT32 i = 0;
    
    MAPSTRING_NAME SuppressMapList[] = {HOOK_SUPPRESS_MAPID_LIST L""};//Array of defined MapIds
    UINT32 MapIdCount = sizeof(SuppressMapList)/sizeof(MAPSTRING_NAME);
    
    
    for(i = 0; i < MapIdCount; i++)
    {
        if((Wcscmp(MapString, (SuppressMapList[i].MapId)) == 0))
        {
           return FALSE; 
        }
    }
       
    return TRUE;
}

/**
  Returns the multilanguage strings for string ID
    
  @param  Handle          Handle of the HII database
  @param  StringId        String ID in the HII database
  @param  LeftString      Label string	
  @param  DefaultString   If string token is not found in the database then this string will be used

  @return CHAR16 *		  String returned with LeftString and retrieved string
**/

CHAR16 *GetMultiLanguageXmlString (VOID *Handle, UINT16 StringId, CHAR16 *LeftString, CHAR16 *DefaultString)
{
    UINTN   iIndex = 0;
    UINTN   TotalStrLength = 0;
    UINTN   StrLength = 0;
    CHAR16  *String = NULL;
    CHAR16  *TotalString = NULL;
    CHAR16  *text = NULL;
    CHAR16  *text2 = NULL;
    CHAR16  *RemoteSupportedLangs = REMOTE_SUPPORTED_LANGUAGES;
    
    for (iIndex = 0; iIndex < gLangCount; iIndex ++)
    {
        if (NULL != StrStr (RemoteSupportedLangs, gLanguages [iIndex].Unicode)) //If language is not there then don't push attribute registry
        {
            text = Uefi21HiiGetStringLanguage (Handle, StringId , gLanguages [iIndex].Unicode);
            if (text && StrLen (text))
            {
                text2 = SkipEscCode (text);
                text2 = CheckSpecialChar(text2);
                MemFreePointer (&text);
            }
            else
            {
                text2 = EfiLibAllocateZeroPool ((StrLen (DefaultString) + 1) * sizeof (CHAR16));
                if (NULL != text2)
                {
                    StrCpy (text2, DefaultString);
                }
            }
            StrLength = (StrLen (text2) + 25) * sizeof (CHAR16);        //25 for left attribute name
            String = EfiLibAllocateZeroPool (StrLength);
            if (NULL != String)
            {
                if (!StrCmp (gLanguages [iIndex].Unicode, L"en-US"))
                {
                    swprintf (String, StrLength, L"%s=\"%s\" ", LeftString, text2);
                }
                else
                {
                    swprintf (String, StrLength, L"%s-%s=\"%s\" ", LeftString, gLanguages [iIndex].Unicode, text2);
                }
                TotalString = MemReallocateZeroPool (TotalString, TotalStrLength, TotalStrLength + StrLength);
                if (NULL != TotalString)
                {
                    StrCat (TotalString, String);
                }
                TotalStrLength += StrLength;
                MemFreePointer (&String);
            }
            MemFreePointer (&text2);
        }
    }
    return TotalString;
}

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
