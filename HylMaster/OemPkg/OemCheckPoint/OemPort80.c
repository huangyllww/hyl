/** @file OemPort80.c
    Function file for Tse hook functionalities.

**/

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include "Token.h"
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include "OemPort80.h"

//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern CHECKPOINT_FUNCTION* CheckpointList[];
//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------
UINT8 FindByteCode(STATUS_CODE_TO_BYTE_MAP *Map, EFI_STATUS_CODE_VALUE Value);
//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------
//**********************************************************************
// Status Code Library (functions used throughout the module)
//**********************************************************************
/**
    Match "Value" to Map->Value and return Map->Byte.

        
    @param Map Pointer to a table (array of structures) that maps status code value (EFI_STATUS_CODE_VALUE) to a byte (UINT8) domain specific value.
    @param Value status code to map

         
    @retval UINT8 domain specific mapping of the Value

**/
VOID OemPort80Checkpoint(IN VOID *PeiServices,IN UINT16 Checkpoint)
{
    UINT8  DataValue;
    
    DataValue = PcdGet8(PcdPostCodeDebug);
    if(DataValue == 1) {
        IoWrite16 (0x80, (UINT16)(Checkpoint));
    }else{
        IoWrite8 (0x80, (UINT8)(Checkpoint&0xFF));
    }
}
/**
    Calls checkpoint status code handlers registered under ReportCheckpoint eLink.

        
    @param PeiServices pointer to Pei Services
    @param Checkpoint checkpoint

    @retval VOID

**/
VOID OemCheckpointStatusReport(IN VOID *PeiServices, IN UINT16 Checkpoint)
{
    UINTN i;
    
    for (i=0; CheckpointList[i]; i++) CheckpointList[i](PeiServices,Checkpoint);
}
/**
    Looks up the checkpoint for the given status code Type
    and Value and calls CheckpointStatusReport function to invoke all checkpoint handlers.

        
    @param PeiServices pointer to the PEI Boot Services table
    @param Type the type and severity of the error that occurred
    @param Value the Class, subclass and Operation that caused the error

    @retval 
        EFI_STATUS

**/
//<AMI_PHDR_END>
EFI_STATUS OemCheckPointStatus(    
    IN VOID *PeiServices,
    IN EFI_STATUS_CODE_TYPE Type, IN EFI_STATUS_CODE_VALUE Value
)
{

    UINT8 n;
    UINT8  DataValue;
    UINT16 Checkpoint16;
    
    n=FindByteCode(OemCheckPointStatusCodes,Value);
    if (n>0) {
        Checkpoint16 = (Type & 0xFF00) | n;
        DataValue = PcdGet8(PcdPostCodeDebug);
        if(DataValue == 1) {
            DEBUG ((DEBUG_INFO, "[OemDbg]CheckPoint = %02X(%02X)\n",Checkpoint16&0xFF,Checkpoint16>>8));
        }
        OemCheckpointStatusReport(PeiServices,Checkpoint16);
    }
    return EFI_SUCCESS;
}

