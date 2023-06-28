//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//****************************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//****************************************************************************
// Revision History
// ----------------
// $Log: $
//
//
//
//****************************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name: AmiPldmInterfaces.c
//
// Description: Defined the interfaces to export the PLDM Tables to BMC and Import PLDM Tables from BMC
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <AmiDxeLib.h>
//#include <Token.h>
#include "Library/BaseMemoryLib.h"
#include "Library/UefiLib.h"
#include "Protocol/HiiDatabase.h"
#include "Library/MemoryAllocationLib.h"
#include "AmiPldmInterfaces.h"
#include "AmiPldmInterfaceDriver.h"
#include "PldmInterfaceElinks.h"
#include "PldmApi.h"

static BOOLEAN gHIIDataInitialized  = FALSE;
static BOOLEAN gCRCInitialized = FALSE;
static ULONG CRCTableData[0x100];
EFI_HII_DATABASE_PROTOCOL *gAmiHiiDatabaseProtocol = NULL;
extern BOOLEAN gUseCache;

typedef enum{PldmStringTable,PldmAttributeTable,PldmValueTable,PldmMapStringTable=4}PLDMTableType;
typedef EFI_STATUS (HOOK_SAVE_PLDM_TABLES)( PLDMTables *PldmTables );
extern HOOK_SAVE_PLDM_TABLES HOOK_SAVE_PLDM_TABLES_LIST EndOfSavePldmTablesHook;
HOOK_SAVE_PLDM_TABLES* SavePldmDataTablesList[] = {HOOK_SAVE_PLDM_TABLES_LIST NULL};

typedef EFI_STATUS (HOOK_RESTORE_PLDM_TABLES)( PLDMTables *PldmTables );
extern HOOK_RESTORE_PLDM_TABLES HOOK_RESTORE_PLDM_TABLES_LIST EndOfRestorePldmTablesHook;
HOOK_RESTORE_PLDM_TABLES* RestorePldmDataTablesList[] = {HOOK_RESTORE_PLDM_TABLES_LIST NULL};

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   RetrieveHIIDatabase
//
// Description: Retrive UEFI 2.1 Hii Database
//
// Paramaters:  OUT UINT8 **HIIData - HII DB buffer
//              OUT UINTN *DataSize - HII DB size
//
// Return:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RetrieveHIIDatabase(UINT8 **HIIData, UINTN *DataSize)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HII_HANDLE Handle = NULL;

    UINT8 *Buffer = NULL;
    UINTN BufferLength = 0;

    Status = gAmiHiiDatabaseProtocol->ExportPackageLists( gAmiHiiDatabaseProtocol, Handle, &BufferLength,
             ( EFI_HII_PACKAGE_LIST_HEADER *)Buffer );

    if ( EFI_ERROR ( Status ) )
    {
        if ( Status == EFI_BUFFER_TOO_SMALL)
        {
            Buffer = ( UINT8 * )calloc( BufferLength, sizeof(UINT8));

            if ( Buffer == NULL)
            {
                Status = EFI_OUT_OF_RESOURCES;
                goto DONE;
            }

            Status = gAmiHiiDatabaseProtocol->ExportPackageLists( gAmiHiiDatabaseProtocol, Handle, &BufferLength,
                     ( EFI_HII_PACKAGE_LIST_HEADER * )Buffer );
        }

        if ( EFI_ERROR( Status ) )
        {
            goto DONE;
        }
    }

    *HIIData = Buffer;
    *DataSize = BufferLength;

