
//**********************************************************************
// $Header: AliIpmiSyncSetup.c
//
// $Function: Ali Setup Option Sync Feature
//
// $Revision:01
//
//**********************************************************************

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include "AliIpmiSyncSetup.h"
#include <Setup.h>
#include <AmdCbsVariable.h>
#include <OemPCIeData.h>

#define OEMPCIEVAR   L"OEMPCIE_DEV"


EFI_GUID                 gOemPcieVarGuid         = OEMPCIEDEV_VARSTORE_GUID;
EFI_GUID                 gEfiSetupVariableGuid   = SETUP_GUID;
SETUP_DATA               NvDataSetup             = {0};
OEMPCIE_SETUP_DATA       NvDataPcie              = {0};
CBS_CONFIG               NvDataAmdSetup          = {0};
UINT32                   SetupAttribute          = 0;
UINT32                   PcieAttribute           = 0;
UINT32                   CbsAttribute            = 0;
IPMI_SETUP_READ_RESPONSE gCurrentNvData          = {0};
IPMI_SETUP_READ_RESPONSE AliIpmiSetupData        = {0};
EFI_IPMI_TRANSPORT       *mIpmiTransportProtocol = NULL;

/**
  GetNvData :

  Update INI_DATA of BMC with BIOS Setup Data

  @return EFI_STATUS Status
**/
EFI_STATUS
EFIAPI
GetNvData(
    IN IPMI_SETUP_READ_RESPONSE *pIpmiSetupData)
{
    EFI_STATUS      Status = EFI_SUCCESS;
    UINTN           VariableSize = 0;

    DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - GetNvData\n"));
    if (pIpmiSetupData == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    //-----------------------------------------------------------------
    // Get Nvram Value
    //-----------------------------------------------------------------
    VariableSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable(
        L"Setup",
        &gEfiSetupVariableGuid,
        &SetupAttribute,
        &VariableSize,
        &NvDataSetup);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    VariableSize = sizeof(OEMPCIE_SETUP_DATA);
    Status = gRT->GetVariable(
        OEMPCIEVAR,
        &gOemPcieVarGuid,
        &PcieAttribute,
        &VariableSize,
        &NvDataPcie);
    if (EFI_ERROR(Status))
    {
        return Status;
    }

    VariableSize = sizeof(CBS_CONFIG);
    Status = gRT->GetVariable(
        L"AmdSetup",
        &gCbsSystemConfigurationGuid,
        &CbsAttribute,
        &VariableSize,
        &NvDataAmdSetup);
    if (EFI_ERROR(Status))
    {
        return Status;
    }
    pIpmiSetupData->Length = MAX_SETUP_OPTIONS;

    // Index 0
    pIpmiSetupData->SetupValue[0] = 0;

    // Index 1
    pIpmiSetupData->SetupValue[1] = ConvertToIpmiValue(NvDataAmdSetup.CbsCmnCpuCpb);

    // Index 2
    pIpmiSetupData->SetupValue[2] = ConvertToIpmiValue(NvDataAmdSetup.CbsCpuSmtCtrl);

    // Index 3
    pIpmiSetupData->SetupValue[3] = ConvertToIpmiValue(NvDataAmdSetup.CbsCmnSVMCtrl);

    // Index 4
    pIpmiSetupData->SetupValue[4] = ReversalConvertToIpmiValue(NvDataSetup.ADVANCE_P_STATE);

    // Index 5
    if (NvDataAmdSetup.CbsDfCmnNumaMode == 1)
    {
        (NvDataAmdSetup.CbsDfCmnMemIntlv == 2) ? 0x81 : 0x83;
    }
    else
    {
        pIpmiSetupData->SetupValue[5] = 0x80;
    }

    // Index 6
    pIpmiSetupData->SetupValue[6] = ConvertToIpmiValue(NvDataSetup.DmiVendorChange);

    // Index 7
    pIpmiSetupData->SetupValue[7] = ConvertToIpmiValue(NvDataAmdSetup.CbsCmnGnbNbIOMMU);
    ;

    // Index 8
    pIpmiSetupData->SetupValue[8] = ConvertToIpmiValue(NvDataSetup.SriovSupport);

    // Index 9
    pIpmiSetupData->SetupValue[9] = (NvDataSetup.OnExtVgaSelect == 1) ? 0x80 : 0x81;

    // Index A
    pIpmiSetupData->SetupValue[0xA] = ConvertToIpmiValue(NvDataSetup.LocalHddBoot);

    // Index B
    pIpmiSetupData->SetupValue[0xB] = ConvertToIpmiValue(NvDataSetup.BiosHotKeySupport);

    // Index C
    pIpmiSetupData->SetupValue[0xC] = 0x80;

    // Index D
    pIpmiSetupData->SetupValue[0xD] = 0x80;

    // Index E
    if (NvDataSetup.TpmEnable != 0)
    {
        if (NvDataSetup.TpmOperation != 0)
            pIpmiSetupData->SetupValue[0xE] = 0x83;
        else
            pIpmiSetupData->SetupValue[0xE] = 0x81;
    }
    else
    {
        pIpmiSetupData->SetupValue[0xE] = 0x80;
    }

    // Index F
    // pIpmiSetupData->SetupValue[0xF] = ConvertToIpmiValue(NvDataSetup.PowerOffRemove);

    // Index 0x10
    pIpmiSetupData->SetupValue[0x10] = ConvertToIpmiValue(NvDataSetup.BootMode);

    return EFI_SUCCESS;
}

/**
  UpdateIpmiDataToNvData :

  Update Ipmi Data to Setup Variables

  @param[in]           IPMI_SETUP_READ_RESPONSE pIpmiDataRead
  @param[out]          UINT8                 UpdatedSetupFlag

  @return EFI_STATUS Status
**/
EFI_STATUS UpdateIpmiDataToNvData(
    IN IPMI_SETUP_READ_RESPONSE *pIpmiDataRead,
    OUT UINT8 *UpdatedSetupFlag)
{
    UINT8           UpdatedFlag;
    UINT8           UpdatedCbsFlag;
    UINT8           UpdatedOemPcieFlag;

    if (pIpmiDataRead == NULL || UpdatedSetupFlag == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

    if (CompareMem(&gCurrentNvData, pIpmiDataRead, sizeof(IPMI_SETUP_READ_RESPONSE)) == 0)
    {
        DEBUG((EFI_D_INFO, "Setup setting get from BMC same with current setting, return with EFI_SUCCESS\n"));
        return EFI_SUCCESS;
    }
    // Index 1: Core Performance Boost
    ConvertIpmiToNvData(NvDataAmdSetup.CbsCmnCpuCpb, pIpmiDataRead->SetupValue[1], UpdatedCbsFlag, OEM_NEED_COLD_RESET);

    // Index 2: SMT
    ConvertIpmiToNvData(NvDataAmdSetup.CbsCpuSmtCtrl, pIpmiDataRead->SetupValue[2], UpdatedCbsFlag, OEM_NEED_COLD_RESET);

    // Index 3: SVM
    ConvertIpmiToNvData(NvDataAmdSetup.CbsCmnSVMCtrl, pIpmiDataRead->SetupValue[3], UpdatedCbsFlag, OEM_NEED_COLD_RESET);

    // Index 4: P-State Control
    ReversalConvertIpmiToNvData(NvDataSetup.ADVANCE_P_STATE, pIpmiDataRead->SetupValue[4], UpdatedFlag, OEM_NEED_NO_RESET);

    // Index 5: Memory Interleaving
    if (gCurrentNvData.SetupValue[5] != pIpmiDataRead->SetupValue[5])
    {
        UpdatedCbsFlag = 1;
        *UpdatedSetupFlag = OEM_NEED_COLD_RESET;
        if (pIpmiDataRead->SetupValue[5] == 0x80)
            NvDataAmdSetup.CbsDfCmnNumaMode = 0;
        else
            NvDataAmdSetup.CbsDfCmnNumaMode = 1;
        if (pIpmiDataRead->SetupValue[5] == 0x81)
            NvDataAmdSetup.CbsDfCmnMemIntlv = 2;
        if ((pIpmiDataRead->SetupValue[5] == 0x82) || (pIpmiDataRead->SetupValue[5] == 0x83))
            NvDataAmdSetup.CbsDfCmnMemIntlv = 1;
    }

    // Index 6: Vendor Change
    ConvertIpmiToNvData(NvDataSetup.DmiVendorChange, pIpmiDataRead->SetupValue[6], UpdatedFlag, OEM_NEED_NO_RESET);

    // Index 7: IOMMU
    ConvertIpmiToNvData(NvDataAmdSetup.CbsCmnGnbNbIOMMU, pIpmiDataRead->SetupValue[7], UpdatedCbsFlag, OEM_NEED_COLD_RESET);

    // Index 8: SRIOV
    ConvertIpmiToNvData(NvDataSetup.SriovSupport, pIpmiDataRead->SetupValue[8], UpdatedFlag, OEM_NEED_NO_RESET);

    // Index 9: Onboard VGA
    if (gCurrentNvData.SetupValue[9] != pIpmiDataRead->SetupValue[9])
    {
        UpdatedFlag = 1;
        *UpdatedSetupFlag = OEM_NEED_WARM_RESET;
        if (pIpmiDataRead->SetupValue[9] == 0x80)
            NvDataSetup.OnExtVgaSelect = 1;
        if (pIpmiDataRead->SetupValue[9] == 0x81)
            NvDataSetup.OnExtVgaSelect = 2;
    }

    // Index A: Local Hdd Boot
    ConvertIpmiToNvData(NvDataSetup.LocalHddBoot, pIpmiDataRead->SetupValue[0xA], UpdatedFlag, OEM_NEED_NO_RESET);

    // Index B: BIOS HotKey Support
    ConvertIpmiToNvData(NvDataSetup.BiosHotKeySupport, pIpmiDataRead->SetupValue[0xB], UpdatedFlag, OEM_NEED_NO_RESET);

    // Index C: Reserved

    // Index D: Reserved

    // Index E: TPM
    if (gCurrentNvData.SetupValue[0xE] != pIpmiDataRead->SetupValue[0xE])
    {
        UpdatedFlag = 1;
        *UpdatedSetupFlag = OEM_NEED_COLD_RESET;
        switch (pIpmiDataRead->SetupValue[0xE])
        {
        case 0x80:
            NvDataSetup.TpmEnable = 0;
            break;
        case 0x81:
            NvDataSetup.TpmEnable = 1;
            break;
        case 0x83:
            NvDataSetup.TpmEnable = 1;
            if (NvDataSetup.Tpm20Device == 1)
                NvDataSetup.TpmOperation = 1;
            else
                NvDataSetup.TpmOperation = TCPA_PPIOP_CLEAR;
            break;
        default:
            break;
        }
    }

    // Index F: Power Off Remove
    // ConvertIpmiToNvData(NvDataSetup.PowerOffRemove,    pIpmiDataRead->SetupValue[0xF],    UpdatedFlag, OEM_NEED_NO_RESET);

    // Index 0x10: Boot Mode
    if (gCurrentNvData.SetupValue[0x10] != pIpmiDataRead->SetupValue[0x10])
    {
        ConvertIpmiToNvData(NvDataSetup.BootMode, pIpmiDataRead->SetupValue[0x10], UpdatedFlag, OEM_NEED_NO_RESET);
        UpdatedOemPcieFlag = 1;
        if (NvDataSetup.BootMode == 0) // Legacy Mode
        {
            NvDataSetup.BootOptionFilter = 1;
            NvDataPcie.OpROMEN[PXEINDX] = 2;
            NvDataPcie.OpROMEN[MASSINDX] = 2;
            NvDataPcie.OpROMEN[VIDEOINDX] = 2;
            NvDataPcie.OpROMEN[OLDINDX] = 2;
        }
        if (NvDataSetup.BootMode == 1) // Uefi Mode
        {
            NvDataSetup.BootOptionFilter = 2;
            NvDataPcie.OpROMEN[PXEINDX] = 1;
            NvDataPcie.OpROMEN[MASSINDX] = 1;
            NvDataPcie.OpROMEN[VIDEOINDX] = 1;
            NvDataPcie.OpROMEN[OLDINDX] = 1;
        }
    }

    if (UpdatedFlag)
    {
        gRT->SetVariable(
            L"Setup",
            &gEfiSetupVariableGuid,
            SetupAttribute,
            sizeof(SETUP_DATA),
            &NvDataSetup);
    }
    if (UpdatedOemPcieFlag)
    {
        gRT->SetVariable(
            OEMPCIEVAR,
            &gOemPcieVarGuid,
            PcieAttribute,
            sizeof(OEMPCIE_SETUP_DATA),
            &NvDataPcie);
    }
    if (UpdatedCbsFlag)
    {
        gRT->SetVariable(
            L"AmdSetup",
            &gCbsSystemConfigurationGuid,
            CbsAttribute,
            sizeof(CBS_CONFIG),
            &NvDataAmdSetup);
    }
    return EFI_SUCCESS;
}

/**
  SyncIpmiDataAndNvData :

  Update BIOS Setup Data with BMC modify data

  @param[in]           EFI_EVENT              Event
  @param[in]           VOID                   *Context

 @return EFI_STATUS Status
**/
VOID
SyncIpmiDataAndNvData(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
    EFI_STATUS                  Status;
    IPMI_SETUP_READ_REQUEST     ReadRequest = {0};
    UINT8                       RequSize;
    IPMI_SETUP_READ_RESPONSE    ReadResponse = {0};
    UINT8                       RespSize;
    IPMI_SETUP_WRITE_REQUEST    WriteRequest = {0};
    UINT8                       WriteResponse[20];
    UINT8                       Retry;
    UINT8                       UpdatedSetupFlag;

    DEBUG((EFI_D_INFO, "[OemDbg] ALI IPMI Bios Setup - SyncIpmiDataAndNvData\n"));

    //
    // Locate IPMI Transport Protocol
    //
    if (mIpmiTransportProtocol == NULL)
    {
        Status = gBS->LocateProtocol(
            &gEfiDxeIpmiTransportProtocolGuid,
            NULL,
            &mIpmiTransportProtocol);
        if (EFI_ERROR(Status) || (mIpmiTransportProtocol == NULL))
        {
            DEBUG((EFI_D_INFO, "[OemDbg] %a Line %d return with %r\n", __FUNCTION__, __LINE__, Status));
            return;
        }
    }

    Status = GetNvData(&gCurrentNvData);
    if (Status != EFI_SUCCESS)
    {
        DEBUG((EFI_D_INFO, "[OemDbg] %a Line %d return with %r\n", __FUNCTION__, __LINE__, Status));
        return;
    }

    //
    // Init Configuration Struct Data Buffer
    //
    Retry = OEM_IPMI_SETUP_NUM_OF_RETRY;
    while (Retry > 0)
    {
        ReadRequest.Index = 0;
        ReadRequest.Length = MAX_SETUP_OPTIONS;
        RequSize = sizeof(IPMI_SETUP_READ_REQUEST);

        Status = mIpmiTransportProtocol->SendIpmiCommand(
            mIpmiTransportProtocol,
            ALI_IPMI_NET_FUN,
            BMC_LUN,
            ALI_IPMI_READ_BIOS_OPTIONS,
            (UINT8 *)&ReadRequest,
            RequSize,
            (UINT8 *)&ReadResponse,
            &RespSize);
        if (EFI_SUCCESS == Status)
        {
            break;
        }
        Retry--;
    }
    if ((Retry == 0) && (Status != EFI_SUCCESS))
    {
        DEBUG((EFI_D_INFO, "[OemDbg] %a Line %d return with %r\n", __FUNCTION__, __LINE__, Status));
        return;
    }

    if (ReadResponse.SetupValue[0] == 0x81) // valid Flag
    {
        Status = UpdateIpmiDataToNvData(&ReadResponse, &UpdatedSetupFlag);
    }
    else if (ReadResponse.SetupValue[0] == 0x82) // Restore Default Value
    {
        Status = UpdateIpmiDataToNvData(&AliIpmiSetupData, &UpdatedSetupFlag);
    }
    else
    {
        DEBUG((EFI_D_INFO, "[OemDbg] %a Line %d return\n", __FUNCTION__, __LINE__));
        return;
    }

    if (EFI_ERROR(Status))
    {
        DEBUG((EFI_D_INFO, "[OemDbg] %a Line %d return with %r\n", __FUNCTION__, __LINE__, Status));
        return;
    }
    Retry = OEM_IPMI_SETUP_NUM_OF_RETRY;
    while (Retry > 0)
    {
        ZeroMem(&WriteRequest, sizeof(IPMI_SETUP_WRITE_REQUEST));
        WriteRequest.Length = 1;
        RequSize = 3; //Only send 'Configuration' byte, BMC will clear all data

        Status = mIpmiTransportProtocol->SendIpmiCommand(
            mIpmiTransportProtocol,
            ALI_IPMI_NET_FUN,
            BMC_LUN,
            ALI_IPMI_WRITE_BIOS_OPTIONS,
            (UINT8 *)&WriteRequest,
            RequSize,
            WriteResponse,
            &RespSize);
        if (EFI_SUCCESS == Status)
        {
            break;
        }
        Retry--;
    }

    //
    // Some NvData changing need reset system
    //
    if (UpdatedSetupFlag)
    {
        DEBUG((EFI_D_INFO, "[OemDbg] OEM IPMI Bios Setup - SyncIpmiDataAndNvData: Reset System\n"));
        (UpdatedSetupFlag & OEM_NEED_COLD_RESET) ? gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL) : gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
}

/**
  SyncNvDataToBmc :

  Update BMC IPMI data BEFORE Boot to OS

  @param[in]           EFI_EVENT              Event
  @param[in]           VOID                   *Context

 @return EFI_STATUS Status
**/
VOID
SyncNvDataToBmc(
    IN EFI_EVENT Event,
    IN VOID *Context)
{
    EFI_STATUS                  Status       = EFI_SUCCESS;
    IPMI_SETUP_WRITE_REQUEST    WriteRequest = {0};
    UINT8                       Retry;
    UINT8                       RequSize;
    UINT8                       WriteResponse[20];
    UINT8                       RespSize;

    //
    // Locate IPMI Transport Protocol
    //
    if (mIpmiTransportProtocol == NULL)
    {
        Status = gBS->LocateProtocol(
            &gEfiDxeIpmiTransportProtocolGuid,
            NULL,
            &mIpmiTransportProtocol);
        if (EFI_ERROR(Status) || (mIpmiTransportProtocol == NULL))
        {
            DEBUG((EFI_D_INFO, "[OemDbg] %a Line %d return with %r\n", __FUNCTION__, __LINE__, Status));
            return;
        }
    }

    GetNvData(&AliIpmiSetupData);
    Retry = OEM_IPMI_SETUP_NUM_OF_RETRY;
    while (Retry > 0)
    {
        WriteRequest.Index = 0;
        WriteRequest.Length = MAX_SETUP_OPTIONS;
        CopyMem (WriteRequest.SetupValue, &AliIpmiSetupData, MAX_SETUP_OPTIONS);
        WriteRequest.SetupValue[0] = 0x81; // Valid Flag
        RequSize = sizeof(IPMI_SETUP_WRITE_REQUEST);

        Status = mIpmiTransportProtocol->SendIpmiCommand(
            mIpmiTransportProtocol,
            ALI_IPMI_NET_FUN,
            BMC_LUN,
            ALI_IPMI_BIOS_SETCONFIG_TO_BMC,
            (UINT8 *)&WriteRequest,
            RequSize,
            WriteResponse,
            &RespSize);
        if (EFI_SUCCESS == Status)
        {
            break;
        }
        Retry--;
    }
    DEBUG((EFI_D_INFO, "[OemDbg] %a return with %r\n", __FUNCTION__, Status));
    return;
}

/**
  AliIpmiSetupEntryPoint :

  Entry point of AliIpmiSetup Module

  @param[in]           EFI_HANDLE             ImageHandle
  @param[in]           EFI_SYSTEM_TABLE       *SystemTable

  @return EFI_STATUS Status
**/
EFI_STATUS
AliIpmiSetupEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    VOID                *NotifyRegistration;
    UINTN               VariableSize;
    UINT32              VariableAttribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
    EFI_EVENT           ReadyToBootEvent = NULL;

    DEBUG((EFI_D_INFO, "[OemDbg] ALI IPMI Bios Setup - AliIpmiSetup EntryPoint\n"));

    VariableSize = sizeof(IPMI_SETUP_READ_RESPONSE);
    Status = gRT->GetVariable(
        L"AliIpmiSetup",
        &gEfiSetupVariableGuid,
        &VariableAttribute,
        &VariableSize,
        &AliIpmiSetupData);
    DEBUG((EFI_D_ERROR, "[OemDbg] ALI IPMI Bios Setup Status %r, PcdGet8(PcdAliMocExist) %x AliIpmiSetupData.SetupValue[0xA] %x \n", Status, PcdGet8(PcdAliMocExist), AliIpmiSetupData.SetupValue[0xA]));
    if (Status == EFI_NOT_FOUND) // Variable not found, set default value
    {
        GetNvData(&AliIpmiSetupData);
        DEBUG((EFI_D_ERROR, "[OemDbg] ALI IPMI Bios Setup Status %r, PcdGet8(PcdAliMocExist) %x AliIpmiSetupData.SetupValue[0xA] %x \n", Status, PcdGet8(PcdAliMocExist), AliIpmiSetupData.SetupValue[0xA]));
        //Ali MOC exist or not, Default setting for LocalHddBoot is not follow the case
        if (PcdGet8(PcdAliMocExist) == (AliIpmiSetupData.SetupValue[0xA] & 0x7F))
        {
            NvDataSetup.LocalHddBoot = (PcdGet8(PcdAliMocExist) == 1)?0:1;
            AliIpmiSetupData.SetupValue[0xA] = ConvertToIpmiValue(NvDataSetup.LocalHddBoot);
            gRT->SetVariable(
                L"Setup",
                &gEfiSetupVariableGuid,
                SetupAttribute,
                sizeof(SETUP_DATA),
                &NvDataSetup);
        }
        Status = gRT->SetVariable(
            L"AliIpmiSetup",
            &gEfiSetupVariableGuid,
            VariableAttribute,
            sizeof(IPMI_SETUP_READ_RESPONSE),
            &AliIpmiSetupData);
    } else
    {
        //Ali MOC exist or not, Default setting for LocalHddBoot is not follow the case
        if (PcdGet8(PcdAliMocExist) ==  (AliIpmiSetupData.SetupValue[0xA] & 0x7F))
        {
            NvDataSetup.LocalHddBoot = (PcdGet8(PcdAliMocExist) == 1)?0:1;
            AliIpmiSetupData.SetupValue[0xA] = ConvertToIpmiValue(NvDataSetup.LocalHddBoot);
            gRT->SetVariable(
                L"Setup",
                &gEfiSetupVariableGuid,
                SetupAttribute,
                sizeof(SETUP_DATA),
                &NvDataSetup);
            Status = gRT->SetVariable(
                L"AliIpmiSetup",
                &gEfiSetupVariableGuid,
                VariableAttribute,
                sizeof(IPMI_SETUP_READ_RESPONSE),
                &AliIpmiSetupData);
        }
    }
    
    if (Status != EFI_SUCCESS)
        return Status;

    //
    // Register the event to Sync Setup from BMC
    //
    EfiCreateProtocolNotifyEvent(
        &gEfiDxeIpmiTransportProtocolGuid,
        TPL_CALLBACK,
        SyncIpmiDataAndNvData,
        NULL,
        &NotifyRegistration);

    Status = gBS->CreateEventEx(
        EVT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        SyncNvDataToBmc,
        NULL,
        &gEfiEventReadyToBootGuid,
        &ReadyToBootEvent);
    DEBUG((EFI_D_INFO, "[OemDbg] ALI IPMI Bios Setup - AliIpmiSetup: Status=%r\n", Status));

    return Status;
}
