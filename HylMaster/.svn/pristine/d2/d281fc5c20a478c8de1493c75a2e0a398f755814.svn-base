/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
*/

/** @file AcpiUpdateCpuInfo.c
    This file is for Updating Cpu Info in SSDT
**/
#include <Token.h>
#include "AcpiUpdateCpuInfo.h"

/**
*  Updates Cpu Info in SSDT table
*  @param   EFI_HANDLE                Image handle
*  @param   EFI_SYSTEM_TABLE          Pointer to EFI System table.
*  
*  @retval  EFI_SUCCESS               Function completed successfully.
*  @retval  Other                     Some error occurred during Initialize.
* 
*/
VOID
EFIAPI
UpdateCpuInfoInDsdt(
        IN EFI_EVENT       Event,
        IN VOID *Context
  )
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_ACPI_TABLE_PROTOCOL                     *AcpiTableProtocol  = NULL;
    EFI_GUID gAmiCpuinfoHobGuid					= AMI_CPUINFO_HOB_GUID;
    UINT32                                      Signature = 
                    EFI_ACPI_5_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE;
    UINTN                                       Index = 0;
    CPUINFO_HOB             					*gCpuInfoHob;
    VOID                    					*FirstHob;
    EFI_ACPI_DESCRIPTION_HEADER                 *DsdtTable = NULL;
    UINTN                                       TableKey = 0;
    UINT32                                      TotalCpuCnt;
    UINT8                                       *ProcAsl;
    PROC_TEMPLATE                               *ProcTemplate;
    SB_DEV_HEADER                               *SbDevHeader;
    SB_DEV_BODY                                 *SbDevBody;
    UINT8                                       CpuNum;
    UINT8                                       i;
    UINT8                                       *temp;
    UINT16                                      ScopeLength;
    UINT32                                      MemoryToAllocate;
    UINT8                  			CoreNumberHi;
    UINT8                  			CoreNumberLo;
    
   
    DEBUG((-1, "In SSDT callback \n"));
    //Locate AcpiTable protocol, if failed return status
    Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, 
                                                        &AcpiTableProtocol);
    if(EFI_ERROR(Status)) {
        return;
    }
    
    FirstHob = GetEfiConfigurationTable(gST, &gEfiHobListGuid);
	if (!FirstHob) return;
	
    gCpuInfoHob = (CPUINFO_HOB*)FirstHob;

	//Find CPUINFO_HOB Hob.
	while (!EFI_ERROR(Status = FindNextHobByType(EFI_HOB_TYPE_GUID_EXTENSION, &gCpuInfoHob)))
	{
		if (guidcmp(&gCpuInfoHob->EfiHobGuidType.Name, &gAmiCpuinfoHobGuid) == 0) break;
	}
	if (EFI_ERROR(Status)) return;
   
    //Get number of entries
    TotalCpuCnt = (UINT32)gCpuInfoHob->CpuCount;
        
    //Calculate Total Memory needed for new table
    //2 bytes to size and 5 to scope and 1 byte to lead byte
	if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
	  MemoryToAllocate = (UINT32)(((TotalCpuCnt * (SBDEVH_STRUCT_SIZE + SBDEVB_STRUCT_SIZE)) + sizeof(EFI_ACPI_DESCRIPTION_HEADER)) + 8);
	} else {
	  MemoryToAllocate = (UINT32)(((TotalCpuCnt * sizeof(PROC_TEMPLATE)) + sizeof(EFI_ACPI_DESCRIPTION_HEADER)) + 8);
    }
    
    //Allocate Memory
    Status = gBS->AllocatePool(
                       EfiBootServicesData,
                       MemoryToAllocate,
                       (VOID **)&ProcAsl
                       );
           
    if (EFI_ERROR(Status))  {
        return;
    }
    
    //Clear the allocated Memory
    gBS->SetMem (ProcAsl, MemoryToAllocate, 0);
    
    temp = (UINT8 *)ProcAsl + sizeof(EFI_ACPI_DESCRIPTION_HEADER);
    *temp = 0x10;
    temp++;
    //Below is new scope length
    if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
      ScopeLength = (UINT16)(TotalCpuCnt * (SBDEVH_STRUCT_SIZE + SBDEVB_STRUCT_SIZE) + 7); //2 bytes to size and 5 to scope
    } else {
      ScopeLength = (UINT16)(TotalCpuCnt * sizeof(PROC_TEMPLATE) + 7); //2 bytes to size and 5 to scope
    }
	
    //Set new scope to new DSDT table
    *temp = (UINT8)((ScopeLength & 0xf) | 0x40);
    temp++;
    *temp = (UINT8)((ScopeLength & 0xff0) >> 4);
    temp++;
    // \_PR or \_SB
    *temp = 0x5C; 
    temp++;
    *temp = '_';
    temp++;
    if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
      *temp = 'S';
      temp++;
      *temp = 'B';
      temp++;
    } else {
      *temp = 'P';
      temp++;
      *temp = 'R';
      temp++;
    }
    *temp = '_';
    temp++;

    //Add Processor Device objects
    if (PcdGetBool (PcdAmdAcpiCpuSsdtProcessorScopeInSb)) {
      SbDevHeader = (SB_DEV_HEADER *)(temp);
    
      for(i = 0; i < TotalCpuCnt; i++)  {
          CpuNum = i;
          SbDevHeader->SbDevOpcode = 0x825B;
          SbDevHeader->PkgLength   = 0x19;
        
          SbDevHeader->SbDevName_C    = 'C';
          SbDevHeader->SbDevName_P    = '0';

          CoreNumberHi = (CpuNum >> 4) & 0x0F;
          SbDevHeader->SbDevName_U    = (UINT8) (SCOPE_NAME_0 + CoreNumberHi);

          CoreNumberLo = CpuNum & 0x0F;
          if (CoreNumberLo < 0xA) {
            SbDevHeader->SbDevName_0  = (UINT8) (SCOPE_NAME_0 + CoreNumberLo);
          } else {
            SbDevHeader->SbDevName_0  = (UINT8) (SCOPE_NAME_A + CoreNumberLo - 0xA);
          }
        
          SbDevBody = (SB_DEV_BODY *) (SbDevHeader + 1);
        
          SbDevBody->HidNameOpcode               = 0x8;
          SbDevBody->HidName_a__                 = HID_NAME__;
          SbDevBody->HidName_a_H                 = HID_NAME_H;
          SbDevBody->HidName_a_I                 = HID_NAME_I;
          SbDevBody->HidName_a_D                 = HID_NAME_D;
          SbDevBody->StringPrefixOpcode          = 0xD;
          SbDevBody->ProcHid_a_A                 = PROC_HID_A;
          SbDevBody->ProcHid_a_C                 = PROC_HID_C;
          SbDevBody->ProcHid_a_P                 = PROC_HID_P;
          SbDevBody->ProcHid_a_I                 = PROC_HID_I;
          SbDevBody->ProcHid_a_0                 = PROC_HID_0;
          SbDevBody->ProcHid_b_0                 = PROC_HID_0;
          SbDevBody->ProcHid_c_0                 = PROC_HID_0;
          SbDevBody->ProcHid_a_7                 = PROC_HID_7;
          SbDevBody->NullChar                    = 0; 
        
          SbDevBody->UidNameOpcode               = 0x8;
          SbDevBody->UidName_a__                 = UID_NAME__;
          SbDevBody->UidName_a_U                 = UID_NAME_U;
          SbDevBody->UidName_a_I                 = UID_NAME_I;
          SbDevBody->UidName_a_D                 = UID_NAME_D;
          SbDevBody->BytePrefixOpcode            = 0xA;
          SbDevBody->ApicId                      = CpuNum;
        
          SbDevHeader = (SB_DEV_HEADER *) (SbDevBody + 1);
        }
      } else {
        ProcTemplate = (PROC_TEMPLATE *)(temp);
    
        for(i = 0; i < TotalCpuCnt; i++)  {
          CpuNum = i;
          ProcTemplate->ProcOpcode = 0x835B;
          ProcTemplate->ProcPkgLen = 0x0B;
          SetProcString(ProcTemplate->ProcNameStr, CpuNum);
          ProcTemplate->ProcID = CpuNum;
          ProcTemplate->PblkAddr[3] =  0x0;
          ProcTemplate->PblkAddr[2] =  0x00;
          if(PcdGet8(PcdAmdCStateMode) == 0){
              ProcTemplate->PblkAddr[1] =  0x00;
              ProcTemplate->PblkAddr[0] =  0x00;
              ProcTemplate->PblkLen     =  0;
          }else{
              ProcTemplate->PblkAddr[1] =  0x08;
              ProcTemplate->PblkAddr[0] =  0x10;
              ProcTemplate->PblkLen     =  6;              
          }
          ProcTemplate++;
        }
      }
    
    
    //Fill ACPI Header
    FillAcpiHeader((EFI_ACPI_DESCRIPTION_HEADER *)ProcAsl, EFI_ACPI_5_1_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE, MemoryToAllocate);
    Status = AcpiTableProtocol->InstallAcpiTable(AcpiTableProtocol, ProcAsl, MemoryToAllocate, &TableKey);
    gBS->FreePool(ProcAsl);

}


