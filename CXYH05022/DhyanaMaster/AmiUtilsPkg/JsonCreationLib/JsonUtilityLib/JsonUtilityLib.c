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

/** @file FwConfigJsonHelper.c
    Contains functions to parse control information types and write to Json format

**/

//---------------------------------------------------------------------------

#include <Token.h>


#include <JsonUtilityLib.h>

#include <string.h>
#include <limits.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
//#include <Ppi/FwVersion.h>
#include <Fid.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/FirmwareVolume.h>
#include "AmiDxeLib.h"

#define FID_FFS_FILE_NAME_GUID \
    { 0x3fd1d3a2, 0x99f7, 0x420b, 0xbc, 0x69, 0x8b, 0xb1, 0xd4, 0x92, 0xa3, 0x32 }

#define INT64_MAX_LIMIT 0x7FFFFFFFFFFFFFFF
#define INT64_MAX_MASK_BIT 0x8000000000000000
//---------------------------------------------------------------------------

#define realloc 			MemReallocateZeroPool
JSON_DATA_FRMT_TYPE PrevStyle = JSON_NOT_SET;
CHAR8  *bArray[] = {"false","true"};

//---------------------------------------------------------------------------

UINTN Swprintf(CHAR16 *Buffer, CHAR16 *Format, ...);
int sprintf(char *str, char const *fmt, ...);
VOID JsonUtilMemFreePointer( VOID **ptr );

extern EFI_BOOT_SERVICES    *pBS;
SYSTEM_BUILD_INFO gSysBuildInfo = {0,0,0,"","",FALSE};

VOID JsonUtilMemFreePointer( VOID **ptr )
{
    if ( ( ptr == NULL ) || ( *ptr == NULL ) )
        return;

    pBS->FreePool( *ptr );
    *ptr = NULL;
}

/**
    Convert CHAR16 String to CHAR8 string

    @param CHAR16 *InputString16

    @retval CHAR8*
**/
CHAR8* RemoteConvertChar16toChar8(CHAR16 *InputString16)
{
    CHAR8 *OutString8 = (CHAR8 *)NULL;
    UINTN Strlen16 = 0;
    UINTN Index = 0;
    
    if ((NULL == InputString16) || (*InputString16 == L'\0'))
    {
        return (CHAR8*)NULL;
    }

    Strlen16 = StrLen(InputString16);
    OutString8 = AllocateZeroPool(Strlen16+1 * sizeof(CHAR8));

    while(InputString16[Index] != L'\0')
    {
        OutString8[Index] = (CHAR8)InputString16[Index];
        Index++;
    }

    OutString8[Index] = '\0';
    return OutString8;
}

/**
    Construct and return Json Data from the String 

    @param BOOLEAN* IsUniCode,JSON_DATA_FRMT_TYPE Style,UINTN Lebel,VOID *LeftNameStr,VOID *RightValueStr

    @retval CHAR16* or CHAR8* based on unicode value
**/

VOID* GetJsonDataStr(BOOLEAN IsUniCode,JSON_DATA_FRMT_TYPE Style,UINTN Lebel,VOID *LeftNameStr,VOID *RightValueStr)
{
	VOID *Str = NULL;
	
	_fltused              = 1;
	if(TRUE == IsUniCode)
		Str = GetJsonDataStr16(Style,Lebel,LeftNameStr,RightValueStr);
	else
		Str = GetJsonDataStr8(Style,Lebel,LeftNameStr,RightValueStr);
	return Str;
}

/**
    Construct and return Json Data from the CHAR8 String 

    @param JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR8 *LeftNameStr,CHAR8 *RightValueStr

    @retval CHAR8* string
**/

