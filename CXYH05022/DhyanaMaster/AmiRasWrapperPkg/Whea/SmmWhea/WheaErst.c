//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file
  Provides OS the interface to write read and clear dummy write
  operation for storing and retrieving  hardware error record
  to and from a persistent storage.
*/

#include "WheaErst.h"

extern WHEA_MODULE_PRIVATE_DATA  *mWheaData;

// Including Flash Protocol for using flash services
//
FLASH_PROTOCOL   *Flash = NULL;
FLASH_PROTOCOL   *Flash;

WHEA_ERST_REGISTERS       *mErstReg;

UINT8                     *mPersistentStoreBufferPtr;
UINT32                    mFreeSpaceInPersistentStore;
UINT16                    mTotalErrorRecord = 0;
EFI_SMM_BASE2_PROTOCOL    *mSmmBase2;

UEFI_ERROR_RECORD_HEADER  *mElarPtr;

WHEA_ERST_ACPI_TABLE ErstAcpiTable = {
  //
  //AcpiHeader ,OEMTABLEID -AMI ERST 
  //
  {ERST_SIG,sizeof(WHEA_ERST_ACPI_TABLE),1,0,"AMIER",0x545352452E494D41,0,0x2E494D41,0},

  //WheaSerialHeader
  //
  {12, 0, WHEA_ERST_INSTR_ENTRIES},

  //ErstBeginWriteEntry [0]
  //
  {
    {ERST_BEGIN_WRITE_OPERATION, ERST_WRITE_REGISTER_VALUE, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     ERST_BEGIN_WRITE_OPERATION, WHEA_BYTE_MASK   },

    //
    //ErstBeginReadEntry [1]
    //
    {ERST_BEGIN_READ_OPERATION, ERST_WRITE_REGISTER_VALUE, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM,  REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     ERST_BEGIN_READ_OPERATION, WHEA_BYTE_MASK },

    //
    //ErstBeginClearEntry [2]
    //
    {ERST_BEGIN_CLEAR_OPERATION, ERST_WRITE_REGISTER_VALUE, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     ERST_BEGIN_CLEAR_OPERATION, WHEA_BYTE_MASK  },

    //
    //ErstEndOperationEntry [3]
    //
    {ERST_END_OPERATION, ERST_WRITE_REGISTER_VALUE, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     ERST_END_OPERATION, WHEA_BYTE_MASK          },

    //
    //ErstSetRecOffsetEntry [4]
    //
    {ERST_SET_RECORD_OFFSET, ERST_WRITE_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_DWORD, REG_BIT_OFFSET0, WHEA_ACCESS_DWORD, 0,
     0x0, WHEA_DWORD_MASK                 },

    //
    //ErstExecOperEntry [5]
    //
    {ERST_EXECUTE_OPERATION, ERST_WRITE_REGISTER_VALUE, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSIO, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, AMI_SMIPORT,
     ERST_EXECUTE_OPERATION_CMD, WHEA_BYTE_MASK                     },

    //
    //ErstCheckBusyEntry [6]
    //
    {ERST_CHECK_BUSY_STATUS, ERST_READ_REGISTER_VALUE, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM,REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     ERST_BUSY_STS_VALUE, WHEA_BYTE_MASK         },

    //
    //ErstGetOperationStatusEntry [7]
    //
    {ERST_GET_OPERATION_STATUS, ERST_READ_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM,  REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     0, WHEA_BYTE_MASK                       },

    //
    //ErstGetRecordIdentifierEntry [8]
    //
    {ERST_GET_RECORD_IDENTIFIER, ERST_READ_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_QWORD, REG_BIT_OFFSET0, WHEA_ACCESS_QWORD, 0,
     0, WHEA_QWORD_MASK    },

    //
    //ErstSetRecordIdentifierEntry [9]
    //
    {ERST_SET_RECORD_IDENTIFIER, ERST_WRITE_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_QWORD, REG_BIT_OFFSET0, WHEA_ACCESS_QWORD, 0,
     0, WHEA_QWORD_MASK     },

    //
    //ErstGetRecordCountEntry [10]
    //
    {ERST_GET_RECORD_COUNT, ERST_READ_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED, 
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_WORD, REG_BIT_OFFSET0, WHEA_ACCESS_WORD, 0 ,
     0, WHEA_WORD_MASK              },

    //
    //ErstBeginDummeyWriteEntry [11]
    //
    {ERST_BEGIN_DUMMY_WRITE_OPERATION, ERST_WRITE_REGISTER_VALUE, 0x0, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
     ERST_BEGIN_DUMMY_WRITE_OPERATION, WHEA_BYTE_MASK       },

     //
    //Reserved [12]
    //
    {ERST_RESERVED, ERST_NOOP, WHEA_CLEAR_FLAG, WHEA_RESERVED,
    WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0,
    0, WHEA_BYTE_MASK    },

    //
    //ErstGetElar [13]
    //
    {ERST_GET_ERROR_LOG_ADDRESS_RANGE, ERST_READ_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED,
    WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_QWORD, REG_BIT_OFFSET0, WHEA_ACCESS_QWORD, 0,
    0, WHEA_QWORD_MASK    },

    //
    //ErstGetElarLength [14]
    //
    {ERST_GET_ERROR_LOG_ADDRESS_LENGTH, ERST_READ_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED,
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_DWORD, REG_BIT_OFFSET0, WHEA_ACCESS_DWORD, 0, 0, 0xffffffff         },

    //
    //ErstGetElarAttributes [15]
    //
    {ERST_GET_ERROR_LOG_ADDRESS_ATTR, ERST_READ_REGISTER, WHEA_CLEAR_FLAG, WHEA_RESERVED, 
     WHEA_ENTRY_ADDRID_SYSMEM, REG_BIT_WIDTH_BYTE, REG_BIT_OFFSET0, WHEA_ACCESS_BYTE, 0, 
     0, WHEA_BYTE_MASK              },
  }
};

