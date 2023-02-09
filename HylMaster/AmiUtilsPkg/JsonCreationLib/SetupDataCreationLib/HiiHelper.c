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

/** @file TseFunctionsInit.c
    Contains implementation for creating index json file with all the mapping strings and it's current values

**/

//---------------------------------------------------------------------------

#include "SetupDataCreationLib.h"

typedef UINT16 VAR_SIZE_TYPE;
#pragma pack(push)
#pragma pack(1)
typedef struct{
    UINT32 signature;
    VAR_SIZE_TYPE size;
    UINT32 next:24;
    UINT32 flags:8;
//  guid and name are there only if NVRAM_FLAG_DATA_ONLY is not set
//  UINT8 guid; 
//  CHAR8 or CHAR16 name[...];
//  UINT8 data[...];
//  if NVRAM_FLAG_EXT_HEDER is set
//  UINT8 extflags;
//  UINT8 extdata[...];
//  VAR_SIZE_TYPE extsize;
}NVAR;
#pragma pack(pop)

typedef struct _NVRAM_STORE_INTERFACE NVRAM_STORE_INTERFACE;
typedef struct {
    UINT8 *NvramAddress;
    UINTN NvramSize;
    EFI_GUID* NvramGuidsAddress;
    UINT8 *pEndOfVars, *pFirstVar;
    INT16 NextGuid;
    VAR_SIZE_TYPE LastVarSize;
    NVAR *pLastReturned;
    UINT8 Flags;
    UINT32 HeaderLength;
    NVRAM_STORE_INTERFACE *Interface;
} NVRAM_STORE_INFO;

VOID NvInitInfoBuffer(
    IN NVRAM_STORE_INFO *pInfo, UINTN HeaderSize, UINT8 Flags,
    NVRAM_STORE_INTERFACE *Interface
);
EFI_STATUS NvGetVariable(
    IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
    OUT UINT32 *Attributes OPTIONAL,
    IN OUT UINTN *DataSize, OUT VOID *Data,
    IN NVRAM_STORE_INFO *pInfo, OUT UINT8 *Flags
);

//---------------------------------------------------------------------------

extern EFI_BOOT_SERVICES    *gBS;

//---------------------------------------------------------------------------

EFI_HII_STRING_PROTOCOL *gHiiString;
#define NVRAM_STORE_FLAG_NON_VALATILE 1

#define DEFAULTS_GUID {0x4599d26f, 0x1a11, 0x49b8, 0xb9, 0x1f, 0x85, 0x87, 0x45, 0xcf, 0xf8, 0x24}
EFI_GUID DefaultsGuid = DEFAULTS_GUID;
CHAR16  gPlatformLang [20];

VOID UpdateNVDefautls(UINT32 index, NVRAM_STORE_INFO *NvInfo, NVRAM_VARIABLE * VarPtr);

/**
    function build the AMI defaults

    @param VOID

    @retval EFI_STATUS

**/
EFI_STATUS TseVarBuildAMIDefaults(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT32  page, control;
    VOID *defaults4FirstBootVar = NULL;
#if TSE_NVRAM_DEFAULTS_SUPPORT
    NVRAM_VARIABLE *OptvarPtr;
    NVRAM_VARIABLE *FailsafevarPtr;
    UINT32 index;

    NVRAM_STORE_INFO NvInfo={0};
    UINTN Nvsize;
    UINT8 * NvVarBuffer=NULL;
    UINT32 attribute=7;
#endif
    //Setting "RemoteConstructDefaults4FirstBoot" variable in NVRAM for first boot only
    if (TSE_SUPPORT_DEFAULT_FOR_STRING_CONTROL)
    {
        EFI_GUID ConstructDefaults4FirstBootGuid = CONSTRUCT_DEFAULTS_FIRST_BOOT_GUID;
        UINTN varSize = 0;
        
        defaults4FirstBootVar = (VOID *)VarGetNvramName (L"RemoteConstructDefaults4FirstBoot", &ConstructDefaults4FirstBootGuid, NULL, &varSize);
        if (NULL == defaults4FirstBootVar)
        {
            VarSetNvramName( L"RemoteConstructDefaults4FirstBoot", &ConstructDefaults4FirstBootGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, &varSize, sizeof (varSize) );
        }
    }
    
    gFailsafeDefaults = EfiLibAllocateZeroPool( sizeof(NVRAM_VARIABLE) * gVariables->VariableCount );
    if ( gFailsafeDefaults == NULL )
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto DONE;
    }
    gOptimalDefaults = EfiLibAllocateZeroPool( sizeof(NVRAM_VARIABLE) * gVariables->VariableCount );
    if (gOptimalDefaults == NULL )
    {
        Status = EFI_OUT_OF_RESOURCES;
        goto DONE;
    }
