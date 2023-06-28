//**********************************************************************
// $Header:Oem OEM IPMI Memory Info
//
// $Revision: 0.01
//
// $Date:  <Lizhq-20161217>
//         Base On OEM IPMI SPEC 20161217, create OEM IPMI Memory Information module.
//**********************************************************************
#include <setup.h>
#include <AmiDxeLib.h>  //<Wangyia001-20170509 Send Mem Info to BMC>
#include <OemIpmiNetFnDefinitions.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
//<Wangyia001-20170509 Send Mem Info to BMC +>
#include <Protocol/Smbios.h>
#include "IndustryStandard/SmBios.h" 
#include  <Library/PcdLib.h>      //<Malj-20181105 Add Mem Info Send to BMC> 

#define TSTR_SIZE                           SMBIOS_STRING_MAX_LENGTH      //temp string length
#define MAXIMUM_MEM_MANUF_STRING_LENGTH     SMBIOS_STRING_MAX_LENGTH
#define MAXIMUM_MEM_PARTNUM_STRING_LENGTH   SMBIOS_STRING_MAX_LENGTH
#define MAXIMUM_MEM_SERIALNUM_STRING_LENGTH SMBIOS_STRING_MAX_LENGTH

extern EFI_BOOT_SERVICES *gBS;  //<Wangyia001-20170506 Send CPU info to BMC>

/**

  Acquire the string associated with the Index from SMBIOS structure and return it.
  The caller is responsible for freeing the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS       The function returns EFI_SUCCESS if successful.
  @retval   EFI_NOT_FOUND     The function returns EFI_NOT_FOUND if unsuccessful.  

**/
EFI_STATUS
SmbiosGetOptionalStringByIndex (
  IN      CHAR8                   *OptionalStrStart,
  IN      UINT8                   Index,
  OUT     CHAR16                  **String
  )
{
  UINTN          StrSize;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart); // size includes null terminator
  // SMBIOS strings are NULL terminated, and end of all strings is indicated with NULL
  // loop until at end of all SMBIOS strings (found NULL terminator at string index past string's NULL), and Index != 0
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero
    return EFI_INVALID_PARAMETER;
  } else {
    AsciiStrToUnicodeStr (OptionalStrStart, *String);
  }

  return EFI_SUCCESS;
}
  
/**
    function to free the pointers

    @param ptr 

    @retval void

**/
VOID MemFreePointer( VOID **ptr )
{
  if ( ( ptr == NULL ) || ( *ptr == NULL ) )
    return;

  gBS->FreePool( *ptr );
  *ptr = NULL;
}
//<Wangyia001-20170509 Send Mem Info to BMC ->

