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
DxioDisableTable(
EFI_PEI_SERVICES **PeiServices,
AMD_CPM_DXIO_TOPOLOGY_TABLE *DxioTopologyTablePtr,
UINT8 EngineType,
UINT8 StartLane,
UINT8 EndLane
)
{
	UINT8                               Index;
	DEBUG((DEBUG_ERROR, "AutoDxioConfiguration Start: StartLane=%d EndLane=%d, EngineType=%d\n", StartLane, EndLane, EngineType));
	for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index++) {

		if (DxioTopologyTablePtr->Port[Index].EngineData.StartLane == StartLane &&
			DxioTopologyTablePtr->Port[Index].EngineData.EndLane == EndLane &&
			DxioTopologyTablePtr->Port[Index].EngineData.EngineType == EngineType)
		{
			DxioTopologyTablePtr->Port[Index].EngineData.EngineType = DxioUnusedEngine;
			return EFI_SUCCESS;
		}
	}
	DEBUG((DEBUG_ERROR, "AutoDxioConfiguration end\n"));
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
	UINT8                               Gpio88, Gpio90, Gpio91, Gpio115;
	UINT8                               GpioMatrix;
	UINT8                               SlotARiserPresent;

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
	SlotARiserPresent = CpmSmnGetGpio(CpmTablePpiPtr, 0, 0, 92) ? 1 : 0;

	if (Gpio88)
	{
		//set GPIO76 to high
		CpmSmnSetGpio(CpmTablePpiPtr, 0, 0, 76, 1);
	}

	GpioMatrix = (Gpio90 << 2) + (Gpio91 << 1) + Gpio88;

	DEBUG((DEBUG_ERROR, "PeiConfigDxioSettings GpioMatrix=%d, SlotARiserPresent=%d\n", GpioMatrix, SlotARiserPresent));
	//// GpioMatrix = 
	////   0 : PHY A: 0~3 4x1 PCIE, 4~7 4x1 SATA, 8~15 1x8 PCIE
	////   2 : PHY A: 0~3 4x1 PCIE, 4~7 1x4 PCIE, 8~15 1x8 PCIE
	////   4 : PHY A: 0~3 1x4 PCIE, 4~7 4x1 SATA, 8~15 1x8 PCIE
	////   6 : PHY A: 
	////           if SlotA_Riser_PRSNT_N==0
	////              0~7 1x8 PCIE, 8~15 1x8 PCIE
	////           else
	////              0~15 1x16 PCIE
	////   7 : PHY A: 0~3 1x4 SATA, 4~7 4x1 XGBE, 8~15 1x8 PCIE
	////   if (GPIO115 == Low): 2x8 PCIE, otherwise is 1x16 PCIE

	switch (GpioMatrix)
	{
	case 0:
		// 4x1 PCIE, 4x1 SATA, 1x8 PCIE
		//LANE 0~3
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);     //disable 4x1 Sata
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 3);    //disable 1x4 PCIE

		//LANE 4~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 7);    //disable 1x4 PCIE

		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7); //disable 4x1 XGBE

		//LANE 0~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 7);   //disable 1x8 PCIE

		//LANE 8~15

		//LANE 0~15
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 15);  //disable 1x16 PCIE
		break;

	case 2:
		// 4x1 PCIE, 1x4 PCIE, 1x8 PCIE
		//LANE 0~3
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);   //disable 4x1 Sata
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 3);   //disable 1x4 PCIE

		//LANE 4~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4, 7);    //disable 4x1 SATA

		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7); //disable 4x1 XGBE

		//LANE 0~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 7);   //disable 1x8 PCIE

		//LANE 8~15

		//LANE 0~15
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 15);  //disable 1x16 PCIE
		break;

	case 4:
		// 1x4 PCIE, 4x1 SATA, 1x8 PCIE
		//LANE 0~3
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);    //disable 4x1 Sata
		//DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 0);   //disable 1x1 PCIE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 1);   //disable 1x2 PCIE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 2, 2);   //disable 1x1 PCIE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 3, 3);   //disable 1x1 PCIE

		//LANE 4~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 7);    //disable 1x4 PCIE

		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6); //disable 4x1 XGBE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7); //disable 4x1 XGBE

		//LANE 0~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 7);   //disable 1x8 PCIE

		//LANE 8~15

		//LANE 0~15
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 15);  //disable 1x16 PCIE
		break;

	case 6:
		if (SlotARiserPresent == 0)
		{
			// 1x8 PCIE, 1x8 PCIE
			//LANE 0~3
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);    //disable 4x1 Sata
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 3);   //disable 1x4 PCIE

			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 1);   //disable 1x2 PCIE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 2, 2);   //disable 1x1 PCIE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 3, 3);   //disable 1x1 PCIE

			//LANE 4~7
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 7);    //disable 1x4 PCIE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4, 7);     //disable 4x1 SATA

			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4); //disable 4x1 XGBE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5); //disable 4x1 XGBE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6); //disable 4x1 XGBE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7); //disable 4x1 XGBE

			//LANE 8~15

			//LANE 0~15
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 15);  //disable 1x16 PCIE


		}
		else
		{
			// 1x16 PCIE
			//LANE 0~3
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 0, 3);    //disable 4x1 Sata
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 3);   //disable 1x4 PCIE

			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 1);   //disable 1x2 PCIE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 2, 2);   //disable 1x1 PCIE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 3, 3);   //disable 1x1 PCIE

			//LANE 4~7
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 7);    //disable 1x4 PCIE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4, 7);     //disable 4x1 SATA

			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 4, 4); //disable 4x1 XGBE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 5, 5); //disable 4x1 XGBE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 6, 6); //disable 4x1 XGBE
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioEthernetEngine, 7, 7); //disable 4x1 XGBE

			//LANE 0~7
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 7);  //disable 1x8 PCIE

			//LANE 8~15
			DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 8, 15);  //disable 1x8 PCIE
		}
		break;

	case 7:
		// 4x1 SATA, 4x1 XGBE, 1x8 PCIE
		//LANE 0~3
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 3);   //disable 1x4 PCIE

		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 1);   //disable 1x2 PCIE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 2, 2);   //disable 1x1 PCIE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 3, 3);   //disable 1x1 PCIE

		//LANE 4~7
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 4, 7);    //disable 1x4 PCIE
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioSATAEngine, 4, 7);     //disable 4x1 SATA
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 7);    //disable 1x8 PCIE

		//LANE 8~15

		//LANE 0~15
		DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 0, 15);  //disable 1x16 PCIE
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
	    DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 16, 31);  //disable 1x16 PCIE
	    break;

	case 1:
	    // LANE 16~31
	    DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 16, 23);  //disable 2x8 PCIE
	    DxioDisableTable(PeiServices, DxioTopologyTableS0Ptr, DxioPcieEngine, 24, 31);  //disable 2x8 PCIE
	    break;

	default:
	    ASSERT(FALSE);
	    break;
	}
	return EFI_SUCCESS;
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