/**
  Initializes services required for Flash Functions

  @retval  EFI_STATUS
*/

EFI_STATUS
InitFlash (
)
{  
    BOOLEAN                 IsInSmm = FALSE;
    EFI_SMM_BASE2_PROTOCOL  *mSmmBase2Protocol;
    EFI_STATUS              Status;

    Status = gBS->LocateProtocol (
                        &gEfiSmmBase2ProtocolGuid,
                        NULL,
                        (VOID **) &mSmmBase2Protocol );

    if(!EFI_ERROR(Status)) {
        mSmmBase2Protocol->InSmm (mSmmBase2, &IsInSmm);
    } else {
        return EFI_NOT_FOUND;
    }
  
    if(IsInSmm) { 
        return gBS->LocateProtocol (
                        &gFlashSmmProtocolGuid,
                        NULL,
                        &Flash );

  } else {
    return gBS->LocateProtocol (
                        &gFlashProtocolGuid,
                        NULL,
                        &Flash );
  }
}

/**
  Creates Error Log Address Range to be used by ERST serial instruction and 
  Persistent Store Buffer.Update ERST Table, Copy ERST ACPI table to boot memory
  space and Update WHEA private data structure

  @retval  EFI_STATUS 
*/

EFI_STATUS 
WheaErstInit (
)
{
    EFI_STATUS              Status;
    PERSISTENT_ERROR_RECORD *PersistentErrorRecordPtr;
    WHEA_ERST_ACPI_TABLE    *ErstAcpiTablePtr;
    UINT64                  TempAddr;
    VOID                    *FvWheaBase;

    TempAddr = FV_WHEA_BASE;
    FvWheaBase = (VOID*)TempAddr;

    InitFlash();

    //Create Error Log Address Range to be used by ERST serial instruction
    //
    Status = gBS->AllocatePool (EfiReservedMemoryType,ELAR_SIZE + sizeof(WHEA_ERST_REGISTERS), &mElarPtr);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    WheaZeroMemory(mElarPtr, ELAR_SIZE + sizeof(WHEA_ERST_REGISTERS) );

    mErstReg = (WHEA_ERST_REGISTERS*)mElarPtr;
    mElarPtr = (UEFI_ERROR_RECORD_HEADER*)((UINT8*)mElarPtr + sizeof(WHEA_ERST_REGISTERS));

    //Create Persistent Store Buffer
    //
    Status = gBS->AllocatePool(EfiRuntimeServicesData,FV_WHEA_SIZE,&mPersistentStoreBufferPtr);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    WheaZeroMemory(mPersistentStoreBufferPtr, FV_WHEA_SIZE );

    //Update ERST Table
    //
    ErstAcpiTable.SerializationInstEntry[0].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->CmdReg);
    ErstAcpiTable.SerializationInstEntry[1].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->CmdReg);
    ErstAcpiTable.SerializationInstEntry[2].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->CmdReg);
    ErstAcpiTable.SerializationInstEntry[11].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->CmdReg);

    ErstAcpiTable.SerializationInstEntry[3].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->EndOperation);
    ErstAcpiTable.SerializationInstEntry[4].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->RecordOffset);

    ErstAcpiTable.SerializationInstEntry[6].RegisterRegion.Address = (UINT64)((UINTN)&mErstReg->BusyStatus);

    ErstAcpiTable.SerializationInstEntry[7].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->OperationStatus);
    ErstAcpiTable.SerializationInstEntry[8].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->ValidRecordIdentifier);
    ErstAcpiTable.SerializationInstEntry[9].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->OSRecordIdentifier);
    ErstAcpiTable.SerializationInstEntry[10].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->RecordCount);

    ErstAcpiTable.SerializationInstEntry[13].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->ElarAddress);
    ErstAcpiTable.SerializationInstEntry[14].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->ElarLength);
    ErstAcpiTable.SerializationInstEntry[15].RegisterRegion.Address= (UINT64)((UINTN)&mErstReg->ElarAttributes);

    mErstReg->ElarAddress= (UINT64)((UINTN)mElarPtr);
    mErstReg->ElarLength= ELAR_SIZE;
    mErstReg->ElarAttributes= 0;

    //Copy ERST ACPI table to boot memory space
    //
    Status = gBS->AllocatePool (EfiBootServicesData, sizeof(WHEA_ERST_ACPI_TABLE), &ErstAcpiTablePtr);
    if (EFI_ERROR(Status)) {
        return Status;
    }
    gBS->CopyMem(ErstAcpiTablePtr, &ErstAcpiTable, sizeof(WHEA_ERST_ACPI_TABLE));

    //Update WHEA private data structure
    //
    mWheaData->Erst= ErstAcpiTablePtr;
    mWheaData->ErrorLogAddressRange= mElarPtr;
    mWheaData->PersistentStoreBuffer= mPersistentStoreBufferPtr;
    mWheaData->ErstEnabled = TRUE;

    // Copy persistent store buffer. Update RecordCount , FirmRecordIdentifier
    // and mFreeSpaceInPersistentStore.
    // mFreeSpaceInPersistentStore will be updated by the end of this function
    //
    mFreeSpaceInPersistentStore= FV_WHEA_SIZE;
    mErstReg->RecordCount= 0;
    mErstReg->ValidRecordIdentifier= 0xffffffffffffffff;

    gBS->CopyMem(mPersistentStoreBufferPtr, FvWheaBase, FV_WHEA_SIZE);

    PersistentErrorRecordPtr= (PERSISTENT_ERROR_RECORD*)mPersistentStoreBufferPtr;

    if (PersistentErrorRecordPtr->NvPrependData.MagicNum == WHEA_PREPEND_MAGIC_NUMBER) {
        while ( (PersistentErrorRecordPtr->NvPrependData.MagicNum == WHEA_PREPEND_MAGIC_NUMBER)
              && ((UINT64)((UINTN)PersistentErrorRecordPtr) < (UINT64)((UINTN)mPersistentStoreBufferPtr+FV_WHEA_SIZE-PS_PREPEND_DATA_SIZE)))
        {

            if (PersistentErrorRecordPtr->NvPrependData.RecordValidity == FLASH_ERASE_VALUE) {
                mErstReg->ValidRecordIdentifier= PersistentErrorRecordPtr->UefiErrorRecord.RecordID;
                mErstReg->RecordCount= mErstReg->RecordCount+1;
            }

            mTotalErrorRecord= mTotalErrorRecord + 1;
            mFreeSpaceInPersistentStore= mFreeSpaceInPersistentStore-(PersistentErrorRecordPtr->UefiErrorRecord.RecLength+PS_PREPEND_DATA_SIZE+PS_APPEND_DATA_SIZE);
            PersistentErrorRecordPtr= (PERSISTENT_ERROR_RECORD*)((UINT8*)PersistentErrorRecordPtr +(PersistentErrorRecordPtr->UefiErrorRecord.RecLength+PS_PREPEND_DATA_SIZE+PS_APPEND_DATA_SIZE));
        }
    }

    return EFI_SUCCESS;
}

