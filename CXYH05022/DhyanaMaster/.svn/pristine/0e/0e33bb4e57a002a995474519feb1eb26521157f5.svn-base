//**        (C)Copyright 1985-2017, American Megatrends, Inc.    **

/** @file Helper.c
    Defined functions to dump content to files and to get control information help

**/

//---------------------------------------------------------------------------
#include <sys/types.h>
#include <Include/AmiDxeLib.h>
#include "setupdata.h"
#include <Library/BaseLib.h>
#include <Token.h>
#include <BiosAttribTypeLib.h>
#include <BiosAttribTypeLibHelper.h>
#include <Include/AMIVfr.h>
#include <Protocol/HiiString.h>
#include <JsonUtilityLib.h>
#include "BiosAttribTypeLib.h"
#include "BiosAttribConfigElinks.h"
#include "JsonHiiLibElinks.h"

//---------------------------------------------------------------------------
#ifndef SUPPORTED_MAPPING_ID_LIST
#define SUPPORTED_MAPPING_ID_LIST     {"x-AMI"},{"x-UEFI-AMI"},
#endif
#define swprintf SPrint

CHAR8 *gSupprtedMapLangArr[]={SUPPORTED_MAPPING_ID_LIST ""};
UINTN gSupportedMapLangCount = sizeof(gSupprtedMapLangArr)/sizeof(CHAR8*) - 1;
CHAR16      BiosControlTypeNames [][50] =
	{
	{L"String"},                            //_CONTROL_TYPE_NULL
	{L"String"},                         //_CONTROL_TYPE_SUBMENU
	{L"String"},                           //_CONTROL_TYPE_LABEL
	{L"String"},                            //_CONTROL_TYPE_TEXT
	{L"String"},                            //_CONTROL_TYPE_DATE
	{L"String"},                            //_CONTROL_TYPE_TIME
	{L"Enumeration"},                        //_CONTROL_TYPE_POPUPSEL
	{L"Empty"},                        		//_CONTROL_TYPE_EMPTY
	{L"String"},                            //_CONTROL_TYPE_MEMO
	{L"String"},                          //_CONTROL_TYPE_MSGBOX
	{L"Boolean"},                        //_CONTROL_TYPE_CHECKBOX 
	{L"Integer"},                         //_CONTROL_TYPE_NUMERIC
	{L"Edit"},                            //_CONTROL_TYPE_EDIT
	{L"Password"},                        //_CONTROL_TYPE_PASSWORD
	{L"String"},                            //_CONTROL_TYPE_MENU
	{L"Enumeration"},                     //_CONTROL_TYPE_ORDERED_LIST
	{L"String"},                       //_CONTROL_TYPE_POPUPEDIT
	{L"String"},                       //_CONTROL_TYPE_VARSTORE_SELECT
	{L"String"},                       //_CONTROL_TYPE_VARSTORE_SELECT_PAIR
	{L"String"},                       //_CONTROL_TYPE_POPUP_STRING
	{L"String"},                       //_CONTROL_TYPE_VARSTORE
	
	{L"String"},                       //_INCONSISTENT_IF
	{L"String"},                       //_NO_SUBMIT_IF
	{L"String"},                       //_DISABLE_IF
	{L"String"},                       //_CONTROL_TYPE_VARSTORE_NAME_VALUE
	{L"String"},                       //_CONTROL_TYPE_VARSTORE_EFI
	{L"String"},                       //_CONTROL_TYPE_VARSTORE_DEVICE
	{L"String"},                       //_CONTROL_TYPE_ACTION
	{L"String"},                       //_CONTROL_TYPE_RESET
	{L"String"},                       //_CONTROL_TYPE_RULE
	{L"String"},                       //_CONTROL_TYPE_REF2
	{L"String"},                       //_CONTROL_TYPE_SLIDE
	{L"String"},                       //_WARN_IF
	};

CHAR16 BiosSetupDataStr[][128] = 
    {
	{L"SupportedSystems"},	
	{L"ProductName"},
	{L"SystemId"},
	{L"FirmwareVersion"},	
	{L"RegistryEntries"},	
	{L"Attributes"},
	{L"DisplayName"},
	{L"AttributeName"},
	{L"HelpText"},
	{L"ReadOnly"},
	{L"Type"},
	{L"Value"},
	{L"ValueDisplayName"},
	{L"ValueName"},				
	{L"WarningText"},
	{L"UpperBound"},
	{L"LowerBound"},
	{L"ScalarIncrement"},
	{L"MinLength"},
	{L"MaxLength"},
	{L"DefaultValue"},
    };

