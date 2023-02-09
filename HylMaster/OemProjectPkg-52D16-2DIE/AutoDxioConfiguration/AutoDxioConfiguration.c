//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//----------------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------------

#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>
#include  <Library/PciLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
#define FchSmnGpio                      0x02D02500

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

EFI_STATUS PeiConfigDxioSettings (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
    );
//-----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
    { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
    &gAmdCpmTablePpiGuid, PeiConfigDxioSettings}
};

//-----------------------------------------------------------------------------
EFI_STATUS
DxioDisableTable (
  EFI_PEI_SERVICES **PeiServices, 
  AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr, 
  UINT8 StartLane,
  UINT8 EndLane
  )
{
  UINT8                               Index;
  DEBUG ((DEBUG_ERROR, "AutoDxioConfiguration Start: StartLane=%d EndLane=%d \n",StartLane,EndLane));     
  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if ( DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane && DxioTopologyTablePtr->Port[Index].EngineData.EndLane == EndLane ) {
      DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;            
      return EFI_SUCCESS;
    }        
  }
  DEBUG ((DEBUG_ERROR, "AutoDxioConfiguration end\n"));              
  return EFI_NOT_FOUND;
}

EFI_STATUS
DxioDisableTableForSwitch( 
  EFI_PEI_SERVICES **PeiServices,
  AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr, 
  UINT8 EngineType,
  UINT8 StartLane )
{    
  UINT8                               Index;
  DEBUG ((DEBUG_ERROR, "[Oem Dxio] AutoDxioConfigurationForSwitch Start: StartLane=%d, EngineType=%d\n",StartLane,EngineType));     
  for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
    if ( DxioTopologyTablePtr->Port[Index].EngineData.EngineType == EngineType && DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane ) {
      DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;            
      return EFI_SUCCESS;
    }        
  }
  DEBUG ((DEBUG_ERROR, "AutoDxioConfigurationForSwitch end\n"));              
  return EFI_NOT_FOUND;
}
/**
 * Get Multi Socket/Die FCH GPIO Pin
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 * @param[in] Socket  The Socket Number.
 * @param[in] Die     The Die Number.
 * @param[in] Pin     The GPIO Pin Number.
 *
 * @retval            The State of GPIO Pin.
 *
 */
UINT8
CpmSmnGetGpio (
  IN       VOID                        *This,
  IN       UINT16                      Socket,
  IN       UINT16                      Die,
  IN       UINT16                      Pin
  )
{
  AMD_CPM_COMMON_FUNCTION *CommonFunctionPtr;
  UINT8                   Value;

  CommonFunctionPtr   = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;
  Value = 0;

  Value = CommonFunctionPtr->SmnRead8 (This, Socket, Die, (UINT32) FchSmnGpio, (UINT32) ((Pin << 2) + 2)) & 0x01;
  return Value;
}


BOOLEAN Is2diecpu()
{
    BOOLEAN Is2die = TRUE;
    UINT32  VidDid = 0xffffffff;
    VidDid = PciRead32(PCI_LIB_ADDRESS(0, 0x1a, 0, 0x00));
    
    if(VidDid != 0xffffffff) Is2die = FALSE;
    
    //DEBUG((EFI_D_ERROR, "Is2diecpu is %d\n",Is2die));
    
    return Is2die;
}

//Hugc_20220129_Add UEFI Mode Graphic SW Function>>>
void DXIOUpdateForGraphicSwitch(
  IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
)
{
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *pDxioTopologyTable=NULL;
  UINT8                                iDxioTable;
  //UINT8                               DisableOnboardSwitch;

  if(PcdGet8(PcdOnBoardVideoEn) != 0)
    return;
  
  pDxioTopologyTable= CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  
  for ( iDxioTable= 0; pDxioTopologyTable->Port[iDxioTable].Flags != DESCRIPTOR_TERMINATE_LIST; iDxioTable++)
  {
    /* code */
    if (pDxioTopologyTable->Port[iDxioTable].EngineData.StartLane != 1)
      continue;

    DEBUG((EFI_D_ERROR, "[OemDbg][%a][%d]Disable Onboard VGA -AST2500\n\n",__FUNCTION__,__LINE__));
    pDxioTopologyTable->Port[iDxioTable].EngineData.EngineType=DxioUnusedEngine;

    break;
  }
  
  return;
}

//Hugc_20220129_Add UEFI Mode Graphic SW Function<<<

/**
 * Read Riser ID GPIO and config DXIO setting per riser id.
 */
