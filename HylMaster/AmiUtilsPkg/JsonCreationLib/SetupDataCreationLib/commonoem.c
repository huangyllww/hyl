//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Archive: /Alaska/SOURCE/Modules/AMITSE2_0/AMITSE/Uefi2.1/CtrlCond.c $
//
// $Author: Premkumara $
//
// $Revision: 13 $
//
// $Date: 2/02/12 2:59a $
//
//*****************************************************************//
/** @file CtrlCond.c

**/
//*************************************************************************

//---------------------------------------------------------------------------
#include "SetupDataCreationLib.h"
#include "CtrlCond.h"
#include "TseUefiHii.h"
#include "TseElinks.h"
#include "AMITSEElinks.h"
#include <AutoId.h> // for MAIN_MAIN

#define NULL_GUID \
    { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }

typedef struct _HII_FORM_ADDRESS
{
	EFI_GUID formsetGuid; // Required
	UINT16		formId;	// Required
	VOID *		Handle;	// Optional
}HII_FORM_ADDRESS;

typedef struct
{
    EFI_GUID            VariableGuid;
    CHAR8               VariableName [50];
}VAR_DYNAMICPARSING_HANDLESUPPRESS;
	
HII_FORM_ADDRESS RootPageList[] = {	AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST { NULL_GUID, 0, NULL}, };
PASSWORD_ENOCDE_LIST_TEMPLATE gPasswordEncodeList [] = {PASSWORD_ENOCDE_LIST {{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 0, 0}};  //Last structure for avoiding build error.
UINT32 gEncodeListCount = sizeof (gPasswordEncodeList)/ sizeof (PASSWORD_ENOCDE_LIST_TEMPLATE);

VAR_DYNAMICPARSING_HANDLESUPPRESS gHandleSuppressVarList [] = {AMITSE_DYNAMICPARSING_HANDLE_SUPPRESS_LIST {NULL_GUID, ""}};

extern PAGE_LIST     *gPages;
extern UINT8         *gApplicationData;
extern PAGE_ID_LIST  *gPageIdList;

//---------------------------------------------------------------------------

/**
    This function test for SubPage as root pages from
    AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST

    @param info 

    @retval BOOLEAN

**/
BOOLEAN isSubPageRootPage(PAGE_INFO *info)
{
    UINTN RootPageListCount = sizeof(RootPageList) / sizeof (HII_FORM_ADDRESS);
    UINTN j;
    PAGE_ID_INFO *pageIdInfo;
    EFI_GUID NullGuid = NULL_GUID;
    // Support to Handle some child pages as root pages.
    for(j=0; j<RootPageListCount;j++)
    {
        if( info->PageFormID == RootPageList[j].formId)
        {
            // If Handle is not null compare. If it null Ignore
            if(RootPageList[j].Handle != NULL)
            {
                if(info->PageHandle != RootPageList[j].Handle)
                    continue;
            }
            
            pageIdInfo = (PAGE_ID_INFO *)(((UINT8 *) gPageIdList) + gPageIdList->PageIdList[info->PageIdIndex]);
                        
            if( EfiCompareGuid(&pageIdInfo->PageGuid,&RootPageList[j].formsetGuid) )//Compare Guid
            {
               return TRUE ;
            }
        }
    }
    return FALSE ;
}

/**
    This function returns the number of SubPage as root pages from
    AMITSE_SUBPAGE_AS_ROOT_PAGE_LIST.

    @param VOID

    @retval UINTN Number of SubPages as root pages

**/
UINTN GetSubPageAsRootPageCount(VOID)
{
    UINTN RootPageListCount = 0 ;  

    RootPageListCount = sizeof(RootPageList) / sizeof (HII_FORM_ADDRESS);
    
    return RootPageListCount ;
}

/**
    This function informs to match the handle or not for creating new variable using VAR_DYNAMICPARSING_HANDLESUPPRESS_LIST ELink

    @param EFI_GUID *, CHAR16   *

    @retval BOOLEAN. Return TRUE handle matches with elink else FALSE

**/
CHAR8* StrDup16to8(CHAR16 *String);
INTN EfiStrCmp (IN CHAR16   *String, IN CHAR16   *String2);
BOOLEAN VariableHandleSuppressed (EFI_GUID *VariableGuid, CHAR16    *VariableName)
{
#if SUPPRESS_HANDLE_FOR_VAR_CREATION    
    UINTN HandleSuppressListCount = sizeof (gHandleSuppressVarList)/sizeof (VAR_DYNAMICPARSING_HANDLESUPPRESS);
    UINTN i = 0;
    CHAR8 *VariableNameStr8 = (CHAR8 *)NULL;
    
    VariableNameStr8 = StrDup16to8 (VariableName);
    if (NULL == VariableNameStr8)
    {
        return FALSE;
    }
    for(i=0; i < HandleSuppressListCount; i++)
    {
        if ( (EfiCompareGuid(VariableGuid, &(gHandleSuppressVarList [i].VariableGuid))) && (Strcmp (VariableNameStr8, gHandleSuppressVarList [i].VariableName) ==0) )
        {
            return TRUE;
        }
    }
#endif  
    return FALSE;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**     5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093     **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
