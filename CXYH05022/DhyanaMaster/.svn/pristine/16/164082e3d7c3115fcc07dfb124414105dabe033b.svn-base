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

/** @file BiosAttribCreationSrc.c
    Defined interfaces to get all the information related to controls and update to attribute json buffer

**/

//---------------------------------------------------------------------------

#include <Include/AmiDxeLib.h>
#include "setupdata.h"
#include <Library/BaseLib.h>
#include <Token.h>
#include <AutoId.h>
#include <BiosAttribTypeLib.h>
#include <BiosAttribTypeLibHelper.h>
#include <Protocol/SimpleFileSystem.h>
#include "Library/MemoryAllocationLib.h"
#include "boot.h"
#include <Include/AMIVfr.h>
#include <TseCommon.h>
#include <Token.h>
#include <Include/stddef.h>
#include <sys/types.h>
#include <string.h>
#include "PwdLib.h"
#include <Library/DebugLib.h>
#include "Library/AmiJsonLib.h"
#include "ConditionalExpressionApi.h"

//---------------------------------------------------------------------------

#define swprintf 			SPrint


MAP_ID_LIST                     *gMapIdList = (MAP_ID_LIST*)NULL;
MULTILANG_VECTOR                *gMultiLangVector = NULL;
static BOOLEAN                  gIsTseInitialized = FALSE;
ATTRIBUTES                      *gAttributeList = NULL;
ATTRIBUTES                      *gTempAttributeList = NULL;
cJSON                           *gDependencyArray = NULL;
cJSON                           *gDependencyObject = NULL;
//<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
cJSON                           *gMenusArray = NULL;
cJSON                           *gMenusObject = NULL;
//<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->

FIRWARE_CONFIG_ATTRIB_PROTOCOL  gFirmwareConfigurationProtocol = {
                                        (GET_FIRMWARE_ATTRIB_CALLBACK)Get_BiosSetupJsonData,
                                        (GET_PASSWORD_ENCRYPTED_DATA)GetPasswordEncryptedData,
                                        ResetBiosConfig,
                                        CheckForSystemPassword,
                                        ValidateSystemPassword
                                    };

extern AMI_CONDITIONAL_MAPIDLIST_PROTOCOL  gAmiCondMapIdListProtocol;

//---------------------------------------------------------------------------
VOID MemFreePointer( VOID **ptr );
EFI_STATUS HiiFixupData();
VOID ProcessUIInitHook(VOID);
UINT16 UefiGetQuestionOffset(VOID *IfrPtr);
UINT16 UefiGetWidth(VOID *IfrPtr);
UINT8 UefiGetBaseValue(VOID *IfrPtr);
UINT64 UefiGetStepValue(VOID *IfrPtr);
UINT64 UefiGetMinValue(VOID *IfrPtr);
UINT64 UefiGetMaxValue(VOID *IfrPtr);
EFI_STATUS UefiGetOneOfOptions(CONTROL_INFO *CtrlInfo, VOID **Handle, UINT16 **OptionPtrTokens,
                               UINT64 **ValuePtrTokens, UINT16 *ItemCount,
                               UINT16 *Interactive,UINT16 *CallBackKey );
BOOLEAN IsControlTypeSupported(UINT8 In_ControlType);
CHAR16 *GetCurrentValue(CONTROL_INFO *In_ControlInfo);
BOOLEAN   IsSupportDefaultForStringControl (VOID);
CHAR16 *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR16 *lang);
UINTN atoi_base(CHAR16 *string, UINT8 base);
ssize_t  EncodeUtf8(char *Dest, wchar_t ch);
int     OneWcToMcLen(const wchar_t InCh);
EFI_STATUS CreateIndexJsonFile (OUT CHAR8 **Out_JsonBuffer,OUT UINTN *Out_JsonbufSize);
EFI_STATUS  Uefi21HiiInitializeProtocol(VOID);
//<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option. +>
const EFI_GUID ServerMgmtConfigGuid = { 0x1239999, 0xfc0e, 0x4b6e, { 0x9e, 0x79, 0xd5, 0x4d, 0x5d, 0xb6, 0xcd, 0x20 } };
extern BOOLEAN EFIAPI CompareGuid (IN CONST GUID  *Guid1,IN CONST GUID  *Guid2);
//<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option. ->
//---------------------------------------------------------------------------

extern PAGE_LIST			*gPages;
extern UINT8				*gApplicationData;
extern CONTROL_INFO			*gControlInfo;
extern UINTN		      	gLangCount;
extern LANGUAGE_DATA	    *gLanguages;
extern BOOT_DATA			*gDriverData;
extern UINTN				gDriverOptionCount;
extern EFI_FILE_PROTOCOL 	*gJsonFilePtr;
extern EFI_BOOT_SERVICES 	*gBS;

extern NVRAM_VARIABLE	*gVariableList;
extern CHAR16 BiosSetupDataStr[][128];
//extern EFI_GUID gFirmwareConfigAttribGuid;
extern BOOLEAN              gBrowserCallbackEnabled;
extern EFI_HII_HANDLE 	gHiiHandle;
extern NVRAM_VARIABLE	*gOptimalDefaults;
extern BOOLEAN gRedfishEnabled;


//<kuaibai-20190315 Fix cannot enter setup with IPMI cmd on first boot +>
#include "bootflow.h"
extern BOOLEAN gEnterSetup;
VOID
judgeBootFlow()
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN      Size = 0;
  UINT32     BootFlow = 0;
  EFI_GUID   BootFlowGuid = BOOT_FLOW_VARIABLE_GUID;

  Size = sizeof(BootFlow);
  Status = pRS->GetVariable (
                L"BootFlow",
                &BootFlowGuid,
                NULL,
                &Size,
                &BootFlow
                );
  if (!EFI_ERROR(Status) && (BootFlow==BOOT_FLOW_CONDITION_FIRST_BOOT))
  {
    gEnterSetup = TRUE;
    DEBUG((DEBUG_INFO, "\n[SK] gEnterSetup set TRUE"));
    return;
  }
  DEBUG((DEBUG_INFO, "\n[SK] BootFlow:%x",BootFlow));
  return;
}
//<kuaibai-20190315 Fix cannot enter setup with IPMI cmd on first boot ->
//---------------------------------------------------------------------------

/**
  Initializes all the Tse methods to access gPageInfo.

  @param VOID 

  @retval VOID

**/
VOID InitializeTseMethods()
{
#if (0==REMOTE_CONFIG_TSE_DEPENDENCY)
    EFI_STATUS Status = EFI_SUCCESS;
    
    Status = Uefi21HiiInitializeProtocol ();
    if ( EFI_ERROR ( Status ) )
       return;
    InitApplicationData ();
    
#else
    if (FALSE == gIsTseInitialized)
    {
      judgeBootFlow();//<kuaibai-20190315 Fix cannot enter setup with IPMI cmd on first boot>
      gLangCount = 1;
        HiiFixupData ();
        ProcessUIInitHook ();
        BootGetLanguages ();
        BootGetBootOptions();
        VarBuildDefaults();
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB>        VarLoadVariables( (VOID **)&gVariableList, gOptimalDefaults ); //kuaibai add update 
        gIsTseInitialized = TRUE;
    } 
    return;//<kuaibai-20190315 Fix cannot enter setup with IPMI cmd on first boot>
#endif
}
/**
  Check for popup String control

  @param CONTROL_TYPE 

  @retval BOOLEAN

**/

BOOLEAN IsStringControl(CONTROL_TYPE  ControlType)
{
	BOOLEAN IsStrControl = FALSE;
	if(_CONTROL_TYPE_POPUP_STRING == ControlType)
	{
		IsStrControl = TRUE;
	}
	return IsStrControl;
}

/**
  initialize Attribute data 

  @param ATTRIBUTES* 

  @retval void

**/

