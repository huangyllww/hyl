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

/** @file SetupDataXmlCreation.c
    Contains Functions defined to create setup data as xml

**/

#include <SetupDataXmlCreation.h>
#include "Library/BaseMemoryLib.h"

UINTN       gSetupDataAllocSize = 0;
UINTN       gSetupDataSize = 0;
CHAR16      *gSetupDataBuffer =NULL;
UINT8       gPrevSplCtrlIndex = 0;
MAP_ID_LIST *gXmlAttrRegMapIdList = NULL;

EFI_STATUS CreateAttribRegMappingList ()
{
    VOID        *MapIdList = (VOID *)NULL;
    EFI_STATUS  Status = EFI_SUCCESS;
    AMI_CONDITIONAL_MAPIDLIST_PROTOCOL *AmiCondMapIdListProtocol = NULL;
    
    Status = pBS->LocateProtocol (&gAmiConditionalMapIdListProtocolGuid, NULL, (void**)&AmiCondMapIdListProtocol);
    if(!EFI_ERROR(Status))
    {
        Status = AmiCondMapIdListProtocol->GetConditionalMapIdList (&MapIdList);
        if (!EFI_ERROR(Status))
        {
            gXmlAttrRegMapIdList = MapIdList;
        }
    }
    return EFI_SUCCESS;
}