/**
*  Entry point for the module
*  Creates callback on gEfiAcpiTableProtocolGuid, so this SSDT is installed first. 
*  @param   EFI_HANDLE                Image handle
*  @param   EFI_SYSTEM_TABLE          Pointer to EFI System table.
*  
*  @retval  EFI_SUCCESS               Function completed successfully.
*  @retval  Other                     Some error occurred during Initialize.
* 
*/
EFI_STATUS
CpuAcpiTableEntryPoint(
        IN EFI_HANDLE       ImageHandle,
        IN EFI_SYSTEM_TABLE *SystemTable
  )
{
    
    EFI_EVENT AcpiTableProtocolEvent;
    VOID * Registration;

    AcpiTableProtocolEvent = EfiCreateProtocolNotifyEvent(
                                &gEfiAcpiTableProtocolGuid,
                                TPL_NOTIFY,
                                UpdateCpuInfoInDsdt,
                                NULL,
                                &Registration
                                );
    
    return EFI_SUCCESS;
                               
    
}

/** 
* Fill the SSDT ACPI Table Header.
*
*   @param  pAcpiHdr    Pointer to ACPI Header
*   
*   @param  Signature   Signature of the ACPI Table
*   
*   @param TblLength     Length of the ACPI Table
* 
*/
VOID 
FillAcpiHeader(
        EFI_ACPI_DESCRIPTION_HEADER *pAcpiHdr, 
        UINT32 Signature, 
        UINTN TblLength
  )
{
    pAcpiHdr->Signature = Signature;
    pAcpiHdr->Length = (UINT32) TblLength;
    pAcpiHdr->Revision = 2;
    CopyMem((VOID *)pAcpiHdr->OemId, (VOID *)OemId, 6);
    CopyMem((VOID *)&pAcpiHdr->OemTableId, (VOID *)OemTableId, 8);
    pAcpiHdr->OemRevision = ACPI_OEM_REV;
    pAcpiHdr->CreatorId = CREATOR_ID_AMI;
    pAcpiHdr->CreatorRevision = ACPI_OEM_REV;
    pAcpiHdr->Checksum = ChsumTbl((UINT8*)pAcpiHdr, pAcpiHdr->Length);
    
}


