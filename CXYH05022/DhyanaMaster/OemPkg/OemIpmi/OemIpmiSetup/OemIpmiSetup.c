/** @file
  
  Copyright (C) 2016 - 2017 Dawning Information Industry Co.,LTD.All rights reserved.
  
  Compatible with <OEM IPMI Command List V1.3> 
  
  This file contains the sub-function of OEM IPMI Command which
  Changes BIOS Setup Option Outband by OEM Ipmi Command
  
  $Revision:0A
  $Date:  Lvych 2016/12/01
  
  $Revision:0B
  $Date:  Machong 2016/12/26
  
  $Revision:0C
  $Date:  LiuYB 2017/6/15
  $Compatiable OEM IPMI Command List v1.3
  $Apply for AMD Naples Platform

  // 1. BMC INIT_DATA only for BMC get/set, BIOS do NOT need to touch it
  // 2. BMC MODI_DATA for bios get setup option
  // 3. NvramData will be modified by IpmiData if Option.Bit[7] is 1
  // 4. Send current setup options to BMC again after updating NvramData
  // 5. Reset system if NvramData updated
  
**/

#include "OemIpmiSetup.h"
#include <OemPCIeDev.h>

#define OPTION_PUT(NvData, IpmiData, Flag)       \
          do {                                   \
            if ((IpmiData) & (1 << 7)) {         \
              (NvData) = (IpmiData) & (0x7F);    \
              (Flag)   = 1;                      \
            }                                    \
          } while (0)

#define OPTION_VALIDATE(IpmiData)                \
          ((IpmiData) & (1 << 7))

// Extern Variables
extern  EFI_BOOT_SERVICES       *gBS;
extern  EFI_RUNTIME_SERVICES    *gRT;

EFI_IPMI_TRANSPORT *mIpmiTransportProtocol = NULL;