EFI_STATUS  
PeiConfigDxioSettings (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi )
{
  EFI_STATUS                          Status;
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
  UINT8                               NvmeCardDet;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS0Ptr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS1Ptr;
  SETUP_DATA                          SetupData;
  UINTN                               VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
  EFI_GUID                            SetupGuid= SETUP_GUID;
  UINT8                               M2CardDet; //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA
  Status = (*PeiServices)->LocatePpi (
            (CPM_PEI_SERVICES**)PeiServices,
            &gAmdCpmTablePpiGuid,
            0,
            NULL,
            (VOID**)&CpmTablePpiPtr
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  if (Is2diecpu()) {
      NvmeCardDet = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,1 ,3  ); //Socket0 die3 gpio23 AGPIO23_3  NVME_CARD_DET_N   2die CPU
  } else {
      NvmeCardDet = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,3 ,23  ); //Socket0 die3 gpio23 AGPIO23_3  NVME_CARD_DET_N   4die CPU
  }
  
  DxioTopologyTableS0Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);    
  DxioTopologyTableS1Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);
  
  DEBUG ((EFI_D_INFO, "[Oem Dxio] NvmeCardDet :%d, ", NvmeCardDet));
  if (NvmeCardDet == 0) { // riser not present, J7
    DEBUG ((EFI_D_INFO, "Has riser\n"));
    PcdSet8(PcdRetimerPresent,1); //<<duanjun 20210602 post aic nvme pcieinf to bmc>
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 48, 63 );
//<duanjun-20200427-support 5xx cpu && 7xx cpu +>
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 48+32, 63+32 );
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->	
  } else { // riser present
    DEBUG ((EFI_D_INFO, "No riser\n"));
    PcdSet8(PcdRetimerPresent,0); //<<duanjun 20210602 post aic nvme pcieinf to bmc>
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 48, 51 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 52, 55 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 56, 59 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 60, 63 );
//<duanjun-20200427-support 5xx cpu && 7xx cpu +>    
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 48+32, 51+32 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 52+32, 55+32 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 56+32, 59+32 );
    Status = DxioDisableTable( PeiServices, DxioTopologyTableS1Ptr, 60+32, 63+32 );
//<duanjun-20200427-support 5xx cpu && 7xx cpu ->	
  }  

  //sata pcie switch
  Status = (*PeiServices)->LocatePpi(
              PeiServices,
              &gEfiPeiReadOnlyVariable2PpiGuid,
              0, 
              NULL,
              &ReadOnlyVariable
              );
        
  VariableSize = sizeof(SETUP_DATA);
  Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                  NULL, &VariableSize, &SetupData);
  DEBUG ((EFI_D_INFO, "[Oem Dxio] CPU1SataPcieSel[0]:%d (J27, 0=SATA)\n", SetupData.CPU0SataPcieSel));
  if (SetupData.CPU0SataPcieSel) {  // J27
    PcdSet8(PcdJ27Nvme,1);  //<<duanjun 20210602 post aic nvme pcieinf to bmc>
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4 );
  } else {
    PcdSet8(PcdJ27Nvme,0);  //<<duanjun 20210602 post aic nvme pcieinf to bmc>
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4 );
  }
 //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA >>> 
  M2CardDet = CpmSmnGetGpio ((VOID*)CpmTablePpiPtr,0 ,0 ,88  ); //Socket0 die0 gpio88 AGPIO88  
  DEBUG ((EFI_D_INFO, "M2CardDet: %d\n", M2CardDet));
  
  if (M2CardDet) {
      Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 2 );
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 3 );
  } else {
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 2 );
    Status = DxioDisableTableForSwitch( PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 3 );
  }
  
  //Hugc-20211218-Add AGPIO88 to decide whether it's PCIE or SATA <<<  
  //Hugc_20220129_Add UEFI Mode Graphic SW Function>>>
  
  DXIOUpdateForGraphicSwitch(CpmTablePpiPtr);
  //Hugc_20220129_Add UEFI Mode Graphic SW Function<<<
  
  
  return Status;
}

//----------------------------------------------------------------------------
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   ProjectPEI_Init
//
// Description: This function is the entry point for this PEI. This function
//              initializes the Project PEI phase.
//
// Parameters:  FfsHeader   Pointer to the FFS file header
//              PeiServices Pointer to the PEI services table
//
// Returns:     Return Status based on errors that occurred while waiting for
//              time to expire.
//
// Notes:       This function could initialize Project for anything.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

EFI_STATUS
AutoDxioConfigurationInit (
    IN       EFI_PEI_FILE_HANDLE   FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  (*PeiServices)->NotifyPpi( PeiServices, PeiNotifyList );

  return Status;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2013, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
