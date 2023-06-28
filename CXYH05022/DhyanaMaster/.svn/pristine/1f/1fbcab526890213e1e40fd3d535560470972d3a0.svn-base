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

/** @file IndexCreationSrc.c
    Contains implementation for creating index json file with all the mapping strings and it's current values

**/

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <sys/types.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/MemoryAllocationLib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Library/UefiLib.h"
#include <Protocol/SimpleFileSystem.h>
#include "BiosAttribValueLibHelper.h"
#include "BiosAttribValueLib.h"
#include "Token.h"
#include "BiosAttribTypeLib.h"
#include "JsonHiiLibraryApi.h"
#include "BiosAttribConfigElinks.h"
#include "JsonHiiLibSrc.h"

extern EFI_BOOT_SERVICES 	*gBS;
extern EFI_RUNTIME_SERVICES *gRT;//<kuaibai-20180708>
#define MAX_MAPID_LENGTH 128//<xuyj1-20200212 add oob function +>
//---------------------------------------------------------------------------

VECTOR gBiosIndexSetupBuf = {
        NULL,
        NULL,
        0,
        0,
        RESERVE_SIZE
};

//---------------------------------------------------------------------------

VOID        *MemReallocateZeroPool( VOID *oldBuffer, UINTN oldSize, UINTN newSize );
UINTN       swprintf ( OUT CHAR16 *Buffer, IN UINTN BufferSize, IN CONST CHAR16 *Format, ... );
EFI_STATUS  ConstructAttributeList ();
VOID        FcMemFreePointer( VOID **ptr );
BOOLEAN     IsFcMapIdSupported (CHAR16 *MapString);
EFI_STATUS ConstructBootOptionList (CHAR8   **BootOptStr);
CHAR16       *SkipCharInString(CHAR16 *String8, CHAR16 *ch);

//---------------------------------------------------------------------------

extern EFI_BOOT_SERVICES    *pBS;
extern EFI_HANDLE gImageHandle;
//extern BOOLEAN gPldmAvailable;
extern BOOLEAN gRedfishEnabled;
extern  BOOT_OPTION **gBootOptions;
extern  UINT64      gBootCount;

//---------------------------------------------------------------------------




/**
    write Json header information to the VECTOR buffer
    @retval void     
**/ 

void FcDump_JsonHeaderInfo()
{
	 CHAR8   TempBuffer[4096] = {""};
	 //<xuyj1001-20200401 Add BIOS OOB Function +>
	 sprintf(TempBuffer,"\n\t\"@Redfish.Copyright\": \"Copyright 2017 Distributed Management Task Force, Inc. (DMTF). All rights reserved.\",\
                    \n\t\"@odata.context\": \"/redfish/v1/$metadata#Systems/Self/Bios\",\
                    \n\t\"@odata.id\": \"/redfish/v1/Systems/Self/Bios\",\
                    \n\t\"@odata.type\": \"#Bios.1.0.0.Bios\",\
                    \n\t\"Id\": \"Bios\",\
                    \n\t\"Name\": \"BIOS Configuration Current Settings\",\
                    \n\t\"Description\": \"BIOS Configuration Current Settings\",\
                    \n\t\"AttributeRegistry\": \"BiosAttributeRegistry%s.%d.%d.%d\",",
                    Get_ProjectTag (), (int)Get_ProjectMajorVersion (),(int)Get_ProjectMinorVersion (), (int)Get_ProjectBuildVersion ());
	 //<xuyj1001-20200401 Add BIOS OOB Function ->
	 AppendString(FALSE,&gBiosIndexSetupBuf,TempBuffer);
}

/**
    write Json MAPID header information to the VECTOR buffer
    @retval void     
**/ 

void Dump_MapIdHeaderInfo()
{
    CHAR8   TempBuffer[4096] = {""};
    sprintf(TempBuffer, ",\n  \"@Redfish.Settings\": {\
                        \n\t\"@odata.type\": \"#Settings.1.0.0.Settings\",\
                        \n\t\"ETag\": \"someetag\",\
                        \n\t\"Messages\": [\n\t\t\t{\
                        \n\t\t\t\"MessageId\": \"Base.1.0.SettingsFailed\",\
                        \n\t\t\t\"RelatedProperties\": [\n\t\t\t\t]\
                        \n\t\t}\n\t\t]\
                        \n\t}");
    AppendString(FALSE,&gBiosIndexSetupBuf,TempBuffer);
}