CHAR8* GetJsonDataStr8(JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR8 *LeftNameStr,CHAR8 *RightValueStr)
{
	CHAR8 *JsonNameValueStr = (CHAR8*)NULL;
	UINTN AllocStrlen = 0;
	CHAR8 NewEntry[128]= "\n";
	CHAR8 AdditionalEntry[128]= ",\n";
	CHAR8 *PrefixStr = NULL;
	CHAR8 Token[128];
	UINTN Index = 0;
//	static JSON_DATA_FRMT_TYPE PrevStyle = JSON_NOT_SET;
	
	if( (1 == Lebel) && (Style==JSON_OBJECT_OPEN))
		 PrevStyle = JSON_NOT_SET;
	
	PrefixStr =  NewEntry;
	if(PrevStyle != JSON_NOT_SET)
	{
		switch(PrevStyle)
		{
		case JSON_DATA_BOOLEAN:
		case JSON_DATA_INTEGER:
		case JSON_DATA:
			if((JSON_OBJECT_CLOSE != Style)&&(JSON_ARRAY_CLOSE != Style))
				PrefixStr =  AdditionalEntry;
			break;
		case JSON_OBJECT_CLOSE:
			if(JSON_OBJECT_OPEN == Style)
				PrefixStr =  AdditionalEntry;
			break;
		case JSON_ARRAY_CLOSE:
			if((JSON_DATA == Style) ||(JSON_DATA_INTEGER == Style))
				PrefixStr =  AdditionalEntry;
			break;
		}		
	}

	if( (1 == Lebel) && (Style==JSON_OBJECT_OPEN))
	{
		PrefixStr[0] = '\0';
	}
#if OPTIMISATION_NO_SPACE_ATTRIB_REGISTRY
	else
	{
		for(Index=1;Index < Lebel;Index++)
		{
			strcat(PrefixStr,"  ");
		}
	}
#endif
	
	switch(Style)
	{
	case JSON_OBJECT_OPEN:
		if(NULL == LeftNameStr)
			strcpy(Token,"{");
		else
			strcpy(Token,": {");
		break;
	case JSON_OBJECT_CLOSE:
			strcpy(Token,"}");
		break;
	case JSON_ARRAY_OPEN:
		if(NULL == LeftNameStr)
			strcpy(Token,"[");
		else
			strcpy(Token,": [");
		break;
	case JSON_ARRAY_CLOSE:
		strcpy(Token,"]");
		break;
	case JSON_DATA_BOOLEAN:
	case JSON_DATA_INTEGER:
	case JSON_DATA:
		strcpy(Token,": ");
		break;
	default:
		break;
		
	}
	
	if(NULL !=LeftNameStr)
		AllocStrlen += strlen(LeftNameStr);
	if(NULL !=RightValueStr)
		AllocStrlen += strlen(RightValueStr);
	if(NULL !=PrefixStr)
		AllocStrlen += strlen(PrefixStr);
	if(NULL !=Token)
		AllocStrlen += strlen(Token);
	AllocStrlen += 20;
	JsonNameValueStr = AllocateZeroPool(AllocStrlen * sizeof(CHAR8));
	
	
	if((NULL !=LeftNameStr)&& (NULL !=RightValueStr) )
	{
		if(Style == JSON_DATA_INTEGER)
			sprintf( JsonNameValueStr , "%s\"%s\"%s%s",PrefixStr,LeftNameStr,Token,RightValueStr);
		else if( Style == JSON_DATA_BOOLEAN )
			sprintf( JsonNameValueStr , "%s\"%s\"%s%s",PrefixStr,LeftNameStr,Token,bArray[Rbatoi_base (RightValueStr, 10)]);
		else
			sprintf( JsonNameValueStr , "%s\"%s\"%s\"%s\"",PrefixStr,LeftNameStr,Token,RightValueStr);
	}
	else if((NULL ==LeftNameStr)&& (NULL ==RightValueStr) )
		sprintf( JsonNameValueStr , "%s%s",PrefixStr,Token);
	else if(NULL ==RightValueStr )
	{
	    if( 3 == Lebel)
	        sprintf( JsonNameValueStr , "%s\"%s\"%s\"\"",PrefixStr,LeftNameStr,Token);
	    else
	        sprintf( JsonNameValueStr , "%s\"%s\"%s",PrefixStr,LeftNameStr,Token);
	}
	else if(NULL ==LeftNameStr )
		sprintf( JsonNameValueStr , "%s%s\"%s\"",PrefixStr,Token,RightValueStr);
	
	PrevStyle = Style;
	return JsonNameValueStr;
}

/**
    Construct and return Json Data from the CHAR16 String 

    @param JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR16 *LeftNameStr,CHAR16 *RightValueStr

    @retval CHAR16* string
**/

