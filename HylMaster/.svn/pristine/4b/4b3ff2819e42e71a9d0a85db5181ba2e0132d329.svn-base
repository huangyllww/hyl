//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************

/** @file HiiLibJsonHandler.c

**/

//---------------------------------------------------------------------------
#include "Efi.h"
#include <Token.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>
#include <Library/MemoryAllocationLib.h>
#include "JsonHiiLibSrc.h"
#include "UEFI21Variable.h"

//---------------------------------------------------------------------------

DATAVECTOR gHiiBiosSetupBuf = {
        NULL,
        NULL,
        0,
        0,
        RESERVE_SIZE
};


extern EFI_BOOT_SERVICES *gBS;
#define realloc 			HiiLib_MemReallocateZeroPool
HIILIB_JSON_DATA_FRMT_TYPE HiiPrevStyle = HII_JSON_NOT_SET;
CHAR8  *BoolArray[] = {"false","true"};

CHAR8 *gFailMapIdBuffer = (CHAR8 *)NULL;
UINTN gFailMapIdAllocBuffSize = 0;
UINTN  gFailMapIdBuffSize = 0;
//---------------------------------------------------------------------------

UINTN Swprintf(CHAR16 *Buffer, CHAR16 *Format, ...);
int sprintf(char *str, char const *fmt, ...);
VOID HiiLib_MemFreePointer( VOID **ptr );
CHAR8* HiiLib_GetJsonDataStr8(HIILIB_JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR8 *LeftNameStr,CHAR8 *RightValueStr);
//---------------------------------------------------------------------------

VOID* HiiLib_GetJsonDataStr(HIILIB_JSON_DATA_FRMT_TYPE Style,UINTN Lebel,VOID *LeftNameStr,VOID *RightValueStr)
{
	VOID *Str = NULL;
	
	_fltused              = 1;
	
	Str = HiiLib_GetJsonDataStr8(Style,Lebel,LeftNameStr,RightValueStr);
	
	return Str;
}