DONE:
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   InitHIIProtocol
//
// Description: Initialize HII protocol if gHIIDataInitialized  is not set.
//              This function initializes UEFI 2.1 protocol
//
// Paramaters:  VOID
//
// Return:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS InitHIIProtocol()
{
    EFI_STATUS Status = EFI_SUCCESS;

    if ( gHIIDataInitialized  )
    {
        return Status;
    }

    //UEFI 2.1 Hii Database Initialization
    Status = pBS->LocateProtocol(&gEfiHiiDatabaseProtocolGuid, NULL, &gAmiHiiDatabaseProtocol);
    
    gHIIDataInitialized  = TRUE;

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   SwapBits
//
// Description: Swap bit 0 for bit 7
//              bit 1 for bit 6, etc.
//
// Paramaters:  ULONG ref
//              char ch
//
// Return:      static ULONG
//
// Modified:
//
// Referral(s):
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
static ULONG SwapBits(ULONG ref, char ch)
{
    ULONG value = 0;
    int i;

    for (i = 1; i < (ch + 1); i++)
    {
        if (ref & 1)
        {
            value |= 1 << (ch - i);
        }

        ref >>= 1;
    }

    return value;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure:   CalculateHiiCRC
//
// Description: calculate the CRC of the Hii Buffer
//
// Paramaters:  const LPBYTE Buffer, UINT Size UINT32 *CRC
//
// Return:      EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
void CalculateHiiCRC(const LPBYTE Buffer, UINT Size, UINT32 *CRC)
{
    LPBYTE pbyte = Buffer;
    UINT32 crc = 0xFFFFFFFF;
    UINT size = Size;
    ULONG ulPolynomial = 0x04C11DB7;
    int i;
    int j;
        
    if (!gCRCInitialized)
    {
        //Initialize the CRC
        // 256 values representing ASCII character codes.
        for (i = 0; i <= 0xFF; i++)
        {
            CRCTableData[i] = SwapBits(i, 8) << 24;
        
            for (j = 0; j < 8; j++)
            {
                CRCTableData[i] = (CRCTableData[i] << 1) ^ (CRCTableData[i] & (1 << 31) ? ulPolynomial : 0);
            }
        
            CRCTableData[i] = SwapBits(CRCTableData[i],  32);
        }
        
        gCRCInitialized = TRUE;
    }

    while (size--)
    {
        crc = (crc >> 8) ^ CRCTableData[(crc & 0xFF) ^ *pbyte++];
    }

    *CRC = crc ^ 0xFFFFFFFF;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:   GetPLDMDataFromBMC
//
// Description: To get the PLDM Tables data from the BMC.
//
// Parameter:  BMC_PLDMPAYLOAD PayLoadData, PLDM_GETBIOSTABLE_REQUEST GetTableRequest,  BYTE TableType,UINT8  **TableData, UINT32 *PldmTableSize
//
// Return Value: EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS GetPLDMDataFromBMC( BMC_PLDMPAYLOAD PayLoadData, PLDM_GETBIOSTABLE_REQUEST GetTableRequest,  BYTE TableType,UINT8  **PldmTableData, UINT32 *PldmTableSize )
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8   *BmcCommandData = (UINT8 *)NULL;
    UINT32  BmcCommandSize = 0;
    BOOLEAN CommandCompleted = FALSE;
    UINT8   CommandCompletionCode = 0;
    UINT8   *IpmiResponseData = (UINT8 *)NULL;
    UINT32  IpmiResponseSize = 0;
    UINT8 *TempBuffer = NULL;
    BYTE    OemIPMICommand = 0;
    UINT32 TableSize = 0;
    
    //IPMI command for PLDM Table GET/SET
    OemIPMICommand = GetOemIpmiCommand();
    
    BmcCommandSize = (sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_GETBIOSTABLE_REQUEST));
    
    BmcCommandData = (UINT8 *)calloc(BmcCommandSize, (sizeof(UINT8 *)));
    if (BmcCommandData == NULL)
    {
        DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: BmcCommandData EFI_OUT_OF_RESOURCES"));
        return EFI_OUT_OF_RESOURCES;
    }
    
    //construct the OEM IPMI command data
    memcpy(BmcCommandData, &PayLoadData, sizeof(BMC_PLDMPAYLOAD));       
    memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD), &GetTableRequest, sizeof(PLDM_GETBIOSTABLE_REQUEST));
    
    //Invoke the Oem IPMI command to the get Table data from  BMC
    //If multipart transfer needed, need to reconstruct the Request data and call the Command again.
    do
    {
        //Response data transfer size maximum is 32KB
        IpmiResponseSize = TABLEDATASIZE32KB;
        
        DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: IpmiResponseSize:%d", IpmiResponseSize));
        Status = ExecuteBMCOemIpmiCommand(GetBmcOemNetFunction(), GetBmcOemLun(), OemIPMICommand, BmcCommandData, BmcCommandSize, &IpmiResponseData, &IpmiResponseSize, &CommandCompletionCode);
        DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: ExecuteBMCOemIpmiCommand Status: %x, CC: %x, IpmiResponseSize:%d", Status, CommandCompletionCode, IpmiResponseSize));
        if ( (EFI_SUCCESS == Status) && (EFI_SUCCESS == CommandCompletionCode) )
        {
            UINT32  TempSize = 0;
            PLDM_GETBIOSTABLE_RESPONSE *Response = (PLDM_GETBIOSTABLE_RESPONSE *)NULL;
            UINT32 NextTransferHandle = (UINT32)NULL;   
            UINT32 DataOffset = sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_GETBIOSTABLE_RESPONSE);
            Response = (PLDM_GETBIOSTABLE_RESPONSE *)(IpmiResponseData + sizeof(BMC_PLDMPAYLOAD));//Response data is from 5th byte
            
            if (Response->CompletionCode != EFI_SUCCESS)
            {
               DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: GET: Response->CompletionCode: %x", Response->CompletionCode));
               Status = Response->CompletionCode;
               break;
            }
            
            NextTransferHandle = Response->NextHandle;
            
            if (IpmiResponseSize < DataOffset)
            {
               DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: Invalid Table data"));
               Status = EFI_OUT_OF_RESOURCES;
               CommandCompleted = TRUE;
               break;
            }
            
            TempSize = IpmiResponseSize - DataOffset;
            DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: TempSize:%d", TempSize));
            
            if ( 0 != TableSize )
            {
               //Check if the buffer already has data
               TempBuffer = (UINT8 *)realloc(TempBuffer, (TableSize + TempSize));
            
               if (TempBuffer == NULL)
               {
                   DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: TempBuffer EFI_OUT_OF_RESOURCES"));
                   Status = EFI_OUT_OF_RESOURCES;
                   break;
               }
            }
            else
            {
               TempBuffer = (UINT8 *)calloc(TempSize, (sizeof(UINT8)));
            
               if (TempBuffer == NULL)
               {
                   DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: TempBuffer EFI_OUT_OF_RESOURCES"));
                   Status = EFI_OUT_OF_RESOURCES;
                   break;
               }
            }
            
            DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: TableSize: %x, TempSize: %x", TableSize, TempSize));
            memcpy(TempBuffer + TableSize, IpmiResponseData + sizeof(BMC_PLDMPAYLOAD) + (sizeof(PLDM_GETBIOSTABLE_RESPONSE)), TempSize);
            
            if (((Response->TransferFlag & TABLE_END)  != TABLE_END) && ((Response->TransferFlag & TABLE_START_AND_END) != TABLE_START_AND_END))
            {
                
               //Update the GetRequest Table data for next data transfer
               GetTableRequest.Handle = NextTransferHandle;
               GetTableRequest.TransferOperationFlag = GETDATANEXTPART;
               GetTableRequest.TableType = TableType;
                                      
               memcpy(BmcCommandData, &PayLoadData, sizeof(BMC_PLDMPAYLOAD));
               memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD), &GetTableRequest, sizeof(PLDM_GETBIOSTABLE_REQUEST));
               TableSize += TempSize;
               DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: GetTableRequest.Handle: %x, GetTableRequest: %x", GetTableRequest.Handle, GetTableRequest.TransferOperationFlag));
            
               if (IpmiResponseData)
               {
                   free(IpmiResponseData);
               }
            }
            else
            {
                TableSize += TempSize;
                
                *PldmTableData = TempBuffer;
                *PldmTableSize = TableSize;
                DEBUG((DEBUG_ERROR, "\n GetPLDMDataFromBMC: Table:%d Get BIOS table done",TableType));
                CommandCompleted = TRUE;
            }
        }
        else
        {
            if(CommandCompletionCode)
            {
                Status = CommandCompletionCode; 
            }
            if (BmcCommandData)
            {
               free(BmcCommandData);
            }
            
            if (IpmiResponseData)
            {
               free(IpmiResponseData);
            }
        }
        
        if (Status)
        {
           return Status;
        }
    } while (CommandCompleted == FALSE);
    
    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure    : SetPLDMDataToBMC
