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

/** @file FwConfigFileOperation.c
    Contains implementation of file operations

**/

//---------------------------------------------------------------------------

#include <AmiDxeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "Library/UefiLib.h"
#include "Library/AmiJsonLib.h"
#include "BiosAttribValueLibHelper.h"
#include "BiosAttribTypeLib.h"
#include "BiosAttribValueLib.h"
#include "JsonHiiLibraryApi.h"
#include "FirmwareConfigDrvSrc.h"
#include <math.h>
#include <Library/DebugLib.h>

char                *gPwdMapId = NULL;
CHAR16              *gNewPassword = NULL;
CHAR16              *gOldPassword = NULL;

extern BOOLEAN                 gNvramChanged;

void ParseArray( cJSON *item,JsonImportType ImportType );
static char * ParseObject( cJSON *item,JsonImportType ImportType );
static char *ParseValue( cJSON *item , JsonImportType ImportType );
EFI_STATUS CheckForPasswordUpdates( CHAR8 *Buffer,  CHAR8 **Value, UINTN *ValueSize, JsonImportType ImportType);

BOOLEAN bMapIDArray = FALSE;
CHAR16 *MapString = (CHAR16 *)NULL;

extern BOOLEAN             gResetBiosConfig;

/**
    Duplicates a String from CHAR8 to CHAR16

    @param CHAR8* string 

    @retval CHAR16 *
**/
CHAR16 *Str8to16( CHAR8 *string )
{
    CHAR16  *text;
    UINTN   i;

    text = AllocateZeroPool ( (1 + strlen ( string )) * sizeof(CHAR16));
    if ( text != NULL )
    {
        i=0;
        while(text[i] = (CHAR16)string[i])
            i++;
        
        return text;
    }
    return NULL;
}

/**
    Render an array to text

    @param cJSON *item

    @retval void 


**/
void  ParseArray ( cJSON *item, JsonImportType ImportType )
{
    char *out=0 ;
    cJSON *child=item->child;
    int numentries=0;
    
    /* How many entries in the array? */
    while (child) numentries++,child=child->next;
    if (!numentries)
    {
        return ;
    }
    
    /* Retrieve all the results: */
    child=item->child;
    while (child )
    {
        ParseValue(child, ImportType );
        child=child->next;
    }
    
    return ; 
}    

/**
    Render an object to text

    @param cJSON *item

    @retval static char * 

**/
static char * ParseObject(cJSON *item,JsonImportType ImportType)
{
    char *str,*ret;
    cJSON *child=item->child;
    int numentries=0,fail=0;
    EFI_STATUS Status = EFI_NOT_FOUND;
    
    /* Count the number of entries. */
    while (child) numentries++,child=child->next;
    if (!numentries)
    {
        return (char *)EFI_NOT_FOUND;
    }
    /* Collect all the results into our arrays: */
    child=item->child;
    while (child)
    {
        UINTN BufferSize = 0;
        
        str = child->string;
        
        if( PASSWORDIMPORT == ImportType )//Update the MapString name
        {
            if( cJSON_Object == child->type)
                gPwdMapId = str;
        }
        else if( !Strcmp(str,"Attributes"))//Compare the configuration Array name
        {
            bMapIDArray = TRUE;
        }
        
        ret = ParseValue(child, ImportType );
        
        if ((RESETBIOSCONFIG == ImportType) && (!Strcmp (str, "ResetBios")))//Update the MapString name
        {
            if ((NULL != ret) && (!Strcmp (ret, "true")))
            {
                gResetBiosConfig = TRUE;
                Status = EFI_SUCCESS;
                break;
            }
        }
        
        //Check for password value updates
        Status = CheckForPasswordUpdates(str, &ret, &BufferSize, ImportType);
        
        if(bMapIDArray)
        {
            MapString = Str8to16( str );
            if( MapString ) //Compare the mapstring value Token
            {
                CHAR8   *MappingId8 = NULL;
                MappingId8 = ConvertChar16toChar8(MapString);
                
                //Update control value length
                if( PASSWORDIMPORT != ImportType )
                    BufferSize = strlen(ret);
                
                gNvramChanged = TRUE;
        
                Status = SetHiiMapStringValue (MapString, ret, BufferSize);
                if( (EFI_SUCCESS == Status))/* && (FALSE == gInternalTables) )*/
                {
                    DEBUG((DEBUG_ERROR, "gInternalTables = TRUE\n"));
                    //gInternalTables = TRUE;
                }
                
                FreePool(MapString);
                FreePool(MappingId8);
                MapString = NULL;
                if( PASSWORDIMPORT == ImportType )
                {
                    //Free the allocate buffers
                    gPwdMapId = NULL;
                    FwMemFreePointer(&gNewPassword);
                    FwMemFreePointer(&gOldPassword);
                    FwMemFreePointer(&ret);
                    bMapIDArray = FALSE;
                }
            }
        }
        child=child->next;
  }
  return (char *)Status; 
}