/**
    Parses every page information. Loops until all pages are completed and forms the SetupData.xml.
    @param  UINTN * -> Setupdata size
    @param  void * -> Setupdata buffer
    @retval void
**/
EFI_STATUS CreateSetupData (void **Buffer, UINTN *BuffSize, UINT8 SplCtrlIndex)
{          
    BOOLEAN         IsTseInitialised = FALSE;
    if( gPrevSplCtrlIndex != SplCtrlIndex)
    {
        gPrevSplCtrlIndex = SplCtrlIndex;
        if(gSetupDataAllocSize)
            IsTseInitialised = TRUE;
        MemFreePointer( (VOID **)&gSetupDataBuffer );
        gSetupDataAllocSize = 0;
        gSetupDataSize = 0;            
        gSetupDataBuffer = NULL;
    }
    
#if REMOTE_COND_EXPRESSION_SUPPORT
    CreateAttribRegMappingList ();
#endif    
    
    if( NULL == gSetupDataBuffer)
    {
        if( !IsTseInitialised )
            XmlCreationInitializeTseMethods ();
        XmlGet_BiosAttributes();
    }

    *Buffer = gSetupDataBuffer;
    *BuffSize = StrLen (gSetupDataBuffer) * sizeof (CHAR16);
	HiiExit ();   //When ALWAYS_PUBLISH_HII_RESOURCES is disabled. TSE is getting notification in MainSetupLoopInitHook itself.
				  //on publishing the packs. This is wrong so calling HiiExit here. If not calling HiiExit then pack notification
				  //is not un registered.
	
    if ((NULL != *Buffer) && (0 != *BuffSize))
    {
        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

/**
    Saves the buffer obtained to a global buffer for future use
    @param  CHAR16 * - string to be stored in global buffer
    @retval void
**/
void WriteToBuffer (CHAR16 *tempBuffer)
{
    UINTN tempSize = StrLen (tempBuffer) * sizeof (CHAR16);
    UINTN TempAllocSize = 0;
    
    if (0 == tempSize)
    {
        return;
    }
    
    gSetupDataSize += tempSize;
    if (gSetupDataSize >= gSetupDataAllocSize)
    {
        //Sometimes tempsize will be huge so checking here
        TempAllocSize = (tempSize >= MEMORY_ALLOC_UNIT) ? (tempSize + 100) : MEMORY_ALLOC_UNIT;     //Extra 100 for safety
        gSetupDataBuffer = realloc (gSetupDataBuffer, (gSetupDataAllocSize * sizeof (CHAR16)), ((gSetupDataAllocSize + TempAllocSize) * sizeof (CHAR16)));
        if (NULL == gSetupDataBuffer)
        {
            return;
        }
        gSetupDataAllocSize += TempAllocSize;
    }
    if (0 == (gSetupDataSize - tempSize))
    {
        UINT16 Header = 0xFEFF; 
        MemCpy (gSetupDataBuffer, &Header, sizeof (UINT16));
        gSetupDataSize += sizeof (UINT16);
    }
    StrCat (gSetupDataBuffer, tempBuffer);
}

/**
     Returns the GUID as a string
    @param EFI_GUID
    @retval text which contains GUID as string
**/
CHAR16 *GetGUIDAsString(EFI_GUID *Guid)
{
   UINTN Size = 1024;
   CHAR16 *guidString = (CHAR16*)EfiLibAllocateZeroPool(Size);
   if(NULL == guidString)
   {
       return guidString;
   }
  
   swprintf( guidString, Size, L"%x-%x-%x-%x%x-%02x%02x%02x%02x%02x%02x", Guid->Data1, Guid->Data2, Guid->Data3, 
                                          Guid->Data4[0], Guid->Data4[1], Guid->Data4[2], Guid->Data4[3], 
                                          Guid->Data4[4], Guid->Data4[5], Guid->Data4[6], Guid->Data4[7] );
   return guidString;
}

/**
     Checks whether the special characters in xml (&,<,>,' and ") and replaces them if present.
    @param text which may have special characters
    @retval text after the special characters are replaced
**/
CHAR16* CheckSpecialChar(CHAR16 *text)
{
    CHAR16 *Buffer = NULL;
    UINTN Size,i=0;
    UINTN Count = 0;
    
    if((NULL ==StrStr(text,L"&"))&&(NULL ==StrStr(text,L"<"))&&(NULL ==StrStr(text,L">"))&&(NULL ==StrStr(text,L"\""))&&(NULL ==StrStr(text,L"'")))
        return text;
    Size = StrLen(text);
    
    for(i=0;i<Size;i++)
    {
        if (
                L'&' == text[i] ||
                L'<' == text[i] ||
                L'>' == text[i] ||
                L'"' == text[i] ||
                L'\'' == text[i]
                )
                {
                    Count ++;
                }
    }
    Buffer  = EfiLibAllocateZeroPool (( (Count * 6) + Size + 3) * sizeof (CHAR16));       //6 -> max "&apos;" chars;; + 3 NULL and safety
    if(NULL == Buffer)
    {
        return Buffer;
    }
    
    for(i=0;i<Size;i++)
    {
        
        switch(text[i])
        {
            case '&':
                StrCat( Buffer, L"&amp;" );
                break;
            case '<':
                StrCat( Buffer, L"&lt;" );
                break;
            case '>':
                StrCat( Buffer, L"&gt;" );
                break;
            case '"':
                StrCat( Buffer, L"&quot;" );
                break;
            case '\'':
                StrCat( Buffer, L"&apos;" );
                break;
            default:
                StrnCat( Buffer, &text[i], 1 );
                break;
        }
        
    }
    MemFreePointer( (VOID **)&text );
    return Buffer;
}

/**
    Extracts the information of each control and writes into a buffer for the control obtained from parent function.
	
    @param CtrlInfo		CONTROL_INFO to be written into xml	
    @param Attributes	Attributes filled from CONTROL_INFO
	@param RefGuid		Reference guid for submenu useful for Ref3 etc.,
	
    @retval void
**/
void WriteControlInfoToBuffer(CONTROL_INFO* CtrlInfo, ATTRIBUTES  *Attributes, EFI_GUID RefGuid)
{
    CHAR16  *DisplayName= (CHAR16 *)NULL ;
    CHAR16  *Help= (CHAR16 *)NULL ;
    CHAR16  *text2= (CHAR16 *)NULL ;
    CHAR16  tempBuffer[MEMORY_ALLOC_UNIT];
    UINTN   Size = MEMORY_ALLOC_UNIT ;
    CHAR16  *AttributeName = (CHAR16  *)NULL;
    GUID_INFO *guidInfo = (GUID_INFO*)NULL;
    EFI_GUID ZeroGuid = {0};
    CHAR16  *Secondtext= (CHAR16 *)NULL ;
    
    if(CtrlInfo == NULL)
        return;
 
    if(CONTROL_TYPE_SUBMENU == CtrlInfo->ControlType)
    {
        UINT16 value = UefiGetSpecialEqIDValue (CtrlInfo, &guidInfo);
       
        if (value != (UINT16)-2)
        {
            if(guidInfo->GuidFlags.GuidLaunchCallback)
            {    
                if( GUID_KEY_AMI_CALLBACK == guidInfo->GuidKey )
                {
                    //Allow save changes and discard changes only
                    if( (SAVE_VALUE != value) && (DISCARD_VALUE != value) )
                        return;
                }
            }
        }
        DisplayName = GetMultiLanguageXmlString (CtrlInfo->ControlHandle, CtrlInfo->QuestionId, L"DisplayName", L"");
        Help = Uefi21HiiGetStringLanguage( CtrlInfo->ControlHandle, CtrlInfo->ControlHelp , NULL );
        if(Help && StrLen( Help ))
        {
            text2 = SkipEscCode( Help );
            if(Help != NULL)
                MemFreePointer((VOID **)&Help);
            Help = CheckSpecialChar(text2);
        }

        AttributeName = XmlGetActualHiiString(CtrlInfo->ControlHandle,CtrlInfo->QuestionId);
        if( AttributeName )
        {
            text2 = SkipEscCode( AttributeName );
            if(AttributeName != NULL)
                MemFreePointer((VOID **)&AttributeName);
            AttributeName = CheckSpecialChar(text2);
            if(CompareGuid(&ZeroGuid, &RefGuid) == 0 )
        	    swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"%s\" Type=\"Submenu\" ControlDestPageID=\"0x%x\" %s HelpText=\"%s\" RefGuid=\"%g\" MappingId=\"%s\" /> ", L"", CtrlInfo->ControlDestPageID, DisplayName, Help, &RefGuid, AttributeName);
            else
        	    swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"%s\" Type=\"Submenu\" ControlDestPageID=\"0x%x\" %s HelpText=\"%s\" RefGuid=\"\" MappingId=\"%s\" /> ", L"", CtrlInfo->ControlDestPageID, DisplayName, Help, AttributeName);
        }
        else
        {
        	if(CompareGuid(&ZeroGuid, &RefGuid) == 0 )
        	    swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"\" Type=\"Submenu\" ControlDestPageID=\"0x%x\" %s HelpText=\"%s\" RefGuid=\"%g\" /> ", CtrlInfo->ControlDestPageID, DisplayName, Help, &RefGuid);           
            else
        	    swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"\" Type=\"Submenu\" ControlDestPageID=\"0x%x\" %s HelpText=\"%s\" RefGuid=\"\" /> ", CtrlInfo->ControlDestPageID, DisplayName, Help);
        }
        WriteToBuffer( tempBuffer );

    }
	else if ( CONTROL_TYPE_TEXT == CtrlInfo->ControlType )
	{
		UINT16 Token = UefiGetTextTwoField(CtrlInfo->ControlPtr);
		
		AttributeName = NULL;
        AttributeName = XmlGetActualHiiString(CtrlInfo->ControlHandle,CtrlInfo->QuestionId);
        
		DisplayName = GetMultiLanguageXmlString (CtrlInfo->ControlHandle, CtrlInfo->QuestionId, L"DisplayName", L"");
		Secondtext = GetMultiLanguageXmlString (CtrlInfo->ControlHandle, Token, L"Text", L"N/A");
		if (NULL == AttributeName)
		{
		    swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"\" Type=\"Text\" %s %s /> ", Secondtext, DisplayName);
		}
		else
		{
		    swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"\" Type=\"Text\" %s %sMappingId=\"%s\"/> ", Secondtext, DisplayName, AttributeName);
		}
		WriteToBuffer(tempBuffer);
	}
    else if ( CONTROL_TYPE_MEMO == CtrlInfo->ControlType )
    {
        CHAR16  *Text2= (CHAR16 *)NULL ;
        UINT16 Token = UefiGetTextTwoField(CtrlInfo->ControlPtr);
        
        AttributeName = NULL;
        AttributeName = XmlGetActualHiiString(CtrlInfo->ControlHandle,CtrlInfo->QuestionId);
                
        DisplayName = GetMultiLanguageXmlString (CtrlInfo->ControlHandle, CtrlInfo->QuestionId, L"SubTitle", L"");
        if (NULL == AttributeName)
        {
            swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"\" Type=\"Memo\" %s /> ", DisplayName);
        }
        else
        {
            swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"\" Type=\"Memo\" %sMappingId=\"%s\"/> ", DisplayName, AttributeName);
        }
        WriteToBuffer(tempBuffer);
    }
    else
    {   
        AttributeName = StrDup( Attributes->_AttributeName );
        if( AttributeName )
        {
            text2 = SkipEscCode( AttributeName );
            if(AttributeName != NULL)
                MemFreePointer((VOID **)&AttributeName);
            AttributeName = CheckSpecialChar(text2);
        }
        
        swprintf( tempBuffer, Size, L"\n\t\t\t\t<Control AttributeName=\"%s\"/> ",Attributes->_AttributeName);
        
        WriteToBuffer( tempBuffer);
    }
    
    if(DisplayName != NULL)
        MemFreePointer((VOID **)&DisplayName);
    if(Help != NULL)
        MemFreePointer((VOID **)&Help);
    if(Secondtext != NULL)
        MemFreePointer((VOID **)&Secondtext);
    if(AttributeName != NULL)
        MemFreePointer((VOID **)&AttributeName);
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