#if TSE_NVRAM_DEFAULTS_SUPPORT
    // Load the Current NVRAM variable then update the defaults.
    OptvarPtr = gOptimalDefaults;
    FailsafevarPtr = gFailsafeDefaults;
    for ( index = 0; index < gVariables->VariableCount; index++, OptvarPtr++, FailsafevarPtr++)
    {
//        if ((VARIABLE_ID_AMITSESETUP == index) && (TseDefaultSetupPasswordSupported ()))
        if (0)
        {
            OptvarPtr->Buffer = VarGetNvram( index, &OptvarPtr->Size );
            if (OptvarPtr->Buffer)
            {
//                MemSet(OptvarPtr->Buffer, ((TsePasswordLength * 2) * sizeof(CHAR16)), 0 );      //Preserving quietboot details
                //if ((!DefaultSetupPwdAtFirstBootOnly ()))
                {
                    //GetDefaultPassword (AMI_PASSWORD_NONE, &OptvarPtr->Buffer);
                }
            }
        }
        else
        {
            OptvarPtr->Buffer = VarGetNvram( index, &OptvarPtr->Size );
        }
        if(OptvarPtr->Size)
        {
            // init mfg defautls from Std Defaults 
            FailsafevarPtr->Buffer = EfiLibAllocateZeroPool (OptvarPtr->Size);
            if (NULL == FailsafevarPtr->Buffer)
                continue;
            if (NULL == OptvarPtr->Buffer)          //Add the fix for: If name value string is not able to retrieve then system hang
            {
                OptvarPtr->Buffer = EfiLibAllocateZeroPool (OptvarPtr->Size);
                if (NULL == OptvarPtr->Buffer)
                    continue;
            }
            MemCopy(FailsafevarPtr->Buffer, OptvarPtr->Buffer,OptvarPtr->Size);
        }
        else
            FailsafevarPtr->Buffer = NULL;
        FailsafevarPtr->Size = OptvarPtr->Size;
    }