/**
  Find valid record
 
  @retval  FoundValidRecord ( 0 or 1 )
*/

UINT8
GetNextValidRecord (
  UINT16                   *NumOfRecords,
  PERSISTENT_ERROR_RECORD  *PersErrorRecordPtr,
  PERSISTENT_ERROR_RECORD  **PersErrorRecordPtr2 )
{
    UINT8 FoundValidRecord=0;

    for (*NumOfRecords=*NumOfRecords-1; *NumOfRecords>0; *NumOfRecords=*NumOfRecords-1) {

        //Modified to update the record pointer to point for the next record every time
        //
        PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE  + PS_APPEND_DATA_SIZE);

        if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {
            *PersErrorRecordPtr2 = PersErrorRecordPtr;
            FoundValidRecord= 1;
            break;
        }
        mTotalErrorRecord = mTotalErrorRecord - 1;
        mFreeSpaceInPersistentStore = mFreeSpaceInPersistentStore
                                 + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE;
    }

    return FoundValidRecord;
}

/**
  Deletes error record from persistent store

*/

VOID
DeleteMarkedErrorRecord ( VOID )
{
    PERSISTENT_ERROR_RECORD  *PersErrorRecordPtr;
    PERSISTENT_ERROR_RECORD  *PersErrorRecordPtr2;
    EFI_STATUS                Status;
    UINT16                    NumOfRecords;
    UINT8                     PersistentStoreModified=0;
    UINT64                    TempAddress;
    VOID                     *FvWheaBase;
    UINT32                    TotalRecToCopy,RecBefDel=0;

    TempAddress = FV_WHEA_BASE;
    FvWheaBase = (VOID*)TempAddress;

    PersErrorRecordPtr  = (PERSISTENT_ERROR_RECORD*)mPersistentStoreBufferPtr;

    for (NumOfRecords=mTotalErrorRecord; NumOfRecords>0; NumOfRecords--) {
        if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  MARKED_AS_DELETE) {
            PersistentStoreModified = 1;
            mTotalErrorRecord = mTotalErrorRecord - 1;
            mFreeSpaceInPersistentStore = mFreeSpaceInPersistentStore
                                   + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE;
            TotalRecToCopy = FV_WHEA_SIZE - mFreeSpaceInPersistentStore - RecBefDel;

            if (GetNextValidRecord(&NumOfRecords, PersErrorRecordPtr, &PersErrorRecordPtr2) == 1) {

                //Modfied TO COPY THE WHOLE RECORD AREA
                MemCpy(PersErrorRecordPtr, PersErrorRecordPtr2, TotalRecToCopy);
                PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE  + PS_APPEND_DATA_SIZE);
            }
            break;
        }
        RecBefDel = RecBefDel + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE;
        PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
    }

    if (PersistentStoreModified == 1) {
        if ( mFreeSpaceInPersistentStore <= FV_WHEA_SIZE ) {

            // Erasing block from base upto whea size
            //
            Status = Flash->Erase((UINT8*) FV_WHEA_BASE,(UINTN)FV_WHEA_SIZE);

            // Writing record only if Erase is successful
            //
            if( Status == EFI_SUCCESS ) {

                // Checking if record count register is not having zero and free space variable does not exceed FV_WHEA_SIZE
                //
                if ( mErstReg->RecordCount != 0 ) {
                    Flash->Write((UINT8*)FV_WHEA_BASE, FV_WHEA_SIZE - mFreeSpaceInPersistentStore ,(UINT8*)mPersistentStoreBufferPtr);
                }
            }
        }
    }
}