//---------------------------------------------------------------------------

CHAR16 *StrDup8to16( CHAR8 *string );
UINT16  UefiGetSpecialEqIDValue(CONTROL_INFO *ControlInfo, GUID_INFO **GuidInfo);
VOID    *VarGetNvramName( CHAR16 *name, EFI_GUID *guid, UINT32 *attributes, UINTN *size );
VOID    *EfiLibAllocateZeroPool ( UINTN   AllocationSize  );
VOID    *VarGetVariable( UINT32 variable, UINTN *size );
VOID    MemFreePointer( VOID **ptr );
UINTN   SPrint (  OUT CHAR16        *Buffer,  IN  UINTN         BufferSize,  IN  CONST CHAR16  *Format,  ...  );
size_t  strlen(const char *s);
char    *strcpy(char * __restrict s1, const char * __restrict s2);

//---------------------------------------------------------------------------

/**
  get control Guid type

  @param CONTROL_INFO* 
  
  @retval UINT32

**/

UINT32 GetControlGuidType(CONTROL_INFO *In_ControlInfo)
{
	UINT16 value = 0;
	GUID_INFO *guidInfo = (GUID_INFO*)NULL;
	UINT32 GuidKey = 0;
	
	value = UefiGetSpecialEqIDValue (In_ControlInfo, &guidInfo);
	
	if (value != (UINT16)-2)
		GuidKey =  guidInfo->GuidKey;
	
	return GuidKey;
			
}

/**
  Construct bios attribute in json formatted string
  @param In_Attributes		 ATTRIBUTES to write in the file
  
  @retval void

**/

void WriteAttributesToFile(ATTRIBUTES 	*In_Attributes)
{
	VALUE* Value = (VALUE*)NULL;
	UINTN	Index = 0;
	static BOOLEAN IsStarted = FALSE;
	CHAR16 *JsonStr = (CHAR16*)NULL;
	
	if(In_Attributes)
	{		
			if(FALSE == IsStarted)
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_OBJECT_OPEN ,4,(CHAR16*)NULL,(CHAR16*)NULL);
				UpdateData(&JsonStr);
				IsStarted = TRUE;
			}
			else
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_OBJECT_OPEN,4,(CHAR16*)NULL,(CHAR16*)NULL);
				UpdateData(&JsonStr);
			}
			
			if((NULL != In_Attributes->_AttributeName)&&(StrLen(In_Attributes->_AttributeName)))
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+2],In_Attributes->_AttributeName);	
				UpdateData(&JsonStr);
			}
			if(NULL != In_Attributes->_DisplayName)
			{
			    GetJsonDataAndUpdate (TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+1],In_Attributes->_DisplayName);
			}
			if(NULL != In_Attributes->_HelpText)
			{
			    GetJsonDataAndUpdate (TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+3],In_Attributes->_HelpText);
			}
			
			JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+4],BOOL_STR(In_Attributes->_ReadOnly));
			UpdateData(&JsonStr);

			JsonStr = GetJsonDataStr(TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+5],BiosControlTypeNames[In_Attributes->_Type]);
			UpdateData(&JsonStr);
			
			//Skip adding value array to checkbox type controls
			if( (CONTROL_TYPE_CHECKBOX != In_Attributes->_Type) && (NULL != In_Attributes->_Value) &&(In_Attributes->_ValueCount) )
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_ARRAY_OPEN,5,BiosSetupDataStr[ATTRIBUTES_INDEX+6],(CHAR16*)NULL);
				UpdateData(&JsonStr);	
				for(Index=0;Index< In_Attributes->_ValueCount ;Index++)
				{
					Value = In_Attributes->_Value +Index;
					JsonStr= GetJsonDataStr(TRUE, JSON_OBJECT_OPEN,6,(CHAR16*)NULL,(CHAR16*)NULL);
					UpdateData(&JsonStr);
					GetJsonDataAndUpdate (TRUE, JSON_DATA,7,BiosSetupDataStr[ATTRIBUTES_INDEX+7],Value->_ValueDisplayName);
					if( CONTROL_TYPE_CHECKBOX == In_Attributes->_Type)
					{
						JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,7,BiosSetupDataStr[ATTRIBUTES_INDEX+8],Value->_ValueName);
						UpdateData(&JsonStr);
					}
					else if( !Wcscmp(L"Enumeration",BiosControlTypeNames[In_Attributes->_Type]) )
					{
					    GetEngJsonDataAndUpdate (TRUE, JSON_DATA,7,BiosSetupDataStr[ATTRIBUTES_INDEX+8],Value->_ValueDisplayName);
					}
					else
					{
						JsonStr = GetJsonDataStr(TRUE, JSON_DATA,7,BiosSetupDataStr[ATTRIBUTES_INDEX+8],Value->_ValueName);
						UpdateData(&JsonStr);
					}
					JsonStr = GetJsonDataStr(TRUE, JSON_OBJECT_CLOSE,6,(CHAR16*)NULL,(CHAR16*)NULL);	
					UpdateData(&JsonStr);
				}
				JsonStr = GetJsonDataStr(TRUE, JSON_ARRAY_CLOSE,5,(CHAR16*)NULL,(CHAR16*)NULL);
				UpdateData(&JsonStr);
			}


			if(NULL != In_Attributes->_UpperBoundStr)
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+10],In_Attributes->_UpperBoundStr);
				UpdateData(&JsonStr);
			}
			if(NULL != In_Attributes->_LowerBoundStr)
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+11],In_Attributes->_LowerBoundStr);
				UpdateData(&JsonStr);
			}
			if(NULL != In_Attributes->_ScalarIncrementStr)
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+12],In_Attributes->_ScalarIncrementStr);
				UpdateData(&JsonStr);
			}
			if(NULL != In_Attributes->_MinLengthStr)
			{
#if REMOTE_CLEAR_PASSWORD_SUPPORT
			    if (CONTROL_TYPE_PASSWORD != In_Attributes->_Type )
			    {
			        JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+13],In_Attributes->_MinLengthStr);
			    }