//<lvych001-20170830 Add "Fan Control Mode" and "AC Power Loss Policy" options sync with standard Ipmi command setting.+>
EFI_STATUS
GetSetACPowerLossPolicyFromStdIPMI (
    IN   EFI_IPMI_TRANSPORT   *IpmiTransportProtocol,
    IN   BOOLEAN              Set,
    IN OUT UINT8              *AcPowerLoss
  )
{
  EFI_STATUS  Status;
  UINT8       CommandData[10]={0};
  UINT8       CommandDataSize;
  UINT8       ResponseData[10]={0};
  UINT8       ResponseDataSize;

  if(Set){
    
    switch ( *AcPowerLoss ) {
      case 0x0:
        CommandData[0] = 0;
        break;
      case 0x1:
        CommandData[0] = 1;
        break;
      case 0x2:
        CommandData[0] = 2;
        break;
      default:
        CommandData[0] = 0;
        break;      	
    }
    
    CommandDataSize = 1;
    ResponseDataSize = 10; 
    Status = IpmiTransportProtocol->SendIpmiCommand (
              IpmiTransportProtocol,  
              IPMI_NETFN_CHASSIS,
              BMC_LUN,
              IPMI_CHASSIS_SET_POWER_RESTORE_POLICY,//Set Power Restore Policy Command
              (UINT8*) CommandData,
              CommandDataSize,
              (UINT8*) ResponseData,
              &ResponseDataSize
              );
    DEBUG((-1, "Set Power Restore Policy Command = %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
  }else{
    ResponseDataSize = 10;
    Status = IpmiTransportProtocol->SendIpmiCommand (
              IpmiTransportProtocol,
              IPMI_NETFN_CHASSIS,
              BMC_LUN,
              IPMI_CHASSIS_GET_STATUS,//Get Chassis Status Command
              NULL,
              0,
              (UINT8*) ResponseData,
              &ResponseDataSize
              );
    DEBUG((-1, "Get Chassis Status Command Status = %x\n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
    
    DEBUG((-1, "Get Chassis Status Command  ResponseData[0] =  %x, Status=%d\n", ResponseData[0], Status));
  
    switch ( (ResponseData[0]>>5)&0x3 ) { // Bit[6:5]
      case 0x0:
        *AcPowerLoss = 0;  //AcPowerLoss = 0  s5
        break;
      case 0x1:
        *AcPowerLoss = 1;  //last status
        break;
      case 0x2:
        *AcPowerLoss = 2;  //AcPowerLoss = 2  s0
        break;
      default:
        *AcPowerLoss = 0;  //AcPowerLoss = 0  s5
        break;
    }
    
    DEBUG((-1, "AcPowerLoss:  %x\n",*AcPowerLoss));
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetSetFanCtrlModeFromStdIPMI (
    IN   EFI_IPMI_TRANSPORT   *IpmiTransportProtocol,
    IN   BOOLEAN              Set,
    IN OUT UINT8              *FanMode
  )
{
  EFI_STATUS  Status;
  UINT8       CommandData[10]={0};
  UINT8       CommandDataSize;
  UINT8       ResponseData[10]={0};
  UINT8       ResponseDataSize;

  if(Set){
    CommandData[0] = 0;
    switch ( *FanMode ) {
      case 0x0:
        CommandData[1] = 0;   //MANUAL
        break;
      case 0x1:
        CommandData[1] = 1;   //BALANCE
        break;
      case 0x2:
        CommandData[1] = 2;   //SILENT
        break;
      case 0x3:
        CommandData[1] = 3;   //PERFORMANCE
        break;
    }
    
    CommandDataSize = 2;
    ResponseDataSize = 10;
    Status = IpmiTransportProtocol->SendIpmiCommand (
              IpmiTransportProtocol,
              OEM_IPMI_NET_FUN,
              BMC_LUN,
              0x0b, //CMD_OEM_SET_FAN_POLICY
              (UINT8*) CommandData,
              CommandDataSize,
              (UINT8*) ResponseData,
              &ResponseDataSize
              );
    DEBUG((-1, "Set fan Policy Command = %r \n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
  }else{
    CommandData[0] = 0;
    CommandDataSize = 1;
    ResponseDataSize = 10;
    Status = IpmiTransportProtocol->SendIpmiCommand (
              IpmiTransportProtocol,
              OEM_IPMI_NET_FUN,
              BMC_LUN,
              0x0c, //CMD_OEM_GET_FAN_POLICY
              (UINT8*) CommandData,
              CommandDataSize,
              (UINT8*) ResponseData,
              &ResponseDataSize
              );
    DEBUG((-1, "Get fan Status Command Status = %x\n", Status));
    if (EFI_ERROR (Status)) {
        return Status;
    }
  
    switch ( ResponseData[0] ) {
      case 0x0:
        *FanMode = 0;  //MANUAL
        break;
      case 0x1:
        *FanMode = 1;  //BALANCE
        break;
      case 0x2:
        *FanMode = 2;  //SILENT
        break;
      case 0x3:
        *FanMode = 3;  //PERFORMANCE
        break;
    }
    
    DEBUG((-1, "FanMode:  %x\n",*FanMode));
  }

  return EFI_SUCCESS;
}

EFI_STATUS UpdateStandardIpmitoSetup(
    IN   EFI_IPMI_TRANSPORT   *IpmiTransportProtocol,
    OUT  UINT8                *UpdatedSetupFlag
)
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  SERVER_MGMT_CONFIGURATION_DATA      ServerMgmtConfiguration;
  UINT32                              AttributesMgmt;
  UINTN                               VariableSizeMgmt;
  UINT8                               AcPowerLoss;
  UINT8                               FanMode;
  UINT8                               UpdatedSetupData = 0;
  OEM_LAST_SETUP_DATA                 OemLastData = {0};
  OEM_LAST_SETUP_DATA                 OriginOemLastData = {0};
  UINTN                               VariableSize =0;
  UINT32                              Attributes = 0;
  UINT8                               lValidFlag;


  VariableSize = sizeof(OEM_LAST_SETUP_DATA);
  Status=gRT->GetVariable(L"OEML_DATA",&gAmiTseOemPortingVar1Guid, &Attributes, &VariableSize, &OemLastData);
  if(EFI_ERROR(Status)){
    lValidFlag=0;
    if(Status==EFI_NOT_FOUND) {
      lValidFlag = 0x80;
    }
  }else{
    lValidFlag = 0x01;
  }

  gBS->CopyMem((UINT8 *)&OriginOemLastData, (UINT8 *)&OemLastData, VariableSize);

  VariableSizeMgmt = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
  Status = gRT->GetVariable (
          L"ServerSetup",
          &gEfiServerMgmtSetupVariableGuid,
          &AttributesMgmt,
          &VariableSizeMgmt,
          &ServerMgmtConfiguration);

  if(!EFI_ERROR(Status)) {
    if((lValidFlag&0x01) && (ServerMgmtConfiguration.AcPowerCtl != OemLastData.AcPowerLoss)){
      //Sync sce modify to stdIPMI
      Status = GetSetACPowerLossPolicyFromStdIPMI(IpmiTransportProtocol,TRUE,(UINT8*)&ServerMgmtConfiguration.AcPowerCtl);
    }else{
      Status = GetSetACPowerLossPolicyFromStdIPMI(IpmiTransportProtocol,FALSE,&AcPowerLoss);
      if(!EFI_ERROR(Status)) {
        if(ServerMgmtConfiguration.AcPowerCtl != AcPowerLoss){
          ServerMgmtConfiguration.AcPowerCtl = AcPowerLoss;
          UpdatedSetupData = 1;
        }
      }
    }
    OemLastData.AcPowerLoss = ServerMgmtConfiguration.AcPowerCtl;
    
    if((lValidFlag&0x01) && (ServerMgmtConfiguration.FanCtl_Mode != OemLastData.FanCtl_Mode)){
      //Sync sce modify to stdIPMI
      Status = GetSetFanCtrlModeFromStdIPMI(IpmiTransportProtocol,TRUE,(UINT8*)&ServerMgmtConfiguration.FanCtl_Mode);
    }else{
      Status = GetSetFanCtrlModeFromStdIPMI(IpmiTransportProtocol,FALSE,&FanMode);
      if(!EFI_ERROR(Status)) {
        if(ServerMgmtConfiguration.FanCtl_Mode != FanMode){
          ServerMgmtConfiguration.FanCtl_Mode = FanMode;
          UpdatedSetupData = 1;
        }
      }
    }
    OemLastData.FanCtl_Mode = ServerMgmtConfiguration.FanCtl_Mode;
  }
  
  //Update oem last data
  if((lValidFlag&0x01) != 0x01){
    Attributes = (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE);
  }

  if( MemCmp((UINT8 *)&OriginOemLastData, (UINT8 *)&OemLastData, sizeof(OEM_LAST_SETUP_DATA)) ){
    Status=gRT->SetVariable(L"OEML_DATA",&gAmiTseOemPortingVar1Guid, Attributes, sizeof(OEM_LAST_SETUP_DATA), &OemLastData);
  }
  if( UpdatedSetupData )
  {
      gRT->SetVariable(
              L"ServerSetup",
              &gEfiServerMgmtSetupVariableGuid,
              AttributesMgmt,
              sizeof(SERVER_MGMT_CONFIGURATION_DATA),
              &ServerMgmtConfiguration );
      //BMC function,don't reset and just get setting. *UpdatedSetupFlag |= OEM_UPDATE_SETUP;
  }

  return Status;
  
}

EFI_STATUS UpdateSetuptoStandardIpmi(
    EFI_IPMI_TRANSPORT   *IpmiTransportProtocol
)
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  SERVER_MGMT_CONFIGURATION_DATA      ServerMgmtConfiguration;
  UINT32                              AttributesMgmt;
  UINTN                               VariableSizeMgmt;
  
  
  VariableSizeMgmt = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
  Status = gRT->GetVariable (
          L"ServerSetup",
          &gEfiServerMgmtSetupVariableGuid,
          &AttributesMgmt,
          &VariableSizeMgmt,
          &ServerMgmtConfiguration);
  if(!EFI_ERROR(Status)) {
    Status = GetSetACPowerLossPolicyFromStdIPMI(IpmiTransportProtocol,TRUE,(UINT8*)&ServerMgmtConfiguration.AcPowerCtl);
    Status = GetSetFanCtrlModeFromStdIPMI(IpmiTransportProtocol,TRUE,(UINT8*)&ServerMgmtConfiguration.FanCtl_Mode);
  }
  
  return Status;
}
//<lvych001-20170830 Add "Fan Control Mode" and "AC Power Loss Policy" options sync with standard Ipmi command setting.->

/**
  OemUpdateSetuptoIpmi :
 
  Update INI_DATA of BMC with BIOS Setup Data
  
  @return EFI_STATUS Status 
**/
EFI_STATUS
EFIAPI
GetNvData(
  IN  OEM_IPMI_SETUP_DATA  *pIpmiSetupData
  )
{
  EFI_STATUS                      Status;
  SETUP_DATA                      NvDataSetup;
  SERVER_MGMT_CONFIGURATION_DATA  NvDataServerMgmtSetup;
  CBS_CONFIG                      NvDataAmdSetup;
  UINTN                           VariableSize;
  UINT32                          VariableAttribute;
  //Mizl-20210813- Temp Remove PXE Sync Feature +>
  #if 0
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
  UINTN                           VarSizeNetwork;
  NETWORK_STACK                   NVDataNetworkStack;
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. ->
  #endif
  //Mizl-20210813- Temp Remove PXE Sync Feature ->
//<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
  PXE_SELECT_SETUP_DATA           PxeVariableData;
  UINT32                          AttributesPxe;
  UINTN                           VariableSizePxe;  
  UINT8                           PXEindex;   
  UINT8                           PXE_Variable_Update;
#endif
//<Chendl001-20170120 Network PXE Function modified by OOB ->
    
//<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1)
  UINT32                          Attributes;
  UINTN                           NicVarSize;
  NIC_CTRL_SETUP_DATA             NicVarData;
  UINT8                           Nic_Variable_Update;
  UINT8                           NicIndex;  
#endif
//<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  
//<Yuanshai-20190710 For :Move PciDevEN OpROMEN SriovEN from "Setup" to "OEMPCIE_DEV" --BEGIN>
  #if defined(OemPCIeDev_SUPPORT) && (OemPCIeDev_SUPPORT == 1) 
  OEM_PCIE_DEV_DATA_PROTOCOL       *OemPcieDevProtocol = NULL;
  UINT8                              OemOpROMEN[PCIDEV_ENTRY_NUMBER];
  
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
  UINT8                              OemSriovEN[MAX_DEV_COUNT];
#endif
  
  Status = gBS->LocateProtocol(&gOemPcieDevProtocolGuid, NULL, (VOID**)&OemPcieDevProtocol);
  if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[OemDbg]LocateProtocol gOemPcieDevProtocolGuid %r\n",Status));
      return Status;
  }
  
  Status = OemPcieDevProtocol->GetSetPcieDevPolicy(OemOpROMEN,OPROMEN,TRUE);
  if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[OemDbg]OemPcieDevProtocol->GetSetPcieDevPolicy OemSriovEN %r\n",Status));
      return Status;
  }
  
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
  Status = OemPcieDevProtocol->GetSetPcieDevPolicy(OemSriovEN,SRIOVEN,TRUE);
  if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[OemDbg]OemPcieDevProtocol->GetSetPcieDevPolicy OemSriovEN %r\n",Status));
      return Status;
  }
#endif
  
  #endif 
//<Yuanshai-20190710 For :Move PciDevEN OpROMEN SriovEN from "Setup" to "OEMPCIE_DEV" --END>  

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - GetNvData\n"));

  if (pIpmiSetupData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //-----------------------------------------------------------------
  // Get Nvram Value
  //-----------------------------------------------------------------
  VariableSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable(
             L"Setup",
             &gEfiSetupVariableGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataSetup
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  VariableSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
  Status = gRT->GetVariable(
             L"ServerSetup", 
             &gEfiServerMgmtSetupVariableGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataServerMgmtSetup
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  VariableSize = sizeof(CBS_CONFIG);
  Status = gRT->GetVariable(
             L"AmdSetup", 
             &gCbsSystemConfigurationGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataAmdSetup
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
  PXE_Variable_Update = 0;
  VariableSizePxe = sizeof(PXE_SELECT_SETUP_DATA);
  Status = gRT->GetVariable(
                 L"PxeSelVar",
                 &gAmiTseOemPortingVar2Guid, 
                 &AttributesPxe, 
                 &VariableSizePxe, 
                 &PxeVariableData
                 ); 
  DEBUG((EFI_D_INFO, "[CMCC-NFV]: Get PxeSelVar Status = %r ...\n", Status));
  if (Status == EFI_SUCCESS) {
    PXE_Variable_Update = 1;
  } else {
    PXE_Variable_Update = 0;
    return Status;//<Lvshh001-20190623 OOB configuration error.Don't sent "0" to BMC when first boot>
  }
#endif
  //<Chendl001-20170120 Network PXE Function modified by OOB ->
    
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1) 
  Nic_Variable_Update = 0;
  NicVarSize = sizeof(NIC_CTRL_SETUP_DATA);
  Status = gRT->GetVariable(
                 L"NicCtrlVar",
                 &gAmiTseOemPortingVar2Guid, 
                 &Attributes, 
                 &NicVarSize, 
                 &NicVarData
                 ); 
  DEBUG((EFI_D_INFO, "[CMCC-NFV]: Get NicCtrlVar Status = %r ...\n", Status));
  if (Status == EFI_SUCCESS) {
      Nic_Variable_Update = 1;
  } else {
      Nic_Variable_Update = 0;
  }
#endif
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  
  //Mizl-20210813- Temp Remove PXE Sync Feature +>
  #if 0
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
  VarSizeNetwork = sizeof (NETWORK_STACK);
  Status = gRT->GetVariable(L"NetworkStackVar",
                   &gEfiNetworkStackSetupGuid,
                   &VariableAttribute,
                   &VarSizeNetwork,
                   &NVDataNetworkStack);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. ->
  #endif
  //Mizl-20210813- Temp Remove PXE Sync Feature ->

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - GetNvData: Init IPMI Data\n"));

  //-----------------------------------------------------------------
  // Init IpmiData with NvData
  //-----------------------------------------------------------------
  MemSet(pIpmiSetupData->CpuOptions,        sizeof(OEM_IPMI_SETUP_CPU_OPTIONS),        0);
  MemSet(pIpmiSetupData->MemoryOptions,     sizeof(OEM_IPMI_SETUP_MEMORY_OPTIONS),     0);
  MemSet(pIpmiSetupData->PowerOptions,      sizeof(OEM_IPMI_SETUP_POWER_OPTIONS),      0);
  MemSet(pIpmiSetupData->IoOptions,         sizeof(OEM_IPMI_SETUP_IO_OPTIONS),         0);
  MemSet(pIpmiSetupData->ThermalOptions,    sizeof(OEM_IPMI_SETUP_THERMAL_OPTIONS),    0);
  MemSet(pIpmiSetupData->ManagementOptions, sizeof(OEM_IPMI_SETUP_MANAGEMENT_OPTIONS), 0);
  MemSet(pIpmiSetupData->BootOptions,       sizeof(OEM_IPMI_SETUP_BOOT_OPTIONS),       0);
  
//<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
  MemSet(pIpmiSetupData->PxeOptions,        sizeof(OEM_IPMI_PXE_OPTIONS),              0);
#endif
//<Chendl001-20170120 Network PXE Function modified by OOB ->

//<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1)
  MemSet(pIpmiSetupData->BiosNicSriovOptions,sizeof(OEM_IPMI_NIC_SRIOV_OPTIONS),       0);
#endif
//<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
    
  MemSet(pIpmiSetupData->MiscOptions,       sizeof(OEM_IPMI_SETUP_MISC_OPTIONS),       0); //<Yaosr001-20190731 add MISC option>
  //-----------------------------------------------------------------
  // Type 0 -- Cpu
  //-----------------------------------------------------------------
  pIpmiSetupData->CpuOptions->SmtMode                   =  NvDataAmdSetup.CbsCpuSmtCtrl;
//<huangjin001-20191018 Fix build error for svm mode +>    
//pIpmiSetupData->CpuOptions->SvmMode                   = NvDataSetup.SVM_MODE;
  pIpmiSetupData->CpuOptions->SvmMode                   = NvDataAmdSetup.CbsCmnSVMCtrl;
//<huangjin001-20191018 Fix build error for svm mode +>    
  pIpmiSetupData->CpuOptions->SriovSupport              = NvDataSetup.SriovSupport;//<YinQiang001-20171102 Add SR-IOV Support Ipmi Command>

  pIpmiSetupData->CpuOptions->CorePerfBoost             = NvDataAmdSetup.CbsCmnCpuCpb;
  pIpmiSetupData->CpuOptions->GlobalCStateControl       = NvDataAmdSetup.CbsCmnCpuGlobalCstateCtrl;
  
  //<YinQiang001-20171120 Change C-state Auto value 2 +>
  if ( 3 == NvDataAmdSetup.CbsCmnCpuGlobalCstateCtrl )
    pIpmiSetupData->CpuOptions->GlobalCStateControl = 2; 
  //<YinQiang001-20171120 Change C-state Auto value 2 ->

  //-----------------------------------------------------------------
  // Type 1 -- Memory
  //-----------------------------------------------------------------
  pIpmiSetupData->MemoryOptions->NUMA                   = 1;
  pIpmiSetupData->MemoryOptions->BankInterleaving       = NvDataSetup.EnableBankIntlv;
  pIpmiSetupData->MemoryOptions->ChannelInterleaving    = NvDataSetup.EnableChannelIntlv;

  //-----------------------------------------------------------------
  // Type 2 -- Power
  //-----------------------------------------------------------------
  pIpmiSetupData->PowerOptions->AcPowerLoss             = NvDataServerMgmtSetup.AcPowerCtl;

  //-----------------------------------------------------------------
  // Type 3 -- Io
  //-----------------------------------------------------------------
  pIpmiSetupData->IoOptions->Iommu                      = NvDataAmdSetup.CbsCmnGnbNbIOMMU;
  switch (NvDataAmdSetup.CbsCmnFchSataClass) {
    // IPMI Value         BIOS SETUP Value          Sata Mode
    // 0                  2                         AHCI
    // 1                  5                         AHCI as ID 0x7904
    // 2                  1                         RAID
    // 3                  0xF                       Auto
    case 2:   pIpmiSetupData->IoOptions->SataMode = 0;  break;
    case 5:   pIpmiSetupData->IoOptions->SataMode = 1;  break;
    case 1:   pIpmiSetupData->IoOptions->SataMode = 2;  break;
    case 0xF: pIpmiSetupData->IoOptions->SataMode = 3;  break;
  }
#if (OemWriteCache_SUPPORT == 1)
  pIpmiSetupData->IoOptions->SataWriteCache              = NvDataSetup.VolatileWriteCache; //<Yaosr001-20200711 Send SataWriteCache to BMC by IPMI.>
#endif
  //-----------------------------------------------------------------
  // Type 4 -- Thermal
  //-----------------------------------------------------------------
  pIpmiSetupData->ThermalOptions->FanCtl_Mode           = NvDataServerMgmtSetup.FanCtl_Mode;


  //-----------------------------------------------------------------
  // Type 5 -- Management
  //-----------------------------------------------------------------
  pIpmiSetupData->ManagementOptions->SharelinkNET       = 0;  // Disabled

  pIpmiSetupData->ManagementOptions->ConsoleRedirection = NvDataSetup.ConsoleRedirectionEnable[0];
  pIpmiSetupData->ManagementOptions->TerminalType       = NvDataSetup.TerminalType[0];
  pIpmiSetupData->ManagementOptions->BaudRate           = NvDataSetup.BaudRate[0] - 3;
  pIpmiSetupData->ManagementOptions->FlowControl        = NvDataSetup.FlowControl[0];
  pIpmiSetupData->ManagementOptions->DataBits           = NvDataSetup.DataBits[0] - 7;
  pIpmiSetupData->ManagementOptions->Parity             = NvDataSetup.Parity[0] - 1;
#if defined(BMC_TIME_SYNC_MODE_SUPPORT) && (BMC_TIME_SYNC_MODE_SUPPORT == 1)
  pIpmiSetupData->ManagementOptions->BMCTimeSyncMode    = NvDataServerMgmtSetup.BMCTimeSyncMode; //<Yaosr001-20190730  Add "BMC Time Sync Mode" options.>
#endif

  switch (NvDataSetup.StopBits[0]) {
    // IPMI Value         BIOS SETUP Value          Stop bits
    // 0                  1                         1
    // 1                  3                         2
    case 1:  pIpmiSetupData->ManagementOptions->StopBits = 0;  break;
    case 3:  pIpmiSetupData->ManagementOptions->StopBits = 1;  break;
  }

  //-----------------------------------------------------------------
  // Type 6 -- Boot
  //-----------------------------------------------------------------
  pIpmiSetupData->BootOptions->BootOptionFilter         = NvDataSetup.BootOptionFilter;
  //Mizl-20210813- Temp Remove PXE Sync Feature +>
  #if 0
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
  pIpmiSetupData->BootOptions->OpROMENPxe               = OemOpROMEN[PXEINDX];
  pIpmiSetupData->BootOptions->LegacyPxeRetryCount      = NvDataSetup.ConfigPXEBootRetry;
  if (NvDataSetup.ConfigPXEBootRetry == 255) {
    pIpmiSetupData->BootOptions->LegacyPxeRetryCount = 3;
  }
  pIpmiSetupData->BootOptions->UefiPxeRetryCount        = NVDataNetworkStack.UefiPxeRetryCount;
  switch (NVDataNetworkStack.UefiPxeRetryCount) {
    // IPMI Value         BIOS SETUP Value
    // 0                  1
    // 1                  2
    // 2                  255
    case 1:  pIpmiSetupData->BootOptions->UefiPxeRetryCount = 0;  break;
    case 2:  pIpmiSetupData->BootOptions->UefiPxeRetryCount = 1;  break;
    case 255:  pIpmiSetupData->BootOptions->UefiPxeRetryCount = 2;  break;
  }
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. ->
  #endif
  //Mizl-20210813- Temp Remove PXE Sync Feature ->
  
  //<Chendl001-20170120 Network PXE Function modified by OOB +>
  //-----------------------------------------------------------------
  // Type 7 --PXE
  //-----------------------------------------------------------------
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
  if (PXE_Variable_Update == 1){
    PXE_Variable_Update = 0; // Clean the flags
    for (PXEindex = 0; PXEindex < MaxPxeDevNum; PXEindex ++){ //<Chendl001-20181223 Optimized PXE Select Module Function>
      if (PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel == 0x04){
        pIpmiSetupData->PxeOptions->PXE[PXEindex] = 0x03;
      } else {
        pIpmiSetupData->PxeOptions->PXE[PXEindex] = PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel;
      }
    }
  }
#endif
  //<Chendl001-20170120 Network PXE Function modified by OOB ->
    
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
  //-----------------------------------------------------------------
  // Type 9 --SRIOV
  //-----------------------------------------------------------------
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1) 
  if (Nic_Variable_Update == 1){
      Nic_Variable_Update = 0;
      for (NicIndex =0; NicIndex < NicVarData.NicNum; NicIndex ++){
          NicVarData.NICInfo[NicIndex].NicSriovEn = OemSriovEN[NicVarData.NICInfo[NicIndex].VarIndex];
          pIpmiSetupData->BiosNicSriovOptions->NicSriovEn[NicIndex] = NicVarData.NICInfo[NicIndex].NicSriovEn;
          DEBUG((EFI_D_INFO, "[CMCC-NFV]pIpmiSetupData->BiosNicSriovOptions->NicSriovEn[NicIndex]\n",pIpmiSetupData->BiosNicSriovOptions->NicSriovEn[NicIndex]));
      }
      Status = gRT->SetVariable(L"NicCtrlVar",&gAmiTseOemPortingVar2Guid, Attributes, NicVarSize, &NicVarData);
      if (EFI_ERROR(Status)) {
          DEBUG((EFI_D_INFO, "[CMCC-NFV] SetVariable NicCtrlVar Fail!!!\n"));
          return Status;
      }
  }
#endif
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  
  //-----------------------------------------------------------------
  // Type 13 -- Misc Options
  //-----------------------------------------------------------------
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
  pIpmiSetupData->MiscOptions->BiosHotKeySupport         = NvDataSetup.BiosHotKeySupport;  //<Yaosr001-20190731 Support BIOS HotKey select.>
#endif
  return EFI_SUCCESS;  
}



/**
  UpdateIpmiDataToNvData :

  Update Ipmi Data to Setup Variables  

  @param[in]           OEM_IPMI_SETUP_DATA pIpmiSetupData  
  @param[out]          UINT8                 UpdatedSetupFlag

  @return EFI_STATUS Status 
**/
EFI_STATUS UpdateIpmiDataToNvData(
  IN  OEM_IPMI_SETUP_DATA  *pIpmiSetupData,
  OUT UINT8                  *UpdatedSetupFlag
  )
{
  EFI_STATUS                      Status;
  SETUP_DATA                      NvDataSetup;
  SERVER_MGMT_CONFIGURATION_DATA  NvDataServerMgmtSetup;
  CBS_CONFIG                      NvDataAmdSetup;
  UINTN                           VariableSize;
  UINT32                          VariableAttribute;
  UINT8                           UpdatedFlag;
  //Mizl-20210813- Temp Remove PXE Sync Feature +>
  #if 0
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
  UINTN                           VarSizeNetwork;
  UINT32                          AttributesNetworkstack;
  NETWORK_STACK                   NVDataNetworkStack;
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI.->
  #endif
  //Mizl-20210813- Temp Remove PXE Sync Feature ->

  //<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
  PXE_SELECT_SETUP_DATA           PxeVariableData;
  UINT32                          AttributesPxe;
  UINTN                           VariableSizePxe;  
  UINT8                           PXEindex;   
  UINT8                           PxeUpdatedFlag = 0;
  UINT8                           Temp_PXEData = 0;
#endif
  //<Chendl001-20170120 Network PXE Function modified by OOB ->
    
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1)
  UINTN                           NicVarSize;
  NIC_CTRL_SETUP_DATA             NicVarData;
  UINT32                          AttributesNic;
  UINT8                           SriovUpdatedFlag = 0;
  UINT8                           NicIndex;
  UINT8                           Temp_SriovData = 0;
#endif
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  
  //<Yuanshai-20190710 For :Move PciDevEN OpROMEN SriovEN from "Setup" to "OEMPCIE_DEV" --BEGIN>
  #if defined(OemPCIeDev_SUPPORT) && (OemPCIeDev_SUPPORT == 1) 
  OEM_PCIE_DEV_DATA_PROTOCOL       *OemPcieDevProtocol = NULL;
  UINT8                              OemOpROMEN[PCIDEV_ENTRY_NUMBER];
  
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
  UINT8                              OemSriovEN[MAX_DEV_COUNT];
#endif
  
  Status = gBS->LocateProtocol(&gOemPcieDevProtocolGuid, NULL, (VOID**)&OemPcieDevProtocol);
  if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[OemDbg]LocateProtocol gOemPcieDevProtocolGuid %r\n",Status));
      return Status;
  }
   
  Status = OemPcieDevProtocol->GetSetPcieDevPolicy(OemOpROMEN,OPROMEN,TRUE);
  if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[OemDbg]OemPcieDevProtocol->GetSetPcieDevPolicy OemOpROMEN %r\n",Status));
      return Status;
  }
  
#if defined(OEM_SRIOV_ENABLE_BY_PORT_SUPPORT) && (OEM_SRIOV_ENABLE_BY_PORT_SUPPORT == 1)
  Status = OemPcieDevProtocol->GetSetPcieDevPolicy(OemSriovEN,SRIOVEN,TRUE);
  if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[OemDbg]OemPcieDevProtocol->GetSetPcieDevPolicy OemSriovEN %r\n",Status));
      return Status;
  }
#endif
  
  #endif 
  //<Yuanshai-20190710 For :Move PciDevEN OpROMEN SriovEN from "Setup" to "OEMPCIE_DEV" --END>
  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - UpdateIpmiDataToNvData\n"));

  if (pIpmiSetupData == NULL || UpdatedSetupFlag == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //-----------------------------------------------------------------
  // Update Nvram -- Setup Potion
  //-----------------------------------------------------------------
  VariableSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
             L"Setup",
             &gEfiSetupVariableGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataSetup
             );
  if (!EFI_ERROR(Status)) {
    UpdatedFlag = 0; 
    // Type 0
    if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupCpuOptions]) {
//<huangjin001-20191018 Fix build error for svm mode +>      
//     OPTION_PUT(NvDataSetup.SVM_MODE,           pIpmiSetupData->CpuOptions->SvmMode,                UpdatedFlag);
//<huangjin001-20191018 Fix build error for svm mode ->           
      OPTION_PUT(NvDataSetup.SriovSupport,       pIpmiSetupData->CpuOptions->SriovSupport,           UpdatedFlag);//<YinQiang001-20171102 Add SR-IOV Support Ipmi Command>
    }

    // Type 1
    if(pIpmiSetupData->ValidIpmiData[OemIpmiSetupMemoryOptions]) {
      //NvDataSetup.NUMA = 1;  // NUMA always Enabled
      OPTION_PUT(NvDataSetup.EnableBankIntlv,    pIpmiSetupData->MemoryOptions->BankInterleaving,    UpdatedFlag);
      OPTION_PUT(NvDataSetup.EnableChannelIntlv, pIpmiSetupData->MemoryOptions->ChannelInterleaving, UpdatedFlag);
    }

    //Type 3
#if (OemWriteCache_SUPPORT == 1)
    OPTION_PUT(NvDataSetup.VolatileWriteCache, pIpmiSetupData->IoOptions->SataWriteCache, UpdatedFlag); //<Yaosr001-20200711 Send SataWriteCache to BMC by IPMI.>
#endif

    // Type 5
    if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupManagementOptions]) {
      OPTION_PUT(NvDataSetup.ConsoleRedirectionEnable[0], pIpmiSetupData->ManagementOptions->ConsoleRedirection, UpdatedFlag);
      OPTION_PUT(NvDataSetup.TerminalType[0],    pIpmiSetupData->ManagementOptions->TerminalType,    UpdatedFlag);
      OPTION_PUT(NvDataSetup.BaudRate[0],        pIpmiSetupData->ManagementOptions->BaudRate + 3,    UpdatedFlag);
      OPTION_PUT(NvDataSetup.FlowControl[0],     pIpmiSetupData->ManagementOptions->FlowControl,     UpdatedFlag);
      OPTION_PUT(NvDataSetup.DataBits[0],        pIpmiSetupData->ManagementOptions->DataBits + 7,    UpdatedFlag);
      OPTION_PUT(NvDataSetup.Parity[0],          pIpmiSetupData->ManagementOptions->Parity + 1,      UpdatedFlag);

      if (OPTION_VALIDATE(pIpmiSetupData->ManagementOptions->StopBits)) {
        OPTION_PUT(NvDataSetup.StopBits[0], pIpmiSetupData->ManagementOptions->StopBits, UpdatedFlag);
        switch (NvDataSetup.StopBits[0]) {
          // IPMI Value         BIOS SETUP Value          Stop bits
          // 0                  1                         1
          // 1                  3                         2
          case 0:  NvDataSetup.StopBits[0] = 1;  break;
          case 1:  NvDataSetup.StopBits[0] = 3;  break;
        }
      }
    }

    // Type 6
    if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupBootOptions]) {
      OPTION_PUT(NvDataSetup.BootOptionFilter,   pIpmiSetupData->BootOptions->BootOptionFilter,      UpdatedFlag);  
      if (NvDataSetup.BootOptionFilter == 0){ // Boot Option Filter set to "Uefi and Legacy" 
      #if defined(FIXED_BOOT_ORDER_SUPPORT) && (FIXED_BOOT_ORDER_SUPPORT == 1)
                NvDataSetup.BootMode = 2;
      #endif
                OemOpROMEN[PXEINDX] = 2;
                OemOpROMEN[MASSINDX] = 2;
                OemOpROMEN[VIDEOINDX] = 2;
                OemOpROMEN[OLDINDX] = 1;
            }
            if (NvDataSetup.BootOptionFilter == 1){ // Boot Option Filter set to "Legacy" 
      #if defined(FIXED_BOOT_ORDER_SUPPORT) && (FIXED_BOOT_ORDER_SUPPORT == 1)
                NvDataSetup.BootMode = 0;
      #endif
                OemOpROMEN[PXEINDX] = 2;
                OemOpROMEN[MASSINDX] = 2;
                OemOpROMEN[VIDEOINDX] = 2;
                OemOpROMEN[OLDINDX] = 2;
            }
            if (NvDataSetup.BootOptionFilter == 2){ // Boot Option Filter set to "Uefi" 
      #if defined(FIXED_BOOT_ORDER_SUPPORT) && (FIXED_BOOT_ORDER_SUPPORT == 1)
                NvDataSetup.BootMode = 1;
      #endif
                OemOpROMEN[PXEINDX] = 1;
                OemOpROMEN[MASSINDX] = 1;
                OemOpROMEN[VIDEOINDX] = 1;
                OemOpROMEN[OLDINDX] = 1;
            }  
      //Mizl-20210813- Temp Remove PXE Sync Feature +>
      #if 0
      //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
      OPTION_PUT(OemOpROMEN[PXEINDX],   pIpmiSetupData->BootOptions->OpROMENPxe,            UpdatedFlag);
      OPTION_PUT(NvDataSetup.ConfigPXEBootRetry, pIpmiSetupData->BootOptions->LegacyPxeRetryCount,   UpdatedFlag);
      if (NvDataSetup.ConfigPXEBootRetry == 3) {
        NvDataSetup.ConfigPXEBootRetry = 255;
      }
      //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. ->
      #endif
      //Mizl-20210813- Temp Remove PXE Sync Feature ->
    }
    
    //<Chendl001-20170120 Network PXE Function modified by OOB +>
    // Type 7
    #if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
    VariableSizePxe = sizeof(PXE_SELECT_SETUP_DATA);
    Status = gRT->GetVariable(
                L"PxeSelVar",
                &gAmiTseOemPortingVar2Guid, 
                &AttributesPxe, 
                &VariableSizePxe, 
                &PxeVariableData
                );
    DEBUG((EFI_D_INFO, "[CMCC-NFV]: Get PxeSelVar Status = %r ...\n", Status));
    if (!EFI_ERROR(Status)) {
      if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupPxeOptions]) {
        for(PXEindex = 0; PXEindex < PxeVariableData.NetDevNum; PXEindex ++){
          if ((pIpmiSetupData->PxeOptions->PXE[PXEindex]) & (1 << 7)) { //Check modify flag is set or not ?    
            //DEBUG((EFI_D_INFO, "[CMCC-NFV]: pIpmiSetupData->PxeOptions->PXE[index] = %x\n",pIpmiSetupData->PxeOptions->PXE[index]));
            Temp_PXEData = pIpmiSetupData->PxeOptions->PXE[PXEindex] & 0x7F; // Get BMC Data
            if ((Temp_PXEData >= 0) && (Temp_PXEData <= 2)){  
              if (PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel != Temp_PXEData){ // Whether we need reboot or not ?
                PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel = Temp_PXEData;
                //<Chendl001-20181211 Network PXE enabled/disabled by OOB, Setup need sync with PxeSelVar +>
                OemOpROMEN[PxeVariableData.NetWorkPostInfo[PXEindex].VarIndex] = PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel;
                DEBUG((EFI_D_INFO, "[CMCC-NFV]: OemOpROMEN[%d] is %x...\n", PxeVariableData.NetWorkPostInfo[PXEindex].VarIndex,OemOpROMEN[PxeVariableData.NetWorkPostInfo[PXEindex].VarIndex]));
                //<Chendl001-20181211 Network PXE enabled/disabled by OOB, Setup need sync with PxeSelVar ->
                PxeUpdatedFlag = 1;     
              }   
            } else {
              if ((Temp_PXEData == 0x03) && (PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel != 0x04)){
                PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel = 0x04;
                //<Chendl001-20181211 Network PXE enabled/disabled by OOB, Setup need sync with PxeSelVar +>
                OemOpROMEN[PxeVariableData.NetWorkPostInfo[PXEindex].VarIndex] = PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel;
                //<Chendl001-20181211 Network PXE enabled/disabled by OOB, Setup need sync with PxeSelVar ->
                PxeUpdatedFlag = 1;
              }
            }
          }     
              
          if(PxeUpdatedFlag == 1){
            UpdatedFlag = 1;
            DEBUG((EFI_D_INFO, "[CMCC-NFV]: PXE Variable Changed --- Pxesel[%d]= %x\n\n", PXEindex, PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel));
          } else {
            if (pIpmiSetupData->PxeOptions->PXE[PXEindex] == 4){
              pIpmiSetupData->PxeOptions->PXE[PXEindex] = 3; //<Tanzhk002-20170108.sync auto(4) to bmc(3) +>
            } 
          } 
        }
      }
          
      //Was PxeSelVar modifiied by Sce tool ?
      for(PXEindex = 0; PXEindex < 16; PXEindex ++){
        if(PxeVariableData.NetWorkPostInfo[PXEindex].NeedSet == 1){
          OemOpROMEN[PxeVariableData.NetWorkPostInfo[PXEindex].VarIndex] = PxeVariableData.NetWorkPostInfo[PXEindex].PxeSel;
          PxeVariableData.NetWorkPostInfo[PXEindex].NeedSet = 0;
          UpdatedFlag = 1;
          PxeUpdatedFlag = 0; // If PxeSelVar changed by SEC tool, we don't need update PxeSelVar
        }
      }
    }