CHAR16* GetJsonDataStr16(JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR16 *LeftNameStr,CHAR16 *RightValueStr)
{
	CHAR16 *JsonNameValueStr = (CHAR16*)NULL;
	UINTN AllocStrlen = 0;
	CHAR16 NewEntry[128]= L"\n";
	CHAR16 AdditionalEntry[128]= L",\n";
	CHAR16 *PrefixStr = NULL;
	CHAR16 Token[128];
	UINTN Index = 0;
//	static JSON_DATA_FRMT_TYPE PrevStyle = JSON_NOT_SET;
	
	PrefixStr =  NewEntry;
	if(PrevStyle != JSON_NOT_SET)
	{
		switch(PrevStyle)
		{
		case JSON_DATA_INTEGER:
		case JSON_DATA:
			if((JSON_OBJECT_CLOSE != Style)&&(JSON_ARRAY_CLOSE != Style))
			{
			    if (NULL != LeftNameStr)
			    {
					//If there is no below if condition then ,(comma) is adding after { Fix added for multilanguage support
			        if (StrCmp (LeftNameStr, L"ValueDisplayName"))
			            PrefixStr =  AdditionalEntry;
			    }
			    else
			        PrefixStr =  AdditionalEntry;
			}
			break;
		case JSON_OBJECT_CLOSE:
			if(JSON_OBJECT_OPEN == Style)
				PrefixStr =  AdditionalEntry;
			break;
		case JSON_ARRAY_CLOSE:
			if((JSON_DATA == Style) ||(JSON_DATA_INTEGER == Style ))
				PrefixStr =  AdditionalEntry;
			break;
		}		
	}

	if( (1 == Lebel) && (Style==JSON_OBJECT_OPEN))
	{
		PrefixStr[0] = L'\0';
	}
#if OPTIMISATION_NO_SPACE_ATTRIB_REGISTRY
	else
	{
		for(Index=1;Index < Lebel;Index++)
		{
				StrCat(PrefixStr,L"  ");
		}
	}
#endif
	
	switch(Style)
	{
	case JSON_OBJECT_OPEN:
		if(NULL == LeftNameStr)
			StrCpy(Token,L"{");
		else
			StrCpy(Token,L": {");
		break;
	case JSON_OBJECT_CLOSE:
			StrCpy(Token,L"}");
		break;
	case JSON_ARRAY_OPEN:
		if(NULL == LeftNameStr)
			StrCpy(Token,L"[");
		else
			StrCpy(Token,L": [");
		break;
	case JSON_ARRAY_CLOSE:
		StrCpy(Token,L"]");
		break;
	case JSON_DATA_INTEGER:
	case JSON_DATA:
		StrCpy(Token,L": ");
		break;
	default:
		break;	
	}
	
	if(NULL !=LeftNameStr)
		AllocStrlen += StrLen(LeftNameStr);
	if(NULL !=RightValueStr)
		AllocStrlen += StrLen(RightValueStr);
	if(NULL !=PrefixStr)
		AllocStrlen += StrLen(PrefixStr);
	if(NULL !=Token)
		AllocStrlen += StrLen(Token);
	AllocStrlen += 20;
	JsonNameValueStr = AllocateZeroPool(AllocStrlen * sizeof(CHAR16));
	
	if((NULL !=LeftNameStr)&& (NULL !=RightValueStr) )
	{
		if(Style == JSON_DATA_INTEGER)
			Swprintf( JsonNameValueStr , L"%s\"%s\"%s%s",PrefixStr,LeftNameStr,Token,RightValueStr);
		else
			Swprintf( JsonNameValueStr , L"%s\"%s\"%s\"%s\"",PrefixStr,LeftNameStr,Token,RightValueStr);
	}
	else if((NULL ==LeftNameStr)&& (NULL ==RightValueStr) )
		Swprintf( JsonNameValueStr , L"%s%s",PrefixStr,Token);
	else if(NULL ==RightValueStr )
		Swprintf( JsonNameValueStr , L"%s\"%s\"%s",PrefixStr,LeftNameStr,Token);
	else if(NULL ==LeftNameStr )
		Swprintf( JsonNameValueStr , L"%s%s\"%s\"",PrefixStr,Token,RightValueStr);
	
	PrevStyle = Style;
	return JsonNameValueStr;
}

/**
    reallocate and copy old CHAR16 VECTOR buffer to new VECTOR buffer

    @param VECTOR *SrcVector,UINTN RequireSize

    @retval EFI_STATUS
**/