#else
			    JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+13],In_Attributes->_MinLengthStr);
#endif
				UpdateData(&JsonStr);
			}
			if(NULL != In_Attributes->_MaxLengthStr)
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+14],In_Attributes->_MaxLengthStr);
				UpdateData(&JsonStr);
			}
			if((NULL != In_Attributes->_WarningText)&&(StrLen(In_Attributes->_WarningText)))
			{
				JsonStr = GetJsonDataStr(TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+9],In_Attributes->_WarningText);
				UpdateData(&JsonStr);
			}
			else
			{
				if(NULL == In_Attributes->_AttributeName)
				{
					JsonStr = GetJsonDataStr(TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+9],L"Not configurable as no attribute name present");
					UpdateData(&JsonStr);
				}
			}
			if(NULL != In_Attributes->_DefaultValue)
			{
				if( ( CONTROL_TYPE_CHECKBOX == In_Attributes->_Type) || (In_Attributes->_Type == _CONTROL_TYPE_NUMERIC))
					JsonStr = GetJsonDataStr(TRUE, JSON_DATA_INTEGER,5,BiosSetupDataStr[ATTRIBUTES_INDEX+15],In_Attributes->_DefaultValue);
				else
					JsonStr = GetJsonDataStr(TRUE, JSON_DATA,5,BiosSetupDataStr[ATTRIBUTES_INDEX+15],In_Attributes->_DefaultValue);
				UpdateData(&JsonStr);
			}
			
			JsonStr = GetJsonDataStr(TRUE, JSON_OBJECT_CLOSE,4,(CHAR16*)NULL,(CHAR16*)NULL);
			UpdateData(&JsonStr);
	}
	else
	{
		JsonStr = GetJsonDataStr(TRUE, JSON_DATA,3,BiosSetupDataStr[ATTRIBUTES_INDEX],(CHAR16*)L"No Entry Found");
		UpdateData(&JsonStr);
	}
}

/**
  Appends biosattribute header to VECTOR buffer
  
  @retval void

**/

