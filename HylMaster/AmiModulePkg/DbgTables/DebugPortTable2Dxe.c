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
#include "DebugPortTable2.h"
#include <Protocol\AcpiSupport.h>

#if	DBG2_SUPPORT
//---------------------------------------------------------------------------
// Constant, Macro and Type Definition(s)
//---------------------------------------------------------------------------
// Constant Definition(s)

// Macro Definition(s)

// Type Definition(s)

// Function Prototype(s)
VOID* SetupDebugDevideInformationData(DBG_PORT_2_TABLE *pDbg2Table);

//---------------------------------------------------------------------------
// Variable and External Declaration(s)
//---------------------------------------------------------------------------
// Variable Declaration(s)
static VOID		*gDebugPortTable2;

// GUID Definition(s)

// Protocol Definition(s)

// External Declaration(s)
extern EFI_ACPI_SUPPORT_PROTOCOL	*gAcpiSupport;
extern EFI_BOOT_SERVICES    *pBS;
extern UINT8    ACPI_OEM_ID[6];     //"A M I";      //add 0 at the end.OemId 6 bytes
extern UINT8    ACPI_OEM_TBL_ID[8]; //"ALASKA"; //add 0 at the end.OemTableId  8 bytes
extern EFI_GUID gConOutStartedGuid;
// Function Definition(s)

//---------------------------------------------------------------------------     

/**
    Setting the default DBGP ACPI header data.

	@retval	EFI_SUCCESS		The constructor always returns EFI_SUCCESS.
**/
EFI_STATUS SetupDbg2HeaderData(DBG_PORT_2_TABLE	*DbgPortTable2)
{
	UINTN		Index;

	DbgPortTable2->AcpiHeader.Signature = DBG2_SIG;
	DbgPortTable2->AcpiHeader.Length = sizeof(DBG_PORT_2_TABLE);
	DbgPortTable2->AcpiHeader.Revision = 0x0;
	DbgPortTable2->AcpiHeader.Checksum = 0x00;
	DbgPortTable2->AcpiHeader.OemRev = ACPI_OEM_REV;
	DbgPortTable2->AcpiHeader.CreatorId = CREATOR_ID_MS;
	DbgPortTable2->AcpiHeader.CreatorRev = CREATOR_REV_MS;

	//Update OemdID & OemTableID
    for(Index=0;Index<6;Index++) 
		((UINT8 *)(DbgPortTable2->AcpiHeader.OemId))[Index]=ACPI_OEM_ID[Index];
    for(Index=0;Index<8;Index++)
		((UINT8 *)(DbgPortTable2->AcpiHeader.OemTblId))[Index]=ACPI_OEM_TBL_ID[Index];

	return EFI_SUCCESS;	
}

/**
    Event feature, publish DBGP table at ready to boot process.

**/
VOID Dbg2TableInstallCallback(
	IN EFI_EVENT Event,
	IN VOID *Context
	)
{
	EFI_STATUS	Status;
	UINTN		TblHandle = 0;
	DBG_PORT_2_TABLE	*DebugPortTable2;
	
	Status = pBS->AllocatePool (EfiBootServicesData, sizeof(DBG_PORT_2_TABLE), &DebugPortTable2);
	ASSERT_EFI_ERROR (Status);	
	if (EFI_ERROR (Status)) return;
	MemSet((VOID*)DebugPortTable2,sizeof(DBG_PORT_2_TABLE), 0);
	
	//Fill ACPI Header
	Status = SetupDbg2HeaderData(DebugPortTable2);
	
	//Fill Debug Device information 
	gDebugPortTable2 = SetupDebugDevideInformationData(DebugPortTable2);
	pBS->FreePool((VOID*)DebugPortTable2);
	
	if(gAcpiSupport == NULL)
		return;
	
	Status = gAcpiSupport->SetAcpiTable (gAcpiSupport,
								gDebugPortTable2, TRUE, 
								EFI_ACPI_TABLE_VERSION_ALL,
								&TblHandle
								);
	TRACE((-1,"Dbg : Install DBG2 tbl %r \n", Status));
	ASSERT_EFI_ERROR(Status);
	
	pBS->FreePool(gDebugPortTable2);
	
	return;
}

/**
    Entry point of the DBG2 driver. Locate ACPI support protocol.
    Call SetupAcpiHeaderData and filled Interface type field and Base Address field. 
	
	@retval	EFI_SUCCESS		DBGP table install successfully.
**/
EFI_STATUS DebugPortTable2DxeEntryPoint(VOID)
{
	EFI_STATUS	Status = EFI_UNSUPPORTED;
	EFI_EVENT	Event;
	VOID		*Registration;
	
	//Add Table
	//Add DBGP Table at ready to boot process.
   	Status = RegisterProtocolCallback(
				&gConOutStartedGuid,
				Dbg2TableInstallCallback,
				NULL,   // Context
				&Event,
				&Registration
				);
	ASSERT_EFI_ERROR(Status);
	if(EFI_ERROR(Status))
		return Status;
	
	return Status;
}
#endif
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
