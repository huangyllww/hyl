//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  OemControlLanPort.c
//
// Description:	Enable/Disable on board Network I350 port according to setup item
//
// Author    : jiangzhy001-20180908-Enable/Disable on board Network I350 port according to setup item
//<AMI_FHDR_END>
//**********************************************************************

#include <Library/PeiServicesLib.h>
#include <Efi.h>
#include <Pei.h>
#include <token.h>
#include <AmiPeiLib.h>
#include "AmiCspLib.h"
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>

#include <AmdCpmPei.h>

EFI_STATUS PeiLanControlCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
);

static EFI_PEI_NOTIFY_DESCRIPTOR PeiLanControlNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmTablePpiGuid,
  PeiLanControlCallback
};

EFI_GUID  gSetupGuid                     = SETUP_GUID;
UINT32                    OemSmnGpio    = 0x02D02500;
UINT32                    OemSmnIomux    = 0x02D01D00;

EFI_STATUS LanEnableDisable (
        AMD_CPM_TABLE_PPI *CpmTablePpiPtr,
        IN UINT8 Port,
        IN UINT8 Enable) {
    
    EFI_STATUS                Status = EFI_SUCCESS;       
    UINT32                    Value           = 0;
    UINT32                    ReadValue       = 0;

    
    if (Port == 0) {
        ReadValue = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr, SOCKET_I350, DIE_I350, (UINT32) OemSmnGpio, (UINT32) (BIOS_LAN0_DISABLE_N << 2));
        Value = ReadValue | BIT23;
        if (Enable == 1)
            Value = Value | BIT22;
        else
            Value = Value & (~BIT22);
        DEBUG((DEBUG_INFO, "LanEnableDisable for Port %x, Enable %x, Value %x\n", Port, Enable, Value));
        CpmTablePpiPtr->CommonFunction.SmnWrite32 (CpmTablePpiPtr, SOCKET_I350, DIE_I350, (UINT32) OemSmnGpio, (UINT32) (BIOS_LAN0_DISABLE_N << 2), Value);
        CpmTablePpiPtr->CommonFunction.SmnWrite8 (CpmTablePpiPtr, SOCKET_I350, DIE_I350, (UINT32) OemSmnIomux, BIOS_LAN0_DISABLE_N, LAN0_GPIO_IOMUX_FUNCTION);
        return EFI_SUCCESS;
    }
    else {
        ReadValue = CpmTablePpiPtr->CommonFunction.SmnRead32 (CpmTablePpiPtr, SOCKET_I350, DIE_I350, (UINT32) OemSmnGpio, (UINT32) (BIOS_LAN1_DISABLE_N << 2));
        Value = ReadValue | BIT23;
        if (Enable == 1)
            Value = Value | BIT22;
        else
            Value = Value & (~BIT22);
        DEBUG((DEBUG_INFO, "LanEnableDisable for Port %x, Enable %x, Value %x\n", Port, Enable, Value));
        CpmTablePpiPtr->CommonFunction.SmnWrite32 (CpmTablePpiPtr, SOCKET_I350, DIE_I350, (UINT32) OemSmnGpio, (UINT32) (BIOS_LAN1_DISABLE_N << 2), Value);
        CpmTablePpiPtr->CommonFunction.SmnWrite8 (CpmTablePpiPtr, SOCKET_I350, DIE_I350, (UINT32) OemSmnIomux, BIOS_LAN1_DISABLE_N, LAN1_GPIO_IOMUX_FUNCTION);
        return EFI_SUCCESS;
    }
}