/**
  Searches the persistent store for given record id.If found then
  mark the error record as cleared.Subtract RecordCount by 1, if
  GetRecordId is pointing to SetRecordId, then make sure GetRecordId
  points to a valid error record

  @param  RecordId

  @retval Command Status
*/

UINT8
ClearErrorRecord (
  IN  UINT64 RecordId 
  )
{
    PERSISTENT_ERROR_RECORD *PersErrorRecordPtr;
    EFI_STATUS               Status;
    UINT16                   NumOfRecords;
    UINT16                   TmpNumOfRecords;
    UINT8                    CommandStatus = ERST_RECORD_NOT_FOUND;

    UINT64                   TempAddress;
    VOID                    *FvWheaBase;

    TempAddress = FV_WHEA_BASE;
    FvWheaBase = (VOID*)TempAddress;

    PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)mPersistentStoreBufferPtr;

    for (NumOfRecords=mTotalErrorRecord;NumOfRecords>0;NumOfRecords--) {
        if (PersErrorRecordPtr->UefiErrorRecord.RecordID == RecordId && PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {
            mErstReg->RecordCount = mErstReg->RecordCount-1;
            PersErrorRecordPtr->NvPrependData.RecordValidity = MARKED_AS_DELETE;
            Status = Flash->Write((UINT8*)FvWheaBase+ ((UINT64)(UINTN)&PersErrorRecordPtr->NvPrependData.RecordValidity) - (UINT64)((UINTN)mPersistentStoreBufferPtr),1, (UINT8*)(&PersErrorRecordPtr->NvPrependData.RecordValidity));


            if (RecordId == mErstReg->ValidRecordIdentifier) {
                if ( mErstReg->RecordCount == 0) {
                    mErstReg->ValidRecordIdentifier = 0xffffffffffffffff;
                } else {
                    PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
                    NumOfRecords = NumOfRecords-1;
                    TmpNumOfRecords = NumOfRecords;
                    for (;NumOfRecords>0;NumOfRecords--) {
                        if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {
                            mErstReg->ValidRecordIdentifier = PersErrorRecordPtr->UefiErrorRecord.RecordID;
                            break;
                        }
                    }
                    if( RecordId == mErstReg->ValidRecordIdentifier) {
                        PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)mPersistentStoreBufferPtr;
                        for (;TmpNumOfRecords>0;TmpNumOfRecords--) {
                            if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {
                                mErstReg->ValidRecordIdentifier = PersErrorRecordPtr->UefiErrorRecord.RecordID;
                                break;
                            }
                        }
                    }
                }
            }
            CommandStatus = ERST_SUCCESS;
            break;
        }
        PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
    }

    return CommandStatus;
}