#endif
    //<Chendl001-20181223 Optimized PXE Select Module Function ->
    
    //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
    // Type 9
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1) 
    NicVarSize = sizeof(NIC_CTRL_SETUP_DATA);
    Status = gRT->GetVariable(
                    L"NicCtrlVar",
                    &gAmiTseOemPortingVar2Guid, 
                    &AttributesNic, 
                    &NicVarSize, 
                    &NicVarData
                    );
    DEBUG((EFI_D_INFO, "[CMCC-NFV]: Get NicCtrlVar Status = %r ...\n", Status));
    DEBUG((EFI_D_INFO, "[CMCC-NFV]: NicVarData.NicNum = %d\n", NicVarData.NicNum));
    if (!EFI_ERROR(Status)) {
        if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupNicSriovOptions]){
            for(NicIndex = 0; NicIndex < NicVarData.NicNum; NicIndex ++){
                if ((pIpmiSetupData->BiosNicSriovOptions->NicSriovEn[NicIndex]) & (1 << 7)){
                    Temp_SriovData = pIpmiSetupData->BiosNicSriovOptions->NicSriovEn[NicIndex] & 0x7F; // Get BMC Data
                    NicVarData.NICInfo[NicIndex].NicSriovEn = Temp_SriovData;
                    OemSriovEN[NicVarData.NICInfo[NicIndex].VarIndex] = NicVarData.NICInfo[NicIndex].NicSriovEn;
                    DEBUG((EFI_D_INFO, "[CMCC-NFV]: Temp_SriovData is %d,NicIndex is %d,VarIndex is %d\n", Temp_SriovData,NicIndex,NicVarData.NICInfo[NicIndex].VarIndex));
                    SriovUpdatedFlag =1;
                    UpdatedFlag = 1;
                }
            }
        }
    }