#endif  //TSE_NVRAM_DEFAULTS_SUPPORT

    // this is the AMIBCP compatible user defaults
    for ( page = 0; page < gPages->PageCount; page++ )
    {
        PAGE_INFO *pageInfo = (PAGE_INFO *)((UINTN)gApplicationData + gPages->PageList[page]);

        if(pageInfo->PageHandle ==0)
            continue;
        /*
        if (NULL == gApp)
        {
            gtempCurrentPage = page;                    //Will be used in UefiGetQuestionOffset. Since gApp will not be filled but we need handle to find name value variable so
        }                                               //saving current page
        else
        {
            gApp->CurrentPage = page;
        }
            */
        for ( control= 0; control < pageInfo->PageControls.ControlCount; control++ )
        {
            CONTROL_INFO *controlInfo = (CONTROL_INFO *)((UINTN)gControlInfo + pageInfo->PageControls.ControlList[control]);

            if ( ( controlInfo->ControlVariable == VARIABLE_ID_LANGUAGE ) || ( controlInfo->ControlVariable == VARIABLE_ID_BOOT_ORDER ) || (controlInfo->ControlVariable == VARIABLE_ID_BBS_ORDER) )
                continue;

            //This will construct the defaults for controls having valid size and no conditional expression
            //This section will be overridden inside the GetSetDefaultsFromControlPtr() call
            if ( controlInfo->ControlDataWidth > 0  && 0 == controlInfo->ControlFlags.ControlEvaluateDefault)
            {
                VOID *ifrData = controlInfo->ControlPtr;
                UINT8 *failValue = NULL, *optValue = NULL;
                UINT32 ControlVarOffset = 0;

                if ( ifrData == NULL )
                    continue;

                failValue = (UINT8 *)controlInfo + sizeof(CONTROL_INFO);
                optValue = failValue + controlInfo->ControlDataWidth;

                ControlVarOffset = UefiGetQuestionOffset(ifrData);
                //Updating gOptimalDefaults and gFailSafeDefaults for stringType controls based on controlPtr
                if (TSE_SUPPORT_DEFAULT_FOR_STRING_CONTROL && controlInfo->ControlType == CONTROL_TYPE_POPUP_STRING)
                {
                    CHAR16 *failDefStr = NULL, *optimalDefStr = NULL;
                    
                    if (*(UINT16*)(failValue))
                    {
                        failDefStr = (VOID*)HiiGetString( controlInfo->ControlHandle, *(UINT16*)(failValue));
                        if (NULL == failDefStr)
                            continue;
                        
                        //Updating gFailsafeDefaults buffer.
                        _VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, controlInfo->ControlVariable, ControlVarOffset, EfiStrLen(failDefStr)*sizeof(CHAR16), (VOID*)failDefStr );
                    }
                    
                    if (*(UINT16*)(optValue))
                    {
                        optimalDefStr = (VOID*)HiiGetString( controlInfo->ControlHandle, *(UINT16*)(optValue));
                        if (NULL == optimalDefStr)
                            continue;
                        
                        //Updating gOptimalDefaults buffer.
                        _VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, ControlVarOffset, EfiStrLen(optimalDefStr)*sizeof(CHAR16), (VOID*)optimalDefStr );
                    }
                    
                    if (NULL == defaults4FirstBootVar && optimalDefStr) //Modify NVRAM for first time for string type controls based on defaults from controlPtr
                    {
                        VOID *optiBuffer = NULL, *failBuffer = NULL;
                        VARIABLE_INFO   *varInfo = (VARIABLE_INFO *)NULL;
                        UINTN size = 0;
                        EFI_STATUS status = EFI_SUCCESS;
                        
                        varInfo = VarGetVariableInfoIndex (controlInfo->ControlVariable);
                        
                        optiBuffer = VarGetNvram( controlInfo->ControlVariable, &size );                        
                        if (NULL == optiBuffer)
                        {
                            continue;
                        }

                        MemCopy ( ((UINT8*)optiBuffer)+ControlVarOffset, (UINT8*)optimalDefStr, EfiStrLen(optimalDefStr)*sizeof(CHAR16) );
                        
                        status = VarSetNvramName( varInfo->VariableName, &varInfo->VariableGuid, varInfo->VariableAttributes, optiBuffer, size );
                        if (optiBuffer)
                            MemFreePointer( (VOID **)&optiBuffer );
                    }
                    if (failDefStr)
                        MemFreePointer( (VOID **)&failDefStr );
                    
                    if (optimalDefStr)
                        MemFreePointer( (VOID **)&optimalDefStr );
                    
                }
                //EIP-93340 End
                else
                {               
                    _VarGetSetValue( VAR_COMMAND_SET_VALUE, gFailsafeDefaults, controlInfo->ControlVariable, ControlVarOffset, controlInfo->ControlDataWidth, failValue );
                    _VarGetSetValue( VAR_COMMAND_SET_VALUE, gOptimalDefaults, controlInfo->ControlVariable, ControlVarOffset, controlInfo->ControlDataWidth, optValue );
                }
            }
        }
    }