/**
  Write error record 
  
  Clears the given error record if it is already available in the persistent store,
  If there is no space in persistent store then delete all records marked as clear,
  Check again if there is free space in persistent store to write a error record, 
  If so append the error record to the persistent store.Increment RecordCount by 1,
  If previously persistent store is empty, then GetRecordId should point to the 
  newly written record, Gets error record from error log address range,if its a
  valid record write the error record to persistent store
*/

VOID
ErstWriteErrorRecord ( VOID )
{
    UEFI_ERROR_RECORD_HEADER  *ErrorRecordPtr;
    EFI_STATUS                 Status;
    UINT8                      CommandStatus = ERST_SUCCESS;
    PERSISTENT_ERROR_RECORD   *PersistentErrorRecordPtr;
    NV_APPEND_DATA            *NvAppendDataPtr;
    UINT64                     TempAddress;
    VOID                      *FvWheaBase;

    TempAddress = FV_WHEA_BASE;
    FvWheaBase = (VOID*)TempAddress;

    //Set Busy Status
    //
    mErstReg->BusyStatus = 1;

    //Get Error Record from Error Log Address Range
    //
    ErrorRecordPtr = (UEFI_ERROR_RECORD_HEADER*)((UINT8*)mElarPtr + mErstReg->RecordOffset);

    //Check if Error Record is a valid error record
    //
    if (ErrorRecordPtr->SignatureStart  == 'REPC' && ErrorRecordPtr->SignatureEnd == 0xffffffff) {
        ClearErrorRecord(mErstReg->OSRecordIdentifier);

        if (mFreeSpaceInPersistentStore < (ErrorRecordPtr->RecLength+PS_PREPEND_DATA_SIZE+PS_APPEND_DATA_SIZE)) {
            DeleteMarkedErrorRecord();
        }

        if (mFreeSpaceInPersistentStore >= (ErrorRecordPtr->RecLength+PS_PREPEND_DATA_SIZE+PS_APPEND_DATA_SIZE)) {

            if (mErstReg->ValidRecordIdentifier == 0xffffffffffffffff) {
                mErstReg->ValidRecordIdentifier = ErrorRecordPtr->RecordID;
            }

            mErstReg->RecordCount = mErstReg->RecordCount + 1;
            mTotalErrorRecord = mTotalErrorRecord + 1;

            CopyMem ((UINT8*)mPersistentStoreBufferPtr + (FV_WHEA_SIZE - mFreeSpaceInPersistentStore+PS_PREPEND_DATA_SIZE), ErrorRecordPtr, ErrorRecordPtr->RecLength);
            PersistentErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)mPersistentStoreBufferPtr + (FV_WHEA_SIZE - mFreeSpaceInPersistentStore));        
            PersistentErrorRecordPtr->NvPrependData.MagicNum = WHEA_PREPEND_MAGIC_NUMBER;
            PersistentErrorRecordPtr->NvPrependData.RecordValidity = FLASH_ERASE_VALUE;
            NvAppendDataPtr = (NV_APPEND_DATA*)((UINT8*)PersistentErrorRecordPtr + ErrorRecordPtr->RecLength + PS_PREPEND_DATA_SIZE);
            NvAppendDataPtr->MagicNum = WHEA_APPEND_MAGIC_NUMBER;
            if( ( mFreeSpaceInPersistentStore <= FV_WHEA_SIZE ) ) {
                Status = Flash->Write((UINT8*)FvWheaBase+(FV_WHEA_SIZE - mFreeSpaceInPersistentStore), ErrorRecordPtr->RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE, (UINT8*)PersistentErrorRecordPtr);
                DEBUG ((EFI_D_ERROR, "EXIT ErstWriteErrorRecordflash %r\n", Status));
                mFreeSpaceInPersistentStore = mFreeSpaceInPersistentStore - (ErrorRecordPtr->RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
            } 
        } else {
            CommandStatus = ERST_NOT_ENOUGH_SPACE;
        }
    } else {
        CommandStatus = ERST_RECORD_NOT_FOUND;
    }

    mErstReg->OperationStatus = CommandStatus;
    mErstReg->BusyStatus = 0;
}