//
// Description  : To set the PLDM Tables data from to the BMC.
//
// Parameter    : BMC_PLDMPAYLOAD PayLoadData, PLDM_SETBIOSTABLE_REQUEST SetTableRequest, BYTE TableType,UINT8  *PldmTableData, UINT32 PldmTableSize
//
// Return Value : EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SetPLDMDataToBMC(BMC_PLDMPAYLOAD PayLoadData, PLDM_SETBIOSTABLE_REQUEST SetTableRequest, BYTE TableType,UINT8  *PldmTableData, UINT32 PldmTableSize)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8   *BmcCommandData = (UINT8 *)NULL;
    UINT32  BmcCommandSize = 0;
    BOOLEAN CommandCompleted = FALSE;
    UINT8   CommandCompletionCode = 0;
    UINT8   *IpmiResponseData = (UINT8 *)NULL;
    UINT32  IpmiResponseSize = 0;
    UINT8 *TempBuffer = NULL;
    BYTE    OemIPMICommand = 0;
    UINT32 TableSize = PldmTableSize;
    UINT32 PacketSize = 0;
    UINT32  BytesTransferred = 0;
    
    //IPMI command for PLDM Table GET/SET
    OemIPMICommand = GetOemIpmiCommand();

    //Check the Pldm Table Size and update the PacketSize
    if (TableSize > TABLEDATASIZE30KB)
    {
       PacketSize = TABLEDATASIZE30KB;
       SetTableRequest.TransferFlag = TABLE_START;
    }
    else
    {
       PacketSize = TableSize;
       SetTableRequest.TransferFlag = TABLE_START_AND_END;
    }
    
    BmcCommandSize = (sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_SETBIOSTABLE_REQUEST) + PacketSize);
                       
    BmcCommandData = (UINT8 *)calloc(BmcCommandSize, (sizeof(UINT8 *)));
    if (BmcCommandData == NULL)
    {
       DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:ERROR OUT OF MEMORY"));
       return EFI_OUT_OF_RESOURCES;
    }
    
    //construct the OEM IPMI command data
    memcpy(BmcCommandData, &PayLoadData, sizeof(BMC_PLDMPAYLOAD));
    memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD), &SetTableRequest, sizeof(PLDM_SETBIOSTABLE_REQUEST));
    memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_SETBIOSTABLE_REQUEST), PldmTableData, PacketSize);
    DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:PacketSize: %x, TransferFlag: %x, Type: %x", PacketSize, SetTableRequest.TransferFlag, SetTableRequest.TableType));
    
    //Invoke the Oem IPMI command to set the Table data in BMC
    //If multipart transfer needed, need to reconstruct the Request data and call the Command again.
    do
    {
        //Response size for SetBiosTable
        IpmiResponseSize = 0x08;
        
        DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:IpmiResponseSize:%d", IpmiResponseSize));
        Status = ExecuteBMCOemIpmiCommand(GetBmcOemNetFunction(), GetBmcOemLun(), OemIPMICommand, BmcCommandData, BmcCommandSize, &IpmiResponseData, &IpmiResponseSize, &CommandCompletionCode);
        DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:ExecuteBMCOemIpmiCommand Status: %x, CC: %x, IpmiResponseSize:%d", Status, CommandCompletionCode, IpmiResponseSize));
        if ( (EFI_SUCCESS == Status) && (EFI_SUCCESS == CommandCompletionCode) )
        {
            PLDM_SETBIOSTABLE_RESPONSE *Response;
            Response = (PLDM_SETBIOSTABLE_RESPONSE *)(IpmiResponseData + sizeof(BMC_PLDMPAYLOAD));//response is from 5th byte
            
            if (Response->CompletionCode != EFI_SUCCESS)
            {
                DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:SET: Response->CompletionCode: %x", Response->CompletionCode));
                Status = Response->CompletionCode;
                break;
            }
            
            if ( ((SetTableRequest.TransferFlag & TABLE_START_AND_END) == TABLE_START_AND_END)
               || ((SetTableRequest.TransferFlag & TABLE_END) == TABLE_END) )
            {
                CommandCompleted = TRUE;
                break;
            }
            else
            {
                UINT32 TempSize;
                
                //Update the SetTable request data
                SetTableRequest.Handle = Response->NextDataTransferHandle;//Pass the response handle as request handle
                BytesTransferred += PacketSize;
                TempSize = TableSize - BytesTransferred;
                
                if (TempSize > TABLEDATASIZE30KB)
                {
                   PacketSize = TABLEDATASIZE30KB;
                   SetTableRequest.TransferFlag = TABLE_MIDDLE;
                }
                else
                {
                   PacketSize = TempSize;
                   SetTableRequest.TransferFlag = TABLE_END;
                }
                SetTableRequest.TableType = TableType;
                
                DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:PacketSize: %x, TransferFlag: %x \n", PacketSize, SetTableRequest.TransferFlag));
                BmcCommandSize = (sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_SETBIOSTABLE_REQUEST) + PacketSize);
                BmcCommandData = (UINT8 *)realloc(BmcCommandData, BmcCommandSize);
                
                if (BmcCommandData == NULL)
                {
                   DEBUG((DEBUG_ERROR, "\n SetPLDMDataToBMC:ERROR OUT OF MEMORY"));
                   return EFI_OUT_OF_RESOURCES;
                }
                
                //Update the BMC Command data
                memcpy(BmcCommandData, &PayLoadData, sizeof(BMC_PLDMPAYLOAD));
                memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD), &SetTableRequest, sizeof(PLDM_SETBIOSTABLE_REQUEST));
                memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_SETBIOSTABLE_REQUEST), PldmTableData + BytesTransferred, PacketSize);
                
                if (IpmiResponseData)
                {
                   free(IpmiResponseData);
                }
            }
        }
        else
        {
            if(CommandCompletionCode)
            {
               Status = CommandCompletionCode; 
            }
            if (BmcCommandData)
            {
               free(BmcCommandData);
            }
            
            if (IpmiResponseData)
            {
               free(IpmiResponseData);
            }
        }
        
        if (Status)
        {
           return Status;
        }
    } while (CommandCompleted == FALSE);
    
    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------------------
