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
#include "DXIOTable.h"
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

//----------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//----------------------------------------------------------------------------
#define FchSmnGpio                      0x02D02500ul
#define FchSmnIomux                     0x02D01D00ul

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

EFI_STATUS PeiConfigDxioSettings(
	IN EFI_PEI_SERVICES             **PeiServices,
	IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
	IN VOID                         *InvokePpi
	);
//-----------------------------------------------------------------------------

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
	{ EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST, \
	&gAmdCpmGpioInitFinishedPpiGuid, PeiConfigDxioSettings }
};

//-----------------------------------------------------------------------------
EFI_STATUS
DxioEnableTable(
EFI_PEI_SERVICES **PeiServices,
AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr,
UINT8 EngineType,
UINT8 StartLane,
UINT8 EndLane
)
{
	UINT8                               Index;
	BOOLEAN								FoundOne = FALSE;

	DEBUG((DEBUG_ERROR, "AutoDxioConfiguration Start: StartLane=%d EndLane=%d, EngineType=%d\n", StartLane, EndLane, EngineType));
	for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {

		if (DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane &&
			DxioTopologyTablePtr->Port[Index].EngineData.EndLane == EndLane &&
			DxioTopologyTablePtr->Port[Index].EngineData.EngineType == EngineType &&
			FoundOne == FALSE)
		{
			// skip myself
			FoundOne = TRUE;
			continue;
		}else{
			if ((DxioTopologyTablePtr->Port[Index].EngineData.StartLane >= StartLane &&
				DxioTopologyTablePtr->Port[Index].EngineData.StartLane <= EndLane)||
				(DxioTopologyTablePtr->Port[Index].EngineData.EndLane >= StartLane &&
				DxioTopologyTablePtr->Port[Index].EngineData.EndLane <= EndLane))
				{
					DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;
					DEBUG((DEBUG_ERROR, "\tUpdate StartLane=%d EndLane=%d to EngineType=%d\n",
								DxioTopologyTablePtr->Port[Index].EngineData.StartLane,
								DxioTopologyTablePtr->Port[Index].EngineData.EndLane,
								DxioUnusedEngine));
				}
		}
	}
	DEBUG((DEBUG_ERROR, "AutoDxioConfiguration end\n"));
	return EFI_SUCCESS;
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
CpmSmnGetGpio(
IN       VOID                        *This,
IN       UINT16                      Socket,
IN       UINT16                      Die,
IN       UINT16                      Pin
)
{
	AMD_CPM_COMMON_FUNCTION *CommonFunctionPtr;
	UINT32                   Value;

	CommonFunctionPtr = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;
	Value = 0;

    /* Read GPIOBank#Ctl */
    Value = CommonFunctionPtr->SmnRead32(This, Socket, Die, (UINT32)FchSmnGpio, (UINT32)(Pin << 2));
    DEBUG((DEBUG_INFO, "CpmSmnGetGpio Pin(%d), Value(0x%08X)\n", Pin, Value));

    /* Return PinSts(BIT16) */
	return (Value >> 16) & 0x1;
}

/**
 * Set Multi Socket/Die FCH GPIO Pin
 *
 *
 * @param[in] This    The pointer of AMD CPM Table Ppi or Protocol
 * @param[in] Socket  The Socket Number.
 * @param[in] Die     The Die Number.
 * @param[in] Pin     The GPIO Pin Number.
 * @param[in] Value   The GPIO Pin Value.
 *
 * @retval            NONE.
 *
 */
VOID
CpmSmnSetGpio(
IN       VOID                        *This,
IN       UINT16                      Socket,
IN       UINT16                      Die,
IN       UINT16                      Pin,
IN       UINT8                       Value
)
{
	AMD_CPM_COMMON_FUNCTION          *CommonFunctionPtr;
	UINT8                            PinMode = 0;

	CommonFunctionPtr = &((AMD_CPM_TABLE_PPI*)This)->CommonFunction;

	PinMode = CommonFunctionPtr->SmnRead8(This, Socket, Die, (UINT32)FchSmnGpio, (UINT32)((Pin << 2) + 2));
	DEBUG((DEBUG_INFO, "CpmSmnSetGpio Pin(%d), PinMode(%x), Value(%d)\n", Pin, PinMode, Value));
	//the highest two bits are reserved
	PinMode = PinMode & 0xC7;
	//pull up
	PinMode |= 0x10;
	//output enable
	PinMode |= 0x80;
	//output state
	PinMode &= 0xBF;
	CommonFunctionPtr->SmnWrite8(This, Socket, Die, (UINT32)FchSmnGpio, (UINT32)((Pin << 2) + 2), PinMode);

	CommonFunctionPtr->SmnWrite8(This, Socket, Die, (UINT32)FchSmnIomux, (UINT32)Pin, Value);

	Value = CommonFunctionPtr->SmnRead8(This, Socket, Die, (UINT32)FchSmnGpio, (UINT32)((Pin << 2) + 2));
	DEBUG((DEBUG_INFO, "CpmSmnSetGpio retValue(%x)\n", Value));
}

/**
 * Read Riser ID GPIO and config DXIO setting per riser id.
 */
EFI_STATUS
PeiConfigDxioSettings(
IN EFI_PEI_SERVICES             **PeiServices,
IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
IN VOID                         *InvokePpi)
{
    EFI_STATUS                          Status;
    AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
    AMD_CPM_DXIO_TOPOLOGY_TABLE         *DxioTopologyTableS0Ptr;
    AMD_CPM_DXIO_TOPOLOGY_TABLE         *TmpDxioTopologyTableS0Ptr;
    UINT8                               Gpio88, Gpio90, Gpio91, Gpio115;
    UINT8                               GpioMatrix;
    UINT8                               Gpio92;
    SETUP_DATA                          SetupData;
    EFI_GUID                            SetupGuid= SETUP_GUID;
    EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
    UINTN                               VariableSize;
    UINT8                               Index;
    
    Status = (*PeiServices)->LocatePpi(
        (CPM_PEI_SERVICES**)PeiServices,
        &gAmdCpmTablePpiGuid,
        0,
        NULL,
        (VOID**)&CpmTablePpiPtr
        );

	if (EFI_ERROR(Status)) {
		return Status;
	}

	DxioTopologyTableS0Ptr = CpmTablePpiPtr->CommonFunction.GetTablePtr2(CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);

	//
	Gpio88 = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 88) ? 1 : 0;
	Gpio90 = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 90) ? 1 : 0;
	Gpio91 = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 91) ? 1 : 0;
	Gpio115 = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 115) ? 1 : 0;
	Gpio92 = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 92) ? 1 : 0;

	if (Gpio88)
	{
		//set GPIO76 to high
		CpmSmnSetGpio(CpmTablePpiPtr, 0, 0, 76, 1);
	}

	GpioMatrix = (Gpio90 << 2) + (Gpio91 << 1) + Gpio88;

	DEBUG((DEBUG_ERROR, "PeiConfigDxioSettings GpioMatrix=%d, Gpio92=%d\n", GpioMatrix, Gpio92));
	//// GpioMatrix = 
	////   0 : PHY A: 0~3 4x1 SATA, 4~5 I350, 6 BMC, 7 PCIE x1 slot, 8~15 1x8 PCIE
	////   2 : PHY A: 0~3 4x1 SATA, 4~7 1x4 PCIE, 8~15 1x8 PCIE
	////   3 : PHY A: 
	////			if Gpio92 == 1
	////				0~3 4x1 SATA, 4~7 4x1 XGBE, 8~15 1x8 PCIE
	////   4 : PHY A: 0~3 1x4 PCIE, 4~5 I356, 6 BMC, 7 PCIE x1 slot, 8~15 1x8 PCIE
	////   6 : PHY A: 
	////           if Gpio92 == 0 
	////              0~7 1x8 PCIE, 8~15 1x8 PCIE
	////           else
	////              0~15 1x16 PCIE
	////   7 : PHY A: 0~3 4x1 SATA, 4~7 4x1 XGBE, 8~15 1x8 PCIE
	////   if (GPIO115 == Low): 2x8 PCIE, otherwise is 1x16 PCIE

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
        
        TmpDxioTopologyTableS0Ptr = AllocatePool(sizeof(AMD_CPM_DXIO_TOPOLOGY_TABLE)/sizeof(UINT8));
         
         if(TmpDxioTopologyTableS0Ptr==NULL){
              return EFI_OUT_OF_RESOURCES;
          }
         
         CopyMem(TmpDxioTopologyTableS0Ptr, DxioTopologyTableS0Ptr, sizeof(AMD_CPM_DXIO_TOPOLOGY_TABLE)); 
        
         ZeroMem(DxioTopologyTableS0Ptr, sizeof(AMD_CPM_DXIO_TOPOLOGY_TABLE)); 
    
    switch (GpioMatrix)
    {
   case 0:
     CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
        //LANE 0~3 SATA
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
        //LANE 4~5 I350
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 5);
        //LANE 6 BMC
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 6, 6);
        //LANE 7 PCIE x1
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 7, 7);
        //LANE 8~15 PCIE x8
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);

		break;

    case 2:
       CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
        //LANE 0~3 SATA
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
        //LANE 4~7 PCIE
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 7);
        //LANE 8~15 PCIE
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);

		break;

    case 3:
        if (Gpio92 == 1) {
            
            
            if(SetupData.DieDxioType0Conf[0] == 0){
            

            CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
            //LANE 0~3 SATA
                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                    //LANE 4~5 I350
                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 5);
                    //LANE 6 BMC
                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 6, 6);
                    //LANE 7 PCIE x1
                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 7, 7);
                    //LANE 8~15 PCIE x8
                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
            }
           if(SetupData.DieDxioType0Conf[0] == 1){
             
               
               CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_b, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_b)/sizeof(UINT8));  
             //LANE 0~3 SATA
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
            //LANE 4~7 XGBE
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
            //LANE 8~15 PCIE x8
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
            }  
            
           if(SetupData.DieDxioType0Conf[0] == 2){
    
           CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_c, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_c)/sizeof(UINT8)); 
                         //LANE 0~3 SATA
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                        //LANE 4~7 XGBE
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                        //LANE 8~15 PCIE x8
                        DxioEnableTable(PeiServices, &DxioTopologyTableS0Ptr[2], DxioPcieEngine, 8, 15);
                        }  
            if(SetupData.DieDxioType0Conf[0] == 3){
                
                CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_d, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_d)/sizeof(UINT8)); 
                         //LANE 0~3 SATA
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                        //LANE 4~7 XGBE
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                        //LANE 8~15 PCIE x8
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                        }  
            if(SetupData.DieDxioType0Conf[0] == 4){
                
                CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_e, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_e)/sizeof(UINT8));
                         //LANE 0~3 SATA
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                        //LANE 4~7 XGBE
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                        //LANE 8~15 PCIE x8
                        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                        }  
            if(SetupData.DieDxioType0Conf[0] == 5){
                CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_f, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_f)/sizeof(UINT8));
                                     //LANE 0~3 SATA
                                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                                    //LANE 4~7 XGBE
                                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                                    //LANE 8~15 PCIE x8
                                    DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                                    } 
           
            break;
        }else{
            DEBUG((DEBUG_ERROR, "GpioMatrix 3 is invalid configuration."));
            ASSERT(FALSE);
        }

    case 4:
        CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
        //LANE 0~3 PCIE
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 3);
        //LANE 4~5 I350
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 5);
        //LANE 6 BMC
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 6, 6);
        //LANE 7 PCIE x1
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 7, 7);
        //LANE 8~15 PCIE x8
        DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);

		break;

    case 6:
        if (Gpio92 == 0)
        {
            CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
            // 1x8 PCIE, 1x8 PCIE
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 7); 
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
        }
        else
        {
            CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
            // 1x16 PCIE
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 15);
        }
        break;

    case 7:
        if(SetupData.DieDxioType0Conf[0] == 0){
                  
                
                  CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_a, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_a)/sizeof(UINT8));
              
                  //LANE 0~3 SATA
                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                          //LANE 4~5 I350
                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 5);
                          //LANE 6 BMC
                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 6, 6);
                          //LANE 7 PCIE x1
                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 7, 7);
                          //LANE 8~15 PCIE x8
                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                  }
                 if(SetupData.DieDxioType0Conf[0] == 1){
                    
                     CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_b, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_b)/sizeof(UINT8));  
                   //LANE 0~3 SATA
                  DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                  //LANE 4~7 XGBE
                  DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                  DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                  DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                  DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                  //LANE 8~15 PCIE x8
                  DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                  }  
                  
                 if(SetupData.DieDxioType0Conf[0] == 2){
          
                 CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_c, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_c)/sizeof(UINT8)); 
                               //LANE 0~3 SATA
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                              //LANE 4~7 XGBE
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                              //LANE 8~15 PCIE x8
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                              }  
                  if(SetupData.DieDxioType0Conf[0] == 3){
                      
                      CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_d, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_d)/sizeof(UINT8)); 
                               //LANE 0~3 SATA
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                              //LANE 4~7 XGBE
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                              //LANE 8~15 PCIE x8
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                              }  
                  if(SetupData.DieDxioType0Conf[0] == 4){
                      
                      CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_e, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_e)/sizeof(UINT8));
                               //LANE 0~3 SATA
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                              //LANE 4~7 XGBE
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                              //LANE 8~15 PCIE x8
                              DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                              }  
                  if(SetupData.DieDxioType0Conf[0] == 5){
                      CopyMem(DxioTopologyTableS0Ptr, &gCpmDxioTopologyTableHONGHAIA1bS0_f, sizeof(gCpmDxioTopologyTableHONGHAIA1bS0_f)/sizeof(UINT8));
                                           //LANE 0~3 SATA
                                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);
                                          //LANE 4~7 XGBE
                                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4);
                                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5);
                                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6);
                                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7);
                                          //LANE 8~15 PCIE x8
                                          DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);
                                          } 

		break;

	default:
		ASSERT(FALSE);
		break;
	}
	
	GpioMatrix = Gpio115;
	DEBUG((DEBUG_ERROR, "PeiConfigDxioSettings GpioMatrix2=%d\n", GpioMatrix));
	
	switch (GpioMatrix)
	{
		case 0:
			// LANE 16 ~23, 24~31: 2x8 PCIE
			DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 16, 23);
			DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 24, 31);
			break;

        case 1:
            
            // LANE 16~31
            DxioEnableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 16, 31);
            break;

        default:
            ASSERT(FALSE);
            break;
    }
  //  Status = XgbeUpdateDxioTable(PeiServices,XgbeInfo);
  //  XgbeInfo = AllocatePool(sizeof(XGBE_INFO));
   //    ZeroMem(XgbeInfo, sizeof(XGBE_INFO)); 
    for ( Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++ ) {
       //EthernetEngine find?
       if ( TmpDxioTopologyTableS0Ptr->Port[Index].EngineData.EngineType == DxioEthernetEngine ) { 
           //XgbeDie=TmpDxioTopologyTableS0Ptr->Port[Index].EngineData.StartLane/32;
           //XgbePort=TmpDxioTopologyTableS0Ptr->Port[Index].EtherNet.EthPortProp0.PortNum;
           //XgbeInfo =TmpDxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressLo
           //Status=XgbeGetMacAddress(PeiServices, XgbeInfo, XgbeDie,XgbePort,&MacAddressLo,&MacAddressHi);
           if(!EFI_ERROR(Status)){
               DxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressLo = TmpDxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressLo;
               DxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressHi = TmpDxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressHi;
               //check MAC address
			   DEBUG((EFI_D_ERROR,"Macaddresslo_af == %x \n", DxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressLo));
               DEBUG((EFI_D_ERROR,"MacaddressHi_af == %x \n", DxioTopologyTableS0Ptr->Port[Index].EtherNet.MacAddressHi));
           }
       }   
       
     } 
    
    
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
AutoDxioConfigurationInit(
IN       EFI_PEI_FILE_HANDLE   FileHandle,
IN CONST EFI_PEI_SERVICES     **PeiServices)
{
	EFI_STATUS    Status = EFI_SUCCESS;

	(*PeiServices)->NotifyPpi(PeiServices, PeiNotifyList);

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