/**
    get mapid string count from the list
    @param MapStringList 
    @param MapListSize
    @retval UINTN
**/

UINTN GetMapIdCount( UINT8 *MapStringList ,  UINT32 MapListSize)
{
	UINTN MapStringCount = 0;
	UINT8       MapIdLength = 0;
	UINT32      Offset = 0;
	HII_MAPIDINFO		*MapIdInfo =  (HII_MAPIDINFO *)MapStringList;
	
	for( ; Offset < MapListSize-4; Offset += sizeof(HII_MAPIDINFO)+MapIdLength )
	{   
		//Obtain string length
		MapIdLength = *(MapStringList + sizeof(MapIdInfo->ControlType));
		
		//Set buffer pointer at the start of next structure.
		MapStringList += sizeof(HII_MAPIDINFO)+MapIdLength;
		MapStringCount++;
	}
	
	return MapStringCount;
}

/**
    check for valid mapid string
    @param TotalMapIdList 
    @param MappingId16
    @param MapIdNumber
    @retval BOOLEAN
**/

BOOLEAN IsMapIdPresent(CHAR16 *TotalMapIdList, CHAR16 *MappingId16, UINTN MapIdNumber)
{
	UINTN j = 0;
	
    if (TRUE == CheckForSpaceInMappingId (MappingId16))
    {
        return TRUE;
    }
    
	if( (NULL == TotalMapIdList) || (0 == MapIdNumber) )
	{
		return FALSE;
	}
	
	while( j < MapIdNumber )
	{
		if( !Wcscmp(TotalMapIdList+(MAX_MAPID_LENGTH*j),MappingId16) )
		{
			return TRUE;
		}
		j++;
	}
	
	return FALSE;
}