// Procedure    : GetTableTagsFromBMC
//
// Description  : Get the PLDM Table Tags from the BMC
//
// Parameter    : VOID
//
// Return Value : EFI_STATUS
//
//---------------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS  SendTagsCommandToBmc(BYTE Command, PLDMTableTags *TableTags)//GetTableTagsFromBMC(PLDMTableTags *TableTags)
{
    BMC_PLDMPAYLOAD PldmPayloadHead;
    PLDM_GET_BIOS_TABLE_TAGS_REQUEST RequestTags;
    PLDM_SET_BIOS_TABLE_TAGS_REQUEST RequestData;
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8   *BmcCommandData = NULL;
    UINT32  BmcCommandSize = 0;
    UINT8   *IpmiResponseData = NULL;
    UINT32  IpmiResponseSize = 0;
    BYTE    OemIPMICommand = 0;
    UINT8   CommandCompletionCode = 0;

    PldmPayloadHead.Request = 1;
    PldmPayloadHead.Datagram = 0;
    PldmPayloadHead.Reserved = 0;
    PldmPayloadHead.InstanceID = 0x6;
    PldmPayloadHead.HdrVer = 0;
    PldmPayloadHead.PLDMType = 0x03;
    PldmPayloadHead.PLDMCommand = Command;

    if (Command == PLDM_GET_BIOS_TABLE_TAGS)
    {
        RequestTags.NumberOfTables = 4;
        RequestTags.TableType[0] = 0x0; //String Table
        RequestTags.TableType[1] = 0x1; //Attribute Table
        RequestTags.TableType[2] = 0x2; //Value Table
        RequestTags.TableType[3] = 0x4; //Mapstring Table

        BmcCommandSize = (sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_GET_BIOS_TABLE_TAGS_REQUEST));
    }
    else
    {
        RequestData.NumberOfTables = 4;
        RequestData.StringTableType = 0x0; //string table
        RequestData.StringTableTag = TableTags->StringTableTag;
        RequestData.attTableType = 0x1; //attribute table
        RequestData.attTableTag = TableTags->AttributeTableTag;
        RequestData.ValTableType = 0x2; //value table
        RequestData.ValTableTag = TableTags->ValueTableTag;
        RequestData.MApTableType = 0x4; //mapping string table
        RequestData.MapTableTag = TableTags->MapTableTag;

        BmcCommandSize = (sizeof(BMC_PLDMPAYLOAD) + sizeof(PLDM_SET_BIOS_TABLE_TAGS_REQUEST));
    }

    OemIPMICommand = GetOemIpmiCommand();

    BmcCommandData = (UINT8 *)calloc(BmcCommandSize, (sizeof(UINT8 *)));

    if (BmcCommandData == NULL)
    {
        DEBUG((DEBUG_ERROR, "\n ERROR OUT OF MEMORY"));
        return EFI_OUT_OF_RESOURCES;
    }

    memcpy(BmcCommandData, &PldmPayloadHead, sizeof(BMC_PLDMPAYLOAD));
    
    if (Command == PLDM_GET_BIOS_TABLE_TAGS)
    {
        memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD), &RequestTags, sizeof(PLDM_GET_BIOS_TABLE_TAGS_REQUEST));
        IpmiResponseSize = 20;
    }
    else
    {
        memcpy(BmcCommandData + sizeof(BMC_PLDMPAYLOAD), &RequestData, sizeof(PLDM_SET_BIOS_TABLE_TAGS_REQUEST));
        IpmiResponseSize = 4;
    }

    //Call OEM IPMI Command.
    Status = ExecuteBMCOemIpmiCommand(GetBmcOemNetFunction(), GetBmcOemLun(), OemIPMICommand,  BmcCommandData, BmcCommandSize, &IpmiResponseData, &IpmiResponseSize, &CommandCompletionCode);
    DEBUG((DEBUG_ERROR, "\n ExecuteOemIpmiCommand Status: %x, CC: %x, IpmiResponseSize:%d", Status, CommandCompletionCode, IpmiResponseSize));

    if ((EFI_SUCCESS == Status) && (EFI_SUCCESS == CommandCompletionCode))
    {
        if (Command == PLDM_GET_BIOS_TABLE_TAGS)
        {
            PLDM_GET_BIOS_TABLE_TAGS_RESPONSE *Response;
            UINT8 CompletionStatus;
            UINTN Size = sizeof(UINT32);
    
            Response = (PLDM_GET_BIOS_TABLE_TAGS_RESPONSE *)(IpmiResponseData + sizeof(BMC_PLDMPAYLOAD));//response is from 5th byte
            CompletionStatus = Response->CompletionCode;
            DEBUG((DEBUG_ERROR, "\n GetTagsCompletionStatus:%x", CompletionStatus));
            Status = CompletionStatus;
    
            if (CompletionStatus == EFI_SUCCESS )
            {
                TableTags->StringTableTag = Response->TableTag[0];
                TableTags->AttributeTableTag = Response->TableTag[1];
                TableTags->ValueTableTag = Response->TableTag[2];
                TableTags->MapTableTag = Response->TableTag[3];
   
                DEBUG((DEBUG_ERROR, "\nTABLETAGS:StringTableTag:%d,AttributeTableTag:%d,ValueTableTag:%d,MappingStringTableTag:%d",
                        TableTags->StringTableTag, TableTags->AttributeTableTag, TableTags->ValueTableTag, TableTags->MapTableTag));
            }//Response Found
            else if (CompletionStatus == BIOS_TABLE_TAG_UNAVAILABLE)
            {
                TableTags->StringTableTag = 0;
                TableTags->AttributeTableTag = 0;
                TableTags->ValueTableTag = 0;
                TableTags->MapTableTag = 0;

                DEBUG((DEBUG_ERROR, "\nTABLETAGS:BIOS_TABLE_TAG_UNAVAILABLE"));
                Status = CompletionStatus;
            }
            else
            {
                Status = CompletionStatus;
            }
        }
        else
        {
            PLDM_SET_BIOS_TABLE_TAGS_RESPONSE *Response;
            UINT8 CompletionStatus;
            UINTN Size = sizeof(UINT32);

            Response = (PLDM_SET_BIOS_TABLE_TAGS_RESPONSE *)(IpmiResponseData + sizeof(BMC_PLDMPAYLOAD));//response is from 5th byte
            CompletionStatus = Response->CompletionCode;
            DEBUG((DEBUG_ERROR, "\n SetTags CompletionStatus:%x", CompletionStatus));

            if (CompletionStatus != EFI_SUCCESS )
            {
                Status = CompletionStatus;
            }
        }
    }//IPMI Command Success
    else
    {
        if(CommandCompletionCode)
        {
            Status = CommandCompletionCode; 
        }
    }

    if (BmcCommandData)
    {
        free(BmcCommandData);
    }

    if (IpmiResponseData)
    {
        free(IpmiResponseData);
    }

    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure    : PldmGetHiiData