void Dump_JsonHeaderInfo()
{
	CHAR16 Comments1[128] = L"\n  \"@Redfish.Copyright\": \"Copyright 2017 Distributed Management Task Force, Inc. (DMTF). All rights reserved.\",";
	CHAR16 Comments2[128] = L"\n  \"@odata.type\": \"#AttributeRegistry.1.0.0.AttributeRegistry\",";
	CHAR16 Header1[128] = L"\n  \"Description\": \"This registry defines a representation of BIOS Attribute instances\",";
	CHAR16 Header2Frmt[128] = L"\n  \"Id\": \"BiosAttributeRegistry%s.%d.%d.%d\",";
	CHAR16 Header2[256];
	CHAR16 Header3Fmt[128] = L"\n  \"Language\": \"%s\",";
    CHAR16 Header3[128];
	CHAR16 Header4Frmt[128] = L"\n  \"Name\": \"%s BIOS Attribute Registry\",";
	CHAR16 Header4[256];
	CHAR16 Header5[128] = L"\n  \"OwningEntity\": \"AMI\",";
	CHAR16 Header6[128] = L"\n  \"RegistryPrefix\": \"BiosAttributeRegistry\",";
	CHAR16 Header7Frmt[128] = L"\n  \"RegistryVersion\": \"%d.%d.%d\",";
	CHAR16 Header7[256];
	CHAR16 *ProjectTag;
	UINTN  iIndex = 0;
	extern MULTILANG_VECTOR *gMultiLangVector;
	extern UINTN            gLangCount;
	
	ProjectTag = StrDup8to16(Get_ProjectTag ());
			
	SPrint(Header2,256,Header2Frmt,ProjectTag,Get_ProjectMajorVersion (),Get_ProjectMinorVersion (),Get_ProjectBuildVersion ());
	SPrint(Header4,256,Header4Frmt,ProjectTag);
	SPrint(Header7,256,Header7Frmt,Get_ProjectMajorVersion (),Get_ProjectMinorVersion (),Get_ProjectBuildVersion ());
    for (iIndex = 0; iIndex < gLangCount; iIndex ++)
    {
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Comments1);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Comments2);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header1);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header2);
        SPrint(Header3,128,Header3Fmt,gMultiLangVector[iIndex].Language);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header3);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header4);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header5);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header6);
        AppendString(TRUE,gMultiLangVector[iIndex].LanguageVector,Header7);
    }
	MemFreePointer( (VOID **)&ProjectTag );
}

/**
  Collect actual Hii string for the requested language passed in paramenter
  @param EFI_HII_HANDLE 
  @param UINT16 
  @param CHAR8*
     
  @retval CHAR16*

**/

CHAR16* GetActualHiiString(EFI_HII_HANDLE handle, UINT16 token)
{
	EFI_STATUS status = EFI_SUCCESS;
	CHAR16 *buffer=NULL;
	UINTN bufferSize = 16;
	EFI_HII_STRING_PROTOCOL *HiiString=(EFI_HII_STRING_PROTOCOL *)NULL;
	CHAR8 *language = NULL;	
	UINTN MaplangIndex = 0;
	
	buffer = EfiLibAllocateZeroPool(bufferSize);
	if(NULL == buffer)
		return buffer;

	status = pBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL, (VOID**)&HiiString);
	if( EFI_ERROR( status ) )
		return buffer;

	for (MaplangIndex=0; MaplangIndex < gSupportedMapLangCount;MaplangIndex++)
	{
		language = gSupprtedMapLangArr[MaplangIndex];
		
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
	}

	if ( EFI_SUCCESS != status )
		MemFreePointer( (VOID **)&buffer );

	return buffer;
}

/**
  append string to VECTOR buffer
  
  @param CHAR16* 
     
  @retval void

**/
void UpdateData(CHAR16 **String)
{
    UINTN                   iIndex = 0;
    extern UINTN            gLangCount;
    extern MULTILANG_VECTOR *gMultiLangVector;
    
    if (NULL != gMultiLangVector)
    {
        for (iIndex = 0; iIndex < gLangCount; iIndex ++)
        {
            AppendString (TRUE, gMultiLangVector [iIndex].LanguageVector, *String);
        }
    }
	MemFreePointer( (VOID **)String );
}

/**
    Returns LIST_MAPPING_STRING_ONLY token value
     
    @param void

    @retval BOOLEAN
     
**/

BOOLEAN AllowMappingStringOnly ()
{
#if LIST_MAPPING_STRING_ONLY
        return LIST_MAPPING_STRING_ONLY ;
#endif          
    return 0;
}

/**
    Returns ALLOW_DUPLICATE_MAP_ID token value
     
    @param void

    @retval BOOLEAN
     
**/
BOOLEAN AllowDuplicateMapID ()
{
#if ALLOW_DUPLICATE_MAP_ID
        return ALLOW_DUPLICATE_MAP_ID ;
#endif          
    return 0;
}