EFI_STATUS ReAllocString16(VECTOR *SrcVector,UINTN RequireSize)
{
	UINTN Size = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR16 *String = (CHAR16*)NULL;
	
	if(NULL != SrcVector)
	{
		if(SrcVector->_SizeFree <= RequireSize)
		{
			Size = SrcVector->_SizeUsed+(RequireSize - SrcVector->_SizeFree)+ SrcVector->_ReserveSize;
			if(0 != Size)
			{
				String = AllocateZeroPool(Size * sizeof(CHAR16));
				if(NULL != String)
				{
					
					if(0!= SrcVector->_SizeUsed)
					{	
						StrCat(String,SrcVector->_String);
						JsonUtilMemFreePointer(&(SrcVector->_String));
					}
					
					SrcVector->_SizeFree = SrcVector->_ReserveSize;
					
					SrcVector->_String = String;
				}
				else
					Status = EFI_OUT_OF_RESOURCES;
			}
			else
				Status = EFI_BAD_BUFFER_SIZE;
		}
	}
	else
		Status = EFI_INVALID_PARAMETER;
	return Status;
}

/**
    reallocate and copy old CHAR8 VECTOR buffer to new VECTOR buffer

    @param VECTOR *SrcVector,UINTN RequireSize

    @retval EFI_STATUS
**/

EFI_STATUS ReAllocString8(VECTOR *SrcVector,UINTN RequireSize)
{
	UINTN Size = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR8 *String = (CHAR8*)NULL;
	
	if(NULL != SrcVector)
	{
		if(SrcVector->_SizeFree <= RequireSize)
		{
			Size = SrcVector->_SizeUsed+(RequireSize - SrcVector->_SizeFree)+ SrcVector->_ReserveSize;
			if(0 != Size)
			{
				String = AllocateZeroPool(Size * sizeof(CHAR8));
				if(NULL != String)
				{
					
					if(0!= SrcVector->_SizeUsed)
					{	
						strcat(String,SrcVector->_String8);
						JsonUtilMemFreePointer(&(SrcVector->_String8) );
					}	
					SrcVector->_SizeFree = SrcVector->_ReserveSize;		
					SrcVector->_String8 = String;
				}
				else
					Status = EFI_OUT_OF_RESOURCES;
			}
			else
				Status = EFI_BAD_BUFFER_SIZE;
		}
	}
	else
		Status = EFI_INVALID_PARAMETER;
	return Status;
}

/**
    Append string to the VECTOR

    @param BOOLEAN IsUnicode,VECTOR *SrcVector,VOID *AppendStr

    @retval EFI_STATUS
**/

EFI_STATUS AppendString(BOOLEAN IsUnicode,VECTOR *SrcVector,VOID *AppendStr)
{
	EFI_STATUS AppendStatus = EFI_SUCCESS;
	
	if(TRUE == IsUnicode)
		AppendStatus = AppendString16(SrcVector,AppendStr);
	else
		AppendStatus = AppendString8(SrcVector,AppendStr);
	return AppendStatus;
}

/**
    Append string to the CHAR16 VECTOR

    @param VECTOR *SrcVector,CHAR16 *AppendStr

    @retval EFI_STATUS
**/

EFI_STATUS AppendString16(VECTOR *SrcVector,CHAR16 *AppendStr)
{
	UINTN RequireSize = 0;
	EFI_STATUS AppendStatus = EFI_SUCCESS;
	
	if((NULL != AppendStr) && (NULL != SrcVector))
	{
		RequireSize = StrLen(AppendStr);
		AppendStatus = ReAllocString16(SrcVector,RequireSize);
		if(EFI_SUCCESS == AppendStatus)
		{
			StrCat(SrcVector->_String,AppendStr);
			SrcVector->_SizeUsed += RequireSize;
			SrcVector->_SizeFree -= RequireSize;
		}
	}
	else
		AppendStatus = EFI_INVALID_PARAMETER;
	return AppendStatus;
}


/**
    Append string to the CHAR8 VECTOR

    @param VECTOR *SrcVector,CHAR8 *AppendStr

    @retval EFI_STATUS
**/

EFI_STATUS AppendString8(VECTOR *SrcVector,CHAR8 *AppendStr)
{
	UINTN RequireSize = 0;
	EFI_STATUS AppendStatus = EFI_SUCCESS;
	
	if((NULL != AppendStr) && (NULL != SrcVector))
	{
		RequireSize = strlen(AppendStr);
		AppendStatus = ReAllocString8(SrcVector,RequireSize);
		if(EFI_SUCCESS == AppendStatus)
		{
			strcat(SrcVector->_String8,AppendStr);
			SrcVector->_SizeUsed += RequireSize;
			SrcVector->_SizeFree -= RequireSize;
		}
	}
	else
		AppendStatus = EFI_INVALID_PARAMETER;
	return AppendStatus;
}