EFI_STATUS PeiLanControlCallback(
    IN  EFI_PEI_SERVICES                **PeiServices,
    IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
    IN  VOID                            *InvokePpi
)
{
    
    EFI_STATUS                          Status;
    SETUP_DATA                          SetupData;
    UINT32                              SetupDataAttributes = 0;
    UINTN                               SetupDataSize = sizeof(SETUP_DATA);
    UINT8                               Port;
    UINT8                               Enable;
    AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    UINT32                    ReadValue       = 0;

    Status = (*PeiServices)->LocatePpi(
        PeiServices,
        &gEfiPeiReadOnlyVariable2PpiGuid,
        0, NULL,
        &ReadOnlyVariable
    );
    ASSERT_PEI_ERROR(PeiServices, Status);
    
    Status = (*PeiServices)->LocatePpi (
          (CPM_PEI_SERVICES**)PeiServices,
          &gAmdCpmTablePpiGuid,
          0,
          NULL,
          (VOID**)&CpmTablePpiPtr
          );
    ASSERT_PEI_ERROR(PeiServices, Status);
    
    Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &gSetupGuid, &SetupDataAttributes,
	                                      &SetupDataSize, &SetupData);
    if (EFI_ERROR(Status)) return Status;

    Enable = SetupData.Lan0;
    for (Port = 0; Port < 2; Port++) {
        if (Port == 1)
            Enable = SetupData.Lan1;
        LanEnableDisable(CpmTablePpiPtr, Port, Enable);
    }

#if PCIE_RESET_SOCKET_NUMBER == 0
    CpmTablePpiPtr->CommonFunction.MmioAndThenOr8 ( CpmTablePpiPtr->MainTablePtr->AcpiMemIoBaseAddr + 0x3C4, 0xEF, 0x10);
    CpmTablePpiPtr->CommonFunction.Stall ( CpmTablePpiPtr, 1000 * 5);
    CpmTablePpiPtr->CommonFunction.MmioAndThenOr8 ( CpmTablePpiPtr->MainTablePtr->AcpiMemIoBaseAddr + 0x3C4, 0xEF, 0x00);
#else
    CpmTablePpiPtr->CommonFunction.SmnAndThenOr32 (CpmTablePpiPtr, PCIE_RESET_SOCKET_NUMBER, PCIE_RESET_DIE_NUMBER, (UINT32) 0x2d013C4,0, 0xEF, 0x10);
    CpmTablePpiPtr->CommonFunction.Stall ( CpmTablePpiPtr, 1000 * 5);
    CpmTablePpiPtr->CommonFunction.SmnAndThenOr32 (CpmTablePpiPtr, PCIE_RESET_SOCKET_NUMBER, PCIE_RESET_DIE_NUMBER, (UINT32) 0x2d013C4,0, 0xEF, 0x00);
#endif
    return EFI_SUCCESS;
}


EFI_STATUS EFIAPI OemControlLanPort (
    IN EFI_FFS_FILE_HEADER      *FfsHeader,
    IN EFI_PEI_SERVICES         **PeiServices
)
{
    EFI_STATUS        Status;

    AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI   *FabricTopologyServices;
    UINTN                                  NumberOfInstalledProcessors = 2;
    UINTN                                  TotalNumberOfDie;
    UINTN                                  TotalNumberOfRootBridges;

    if(PCIE_RESET_SOCKET_NUMBER > 0)
    {
        Status = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gAmdFabricTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &FabricTopologyServices
                                   );
        DEBUG((-1, "OemControlLanPort FabricTopologyServices Status %r\n", Status));
        if (Status == EFI_SUCCESS) {
        FabricTopologyServices->GetSystemInfo(&NumberOfInstalledProcessors, &TotalNumberOfDie, &TotalNumberOfRootBridges);
        }
        DEBUG((-1, "OemControlLanPort FabricTopologyServices NumberOfInstalledProcessors %x\n", NumberOfInstalledProcessors));
        if (NumberOfInstalledProcessors < 2)
        {
            DEBUG((EFI_D_ERROR, "[Cancon.%a.%d]Only One Processor has been installed, Can not touch the CPU1's Resource\n",__FUNCTION__,__LINE__));
            return Status;
        }
    }
    Status = (**PeiServices).NotifyPpi (PeiServices, &PeiLanControlNotify);
    ASSERT_PEI_ERROR (PeiServices, Status);
    
    return Status;
}
//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
