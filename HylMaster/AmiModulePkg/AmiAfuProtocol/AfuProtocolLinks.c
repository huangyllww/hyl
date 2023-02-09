//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file 
AmiAfuProtoclLink.c

This is Elinks file.

*/

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include <Token.h>
#include <AmiLib.h>

#include <Setup.h>

#include <Protocol/AmiAfuProtocol.h>

//---------------------------------------------------------------------------

//----------------------------------------------------------------------
#pragma pack(1)
typedef struct {
    CHAR16      *BootVarName;
    EFI_GUID    Guid;
    UINT32      Attrib;
    UINTN       Size;
    UINT8       *Data;
    VOID        *NextData;
} SAVED_VAR;
#pragma pack()

extern EFI_GUID gAmiSmbiosNvramGuid;
//---------------------------------------------------------------------------
// Module defined global variables
//---------------------------------------------------------------------------
SAVED_VAR           *gRestoreVarList = NULL;

//----------------------------------------------------------------------
// MACROs
//---------------------------------------------------------------------------
#define DEFAULT_VARIABLE_NAME_SIZE     100
#define AMITSESETUP_AFU_GUID \
{ 0xc811fa38, 0x42c8, 0x4579, 0xa9, 0xbb, 0x60, 0xe9, 0x4e, 0xdd, 0xfb, 0x34 }

/**
 	PreserveVariables()

	This function Preserve variables.	
	
	@param Flag -  The ability to retain data.

	@retval NONE
*/ 
VOID PreserveVariables( UINT32 Flag )
{
    UINTN               VarNameSize = 2, VariableSize, ExtAuthHdrSize = 0, Result;
    UINTN               MaxVarNameSize;
    CHAR16              *VarName = NULL, *OldVarName = NULL;
    UINT8               *VariableData, *pData,  IsEfiBootVars = 0;
    UINT32              VariableAttr;
    EFI_GUID            VarGuid;
    EFI_STATUS          Status;
    SAVED_VAR           *pLastVar = NULL;

    EFI_GUID            TSESetupGuid = AMITSESETUP_AFU_GUID;
//    EFI_GUID            SetupGuid = SETUP_GUID;
    
TRACE((-1, "[EDBG] PreserveVariables \n" ));
    // Locate a big enough buffer for following process.
    VarNameSize = MaxVarNameSize = DEFAULT_VARIABLE_NAME_SIZE;
    Status = pBS->AllocatePool (
		    EfiRuntimeServicesData,
		    VarNameSize, (VOID **)&VarName
    	    	   );
    if(EFI_ERROR(Status)) return;

    // Get frist variable
    VarName[0] = 0;
    do{
        VarNameSize = MaxVarNameSize;
        Status = pRS->GetNextVariableName (&VarNameSize, VarName, &VarGuid);

        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            // Default Buffer is not enough, allocate new enough buffer.
            OldVarName = VarName;
            Status = pBS->AllocatePool (
                          EfiRuntimeServicesData,
                          VarNameSize, (VOID **)&VarName);

            // Update last Variable Name to New Buffer for continuing search.    
            MemCpy(VarName, OldVarName, MaxVarNameSize);
                
            // Free old working buffer and update Max Buffer size.    
            pBS->FreePool(OldVarName);
            MaxVarNameSize = VarNameSize;
            continue;
        }
        if(!EFI_ERROR(Status))
        {

	if(Flag & PRESERVE_SMBIOS && Result != 0)
		Result = guidcmp(&gAmiSmbiosNvramGuid, &VarGuid);	
	
	if(Flag & PRESERVE_SETUP && Result != 0)
		Result = guidcmp(&TSESetupGuid, &VarGuid);	
	
	if (Result != 0) 
		continue;
           
            VariableSize = 0;
            Status = pRS->GetVariable(VarName, &VarGuid,
                                      &VariableAttr, &VariableSize, NULL);

            if(Status != EFI_BUFFER_TOO_SMALL) continue;

            Status = pBS->AllocatePool (
                          EfiRuntimeServicesData,
                          VariableSize + ExtAuthHdrSize, (VOID **)&VariableData);

            if(EFI_ERROR(Status)) continue;

            Status = pRS->GetVariable(VarName, &VarGuid, \
                            &VariableAttr, &VariableSize, \
                            VariableData + ExtAuthHdrSize);

            if(EFI_ERROR(Status))
            {
                pBS->FreePool(VariableData);
                continue;
            }

            // Store variable in Link list
            Status = pBS->AllocatePool (
                          EfiRuntimeServicesData,
                          sizeof(SAVED_VAR), (VOID **)&pData);

            if(EFI_ERROR(Status))
                continue;

            Status = pBS->AllocatePool (
                          EfiRuntimeServicesData,
                          VarNameSize, (VOID **)&OldVarName);

            if(EFI_ERROR(Status))
                continue;

            MemCpy(OldVarName, VarName, VarNameSize);

            MemCpy(&((SAVED_VAR*)pData)->Guid, &VarGuid, sizeof(EFI_GUID));

            ((SAVED_VAR*)pData)->BootVarName = OldVarName;
            ((SAVED_VAR*)pData)->Attrib = VariableAttr;
            ((SAVED_VAR*)pData)->Size = VariableSize + ExtAuthHdrSize;
            ((SAVED_VAR*)pData)->Data = VariableData;
            ((SAVED_VAR*)pData)->NextData = NULL;

                // Init Variable List
                if (gRestoreVarList == NULL) {
                    gRestoreVarList = (SAVED_VAR*)pData;
                    pLastVar = NULL;
                }    
                if (pLastVar != NULL) pLastVar->NextData = (VOID*)pData;
                pLastVar = (SAVED_VAR*)pData;

                // Reset ExtAuthHdrSize for next search..
            ExtAuthHdrSize = 0; 
        }
        
        Result = -1;
        //If any other error, break for avoiding dead loop
        if(EFI_ERROR(Status)&&(Status!=EFI_BUFFER_TOO_SMALL))
            break;

    }while(Status != EFI_NOT_FOUND);
    // Disabled Searching Boot Time Variables
//    if (NvramControl) NvramControl->ShowBootTimeVariables(FALSE);
    pBS->FreePool(VarName);
}

/**
 	RestoreVariables()

	This function is Restore variables.	
	
	@param Flag -  The ability to retain data.

	@retval NONE
*/ 
VOID RestoreVariables (UINT32 Flag)
{
    SAVED_VAR           *TempPoint;
    UINTN               VariableSize = 0, Index = 0;
    UINT8               *VariableData = NULL;
    EFI_STATUS          Status;

    TRACE((-1, "[EDBG] RestoreVariables \n" ));
    
    while (gRestoreVarList != NULL) {
    
	    if (gRestoreVarList != NULL) {
            TempPoint = gRestoreVarList;
            gRestoreVarList = TempPoint->NextData;
        } else return;          // This condition should not happen.

        Status = pRS->SetVariable (  TempPoint->BootVarName,
                                         &TempPoint->Guid,
                                         TempPoint->Attrib,
                                         TempPoint->Size,
                                         TempPoint->Data );

        pBS->FreePool(TempPoint->BootVarName);
        pBS->FreePool(TempPoint->Data);
        pBS->FreePool(TempPoint);
    }    
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