/**
    Obtains the values from Pldm tables excluding password control and saves the values in a buffer.
    @param  void
    @retval Status
**/
EFI_STATUS ConstructAttributeList ()
{
    EFI_STATUS  Status = EFI_INVALID_PARAMETER;
    UINT8       *MapIdList, *TempMapIdList;
    UINT32      MapIdListSize = 0;
    UINT32      Offset = 0;
    CHAR16      *MappingId16 = NULL;
    CHAR8       *MappingId8 = NULL;
    UINT16       ControlType = 0;
    UINT8       MapIdLength = 0;
    UINTN       BufferSize     = 0;
    void        *Buffer = NULL;
    HII_MAPIDINFO		*MapIdInfo;
   // CHAR8        TempBuffer[1024] = {""};
    CHAR8 		*JsonStr8 = (CHAR8*)NULL;
    CHAR16 		*TotalMapIdList = (CHAR16 *)NULL;
    UINTN 		MapIdCount = 0;
    UINTN 		MapIdIndex = 0;
    UINTN 		i = 0;
    BOOLEAN 	MapIdPresent = FALSE;
    UINTN 		MapIdSize = MAX_MAPID_LENGTH;   //huangrj-20230614 fix creating current_setting.Json file may occur exception +>
    CHAR8		*EmptyPwd = NULL;
    
    //gPldmAvailable = FALSE;
    Status = GetHiiMapStringList (&MapIdList, &MapIdListSize);
    if (EFI_ERROR (Status))
    {
        return Status;
    }
    //gMapIdCount = 0;
    TempMapIdList = MapIdList;
    
    MapIdCount = GetMapIdCount(MapIdList,MapIdListSize);
    if( 0 == MapIdCount )
    {
    	return EFI_NOT_FOUND;
    }
    TotalMapIdList = (CHAR16 *)AllocateZeroPool( (MapIdCount+1) * (MapIdSize*sizeof(CHAR16)) );
    if( NULL == TotalMapIdList )
	{
		return EFI_OUT_OF_RESOURCES;
	}
    
    //Make the Token enabled to get the Redfish enabled changes from Pldm library
   // gRedfishEnabled = TRUE;
    
    MapIdInfo = (HII_MAPIDINFO *)MapIdList;
    //excluding last 4 bytes and size of HII_MAPIDINFO since CRC and padding will be added at the end of the buffer.
    
    for( ; Offset < (MapIdListSize - 4 - sizeof(HII_MAPIDINFO)); Offset += sizeof(HII_MAPIDINFO)+MapIdLength )
    {   
    	MapIdPresent = FALSE;
        //Obtain string length and Mapping Id from the location after the structure HII_MAPIDINFO.
        MapIdLength = *(MapIdList + sizeof(MapIdInfo->ControlType));
        MappingId16 = AllocateZeroPool((MapIdLength+1)*sizeof(CHAR16));
        MemCpy(MappingId16, MapIdList+sizeof(HII_MAPIDINFO), MapIdLength);
        
        //Check to export the MapId or not
        if( FALSE ==  IsFcMapIdSupported(MappingId16 ))
        {
            MapIdList += sizeof(HII_MAPIDINFO)+MapIdLength;
            FreePool(MappingId16);
            continue;
        }
                
        //Check the mapid list and skip repeated mapstrings 
        MapIdPresent = IsMapIdPresent(TotalMapIdList,MappingId16,MapIdIndex);
        if(TRUE == MapIdPresent)
        {
			//Set buffer pointer at the start of next structure.
			MapIdList += sizeof(HII_MAPIDINFO)+MapIdLength;
			FreePool(MappingId16);
        	continue;
        }
        
        //huangrj-20230614 fix creating current_setting.Json file may occur exception +>
        //Wcscpy(TotalMapIdList+(MapIdSize*MapIdIndex),MappingId16);
        MemCpy(TotalMapIdList+(MapIdSize*MapIdIndex),MappingId16, MapIdSize * sizeof(CHAR16));
        //huangrj-20230614 fix creating current_setting.Json file may occur exception ->
        
        MapIdIndex++;
       
        Status = GetHiiMapStringValue (MappingId16, &Buffer, &BufferSize, &ControlType);
        if ((EFI_SUCCESS == Status))    // && (0x2 != ControlType))
        {                           
            MappingId8 = RemoteConvertChar16toChar8(MappingId16);  
            //////////////////////
            // For numeric control
            if( (EFI_IFR_NUMERIC_OP == ControlType) || (EFI_IFR_CHECKBOX_OP == ControlType) )
            {
            	JsonStr8 = GetJsonDataStr(FALSE, JSON_DATA_INTEGER,3,MappingId8,Buffer); 
            }
            else if ( EFI_IFR_PASSWORD_OP == ControlType )
            {
            	EmptyPwd = (CHAR8 *)AllocateZeroPool (BufferSize);
                if(NULL == EmptyPwd)
            	{
            		continue;
            	}
                if (memcmp (EmptyPwd, Buffer, BufferSize))
                {
                	JsonStr8 = GetJsonDataStr(FALSE, JSON_DATA,3,MappingId8,"***");
                }
                else
                {
                	JsonStr8 = GetJsonDataStr(FALSE, JSON_DATA,3,MappingId8,"");
                }
                FreePool (EmptyPwd);
            }
            else
            {
            	JsonStr8 = GetJsonDataStr(FALSE, JSON_DATA,3,MappingId8,Buffer); 
            }
            FcUpdateData(&JsonStr8);
            ///////////////////////////////////   
            if( Buffer )      	
            FreePool(Buffer);
            FreePool(MappingId8);
        }
        
        //Set buffer pointer at the start of next structure.
        MapIdList += sizeof(HII_MAPIDINFO)+MapIdLength;
        FreePool(MappingId16);
    }
    //gRedfishEnabled = FALSE;
    
    free (TempMapIdList);
    FreePool(TotalMapIdList);
       
    ConstructBootOptionList (&JsonStr8);
    if(NULL != JsonStr8)
    {
		AppendString(FALSE,&gBiosIndexSetupBuf,",\n\t");
		FcUpdateData(&JsonStr8);
    }
    
    return EFI_SUCCESS; 
}