#if TSE_NVRAM_DEFAULTS_SUPPORT  //EIP-47260: To build TSE without NVRAM module support.
    // StdDefaults
    Nvsize = 0;
    NvVarBuffer = VarGetNvramName( L"StdDefaults", &DefaultsGuid, &attribute, &Nvsize );
    if(NvVarBuffer && Nvsize)
    {
        NvInfo.NvramAddress = NvVarBuffer;
        NvInfo.NvramSize = Nvsize;
        NvInitInfoBuffer(&NvInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );

        // Update the defaults.
        OptvarPtr = gOptimalDefaults;
        for ( index = 0; index < gVariables->VariableCount; index++, OptvarPtr++)
        {
            UpdateNVDefautls(index, &NvInfo, OptvarPtr);
        }
        MemFreePointer( (VOID **)&NvVarBuffer );
    }

    // MfgDefaults
    Nvsize = 0;
    NvVarBuffer = VarGetNvramName( L"MfgDefaults", &DefaultsGuid, &attribute, &Nvsize );
    if(NvVarBuffer && Nvsize)
    {
        NvInfo.NvramAddress = NvVarBuffer;
        NvInfo.NvramSize = Nvsize;
        NvInitInfoBuffer(&NvInfo, 0, NVRAM_STORE_FLAG_NON_VALATILE, NULL );

        FailsafevarPtr = gFailsafeDefaults;
        for ( index = 0; index < gVariables->VariableCount; index++, FailsafevarPtr++ )
        {
            UpdateNVDefautls(index, &NvInfo, FailsafevarPtr);
        }
        MemFreePointer( (VOID **)&NvVarBuffer );
    }
    //Updating optimalDefaults and gFailsafeDefaults for STRING_TYPE controls 
    //based on defalut stringID present in controlPtr
        GetSetDefaultsFromControlPtr(gOptimalDefaults, gFailsafeDefaults);
#endif  //TSE_NVRAM_DEFAULTS_SUPPORT
DONE:
    return Status;
}

/**

    @param 

    @retval EFI_STATUS
        EFI_SUCCESS
**/
EFI_STATUS Uefi21HiiInitializeProtocol(VOID)
{
    EFI_STATUS Status = EFI_SUCCESS;

    /*
    Status = gBS->LocateProtocol(&gEfiHiiFontProtocolGuid, NULL, (VOID**)&gHiiFont);
    if(EFI_ERROR(Status))
    {
        goto DONE;
    }
    */
    Status = gBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, (VOID**)&gHiiDatabase);
    if(EFI_ERROR(Status))
    {
        goto DONE;
    }
    Status = gBS->LocateProtocol(&gEfiHiiStringProtocolGuid, NULL,(VOID**) &gHiiString);
    if(EFI_ERROR(Status))
    {
        goto DONE;
    }
    Status = HiiExtendedInitializeProtocol();

DONE:
    return Status;
}

/**
        Procedure   :   FormBrowserLocateSetupHandles

        Description :   return Handles and count that is passed to SendForm interface.

        Input       :   OUT handleBuffer and count

        Output      :   Status

**/
EFI_STATUS FormBrowserLocateSetupHandles(VOID*  **handleBuffer,UINT16 *count)
{
  EFI_STATUS status = EFI_UNSUPPORTED;

#if SETUP_FORM_BROWSER_SUPPORT
  /*
  if(gSfHandles)
  {
    *handleBuffer = (VOID**)gSfHandles;
    *count = (UINT16)gSfHandleCount;
  }
  else
  {
    status =  EFI_NOT_FOUND;
  }
  */
#else
  status = EFI_UNSUPPORTED;
#endif
  return status;
}

/**
        Procedure   :   FormBrowserHandleValid

        Description :   return True Browser is showing forms from SendForm interface.

        Input       :   none

        Output      :   BOOLEAN

**/
BOOLEAN FormBrowserHandleValid(VOID)
{
#if SETUP_FORM_BROWSER_SUPPORT
    if(0)
        return TRUE;
#endif
    return FALSE;
}

EFI_STATUS UefiHiiWrapperGetString( EFI_HII_HANDLE handle, CHAR8 *Language, EFI_STRING_ID StringId, CHAR16 *buffer,UINTN *bufferSize )
{
    if ( handle == INVALID_HANDLE )
        handle = gHiiHandle;

    return gHiiString->GetString(gHiiString, Language, handle, StringId, buffer, bufferSize, NULL);
}

