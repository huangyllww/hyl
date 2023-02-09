//**********************************************************************
// $Header: OEM IPMI: BIOS&SPS Version
//
// $Revision: 0.01
//
// $Date:  <Lizhq-20161221> 
//         Base On OEM IPMI SPEC 20161217, send BIOS&SPS Version to BMC.
//**********************************************************************
#include <setup.h>
#include <OemIpmiNetFnDefinitions.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>


#include <AmiLib.h>
#include <Library/PrintLib.h>

#define OEM_IPMI_STR_VERSION_LENGTH 32

typedef enum {
  BIOSVersion  = 1
};

EFI_STATUS
OemIpmiSetBIOSVersion(VOID)
{
  EFI_STATUS                Status = EFI_UNSUPPORTED;   
  PACKED_BIOS_VERSION       OemBiosVersion; 
  UINT8   					ResponseDataSize=0;
  
  ZeroMem((VOID*)&OemBiosVersion, sizeof(PACKED_BIOS_VERSION));
  
  OemBiosVersion.BiosMeSelect = BIOSVersion;
  
//  AsciiStrCpyS(OemBiosVersion.StrVersion, OEM_IPMI_STR_VERSION_LENGTH, CONVERT_TO_STRING(OEM_BIOS_VERSION));
  AsciiStrCpyS(OemBiosVersion.StrVersion, OEM_IPMI_STR_VERSION_LENGTH, CONVERT_TO_STRING(BIOS_TAG));//<YinQiang001-20181205 Send BIOS Version Full Name to BMC>
  DEBUG((EFI_D_ERROR, "%a Version is: %a\n", "BIOS" ,  OemBiosVersion.StrVersion));

  Status = OemIpmiCmdSend(
      CMD_OEM_SET_BIOS_VERSION_INFO,
      (VOID*) &OemBiosVersion,
      sizeof(PACKED_BIOS_VERSION),
      NULL,
      &ResponseDataSize
      );
  
  return Status;
}

EFI_STATUS
OemBiosInfoEntryPoint (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  EFI_STATUS  	    Status = EFI_SUCCESS;
  
  DEBUG((EFI_D_ERROR, __FUNCTION__" Start\n")); 
  
  Status = OemIpmiSetBIOSVersion();
  DEBUG((EFI_D_ERROR, "Set BIOS Version Status: %r\n", Status));
  
  return  Status;
}                                                        

//***************************************************************