#endif
    //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
    
    // Type 13
    //<Yaosr001-20190731 Support BIOS HotKey select. +>
    if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupMiscOptions]) {
#if defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
      OPTION_PUT(NvDataSetup.BiosHotKeySupport,  pIpmiSetupData->MiscOptions->BiosHotKeySupport,    UpdatedFlag); 
#endif
    }
    //<Yaosr001-20190731 Support BIOS HotKey select. ->

    DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - UpdateIpmiDataToNvData: NvData Changed=%d\n", UpdatedFlag));

    if (UpdatedFlag) {
      gRT->SetVariable(
             L"Setup",
             &gEfiSetupVariableGuid,
             VariableAttribute,
             sizeof(SETUP_DATA),
             &NvDataSetup
             );
    Status = OemPcieDevProtocol->GetSetPcieDevPolicy(OemOpROMEN,OPROMEN,FALSE);
    if(!EFI_ERROR(Status)) {
       DEBUG((EFI_D_INFO, "[CMCC-NFV]: Set OemOpROMEN Variable Success ...\n"));
    } else {
       DEBUG((EFI_D_INFO, "[CMCC-NFV]: Set OemOpROMEN Variable Failed ...\n"));
    }
    
    //<Chendl001-20181223 Optimized PXE Select Module Function +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
    VariableSizePxe = sizeof(PXE_SELECT_SETUP_DATA);
    AttributesPxe=(EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE); 
    Status = gRT->SetVariable(
                  L"PxeSelVar",
                  &gAmiTseOemPortingVar2Guid,
                  AttributesPxe, 
                  VariableSizePxe,
                  &PxeVariableData
                  );
    if(!EFI_ERROR(Status)) {
      DEBUG((EFI_D_INFO, "[CMCC-NFV]: Update PxeSelVar Variable Success ...\n"));
    } else {
      DEBUG((EFI_D_INFO, "[CMCC-NFV]: Update PxeSelVar Variable Failed ...\n"));
    }
#endif
    //<Chendl001-20181223 Optimized PXE Select Module Function ->
    
    //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION == 1) 
    if (SriovUpdatedFlag ==1){
        
        Status = OemPcieDevProtocol->GetSetPcieDevPolicy(OemSriovEN,SRIOVEN,FALSE);
        if(!EFI_ERROR(Status)) {
           DEBUG((EFI_D_INFO, "[CMCC-NFV]: Set OemSriovEN Variable Success ...\n"));
        } else {
           DEBUG((EFI_D_INFO, "[CMCC-NFV]: Set OemSriovEN Variable Failed ...\n"));
        }
        
        SriovUpdatedFlag = 0;
        NicVarSize = sizeof(NIC_CTRL_SETUP_DATA);
        AttributesNic=(EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE); 
        Status = gRT->SetVariable(
                        L"NicCtrlVar",
                        &gAmiTseOemPortingVar2Guid,
                        AttributesNic, 
                        NicVarSize,
                        &NicVarData
                        );
        if(!EFI_ERROR(Status)) {
            DEBUG((EFI_D_INFO, "[CMCC-NFV]: Update NicVarData Variable Success ...\n"));
        } else {
            DEBUG((EFI_D_INFO, "[CMCC-NFV]: Update NicVarData Variable Failed ...\n"));
        }
    }
#endif
    //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
    
    *UpdatedSetupFlag |= (OEM_UPDATE_SETUP | OEM_NEED_COLD_RESET);
    }
  }


  //-----------------------------------------------------------------
  // Update Nvram -- Server Management Potion
  //-----------------------------------------------------------------
  VariableSize = sizeof(SERVER_MGMT_CONFIGURATION_DATA);
  Status = gRT->GetVariable (
             L"ServerSetup",
             &gEfiServerMgmtSetupVariableGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataServerMgmtSetup
             );
  if (!EFI_ERROR(Status)) {
      UpdatedFlag = 0;

    // Type 2
    if(pIpmiSetupData->ValidIpmiData[OemIpmiSetupPowerOptions]) {
      OPTION_PUT(NvDataServerMgmtSetup.AcPowerCtl,   pIpmiSetupData->PowerOptions->AcPowerLoss,       UpdatedFlag);
    }

    // Type 4
    if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupThermalOptions]) {
      OPTION_PUT(NvDataServerMgmtSetup.FanCtl_Mode,  pIpmiSetupData->ThermalOptions->FanCtl_Mode,     UpdatedFlag);
    }

    // Type 5
    //<Yaosr001-20190730  Add "BMC Time Sync Mode" options. +>