/**
    Returns the string for incoming token.
    If lang is valid then return string for this lang
    else return string for PlatformLang variable
    else return english string - default

    @param Handle, string token and lang code

    @retval Sting buffer

**/
CHAR16 *Uefi21HiiGetStringLanguage(EFI_HII_HANDLE handle, UINT16 token, CHAR16 *lang)
{
    EFI_STATUS status;
    CHAR16 *buffer = NULL;
    UINTN bufferSize = 0,Size=0;
//  UINT8 *language = NULL;
 CHAR8 *language = NULL;

    status = Uefi21HiiInitializeProtocol();
    if ( EFI_ERROR(status) )
    {
        return buffer;
    }

        /* This is a Quick Fix. We need to get the Default Languag here. */
    if(lang)
        language = StrDup16to8(lang);
    else
    {
        
        //language = VarGetNvramName( L"PlatformLang", &gEfiGlobalVariableGuid, NULL, &Size );
        if (*gPlatformLang)     //gPlatformLang will filled in mainsetuploop only if any call comes before it get from NVRAM
        {
            language = EfiLibAllocateZeroPool (20 * sizeof (CHAR16));
            if (NULL != language)
            {
                EfiStrCpy ((CHAR16 *)language, gPlatformLang);
            }
        }
        else
        {
            language = VarGetNvramName( L"PlatformLang", &gEfiGlobalVariableGuid, NULL, &Size );
            if (NULL != language)
            {
            EfiZeroMem (gPlatformLang, sizeof (gPlatformLang));
            EfiCopyMem (gPlatformLang, language, Size);
            }
        }
                
    }
    if(NULL == language)
      return NULL;
    if ( handle == INVALID_HANDLE )
        handle = gHiiHandle;      
    status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);  

  if(status == EFI_INVALID_PARAMETER)
  {
    buffer = (CHAR16*)EfiLibAllocateZeroPool(sizeof(CHAR16));
    if(buffer == NULL)
    {
      return buffer;
    }
    status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);
  }
    // String not found with currnet Lang
    if (( EFI_ERROR(status) ) && (bufferSize==0))
    {
        //81617 : Avoiding memory leaks in TSE
        MemFreePointer( (VOID **)&language );
        language = StrDup8(CONVERT_TO_STRING(DEFAULT_LANGUAGE_CODE));
        status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
// String not found in Default Lang
        if (( EFI_ERROR(status) )&&(bufferSize==0))
        {
            if(EfiAsciiStrCmp(language, "en-US")!=0)
            {
                MemFreePointer( (VOID **)&language );
                language = StrDup8("en-US");
                status = UefiHiiWrapperGetString(handle, language,token ,buffer, &bufferSize );
// string not found in english
                if (( EFI_ERROR(status) )&&(bufferSize==0))
                {
//81617 : Avoiding memory leaks in TSE
                    MemFreePointer( (VOID **)&language );
                    return buffer;
                }
            }
            else
            {
//81617 : Avoiding memory leaks in TSE
                MemFreePointer( (VOID **)&language );
                return buffer;
            }
        }
        if (EFI_BUFFER_TOO_SMALL == status)
        {
            buffer = (CHAR16*)EfiLibAllocateZeroPool(bufferSize);
            if(buffer == NULL)
            {
              return buffer;
            }
            status = UefiHiiWrapperGetString (handle, language, token, buffer, &bufferSize );
            if (!EFI_ERROR (status))
            {
                MemFreePointer( (VOID **)&language );
                return buffer;
            }
        }
    }
    buffer = (CHAR16*)EfiLibAllocateZeroPool( bufferSize );
    if ( buffer == NULL )
        return buffer;
    
    status = HiiLibGetStringEx(handle,token,language,&bufferSize,buffer);


    if ( EFI_ERROR(status) )
        MemFreePointer( (VOID **)&buffer );
    MemFreePointer( (VOID **)&language );

    return buffer;
}

CHAR16 *HiiGetString( VOID* handle, UINT16 token )
{
    if(INVALID_TOKEN != token)
        return Uefi21HiiGetStringLanguage ( handle, token, NULL );
    else
        return NULL;
}