/**
    Render the number nicely from the given item into a string

    @param cJSON *item

    @retval static char * 

**/
static char *ParseNumber(cJSON *item)
{
    char *str=0;
        
    if(item->num_sub_type == cJSON_NumberInt){
        if (item->valueint==0)
        {
            str=(char*)AllocatePool(2); /* special case for 0. */
            if (str) strcpy(str,"0");
        }
        else 
        {
            str=(char*)AllocatePool(21);    /* 2^64+1 can be represented in 21 chars. */
            if (str) sprintf(str, "%lld", item->valueint);
        }
    }
    else{
        if (item->valuedouble==0)
        {
            str=(char*)AllocatePool(2); /* special case for 0. */
            if (str) strcpy(str,"0");
        }
        else
        {
            str=(char*)AllocatePool(64);    /* This is a nice tradeoff. */
            if (str)
            {
                if (fabs(floor(item->valuedouble)-item->valuedouble)<=DBL_EPSILON && fabs(item->valuedouble)<1.0e60)sprintf(str, "%.0f", item->valuedouble);
                else if (fabs(item->valuedouble)<1.0e-6 || fabs(item->valuedouble)>1.0e9) sprintf(str, "%e", item->valuedouble);
                else sprintf(str, "%f", item->valuedouble);
            }
        }
    }
    return str;
}

/**
    Returns the Token type values 

    @param cJSON *item

    @retval static char * 

**/
static char* Parsestrdup(const char* str)
{
      size_t len;
      char* copy;

      len = Strlen(str) + 1;
      if (!(copy = (char*)AllocatePool(len))) 
          return 0;
      MemCpy(copy,str,len);
      return copy;
}

/**
    Render a value to text.

    @param cJSON *item

    @retval static char * 

**/
static char *ParseValue (cJSON *item,JsonImportType ImportType )
{
    char *out=0;
    if (!item) return 0;
    
    switch ((item->type)&255)
    {
        case cJSON_NULL: 
            out = Parsestrdup("null");
            break;
        case cJSON_False: 
            out = Parsestrdup("false");
            break;
        case cJSON_True: 
            out = Parsestrdup("true");
            break;
        case cJSON_Number:
            out = ParseNumber(item);
            break;
        case cJSON_String: 
            out = item->valuestring;
            break;
        case cJSON_Array: 
            ParseArray(item, ImportType);
            bMapIDArray = FALSE;
            break;
        case cJSON_Object:
            out = ParseObject(item, ImportType);
            break;
    }
    return out;
}

/**
    Checks for password import flag and updates the encrypted password values
     
    @param Buffer       Mapstring name
    @param Value        Value for mapstring controls to be returned
    @param ValueSize    Value size for mapstring controls to be returned
    @param ImportType   specifies the import type

    @retval EFI_STATUS return status
     
**/ 

EFI_STATUS CheckForPasswordUpdates( CHAR8 *Buffer,  CHAR8 **Value, UINTN *ValueSize, JsonImportType ImportType)
{
    EFI_STATUS Status = EFI_SUCCESS;

    if( PASSWORDIMPORT == ImportType )//Check for password import type
    {
        if( gPwdMapId && !Strcmp( Buffer,gPwdMapId ))
        {
            FIRWARE_CONFIG_ATTRIB_PROTOCOL *FirmwareConfigurationProtocol = (FIRWARE_CONFIG_ATTRIB_PROTOCOL *)NULL;
            CHAR16 *PwdMapString = ( CHAR16 *)NULL;
            CHAR8 *EncryptedPwd = (CHAR8 *)NULL;
            UINTN PwdSize = 0;

            //Validate the passwords and get the encrypted data
            PwdMapString = Str8to16( gPwdMapId );
            
            Status = pBS->LocateProtocol (&gFirmwareConfigAttribGuid, NULL, (void**)&FirmwareConfigurationProtocol);
            if (!EFI_ERROR (Status))
            {
             //Get the encrypted value for password
             Status = FirmwareConfigurationProtocol->GetPasswordEncryptedData(PwdMapString, gNewPassword,gOldPassword, &EncryptedPwd,&PwdSize);
             DEBUG((DEBUG_ERROR, "GetPasswordEncryptedData Status:%x\n",Status)); 
            }
            
            FreePool(PwdMapString);
            
            if( !EFI_ERROR(Status))
            {
                bMapIDArray = TRUE;
                
                //Update MapId and Value
                *Value = EncryptedPwd;
                *ValueSize = PwdSize;
            }
            else
                bMapIDArray = FALSE;
        }
        else if( !Strcmp( Buffer, "NewPassword") )
        {
           gNewPassword = Str8to16( *Value );
        }
        else if( !Strcmp( Buffer, "OldPassword") )
        {
           gOldPassword = Str8to16( *Value );
        } 
    }
   
   return Status;
}

/**
     Parsed the JSON output and imports the mapstring values.

     @param void

     @retval EFI_STATUS 

**/
EFI_STATUS ParseJsonandImport (CHAR8 *IndexBuffer, UINT32 IndexBufferSize, JsonImportType ImportType )
{
   char         *RetValue=0;
   cJSON        *ConfigDataArray =  (cJSON *)NULL;
   EFI_STATUS   Status = EFI_NOT_FOUND;
       
   DEBUG ((DEBUG_ERROR, "\nImportIndexJsonData ImportFileData JsonBufferSize:%d",IndexBufferSize));
   if (NULL == IndexBuffer || 0 == IndexBufferSize)
   {
       return EFI_UNSUPPORTED;
   }
   gNvramChanged = FALSE;

   ConfigDataArray = cJSON_Parse( IndexBuffer );
   DEBUG ((DEBUG_ERROR, "\nImportIndexJsonData cJSON_Parse ConfigDataArray:%x",ConfigDataArray));
       
   if (NULL != ConfigDataArray)
   {
       RetValue = ParseValue (ConfigDataArray, ImportType);     //Parse the linked list and import the values
       cJSON_Delete (ConfigDataArray);                          //Free the Json data list

       if(RetValue)
           return EFI_OUT_OF_RESOURCES;
       else
           Status = EFI_SUCCESS;
   }
   
   return Status;
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
