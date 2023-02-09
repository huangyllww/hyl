/** @file
  
  Copyright (C) 2016 - 2017 Dawning Information Industry Co.,LTD.All rights reserved.
  
  Compatible with <OEM IPMI Command List V1.3>
  
  This file contains the definition of GUID and Data Structure of BIOS Setup Options(OEM)
  
**/

#ifndef __OEM_IPMI_SETUP_H__
#define __OEM_IPMI_SETUP_H__

#include <AMIHobs.h>
#include <AmiDxeLib.h>
#include <Library/MemoryAllocationLib.h>


#include <Library/UefiLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Setup.h>
#include "ServerMgmtSetupVariable.h"
#include "Library/Family/0x17/ZP/External/Resource/AmdCbsVariable.h"
#include <IpmiNetFnChassisDefinitions.h> //<lvych001-20170830 Add "Fan Control Mode" and "AC Power Loss Policy" options sync with standard Ipmi command setting.>
#include <NetworkStackSetup.h> //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI.>

//<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_CTRL_FUNCTION_SUPPORT) && (NIC_CTRL_FUNCTION_SUPPORT==1)
#include <NicCtrl.h>
#endif
//<Lvshh001-20190621 Network SRIOV Function modified by OOB ->

EFI_GUID gEfiSetupVariableGuid   = { 0xEC87D643, 0xEBA4, 0x4BB5, {0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9} };
EFI_GUID gOemIpmiNvramUpdateGuid = { 0xaa264f75, 0x4b9e, 0x4421, {0xbb, 0x36, 0x96, 0x51, 0x8a, 0x46, 0xc4, 0x4e} };


//-----------------------------------------------------------------------------
//**************** OEM IPMI Command *************************************
//-----------------------------------------------------------------------------

#define OEM_UPDATE_SETUP                   0x01
#define OEM_NEED_COLD_RESET                0x02


#define OEM_IPMI_NET_FUN                   0x3A

#define OEM_IPMI_SET_FAN_POLICY            0x0B
#define OEM_IPMI_GET_FAN_POLICY            0x0C
#define OEM_IPMI_SET_BIOS_OPTIONS          0xC0
#define OEM_IPMI_GET_BIOS_OPTIONS          0xC1

#define OEM_IPMI_SETUP_REQUEST_LENGTH      20
#define OEM_IPMI_SETUP_RESPONSE_LENGTH     20

#define OEM_IPMI_SETUP_NUM_OF_RETRY        3

typedef enum {
  OemIpmiSetupCpuOptions = 0,
  OemIpmiSetupMemoryOptions,
  OemIpmiSetupPowerOptions,
  OemIpmiSetupIoOptions,
  OemIpmiSetupThermalOptions,
  OemIpmiSetupManagementOptions,
  OemIpmiSetupBootOptions,
//<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(CMCC_NFV_PROJECT_SUPPORT) && (CMCC_NFV_PROJECT_SUPPORT == 1)  
  OemIpmiSetupPxeOptions, 
#endif
//<Chendl001-20170120 Network PXE Function modified by OOB ->
//<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION==1)
  OemIpmiSetupNicSriovOptions = 9,
#endif
//<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  OemIpmiSetupMiscOptions = 13, //<Yaosr001-20190731 add MISC option.>
  OemIpmiSetupMaxOptions
} OEM_IPMI_SETUP_OPTION;

typedef enum {
  OemIpmiSetupSourceBmc = 0,
  OemIpmiSetupSourceBios
} OEM_IPMI_SETUP_SOURCE;

typedef enum {
  OemIpmiSetupDataModify = 0,
  OemIpmiSetupDataInitialized
} OEM_IPMI_SETUP_TYPE;


//-----------------------------------------------------------------------------
//**************** OEM IPMI Command-Setup *******************************
//-----------------------------------------------------------------------------
#pragma pack(push)
#pragma	pack(1)

typedef struct {
  UINT8   Source;
  UINT8   Selector;
} OEM_IPMI_BIOS_SET_REQ_OPTIONS;

typedef struct {
  UINT8   Selector;
  UINT8   DataType;
} OEM_IPMI_BIOS_GET_REQ_OPTIONS;

typedef struct {
  UINT8   CompCode;
  UINT8   ValidFlag;
} OEM_IPMI_BIOS_GET_RES_OPTIONS;


typedef struct {
  UINT8   SmtMode;
  UINT8   SvmMode;
  UINT8   CorePerfBoost;
  UINT8   GlobalCStateControl;
  UINT8   SriovSupport; //<YinQiang001-20171102 Add SR-IOV Support Ipmi Command>
  UINT8   Reserved[11]; //<YinQiang001-20171102 Add SR-IOV Support Ipmi Command>
} OEM_IPMI_SETUP_CPU_OPTIONS;

typedef struct {
  UINT8   NUMA;
  UINT8   BankInterleaving;
  UINT8   ChannelInterleaving;
  UINT8   Reserved[13];
} OEM_IPMI_SETUP_MEMORY_OPTIONS;

typedef struct {
  UINT8   AcPowerLoss;
  UINT8   Reserved[15];
} OEM_IPMI_SETUP_POWER_OPTIONS;

