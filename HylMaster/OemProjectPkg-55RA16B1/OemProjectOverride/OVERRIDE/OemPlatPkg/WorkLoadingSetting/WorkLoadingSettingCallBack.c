#include <AmiDxeLib.h>
#include <AmiCspLib.h>
//Produced Protocols

//Consumed Protocols
#include <Protocol/PciIo.h>
#include <Uefi/UefiBaseType.h>
#include <Token.h>
#include "Setup.h"
#include "Library/PcdLib.h"
#include "Library/AmdCbsVariable.h"
#include "WorkLoadingSetting.h"
#include "OemLib.h"
#include "Uefi/UefiInternalFormRepresentation.h"
#include "Library/BaseMemoryLib.h"
#include "Protocol/AMIPostMgr.h"
#include "Library/HiiLib.h"
#include "AmiDxeLib.h" //Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue+>

static AMI_POST_MANAGER_PROTOCOL *mPostMgr = NULL;

EFI_STRING WarningTitle = NULL;
EFI_STRING WarningMessage = NULL;
EFI_STRING HintMessageHeader = NULL;

EFI_STATUS WorkLoadingCallBack(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class,
    IN UINT16 SubClass,
    IN UINT16 KeyValue)
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_GUID gSetupGuid = SETUP_GUID;
  SETUP_DATA pSetup;
  SETUP_DATA *IfrNvData = &pSetup;
  UINTN BufferSize;

  CALLBACK_PARAMETERS *CallbackParameters = NULL;

  UINT8 iWorkingMode, iParam, iParamDesc;

  CHAR8 HintMessage[1024];
  CHAR16 wHintMessage[1024];

  BOOLEAN fFoundValidParam = FALSE;

  //<huangrj-20220531 sync SMT default value referring to boardid in Application Profile Configuration +>
    UINT32 Value;
    UINT8 BoardId;
  //<huangrj-20220531 sync SMT default value referring to boardid in Application Profile Configuration ->  
    
  MODULE_START

  if ((KeyValue != WORK_LOADING_KEY))
    MODULE_END(Status = EFI_INVALID_PARAMETER)

  CallbackParameters = GetCallbackParameters();

//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue+>
  if (CallbackParameters->Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD)
  {
    ZeroMem(&wHintMessage[0], 1024 * sizeof(CHAR16));
    InitString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_HINT_INFORMATION), wHintMessage);
    MODULE_END(Status = EFI_UNSUPPORTED);
  }

