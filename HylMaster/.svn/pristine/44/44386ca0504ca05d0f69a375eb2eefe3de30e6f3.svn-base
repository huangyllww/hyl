//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2017, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  			**
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************

/** @file 
PldmPreserveImport.c

Responsible for reading Pldm table and import it

*/

//************************************************************************
//					Include Section
//************************************************************************


#include "AmiDxeLib.h"
#include "PldmPreserve.h"
#include "Token.h"

AMIPLDM_PRESERVE_INTERFACE_PROTOCOL gAmiPreservePldmInterfaceProtocol;
extern EFI_STATUS
CreatePldmTables(
    UINT8  *,       // pointer to HII export data
    UINT32  ,       // size of Hii Data
    UINT8 **,   // return string table pointer
    UINT32 *,    // return string table size
    UINT8 **,// return attribute table pointer
    UINT32 *, // return attribute table size
    UINT8 **,    // return value table pointer
    UINT32 *,     // return value size
    UINT8 **,// return map string table pointer
    UINT32 *, // return map string table size
    BOOLEAN        // save unreferenced variables
);
extern void SetPreserveMode(UINTN Mode) ;
//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure    : PreservingCreatePldmTables
//
// Description  : Creates the Pldm Tables using current Hii data and returns the Tables(access allsetup value or modified value, depending requirement)
//
// Paramaters   : PLDMTables *PldmTables, BOOLEAN ExportVariables, void *Buffer, UINNT *BufferSize, UINTN Operation
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PreservingCreatePldmTables (PresrvePLDMTables *PldmTables, BOOLEAN ExportVariables, void *Buffer, UINTN *BufferSize, UINTN Operation)
{
   EFI_STATUS Status = EFI_SUCCESS;
   UINT8 *HIIData = (UINT8 *)NULL;
   UINT8 *StringTable = (UINT8 *)NULL;
   UINT8 *AttributeTable = (UINT8 *)NULL;
   UINT8 *ValueTable = (UINT8 *)NULL;
   UINT8 *MapStringTable = (UINT8 *)NULL;
   UINT32 StringSize = 0;
   UINT32 AttributeSize = 0;
   UINT32 ValueSize = 0;
   UINT32 MapSize = 0;
   UINT32 CRC32 = 0;
   UINTN DBSize = 0;
//if the bit is set include all controls
   //Get the HII Data and store in a buffer.
   Status = AMI_RetrieveHIIData( &HIIData, &DBSize, &CRC32 );

   if(EFI_ERROR(Status))
   {
       return Status;
   }
   if(( Operation & BCP_ALL ) == BCP_ALL)
   {
       SetPreserveMode(BCP_ALL) ;
       DEBUG((DEBUG_ERROR, "\n PldmCreatePldmTables: BCP_ALL"));
   }
   else
   {
       SetPreserveMode(BCP_ONLY_DEFAULTS) ;
       DEBUG((DEBUG_ERROR, "\n PldmCreatePldmTables: BCP_ONLY_DEFAULTS"));
   }
   
   //Create the PLDM Tables from the HII data
   Status = CreatePldmTables( 
                HIIData,
                (UINT32)DBSize,
                &StringTable,
                &StringSize,    
                &AttributeTable, 
                &AttributeSize,  
                &ValueTable,     
                &ValueSize,      
                &MapStringTable,
                &MapSize,
                ExportVariables
                );
   DEBUG((DEBUG_ERROR, "\n PreservePldmCreatePldmTables: CreatePldmTables status:%x",Status));
   if(EFI_ERROR(Status))
   {
       return Status;
   }
   
   //Assign the tables value to the PldmTables structure
   PldmTables->StringTable = StringTable;
   PldmTables->StringSize = StringSize;
   PldmTables->AttributeTable = AttributeTable;
   PldmTables->AttributeSize = AttributeSize;
   PldmTables->ValueTable = ValueTable;
   PldmTables->ValueSize = ValueSize;
   PldmTables->MapStringTable = MapStringTable;
   PldmTables->MapSize = MapSize;

   return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure    : PreservingImportPldmTables
//
// Description  : Imports the pldm data to the Hii
//
// Paramaters   : PLDMTables *PldmTables, void *Buffer, UINTN *BufferSize, UINTN Operation
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PreservingImportPldmTables (PresrvePLDMTables *PldmTables, void *Buffer, UINTN *BufferSize, UINTN Operation)
{
    EFI_STATUS Status = EFI_UNSUPPORTED;
    
    // yet to implement
    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure	: 	InstallPreserveInterface
//
// Description :  Install protocol to support preserve
//
// Parameter : 	EFI_HANDLE ImageHandle
//
// Return Value: EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InstallPreserveInterface(IN EFI_HANDLE ImageHandle)
{
	EFI_STATUS Status = EFI_SUCCESS;
	
	gAmiPreservePldmInterfaceProtocol.CreatePldm = PreservingCreatePldmTables;
	gAmiPreservePldmInterfaceProtocol.ImportPldm = PreservingImportPldmTables;

	Status = gBS->InstallMultipleProtocolInterfaces(&ImageHandle, &gAmiPreservePldmInterfaceProtocolGuid, (void *)&gAmiPreservePldmInterfaceProtocol, NULL);
	DEBUG((DEBUG_ERROR, "\n InstallPreserveInterface: InstallInterface status:%r",Status));
	return Status;
}


//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2015, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************