/**
    Returns TRUE of the control type is supported
     
    @param UINT8 In_ControlType

    @retval BOOLEAN
     
**/
BOOLEAN IsControlTypeSupported(UINT8 In_ControlType)
{
    BOOLEAN IsSupported = TRUE;
    UINTN Count = 0;
    UINTN Index = 0;
    CONTROL_TYPE SupportedControlArr[]= { UNSUPPORTED_CONTROL_TYPE };

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
BOOLEAN IsMapIdSupported(CHAR16 *MapString)
{
    UINT32 i = 0;
    
    MAPSTRING_NAME SuppressMapList[] = {HOOK_SUPPRESS_MAPID_LIST L""};//Array of defined MapIds
    UINT32 MapIdCount = sizeof(SuppressMapList)/sizeof(MAPSTRING_NAME);
    
    if (NULL == MapString)
    {
        return FALSE;
    }
    
    for(i = 0; i < MapIdCount; i++)
    {
        if((Wcscmp(MapString, (SuppressMapList[i].MapId)) == 0))
        {
           return FALSE; 
        }
    }
       
    return TRUE;
}
#if (0==REMOTE_CONFIG_TSE_DEPENDENCY)
/**
    Duplicates a String of type CHAR8
    @param string 
    @retval CHAR8 *
**/

CHAR8 *StrDup8( CHAR8 *string )     //TODO delete it
{
    CHAR8 *text;

    text = EfiLibAllocateZeroPool(1 + strlen( string ));
    if ( text != NULL )
        strcpy( text, string );

    return text;
}
#endif

/**
  Finds the English string and updates the vector string
    
  @param  IsUniCode			Unused 
  @param  Style				JSON_DATA_FRMT_TYPE
  @param  Lebel				Indendation spaces
  @param  LeftNameStr		Label for the option
  @param  RightValueStr		Value for the option. This structure is parsed to find English string

  @return VOID
**/

VOID GetEngJsonDataAndUpdate (BOOLEAN IsUniCode, JSON_DATA_FRMT_TYPE Style, UINTN Lebel, VOID *LeftNameStr, MULTI_LANG_STRINGS *RightValueStr)
{
    UINTN iIndex = 0;
    UINTN jIndex = 0;
    VOID *Str = NULL;
    extern UINTN            gLangCount;
    extern MULTILANG_VECTOR *gMultiLangVector;
    
    if (NULL != gMultiLangVector)
    {
        iIndex = FindEnglishString (RightValueStr);
        for (jIndex = 0; jIndex < gLangCount; jIndex ++)
        {
            Str = GetJsonDataStr16 (Style,Lebel,LeftNameStr,RightValueStr[iIndex].String);
            AppendString (TRUE, gMultiLangVector [jIndex].LanguageVector, Str);
            MemFreePointer ( (VOID **)&Str);
        }
    }
}

/**
  Updates the vector string
    
  @param  IsUniCode			Unused 
  @param  Style				JSON_DATA_FRMT_TYPE
  @param  Lebel				Indendation spaces
  @param  LeftNameStr		Label for the option
  @param  RightValueStr		Lable value

  @return VOID
**/

VOID GetJsonDataAndUpdate (BOOLEAN IsUniCode, JSON_DATA_FRMT_TYPE Style, UINTN Lebel, VOID *LeftNameStr, MULTI_LANG_STRINGS *RightValueStr)
{
    UINTN iIndex = 0;
    UINTN jIndex = 0;
    VOID *Str = NULL;
    extern UINTN            gLangCount;
    extern MULTILANG_VECTOR *gMultiLangVector;
    
    if (NULL != gMultiLangVector)
    {
        for (iIndex = 0; iIndex < gLangCount; iIndex ++)
        {
            for (jIndex = 0; jIndex < gLangCount; jIndex ++)
            {
                if (!StrCmp (gMultiLangVector [iIndex].Language, RightValueStr[jIndex].StringLang))
                {
                    Str = GetJsonDataStr16 (Style,Lebel,LeftNameStr,RightValueStr[jIndex].String);
                    AppendString (TRUE, gMultiLangVector [iIndex].LanguageVector, Str);
                    MemFreePointer ( (VOID **)&Str);
                    break;
                }
            }
        }
    }
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