void Init_AttributeData(ATTRIBUTES      *DestAttributes)
{
		DestAttributes->_AttributeName = (CHAR16*)NULL;
		DestAttributes->_DisplayName = NULL;
		DestAttributes->_HelpText = NULL;
		DestAttributes->_Value = (VALUE*)NULL;		
		DestAttributes->_WarningText = (CHAR16*)NULL;	
		DestAttributes->_UpperBoundStr = (CHAR16*)NULL;
		DestAttributes->_LowerBoundStr = (CHAR16*)NULL;
		DestAttributes->_ScalarIncrementStr = (CHAR16*)NULL;
		DestAttributes->_MinLengthStr = (CHAR16*)NULL;
		DestAttributes->_MaxLengthStr = (CHAR16*)NULL;
		DestAttributes->_DefaultValue = (CHAR16*)NULL;
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
        DestAttributes->_MenuName = (CHAR16*)NULL;
        DestAttributes->_MenuPath = (CHAR16*)NULL;
        DestAttributes->_DisplayOrder = (CHAR16*)NULL;
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->
}

/**
  check for unsigned integer 

  @param CONTROL_INFO* 

  @retval UINT8

**/

UINT8 IsUnSignedInteger(CONTROL_INFO *In_ControlInfo)
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

/**
  get current value from control info

  @param CONTROL_INFO* 

  @retval CHAR16

**/

CHAR16 *GetCurrentValue(CONTROL_INFO *In_ControlInfo)
{
	UINT32 size = 0;
	UINT32 offset = 0;
	void *CtrlValue = ( void *)NULL;
	CHAR16  *DefaultValue = (CHAR16*)NULL;
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR16 *OnOffStr[] = {L"false",L"true"};
	UINT8 Base= 0; 
	UINTN  NumericValue = 0;
	
	if (VARIABLE_ID_LANGUAGE == In_ControlInfo->ControlVariable)
	{
		return DefaultValue;
	}
	offset = UefiGetQuestionOffset(In_ControlInfo->ControlPtr);
	size = UefiGetWidth(In_ControlInfo->ControlPtr);	
	
	if(size)
	{
		CtrlValue = (UINT8 *)EfiLibAllocateZeroPool (size + 10); //+10 is for safety
		
		if ((gOptimalDefaults[In_ControlInfo->ControlVariable].Buffer != NULL) && (gOptimalDefaults[In_ControlInfo->ControlVariable].Size))
		{
			MemCopy (CtrlValue, gOptimalDefaults [In_ControlInfo->ControlVariable].Buffer + offset, size);
		}
		if(EFI_SUCCESS == Status)
		{
			if(NULL != CtrlValue)
			{
				if( CONTROL_TYPE_CHECKBOX == In_ControlInfo->ControlType )
				{
					NumericValue = *((UINTN*)CtrlValue);
					DefaultValue  = EfiLibAllocateZeroPool(128 * sizeof(CHAR16));
					if(NumericValue < 2)
						swprintf( DefaultValue , 128 , L"%s",OnOffStr[NumericValue]);
					else
						swprintf( DefaultValue , 128 , L"%s",OnOffStr[0]);
				}
				else
				{		
					if( TRUE == IsStringControl(In_ControlInfo->ControlType) )
					{	
						DefaultValue  = EfiLibAllocateZeroPool( ((size+1) * sizeof(CHAR16)) );  
						if(TSE_SUPPORT_DEFAULT_FOR_STRING_CONTROL)
							MemCopy (DefaultValue, CtrlValue, size);
					}
					else
					{
						Base = IsUnSignedInteger(In_ControlInfo);
						DefaultValue = ConvVfrNumToString(*((UINT64*)CtrlValue),size,Base);
					}
				}
			}
		}
		MemFreePointer(&(CtrlValue));
	}

	return DefaultValue;
}

/**
  get config data from control info

  @param ATTRIBUTES* 
  @param CONTROL_INFO* 

  @retval void

**/

void get_ConfigData(ATTRIBUTES      *Attributes,CONTROL_INFO 	*In_CtrlInfo)
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
		Base = IsUnSignedInteger(In_CtrlInfo);
		UpperBoundStr = ConvVfrNumToString((UINT64)UefiGetMaxValue(In_CtrlInfo->ControlPtr),size,Base);
		
		size = UefiGetWidth(In_CtrlInfo->ControlPtr);
		Base = IsUnSignedInteger(In_CtrlInfo);
		LowerBoundStr = ConvVfrNumToString((UINT64)UefiGetMinValue(In_CtrlInfo->ControlPtr),size,Base);
		Attributes->_UpperBoundStr   = UpperBoundStr;
		Attributes->_LowerBoundStr   = LowerBoundStr;
		Attributes->_ScalarIncrementStr = ScalarIncrementStr;	
	}
}



/**
  update attribute value from control info

  @param ATTRIBUTES* 
  @param VOID* 
  @param CONTROL_INFO* 

  @retval BOOLEAN

**/

BOOLEAN get_AttributeValue(ATTRIBUTES      *Attributes,VOID *PageHandle,CONTROL_INFO *In_ControlInfo)
{
	VALUE* Value = (VALUE*)NULL;
	UINT64 	*ValueList=NULL;
	UINT16 	OptionCount = 0;
	UINT16 	*OptionList=NULL;
	UINTN   Index = 0;
	CHAR16  *ValueName = (CHAR16*)NULL;
	BOOLEAN IsSkipped = FALSE;
	BOOLEAN DefaultUpdated = FALSE;
    GUID_INFO *guidInfo = (GUID_INFO*)NULL;
    UINTN       jIndex = 0;
    MULTI_LANG_STRINGS  *ValueDisplayName = NULL;
    
    if( CONTROL_TYPE_NUMERIC == In_ControlInfo->ControlType )
    {
    	return FALSE;
    }
    
    //Condition to skip boot order control
    if ((UINT16)-1  == _SpecialGetValue (In_ControlInfo, &guidInfo) )
    {
        UINT32 GuidKey = 0;

        GuidKey = GetControlGuidType(In_ControlInfo);
		// Handle boot options. Set type as string and free unwanted text.
        if(GUID_KEY_BOOT_NOW == GuidKey)
        {
            CHAR16 DisplayName[] = L"Boot Option Priorities";
            EFI_GUID BootOptNameGuid = {0x5b5f93b7, 0x8f58, 0x47ce, 0x80, 0x3b, 0x48, 0x23, 0x6e, 0x80, 0x31, 0xaf};
            UINT32 Attrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
            UINTN Size = StrLen(Attributes->_AttributeName)*sizeof(CHAR16)+2;
            CHAR16 *BootOptionMapId = NULL; 
    
            // save mapId of boot options to use while creating option list and during json import
            BootOptionMapId = EfiLibAllocateZeroPool( Size );
            if ( NULL != BootOptionMapId)
            {
                StrCpy(BootOptionMapId, Attributes->_AttributeName);
                pRS->SetVariable (L"BootOptMapId", &BootOptNameGuid, Attrib, Size, BootOptionMapId); 
                MemFreePointer(&BootOptionMapId);
            }
    
            // Set Display name as "Boot Option Priorities"
            FreeMultiLangStrings (Attributes->_DisplayName);
            Attributes->_DisplayName = NULL;
            Attributes->_DisplayName = FormMultiLangStrings (NULL, 0, DisplayName, 0);
            FreeMultiLangStrings (Attributes->_HelpText);
            Attributes->_HelpText = NULL;
            MemFreePointer(&(Attributes->_DefaultValue));
            Attributes->_Type = _CONTROL_TYPE_POPUP_STRING;
            Attributes->_MaxLengthStr = EfiLibAllocateZeroPool( 5 *sizeof(CHAR16));
            StrCpy(Attributes->_MaxLengthStr, L"8192");           
            return IsSkipped;
        }

        if (!(GUID_KEY_LANG_CODES == GuidKey))
        {
            return TRUE;
        }
    }
    
	if( CONTROL_TYPE_CHECKBOX == In_ControlInfo->ControlType )
	{
		Value =EfiLibAllocateZeroPool( 2 *sizeof(VALUE));
		if(NULL != Value)
		{
			CHAR16 *DisplayStr[] = {L"Disabled",L"Enabled"};
			CHAR16 *OnOffStr[] = {L"false",L"true"};
			for( Index = 0; Index < 2; Index++)
			{
			    Value[Index]._ValueDisplayName = FormMultiLangStrings (NULL, 0, DisplayStr [Index], 0);
				ValueName = EfiLibAllocateZeroPool(128 * sizeof(CHAR16));
				swprintf( ValueName, 128 , L"%s",OnOffStr[Index]);
				Value[Index]._ValueName =  ValueName;
			}
		}	
		OptionCount =2;
	}
	else
	{
		UefiGetOneOfOptions( In_ControlInfo, &In_ControlInfo->ControlHandle, &OptionList, &ValueList, &OptionCount,NULL,NULL);
		if(NULL != OptionList)
		{
			Value =EfiLibAllocateZeroPool( OptionCount *sizeof(VALUE));
			if(NULL != Value)
			{
				for( Index = 0; Index < OptionCount; Index++)
				{
				    ValueDisplayName = FormMultiLangStrings (PageHandle, OptionList [Index], NULL, 0);
				    
				    if (NULL != ValueDisplayName)
				    {

					    Value[Index]._ValueDisplayName = ValueDisplayName; 
				    }
				    if (NULL == Value[Index]._ValueDisplayName)
				    {
				        continue;
				    }
					ValueName = EfiLibAllocateZeroPool(128 * sizeof(CHAR16));
					swprintf( ValueName, 128 , L"%d",ValueList[Index]);
					Value[Index]._ValueName =  ValueName;
					if( (NULL != Attributes->_DefaultValue ) && (DefaultUpdated == FALSE) &&
					        (ValueList[Index] == atoi_base(Attributes->_DefaultValue, 10)) )
					{
					    CHAR16  *DefaultValue = (CHAR16*)NULL;
					    UINTN Size = 0;
					    jIndex = FindEnglishString (Value[Index]._ValueDisplayName);
					    Size = StrLen (Value[Index]._ValueDisplayName[jIndex].String);
					    
					    //Update Enumeration control default as string 
					    if(Attributes->_DefaultValue)
					        FreePool(Attributes->_DefaultValue);
					    
					    DefaultValue= EfiLibAllocateZeroPool( (Size+1)* sizeof(CHAR16) );
					    swprintf( DefaultValue, (Size*sizeof(CHAR16)) , L"%s",Value[Index]._ValueDisplayName[jIndex].String);
					    Attributes->_DefaultValue = DefaultValue;
					    DefaultUpdated = TRUE;
					}
				}
			}
		}		
	}
		
	Attributes->_Value = Value;
	Attributes->_ValueCount = OptionCount;

	MemFreePointer(&(OptionList));
	MemFreePointer(&(ValueList));
	return IsSkipped;
}