#if defined(BMC_TIME_SYNC_MODE_SUPPORT) && (BMC_TIME_SYNC_MODE_SUPPORT == 1)
    if (pIpmiSetupData->ValidIpmiData[OemIpmiSetupManagementOptions]) {
      OPTION_PUT(NvDataServerMgmtSetup.BMCTimeSyncMode, pIpmiSetupData->ManagementOptions->BMCTimeSyncMode, UpdatedFlag);
    }
#endif
    //<Yaosr001-20190730  Add "BMC Time Sync Mode" options. ->

    DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - UpdateIpmiDataToNvData: MgmtData Changed=%d\n", UpdatedFlag));

    if (UpdatedFlag) {
      gRT->SetVariable(
             L"ServerSetup",
             &gEfiServerMgmtSetupVariableGuid,
             VariableAttribute,
             sizeof(SERVER_MGMT_CONFIGURATION_DATA),
             &NvDataServerMgmtSetup
             );
      *UpdatedSetupFlag |= (OEM_UPDATE_SETUP | OEM_NEED_COLD_RESET);
    }
  }


  //-----------------------------------------------------------------
  // Update Nvram -- Amd Option
  //-----------------------------------------------------------------
  VariableSize = sizeof(CBS_CONFIG);
  Status = gRT->GetVariable (
             L"AmdSetup",
             &gCbsSystemConfigurationGuid,
             &VariableAttribute,
             &VariableSize,
             &NvDataAmdSetup
             );
  if (!EFI_ERROR(Status)) {
    UpdatedFlag = 0;

    // Type 0
    if(pIpmiSetupData->ValidIpmiData[OemIpmiSetupCpuOptions]) {
        OPTION_PUT(NvDataAmdSetup.CbsCpuSmtCtrl,            pIpmiSetupData->CpuOptions->SmtMode,                UpdatedFlag);  
//<huangjin001-20191018 Fix build error for svm mode +>      
      OPTION_PUT(NvDataAmdSetup.CbsCmnSVMCtrl,             pIpmiSetupData->CpuOptions->SvmMode,                UpdatedFlag);
//<huangjin001-20191018 Fix build error for svm mode ->  
      OPTION_PUT(NvDataAmdSetup.CbsCmnCpuCpb,              pIpmiSetupData->CpuOptions->CorePerfBoost,       UpdatedFlag);
      OPTION_PUT(NvDataAmdSetup.CbsCmnCpuGlobalCstateCtrl, pIpmiSetupData->CpuOptions->GlobalCStateControl, UpdatedFlag);
      
      //<YinQiang001-20171120 Change C-state Auto value 2 +>
      if ( 2 == NvDataAmdSetup.CbsCmnCpuGlobalCstateCtrl )
          NvDataAmdSetup.CbsCmnCpuGlobalCstateCtrl = 3;
      //<YinQiang001-20171120 Change C-state Auto value 2 ->
    }

    // Type 3
    if(pIpmiSetupData->ValidIpmiData[OemIpmiSetupIoOptions]) {
      OPTION_PUT(NvDataAmdSetup.CbsCmnGnbNbIOMMU,          pIpmiSetupData->IoOptions->Iommu,                UpdatedFlag);
      OPTION_PUT(NvDataAmdSetup.CbsCmnFchSataClass,        pIpmiSetupData->IoOptions->SataMode,             UpdatedFlag); 
      //<Kangmm001-20200319 Fix mantis 0037566 SATA mode changed follow by IOMMU setting +>
      if(( pIpmiSetupData->IoOptions->SataMode ) & ( 1 << 7 ) )
      {
          switch(NvDataAmdSetup.CbsCmnFchSataClass) {
            // IPMI Value         BIOS SETUP Value          Sata Mode
            // 0                  2                         AHCI
            // 1                  5                         AHCI as ID 0x7904
            // 2                  1                         RAID
            // 3                  0xF                       Auto
            case 0:   NvDataAmdSetup.CbsCmnFchSataClass = 2;    break;
            case 1:   NvDataAmdSetup.CbsCmnFchSataClass = 5;    break;
            case 2:   NvDataAmdSetup.CbsCmnFchSataClass = 1;    break;
            case 3:   NvDataAmdSetup.CbsCmnFchSataClass = 0xF;  break;
            default:  NvDataAmdSetup.CbsCmnFchSataClass = 0xF;  break;
          }
      }
      //<Kangmm001-20200319 Fix mantis 0037566 SATA mode changed follow by IOMMU setting ->
    }

    DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - UpdateIpmiDataToNvData: CbsData Changed=%d\n", UpdatedFlag));

    if (UpdatedFlag) {
      gRT->SetVariable(
             L"AmdSetup",
             &gCbsSystemConfigurationGuid,
             VariableAttribute,
             sizeof(CBS_CONFIG),
             &NvDataAmdSetup
             );
      *UpdatedSetupFlag |= (OEM_UPDATE_SETUP | OEM_NEED_COLD_RESET);
    }
    
    //Mizl-20210813- Temp Remove PXE Sync Feature +>
    #if 0
    //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
    //-----------------------------------------------------------------
    // Update Nvram -- NetworkStack option
    //-----------------------------------------------------------------
    VarSizeNetwork = sizeof (NETWORK_STACK);
    Status= gRT->GetVariable(L"NetworkStackVar",
                       &gEfiNetworkStackSetupGuid,
                       &AttributesNetworkstack,
                       &VarSizeNetwork, 
                       &NVDataNetworkStack);
    if (!EFI_ERROR(Status)) {
      UpdatedFlag = 0;
      //TYPE 6
      if(pIpmiSetupData->ValidIpmiData[OemIpmiSetupBootOptions]) {
        OPTION_PUT(NVDataNetworkStack.UefiPxeRetryCount,     pIpmiSetupData->BootOptions->UefiPxeRetryCount,     UpdatedFlag);
        switch (NVDataNetworkStack.UefiPxeRetryCount) {
          // IPMI Value         BIOS SETUP Value
          // 0                  1
          // 1                  2
          // 2                  255
          case 0:  NVDataNetworkStack.UefiPxeRetryCount = 1;  break;
          case 1:  NVDataNetworkStack.UefiPxeRetryCount = 2;  break;
          case 2:  NVDataNetworkStack.UefiPxeRetryCount = 255;  break;
        }
      }
      DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - UpdateIpmiDataToNvData: NetworkStackData Changed=%d\n", UpdatedFlag));

      if (UpdatedFlag) {
        gRT->SetVariable(
                    L"NetworkStackVar",
                    &gEfiNetworkStackSetupGuid,
                    AttributesNetworkstack,
                    sizeof(NETWORK_STACK),
                    &NVDataNetworkStack);
        *UpdatedSetupFlag |= (OEM_UPDATE_SETUP | OEM_NEED_COLD_RESET);
      }
    }
    //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. ->
    #endif
    //Mizl-20210813- Temp Remove PXE Sync Feature ->
  }

  return Status;
}



