#ifndef __WORK_LOADING_SETTING__
#define __WORK_LOADING_SETTING__

#define MaxValidSetting 3
#define InvalidSetting  0xFF

#define NB_SMN_INDEX_EXT_3_BIOS 0x00C0
#define NB_SMN_INDEX_3_BIOS 0x00C4
#define NB_SMN_DATA_3_BIOS 0x00C8

//SGEZT#29383:Remove P-STATE Control Option+> #define PStateMode(Mode) "Advanced P-State",Mode,PSTATE_NORMAL,"Normal",PSTATE_PERFORMANCE,"Performance",InvalidSetting,"NULL"
#define FixPstateControl(Mode) "Fix P-State Control",Mode,FIX_PSTATE_CONTROL_AUTO,"Auto",FIX_PSTATE_CONTROL_P0,"P0",FIX_PSTATE_CONTROL_P2,"P2"
#define DeterminismSlider(Mode) "Determinism Slider",Mode,DETERMINISM_SLIDER_POWER,"Power",DETERMINISM_SLIDER_PERFORMANCE,"Performance",DETERMINISM_SLIDER_AUTO,"Auto"
#define cTDPControl(Mode) "cTDP",Mode,CTDP_AUTO,"Auto",CTDP_MANUAL,"Manual",InvalidSetting,"NULL"
#define SmtMode(Mode) "SMT",Mode,SMT_DISABLE,"Disabled",SMT_ENABLE,"Enabled",InvalidSetting,"NULL"
#define IOMMU(Mode) "IOMMU",Mode,IOMMU_DISABLE,"Disabled",IOMMU_ENABLE,"Enabled",InvalidSetting,"NULL"
#define SVM(Mode) "SVM",Mode,SVM_DISABLE,"Disabled",SVM_ENABLE,"Enabled",InvalidSetting,"NULL"
#define SRIOV(Mode) "SR-IOV",Mode,SRIOV_DISABLE,"Disabled",SRIOV_ENABLE,"Enabled",InvalidSetting,"NULL"
#define CorePerformanceBoost(Mode) "Core Performanced Boost",Mode,PERFORMANCE_BOOST_DISABLED,"Disabled",PERFORMANCE_BOOST_AUTO,"Enabled",InvalidSetting,"NULL"
#define GlobalCState(Mode) "Global C-State",Mode,GLOBAL_C_DISABLED,"Disabled",GLOBAL_C_ENABLED,"Enabled",GLOBAL_C_AUTO,"Auto"

typedef enum
{
  PSTATE_NORMAL = 0,
  PSTATE_PERFORMANCE
} P_STATE;
typedef enum
{
  FIX_PSTATE_CONTROL_AUTO = 0,
  FIX_PSTATE_CONTROL_P0,
  FIX_PSTATE_CONTROL_P1,
  FIX_PSTATE_CONTROL_P2
} FIX_PSTATE_CONTROL;

typedef enum
{
  DETERMINISM_SLIDER_POWER = 0,
  DETERMINISM_SLIDER_PERFORMANCE,
  DETERMINISM_SLIDER_AUTO = 0xF
} DETERMINISM_SLIDER;

typedef enum
{
  CTDP_AUTO,
  CTDP_MANUAL
} CTDP_CNTL_SETTING;

typedef enum
{
  SMT_DISABLE,
  SMT_ENABLE
} SMT_CNTL_SETTING;

typedef enum
{
  IOMMU_DISABLE = 0,
  IOMMU_ENABLE
} IOMMU_SETTING;

typedef enum
{
  SVM_DISABLE = 0,
  SVM_ENABLE
} SVM_STATE;

typedef enum
{
  SRIOV_DISABLE = 0,
  SRIOV_ENABLE
} SRIOV_SETTING;

typedef enum
{
  PERFORMANCE_BOOST_DISABLED,
  PERFORMANCE_BOOST_AUTO
} PERFORMANCE_BOOST_SETTING;

typedef enum
{
  GLOBAL_C_DISABLED,
  GLOBAL_C_ENABLED,
  GLOBAL_C_AUTO
} GLOBAL_C_SETTING;

typedef enum
{
  //SGEZT#29383:Remove P-STATE Control Option+> PStateModeOffset,
  FixPState,
  DeterminismSliderOffset,
  cTDPControlOffset,
  SmtModeOffset,
  IOMMUOffset,
  SVMOffset,
  SRIOVOffset,
  CorePerformanceBoostOffset,
  GlobalCStateOffset,
  MaxParamSettingPreserved
} SETTING_OFFSET;