/**
  check for duplicate map id

  @param CHAR16* 

  @retval BOOLEAN

**/

BOOLEAN IsDuplicateMapId(CHAR16 *MapID)
{
	
	MAP_ID_LIST *Temp = (MAP_ID_LIST*)NULL;
	MAP_ID_LIST *Visit = (MAP_ID_LIST*)NULL;
	MAP_ID_LIST *Prev = (MAP_ID_LIST*)NULL;
	BOOLEAN DuplicateMapIdStatus = FALSE;
	
	if( NULL == MapID )
		return TRUE;
	if(NULL == gMapIdList)
	{
		gMapIdList = EfiLibAllocateZeroPool(sizeof(MAP_ID_LIST));
		if(gMapIdList)
		{
			gMapIdList->Next =(MAP_ID_LIST*)NULL;
			gMapIdList->MapID = EfiLibAllocateZeroPool( (StrLen(MapID)+1)*2);
			StrCpy(gMapIdList->MapID,MapID);
		}
	}
	else
	{
		for(Visit = gMapIdList;Visit!= NULL;Visit=Visit->Next)
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
  Validates the control with its mapping ID

  @param 	In_CtrlInfo			CONTROL_INFO to validate whether its valid control with valid mapping ID	 
  @param 	In_PageHandle		CONTROL_INFO's page handle 
  @param 	In_DisplayName		Multilanguage string need to be filled by this function 
  @param 	In_AttributeName	Mapping ID name filled by this function 
  
  @return 	BOOLEAN
  @retval 	TRUE 				If control is having valid mapping ID
  @retval  	TRUE 				If control is not having valid mapping ID

**/

BOOLEAN ValidateMapId (CONTROL_INFO *In_CtrlInfo, EFI_HANDLE In_PageHandle, MULTI_LANG_STRINGS **In_DisplayName, CHAR16 **In_AttributeName)
{
    MULTI_LANG_STRINGS  *DisplayName = NULL;
    CHAR16              *TempDisplayName = (CHAR16*)NULL;
	CHAR16              *AttributeName = (CHAR16*)NULL;
	BOOLEAN 		    ValidateStatus = FALSE;
	
	if( TRUE == IsControlTypeSupported((UINT8)In_CtrlInfo->ControlType) )
	{
#if REMOTE_COND_EXPRESSION_SUPPORT	 
		VARIABLE_INFO *varInfo = NULL;
		   
	    varInfo = VarGetVariableInfoIndex(In_CtrlInfo->ControlVariable);
	    if ((NULL != varInfo) && (!StrCmp (varInfo->VariableName, L"SystemAccess")))
	    {
	        *In_AttributeName = EfiLibAllocateZeroPool (20 * sizeof(CHAR16));
	        if (NULL != *In_AttributeName)
	        {
	            StrCpy (*In_AttributeName, SYSTEM_ACCESS_MAPPINGID);
	            *In_DisplayName = FormMultiLangStrings (NULL, 0, L"System Access", 0);
	        }
	        return TRUE;
	    }
#endif	    
		AttributeName = GetActualHiiString(In_PageHandle,In_CtrlInfo->QuestionId);
		
        if (NULL != AttributeName)
        {
            if (TRUE == CheckForSpaceInMappingId (AttributeName))
            {
                MemFreePointer (&AttributeName);
                return FALSE;
            }
        }
		if( (AllowDuplicateMapID()) || (FALSE == IsDuplicateMapId(AttributeName)) )					
		{
			if(AllowMappingStringOnly())					
			{
				if( (NULL ==AttributeName) ||(!StrLen(AttributeName)))
				{
					MemFreePointer(&AttributeName);	
				}
				else
					ValidateStatus = TRUE;
			}
			else
			{
				TempDisplayName = Uefi21HiiGetStringLanguage(In_PageHandle,In_CtrlInfo->QuestionId,(CHAR16*)NULL);
				if( (FALSE == In_CtrlInfo->ControlFlags.ControlVisible) || (NULL ==TempDisplayName) ||(!StrLen(TempDisplayName)))
				{
					MemFreePointer(&AttributeName);
				}
				else
				{
					MemFreePointer (&TempDisplayName);
					ValidateStatus = TRUE;
				}
			}
			if (TRUE == ValidateStatus)
			{
			    DisplayName = FormMultiLangStrings (In_PageHandle, In_CtrlInfo->QuestionId, NULL, 0);
			}
			*In_DisplayName = DisplayName;
			*In_AttributeName = AttributeName;
		}
	}
	return ValidateStatus;
}

/**
    Convert CHAR String to CHAR16 string

    @param CHAR8*

    @retval CHAR16*
**/

CHAR16 *RemoteConvertChar8toChar16( CHAR8 *string )
{
    CHAR16  *text;
    UINTN   i;

    text = EfiLibAllocateZeroPool( (1 + strlen( string )) * sizeof(CHAR16));
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
    }

    return text;
}

static unsigned parse_hex4(const char *str)
{
    unsigned h=0;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    h=h<<4;str++;
    if (*str>='0' && *str<='9') h+=(*str)-'0'; else if (*str>='A' && *str<='F') h+=10+(*str)-'A'; else if (*str>='a' && *str<='f') h+=10+(*str)-'a'; else return 0;
    return h;
}

/**
    Parse string and remove illegal json characters
	
    @param CHAR8*

    @retval CHAR8*
**/

char *parse_string(const char *str)
{
    char *ptr;char *ptr2;char *out;int len=0;unsigned uc,uc2;
    const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

    ptr = str;
    while (*ptr && ++len) if (*ptr++ == '\\')ptr++;;  /* Skip escaped quotes. */
    out=(char*)EfiLibAllocateZeroPool(len+1); /* This is how long we need for the string, roughly. */
    if (!out) return 0;

    // Skip " and \ Allow \",\\,\b,\f,\n,\r,\t,\uWXYZ, where WXYZ are 4 Hexadecimal digits
    ptr = str;ptr2=out;
    while (*ptr)
    {
        if ( (*ptr!='\\') && (*ptr!='"')) *ptr2++=*ptr++;
        else
        {
            ptr++;
            if( '\0' == *ptr )
                break;
            switch (*ptr)
            {
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
                case 'u':    /* transcode utf16 to utf8. */
                uc=parse_hex4(ptr+1);ptr+=4;    /* get the unicode char. */
                
                if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)    break;  /* check for invalid.   */
                
                if (uc>=0xD800 && uc<=0xDBFF)   /* UTF16 surrogate pairs.   */
                {
                    if (ptr[1]!='\\' || ptr[2]!='u')    break;  /* missing second-half of surrogate.    */
                    uc2=parse_hex4(ptr+3);ptr+=6;
                    if (uc2<0xDC00 || uc2>0xDFFF)       break;  /* invalid second-half of surrogate.    */
                    uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                }
                
                len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;
                
                switch (len) {
                case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                case 1: *--ptr2 =(uc | firstByteMark[len]);
                }
                ptr2+=len;
                break;
                default:  *ptr2++=*ptr; break;
            }
            ptr++;
        }
    }
    return out;
}