/**
    Allocate new Duplicate string

    @param CHAR16 *Src

    @retval CHAR16 *
**/

CHAR16 *DupString(CHAR16 *Src)
{
	CHAR16 *Dest = (CHAR16*)NULL;
	UINTN AllocStrlen = 0;
	if(NULL != Src)
	{
		AllocStrlen = StrLen(Src);	
		Dest =AllocateZeroPool( (AllocStrlen+2) *sizeof(CHAR16));
		StrCpy(Dest,Src);
	}
	return Dest;
}

/**
    replace  string in the source string

    @param CHAR16 *Src,CHAR16 *FindStr,CHAR16 *ReplaceStr

    @retval void
**/

void ReplaceString(CHAR16 *Src,CHAR16 *FindStr,CHAR16 *ReplaceStr)
{
	UINTN	Index = 0;
	UINTN	SubIndex = 0;
	UINTN 	FindStrIndex = 0;
	BOOLEAN IsStringFound = FALSE;
	
	if(NULL != Src)
	{
		for(Index = 0;Src[Index]!= 0;Index++)
		{
			if(Src[Index] == FindStr[SubIndex])
			{
				IsStringFound = TRUE;
				for(SubIndex = 0; FindStr[SubIndex]!= 0;SubIndex++)
				{
					if((Src[Index+SubIndex]== 0)&&(FindStr[SubIndex] != Src[Index+SubIndex]))
					{
						IsStringFound = FALSE;
						break;
					}	
				}
				if(TRUE == IsStringFound)
				{
					for(SubIndex = 0; FindStr[SubIndex] != 0;SubIndex++)
						Src[Index+SubIndex] = ReplaceStr[SubIndex];
					Index += SubIndex;
					
				}
				SubIndex = 0;
				
			}
		}
	}
}

/**
    function to convet a string to integer

    @param CHAR16 *string, UINT8 base

    @retval UINTN
**/

UINTN Rbatoi_base (CHAR8 *string, UINT8 base)
{
    UINTN dval = 0;//EIP:60563 to support signed integers for NUMERIC opcode
    CHAR8 ch;

    if ( base == 10 )
    {
        while ( (ch = *string++ ) != 0x0)
        {
            if ( ch >= '0' && ch <= '9' )
                dval = dval * 10 + ch - '0';
            else
                break;

        }
    }
    else if ( base == 16 )
    {
        while ( (ch = *string++) != 0x0 )
        {
            if ( ch >= '0' && ch <= '9' )
                dval = dval * 16 + ch - '0';
            else if ( ch >= 'a' && ch <= 'f' )
                dval = dval * 16 + ch - 'a' + 10;
            else if ( ch >= 'A' && ch <= 'F' )
                dval = dval * 16 + ch - 'A' + 10;
            else
                break;
        }
    }//EIP:60563 to support signed integers for NUMERIC opcode
    else if( base == 32)
    {
        if(string[0]== '-')
        {
            string++;
            while ( (ch = *string++ ) != 0x0)
            {
                if ( ch >= '0' && ch <= '9' )
                dval = dval * 10 + ch - '0';
            else
                break;
            }
            dval= (INTN)((0xffffffffffffffff - dval )+ 0x1);
        }
        else
        {
            while ( (ch = *string++ ) != 0x0)
            {
                if ( ch >= '0' && ch <= '9' )
                dval = dval * 10 + ch - '0';
            else
                break;
            }
        }
    }
    return dval;
}

/**
    Convert unsigned number to string

    @param UINT64 InputVal,CHAR16 *String

    @retval void
**/