/**
    Obtains the boot options and index values from NVRAM and saves the values in a buffer.
    @param  void
    @retval Status
**/
EFI_STATUS ConstructBootOptionList (CHAR8   **BootOptStr)
{
    UINT8    StrLength = 0; 
    CHAR16   *OnOffStr[] = {L"false",L"true"};
    CHAR16   *JsonStr = NULL, *BootOptionMapId = NULL;
    CHAR8   *JsonStr8 = NULL;
    UINTN    BufferSize = 2048, i;
    UINT16   Index = 0;
    UINTN   size = 0;
    EFI_GUID    BootOptNameGuid = {0x5b5f93b7, 0x8f58, 0x47ce, 0x80, 0x3b, 0x48, 0x23, 0x6e, 0x80, 0x31, 0xaf};
    EFI_STATUS 	Status = EFI_INVALID_PARAMETER;
    
    JsonStr = (CHAR16 *)AllocateZeroPool ( BufferSize * sizeof(CHAR16) );
    //<Wangbn002-20191231-Fixed POST hang 92 issue, OOB memory overflow caused exception 0D +>
        //JsonStr8 = (CHAR8 *)AllocateZeroPool ( BufferSize );
        //if ( (NULL != JsonStr) && (NULL != JsonStr8) )
        if ( NULL != JsonStr )
	// if ( (NULL != JsonStr) && (NULL != JsonStr8) )
    //<Wangbn002-20191231-Fixed POST hang 92 issue, OOB memory overflow caused exception 0D ->
   
    {          
        //<OEMDBG-hujf1001-20201105 Fix system hang when no boot device insert +>
        Status = _PopulateGlobalBootOptions();    
        if( EFI_SUCCESS != Status)
            goto DONE;
        //<OEMDBG-hujf1001-20201105 Fix system hang when no boot device insert ->
		//<kuaibai-20180708> Status = pRS->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, BootOptionMapId);
		Status = gRT->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, BootOptionMapId);
		if( EFI_BUFFER_TOO_SMALL == Status)
		{
			BootOptionMapId =  AllocateZeroPool((size+1) * sizeof(CHAR16));
			//<kuaibai-20180708> Status = pRS->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, BootOptionMapId);
			Status = gRT->GetVariable (L"BootOptMapId", &BootOptNameGuid, NULL, &size, BootOptionMapId);
		}

		if( EFI_ERROR(Status) )
		{
		    //<Wangbn002-20191231-Fixed POST hang 92 issue, OOB memory overflow caused exception 0D +>
		                //FcMemFreePointer((VOID**)&JsonStr8);
		    //<Wangbn002-20191231-Fixed POST hang 92 issue, OOB memory overflow caused exception 0D ->
		                goto  DONE;
		            }
		    //<Wangbn002-20191231-Fixed POST hang 92 issue, OOB memory overflow caused exception 0D +>
		            JsonStr8 = (CHAR8 *)AllocateZeroPool ( gBootCount * 200 );  //Assuming the length of the single boot option is 200.
		            if (NULL == JsonStr8) {
		                goto  DONE;
		            }
		    //<Wangbn002-20191231-Fixed POST hang 92 issue, OOB memory overflow caused exception 0D ->
		
		strcpy(JsonStr8,"\"");
        strcat( JsonStr8, RemoteConvertChar16toChar8(BootOptionMapId));
        strcat( JsonStr8, "\":\"");
   
   		//Skip comma (,) if there and save boot name, boot order index, and enabled/disabled as a string
        for(i = 0; i < gBootCount; i++, Index+=2)
        {
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB +>
//kuaibai add update >>>
            if (gBootOptions[i]->Active & LOAD_OPTION_HIDDEN) //If hidden option don't send it to server
            {
                continue;
            }
            //If we are not AND with LOAD_OPTION_ACTIVE then any other value of OnOffStr will corrupt the boot option string
            //Spiral swprintf ( JsonStr, BufferSize, L"%S,0x%02X%02X,%S;", SkipCharInString( gBootOptions[i]->Name, L"," ) , gBootOrder[Index+1], gBootOrder[Index], OnOffStr[gBootOptions[i]->Active]);
            swprintf ( JsonStr, BufferSize, L"%S,0x%02X%02X,%S;", SkipCharInString( gBootOptions[i]->Name, L"," ) , gBootOrder[Index+1], gBootOrder[Index], OnOffStr[gBootOptions[i]->Active & LOAD_OPTION_ACTIVE]);
//kuaibai add update <<<
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB ->
            strcat(JsonStr8, RemoteConvertChar16toChar8(JsonStr));
        }
        
        strcat(JsonStr8,"\"");
DONE:               
        FcMemFreePointer((VOID**)&JsonStr);
        FcMemFreePointer((VOID**)&BootOptionMapId);
    }

    *BootOptStr = JsonStr8;
    return EFI_SUCCESS; 
}

