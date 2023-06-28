//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************

/** @file DebugPortTableDxe.c
	Entry point for Debug Port Table module initialization.

**/

//---------------------------------------------------------------------------
// Include(s)
//---------------------------------------------------------------------------
#include <AmiDxeLib.h>
#include "DebugPortTable.h"
#include <Protocol\AcpiSupport.h>
#include <Protocol\ConsoleControl.h>
#include <Setup.h>

//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)
UINT8    ACPI_OEM_ID[6]     = CONVERT_TO_STRING(T_ACPI_OEM_ID);     //"A M I";      //add 0 at the end.OemId 6 bytes
UINT8    ACPI_OEM_TBL_ID[8] = CONVERT_TO_STRING(T_ACPI_OEM_TBL_ID); //"ALASKA"; //add 0 at the end.OemTableId  8 bytes

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)


//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
static DBG_PORT_TABLE	gDebugPortTable;
EFI_ACPI_SUPPORT_PROTOCOL	*gAcpiSupport = NULL;
// GUID Definition(s)

EFI_GUID gAcpiSupportGuid = EFI_ACPI_SUPPORT_GUID;
EFI_GUID gConOutStartedGuid = CONSOLE_OUT_DEVICES_STARTED_PROTOCOL_GUID;
// Protocol Definition(s)

// External Declaration(s)
#if	DBG2_SUPPORT	
extern EFI_STATUS DebugPortTable2DxeEntryPoint(VOID);
#endif
// Function Definition(s)

//---------------------------------------------------------------------------     

/**
    Setting the default DBGP ACPI header data.

**/
VOID SetupDbgpHeaderData(VOID)
{
	UINTN		Index;

	gDebugPortTable.AcpiHeader.Signature = DEBUG_PORT_SIG;
	gDebugPortTable.AcpiHeader.Length = sizeof(DBG_PORT_TABLE);
	gDebugPortTable.AcpiHeader.Revision = 0x1;
	gDebugPortTable.AcpiHeader.Checksum = 0x00;
	gDebugPortTable.AcpiHeader.OemRev = ACPI_OEM_REV;
	gDebugPortTable.AcpiHeader.CreatorId = CREATOR_ID_MS;
	gDebugPortTable.AcpiHeader.CreatorRev = CREATOR_REV_MS;

	//Update OemdID & OemTableID
    for(Index=0;Index<6;Index++) 
		((UINT8 *)(gDebugPortTable.AcpiHeader.OemId))[Index]=ACPI_OEM_ID[Index];
    for(Index=0;Index<8;Index++)
		((UINT8 *)(gDebugPortTable.AcpiHeader.OemTblId))[Index]=ACPI_OEM_TBL_ID[Index];
}

/**
    Event feature, publish DBGP table at ready to boot process.

**/
VOID DbgpTableInstallCallback(
	IN EFI_EVENT Event,
	IN VOID *Context
	)
{
	EFI_STATUS	Status;
	UINTN		TblHandle = 0;
	
	Status = gAcpiSupport->SetAcpiTable (gAcpiSupport,
								&gDebugPortTable, TRUE, 
								EFI_ACPI_TABLE_VERSION_ALL,
								&TblHandle
								);
	TRACE((-1,"Dbg : Install DBGP tbl %r \n", Status));
	ASSERT_EFI_ERROR(Status);
	
	return;
}

/**
    Entry point of the DBGP driver. Locate ACPI support protocol.
    Call SetupAcpiHeaderData and filled Interface type field and Base Address field. 

	@param	ImageHandle		The firmware allocated handle for the EFI image.
	@param	SystemTable		A pointer to the EFI System Table.
	
	@retval	EFI_SUCCESS		DBGP table install successfully.
**/
EFI_STATUS DebugPortTableDxeEntryPoint(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS	Status = EFI_UNSUPPORTED;
	EFI_EVENT	Event = NULL;
	VOID		*Registration = NULL;
#if ACPI_SUPPORT && ACPI_BUILD_TABLES_2_0
	UINTN		AsptTblHandle = 0;
	static EFI_GUID SetupVariableGuid = SETUP_GUID;
	SETUP_DATA	SetupData;
	UINTN	Size = sizeof(SETUP_DATA);

	InitAmiLib(ImageHandle,SystemTable);

	Status = pBS->LocateProtocol (&gAcpiSupportGuid, NULL, &gAcpiSupport);
	if (EFI_ERROR (Status)) return Status;

	Status = pRS->GetVariable(L"Setup", &SetupVariableGuid, NULL, &Size, &SetupData);
	if(EFI_ERROR(Status))
		return	Status;;
	
#if	DBGP_SUPPORT
	if(SetupData.DBGPEnable){
		//Fill ACPI Header
		SetupDbgpHeaderData();
		
		//Fill DBGP fields
		gDebugPortTable.InterfaceType = DBGP_INTERFACE_TYPE;
		gDebugPortTable.BaseAddress.AddrSpcID = DBGP_ADDR_SPC_ID;
		gDebugPortTable.BaseAddress.RegBitWidth = DBGP_ADDR_BIT_WIDTH;
		gDebugPortTable.BaseAddress.RegBitOffs = DBGP_ADDR_BIT_OFFSET;
		gDebugPortTable.BaseAddress.AccessSize = DBGP_ACCESS_SIZE;
		gDebugPortTable.BaseAddress.Address = PcdGet64 (PcdSerialRegisterBase);
		
		TRACE((-1,"Dbg1 : DBGP signature [%lx] \n",gDebugPortTable.AcpiHeader.Signature));
		//Add DBGP Table at ready to boot process.
		Status = RegisterProtocolCallback(
					&gConOutStartedGuid,
					DbgpTableInstallCallback,
					NULL,   // Context
					&Event,
					&Registration
					);
		ASSERT_EFI_ERROR (Status);	
		if(EFI_ERROR(Status))
			return Status;
	}
#endif//#if	DBGP_SUPPORT
	
#if	DBG2_SUPPORT
	//Create DBG2 Table
	if(SetupData.DBG2Enable)
		Status = DebugPortTable2DxeEntryPoint();
#endif//#if	DBG2_SUPPORT	

#endif//#if ACPI_SUPPORT && ACPI_BUILD_TABLES_2_0
	return Status;
}

//***********************************************************************
//***********************************************************************
//**                                                                   **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.          **
//**                                                                   **
//**                       All Rights Reserved.                        **
//**                                                                   **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093         **
//**                                                                   **
//**                       Phone: (770)-246-8600                       **
//**                                                                   **
//***********************************************************************
//***********************************************************************