/**
  Search the persistent store for given record id.If the error record is found
  then transfer the record to the error log address range update GetRecordId
  register to point to next error record

*/

VOID
ErstReadErrorRecord ( VOID )
{
    PERSISTENT_ERROR_RECORD          *PersErrorRecordPtr;
    UINT16                            NumOfRecords;
    UINT16                            TmpNumOfRecords;
    UINT8                             CommandStatus = ERST_RECORD_NOT_FOUND;
    ERROR_RECORD_SERIALIZATION_INFO  *ErrorRecordSerInfoPtr;
    UEFI_ERROR_RECORD_HEADER         *UefiErrorRecordPtr;

    //Set Busy Status
    //
    mErstReg->BusyStatus = 1;

    PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)mPersistentStoreBufferPtr;

    if (mErstReg->OSRecordIdentifier==0 && mErstReg->RecordCount != 0) {

      for (NumOfRecords=mTotalErrorRecord;NumOfRecords>0;NumOfRecords--) {
        if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {

          if ( PcdGet16(PcdElarSize) - mErstReg->RecordOffset >=  PersErrorRecordPtr->UefiErrorRecord.RecLength) {
             CopyMem ((UINT8*)mElarPtr + mErstReg->RecordOffset, &PersErrorRecordPtr->UefiErrorRecord, PersErrorRecordPtr->UefiErrorRecord.RecLength);
             CommandStatus = ERST_SUCCESS;
             break;
          } else {

              CommandStatus = ERST_NOT_ENOUGH_SPACE;
              break;
          }
        }
        PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
      }

    } else {
        for (NumOfRecords=mTotalErrorRecord; NumOfRecords>0; NumOfRecords--) {
          if (PersErrorRecordPtr->UefiErrorRecord.RecordID == mErstReg->OSRecordIdentifier && PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {

            if ( PcdGet16(PcdElarSize)- mErstReg->RecordOffset >=  PersErrorRecordPtr->UefiErrorRecord.RecLength) {
              CopyMem ((UINT8*)mElarPtr + mErstReg->RecordOffset, &PersErrorRecordPtr->UefiErrorRecord, PersErrorRecordPtr->UefiErrorRecord.RecLength);
            } else {
                CommandStatus = ERST_NOT_ENOUGH_SPACE;
                break;
            }

            if (mErstReg->OSRecordIdentifier == mErstReg->ValidRecordIdentifier) {
              PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
              TmpNumOfRecords= NumOfRecords;
              for (;NumOfRecords>0;NumOfRecords--) {
                if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {
                  mErstReg->ValidRecordIdentifier = PersErrorRecordPtr->UefiErrorRecord.RecordID;
                  break;
                }
              }

              if( mErstReg->OSRecordIdentifier == mErstReg->ValidRecordIdentifier) {
                PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)mPersistentStoreBufferPtr;
                for (;TmpNumOfRecords>0;TmpNumOfRecords--) {
                  if (PersErrorRecordPtr->NvPrependData.RecordValidity ==  FLASH_ERASE_VALUE) {
                  mErstReg->ValidRecordIdentifier = PersErrorRecordPtr->UefiErrorRecord.RecordID;
                  break;
                  }
                }
              }
            }

            //BUG FIX
            //
            //Windows 2008 server rc1 BUG, windows 2008 server expects Atributes field to be 0.
            //This bug may be fixed in future version of windows, until then need this code here.
            //
            UefiErrorRecordPtr= (UEFI_ERROR_RECORD_HEADER*)((UINT8*)mElarPtr + mErstReg->RecordOffset);
            ErrorRecordSerInfoPtr = (ERROR_RECORD_SERIALIZATION_INFO *) &UefiErrorRecordPtr->OSReserved;
            ErrorRecordSerInfoPtr->Attributes = 0;

            CommandStatus = ERST_SUCCESS;
            break;
          }

          PersErrorRecordPtr = (PERSISTENT_ERROR_RECORD*)((UINT8*)PersErrorRecordPtr + PersErrorRecordPtr->UefiErrorRecord.RecLength + PS_PREPEND_DATA_SIZE + PS_APPEND_DATA_SIZE);
        }

    }

    if (mErstReg->RecordCount == 0) {
      mErstReg->OperationStatus = ERST_RECORD_STORE_EMPTY;
    } else {
        mErstReg->OperationStatus = CommandStatus;
    }
    mErstReg->BusyStatus = 0;
}