//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue->
  if (CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGED)
    MODULE_END(Status = EFI_SUCCESS);

  if (mPostMgr == NULL)
  {
    Status = pBS->LocateProtocol(&gAmiPostManagerProtocolGuid, NULL, &mPostMgr);
    if (EFI_ERROR(Status) || !mPostMgr)
    {
      MODULE_END(Status = EFI_SUCCESS)
    }
  }

  /**
  Note:Get Pop Message String
  **/
  if (WarningTitle == NULL)
  {
    WarningTitle = HiiGetString(HiiHandle, STR_WORK_LOADING_WARNING_TITLE_INFORMATION, "en-US");
    HiiSetString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_WARNING_TITLE_INFORMATION), WarningTitle, "en-US");

    WarningTitle = HiiGetString(HiiHandle, STR_WORK_LOADING_WARNING_TITLE_INFORMATION, "zh-chs");
    HiiSetString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_WARNING_TITLE_INFORMATION), WarningTitle, "zh-chs");
  }
  WarningTitle = HiiGetString(HiiHandle, STR_WORK_LOADING_WARNING_TITLE_INFORMATION, NULL);

  if (WarningMessage == NULL)
  {
    WarningMessage = HiiGetString(HiiHandle, STR_WORK_LOADING_WARNING_MESSAGE_INFORMATION, "en-US");
    HiiSetString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_WARNING_MESSAGE_INFORMATION), WarningMessage, "en-US");

    WarningMessage = HiiGetString(HiiHandle, STR_WORK_LOADING_WARNING_MESSAGE_INFORMATION, "zh-chs");
    HiiSetString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_WARNING_MESSAGE_INFORMATION), WarningMessage, "zh-chs");
  }
  WarningMessage = HiiGetString(HiiHandle, STR_WORK_LOADING_WARNING_MESSAGE_INFORMATION, NULL);

  if (HintMessageHeader == NULL)
  {
    HintMessageHeader = HiiGetString(HiiHandle, STR_WORK_LOADING_HINT_MESSAGE_INFORMATION, "en-US");
    HiiSetString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_HINT_MESSAGE_INFORMATION), HintMessageHeader, "en-US");

    HintMessageHeader = HiiGetString(HiiHandle, STR_WORK_LOADING_HINT_MESSAGE_INFORMATION, "zh-chs");
    HiiSetString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_HINT_MESSAGE_INFORMATION), HintMessageHeader, "zh-chs");
  }
  HintMessageHeader = HiiGetString(HiiHandle, STR_WORK_LOADING_HINT_MESSAGE_INFORMATION, NULL);
  /**
  Note:Get WorkLoading Mode
  **/
  BufferSize = sizeof(SETUP_DATA);
  Status = HiiLibGetBrowserData(&BufferSize, IfrNvData, &gSetupGuid, L"Setup");
  if (EFI_ERROR(Status))
    MODULE_END(Status);

  if (pSetup.Workloading == ManualMode)
  {
    mPostMgr->DisplayMsgBox(WarningTitle, WarningMessage, MSGBOX_TYPE_OK, NULL);
    InitString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_HINT_INFORMATION), WarningMessage);
    MODULE_END(Status = EFI_SUCCESS);
  }

  for (iWorkingMode = 0; WorkLoadTuning[iWorkingMode].WorkLoadingMode != ManualMode; iWorkingMode++)
    if (pSetup.Workloading == WorkLoadTuning[iWorkingMode].WorkLoadingMode)
      break;

  ZeroMem(&HintMessage[0], 1024 * sizeof(CHAR8));
  ZeroMem(&wHintMessage[0], 1024 * sizeof(CHAR16));

  Swprintf(wHintMessage, L"%s", HintMessageHeader);

  /**
  Note:Enumrate All Setting According To The WorkLoadTuning
  **/
  for (iParam = 0; iParam < MaxParamSettingPreserved; iParam++)
  {
    fFoundValidParam = FALSE;

    //<huangrj-20220531 sync SMT default value referring to boardid in Application Profile Configuration +>
    if (WorkLoadTuning[iWorkingMode].WorkLoadingMode == BiosDefaultMode && iParam == SmtModeOffset)
    {
        PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 1);
        PciWrite32(NB_SMN_INDEX_3_BIOS, (0x02D02500 + (BOARD_ID_GPIO << 2)));
        Value = PciRead32(NB_SMN_DATA_3_BIOS);
        BoardId = ((UINT8)(Value >> 16)) & 0x01;
           
        //clear in case other functions don't pay attention to this reg
        PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
        
        if (BoardId == 1)
        {
            WorkLoadTuning[iWorkingMode].WorkLoadingSetting[iParam].Setting = SMT_DISABLE; 
        }
    }
    //<huangrj-20220531 sync SMT default value referring to boardid in Application Profile Configuration ->
    
    for (iParamDesc = 0; WorkLoadTuning[iWorkingMode].WorkLoadingSetting[iParam].WorkloadingDesc[iParamDesc].ValidSetting != InvalidSetting &&
                         iParamDesc < MaxValidSetting;
         iParamDesc++)
      if (WorkLoadTuning[iWorkingMode].WorkLoadingSetting[iParam].Setting ==
          WorkLoadTuning[iWorkingMode].WorkLoadingSetting[iParam].WorkloadingDesc[iParamDesc].ValidSetting)
      {
        fFoundValidParam = TRUE;
        break;
      }

    if (fFoundValidParam == TRUE)
    {
      Swprintf(wHintMessage, L"%s\r\n%a:%a", wHintMessage, WorkLoadTuning[iWorkingMode].WorkLoadingSetting[iParam].VariableName,
               WorkLoadTuning[iWorkingMode].WorkLoadingSetting[iParam].WorkloadingDesc[iParamDesc].Description);
    }
  }

  mPostMgr->DisplayMsgBox(WarningTitle, wHintMessage, MSGBOX_TYPE_OK, NULL);

  InitString(HiiHandle, STRING_TOKEN(STR_WORK_LOADING_HINT_INFORMATION), wHintMessage);

  MODULE_END(Status = EFI_SUCCESS);
}