//
// Description  : Retrieve the HII Database and its CRC32
//
// Paramaters   : UINT8 **HiiData, UINTN *DataSize, UINT32 *CRC32
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PldmGetHiiData (UINT8 **HiiData, UINTN *DataSize, UINT32 *CRC32)
{
    EFI_STATUS Status = EFI_SUCCESS;
       
    Status = InitHIIProtocol();

    if ( EFI_ERROR ( Status ) )
    {
        goto DONE;
    }

    Status = RetrieveHIIDatabase( HiiData, DataSize );

    if ( EFI_ERROR ( Status ) )
    {
        goto DONE;
    }

    CalculateHiiCRC( *HiiData, ( UINT32 )*DataSize, ( UINT32 * )CRC32 );

DONE:
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure    : PldmSetPldmMode
//
// Description  : Set the Pldm cache mode i.e.whether to get/set data from Browser or NVRAM
//
// Paramaters   : BOOLEAN PldmMode
//
// Return       : VOID
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
VOID EFIAPI PldmSetPldmMode (BOOLEAN PldmMode)
{
    //Set the gUseCache variable value of Pldm library
    gUseCache = PldmMode;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure    : PldmCreatePldmTables
//
// Description  : Creates the Pldm Tables using current Hii data and returns the Tables
//
// Paramaters   : PLDMTables *PldmTables, BOOLEAN ExportVariables
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PldmCreatePldmTables (PLDMTables *PldmTables, BOOLEAN ExportVariables)
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

   //Get the HII Data and store in a buffer.
   Status = PldmGetHiiData( &HIIData, &DBSize, &CRC32 );

   if (Status != EFI_SUCCESS)
   {
       return Status;
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
   DEBUG((DEBUG_ERROR, "\n PldmCreatePldmTables: CreatePldmTables status:%x",Status));
   if (Status != EFI_SUCCESS)
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
// Procedure    : PldmImportPldmTables
//
// Description  : Imports the pldm data to the Hii
//
// Paramaters   : PLDMTables *PldmTables
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PldmImportPldmTables (PLDMTables *PldmTables)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 *HIIData = (UINT8 *)NULL;
    UINT8 *EngStringTable = (UINT8 *)NULL;
    UINT8 *AttributeTable = (UINT8 *)NULL;
    UINT8 *ValueTable = (UINT8 *)NULL;
    UINT8 *MapStringTable = (UINT8 *)NULL;
    UINT32 EngStringSize = 0;
    UINT32 AttributeSize = 0;
    UINT32 ValueSize = 0;
    UINT32 MapStringSize = 0;
    UINT32 CRC32 = 0;
    UINTN DBSize = 0;
    
    //Get the Pldm tables data
    EngStringTable = PldmTables->StringTable;
    EngStringSize = PldmTables->StringSize;
    AttributeTable = PldmTables->AttributeTable;
    AttributeSize = PldmTables->AttributeSize;
    ValueTable = PldmTables->ValueTable;
    ValueSize = PldmTables->ValueSize;
    MapStringTable = PldmTables->MapStringTable;
    MapStringSize = PldmTables->MapSize;
    
    //Get HII dump and store in a buffer
    Status = PldmGetHiiData( &HIIData, &DBSize, &CRC32 );
    
    if (Status != EFI_SUCCESS)
    {
        return Status;
    }
    
    //Import the PLDM tables to HII
    if (AttributeTable && MapStringTable && EngStringTable && ValueTable &&
            AttributeSize && MapStringSize && EngStringSize && ValueSize)
    {
        Status = ImportMappedPldmValues(
                    HIIData,
                    (UINT32)DBSize,
                    ValueTable,
                    ValueSize,
                    AttributeTable, 
                    AttributeSize,
                    MapStringTable, 
                    MapStringSize,
                    EngStringTable, 
                    EngStringSize
                    );
    
        DEBUG((DEBUG_ERROR, "\n PldmImportPldmTables: ImportMappedPldmValues status:%x",Status));
        if (Status != EFI_SUCCESS)
        {
            return Status;
        }
    
        Status = ImportVarValuesByHiiHandle(
                     NULL,
                     ValueTable,
                     ValueSize,
                     AttributeTable, 
                     AttributeSize,
                     EngStringTable, 
                     EngStringSize
                     );
    }
    else
    {
        DEBUG((DEBUG_ERROR, "\n PldmImportPldmTables: Invalid Tables data"));
        Status = EFI_BAD_BUFFER_SIZE;
    }
    
    return Status;
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
//
// Procedure    : PldmCreatePldmByHiiHandle
//
// Description  : Creates the Pldm Tables using the Hii Handles data and returns the Tables
//
// Paramaters   : EFI_HII_HANDLE *Handles,UINT64 Count,PLDMTables *PldmTables, BOOLEAN ExportVariables
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PldmCreatePldmByHiiHandle (EFI_HII_HANDLE *Handles,UINT64 Count,PLDMTables *PldmTables, BOOLEAN ExportVariables)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 *StringTable = (UINT8 *)NULL;
    UINT8 *AttributeTable = (UINT8 *)NULL;
    UINT8 *ValueTable = (UINT8 *)NULL;
    UINT8 *MapStringTable = (UINT8 *)NULL;
    UINT32 StringSize = 0;
    UINT32 AttributeSize = 0;
    UINT32 ValueSize = 0;
    UINT32 MapSize = 0;
    UINTN Index = 0;
    
    //Create the PLDM Tables from the HII data
    Status = CreatePldmTablesByHiiHandles(
                Handles,       
                (UINT32)Count,       
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
    DEBUG((DEBUG_ERROR, "PldmCreatePldmByHiiHandle:CreatePldmTablesByHiiHandles status:%x\r\n", Status));
    
    if (Status != EFI_SUCCESS)
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
// Procedure    : PldmImportPldmByHiiHandle
//
// Description  : Import the Pldm tables data to the Hii
//
// Paramaters   : EFI_HII_HANDLE Handles,PLDMTables *PldmTables
//
// Return       : EFI_STATUS
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI PldmImportPldmByHiiHandle (EFI_HII_HANDLE Handle,PLDMTables *PldmTables)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINT8 *HIIDBData = (UINT8 *)NULL;
    UINT8 *EngStringTable = (UINT8 *)NULL;
    UINT8 *AttributeTable = (UINT8 *)NULL;
    UINT8 *ValueTable = (UINT8 *)NULL;
    UINT8 *MapStringTable =(UINT8 *)NULL;
    UINT32 EngStringSize = 0;
    UINT32 AttributeSize = 0;
    UINT32 ValueSize = 0;
    UINT32 MapStringSize = 0;
    UINT32 CRC32 = 0;
    UINTN DBSize = 0;
    
    EngStringTable = PldmTables->StringTable;
    EngStringSize = PldmTables->StringSize;
    AttributeTable = PldmTables->AttributeTable;
    AttributeSize = PldmTables->AttributeSize;
    ValueTable = PldmTables->ValueTable;
    ValueSize = PldmTables->ValueSize;
    MapStringTable = PldmTables->MapStringTable;
    MapStringSize = PldmTables->MapSize;
    
    //Get HII dump and store in a buffer
    Status = PldmGetHiiData( &HIIDBData, &DBSize, &CRC32 );
    
    if (Status != EFI_SUCCESS)
    {
        return Status;
    }
    
    //Import the PLDM tables to HII
    if (AttributeTable && EngStringTable && ValueTable &&
            AttributeSize && EngStringSize && ValueSize && MapStringTable && MapStringSize )
    {
        //Import User defaults PLDM tables with mapping
        Status = ImportMappedPldmValues(
                    HIIDBData,
                    (UINT32)DBSize,
                    ValueTable,
                    ValueSize,
                    AttributeTable, 
                    AttributeSize,
                    MapStringTable, 
                    MapStringSize,
                    EngStringTable, 
                    EngStringSize
                    );
    
        DEBUG((DEBUG_ERROR, "PldmImportPldmByHiiHandle:ImportMappedPldmValues status:%x\r\n", Status));
    }
    else
    {
        Status = EFI_BAD_BUFFER_SIZE;
    }
    
    //Import the PLDM tables to HII
    if (AttributeTable && EngStringTable && ValueTable &&
            AttributeSize && EngStringSize && ValueSize)
    {
        Status = ImportVarValuesByHiiHandle(
                     Handle,
                     ValueTable,
                     ValueSize,
                     AttributeTable, // attribute table from foreign system
                     AttributeSize,
                     EngStringTable, // english string table from foreign system
                     EngStringSize
                 );
        DEBUG((DEBUG_ERROR, "PldmImportPldmByHiiHandle:ImportVarValuesByHiiHandle Status:%x\r\n", Status));
    }
    else
    {
        Status = EFI_BAD_BUFFER_SIZE;
    }
    
    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure    : PldmRestoreConfigurationFromBmc
//
// Description  : Reads the BIOS configuration PLDM Tables data From BMC
//
// Parameter    :   OUT UINT8  **ValueTable,        // ValueTable pointer   
//                  OUT UINT32  *ValueSize,         // value table size    
//                  OUT UINT8  **AttributeTable,    // AttributeTable table pointer
//                  OUT  UINT32  *AttributeSize,    //AttributeTable Size
//                  OUT  UINT8  **StringTable,      //English String Table Pointer 
//                  OUT  UINT32  *StringSize,       //English string table size
//                  OUT UINT8  **MapStringTable,    // x-AMI string table from foreign system
//                  OUT UINT32  *MapStringSize      //Map String Table Size
// Return Value : EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PldmRestoreConfigurationFromBmc( 
        PLDMTables *PldmTables
    )
{
    UINT8  **ValueTable = (UINT8  **)NULL;
    UINT32  ValueSize = 0;
    UINT8  **AttributeTable = (UINT8  **)NULL;
    UINT32  AttributeSize = 0;
    UINT8  **StringTable = (UINT8  **)NULL;
    UINT32  StringSize = 0;
    UINT8  **MapStringTable = (UINT8  **)NULL;
    UINT32  MapStringSize = 0; 
    EFI_STATUS Status = EFI_SUCCESS;
    BMC_PLDMPAYLOAD PldmPayloadHead = {0};
    BYTE TableType;
    
    PldmPayloadHead.Request = 1;
    PldmPayloadHead.Datagram = 0;
    PldmPayloadHead.Reserved = 0;
    PldmPayloadHead.InstanceID = 0;
    PldmPayloadHead.HdrVer = 0;
    PldmPayloadHead.PLDMType = 0x03;//PLDM for BIOS_Control_and_Configuration
    PldmPayloadHead.PLDMCommand = PLDM_GET_BIOSTABLE;//PLDM_GET_BIOSTABLE to get the Tables form BMC
        
    for(TableType = PldmStringTable; TableType <= PldmMapStringTable; TableType++)
    {
        PLDM_GETBIOSTABLE_REQUEST GetBiosTableRequest = {0};
        
        PldmPayloadHead.InstanceID = TableType;
        
        //Fill the Get BiosTable Request data
        GetBiosTableRequest.Handle = 0;
        GetBiosTableRequest.TransferOperationFlag = GETDATAFIRSTPART;
        GetBiosTableRequest.TableType = TableType;
        DEBUG((DEBUG_ERROR, "\n RestoreConfigurationFromBmc:TransferFlag: %x, Type: %x", GetBiosTableRequest.TransferOperationFlag, GetBiosTableRequest.TableType));
        
        switch (TableType)
        {
            case PldmStringTable:
              Status =  GetPLDMDataFromBMC(PldmPayloadHead, GetBiosTableRequest, TableType, StringTable, &StringSize);
              DEBUG((DEBUG_ERROR, "\n RestoreConfigurationFromBmc: PldmStringTable GetPLDMDataFromBMC Status: %x", Status));
              if(Status == EFI_SUCCESS)
              {
            	  PldmTables->StringTable = *StringTable ;
            	  PldmTables->StringSize = StringSize ;
              }
              else
              {
            	  PldmTables->StringTable = (UINT8 *)NULL ;
            	  PldmTables->StringSize = 0;
              }
              break;
            
            case PldmAttributeTable:
              Status =  GetPLDMDataFromBMC(PldmPayloadHead, GetBiosTableRequest, TableType, AttributeTable, &AttributeSize);
              DEBUG((DEBUG_ERROR, "\n RestoreConfigurationFromBmc: PldmAttributeTable GetPLDMDataFromBMC Status: %x", Status));
              if(Status == EFI_SUCCESS)
              {
            	  PldmTables->AttributeTable = *AttributeTable ;
            	  PldmTables->AttributeSize = AttributeSize;
              }
              else
              {
            	  PldmTables->AttributeTable = (UINT8 *)NULL ;
            	  PldmTables->AttributeSize = 0;
              }
              break;
            
            case PldmValueTable:
              Status =  GetPLDMDataFromBMC(PldmPayloadHead, GetBiosTableRequest, TableType, ValueTable, &ValueSize);
              DEBUG((DEBUG_ERROR, "\n RestoreConfigurationFromBmc: PldmValueTable GetPLDMDataFromBMC Status: %x", Status));
              if(Status == EFI_SUCCESS)
              {
            	  PldmTables->ValueTable = *ValueTable ;
            	  PldmTables->ValueSize = ValueSize ;
              }
              else
              {
            	  PldmTables->ValueTable = (UINT8 *)NULL ;
            	  PldmTables->ValueSize = 0;
              }
              break;
            
            case PldmMapStringTable:
              Status =  GetPLDMDataFromBMC(PldmPayloadHead, GetBiosTableRequest, TableType, MapStringTable, &MapStringSize);
              DEBUG((DEBUG_ERROR, "\n RestoreConfigurationFromBmc: PldmMapStringTable GetPLDMDataFromBMC Status: %x", Status));
              if(Status == EFI_SUCCESS)
              {
            	  PldmTables->MapStringTable = *MapStringTable ;
            	  PldmTables->MapSize = MapStringSize ;
              }
              else
              {
            	  PldmTables->MapStringTable = (UINT8 *)NULL ;
            	  PldmTables->MapSize = 0;
              }
              break;
                                         
            default:
              DEBUG((DEBUG_ERROR, "\n RestoreConfigurationFromBmc: Not Valid Table Type:%d", TableType));
        }
        
        if(EFI_SUCCESS != Status)
        {
            return Status;
        }
    }
    
    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure:   PldmSaveConfigurationInBmc
//
// Description: To Save the BIOS configuration PLDM Tables in BMC.
//
// Parameter:   IN  UINT8  *ValueTable,     // ValueTable pointer 
//              IN  UINT32  ValueSize,      // value table size 
//              IN  UINT8  *AttributeTable, // AttributeTable table pointer
//              IN   UINT32  AttributeSize, //AttributeTable Size
//              IN   UINT8  *StringTable,   //English String Table Pointer 
//              IN   UINT32  StringSize,    //English string table size
//              IN  UINT8  *MapStringTable, // x-AMI string table from foreign system
//              IN  UINT32  MapStringSize   //Map String Table Size
//
// Return Value: EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS PldmSaveConfigurationInBmc(  
        PLDMTables *PldmTables
    )
{
    UINT8  *ValueTable;
    UINT32  ValueSize;     
    UINT8  *AttributeTable;
    UINT32  AttributeSize;
    UINT8  *StringTable;
    UINT32  StringSize;
    UINT8  *MapStringTable;
    UINT32  MapStringSize; 
    EFI_STATUS Status = EFI_SUCCESS;
    BMC_PLDMPAYLOAD PldmPayloadHead = {0};
    BYTE TableType;
    
    ValueTable = PldmTables->ValueTable;
    ValueSize = PldmTables->ValueSize;
    AttributeTable = PldmTables->AttributeTable;
    AttributeSize = PldmTables->AttributeSize;
    StringTable = PldmTables->StringTable;
    StringSize = PldmTables->StringSize;
    MapStringTable = PldmTables->MapStringTable;
    MapStringSize = PldmTables->MapSize;
    
    PldmPayloadHead.Request = 1;
    PldmPayloadHead.Datagram = 0;
    PldmPayloadHead.Reserved = 0;
    PldmPayloadHead.InstanceID = 0;
    PldmPayloadHead.HdrVer = 0;
    PldmPayloadHead.PLDMType = 0x03;//PLDM for BIOS_Control_and_Configuration
    PldmPayloadHead.PLDMCommand = PLDM_SET_BIOSTABLE;//PLDM_SET_BIOSTABLE to set the Tables to BMC
            
    for(TableType = PldmStringTable; TableType <= PldmMapStringTable; TableType++)
    {
        PLDM_SETBIOSTABLE_REQUEST SetBiosTableRequest = {0};
                
        PldmPayloadHead.InstanceID = TableType;
        
        //Fill the Set Table Request
        SetBiosTableRequest.Handle = 0;
        SetBiosTableRequest.TableType = TableType;
                    
        switch (TableType)
        {
          case PldmStringTable:
              Status =  SetPLDMDataToBMC(PldmPayloadHead, SetBiosTableRequest, TableType, StringTable, StringSize);
              DEBUG((DEBUG_ERROR, "\n SaveConfigurationInBmc PldmStringTable SetPLDMDataToBMC Status: %x", Status));
              break;
          
          case PldmAttributeTable:
              Status =  SetPLDMDataToBMC(PldmPayloadHead, SetBiosTableRequest, TableType, AttributeTable, AttributeSize);
              DEBUG((DEBUG_ERROR, "\n SaveConfigurationInBmc PldmAttributeTable SetPLDMDataToBMC Status: %x", Status));
              break;
          
          case PldmValueTable:
              Status =  SetPLDMDataToBMC(PldmPayloadHead, SetBiosTableRequest, TableType, ValueTable, ValueSize);
              DEBUG((DEBUG_ERROR, "\n SaveConfigurationInBmc PldmValueTable SetPLDMDataToBMC Status: %x", Status));
              break;
          
          case PldmMapStringTable:
              Status =  SetPLDMDataToBMC(PldmPayloadHead, SetBiosTableRequest, TableType, MapStringTable, MapStringSize);
              DEBUG((DEBUG_ERROR, "\n SaveConfigurationInBmc PldmMapStringTable SetPLDMDataToBMC Status: %x", Status));
              break;
                                         
          default:
              DEBUG((DEBUG_ERROR, "\n SaveConfigurationInBmc :Not Valid Table Type:%d", TableType));
        }
        if(EFI_SUCCESS != Status)
        {
            return Status;
        }
    }
    
    return Status;
}

EFI_STATUS EFIAPI PldmSetTagsInBmc (PLDMTableTags *TableTags)
{
    EFI_STATUS Status;
    
    Status = SendTagsCommandToBmc( PLDM_SET_BIOS_TABLE_TAGS, TableTags );
            
    return Status;
}

EFI_STATUS EFIAPI PldmGetTagsFromBmc (PLDMTableTags *TableTags)
{
    EFI_STATUS Status;
    
    Status = SendTagsCommandToBmc( PLDM_GET_BIOS_TABLE_TAGS, TableTags );

    return Status;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure    : SavePldmDataTables
//
// Description  : Defines the method to save the PLDM data Tables
//
// Parameter    : PLDMTables *PldmTables  
//
// Return Value : EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS SavePldmDataTables(  
        PLDMTables *PldmTables
    )
{
    //Define the custom method to Save the PLDM Tables
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure    : RestorePldmByCustomMethod
//
// Description  : Defines the method to restore the PLDM Tables data
//
// Parameter    :   
//
// Return Value : EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS RestorePldmDataTables( PLDMTables *PldmTables )
{
    //Define the custom method to restore the PLDM Tables
    return EFI_SUCCESS;
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure    : SavePldmByCustomMethod
//
// Description  : Invokes the defined methods to save the PLDM configuration Tables  
//
// Parameter    : PLDMTables *PldmTables  
//
// Return Value : EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI SavePldmByCustomMethod (PLDMTables *PldmTables)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN i = 0;

    for (i = 0; SavePldmDataTablesList[i] && (Status == EFI_SUCCESS); i++)
    {
       Status = SavePldmDataTablesList[i](PldmTables);
    }
    
    return Status;   
}

//<AMI_PHDR_START>
//--------------------------------------------------------------------
// Procedure    : RestorePldmByCustomMethod
//
// Description  : Invokes the defined methods to save the PLDM configuration Tables
//
// Parameter    : PLDMTables *PldmTables  
//
// Return Value : EFI_STATUS
//--------------------------------------------------------------------
//<AMI_PHDR_END>
EFI_STATUS EFIAPI RestorePldmByCustomMethod (PLDMTables *PldmTables)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN i = 0;
    
    for (i = 0; RestorePldmDataTablesList[i] && (Status == EFI_SUCCESS); i++)
    {
       Status = RestorePldmDataTablesList[i](PldmTables);
    }
    
    return Status;  
}

//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2015, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**    5555 Oakbrook Pkwy, Norcross, Suite 200, Georgia 30093   **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