/**
    Removes the illegal chars in json for the incoming string

    @param CHAR16 ** -> String to which illegal chars will be removed and returned in same.
    
    @retval EFI_STATUS
**/
EFI_STATUS RemoveSpecialChars (CHAR16 **String)
{
    char    *str, *out;
    CHAR16  *TempStr = *String, ch;
    UINTN   NumChar = 0;
    UINTN 	iIndex = 0;

    //Skip the characters \r and \n
	while ((*String) [iIndex] != L'\0')
	{
		if (((*String) [iIndex] == L'\r' ) || ((*String) [iIndex] == L'\n'))
			(*String) [iIndex] = L' ';
		
		iIndex ++;
	}
	
    //Estimate bytes to represent unicode chars and calculate string length
    for ( ;*TempStr; TempStr++)
    {
        if (*TempStr > 0x80)
            NumChar += OneWcToMcLen(*TempStr);
        else
            NumChar++;
    }
        
    str = EfiLibAllocateZeroPool(NumChar+1);
    if (!str) return EFI_OUT_OF_RESOURCES;
    out = str;
    
    //Convert unicode char into multibyte character
    for ( TempStr = *String; *TempStr; TempStr++ )
    {
        ch = *TempStr;
        if (ch < 0x80)
            *str++ = (char)ch;
        else
        {
            NumChar = EncodeUtf8(str,ch);
            str+= NumChar;
        }
    }

     str = out;
	 out = parse_string(str);
	 FreePool (*String);
	 *String = RemoteConvertChar8toChar16(out);
	 MemFreePointer(&str);	 
	 MemFreePointer(&out);
	 return EFI_SUCCESS;
}

//<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
/**
    Removes space for the incoming string

    @param CHAR16 ** -> String to which chars will be removed and returned in same.
    
    @retval EFI_STATUS
**/
EFI_STATUS RemoveSpaceChars (CHAR16 *String)
{
  CHAR16 *tmp = String;
  UINTN  Index = 0;

  while(*tmp) {
    if (*tmp != ' ')
      String[Index++] = *tmp;
    tmp++;
  }
  String[Index] = L'\0';

  return EFI_SUCCESS;
}
//<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->