CHAR8* HiiLib_GetJsonDataStr8(HIILIB_JSON_DATA_FRMT_TYPE Style,UINTN Lebel,CHAR8 *LeftNameStr,CHAR8 *RightValueStr)
{
	CHAR8 *JsonNameValueStr = (CHAR8*)NULL;
	UINTN AllocStrlen = 0;
	CHAR8 NewEntry[128]= "\n";
	CHAR8 AdditionalEntry[128]= ",\n";
	CHAR8 *PrefixStr = NULL;
	CHAR8 Token[128];
	UINTN Index = 0;
//	static JSON_DATA_FRMT_TYPE HiiPrevStyle = JSON_NOT_SET;
	
	if( (1 == Lebel) && (Style==HII_JSON_OBJECT_OPEN))
		HiiPrevStyle = HII_JSON_NOT_SET;
	
	PrefixStr =  NewEntry;
	if(HiiPrevStyle != HII_JSON_NOT_SET)
	{
		switch(HiiPrevStyle)
		{
		case HII_JSON_DATA_BOOLEAN:
		case HII_JSON_DATA_INTEGER:
		case HII_JSON_DATA:
			if((HII_JSON_OBJECT_CLOSE != Style)&&(HII_JSON_ARRAY_CLOSE != Style))
				PrefixStr =  AdditionalEntry;
			break;
		case HII_JSON_OBJECT_CLOSE:
			if(HII_JSON_OBJECT_OPEN == Style)
				PrefixStr =  AdditionalEntry;
			break;
		case HII_JSON_ARRAY_CLOSE:
			if((HII_JSON_DATA == Style) ||(HII_JSON_DATA_INTEGER == Style))
				PrefixStr =  AdditionalEntry;
			break;
		}		
	}

	if( (1 == Lebel) && (Style==HII_JSON_OBJECT_OPEN))
	{
		PrefixStr[0] = '\0';
	}
	else
	{
		for(Index=1;Index < Lebel;Index++)
		{
			strcat(PrefixStr,"  ");
		}
	}
	
	switch(Style)
	{
	case HII_JSON_OBJECT_OPEN:
		if(NULL == LeftNameStr)
			strcpy(Token,"{");
		else
			strcpy(Token,": {");
		break;
	case HII_JSON_OBJECT_CLOSE:
			strcpy(Token,"}");
		break;
	case HII_JSON_ARRAY_OPEN:
		if(NULL == LeftNameStr)
			strcpy(Token,"[");
		else
			strcpy(Token,": [");
		break;
	case HII_JSON_ARRAY_CLOSE:
		strcpy(Token,"]");
		break;
	case HII_JSON_DATA_BOOLEAN:
	case HII_JSON_DATA_INTEGER:
	case HII_JSON_DATA:
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
		if(Style == HII_JSON_DATA_INTEGER)
			sprintf( JsonNameValueStr , "%s\"%s\"%s%s",PrefixStr,LeftNameStr,Token,RightValueStr);
		else if( Style == HII_JSON_DATA_BOOLEAN )
			sprintf( JsonNameValueStr , "%s\"%s\"%s%s",PrefixStr,LeftNameStr,Token,BoolArray[HiiLib_Rbatoi_base (RightValueStr, 10)]);
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
	
	HiiPrevStyle = Style;
	return JsonNameValueStr;
}

EFI_STATUS HiiLib_ReAllocString8(DATAVECTOR *SrcVector,UINTN RequireSize)
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
						HiiLib_MemFreePointer(&(SrcVector->_String8) );
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

EFI_STATUS HiiLib_AppendString8(DATAVECTOR *SrcVector,CHAR8 *AppendStr)
{
	UINTN RequireSize = 0;
	EFI_STATUS AppendStatus = EFI_SUCCESS;
	
	if((NULL != AppendStr) && (NULL != SrcVector))
	{
		RequireSize = strlen(AppendStr);
		AppendStatus = HiiLib_ReAllocString8(SrcVector,RequireSize);
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
EFI_STATUS HiiLib_AppendString(DATAVECTOR *SrcVector,VOID *AppendStr)
{
    EFI_STATUS AppendStatus = EFI_SUCCESS;
    
    AppendStatus = HiiLib_AppendString8(SrcVector,AppendStr);
    
    return AppendStatus;
}
void HiiLib_UpdateData(CHAR8 **String)
{   
    HiiLib_AppendString(&gHiiBiosSetupBuf,*String);
    HiiLib_MemFreePointer((VOID**)String);
}
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   atoi_base
//
// Description: function to convet a string to integer
//
// Input:   CHAR16 *string, UINT8 base
//
// Output:  integer
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINTN HiiLib_Rbatoi_base (CHAR8 *string, UINT8 base)
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
    Saves MapIds if failed to set to configuration

    @param CHAR8*

    @retval void  

**/
void SaveFailedMapId (CHAR8 *tempBuffer)
{
    UINTN tempSize;
    CHAR8 AddComma[2] = {0};
    UINTN TempAllocSize = 0;
    CHAR8 Buffer[512];
    
    if( 0 != gFailMapIdBuffSize)
        strcpy (AddComma, ",");
    sprintf(Buffer,"%s\n\t\"%s\"",AddComma,tempBuffer);
    tempSize = strlen(Buffer);
    if (0 == tempSize)  
    {
        return;
    }
    
    gFailMapIdBuffSize += tempSize;
    if (gFailMapIdBuffSize >= gFailMapIdAllocBuffSize)
    {
        TempAllocSize = (tempSize >= MEMORY_ALLOC_UNIT) ? (tempSize + 100) : MEMORY_ALLOC_UNIT;     //Extra 100 for safety
        gFailMapIdBuffer = HiiLib_MemReallocateZeroPool( gFailMapIdBuffer, gFailMapIdAllocBuffSize, gFailMapIdAllocBuffSize + TempAllocSize);
        if (NULL == gFailMapIdBuffer)
        {
            return;
        }
        gFailMapIdAllocBuffSize += TempAllocSize;
    }
    strcat (gFailMapIdBuffer, Buffer);
}

//*****************************************************************
//*****************************************************************
//**                                                             **
//**        (C)Copyright 1985-2016, American Megatrends, Inc.    **
//**                                                             **
//**                       All Rights Reserved.                  **
//**                                                             **
//**    5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093 **
//**                                                             **
//**                       Phone (770)-246-8600                  **
//**                                                             **
//*****************************************************************
//*****************************************************************