/**
  SyncIpmiDataAndNvData :

  Update BIOS Setup Data with BMC modify data

  @param[in]           EFI_EVENT              Event
  @param[in]           VOID                   *Context

 @return EFI_STATUS Status 
**/
EFI_STATUS
EFIAPI
SyncIpmiDataAndNvData (
  IN EFI_EVENT              Event,
  IN VOID                   *Context
  )
{
  EFI_STATUS            Status;
  OEM_IPMI_SETUP_DATA IpmiSetupData;
  UINT8                 IpmiBuf[OemIpmiSetupMaxOptions][OEM_IPMI_SETUP_REQUEST_LENGTH] = {0};
  UINT8                 TempBuf[OEM_IPMI_SETUP_RESPONSE_LENGTH];  // Response Buffer
  UINT8                 RequSize;
  UINT8                 RespSize;
  UINT8                 Index;
  UINT8                 Retry;
  UINT8                 UpdatedSetupFlag;

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData\n"));

  //
  // Locate IPMI Transport Protocol
  //
  if (mIpmiTransportProtocol == NULL) {
    Status = gBS->LocateProtocol (
               &gEfiDxeIpmiTransportProtocolGuid,
               NULL,
               &mIpmiTransportProtocol
               );
    if (EFI_ERROR(Status) || (mIpmiTransportProtocol == NULL)) {
      return EFI_NOT_FOUND;
    }
  }

  //
  // Init Configuration Struct Data Buffer
  //
  MemSet(&IpmiSetupData, sizeof(IpmiSetupData), 0);
  IpmiSetupData.CpuOptions        = (OEM_IPMI_SETUP_CPU_OPTIONS        *)&IpmiBuf[OemIpmiSetupCpuOptions][2];
  IpmiSetupData.MemoryOptions     = (OEM_IPMI_SETUP_MEMORY_OPTIONS     *)&IpmiBuf[OemIpmiSetupMemoryOptions][2];
  IpmiSetupData.PowerOptions      = (OEM_IPMI_SETUP_POWER_OPTIONS      *)&IpmiBuf[OemIpmiSetupPowerOptions][2];
  IpmiSetupData.IoOptions         = (OEM_IPMI_SETUP_IO_OPTIONS         *)&IpmiBuf[OemIpmiSetupIoOptions][2];
  IpmiSetupData.ThermalOptions    = (OEM_IPMI_SETUP_THERMAL_OPTIONS    *)&IpmiBuf[OemIpmiSetupThermalOptions][2];
  IpmiSetupData.ManagementOptions = (OEM_IPMI_SETUP_MANAGEMENT_OPTIONS *)&IpmiBuf[OemIpmiSetupManagementOptions][2];
  IpmiSetupData.BootOptions       = (OEM_IPMI_SETUP_BOOT_OPTIONS       *)&IpmiBuf[OemIpmiSetupBootOptions][2];

  //<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1)
  IpmiSetupData.PxeOptions        = (OEM_IPMI_PXE_OPTIONS              *)&IpmiBuf[OemIpmiSetupPxeOptions][2];
#endif
  //<Chendl001-20170120 Network PXE Function modified by OOB ->
    
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION==1)
  IpmiSetupData.BiosNicSriovOptions  = (OEM_IPMI_NIC_SRIOV_OPTIONS     *)&IpmiBuf[OemIpmiSetupNicSriovOptions][2];
