/** @file OemTSEHook.c
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
#include "OemTSEHook.h"

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------
extern EFI_RUNTIME_SERVICES     *gRT;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_SYSTEM_TABLE         *gST;
Tse_Setup_transfer              *pTseSetuptransferInterface = NULL; 
static UINT8                    gOemHideValue=0;
static UINT8                    gAdvanceModeInit=0;

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------
extern EFI_STATUS EfiLibNamedEventSignal (IN EFI_GUID  *Name );      //EIP125219


#if defined(SETUP_ADVANCE_MODE_HOT_KEY_SUPPORT)&& (SETUP_ADVANCE_MODE_HOT_KEY_SUPPORT == 1) //<lvych00120160314+>
VOID HandleOemAdvanceModeKey(VOID *app, VOID *hotkey, VOID *cookie )
{
   EFI_STATUS  Status;
   EFI_GUID gTseTransferGuid = TSE_TRANSFER_GUID;  
   OEM_NO_DEFAULT_VARIABLES             *OemSetupDataPtr,OemSetupData;//,OemSetupData;
   UINTN                                 OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
   EFI_GUID mAmiTseOemPortingVar1Guid =  OEM_PORT_VARIABEL1;
   
    if(pTseSetuptransferInterface == NULL){
        Status = pBS->LocateProtocol(&gTseTransferGuid, NULL, &pTseSetuptransferInterface);
        if(EFI_ERROR(Status)) return;
    }
  
    OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
    Status = gRT->GetVariable(L"OemPortVar1", &mAmiTseOemPortingVar1Guid, NULL,
                &OemSetupDataSize, &OemSetupData);
    TRACE ((-1, "\nOemPortVar1 Status = %d.", Status));  
    if(EFI_ERROR(Status)) return;
    
    Status = pTseSetuptransferInterface->VarGetBuffer(L"OemPortVar1",mAmiTseOemPortingVar1Guid,(UINT8**)&OemSetupDataPtr,&OemSetupDataSize);
    if(!EFI_ERROR(Status)) {
      gAdvanceModeInit = 1;
        if(OemSetupDataPtr->OemHide == 0) {
            OemSetupDataPtr->OemHide = 0x11;
        }else {
            OemSetupDataPtr->OemHide = 0;
        }
    
    OemSetupData.OemHide = OemSetupDataPtr->OemHide;
    gOemHideValue = OemSetupDataPtr->OemHide;
    gRT->SetVariable( L"OemPortVar1", \
                      &mAmiTseOemPortingVar1Guid, \
                      EFI_VARIABLE_BOOTSERVICE_ACCESS, \
                      sizeof(OEM_NO_DEFAULT_VARIABLES), \
                      &OemSetupData);
    }
   
    return;
}

VOID LoadedOemAdvanceModeDefaults()
{
   EFI_STATUS  Status;
   EFI_GUID gTseTransferGuid = TSE_TRANSFER_GUID;  
   OEM_NO_DEFAULT_VARIABLES             *OemSetupDataPtr;//,OemSetupData;
   UINTN                                 OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
   EFI_GUID mAmiTseOemPortingVar1Guid =  OEM_PORT_VARIABEL1;
   
    if(pTseSetuptransferInterface == NULL || gAdvanceModeInit == 0) return;
    
    Status = pTseSetuptransferInterface->VarGetBuffer(L"OemPortVar1",mAmiTseOemPortingVar1Guid,(UINT8**)&OemSetupDataPtr,&OemSetupDataSize);
    if(!EFI_ERROR(Status)) {
            OemSetupDataPtr->OemHide = gOemHideValue;
    }
   
    return;
}

VOID InitOemAdvanceModeKey()
{
   EFI_STATUS                           Status;
   OEM_NO_DEFAULT_VARIABLES             OemSetupData;
   UINTN                                OemSetupDataSize = sizeof(OEM_NO_DEFAULT_VARIABLES);
   EFI_GUID                             mAmiTseOemPortingVar1Guid =  OEM_PORT_VARIABEL1;

    //Machong 2015/07/20 {
    Status = pRS->GetVariable(L"OemPortVar1", &mAmiTseOemPortingVar1Guid, NULL,
                                &OemSetupDataSize, &OemSetupData);
    TRACE ((-1, "\nInitOemAdvanceModeKey Status = %d.", Status));
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
    //Machong 2015/07/20 } 

    return;
}
#endif //defined(SETUP_ADVANCE_MODE_HOT_KEY_SUPPORT)&& (SETUP_ADVANCE_MODE_HOT_KEY_SUPPORT == 1) //<lvych00120160314->

VOID OemSetupLoadDefault(VOID)
{
	EFI_GUID gSetupLoadDefaultGuid = SETUP_LOAD_DEFAULT_GUID;

    //<Wangyia001-20171225 Improve the code about Pcie Device List funciton +>
    //EfiLibNamedEventSignal(&gSetupLoadDefaultGuid);
    EfiLibNamedEventSignal(&gOemSetupLoadDefaultGuid);
    //<Wangyia001-20171225 Improve the code about Pcie Device List funciton ->
}

//<Machong-20170405-For HDD Frozen switch+>
#if OVERRIDE_TSEIDEPasswordFreezeDevices
extern VOID  IDEPasswordFreezeDevices();
VOID TSEIDEPasswordFreezeDevices()
{
#if defined SECURITY_SUPPORT && SECURITY_SUPPORT == 1
  SETUP_DATA                   SetupData;
  UINTN                        VariableSize;
  EFI_STATUS                   Status;
  EFI_GUID SetupVariableGuid = SETUP_GUID;

  TRACE((-1, "Override TSEIDEPasswordFreezeDevices Start\n")); 
  VariableSize = sizeof(SETUP_DATA);
  pBS->SetMem(&SetupData, sizeof(SETUP_DATA), 0);
  
  Status = pRS->GetVariable(L"Setup", &SetupVariableGuid, NULL, &VariableSize, &SetupData);
  if (EFI_ERROR(Status)) {
      pBS->SetMem(&SetupData, sizeof(SETUP_DATA), 0);
      return;
  }
  
  TRACE((-1, "SetupData.FrozenLock=%d\n",SetupData.FrozenLock)); 
  if(SetupData.FrozenLock)
    IDEPasswordFreezeDevices();
  TRACE((-1, "Override TSEIDEPasswordFreezeDevices End\n")); 
#endif
}
#endif
//<Machong-20170405-For HDD Frozen switch->