void ConvertIntToString(UINT64 InputVal,CHAR16 *String)
{
	UINT64 Val = 0;
	UINT8 Digit = 0;
	CHAR8 RevString[128];

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

/**
    Convert number to string

    @param UINT64 Num,UINTN ControlDataWidth,UINT8 IsUnSigned

    @retval CHAR16 *
**/

CHAR16* ConvVfrNumToString(UINT64 Num,UINTN ControlDataWidth,UINT8 IsUnSigned)
{
	CHAR16 *Text = NULL;
	Text = (CHAR16*)AllocateZeroPool(128 * sizeof(CHAR16));

	if (1 ==IsUnSigned )
		ConvertIntToString((UINT64)Num,Text);
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

/**
    Checks for space in input string

    @param CHAR16 * -> Mapping id string

    @retval BOOLEAN
**/
BOOLEAN CheckForSpaceInMappingId (CHAR16 *AttributeName)
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
    Returns PROJECT_MAJOR_VERSION token value
     
    @param void

    @retval UINT32
     
**/
UINT32 GetFw_ProjectMajorVersion ()
{
#ifdef PROJECT_MAJOR_VERSION
        return PROJECT_MAJOR_VERSION;
#endif          
    return 0;
}

/**
    Returns PROJECT_MINOR_VERSION token value
     
    @param void

    @retval UINT32
     
**/
UINT32 GetFw_ProjectMinorVersion ()
{
#ifdef PROJECT_MINOR_VERSION
        return PROJECT_MINOR_VERSION;
#endif          
    return 0;
}

/**
    Returns PROJECT_BUILD token value
     
    @param void

    @retval UINT32
     
**/
UINT32 GetFw_ProjectBuildVersion ()
{
#ifdef PROJECT_BUILD
        return PROJECT_BUILD;
#endif          
    return 0;
}

/**
    Returns PROJECT_TAG token value
     
    @param void

    @retval CHAR8 *
     
**/
CHAR8 * GetFw_ProjectTag ()
{
#ifdef PROJECT_TAG
        return CONVERT_TO_STRING(PROJECT_TAG);
#endif          
    return "";
}


/**
    To get the FW_VERSION from ACPI FID table 

    @param FW_VERSION *Fid

    @retval Firmware Get Status
**/

EFI_STATUS GetFirmwareData( FW_VERSION *Fid)
{
    static EFI_GUID FidFileName = FID_FFS_FILE_NAME_GUID;
    EFI_STATUS Status = EFI_NOT_FOUND;
    EFI_HANDLE *FvHandle = NULL;
    UINTN FvCount = 0;
    UINTN i = 0;
    UINTN BufferSize = 0;
    VOID *Buffer = NULL;

    Status = pBS->LocateHandleBuffer(ByProtocol, &gEfiFirmwareVolume2ProtocolGuid, NULL, &FvCount, &FvHandle);
    DEBUG ((DEBUG_ERROR,"\nJsonUtilityLib:EfiFVBlock2Guid Status = %r\n", Status));

    if (!EFI_ERROR (Status))
    {
		for(i = 0; i < FvCount; i++) 
		{
			EFI_FIRMWARE_VOLUME_PROTOCOL *Fv;
			UINT32 AuthStatus;
				
			Status = pBS->HandleProtocol(FvHandle[i], &gEfiFirmwareVolume2ProtocolGuid, (void **)&Fv);
	
			if (EFI_ERROR(Status)) 
				continue;
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:FVblockInterface Status = %r\n",Status));
			Buffer = 0;
			BufferSize = 0;
			Status = Fv->ReadSection(Fv, &FidFileName, EFI_SECTION_FREEFORM_SUBTYPE_GUID, 0, &Buffer, &BufferSize, &AuthStatus);
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ReadSection Status = %r\n",Status));
			if (!EFI_ERROR(Status)) 
			{
		           DEBUG((DEBUG_ERROR, "extracted section with guid %g\n", (EFI_GUID *)Buffer));
			   Buffer = (UINT8 *)Buffer + sizeof(EFI_GUID);
			   MemCpy(Fid, Buffer, sizeof(FW_VERSION));
			   Buffer = (UINT8 *)Buffer - sizeof(EFI_GUID);
			   pBS->FreePool(Buffer);
			   DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ACPI Fid Table Status = %r\n",EFI_SUCCESS));
			   Status= EFI_SUCCESS;
			   break;
			 }
		}
	}
    pBS->FreePool(FvHandle);
    return Status;
}

/**
    convert character array(not Null terminated string) into interger 

    @param CHAR8   *ProjectTag,UINT8 Length

    @retval UINT8
**/

UINT8 ConvCharArrayToNumber(CHAR8   *ProjectTag,UINT8 Length)
{
	UINT8   Index = 0,SubIndex = 0,Value = 0,CurPosVal = 0;
	
	if(ProjectTag)
	{
		for(Index =0 ;Index < Length;Index++)
		{
			CurPosVal = (ProjectTag[Index]-'0');
			if( (0 == CurPosVal) && (0 == Index))
				continue;
			for(SubIndex =Index ;SubIndex < (Length-1);SubIndex++)
				CurPosVal *= 10;
			
			Value += CurPosVal;
		}
	}
	return Value;
}


/**
    extract project tag from ACPI bios tag

    @param SysBuildInfo		Structure to find and fill project tag 	

    @return void
**/

void GetProjectTagFromBiosTag(SYSTEM_BUILD_INFO *SysBuildInfo)
{
    memcpy (SysBuildInfo->ProjectTag, SysBuildInfo->BiosTag, 5);
    
    //commented the below code because of following, first 5 bytes of BiosTag has ProjectTag
    //BIOS_TAG -> Help  = "BIOS Tag inserted into Firmware ID structure.\9 characters. \5 characters of project tag followed by 3 characters of project version and terminating zero"
    
/*    
	UINT8   Length = 0,BiostagLen = 0;
	UINT8   Index = 0,SubIndex =0;
	UINT8 MajorVersionIndex = 0;
	BOOLEAN VersionMatched = FALSE;
	CHAR8 *MajorVersionPtr= NULL;
	CHAR8 BiosTag [10];
	
	sprintf( (CHAR8*)(BiosTag) , "%d%d",SysBuildInfo->ProjectMajorVersion,SysBuildInfo->ProjectMinorVersion);

	MajorVersionPtr = strstr(SysBuildInfo->BiosTag,BiosTag);
	if(NULL != MajorVersionPtr)
	{
	   for(Index = 0;SysBuildInfo->BiosTag+Index != MajorVersionPtr;Index++)
		SysBuildInfo->ProjectTag[Index] = SysBuildInfo->BiosTag[Index];
	   SysBuildInfo->ProjectTag[Index] = '\0';
	}
	else
		SysBuildInfo->ProjectTag[0] = '\0';
*/
}


/**
     build SYSTEM_BUILD_INFO bufr from ACPI FID table or directly from respective macros

    @param SYSTEM_BUILD_INFO *SysBuildInfo

    @retval EFI_STATUS
**/

EFI_STATUS GetSystemBuildInfo(SYSTEM_BUILD_INFO *SysBuildInfo)
{
	FW_VERSION	FwVersionData;
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8  MinVerLength = 0,MajorVerLength = 0;
	UINT8   Index = 0,SubIndex = 0;
	
	if(NULL != SysBuildInfo)
	{
		Status = GetFirmwareData(&FwVersionData);
		
		if( EFI_SUCCESS == Status )
		{
			SysBuildInfo->ProjectBuildVersion = 0;//as no entry in ACPI FID table
			
			MinVerLength = sizeof(FwVersionData.ProjectMinorVersion)/sizeof(CHAR8) -1;
			SysBuildInfo->ProjectMinorVersion = ConvCharArrayToNumber(FwVersionData.ProjectMinorVersion,MinVerLength);
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ProjectMinorVersion(ACPI) = %d\n",SysBuildInfo->ProjectMinorVersion));
			MajorVerLength = sizeof(FwVersionData.ProjectMajorVersion)/sizeof(CHAR8) -1;
			SysBuildInfo->ProjectMajorVersion = ConvCharArrayToNumber(FwVersionData.ProjectMajorVersion,MajorVerLength);		
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ProjectMajorVersion(ACPI) = %d\n",SysBuildInfo->ProjectMajorVersion));
			strcpy (SysBuildInfo->BiosTag, FwVersionData.BiosTag);
			
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:BiosTag(ACPI) = %a\n",SysBuildInfo->BiosTag));
			GetProjectTagFromBiosTag(SysBuildInfo);
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ProjectTag(ACPI) = %a\n",SysBuildInfo->ProjectTag));
			SysBuildInfo->IsLoaded = TRUE;
			
		}
		else
		{
			SysBuildInfo->ProjectMajorVersion = GetFw_ProjectMajorVersion();
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ProjectMajorVersion(LOCAL) = %d\n",SysBuildInfo->ProjectMajorVersion));
			SysBuildInfo->ProjectMinorVersion = GetFw_ProjectMinorVersion();
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ProjectMinorVersion(LOCAL) = %d\n",SysBuildInfo->ProjectMinorVersion));
			SysBuildInfo->ProjectBuildVersion = GetFw_ProjectBuildVersion();
			
			strcpy ((CHAR8*)(SysBuildInfo->ProjectTag), GetFw_ProjectTag());
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:ProjectTag(LOCAL) = %a\n",SysBuildInfo->ProjectTag));
			sprintf( (CHAR8*)(SysBuildInfo->BiosTag) , "%s%d%d",SysBuildInfo->ProjectTag,SysBuildInfo->ProjectMajorVersion,SysBuildInfo->ProjectMinorVersion);
			DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:BiosTag(LOCAL) = %a\n",SysBuildInfo->BiosTag));
			SysBuildInfo->IsLoaded = TRUE;
			Status = EFI_SUCCESS;
		}
	}
	return Status;
}

/**
     return Bios Tag (combination of Project tag, project major and minor version)

    @param 

    @retval CHAR8*
**/

CHAR8* Get_BiosTag()
{
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR8* BiosTagStr8 = NULL;
	if(FALSE == gSysBuildInfo.IsLoaded)
	{
		Status = GetSystemBuildInfo(&gSysBuildInfo);
	}
	
	if((!EFI_ERROR (Status)) && (TRUE == gSysBuildInfo.IsLoaded))
	{
		BiosTagStr8 = gSysBuildInfo.BiosTag;
	}
	DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Get_BiosTag = %a\n",BiosTagStr8));
	return BiosTagStr8;
}


/**
     get Project Tag 

    @param 

    @retval CHAR8*
**/

CHAR8* Get_ProjectTag()
{
	EFI_STATUS Status = EFI_SUCCESS;
	CHAR8* ProjectTagStr8 = NULL;
	if(FALSE == gSysBuildInfo.IsLoaded)
	{
		Status = GetSystemBuildInfo(&gSysBuildInfo);
	}
	if((!EFI_ERROR (Status))&& (TRUE == gSysBuildInfo.IsLoaded))
	{
		ProjectTagStr8 = gSysBuildInfo.ProjectTag;
	}
	DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Get_ProjectTag = %a\n",ProjectTagStr8));
	return ProjectTagStr8;
}


/**
     get Project MajorVersion 

    @param 

    @retval UINT8
**/

UINT8 Get_ProjectMajorVersion()
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ProjectMajorVersion = 0;
	if(FALSE == gSysBuildInfo.IsLoaded)
	{
		Status = GetSystemBuildInfo(&gSysBuildInfo);
	}
	if((!EFI_ERROR (Status)) && (TRUE == gSysBuildInfo.IsLoaded))
	{
		ProjectMajorVersion = gSysBuildInfo.ProjectMajorVersion;
	}
	DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Get_ProjectMajorVersion = %d\n",ProjectMajorVersion));
	return ProjectMajorVersion;
}

/**
     get Project MinorVersion 

    @param 

    @retval UINT8
**/

UINT8 Get_ProjectMinorVersion()
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ProjectMinorVersion = 0;
	if(FALSE == gSysBuildInfo.IsLoaded)
	{
		Status = GetSystemBuildInfo(&gSysBuildInfo);
	}
	if((!EFI_ERROR (Status)) && (TRUE == gSysBuildInfo.IsLoaded))
	{
		ProjectMinorVersion = gSysBuildInfo.ProjectMinorVersion;
	}
	DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Get_ProjectMinorVersion = %d\n",ProjectMinorVersion));
	return ProjectMinorVersion;
}

/**
     get Project BuildVersion 

    @param 

    @retval UINT8
**/

UINT8 Get_ProjectBuildVersion()
{
	EFI_STATUS Status = EFI_SUCCESS;
	UINT8 ProjectBuildVersion = 0;
	if(FALSE == gSysBuildInfo.IsLoaded)
	{
		Status = GetSystemBuildInfo(&gSysBuildInfo);
	}
	if((!EFI_ERROR (Status)) && (TRUE == gSysBuildInfo.IsLoaded))
	{
		ProjectBuildVersion = gSysBuildInfo.ProjectBuildVersion;
	}
	DEBUG((DEBUG_ERROR, "\nJsonUtilityLib:Get_ProjectBuildVersion = %d\n",ProjectBuildVersion));
	return ProjectBuildVersion;
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
