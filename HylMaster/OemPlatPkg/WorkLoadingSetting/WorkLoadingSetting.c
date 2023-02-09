
#include <AmiDxeLib.h>
#include <AmiCspLib.h>

// Produced Protocols

// Consumed Protocols
#include <Protocol\PciIo.h>
#include <Uefi\UefiBaseType.h>
#include <Token.h>
#include "Setup.h"
#include "Library/PcdLib.h"
#include "Library/AmdCbsVariable.h"

#include "WorkLoadingSetting.h"

#include "OemLib.h"
//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue+>
#include "Library/UefiLib.h"
#define  EFI_TPL_CALLBACK 8
//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue->

#define SyncDebugModeSetting(CurrentSetting, ValidSetting, SyncFlag) \
  if (CurrentSetting != ValidSetting)                                \
  {                                                                  \
    CurrentSetting = ValidSetting;                                   \
    SyncFlag = TRUE;                                                 \
  }

//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue+>
#define WorkLoadingSetting(index,ParamOffset)\
    WorkLoadTuning[##index##].WorkLoadingSetting[##ParamOffset##].Setting
//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue->

EFI_GUID gEfiSetupGuid = SETUP_GUID;

VOID OemNbSmnRead(
    IN UINT32 DieNum,
    IN UINT32 Address,
    IN UINT32 *Value)
{
  UINT32 read_value = 0;
  PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, DieNum);
  PciWrite32(NB_SMN_INDEX_3_BIOS, Address);
  read_value = PciRead32(NB_SMN_DATA_3_BIOS);
  *Value = read_value;
  PciWrite32(NB_SMN_INDEX_EXT_3_BIOS, 0);
}