/**
    reads the NV Varaiable from Nram buffer using NVLib.

    @param variable NVRAM_STORE_INFO *pInfo , UINTN *size

    @retval VOID

**/
VOID *TSEGetNVVariable( UINT32 variable, NVRAM_STORE_INFO *pInfo , UINTN *size )
{
    VOID *buffer = NULL;
#if TSE_NVRAM_DEFAULTS_SUPPORT
    EFI_STATUS Status = EFI_UNSUPPORTED;
    VARIABLE_INFO *varInfo;
 //UINT8 Flags;  To fix the CPU hangs when Core is update  to 4.6.4.1 

    varInfo = VarGetVariableInfoIndex( variable );
    if ( varInfo == NULL )
        return buffer;

    *size=0;
    Status = NvGetVariable(
        varInfo->VariableName,
        &varInfo->VariableGuid,
        NULL,
        size,
        buffer,
        pInfo, /*&Flags*/ NULL          //To fix the CPU hangs when Core is update  to 4.6.4.1 
         
        );

    if ( Status != EFI_BUFFER_TOO_SMALL )
        return buffer;

    buffer = EfiLibAllocateZeroPool( *size );
    if ( buffer == NULL )
        return buffer;

    Status = NvGetVariable(
        varInfo->VariableName,
        &varInfo->VariableGuid,
        NULL,
        size,
        buffer,
        pInfo,/*&Flags*/ NULL           //To fix the CPU hangs when Core is update  to 4.6.4.1 
        );
        
    if ( EFI_ERROR( Status ) )
        MemFreePointer( (VOID *)&buffer );

#endif //TSE_NVRAM_DEFAULTS_SUPPORT
    return buffer;
}

/**
    Updated the Variable buffer with NVRam Defaults buffer.

    @param index NVRAM_STORE_INFO *NvInfo, NVRAM_VARIABLE * VarPtr

    @retval VOID

**/
VOID UpdateNVDefautls(UINT32 index, NVRAM_STORE_INFO *NvInfo, NVRAM_VARIABLE * VarPtr)
{
    UINTN Nvsize;
    UINT8 * NvVarBuffer;

    Nvsize = 0;
    NvVarBuffer = TSEGetNVVariable( index, NvInfo , &Nvsize );
    if(NvVarBuffer && Nvsize)
    {
        if(index == VARIABLE_ID_AMITSESETUP)
        {
            if(TSE_LOAD_PASSWORD_ON_DEFAULTS)
            {
                // To preserve password
                if(Nvsize>=sizeof(AMITSESETUP))
                {
                    if(VarPtr->Buffer && (VarPtr->Size>=sizeof(AMITSESETUP)))
                    {
                        MemCopy(((AMITSESETUP*)NvVarBuffer)->UserPassword, ((AMITSESETUP*)VarPtr->Buffer)->UserPassword,SETUP_PASSWORD_LENGTH*sizeof(CHAR16));
                        MemCopy(((AMITSESETUP*)NvVarBuffer)->AdminPassword, ((AMITSESETUP*)VarPtr->Buffer)->AdminPassword,SETUP_PASSWORD_LENGTH*sizeof(CHAR16));
                    }
                }
            }
        }

        if(VarPtr->Size >= Nvsize)
        {
            MemCopy(VarPtr->Buffer,NvVarBuffer,Nvsize);
            MemFreePointer( (VOID **)&NvVarBuffer );
        }
        else
        {
            MemFreePointer( (VOID **)&VarPtr->Buffer);
            VarPtr->Buffer = NvVarBuffer;
            VarPtr->Size = Nvsize;
        }
    }

}

/**
    function to handle the operation of getting the Ifr Value.

    @param controlinfo 
    @param guidInfo 

    @retval UINT16

**/
UINT16 _SpecialGetValue( CONTROL_INFO *controlinfo, GUID_INFO **guidInfo )
{
    return UefiGetSpecialEqIDValue(controlinfo, guidInfo);
}

