/** @file AliTSEHook.c
    Function file for Tse hook functionalities.

**/

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include "AmiLib.h"
#include "AmiDxeLib.h"
#include "TseCommon.h"
#include <Setup.h>
#include "Timer.h"
#include "commonoem.h"
#include "Include/Protocol/AMIPostMgr.h"
#include "Include/Protocol/EsaTseInterfaces.h"
#include <TseSetupTransfer.h>
#include "Token.h"
#include "AMIVfr.h"

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------
extern EFI_RUNTIME_SERVICES     *gRT;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_SYSTEM_TABLE         *gST;
Tse_Setup_transfer              *tTseSetuptransferInterface = NULL; 
static UINT8                    gOemHideValue=0;
static UINT8                    gAdvanceModeInit=0;
extern BOOLEAN gEnterSetup;

VOID InitAliAdvanceModeKey()
{
   EFI_STATUS                           Status;
   OEM_NO_DEFAULT_VARIABLES             OemSetupData;
   UINTN                                OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
   EFI_GUID                             mAmiTseOemPortingVar1Guid =  OEM_PORT_VARIABEL1;

    Status = pRS->GetVariable(L"OemPortVar1", &mAmiTseOemPortingVar1Guid, NULL,
                                &OemSetupDataSize, &OemSetupData);
    TRACE ((-1, "\nInitAliAdvanceModeKey Status = %d.", Status));
    #if SETUP_DEBUG_MODE_SUPPORT
        OemSetupData.OemHide=0;
    #else
        OemSetupData.OemHide=0x11;
    #endif    
        gRT->SetVariable( L"OemPortVar1", \
                          &mAmiTseOemPortingVar1Guid, \
                          EFI_VARIABLE_BOOTSERVICE_ACCESS, \
                          OemSetupDataSize, \
                          &OemSetupData);

    return;
}

VOID SetAliAdvanceModeKey()
{
   EFI_STATUS                           Status;
   OEM_NO_DEFAULT_VARIABLES             OemSetupData;
   UINTN                                OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
   EFI_GUID                             mAmiTseOemPortingVar1Guid =  OEM_PORT_VARIABEL1;

    Status = pRS->GetVariable(L"OemPortVar1", &mAmiTseOemPortingVar1Guid, NULL,
                                &OemSetupDataSize, &OemSetupData);
    TRACE ((-1, "\nSetAliAdvanceModeKey Status = %d.", Status));
    
    OemSetupData.OemHide=0x11;
    gOemHideValue = 0x11;
    gAdvanceModeInit = 1;
   
    gRT->SetVariable( L"OemPortVar1", \
                          &mAmiTseOemPortingVar1Guid, \
                          EFI_VARIABLE_BOOTSERVICE_ACCESS, \
                          OemSetupDataSize, \
                          &OemSetupData);
    gEnterSetup = TRUE;
    return;
}

VOID LoadedAliAdvanceModeDefaults()
{
   EFI_STATUS                  Status;
   EFI_GUID                    gTseTransferGuid = TSE_TRANSFER_GUID;  
   OEM_NO_DEFAULT_VARIABLES    *OemSetupDataPtr;//,OemSetupData;
   UINTN                       OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
   EFI_GUID                    mAmiTseOemPortingVar1Guid =  OEM_PORT_VARIABEL1;
   SETUP_DATA                  *SetupData;
   UINTN                       SetupDataSize = sizeof(SETUP_DATA);
   EFI_GUID                    gEfiSetupVariableGuid = SETUP_GUID;
   
   if(gAdvanceModeInit == 0) return;
   
   if(tTseSetuptransferInterface == NULL){
       Status = pBS->LocateProtocol(&gTseTransferGuid, NULL, &tTseSetuptransferInterface);
       if(EFI_ERROR(Status)) return;
   }
   
    Status = tTseSetuptransferInterface->VarGetBuffer(L"OemPortVar1",mAmiTseOemPortingVar1Guid,(UINT8**)&OemSetupDataPtr,&OemSetupDataSize);
    if(!EFI_ERROR(Status)) {
            OemSetupDataPtr->OemHide = gOemHideValue;
    }
    
    Status = tTseSetuptransferInterface->VarGetBuffer( L"Setup",gEfiSetupVariableGuid,(UINT8**)&SetupData,&SetupDataSize);
    if(!EFI_ERROR(Status)) {
        SetupData->LocalHddBoot = (PcdGet8(PcdAliMocExist) == 1)?0:1;;
    }

    return;
}
