#include <Library/UefiBootServicesTableLib.h>
#include <AmiLib.h>
#include <Acpi20.h>
#include <Library/UefiLib.h>
#include "ofbd_cfg.h"

VOID    AFU_ACPITable_CheckFlow();
VOID    AFU_ACPITable_FTF_MLAP_String_Check(PBYTE buff);
EFI_STATUS LocateEfiConfigurationTable(EFI_GUID *VendorGuid, VOID **VendorTable);
EFI_STATUS UC2_GetAcpiSmiPort(UINT16 *SmiPort);
UINT32 UC2_GetAcpiGenericTable(UINT32 Name, VOID *Buffer, UINT32 BufferSize);
UINT32 UC2_GetAcpiDsdt(VOID *Buffer, UINT32 BufferSize);
UINT32 UC2_GetAcpiTable(UINT32 Name, VOID *Buffer, UINT32 BufferSize);

extern VOID PrintGuid2(UINT8 *pGuid);
BOOLEAN  AFU_bMLAP_APL_Enable = FALSE;

VOID    AFU_ACPITable_CheckFlow()
{
    PBYTE   pDSDTBuf = (PBYTE)NULL;
    UINT32  DSDTSize = 0;
    UINT32  sigFadt = 0x54445344; // "DSDT
    UINT32 i;
    PBYTE pDSDT_Tempbuff;
    
    DSDTSize = UC2_GetAcpiTable(sigFadt, pDSDTBuf, DSDTSize);
    //Print(L" DSDTSize = 0x%x\n",DSDTSize);
    pDSDTBuf = AllocMem(DSDTSize);
    DSDTSize = UC2_GetAcpiTable(sigFadt, pDSDTBuf, DSDTSize);
    //Print(L" DSDTSize = 0x%x\n",DSDTSize);
    if(pDSDTBuf)
    {
        for (i = 0 ; i < DSDTSize ; i++)
        {   
            pDSDT_Tempbuff = pDSDTBuf+i;

            AFU_ACPITable_FTF_MLAP_String_Check(pDSDT_Tempbuff);
        }
    }
}

VOID     AFU_ACPITable_FTF_MLAP_String_Check(PBYTE buff)
{
    UINT8* pMLAPValue;
    
    if (0 == MemCmp (buff, "MLAP", 4))
    {
        pMLAPValue = (UINT8*)(buff + 4);
        //Print(L"MLAPValue = 0x%x\n", *pMLAPValue);
        if (*pMLAPValue == 1)
        {
            //Print(L" FTF Mode Support.\n");
        }
        else if (*pMLAPValue == 0)
        {
            //Print(L" APL Platform Support.\n");
            AFU_bMLAP_APL_Enable = TRUE;
        }
    }
}

EFI_STATUS LocateEfiConfigurationTable(EFI_GUID *VendorGuid, VOID **VendorTable)
{
	UINTN						countSysTables	= gST->NumberOfTableEntries;
	EFI_CONFIGURATION_TABLE		*pConfigTable	= gST->ConfigurationTable;
	UINTN i;
	
	//Print(L"countSysTables :%d\n",countSysTables);
	for (i = 0; i < countSysTables; i++)
		if (MemCmp((VOID *)&(pConfigTable[i].VendorGuid), (VOID *)VendorGuid, sizeof(EFI_GUID)) == 0) {
		    //Print(L"%d ==========================\n",i);
		    //PrintGuid2((UINT8*)&(pConfigTable[i].VendorGuid));
		    //PrintGuid2((UINT8*)VendorGuid);
			*VendorTable = pConfigTable[i].VendorTable;
			return EFI_SUCCESS;
		}

	return EFI_NOT_FOUND;
}

EFI_STATUS UC2_GetAcpiSmiPort(UINT16 *SmiPort)
{
	UINT32	sigFadt = 0x50434146;	// "FACP" Fixed ACPI Description Table (FADT)
	UINT32	size = 0;
	UINT8	*buffer = 0;

	size = UC2_GetAcpiTable(sigFadt, buffer, size);
	if (size) {
		buffer = (UINT8 *)AllocMem(size);
		UC2_GetAcpiTable(sigFadt, buffer, size);
	} else
		return EFI_NOT_FOUND;

	*SmiPort = (UINT16)(((PFACP_20)buffer)->SMI_CMD);
	FreeMem(buffer);

	return EFI_SUCCESS;
}