/**

    @param 

    @retval 

**/
CHAR8 *_GetSupportedLanguages (EFI_HII_HANDLE HiiHandle)
{
  EFI_STATUS  status;
  UINTN       BufferSize;
  CHAR8       *LanguageString;

  status = Uefi21HiiInitializeProtocol ();

  //
  // Collect current supported Languages for given HII handle
  //
  BufferSize = 0x1000;
  LanguageString = (CHAR8*)EfiLibAllocateZeroPool (BufferSize);
  status = gHiiString->GetLanguages (gHiiString, HiiHandle, LanguageString, &BufferSize);
  if(status == EFI_BUFFER_TOO_SMALL)
  {
    MemFreePointer((VOID**)&LanguageString);
    LanguageString = (CHAR8*)EfiLibAllocateZeroPool (BufferSize);
    status = gHiiString->GetLanguages (gHiiString, HiiHandle, LanguageString, &BufferSize);
  }

  if(EFI_ERROR(status))
  {
    LanguageString = NULL;
  }

  return LanguageString;
}


CHAR8 *
EFIAPI
GetBestLanguage (
  IN CONST CHAR8  *SupportedLanguages, 
  IN BOOLEAN      Iso639Language,
  ...
  );

UINT16 Uefi21HiiChangeStringLanguage(VOID* handle, UINT16 token, CHAR16 *lang, CHAR16 *string)
{
    EFI_STATUS      status = EFI_SUCCESS;
    UINT16          newToken = INVALID_TOKEN;
    CHAR8           *passedLang = (CHAR8 *)NULL;
    CHAR8           *Languages = (CHAR8 *)NULL;
    CHAR8           *LangStrings = (CHAR8 *)NULL;
    //CHAR8         Lang[RFC_3066_ENTRY_SIZE];
    CHAR8           *Lang = (CHAR8 *)NULL;

    status = Uefi21HiiInitializeProtocol();

    if((EFI_ERROR(status)) || (string == NULL)) 
    {
        return (UINT16)INVALID_TOKEN;
    }

    passedLang = StrDup16to8(lang);

    Languages = _GetSupportedLanguages(handle);
    if(Languages == NULL)
        return INVALID_TOKEN;

    //
    //Iterate through the languages supported by package list to see whether passed language is supported. If yes, add/change the string. 
    //
    LangStrings = Languages;
    if (*LangStrings != 0)
    {
        Lang = GetBestLanguage(LangStrings, FALSE, passedLang, NULL );
//      _GetNextLanguage (&LangStrings, Lang);
        if(token)
        {
            status = gHiiString->SetString(gHiiString, handle, token, Lang, string, NULL);
            newToken = token;
        }
        else
        {
            status = gHiiString->NewString(gHiiString, handle, &newToken, Lang, NULL, string, NULL);
        }

        if(EFI_ERROR(status))
        {
            newToken = INVALID_TOKEN;
        }
    }
    else
        newToken = INVALID_TOKEN;

    MemFreePointer ((VOID **)&passedLang);
    MemFreePointer ((VOID **)&Languages);
    MemFreePointer ((VOID **)&Lang);
    return newToken;
}


/**
    function to add a string

    @param handle CHAR16 *string

    @retval token

**/
UINT16 HiiAddString( VOID* handle, CHAR16 *string )
{
    UINT16 token = 0;

    if(SETUP_ITK_COMPATIBILITY)
    {
        token = Uefi21HiiChangeStringLanguage( handle, 0, gLanguages[0].Unicode, string );
    }
    else
    {
        UINT16 FirstToken;
        UINTN i, j;

        FirstToken = token = Uefi21HiiChangeStringLanguage( handle, 0, gLanguages[0].Unicode, string );
        if ( token == INVALID_TOKEN )
            return token;
        for ( j = 0, i = 1; i < gLangCount; i++ )
        {
            if ( i != j )
                token = Uefi21HiiChangeStringLanguage( handle, token, gLanguages[i].Unicode, string );
            if ( token == INVALID_TOKEN )
            {
                token = Uefi21HiiChangeStringLanguage( handle, 0, gLanguages[i].Unicode, string );
                if ( token == INVALID_TOKEN )
                    return FirstToken; //token;
                j = i;
                i = 0;
            }
        }
    }
    return token;
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