/**
    Fills the Attribute registry fields

    @retval void
**/
void Get_BiosAttributes(){
	UINTN 			    PageIndex = 0;
	UINTN 			    ControlIndex = 0;
	PAGE_INFO 		    *PageInfo = (PAGE_INFO*)NULL;
	CONTROL_INFO 	    *CtrlInfo = (CONTROL_INFO *)NULL;
	ATTRIBUTES          *Attributes;
	MULTI_LANG_STRINGS  *DisplayName = NULL;
	CHAR16              *AttributeName   = (CHAR16*)NULL;
	CHAR16              *FormattedDisplayName   = (CHAR16*)NULL;
	MULTI_LANG_STRINGS  *FormattedHelpText = NULL;
	CHAR16              *TempFormattedHelpText   = (CHAR16*)NULL;
	EFI_STATUS          Status = EFI_SUCCESS;
    EFI_GUID            FormGuid;
    BOOLEAN             ReadOnly = FALSE;
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
    CHAR16              *MenuName = (CHAR16*)NULL;//<libj-20220234 fix OOB assert hang 92>
    CHAR16              *DisplayOrder = (CHAR16*)NULL;
    CHAR16              *MenuPath = (CHAR16*)NULL;
    UINTN               Size = 0;
    ATTRIBUTES          *TempAttributes;
    BOOLEAN             IsHide = FALSE; //<Yaosr-20210312 Improve redfish: Add Hide attribute.>
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->

	for ( PageIndex = 1; PageIndex < (UINTN)gPages->PageCount; PageIndex++ ) 
	{
	    CHAR16          tempBuffer[4096] = {L""};

        PageInfo = (PAGE_INFO*)( (UINTN)gApplicationData + gPages->PageList[PageIndex] );
		if( NULL == PageInfo->PageHandle )   //ignoring setup
			continue;

	    Status = GetPageGuidFromPgIndex (PageInfo->PageID, &FormGuid);
	    if (EFI_ERROR (Status))
	    {
	        continue;
	    }


		for ( ControlIndex = 0; ControlIndex < PageInfo->PageControls.ControlCount; ControlIndex++ )
		{
		    ReadOnly = FALSE;
		    IsHide = FALSE; //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web.>
			CtrlInfo = (CONTROL_INFO *)((UINT8 *)(gControlInfo) + PageInfo->PageControls.ControlList[ControlIndex]);
			if (NULL == CtrlInfo)
			{
			    continue;
			}

            //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
            // Do not send Main/Save&Exit
            if ( (PageIndex == 1) && ( (ControlIndex == 0) || (ControlIndex == 5) ) ) {
              continue;
            }
            //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->


#if SUPPRESS_COND_EXPRESSION_CONTROLS
             if(( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr) != NULL )
                 if(( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_SUPPRESS_IF_OP
                          || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_NO_SUBMIT_IF_OP
                          || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_INCONSISTENT_IF_OP
                          || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_GRAY_OUT_IF_OP
                          || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_DISABLE_IF_OP
                          || ( (EFI_IFR_OP_HEADER*)CtrlInfo->ControlConditionalPtr)->OpCode == EFI_IFR_WARNING_IF_OP )
                      continue;
#endif
             
			if(FALSE == ValidateMapId(CtrlInfo,PageInfo->PageHandle,&DisplayName,&AttributeName))
				continue;

			//Check to export the MapId or not
			 if( FALSE ==  IsMapIdSupported(AttributeName ))
//<Wangbn001-20200106-Fixed memory leak issue +>
			 {

			     if(AttributeName)
			         FreePool(AttributeName);
			     FreeMultiLangStrings(DisplayName) ;
			     continue;
			 }
//<Wangbn001-20200106-Fixed memory leak issue ->

			
//			Init_AttributeData(Attributes);
             Attributes = EfiLibAllocateZeroPool (sizeof (ATTRIBUTES));
             if (NULL == Attributes)
             {
                 continue;
             }
			Attributes->_AttributeName = AttributeName;

			if (NULL != DisplayName)
			{
				Attributes->_DisplayName = DisplayName;
			}
			FormattedHelpText = FormMultiLangStrings (PageInfo->PageHandle,CtrlInfo->ControlHelp, NULL, 0);
			if (NULL != FormattedHelpText)
			{

			    Attributes->_HelpText = FormattedHelpText;
			}
			Attributes->_ReadOnly     = CtrlInfo->ControlFlags.ControlReadOnly;
			Attributes->_Type         = (CONTROL_TYPE)CtrlInfo->ControlType;
			Attributes->_WarningText = NULL;
			Attributes->_DefaultValue = NULL;
			Attributes->ControlKey = CtrlInfo->ControlKey;
			MemCpy (&(Attributes->PageGuid), &FormGuid, sizeof (EFI_GUID));

            //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
            Attributes->_GrayOut     = FALSE;
            Attributes->_Hidden      = FALSE;
            //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->


			get_ConfigData(Attributes,CtrlInfo);
			if( _CONTROL_TYPE_PASSWORD != (CONTROL_TYPE)CtrlInfo->ControlType )
			    Attributes->_DefaultValue = GetCurrentValue(CtrlInfo);	
			
            //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
            // Add MenuName.
            if (Attributes->_Type == 0x1) {//Submenu
              Size = StrLen(DisplayName->String) * sizeof(CHAR16) + 2;
              MenuName = EfiLibAllocateZeroPool( Size );

              if (NULL != MenuName) {
                StrCpy(MenuName, DisplayName->String);
                RemoveSpaceChars(MenuName);
                Attributes->_MenuName = MenuName;
              }
            }

            //Add DisplayOrder.
            DisplayOrder  = EfiLibAllocateZeroPool(2 + sizeof(CHAR16));
            Swprintf( DisplayOrder, L"%d", ControlIndex);
            Attributes->_DisplayOrder = DisplayOrder;

            //Add MenuPath. +
            Attributes->_ControlDestPageID = CtrlInfo->ControlDestPageID;
            if (PageIndex == 1) {
              Attributes->_MenuPath = L"./";
            } else {
              //Judge PageIndex == ControlDestPageID
              gTempAttributeList = gAttributeList;
              while (gTempAttributeList) {
                if (gTempAttributeList->_ControlDestPageID == PageIndex) {
                  if ((gTempAttributeList->_MenuPath != NULL) && (gTempAttributeList->_MenuName != NULL)) {
                    Size = (StrLen(gTempAttributeList->_MenuPath) + StrLen(gTempAttributeList->_MenuName)) * sizeof(CHAR16) + 2;
                    MenuPath = EfiLibAllocateZeroPool( Size );
                    if (gTempAttributeList->_MenuPath == L"./")
                      Swprintf( MenuPath, L"%s%s", gTempAttributeList->_MenuPath, gTempAttributeList->_MenuName);
                    else
                      Swprintf( MenuPath, L"%s/%s", gTempAttributeList->_MenuPath, gTempAttributeList->_MenuName);
                    Attributes->_MenuPath = MenuPath;
                    //FreePool(MenuPath);
                  }
                  break;
                }
                gTempAttributeList = gTempAttributeList->Next;
              }
              gTempAttributeList = TempAttributes;
            }
            //<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option. +>
            if (CompareGuid(&FormGuid, (EFI_GUID *)&ServerMgmtConfigGuid) && (Attributes->_MenuPath == NULL)) {
              Attributes->_MenuPath = L"./ServerMgmt";
            }
            //<libj-20220234 fix OOB assert hang 92+>
            if(MenuName != NULL)
            {
                if (!StrCmp(MenuName, L"MokshaCommonOptions") || !StrCmp(MenuName, L"DFCommonOptions") || !StrCmp(MenuName, L"UMCCommonOptions") || !StrCmp(MenuName, L"NBIOCommonOptions") || !StrCmp(MenuName, L"FCHCommonOptions")) {
                    Attributes->_MenuPath = L"./HygonCbs";
                    MenuName = L" ";
                }
            }
            //<libj-20220234 fix OOB assert hang 92->
            //<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option. ->
            //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->

			if(TRUE == get_AttributeValue(Attributes,PageInfo->PageHandle,CtrlInfo))
			{
			    MemFreePointer (&Attributes);
			    continue;
				//WriteAttributesToFile(&Attributes);
			}
#if REMOTE_COND_EXPRESSION_SUPPORT
			if (NULL != CtrlInfo->ControlConditionalPtr && (StrCmp (AttributeName, SYSTEM_ACCESS_MAPPINGID)) && (StrCmp (AttributeName, BOOT_ORDER_MAPPINGID)))
            {
                //<Yaosr-20210312 Improve redfish: Add Hide attribute. +>
                Status = AddControlDependencies (CtrlInfo, &(Attributes->Dependencies), Attributes->_AttributeName, &ReadOnly, &IsHide);
                if (IsHide == TRUE) {
                  Attributes->_Hidden = TRUE;
                }
                //<Yaosr-20210312 Improve redfish: Add Hide attribute. ->

			    if (EFI_ERROR (Status))
			    {
			        DEBUG((DEBUG_ERROR, "\nFailed to evaluate dependency for %S", Attributes->_AttributeName));
			        continue;
			    }
			    if (TRUE == ReadOnly)      //constant conditional expression might return TRUE eg., grayoutif TRUE
			    {
			        Attributes->_ReadOnly = ReadOnly;
			    }
            }
			AddMappingIdToList (AttributeName, CtrlInfo->ControlKey, FormGuid, Attributes);
#endif			
			if (NULL == gAttributeList)
			{
			    //<Wangbn001-20200106-Fixed memory leak issue +>
			    //              gAttributeList = EfiLibAllocateZeroPool (sizeof (ATTRIBUTES));
			    //              if (NULL != gAttributeList)
			    //              {
			                        gAttributeList = Attributes;
			    //              }
			    //<Wangbn001-20200106-Fixed memory leak issue ->
	            gTempAttributeList = gAttributeList;
	            TempAttributes = Attributes;//<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web.>

			}
			else
			{
			    gTempAttributeList->Next = Attributes;
			    gTempAttributeList = Attributes;
                TempAttributes = Attributes;//<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web.>
			}
			DisplayName = NULL;
			AttributeName = NULL;
		}
	}
	gTempAttributeList = gAttributeList;
	Status = EFI_SUCCESS;
	while (gTempAttributeList)
	{
#if REMOTE_COND_EXPRESSION_SUPPORT	
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
        if ((gTempAttributeList->_MenuPath != L"./") && (gTempAttributeList->_MenuPath != NULL)) {//yaosr debug
	    Status = WriteDependencyToFile (gTempAttributeList);
        }
	    //if (!EFI_ERROR (Status))
	    //{
	        gComplexValue = gTempAttributeList->ComplexValue;           //Don't want to modify the interface so added the global variable
	        AddConditionalMapIdToList(gTempAttributeList->_AttributeName);
#endif
	        //WriteAttributesToFile (gTempAttributeList);
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->
#if REMOTE_COND_EXPRESSION_SUPPORT
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
//	    }
//	    else if (EFI_NOT_READY == Status)
//	    {
//	        gComplexValue = gTempAttributeList->ComplexValue;           //Don't want to modify the interface so added the global variable
//	        AddConditionalMapIdToList(gTempAttributeList->_AttributeName);
//	    }
//#if AMIUTIL_COMPLEX_EVAL_SUPPORT
//	    else
//	    {
//	        if ((_CONTROL_TYPE_SUBMENU != gTempAttributeList->_Type) && (_CONTROL_TYPE_TEXT != gTempAttributeList->_Type) && (_CONTROL_TYPE_MEMO != gTempAttributeList->_Type)) {
//	            if (gTempAttributeList->_AttributeName && (StrCmp (gTempAttributeList->_AttributeName, SYSTEM_ACCESS_MAPPINGID))) {
//	                gComplexValue = 1;           //Don't want to modify the interface so added the global variable
//	                AddConditionalMapIdToList(gTempAttributeList->_AttributeName);
//	                WriteAttributesToFile (gTempAttributeList);
//	            }
//	        }
//	    }
//#endif	    
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
#endif	
        //<Yaosr-20210312 Improve redfish: diplay bios options according to the BIOS setup hierarchy on the BMC web. +>
        if ((gTempAttributeList->_ControlDestPageID == 0xFFFF) && (StrCmp (gTempAttributeList->_AttributeName, SYSTEM_ACCESS_MAPPINGID)) /*&& (gTempAttributeList->_MenuPath != NULL)*/) {
          WriteAttributesToFile (gTempAttributeList);
        }
        //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->
        //<Yaosr-20210312 Improve redfish: Add Menus array. +>
        if ((gTempAttributeList->_ControlDestPageID != 0xFFFF) && (gTempAttributeList->_MenuPath != NULL)) {
          Status = WriteMenusToFile (gTempAttributeList);
        }
        //<Yaosr-20210312 Improve redfish: Add Menus array. ->

	    gTempAttributeList = gTempAttributeList->Next;
	}
	FreeAttributeData (&gAttributeList);
	FreeConditionalMapIdList (&gMapIdList);
}
//<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option. +>
//Add CBS and ServerMgmt menus >>>
EFI_STATUS WriteMenusForCbsAndServerMgmt () {
  EFI_STATUS    Status = EFI_NOT_FOUND;
  cJSON         *MenusObject = NULL;
  cJSON         *MenusObject_1 = NULL;
  CHAR8         *String8 = NULL;
  extern cJSON  *gMenusArray;

  if (NULL == gMenusArray)
    return Status;

  MenusObject = cJSON_CreateObject ();
  String8 = StrDup16to8 (L"HYGON CBS");
  cJSON_AddStringToObject (MenusObject, "DisplayName", String8);

  cJSON_AddNumberIntToObject (MenusObject, "DisplayOrder", 6);

  cJSON_AddBoolToObject (MenusObject, "GrayOut", (int)0);
  cJSON_AddBoolToObject (MenusObject, "Hidden", (int)0);

  String8 = StrDup16to8 (L"HygonCbs");
  cJSON_AddStringToObject (MenusObject, "MenuName", String8);

  String8 = StrDup16to8 (L"./");
  cJSON_AddStringToObject (MenusObject, "MenuPath", String8);

  cJSON_AddBoolToObject (MenusObject, "ReadOnly", (int)0);
  
  //
  MenusObject_1 = cJSON_CreateObject ();
  String8 = StrDup16to8 (L"Server Mgmt");
  cJSON_AddStringToObject (MenusObject_1, "DisplayName", String8);

  cJSON_AddNumberIntToObject (MenusObject_1, "DisplayOrder", 7);

  cJSON_AddBoolToObject (MenusObject_1, "GrayOut", (int)0);
  cJSON_AddBoolToObject (MenusObject_1, "Hidden", 0);

  String8 = StrDup16to8 (L"ServerMgmt");
  cJSON_AddStringToObject (MenusObject_1, "MenuName", String8);

  String8 = StrDup16to8 (L"./");
  cJSON_AddStringToObject (MenusObject_1, "MenuPath", String8);
  MemFreePointer (&String8);
  cJSON_AddBoolToObject (MenusObject_1, "ReadOnly", (int)0);

  cJSON_AddItemToArray (gMenusArray, MenusObject);
  cJSON_AddItemToArray (gMenusArray, MenusObject_1);

  return EFI_SUCCESS;
}
//<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option. ->


/**
  Collect Bios attribute json buffer

  @retval BOOLEAN

**/

void BiosSetupDataConfig()
{	
	CHAR16 *JsonStr = (CHAR16*)NULL;
#if REMOTE_COND_EXPRESSION_SUPPORT
	CHAR8  *DependencyBuffer8 = NULL;
    CHAR16  *DependencyBuffer16 = NULL;
	UINTN   DependencyBufferLength = 0;
#endif	
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
    CHAR8   *MenusBuffer8 = NULL;
    CHAR16  *MenusBuffer16 = NULL;
    UINTN    MenusBufferLength = 0;
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->
	JsonStr = GetJsonDataStr( TRUE,JSON_OBJECT_OPEN,1,(CHAR16*)NULL,(CHAR16*)NULL);
	UpdateData(&JsonStr);
	Dump_JsonHeaderInfo();
	JsonStr = GetJsonDataStr( TRUE,JSON_OBJECT_OPEN,2,(CHAR16*)L"RegistryEntries",(CHAR16*)NULL);
	UpdateData(&JsonStr);
	JsonStr = GetJsonDataStr( TRUE,JSON_ARRAY_OPEN,3,BiosSetupDataStr[ATTRIBUTES_INDEX],(CHAR16*)NULL);
	UpdateData(&JsonStr);

#if REMOTE_COND_EXPRESSION_SUPPORT
	gDependencyObject = cJSON_CreateObject();
	gDependencyArray = cJSON_CreateArray ();
#endif	
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
    gMenusObject = cJSON_CreateObject();
    gMenusArray = cJSON_CreateArray ();
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->
    DEBUG((DEBUG_INFO, "\nGet_BiosAttributes"));
	Get_BiosAttributes ();
	DEBUG((DEBUG_INFO, "\nGet_BiosAttributes1"));
#if REMOTE_COND_EXPRESSION_SUPPORT
	cJSON_AddItemToObject (gDependencyObject, "Dependencies", gDependencyArray);
	DependencyBuffer8 = cJSON_Print (gDependencyObject);
	cJSON_Delete (gDependencyObject);
#endif	
	
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
    WriteMenusForCbsAndServerMgmt(); //<Yaosr002-20210312 Improve redfish: for show ServerMgmt and HygonCbs option.>
    cJSON_AddItemToObject (gMenusObject, "Menus", gMenusArray);
    MenusBuffer8 = cJSON_Print (gMenusObject);
    cJSON_Delete (gMenusObject);
    //<Yaosr-20210312 Improve redfish: diplay bios options according to the BIOS setup hierarchy on the BMC web. ->
	JsonStr = GetJsonDataStr( TRUE,JSON_ARRAY_CLOSE,3,(CHAR16*)NULL,(CHAR16*)NULL);
	UpdateData(&JsonStr);

#if REMOTE_COND_EXPRESSION_SUPPORT
	DependencyBufferLength = AsciiStrLen (DependencyBuffer8);
	DependencyBuffer8 [0] = ','; //replacing { with , and } with space
	DependencyBuffer8 [DependencyBufferLength - 1] = ' ';
	DependencyBuffer16 = StrDup8to16 (DependencyBuffer8);
	UpdateData(&DependencyBuffer16);
	//MemFreePointer (&DependencyBuffer8);
#endif	
	
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. +>
    MenusBufferLength = AsciiStrLen (MenusBuffer8);
    MenusBuffer8 [0] = ','; //replacing { with , and } with space
    MenusBuffer8 [MenusBufferLength - 1] = ' ';
    MenusBuffer16 = StrDup8to16 (MenusBuffer8);
    UpdateData(&MenusBuffer16);
    //<Yaosr-20210312 Improve redfish: display bios options according to the BIOS setup hierarchy on the BMC web. ->
	
	JsonStr = GetJsonDataStr( TRUE,JSON_OBJECT_CLOSE,2,(CHAR16*)NULL,(CHAR16*)NULL);
	UpdateData(&JsonStr);
	JsonStr = GetJsonDataStr( TRUE,JSON_OBJECT_CLOSE,1,(CHAR16*)NULL,(CHAR16*)NULL);
	UpdateData(&JsonStr);
}

/**
  Frees the elements in MULTI_LANG_STRINGS structure
    
  @param  MultiLangStrings	Structure to be freed

  @return VOID
**/

VOID FreeMultiLangStrings (MULTI_LANG_STRINGS *MultiLangStrings)
{
    UINTN       Index = 0;
    
    if (NULL != MultiLangStrings)
    {
        for (Index = 0; Index < gLangCount; Index ++)
        {
            MemFreePointer (&(MultiLangStrings [Index].String));
        }
        MemFreePointer (&MultiLangStrings);
    }
}

/**
  Frees the Attribute Data

  @param Attributes		 ATTRIBUTES structure will be freed

  @retval void

**/

void FreeAttributeData (ATTRIBUTES **TempAttributes)
{
	VALUE		*Value = (VALUE*)NULL;
	UINTN       Index = 0;
	ATTRIBUTES  *Attributes;
	
	Attributes = *TempAttributes;
	while (Attributes)
	{
        if(NULL != Attributes)
        {
            FreeMultiLangStrings (Attributes->_DisplayName);
            FreeMultiLangStrings (Attributes->_HelpText);
            MemFreePointer(&(Attributes->_AttributeName));
            
            if(NULL != Attributes->_Value)
            {
                for(Index = 0;Index<Attributes->_ValueCount;Index++)
                {
                    Value = &(Attributes->_Value[Index]);
                    MemFreePointer( (VOID**)&(Value->_ValueName));
                    FreeMultiLangStrings (Value->_ValueDisplayName);
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
        *TempAttributes = Attributes->Next;
        MemFreePointer (&Attributes);
        Attributes = *TempAttributes;
	}
}

/**
  Collect Bios attribute json data

  @param Out_JsonBuffer			Attribute registry data returned by this function.
  @param Out_JsonbufSize 		Size of the attribute registry will be returned in this
  
  @retval EFI_STATUS

**/

EFI_STATUS Get_BiosSetupJsonData (OUT CONST CHAR16 **Out_JsonBuffer, OUT UINTN *Out_JsonbufSize)
{
	EFI_STATUS      Status = EFI_NOT_FOUND;
	UINTN           iIndex = 0;
	extern UINTN    gLangCount;
	extern CHAR16   gRemoteCurrentLang [20];

	*Out_JsonBuffer = NULL;
	*Out_JsonbufSize = 0;
	
    gBrowserCallbackEnabled = TRUE;     //Using TSE cache via PLDM. BrowserCallback function will be used
	if(NULL != Out_JsonbufSize)
	{
	    InitializeTseMethods();
		
	    if(NULL == gMultiLangVector)
	    {
	        gMultiLangVector = EfiLibAllocateZeroPool (sizeof (MULTILANG_VECTOR) * gLangCount);
	        if (NULL != gMultiLangVector)
	        {
	            for (iIndex = 0; iIndex < gLangCount; iIndex ++)
	            {
	                StrCpy (gMultiLangVector [iIndex].Language, gLanguages[iIndex].Unicode);
	                gMultiLangVector[iIndex].LanguageVector = EfiLibAllocateZeroPool (sizeof (VECTOR));
	                if (NULL != gMultiLangVector[iIndex].LanguageVector)
	                {
	                    gMultiLangVector [iIndex].LanguageVector->_ReserveSize = RESERVE_SIZE;
	                }
	            }
	        }
	        BiosSetupDataConfig ();
	    }
        for (iIndex = 0; iIndex < gLangCount; iIndex ++)
        {
            if (!StrCmp (gMultiLangVector [iIndex].Language, gRemoteCurrentLang))
            {
                *Out_JsonBuffer = gMultiLangVector [iIndex].LanguageVector->_String;
                *Out_JsonbufSize = gMultiLangVector [iIndex].LanguageVector->_SizeUsed;
                Status = EFI_SUCCESS;
                break;
            }
        }
	}
    gBrowserCallbackEnabled = FALSE;     //Using TSE cache via PLDM. BrowserCallback function will be used
	
	return Status;
}

/**
    Verify the Old password and gets the encrypted data for the password of the Mapstring control,
     
    @param MapId Control        Mapstring name
    @param NewPasswdData        New password data
    @param OldPasswdData        Old password data
    @param EncryptedPwdBuffer   Encoded format of the newpassword will be returned
    @param PwdSize              Encoded password length will be returned
    
     @retval EFI_STATUS return status
     
**/
EFI_STATUS GetPasswordEncryptedData ( CHAR16 *MapId, CHAR16 *NewPasswdData, CHAR16 *OldPasswdData, VOID **EncryptedPwdBuffer, UINTN *PwdSize)
{
    UINTN        PageIndex = 0;
    EFI_STATUS Status = EFI_NOT_FOUND;
    CHAR16 *TempEncodBuffer= (CHAR16 *)NULL;
    CHAR16 *OldSysPassword= (CHAR16 *)NULL;
    CHAR16 *TempOldEncodePwd = (CHAR16 *)NULL;
    CHAR16 *EmptyPassword= (CHAR16 *)NULL;
    CHAR16 *AttributeName   = (CHAR16*)NULL;
    VOID *data = NULL;
    UINTN       ControlIndex = 0;
    PAGE_INFO   *PageInfo = (PAGE_INFO*)NULL;
    CONTROL_INFO    *CtrlInfo = (CONTROL_INFO *)NULL;
    
    if( NULL == NewPasswdData )
        return Status;
    
    gBrowserCallbackEnabled = TRUE;     //Using TSE cache via PLDM. BrowserCallback function will be used
    InitializeTseMethods();
                
    for ( PageIndex = 1; PageIndex < (UINTN)gPages->PageCount; PageIndex++ ) 
    {
        PageInfo = (PAGE_INFO*)( (UINTN)gApplicationData + gPages->PageList[PageIndex] );
        if( NULL == PageInfo->PageHandle )   //ignoring setup
            continue;
            
        for ( ControlIndex = 0; ControlIndex < PageInfo->PageControls.ControlCount; ControlIndex++ )
        {
            CtrlInfo = (CONTROL_INFO *)((UINT8 *)(gControlInfo) + PageInfo->PageControls.ControlList[ControlIndex]);
    
            if( CONTROL_TYPE_PASSWORD == CtrlInfo->ControlType )
            {
                AttributeName = GetActualHiiString(PageInfo->PageHandle,CtrlInfo->QuestionId);
                if( NULL == AttributeName )
                    continue;
                if( StrCmp(AttributeName,MapId) )
                    continue;
                //
                //MapString found
                //
                data = CtrlInfo->ControlPtr;
                
                //Check for New password minimum and maximum lengths
                if( StrLen(NewPasswdData) != 0 && (StrLen(NewPasswdData) < UefiGetMinValue(data) || 
                        StrLen(NewPasswdData) > UefiGetMaxValue(data) ))
                {
                	TRACE((TRACE_ALWAYS, "GetPasswordEncryptedData: NewPasswdData Length mismatch\n")); 
                	Status = EFI_INVALID_PARAMETER;
                    goto DONE;
                }
 
                 //Check for New password minimum and maximum lengths
                //Get the current password value
                OldSysPassword = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
                if ( OldSysPassword == NULL )
                    goto DONE;
                MemSet( OldSysPassword, ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16), 0 );

                DEBUG((DEBUG_ERROR, "TEST:Before OldSysPassword:%x\n",OldSysPassword[0]));
                VarGetValue( CtrlInfo->ControlVariable, 
                    UefiGetQuestionOffset(data), 
                    (UINT8)UefiGetMaxValue(data)*2, OldSysPassword );
                DEBUG((DEBUG_ERROR, "TEST:After OldSysPassword:%x,Offset:%d\n",OldSysPassword[0],  UefiGetQuestionOffset(data)));
                //If Old password data is there, it should match current password
                if(  (NULL != OldPasswdData) && (StrLen(OldPasswdData) > 0) )
                {
                    //Old password present,check for length
                    if( StrLen(OldPasswdData) < UefiGetMinValue(data) || 
                            StrLen(OldPasswdData) > UefiGetMaxValue(data) )
                    {
                	DEBUG((DEBUG_ERROR, "GetPasswordEncryptedData: OldPasswdData Length mismatch\n")); 
                        goto DONE;
                    }
                                    
                    //Encode the old password data
                    Status = Fw_EncodePasswordData( OldPasswdData,  CtrlInfo, &TempOldEncodePwd );
                    if( EFI_ERROR(Status) )
                        goto DONE;
                        
                    //Compare the data with Old password
                    if( EfiCompareMem(TempOldEncodePwd,OldSysPassword,(UINT8)UefiGetMaxValue(data) * sizeof(CHAR16)))
                    {
                	DEBUG((DEBUG_ERROR, "GetPasswordEncryptedData: current Password and OldPasswd mismatch\n"));
                        Status = EFI_OUT_OF_RESOURCES;
                        goto DONE;
                    }
                }
                else if(NULL != OldPasswdData)//Old Password entry is there with NULL value
                {
                    
                    EmptyPassword = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
                    if ( EmptyPassword == NULL )
                       goto DONE;
                    MemSet( EmptyPassword, ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16), 0 );
                    
                    DEBUG((DEBUG_ERROR, "TEST:EmptyPassword:%x\n",EmptyPassword[0]));
                    //Check if current password is present
                    if( EfiCompareMem(EmptyPassword,OldSysPassword,(UINT8)UefiGetMaxValue(data) * sizeof(CHAR16)))
                    {
                	DEBUG((DEBUG_ERROR, "GetPasswordEncryptedData: Current password present,OldPassword NULL mismatch\n"));
                        Status = EFI_OUT_OF_RESOURCES;
                        goto DONE;
                    }
                }
                //Encode the new password
                if (StrLen (NewPasswdData))
                {
                	Status = Fw_EncodePasswordData( NewPasswdData,  CtrlInfo, &TempEncodBuffer );
                }
                else
                {
                	TempEncodBuffer = EfiLibAllocateZeroPool (((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16));
                }
                *EncryptedPwdBuffer = TempEncodBuffer;
                *PwdSize = (UINTN)( UefiGetMaxValue(data) * sizeof(CHAR16) );
                
                goto DONE;
            }
        }
    }
    
DONE:
    //Free the allocated memory
    MemFreePointer(&OldSysPassword);
    MemFreePointer(&AttributeName);
    MemFreePointer(&TempOldEncodePwd);
    MemFreePointer(&EmptyPassword);
    
    gBrowserCallbackEnabled = FALSE;     //Using TSE cache via PLDM. BrowserCallback function will be used
    
    return Status;
}

/**
    Encodes the password data passed for the control
     
    @param Password         Password data
    @param CtrlInfo         control Info
    @param EncodedBuffer    Encoded format of the password will be returned

    @retval EFI_STATUS return status
     
**/
EFI_STATUS Fw_EncodePasswordData( CHAR16 *Password,  CONTROL_INFO    *CtrlInfo, CHAR16 **EncodedBuffer )
{
    CHAR16 *PwdEncodeBuffer = (CHAR16 *)NULL;
    UINTN   ii;
    VOID *data = CtrlInfo->ControlPtr;
    
    //Copy the password to temporary buffer
    PwdEncodeBuffer = EfiLibAllocateZeroPool( ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16) );
    if ( PwdEncodeBuffer == NULL )
        return EFI_OUT_OF_RESOURCES;
    
    MemSet( PwdEncodeBuffer, ((UINT8)UefiGetMaxValue(data)+1) * sizeof(CHAR16), 0 );
    StrCpy(PwdEncodeBuffer,Password);
        
    if (
      (SETUP_PASSWORD_NON_CASE_SENSITIVE) && 
      ( (VARIABLE_ID_AMITSESETUP == CtrlInfo->ControlVariable) || (VARIABLE_ID_IDE_SECURITY == CtrlInfo->ControlVariable) )
      )
    {
        for (ii = 0; ii < (UefiGetMaxValue(data)); ii++)
            PwdEncodeBuffer [ii] = ((PwdEncodeBuffer [ii]>=L'a')&&(PwdEncodeBuffer [ii]<=L'z'))?(PwdEncodeBuffer [ii]+L'A'-L'a'):PwdEncodeBuffer [ii];
    }
#if REMOTE_CONFIG_TSE_DEPENDENCY
    //TODO else case
    if( IsPasswordEncodeEnabled( CtrlInfo )){
        PasswordEncodeHook( PwdEncodeBuffer, (UINT8)UefiGetMaxValue(data) * sizeof(CHAR16));
    }
    
    *EncodedBuffer = PwdEncodeBuffer;
#endif
            
    return EFI_SUCCESS;
}

/**
  Installs protocols to retrieve the json files and to support multilanguage
    
**/

VOID FirmwareConfigAttribHook(VOID)
{
    extern MULTI_LANGUAGE_PROVISION_PROTOCOL    gMultiLangProvProtocol;
    extern EFI_GUID                             gMultiLangProvProtocolGuid;
    
	gBS->InstallProtocolInterface(&gImageHandle, &gFirmwareConfigAttribGuid, EFI_NATIVE_INTERFACE, &gFirmwareConfigurationProtocol);
	gBS->InstallProtocolInterface(&gImageHandle, &gMultiLangProvProtocolGuid, EFI_NATIVE_INTERFACE, &gMultiLangProvProtocol);
	gBS->InstallProtocolInterface(&gImageHandle, &gAmiConditionalMapIdListProtocolGuid, EFI_NATIVE_INTERFACE, &gAmiCondMapIdListProtocol);
}

/**
    Loads the setup configuration values
     
    @param UINT8
    
    @retval EFI_STATUS return status
     
**/
EFI_STATUS ResetBiosConfig (UINT8 Operation)
{
	BOOLEAN SaveStatus = EFI_SUCCESS;
	
    gBrowserCallbackEnabled = TRUE;
    InitializeTseMethods ();
    gBrowserCallbackEnabled = FALSE;
    
#if REMOTE_CONFIG_TSE_DEPENDENCY     
    LoadSetupDefaultsHook (gOptimalDefaults, 0);		//0 for not displaying message box
    SaveStatus = _SaveValues (0);
#endif
    
    if (TRUE == SaveStatus)
    	return EFI_SUCCESS;
    else
    	return EFI_ABORTED;
}

/**
    Checks for system password presence
    
    @param UINT32 * -> Users responsible to pass allocated parameter
     
    @retval EFI_STATUS
     
**/
EFI_STATUS CheckForSystemPassword (UINT32 *PasswordInstalled)
{
	*PasswordInstalled = PasswordCheckInstalled ();
	return EFI_SUCCESS;
}


/**
    Validates the parameter with the already set system password
     
    @param CHAR16 *
    @param UINT32 *  -> Users responsible to pass allocated parameter
    @param BOOLEAN *  -> Users responsible to pass allocated parameter
    
    @retval EFI_STATUS
     
**/
EFI_STATUS ValidateSystemPassword (CHAR16 *PasswordEntered, UINT32 *PasswordInstalled, BOOLEAN 	*PasswordMatched)
{
	*PasswordInstalled = PasswordAuthenticate (PasswordEntered);
	
	if ((AMI_PASSWORD_USER == *PasswordInstalled) || (AMI_PASSWORD_ADMIN == *PasswordInstalled))
	{
		*PasswordMatched = TRUE;
	}
	return EFI_SUCCESS;
}

/**
  Forms all available language strings and returns it
    
  @param  Handle		Handle for HII database     
  @param  StrToken   	String token id in HII database
  @param  String     	If handle is NULL this string is used for all language strings	formation
  @param  BitMasking 	For future use

  @return MULTI_LANG_STRINGS *

**/

MULTI_LANG_STRINGS *FormMultiLangStrings (EFI_HII_HANDLE Handle, UINT16 StrToken, CHAR16 *String, UINTN BitMasking)
{
    UINTN               iIndex = 0;
    CHAR16              *TempString = NULL;
    CHAR16              *FormattedString = NULL;
    MULTI_LANG_STRINGS  *MultiLangStrings = NULL;
    
    MultiLangStrings = EfiLibAllocateZeroPool (sizeof (MULTI_LANG_STRINGS) * gLangCount);
    if (NULL != MultiLangStrings)
    {
        for (iIndex = 0; iIndex < gLangCount; iIndex ++)
        {
            if (NULL != Handle)
            {
                TempString = Uefi21HiiGetStringLanguage (Handle, StrToken, gLanguages[iIndex].Unicode);
                String = TempString;
            }
            StrCpy (MultiLangStrings [iIndex].StringLang, gLanguages[iIndex].Unicode);
            FormattedString = SkipEscCode (String);
            RemoveSpecialChars (&FormattedString);            
            
            MultiLangStrings [iIndex].String = EfiLibAllocateZeroPool ((StrLen (FormattedString) + 1) * sizeof (CHAR16));
            if (NULL != MultiLangStrings [iIndex].String)
            {
                StrCpy (MultiLangStrings [iIndex].String, FormattedString);
            }
            MemFreePointer (&TempString);
            MemFreePointer (&FormattedString);
        }
    }
    return MultiLangStrings;
}

/**
  Finds the English string index in MULTI_LANG_STRINGS structure
    
  @param  MULTI_LANG_STRINGS	Structure to which English string need to be find

  @return UINTN  				Index of the English string in MULTI_LANG_STRINGS structure 

**/

UINTN FindEnglishString (MULTI_LANG_STRINGS *MultiLangStrings)
{
    UINTN    iIndex = 0;
    
    for (iIndex = 0; iIndex < gLangCount; iIndex ++)
    {
        if (!StrCmp (MultiLangStrings [iIndex].StringLang, L"en-US"))
        {
            return iIndex;
        }
    }
    return 0;
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