typedef struct{

  UINT8   ValidSetting;
  CHAR8   Description[15];
}WORK_LOADING_DES;
typedef struct _WORKLOAD_TUNING_SETTING
{
  CHAR8 VariableName[30];
  UINT8 Setting;
  WORK_LOADING_DES  WorkloadingDesc[MaxValidSetting];
} WORKLOAD_TUNING_SETTING;


typedef struct _WORKLOAD_TUNING_MODE
{
  UINT8 WorkLoadingMode;
  WORKLOAD_TUNING_SETTING WorkLoadingSetting[MaxParamSettingPreserved];
} WORKLOAD_TUNING_MODE;



/**
Note:
All Related Param Setting Sequence Should Be Following The Enum Definition of SETTING_OFFSET.
Otherwise, The Setting Will Mismatch!!!
**/
//SGEZT#29383:Remove P-STATE Control Option+>
WORKLOAD_TUNING_MODE WorkLoadTuning[] = {
    //Balanced Mode
    {
        BalancedMode, FixPstateControl(FIX_PSTATE_CONTROL_AUTO),DeterminismSlider(DETERMINISM_SLIDER_PERFORMANCE), cTDPControl(CTDP_AUTO),
        SmtMode(SMT_ENABLE), IOMMU(IOMMU_DISABLE), SVM(SVM_ENABLE), SRIOV(SRIOV_ENABLE), CorePerformanceBoost(PERFORMANCE_BOOST_AUTO), GlobalCState(GLOBAL_C_DISABLED)},
    //Power Efficiency Mode
    {
        PowerEfficiencyMode,FixPstateControl(FIX_PSTATE_CONTROL_AUTO),DeterminismSlider(DETERMINISM_SLIDER_POWER), cTDPControl(CTDP_AUTO),
        SmtMode(SMT_ENABLE), IOMMU(IOMMU_DISABLE), SVM(SVM_DISABLE), SRIOV(SRIOV_DISABLE), CorePerformanceBoost(PERFORMANCE_BOOST_AUTO), GlobalCState(GLOBAL_C_DISABLED)},
    //High Performance Mode
    {
        HighPerformanceMode,FixPstateControl(FIX_PSTATE_CONTROL_P0), DeterminismSlider(DETERMINISM_SLIDER_PERFORMANCE), cTDPControl(CTDP_MANUAL),
        SmtMode(SMT_ENABLE), IOMMU(IOMMU_DISABLE), SVM(SVM_DISABLE), SRIOV(SRIOV_DISABLE), CorePerformanceBoost(PERFORMANCE_BOOST_AUTO), GlobalCState(GLOBAL_C_DISABLED)},
    //Virtualization Mode
    {
        VirtualizationMode,FixPstateControl(FIX_PSTATE_CONTROL_AUTO), DeterminismSlider(DETERMINISM_SLIDER_PERFORMANCE), cTDPControl(CTDP_AUTO),
        SmtMode(SMT_ENABLE), IOMMU(IOMMU_ENABLE), SVM(SVM_ENABLE), SRIOV(SRIOV_ENABLE), CorePerformanceBoost(PERFORMANCE_BOOST_AUTO), GlobalCState(GLOBAL_C_DISABLED)},
    //BIOSDefaultSetting
    {
        BiosDefaultMode,FixPstateControl(FIX_PSTATE_CONTROL_AUTO), DeterminismSlider(DEFAULT_DETERNIBISM_SLIDER_POLICY), cTDPControl(DEFAULT_CTDP_CONTROL_POLICY),
        SmtMode(DEFAULT_SMT_CONTROL_POLICY), IOMMU(DEFAULT_CBS_IOMMU), SVM(DEFAULT_SVM_CONTROL_POLICY), SRIOV(DEFAULT_PCIBUS_ONEOF_SRIOV), CorePerformanceBoost(DEFAULT_CORE_PERFORMANCE_BOOST), GlobalCState(DEFAULT_GLOBAL_C_STATE_CONTROL)},
    //Manual Mode
    {
        ManualMode,FixPstateControl(FIX_PSTATE_CONTROL_AUTO),DeterminismSlider(DEFAULT_DETERNIBISM_SLIDER_POLICY), cTDPControl(DEFAULT_CTDP_CONTROL_POLICY),
        SmtMode(DEFAULT_SMT_CONTROL_POLICY), IOMMU(DEFAULT_CBS_IOMMU), SVM(DEFAULT_SVM_CONTROL_POLICY), SRIOV(DEFAULT_PCIBUS_ONEOF_SRIOV), CorePerformanceBoost(DEFAULT_CORE_PERFORMANCE_BOOST), GlobalCState(DEFAULT_GLOBAL_C_STATE_CONTROL)},
};
//SGEZT#29383:Remove P-STATE Control Option ->
#endif