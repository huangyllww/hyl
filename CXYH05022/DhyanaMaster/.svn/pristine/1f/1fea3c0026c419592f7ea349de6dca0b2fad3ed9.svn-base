//************************************************************************
//************************************************************************
//**                                                                  					    	**
//**        (C)Copyright 2014, American Megatrends, Inc.            	**
//**                                                                  					    	**
//**                       All Rights Reserved.                       				**
//**                                                                  						**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  **
//**                                                                  						**
//**                       Phone: (770)-246-8600                      				**
//**                                                                  						**
//************************************************************************
//************************************************************************

//************************************************************************
// $Archive: $
//
// $Author: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************//
// Revision History
// ----------------
// $Log: $
//
//************************************************************************
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  AMIRescue.cpp
//
// Description:	Contains AmiResce Entry point
//
//<AMI_FHDR_END>
//**********************************************************************

//************************************************************************
//					Include Section
//************************************************************************
//#include "Types.h"
#include "EsaDrvMgr.h"

extern "C"
{
extern EFI_RUNTIME_SERVICES  	*gRT;
VOID *    EFIAPI    AllocateZeroPool (        IN UINTN  AllocationSize    );
CHAR8 *gLang = (CHAR8*)NULL;
void UpdateMultilanguageVar();
extern EFI_GUID gEfiGlobalVariableGuid;
}

void UpdateMultilanguageVar()
{
	VOID *DefBuffer = (VOID*)NULL;
	UINTN DefBuffSize = 0;
	UINT32 Attributes = 0;
	EFI_STATUS Status = EFI_SUCCESS;
	
	Status = gRT->GetVariable ( (CHAR16 *)L"PlatformLang", (EFI_GUID*)(&gEfiGlobalVariableGuid),(UINT32*)&Attributes,(UINTN*)&DefBuffSize,(VOID*)DefBuffer);   
	if(EFI_BUFFER_TOO_SMALL == Status){
		DefBuffer = (VOID*)AllocateZeroPool(DefBuffSize);
		Status = gRT->GetVariable ( (CHAR16 *)L"PlatformLang", (EFI_GUID*)(&gEfiGlobalVariableGuid),(UINT32*)&Attributes,(UINTN*)&DefBuffSize,(VOID*)DefBuffer);   
	}
	if (NULL == DefBuffer)
	{
		gLang = "en";
	}
	else
		gLang	= (CHAR8 *)DefBuffer;
}

//************************************************************************
//************************************************************************
//**                                                    		    	**
//**        (C)Copyright 2014, American Megatrends, Inc.            	**
//**                                                    		    	**
//**                       All Rights Reserved.            				**
//**                                                    				**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                    				**
//**                       Phone: (770)-246-8600           				**
//**                                                    				**
//************************************************************************
//************************************************************************