//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   SyncIpmiSetupConfig
//
// Description: Check for the Ipmi OEM Setup Configuration and Set 
// certain variable bmc specified
//
// Input: None
//
// Output:  None
//
//<AMI_PHDR_END>
//**********************************************************************
EFI_STATUS
EFIAPI
SyncWorkLoadingSetting (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  
  SETUP_DATA SetupData;
  CBS_CONFIG CbsSetupData;

  UINTN VariableSize = sizeof(SETUP_DATA);
  UINTN CbsVariableSize = sizeof(CBS_CONFIG);

  EFI_STATUS Status;
  UINT32 SetupAttributes;
  UINT32 CBSAttributes;

  BOOLEAN SyncWorkLoadingFlag = FALSE;

  UINT8 iWorkingMode;

  UINT32 Value1 = 0;
  UINT32 Value2 = 0;
  UINT32 TDP = 0;

  MODULE_START

  /**
  Note:Get Setup Variable Or CBS Related Variable
  **/
  Status = pRS->GetVariable(L"Setup", &gEfiSetupGuid, &SetupAttributes, &VariableSize, &SetupData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  Status = pRS->GetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid, &CBSAttributes, &CbsVariableSize, &CbsSetupData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  /**
  Note:
  If Workloading Is Set To Auto,No Need Update Any Setting,Just Keep It!!!
  If Workloading Is Set To Bios Defauly, The Setting Will Be Loaded To Default Setting
  **/
  if (SetupData.Workloading == ManualMode)
    MODULE_END(Status = EFI_NO_MAPPING)

  for (iWorkingMode = 0; WorkLoadTuning[iWorkingMode].WorkLoadingMode != ManualMode; iWorkingMode++)
    if (SetupData.Workloading == WorkLoadTuning[iWorkingMode].WorkLoadingMode)
      break;
//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue+>
  //PSTATE_PERFORMANCE
  SyncDebugModeSetting(SetupData.ADVANCE_P_STATE, WorkLoadingSetting(iWorkingMode,PStateModeOffset), SyncWorkLoadingFlag);
  //CbsCmnSVMCtrl
  SyncDebugModeSetting(SetupData.SriovSupport, WorkLoadingSetting(iWorkingMode,SRIOVOffset), SyncWorkLoadingFlag);
  //CbsCmnDeterminismSlider
  SyncDebugModeSetting(CbsSetupData.CbsCmnDeterminismSlider, WorkLoadingSetting(iWorkingMode,DeterminismSliderOffset), SyncWorkLoadingFlag);
  //CbsCmnSVMCtrl
  SyncDebugModeSetting(CbsSetupData.CbsCmnSVMCtrl, WorkLoadingSetting(iWorkingMode,SVMOffset), SyncWorkLoadingFlag);
  //CbsCmnGnbNbIOMMU
  SyncDebugModeSetting(CbsSetupData.CbsCmnGnbNbIOMMU, WorkLoadingSetting(iWorkingMode,IOMMUOffset), SyncWorkLoadingFlag);
  //CbsCmncTDPCtl
  SyncDebugModeSetting(CbsSetupData.CbsCmncTDPCtl, WorkLoadingSetting(iWorkingMode,cTDPControlOffset), SyncWorkLoadingFlag);
  //CbsCmncTDPCtl
  SyncDebugModeSetting(CbsSetupData.CbsCpuSmtCtrl, WorkLoadingSetting(iWorkingMode,SmtModeOffset), SyncWorkLoadingFlag);
  //CbsCmnCpuCpb
  SyncDebugModeSetting(CbsSetupData.CbsCmnCpuCpb, WorkLoadingSetting(iWorkingMode,CorePerformanceBoostOffset), SyncWorkLoadingFlag);
  //CbsCmnCpuGlobalCstateCtrl
  SyncDebugModeSetting(CbsSetupData.CbsCmnCpuGlobalCstateCtrl, WorkLoadingSetting(iWorkingMode,GlobalCStateOffset), SyncWorkLoadingFlag);
//Mizl-20220312- SGEZT#22553:Ipmrove WorkLoading Logic & Fix Load Default Issue->
  /**
  Note:Update cTDP Setting According To cTDP Ctl Setting
  **/
  if (CbsSetupData.CbsCmncTDPCtl == CTDP_MANUAL)
  {
    OemNbSmnRead(0, 0x5d374, &Value1);
    OemNbSmnRead(0, 0x5d378, &Value2);
    //Mizl-20220312-SGEZT#22553 Improve WorkLoading Design+>
    //Acording Hygon's Feedback,cTDP Max Setting Is Limitted By HW
    //According To Project Manager's Request,Update CTDP=TDP+20
    TDP = ((Value2 & 0x00000001) << 7) | ((Value1 & 0xFE000000) >> 25)+20;
    //Mizl-20220312-SGEZT#22553 Improve WorkLoading Design->
  }
  CbsSetupData.CbsCmncTDPLimit = TDP;

  if (SyncWorkLoadingFlag != TRUE)
    MODULE_END(Status = EFI_ALREADY_STARTED)
  //Sync Setting
  Status = pRS->SetVariable(L"Setup", &gEfiSetupGuid, SetupAttributes, VariableSize, &SetupData);
  if (EFI_ERROR(Status))
    MODULE_END(Status)

  Status = pRS->SetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid, CBSAttributes, CbsVariableSize, &CbsSetupData);
  MODULE_END(Status)
}
/**
    This function contain all the initializes code in DXE Stage for SyncSetupSetting.

    @param ImageHandle - Image Handle.
    @param SystemTable - Pointer to the System table.

    @retval EFI_STATUS.

    @note  None.

**/

EFI_STATUS
WorkLoadingSetting_Entry(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS  Status;

  InitAmiLib(ImageHandle, SystemTable);

  MODULE_START
  //Mizl-20220312-SGEZT#22553 Improve WorkLoading Design+>
  Status =  EfiNamedEventListen (
              &gOemNvramUpdateGuid,
              EFI_TPL_CALLBACK,
              SyncWorkLoadingSetting,
              NULL,
              NULL
              );
  //Mizl-20220312-SGEZT#22553 Improve WorkLoading Design->

  MODULE_END(Status)
}