//OEM IPMI Set Dimm Info to BMC
EFI_STATUS
OemIpmiSetDimmInfo(VOID)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  PACKED_DIMM_INFO            OemDimmInfo;
  SMBIOS_TABLE_TYPE17         *SmbiosType17Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_SMBIOS_TYPE             SmbiosType;
  EFI_SMBIOS_TABLE_HEADER     *SmbiosRecord;
  CHAR16                      *NewString = NULL;
  CHAR16                      *TempManuf = NULL;
  CHAR16                      *TempPartNum = NULL;
  CHAR16                      *TempSN = NULL;
  UINT8                       StrIndex;
  UINT32                      i;
  UINT8   			  		  ResponseDataSize=0;

  DEBUG((EFI_D_INFO, __FUNCTION__" Entered \n"));

  //<Wangyia001-20170509 Send Mem Info to BMC +>  
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  DEBUG((EFI_D_INFO, "GetMemoryInfo gEfiSmbiosProtocolGuid Status: %r\n",Status));
  if(EFI_ERROR(Status)) return Status;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;

  NewString = AllocateZeroPool (TSTR_SIZE);
  if(NewString == NULL) {
    DEBUG((EFI_D_ERROR, "Allocate ZERO memory failure: 0x%x\n", NewString));
    return Status;
  }
  TempManuf = AllocateZeroPool (MAXIMUM_MEM_MANUF_STRING_LENGTH);
    if(TempManuf == NULL) {
    DEBUG((EFI_D_ERROR, "Allocate ZERO memory failure: 0x%x\n", TempManuf));
    return Status;
  }
  TempPartNum = AllocateZeroPool (MAXIMUM_MEM_PARTNUM_STRING_LENGTH);
    if(TempPartNum == NULL) {
    DEBUG((EFI_D_ERROR, "Allocate ZERO memory failure: 0x%x\n", TempPartNum));
    return Status;
  }
  TempSN = AllocateZeroPool (MAXIMUM_MEM_SERIALNUM_STRING_LENGTH);
    if(TempSN == NULL) {
    DEBUG((EFI_D_ERROR, "Allocate ZERO memory failure: 0x%x\n", TempSN));
    return Status;
  }

  for (i = 0; ; ++i) {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    DEBUG((EFI_D_INFO, "SmbiosRecord Status: %r\n",Status));
    //Package buffer init.
    ZeroMem((VOID*)&OemDimmInfo, sizeof(PACKED_DIMM_INFO));
    ZeroMem((VOID*)NewString, sizeof(TSTR_SIZE));
    ZeroMem((VOID*)TempManuf, sizeof(MAXIMUM_MEM_MANUF_STRING_LENGTH));
    ZeroMem((VOID*)TempPartNum, sizeof(MAXIMUM_MEM_PARTNUM_STRING_LENGTH));
    ZeroMem((VOID*)TempSN, sizeof(MAXIMUM_MEM_SERIALNUM_STRING_LENGTH));

    SmbiosType17Record = (SMBIOS_TABLE_TYPE17 *) SmbiosRecord;
    
    if (SmbiosType17Record->Size == 0) {
      OemDimmInfo.DimmPresent = 0;
      continue;
    } else {
      OemDimmInfo.DimmPresent = 1;
    }

    StrIndex = SmbiosType17Record->DeviceLocator;
    Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType17Record + SmbiosType17Record->Hdr.Length), StrIndex, &NewString);
    DEBUG((EFI_D_INFO, "SmbiosType17Record DeviceLocator Status: %r\n",Status));
    ASSERT_EFI_ERROR(Status);
    
    OemDimmInfo.DimmNode = (UINT8)(NewString[3] - 0x30);
    OemDimmInfo.DimmChannel = (UINT8)(NewString[9] - 0x41);
    OemDimmInfo.DimmNum = (UINT8)(NewString[10] - 0x30);  
    DEBUG((EFI_D_INFO, "socket=%d,channel=%d,dimm=%d\n",OemDimmInfo.DimmNode,OemDimmInfo.DimmChannel,OemDimmInfo.DimmNum));    

    OemDimmInfo.DimmSizeUnit = Unit_GB; 
    OemDimmInfo.DimmSize  = SmbiosType17Record->Size >> 10;
    if (SmbiosType17Record->Size == 0x7FFF) {
      OemDimmInfo.DimmSize = SmbiosType17Record->ExtendedSize >> 10;
    }
    
    OemDimmInfo.DramType = 0x04; //DDR4, may need change
    OemDimmInfo.MemoryType = 0x01; //may need change
    if (OemDimmInfo.DramType != 0x03) { //not SPD_TYPE_DDR3
      switch (SmbiosType17Record->ConfiguredMemoryClockSpeed) {
      case 800:
        OemDimmInfo.DimmWorkFreq = MemFreq800MT;
        break;
      case 1000:
        OemDimmInfo.DimmWorkFreq = MemFreq1000MT;
        break;
      case 1066:                            
      case 1067:       
        OemDimmInfo.DimmWorkFreq = MemFreq1067MT;
        break;
      case 1200:
        OemDimmInfo.DimmWorkFreq = MemFreq1200MT;
        break;
      case 1333:                            
      case 1334:
        OemDimmInfo.DimmWorkFreq = MemFreq1333MT;
        break;
      case 1400:
        OemDimmInfo.DimmWorkFreq = MemFreq1400MT;
        break;
      case 1600:
        OemDimmInfo.DimmWorkFreq = MemFreq1600MT;
        break;
      case 1800:
        OemDimmInfo.DimmWorkFreq = MemFreq1800MT;
        break;
      case 1866:
      case 1867:        
        OemDimmInfo.DimmWorkFreq = MemFreq1867MT;
        break;
      case 2000:
        OemDimmInfo.DimmWorkFreq = MemFreq2000MT;
        break;
      case 2132:
      case 2133:
        OemDimmInfo.DimmWorkFreq = MemFreq2133MT;
        break;
      case 2200:
        OemDimmInfo.DimmWorkFreq = MemFreq2200MT;  
        break;
      case 2400:
        OemDimmInfo.DimmWorkFreq = MemFreq2400MT;
        break;
      case 2600:
        OemDimmInfo.DimmWorkFreq = MemFreq2600MT;
        break;
      case 2666:
      case 2667:
        OemDimmInfo.DimmWorkFreq = MemFreq2667MT;
        break;
      case 2800:        
        OemDimmInfo.DimmWorkFreq = MemFreq2800MT;
        break;
      case 2933:                            
      case 2934:
        OemDimmInfo.DimmWorkFreq = MemFreq2933MT;
        break;
      case 3000:
        OemDimmInfo.DimmWorkFreq = MemFreq3000MT;
        break;
      case 3200:
        OemDimmInfo.DimmWorkFreq = MemFreq3200MT;
        break;
      default:
        break;
      } 

      switch (SmbiosType17Record->Speed) {
      case 800:
        OemDimmInfo.DimmFreq = MemFreq800MT;
        break;
      case 1000:
        OemDimmInfo.DimmFreq = MemFreq1000MT;
        break;
      case 1066:                            
      case 1067:
        OemDimmInfo.DimmFreq = MemFreq1067MT;
        break;
      case 1200:
        OemDimmInfo.DimmFreq = MemFreq1200MT;
        break;
      case 1333:                            
      case 1334:
        OemDimmInfo.DimmFreq = MemFreq1333MT;
        break;
      case 1400:
        OemDimmInfo.DimmFreq = MemFreq1400MT;
        break;
      case 1600:
        OemDimmInfo.DimmFreq = MemFreq1600MT;
        break;
      case 1800:
        OemDimmInfo.DimmFreq = MemFreq1800MT;
        break;
      case 1866:
      case 1867:
        OemDimmInfo.DimmFreq = MemFreq1867MT;
        break;
      case 2000:
        OemDimmInfo.DimmFreq = MemFreq2000MT;
        break;
      case 2132:
      case 2133:
        OemDimmInfo.DimmFreq = MemFreq2133MT;
        break;
      case 2200:
        OemDimmInfo.DimmFreq = MemFreq2200MT;
        break;
      case 2400:
        OemDimmInfo.DimmFreq = MemFreq2400MT;
        break;
      case 2600:
        OemDimmInfo.DimmFreq = MemFreq2600MT;
        break;
      case 2666:
      case 2667:
        OemDimmInfo.DimmFreq = MemFreq2667MT;
        break;
      case 2800:
        OemDimmInfo.DimmFreq = MemFreq2800MT;
        break;
      case 2933:                            
      case 2934:
        OemDimmInfo.DimmFreq = MemFreq2933MT;
        break;
      case 3000:
        OemDimmInfo.DimmFreq = MemFreq3000MT;
        break;
      case 3200:
        OemDimmInfo.DimmFreq = MemFreq3200MT;
        break;
        
      default:
        break;
      }
    }
    DEBUG((EFI_D_INFO, "Size=%d,DramType=%x,MemoryType=%x\n",OemDimmInfo.DimmSize,OemDimmInfo.DramType,OemDimmInfo.MemoryType));

    StrIndex = SmbiosType17Record->Manufacturer;
    Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType17Record + SmbiosType17Record->Hdr.Length), StrIndex, &TempManuf);
    DEBUG((EFI_D_INFO, "SmbiosType17Record Manufacturer Status: %r\n",Status));
    ASSERT_EFI_ERROR(Status);
    DEBUG((EFI_D_INFO, "TempManuf: %s\n",TempManuf));
    UnicodeStrToAsciiStr (TempManuf, OemDimmInfo.DimmManufacture);
    DEBUG((EFI_D_INFO, "OemDbgDimmInfo.DimmManufacture: %a\n",OemDimmInfo.DimmManufacture));

    StrIndex = SmbiosType17Record->PartNumber;
    Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType17Record + SmbiosType17Record->Hdr.Length), StrIndex, &TempPartNum);
    DEBUG((EFI_D_INFO, "SmbiosType17Record PartNumber Status: %r\n",Status));
    ASSERT_EFI_ERROR(Status); 
    DEBUG((EFI_D_INFO, "TempPartNum: %s\n",TempPartNum));
    UnicodeStrToAsciiStr (TempPartNum, OemDimmInfo.DimmPartNum);
    DEBUG((EFI_D_INFO, "OemDbgDimmInfo.DimmPartNum: %a\n",OemDimmInfo.DimmPartNum));

    StrIndex = SmbiosType17Record->SerialNumber;
    Status = SmbiosGetOptionalStringByIndex ((CHAR8*)((UINT8*)SmbiosType17Record + SmbiosType17Record->Hdr.Length), StrIndex, &TempSN);
    DEBUG((EFI_D_INFO, "SmbiosType17Record SerialNumber Status: %r\n",Status));
    ASSERT_EFI_ERROR(Status); 
    DEBUG((EFI_D_INFO, "TempSN: %s\n",TempSN));
    UnicodeStrToAsciiStr (TempSN, OemDimmInfo.DimmSN);
    DEBUG((EFI_D_INFO, "OemDbgDimmInfo.DimmSN: %a\n",OemDimmInfo.DimmSN));
    //<Malj-20181105 Add Mem Info Send to BMC +> 
    //<Kangmm001-20190516 Regression issue caused by Mantis0028709 +>
    OemDimmInfo.EccCapbility = ((SmbiosType17Record->TotalWidth - SmbiosType17Record->DataWidth) & BIT3 )? 2 : 1;
    //<Kangmm001-20190516 Regression issue caused by Mantis0028709 -> 
    OemDimmInfo.ModuleType = PcdGet8(PcdSystemModuleType); //<Malj-20181106 Fixed Mem Info Send to BMC> 
    OemDimmInfo.RankNum = SmbiosType17Record->Attributes;
    //<Malj-20181106 Fixed Mem Info Send to BMC +> 
    OemDimmInfo.DramWidth = (PcdGet8(PcdSystemDramWidth)+1); //For spd byte12  Bit [2, 1, 0] :
                                                               //000 = 4 bits
                                                               //001 = 8 bits
                                                               //010 = 16 bits
                                                               //011 = 32 bits
                                                               //But Oem define:
                                                               //  UINT8 DramWidth;
                                                               //  1: x4
                                                               //  2: x8
                                                               //  3: x16
                                                               //  4: x32
    //<Malj-20181106 Fixed Mem Info Send to BMC -> 
    OemDimmInfo.DdrVoltage = (UINT8)(SmbiosType17Record->ConfiguredVoltage / 10); //<hujf1001-20201108 Send DDR4 Voltage to BMC, 1.2V will send 120 +>
    DEBUG((EFI_D_INFO, " DdrVoltage:%d ",OemDimmInfo.DdrVoltage ));
    DEBUG((EFI_D_INFO, " EccCapbility:%d ", OemDimmInfo.EccCapbility));
    DEBUG((EFI_D_INFO, " ModuleType:%d ",OemDimmInfo.ModuleType ));
    DEBUG((EFI_D_INFO, " RankNum:%d ", OemDimmInfo.RankNum));
    DEBUG((EFI_D_INFO, " DramWidth:%d ", OemDimmInfo.DramWidth));
    DEBUG((EFI_D_INFO, "\n"));
    //<Malj-20181105 Add Mem Info Send to BMC ->
    //Initialize OEM IPMI package header
    OemDimmInfo.SetServerRespositoryHeader.ParameterSelector = SreverRespositorySubTypeMemory;
    OemDimmInfo.SetServerRespositoryHeader.SetCommandSource = SourceBIOS;
    OemDimmInfo.DataValid = BIOSPackageValid;
        
    Status |= OemIpmiCmdSend(
      CMD_OEM_SET_SERVER_REPOSITORY_INFO,
      (VOID*) &OemDimmInfo,
      sizeof(PACKED_DIMM_INFO),
	  NULL,
	  &ResponseDataSize
      );
  }

  MemFreePointer( (VOID **)&NewString );
  MemFreePointer( (VOID **)&TempManuf );
  MemFreePointer( (VOID **)&TempPartNum );
  MemFreePointer( (VOID **)&TempSN );
  //<Wangyia001-20170509 Send Mem Info to BMC ->

  DEBUG((EFI_D_INFO, " *** Set DIMM Info Over! ***\n"));
  return Status;
}

//**********************************************************************
//<AMI_PHDR_START>
// Procedure:   OemIPMIEntryPoint
//
// Description: Entry point for OemIPMI
//
// Input: IN EFI_HANDLE ImageHandle - Handle for the image of this driver
//        IN EFI_SYSTEM_TABLE *SystemTable - Pointer to the EFI System Table
//
// Output:  EFI_SUCCESS - Protocol Callback is successfully registered
//
//<AMI_PHDR_END>
//**********************************************************************

EFI_STATUS
OemMemInfoEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  
  DEBUG((EFI_D_INFO, __FUNCTION__" Start\n")); 
  Status = OemIpmiSetDimmInfo();
  if (Status == EFI_NOT_FOUND) {
    Status = EFI_SUCCESS;
  }
  DEBUG((EFI_D_INFO, "Send Memory info status: %r\n",Status)); 
  return  Status;
}                                                        

//***************************************************************