UINT32 UC2_GetAcpiGenericTable(UINT32 Name, VOID *Buffer, UINT32 BufferSize)
{
    EFI_STATUS		Status;

	PRSDT_PTR_20	ptrRsdp = 0;
	PRSDT32			ptrRsdt = 0;
	PXSDT_20		ptrXsdt = 0;

	EFI_GUID		guidAcpi10 = ACPI_10_TABLE_GUID;
	EFI_GUID		guidAcpi20 = EFI_ACPI_20_TABLE_GUID;

	UINTN i;
	
	// ACPI 1.0
	Status = LocateEfiConfigurationTable(&guidAcpi10, (VOID **)&ptrRsdp);
	if(EFI_SUCCESS == Status) {
		ptrRsdt = (PRSDT32)(ptrRsdp->RsdtAddr);
		for (i = 0; i < (ptrRsdt->Header.Length - sizeof(ACPI_HDR)) / sizeof(UINT32); i++) {
			if (((ACPI_HDR *)(ptrRsdt->Ptrs[i]))->Signature == Name) {
				if (BufferSize >= ((ACPI_HDR *)(ptrRsdt->Ptrs[i]))->Length)
					MemCpy(Buffer, (VOID *)(ptrRsdt->Ptrs[i]), ((ACPI_HDR *)(ptrRsdt->Ptrs[i]))->Length);
				return ((ACPI_HDR *)(ptrRsdt->Ptrs[i]))->Length;
			}
		}
	}

	// ACPI 2.0
	Status = LocateEfiConfigurationTable(&guidAcpi20, (VOID **)&ptrRsdp);
	if(EFI_SUCCESS == Status) {
		ptrXsdt = (PXSDT_20)(ptrRsdp->XsdtAddr);
		for (i = 0; i < (ptrXsdt->Header.Length - sizeof(ACPI_HDR)) / sizeof(UINT64); i++) {
			if (((ACPI_HDR *)(ptrXsdt->Ptrs[i]))->Signature == Name) {
				if (BufferSize >= ((ACPI_HDR *)(ptrXsdt->Ptrs[i]))->Length)
					MemCpy(Buffer, (VOID *)(ptrXsdt->Ptrs[i]), ((ACPI_HDR *)(ptrXsdt->Ptrs[i]))->Length);
				return ((ACPI_HDR *)(ptrXsdt->Ptrs[i]))->Length;
			}
		}
	}

	return 0;
}

UINT32 UC2_GetAcpiDsdt(VOID *Buffer, UINT32 BufferSize)
{
	UINT32		sigFadt = 0x50434146;	// "FACP" Fixed ACPI Description Table
	PFACP_20	buffer = 0;
	UINT32		fadtSize = 0;
	UINT32      DSTDSize = 0;

	fadtSize = UC2_GetAcpiGenericTable(sigFadt, (VOID *)buffer, fadtSize);
	//Print(L" fadtSize = 0x%x\n",fadtSize);
	if (fadtSize) {
		buffer = (PFACP_20)AllocMem(fadtSize);
		if (fadtSize == UC2_GetAcpiTable(sigFadt, (VOID *)buffer, fadtSize))		// Get FADT
		{
			if (BufferSize >= ((ACPI_HDR *)(buffer->DSDT))->Length)
			{
				MemCpy(Buffer, (VOID *)(buffer->DSDT), ((ACPI_HDR *)(buffer->DSDT))->Length);
				DSTDSize = ((ACPI_HDR *)(buffer->DSDT))->Length;    

				FreeMem((PBYTE)buffer);
		        return DSTDSize;
			}		
		}

		DSTDSize = ((ACPI_HDR *)(buffer->DSDT))->Length;
		FreeMem((PBYTE)buffer);

		return DSTDSize;
	}

	return 0;
}

UINT32 UC2_GetAcpiTable(UINT32 Name, VOID *Buffer, UINT32 BufferSize)
{
    switch (Name) {
        case 0x54445344 :       // "DSDT"
            return UC2_GetAcpiDsdt(Buffer, BufferSize);
        default :
            return UC2_GetAcpiGenericTable(Name, Buffer, BufferSize);
    }
}