/**
  Clears the Error record from persistent store.Calls ClearErrorRecord function

*/

VOID 
ErstClearErrorRecord ( VOID )
{
    UINT8   CommandStatus;

    //Set Busy Status
    mErstReg->BusyStatus = 1;
    CommandStatus = ClearErrorRecord(mErstReg->OSRecordIdentifier);
    mErstReg->OperationStatus = CommandStatus;
    mErstReg->BusyStatus = 0;
}

/**
  Writes a dummy error record to from persitent store

*/

VOID 
ErstDummyWriteErrorRecord ( VOID )
{
    mErstReg->OperationStatus = ERST_SUCCESS;
    mErstReg->BusyStatus = 0;
}

/**
  SMI handler for ERST

  @param  DispatchHandle  - EFI Handle
  @param  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT
  
  @retval always return EFI_SUCCESS
*/

EFI_STATUS 
EFIAPI
ErstSWSMIHandler(
  IN EFI_HANDLE                        DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT *RegisterContext,
  IN OUT EFI_SMM_SW_CONTEXT            *SwContext,
  IN OUT UINTN                         *CommBufferSize
)
{
    // check if storage enabled
    if(!mWheaData->ErstEnabled){
        mErstReg->OperationStatus = ERST_HARDWARE_NOT_AVAILABLE;
        mErstReg->BusyStatus = 0;
        //DEBUG((EFI_D_ERROR, "==ERST_HW_NOT_AVAILABLE\n"));
        return EFI_SUCCESS;
    }
    
    if (mErstReg->CmdReg == ERST_BEGIN_WRITE_OPERATION) {
      ErstWriteErrorRecord();

    } else if(mErstReg->CmdReg == ERST_BEGIN_READ_OPERATION) {
       ErstReadErrorRecord();

    } else if(mErstReg->CmdReg == ERST_BEGIN_CLEAR_OPERATION) {
       ErstClearErrorRecord();

    } else if(mErstReg->CmdReg == ERST_BEGIN_DUMMY_WRITE_OPERATION) {
       ErstDummyWriteErrorRecord();
    }
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2017, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