#endif
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  
  IpmiSetupData.MiscOptions       = (OEM_IPMI_SETUP_MISC_OPTIONS       *)&IpmiBuf[OemIpmiSetupMiscOptions][2];//<Yaosr001-20190731 add MISC option.>
  //
  // Get BMC Modified Data
  //
  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData: GetBmcData\n"));

  for (Index = 0; Index < OemIpmiSetupMaxOptions; Index++) {
    //<Yaosr001-20190731 Do not send Index 7/8/9 info to bmc. +>
    if (Index >= MIN_Index_NOT_SENT_TO_BMC && Index <= MAX_Index_NOT_SENT_TO_BMC) {
      continue;
    }
    //<Yaosr001-20190731 Do not send Index 7/8/9 info to bmc. ->
    
    TempBuf[0] = Index;
    TempBuf[1] = OemIpmiSetupDataModify; //<Machong-20161229 get MOD_DATA from BMC>
    RequSize   = 2;
    RespSize   = OEM_IPMI_SETUP_RESPONSE_LENGTH;

    Status = mIpmiTransportProtocol->SendIpmiCommand(
               mIpmiTransportProtocol,
               OEM_IPMI_NET_FUN,
               BMC_LUN,
               OEM_IPMI_GET_BIOS_OPTIONS,
               TempBuf,
               RequSize,
               &IpmiBuf[Index][1],
               &RespSize
               );
    IpmiBuf[Index][0] = mIpmiTransportProtocol->CommandCompletionCode;

    if (!EFI_ERROR(Status) && (IpmiBuf[Index][0] == 0 /*CompletionCode*/) && IpmiBuf[Index][1] == 1/*Flag*/) {
      IpmiSetupData.ValidIpmiData[Index] = 1;
    }
  }

  //
  // Update IpmiDate to NvData
  //
  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData: Update Nvram\n"));

  UpdatedSetupFlag = 0;

  UpdateStandardIpmitoSetup(mIpmiTransportProtocol,&UpdatedSetupFlag); //<lvych001-20170830 Add "Fan Control Mode" and "AC Power Loss Policy" options sync with standard Ipmi command setting.>
  Status = UpdateIpmiDataToNvData(&IpmiSetupData, &UpdatedSetupFlag);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Last, refresh all NvData again and sync back to BMC
  //
  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData: Get Current Options\n"));

  Status = UpdateSetuptoStandardIpmi(mIpmiTransportProtocol);//<lvych001-20170830 Add "Fan Control Mode" and "AC Power Loss Policy" options sync with standard Ipmi command setting.>
  Status = GetNvData(&IpmiSetupData);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData: Send back to BMC\n"));

  for (Index = 0; Index < OemIpmiSetupMaxOptions; Index++) {
    
    //<Yaosr001-20190731 Do not send Index 7/8/9 info to bmc. +>
    if (Index >= MIN_Index_NOT_SENT_TO_BMC && Index <= MAX_Index_NOT_SENT_TO_BMC) {
      continue;
    }
    //<Yaosr001-20190731 Do not send Index 7/8/9 info to bmc. ->
    
    IpmiBuf[Index][0] = OemIpmiSetupSourceBios;  //Source
    IpmiBuf[Index][1] = Index;                     //Selector
    RequSize          = sizeof(OEM_IPMI_SETUP_CPU_OPTIONS) + 2;
    RespSize          = OEM_IPMI_SETUP_RESPONSE_LENGTH;

    Retry = OEM_IPMI_SETUP_NUM_OF_RETRY;
    while (Retry-- > 0) {
      Status = mIpmiTransportProtocol->SendIpmiCommand(
                 mIpmiTransportProtocol,
                 OEM_IPMI_NET_FUN,
                 BMC_LUN,
                 OEM_IPMI_SET_BIOS_OPTIONS,
                 IpmiBuf[Index],
                 RequSize,
                 TempBuf,
                 &RespSize
                 );
      if (EFI_SUCCESS == Status) {
        break;
      }
    }
  }

  //
  // Some NvData changing need reset system
  //
  if (UpdatedSetupFlag) {
    DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData: Reset System\n"));
    (UpdatedSetupFlag & OEM_NEED_COLD_RESET) ? gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL) : gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }

  return EFI_SUCCESS;
}