char * ItoaCpuHelper(INT64 value, char *string,int radix, BOOLEAN is_int64) {
	UINTN digit;
	UINT64 v = (value>=0) 
			   ? (UINT64) value
			   : (radix==10) 
				 ? (UINT64)-value 
				 : (is_int64) 
				   ? (UINT64)value 
				   : (UINT32)value;
	if (v)
		while (v) {
            #ifdef MDE_CPU_ARM
		        v = DivU64x32Remainder(v,radix,&digit);
            #else
		        v = Div64(v,radix,&digit);
            #endif // MDE_CPU_ARM
			if (digit<0xa) *string=(char)(digit+'0');
			else *string=(char)(digit-0xa+'A');
			string++;
		}
	else *string++='0';
	if (radix==10 && value<0) *string++='-';
	*string--=0;
	return string;
}


char * ItoaCpuEx(INT64 value, char *string,int radix, BOOLEAN is_int64) {
	char *strp=string;
	char *str2p=ItoaCpuHelper(value,strp,radix,is_int64);
	//reverse string
	while(strp<str2p) {
		char temp=*strp;
		*strp=*str2p;
		*str2p=temp;
		strp++;str2p--;
	}
	return string;
}

/** 
* Set Processor name string
*
*   @param  ProcNameStr    Pointer to the String location
*   
*   @param  CpuNumber      Cpu id number
*   
* 
*/
VOID 
SetProcString(
        UINT8 *ProcNameStr, 
        UINT8 CpuNumber
  )
{
	CHAR8 Digit[2];
	
    if (CpuNumber > 0xff) return;
    ASSERT(CpuNumber <= 0xff);

    ProcNameStr[1] = '0';
    ProcNameStr[0] = 'C';
    
    ItoaCpuEx(CpuNumber, Digit, 16, FALSE);
    
    if(CpuNumber <= 0xf ){
    	ProcNameStr[3] = (UINT8) Digit[0];
    	ProcNameStr[2] = '0';
    }
    else{
        ProcNameStr[3] = (UINT8) Digit[1];
        ProcNameStr[2] = (UINT8) Digit[0];
    }

}


/** 
* calculates the checksum of a table starting at TblStart of length 
* BytesCount and returns the checksum value.
*
*   @param  TblStart    Starting address of the memory area to checksum
*   
*   @param  BytesCount  Length in bytes of the memory area to checksum
*   
*   @retval UINT8       value starting from TblStart and ending at 
*                       TblStart + BytesCount.
* 
*/
UINT8 
ChsumTbl(
        UINT8* TblStart, 
        UINT32 BytesCount
  )
{
    UINTN       i;
    UINT8       res=*TblStart;
    
    for (i=1; i<BytesCount; i++) res+=TblStart[i];
    
    res=0-res;
    return(res);
}



/**
//****************************************************************************
//****************************************************************************
//**                                                                        **
//**             (C)Copyright 1985-2019, American Megatrends, Inc.          **
//**                                                                        **
//**                          All Rights Reserved.                          **
//**                                                                        **
//**             5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093          **
//**                                                                        **
//**                          Phone (770)-246-8600                          **
//**                                                                        **
//****************************************************************************
//****************************************************************************
*/