typedef struct {
  UINT8   Iommu;
  UINT8   SataMode;
#if (OemWriteCache_SUPPORT == 1)
  UINT8   SataWriteCache; //<Yaosr001-20200711 Send SataWriteCache to BMC by IPMI.>
#endif
  UINT8   Reserved[13];
} OEM_IPMI_SETUP_IO_OPTIONS;

typedef struct {
  UINT8   FanCtl_Mode;
  UINT8   Reserved[15];
} OEM_IPMI_SETUP_THERMAL_OPTIONS;

typedef struct {
  UINT8   SharelinkNET;
  UINT8   ConsoleRedirection;
  UINT8   TerminalType;
  UINT8   BaudRate;
  UINT8   FlowControl;
  UINT8   DataBits;
  UINT8   Parity;
  UINT8   StopBits;
  UINT8   BMCTimeSyncMode; //<huangjin001-20191211  Add "BMC Time Sync Mode" options.>
  UINT8   Reserved[7];
} OEM_IPMI_SETUP_MANAGEMENT_OPTIONS;

typedef struct {
  UINT8   BootOptionFilter;
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. +>
  UINT8   OpROMENPxe;
  UINT8   LegacyPxeRetryCount;
  UINT8   UefiPxeRetryCount;
  //<Yaosr002-20200711 Add PXE and PXE retry count to BMC by IPMI. ->
  UINT8   Reserved[12];
} OEM_IPMI_SETUP_BOOT_OPTIONS;

//<Yaosr001-20190731 Support BIOS HotKey select. +>
typedef struct {
  UINT8   BiosHotKeySupport; //defined(BIOS_HOTKEY_SUPPORT) && (BIOS_HOTKEY_SUPPORT == 1)
  UINT8   DebugModeSync; ////<Kangmm-20210616-Add BMC ipmi debug support
  UINT8   Reserved[14];
  //UINT8   Reserved[16];
} OEM_IPMI_SETUP_MISC_OPTIONS;
//<Yaosr001-20190731 Support BIOS HotKey select. ->

//<Chendl001-20170120 Network PXE Function modified by OOB +>
//<Chendl001-20181223 Optimized PXE Select Module Function +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
#pragma pack(1)                        
typedef struct {
  UINTN    VarIndex;     // Index in SetupData.OpROME
  UINT8    Network_Bus;  // Network device bus number
  UINT8    Network_Dev;  // Network device device number
  UINT8    Network_Func; // Network device function number
  UINT8    PxeSel;       // Pxe enable or not
  UINT8    NeedSet;      // Set by Python tool
  UINT8    Reserved[3];  // Reserved 
} NETWORK_PXE_INFO;

typedef struct _PXE_SELECT_SETUP_DATA {
  NETWORK_PXE_INFO      NetWorkPostInfo[MaxPxeDevNum]; 
  UINT8                 NetDevNum;
} PXE_SELECT_SETUP_DATA;
#pragma pack()

typedef struct {
  UINT8   PXE[16];
} OEM_IPMI_PXE_OPTIONS;
//<Chendl001-20181223 Optimized PXE Select Module Function ->
#endif
//<Chendl001-20170120 Network PXE Function modified by OOB ->

//<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION==1)
typedef struct _OEM_IPMI_NIC_SRIOV_OPTIONS {
  UINT8   NicSriovEn[16];
} OEM_IPMI_NIC_SRIOV_OPTIONS;
#endif
//<Lvshh001-20190621 Network SRIOV Function modified by OOB ->

typedef struct {
  UINT8                               ValidIpmiData[OemIpmiSetupMaxOptions];
  OEM_IPMI_SETUP_CPU_OPTIONS        *CpuOptions;
  OEM_IPMI_SETUP_MEMORY_OPTIONS     *MemoryOptions;
  OEM_IPMI_SETUP_POWER_OPTIONS      *PowerOptions;
  OEM_IPMI_SETUP_IO_OPTIONS         *IoOptions;
  OEM_IPMI_SETUP_THERMAL_OPTIONS    *ThermalOptions;
  OEM_IPMI_SETUP_MANAGEMENT_OPTIONS *ManagementOptions;
  OEM_IPMI_SETUP_BOOT_OPTIONS       *BootOptions;
  //<Chendl001-20170120 Network PXE Function modified by OOB +>
#if defined(NIC_PXE_CTRL_FUNCTION) && (NIC_PXE_CTRL_FUNCTION == 1) 
  OEM_IPMI_PXE_OPTIONS              *PxeOptions;
#endif  
  //<Chendl001-20170120 Network PXE Function modified by OOB ->
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB +>
#if defined(NIC_SRIOV_CTRL_FUNCTION) && (NIC_SRIOV_CTRL_FUNCTION==1)
  OEM_IPMI_NIC_SRIOV_OPTIONS        *BiosNicSriovOptions;
#endif
  //<Lvshh001-20190621 Network SRIOV Function modified by OOB ->
  OEM_IPMI_SETUP_MISC_OPTIONS       *MiscOptions;//<Yaosr001-20190731 Support BIOS HotKey select.>
} OEM_IPMI_SETUP_DATA;

typedef struct _OEM_LAST_SETUP_DATA {
  UINT8   AcPowerLoss;
  UINT8   FanCtl_Mode;
} OEM_LAST_SETUP_DATA;
#pragma pack(pop)

#endif // __OEM_OEM_IPMI_SETUP_H__