/**
  SavedConfigChangesHook :
 
  TSE Hook for SavedConfigChanges
 
  @param[in]           EFI_EVENT              Event
  @param[in]           VOID                   *Context
  
  @return EFI_STATUS Status
**/
EFI_STATUS
EFIAPI
SavedConfigChangesHook (
  IN EFI_EVENT              Event,
  IN VOID                   *Context
  )
{
  EFI_STATUS            Status;
  UINT8                 RequSize;
  UINT8                 RequBuf[OemIpmiSetupMaxOptions][OEM_IPMI_SETUP_REQUEST_LENGTH] = { 0 };
  UINT8                 RespSize;
  UINT8                 TempBuf[OEM_IPMI_SETUP_RESPONSE_LENGTH];  // Response Buffer
  UINT8                 Index;
  UINT8                 Retry;
  OEM_IPMI_SETUP_DATA IpmiSetupData;

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SavedConfigChangesHook\n"));

  //
  // Locate IPMI Transport Protocol
  //
  if (mIpmiTransportProtocol == NULL) {
    Status = gBS->LocateProtocol (
               &gEfiDxeIpmiTransportProtocolGuid,
               NULL,
               &mIpmiTransportProtocol
               );
    if (EFI_ERROR(Status) || (mIpmiTransportProtocol == NULL)) {
      return EFI_NOT_FOUND;
    }
  }

  //
  // Init NvData to IpmiData
  //
  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SavedConfigChangesHook: Get NvramData\n"));

  MemSet(&IpmiSetupData, sizeof(IpmiSetupData), 0);
  IpmiSetupData.CpuOptions        = (OEM_IPMI_SETUP_CPU_OPTIONS        *)&RequBuf[OemIpmiSetupCpuOptions][2];
  IpmiSetupData.MemoryOptions     = (OEM_IPMI_SETUP_MEMORY_OPTIONS     *)&RequBuf[OemIpmiSetupMemoryOptions][2];
  IpmiSetupData.PowerOptions      = (OEM_IPMI_SETUP_POWER_OPTIONS      *)&RequBuf[OemIpmiSetupPowerOptions][2];
  IpmiSetupData.IoOptions         = (OEM_IPMI_SETUP_IO_OPTIONS         *)&RequBuf[OemIpmiSetupIoOptions][2];
  IpmiSetupData.ThermalOptions    = (OEM_IPMI_SETUP_THERMAL_OPTIONS    *)&RequBuf[OemIpmiSetupThermalOptions][2];
  IpmiSetupData.ManagementOptions = (OEM_IPMI_SETUP_MANAGEMENT_OPTIONS *)&RequBuf[OemIpmiSetupManagementOptions][2];
  IpmiSetupData.BootOptions       = (OEM_IPMI_SETUP_BOOT_OPTIONS       *)&RequBuf[OemIpmiSetupBootOptions][2];

  //<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1)
  IpmiSetupData.PxeOptions        = (OEM_IPMI_PXE_OPTIONS              *)&RequBuf[OemIpmiSetupPxeOptions][2];
#endif
  //<Chendl001-20170120 Network PXE Function modified by OOB ->
    
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION==1)
  IpmiSetupData.BiosNicSriovOptions=(OEM_IPMI_NIC_SRIOV_OPTIONS        *)&RequBuf[OemIpmiSetupNicSriovOptions][2];
#endif
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  
  IpmiSetupData.MiscOptions       = (OEM_IPMI_SETUP_MISC_OPTIONS       *)&RequBuf[OemIpmiSetupMiscOptions][2];//<Yaosr001-20190731 add MISC option.>
  Status = GetNvData(&IpmiSetupData);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SavedConfigChangesHook: Send to BMC\n"));

  //
  // Send back to BMC
  //
  for (Index = 0; Index < OemIpmiSetupMaxOptions; Index++) {
    //<Yaosr001-20190731 Do not send Index 7/8/9 info to bmc. +>
    if (Index >= MIN_Index_NOT_SENT_TO_BMC && Index <= MAX_Index_NOT_SENT_TO_BMC) {
      continue;
    }
    //<Yaosr001-20190731 Do not send Index 7/8/9 info to bmc. ->
    
    RequBuf[Index][0] = OemIpmiSetupSourceBios;  //Source
    RequBuf[Index][1] = Index;                     //Selector
    RequSize          = sizeof(OEM_IPMI_SETUP_CPU_OPTIONS) + 2;
    RespSize          = OEM_IPMI_SETUP_RESPONSE_LENGTH;

    Retry = OEM_IPMI_SETUP_NUM_OF_RETRY;
    while (Retry-- > 0) {
      Status = mIpmiTransportProtocol->SendIpmiCommand (
                 mIpmiTransportProtocol,
                 OEM_IPMI_NET_FUN,
                 BMC_LUN,
                 OEM_IPMI_SET_BIOS_OPTIONS,
                 RequBuf[Index],                   //Reused the buffer We have consumed
                 RequSize,
                 TempBuf,
                 &RespSize
                 );
      if (EFI_SUCCESS == Status) {
        break;
      }
    }
  }
  
  return Status;
}

/**
  OemIpmiSetupEntryPoint :
 
  Entry point of OemIpmiSetup Module
 
  @param[in]           EFI_HANDLE             ImageHandle
  @param[in]           EFI_SYSTEM_TABLE       *SystemTable
 
  @return EFI_STATUS Status 
**/
EFI_STATUS
OemIpmiSetupEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    NewHandle = NULL;
  VOID          *NotifyRegistration;

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - OemIpmiSetup EntryPoint\n"));

  //
  // Register the event to Enable IPMI force boot.
  //
  EfiCreateProtocolNotifyEvent(
    &gEfiDxeIpmiTransportProtocolGuid,
    TPL_CALLBACK,
    SyncIpmiDataAndNvData,
    NULL,
    &NotifyRegistration
    );

  Status = EfiNamedEventListen(
    &gOemIpmiNvramUpdateGuid,
    TPL_CALLBACK,
    SavedConfigChangesHook,
    NULL,
    NULL
    );

  DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - OemIpmiSetup: Status=%r\n", Status));

  return  Status;
}