/**
    Skips character from a string returns the string
    @param  CHAR16 * - String
    @param  CHAR16 * - character to be skipped
    @retval  CHAR16 * - modified string
**/
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB +>
// kuaibai add update 
CHAR16* SkipCharInString(CHAR16 *String16, CHAR16 *ch)
{
    CHAR16 *OutString16 = (CHAR16 *)NULL;
    UINTN Strlen16 = 0;
    UINTN Index = 0, SubIndex = 0;
    
    if ((NULL == String16) || (*String16 == '\0'))
    {
        return (CHAR16*)NULL;
    }
    
    if( NULL == StrStr(String16,ch) )
        return String16;
    
    Strlen16 = StrLen(String16);
    //zanggy-20210928-Fix exception when insert kinston U disk+>
    OutString16 = AllocateZeroPool((Strlen16+1) * sizeof (CHAR16));
    //zanggy-20210928-Fix exception when insert kinston U disk->
    while(String16[Index] != '\0')
    {
        if( *ch != String16[Index] )
        {
            OutString16[SubIndex++] = String16[Index];
        }
        Index++;
    }

    OutString16[SubIndex] = '\0';
        
    return OutString16;
}
//kuaibai add update
//<kuaibai001-20180813 Fix hang when importing bootorder changes from OOB ->
/**
    Create Index json file
    @param JsonbufSize 
    @retval CHAR8*
**/

EFI_STATUS CreateIndexJsonFile (OUT CHAR8 **Out_JsonBuffer,OUT UINTN *Out_JsonbufSize)
{
	CHAR8 *JsonStr8 = (CHAR8*)NULL;
	CHAR8 *JsonBuffer = (CHAR8*)NULL;
	UINTN JsonbufSize = 0;
	EFI_STATUS Status = EFI_SUCCESS;

	if((NULL == Out_JsonBuffer) && (NULL == Out_JsonbufSize) )
		return EFI_INVALID_PARAMETER;

	*Out_JsonBuffer = NULL;
	*Out_JsonbufSize = 0;

	if(NULL == gBiosIndexSetupBuf._String8)
	{
		JsonStr8 = GetJsonDataStr(FALSE, JSON_OBJECT_OPEN,1,(CHAR8*)NULL,(CHAR8*)NULL);
		FcUpdateData(&JsonStr8);
		FcDump_JsonHeaderInfo();
		JsonStr8 = GetJsonDataStr(FALSE, JSON_OBJECT_OPEN,2,(CHAR8*)"Attributes",(CHAR8*)NULL);
		FcUpdateData(&JsonStr8);
		Status = ConstructAttributeList();
		JsonStr8 = GetJsonDataStr(FALSE, JSON_OBJECT_CLOSE,2,(CHAR8*)NULL,(CHAR8*)NULL);	
		FcUpdateData(&JsonStr8);
        Dump_MapIdHeaderInfo();
        FcUpdateData(&JsonStr8);		
		JsonStr8 = GetJsonDataStr(FALSE, JSON_OBJECT_CLOSE,1,(CHAR8*)NULL,(CHAR8*)NULL);
		FcUpdateData(&JsonStr8);
	}
	if(NULL != gBiosIndexSetupBuf._String8)
	{
		*Out_JsonBuffer = gBiosIndexSetupBuf._String8;
		*Out_JsonbufSize = gBiosIndexSetupBuf._SizeUsed;
	}
	FcCleanUpBiosSetup();
//	UpdateFailedMapIds(JsonBuffer);
	
	return Status;
}

/**
    Clean VECTOR buffer
    @retval void
**/

void FcCleanUpBiosSetup()
{
	if(NULL != gBiosIndexSetupBuf._String8)
	{
		gBiosIndexSetupBuf._String8 = NULL;
	}
	gBiosIndexSetupBuf._SizeFree =0;
	gBiosIndexSetupBuf._SizeUsed = 0;
}

/**
    append string to VECTOR buf
    @param String 
    @retval void
**/

void FcUpdateData(CHAR8 **String)
{	
	AppendString(FALSE,&gBiosIndexSetupBuf,*String);
	FcMemFreePointer((VOID**)String);
}

/**
    Returns TRUE if the MapString is not in Suppress MapIdList
     
    @param CHAR16 *MapString

    @retval BOOLEAN
     
**/
BOOLEAN IsFcMapIdSupported(CHAR16 *MapString)
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

VOID FcMemFreePointer( VOID **ptr )
{
	if ( ( ptr == NULL ) || ( *ptr == NULL ) )
		return;

	pBS->FreePool( *ptr );
	*ptr = NULL;
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
