//
// $Header: $
//
// $Revision: $
//
// $Date: $
//
//*****************************************************************************

//<OEM_FHDR_START>
//----------------------------------------------------------------------------
//
// Name:        OemRtcResetHook.c
//
// Description: Detect RTC Power state, if RTCRST#, save BIOS password into HOB.
//
// Notes:       
//
//----------------------------------------------------------------------------
//<OEM_FHDR_END>


// Module specific Includes
#include <EFI.h>
#include <Token.h>
#include <AmiPeiLib.h>
#include <AmiDxeLib.h>
#include <AmiCspLib.h>
#include <Protocol\BootScriptSave.h>
#include <SB.h>
#include <Library/PciLib.h>
#include <Ipmi/PeiIpmiCmosClear/PeiIpmiCmosClear.h>
#include <OemRtcResetHook.h>

//<Wangyia001-20170504 Add solution to check whether CMOS is cleared +>
#define FCH_PMIOA_REG5A          0x5A    // CmosControl
#define FCH_IOMAP_REGCD6         0x0CD6  // PM_Index
#define FCH_IOMAP_REGCD7         0x0CD7  // PM_Data
  //<xuyj1-20181208 Add solution to check whether CMOS is cleared according to HYGON suggestion +>
#define FCH_PMIOA_REGFF         0xFF  // PM_Data
  //<xuyj1-20181208 Add solution to check whether CMOS is cleared according to HYGON suggestion ->
//<Wangyia001-20170504 Add solution to check whether CMOS is cleared ->
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// END of SPI Flash related porting hooks
//----------------------------------------------------------------------------

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure: IsPchRtcReset
//
// Description:
//  This function determines if the system should boot with the default configuration.
//  This function is called by NVRAM PEI code.
//  If boot with default configuration is detected, default values for NVRAM variables are used.
//
// Input:
//  **PeiServices - pointer to a pointer to the PEI Services Table.
//  *ReadVariablePpi - pointer to EFI_PEI_READ_ONLY_VARIABLE2_PPI PPI. The pointer can be used to
//                     read and enumerate existing NVRAM variables.
//
// Output:
//  TRUE - Boot with default configuration is detected
//  FALSE - Boot with default configuration is not detected
//
// Notes:
//  This routine is called very early, prior to SBPEI and NBPEI
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
//Lizhq 20140821 Reserve Password after clear CMOS.[m]{
BOOLEAN OemRtcResetHook(
    IN EFI_PEI_SERVICES **PeiServices,
    IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *ReadVariablePpi )
{
  EFI_STATUS  Status;
  UINTN       Size;
  EFI_GUID    AmiTseSetupGuid = AMITSESETUP_GUID;
  AMI_TSE_HOB *AmiTsehob = NULL;
  AMITSESETUP AmiTseData={0};
  //CmosClearSource
  //BIT0: RTCRST#
  //BIT1: IPMI CMOS clear
  UINT8       CmosClearSource = 0;
  UINT8       Data8 = 0;
  UINT8       CmosCtrl;  //<Wangyia001-20170504 Add solution to check whether CMOS is cleared>
    
  PEI_TRACE((EFI_D_INFO, PeiServices, "OemRtcResetHook Start\n"));
  //<xuyj1-20181208 Add solution to check whether CMOS is cleared according to HYGON suggestion +>
  /*
  //<Wangyia001-20170504 Add solution to check whether CMOS is cleared +>
  //Check whether CMOS is cleared.
  IoWrite8 (FCH_IOMAP_REGCD6, FCH_PMIOA_REG5A);
  CmosCtrl = IoRead8 (FCH_IOMAP_REGCD7);
  PEI_TRACE((EFI_D_INFO, PeiServices, "CmosCtrl_1:0x%x\n", CmosCtrl));
  
  if (CmosCtrl & 0x40) {
    CmosClearSource |= BIT0;
  }
  
  CmosCtrl = CmosCtrl & (~BIT4);
  CmosCtrl = CmosCtrl | BIT5;
  PEI_TRACE((EFI_D_INFO, PeiServices, "CmosCtrl_2:0x%x\n", CmosCtrl));
  IoWrite8 (FCH_IOMAP_REGCD6, FCH_PMIOA_REG5A);
  IoWrite8 (FCH_IOMAP_REGCD7,CmosCtrl);
  //<Wangyia001-20170504 Add solution to check whether CMOS is cleared ->
  */
  
  
  IoWrite8(FCH_IOMAP_REG72,FCH_PMIOA_REGFF);
  CmosCtrl = IoRead8(FCH_IOMAP_REG73);
  if(CmosCtrl != 0xAA){
     CmosClearSource |= BIT0;
  }
  DEBUG((EFI_D_ERROR, "CmosCtrl_1:0x%x\n", CmosCtrl));
  IoWrite8 (FCH_IOMAP_REG72,FCH_PMIOA_REGFF);
  IoWrite8 (FCH_IOMAP_REG73,0xAA);
  //<xuyj1-20181208 Add solution to check whether CMOS is cleared according to HYGON suggestion ->
  
  //Get IPMI CMOS clear flag
  Size = sizeof (UINT8);
  Status = ReadVariablePpi->GetVariable (
                                  ReadVariablePpi,
                                  L"IpmiCmosClear",
                                  &gEfiIpmiCmosClearVariableGuid,
                                  NULL,
                                  &Size,
                                  &Data8 );
  PEI_TRACE((EFI_D_INFO, PeiServices, "ReadVariablePpi->GetVariable Status %r  \n", Status));
  if ( !EFI_ERROR (Status)) {
    CmosClearSource |= BIT1;
  }

  PEI_TRACE((EFI_D_INFO, PeiServices, "CmosClearSource:0x%x\n", CmosClearSource));

  //Do something if CMOS was cleared.
  if(CmosClearSource != 0){
  // Preserve the Admin and User password Information in a HOB
  AmiTsehob = AllocateZeroPool(sizeof(AMI_TSE_HOB));
  PEI_TRACE((EFI_D_INFO, PeiServices, "AmiTsehob:0x%X\n", AmiTsehob));
  if (AmiTsehob != NULL) {
    Status = (**PeiServices).CreateHob(PeiServices,
                EFI_HOB_TYPE_GUID_EXTENSION,
                sizeof(AMI_TSE_HOB),
                &AmiTsehob
              );
    
    PEI_TRACE((EFI_D_INFO, PeiServices, "Create HOB:%r\n", Status));

    if (!EFI_ERROR(Status)) {
      AmiTsehob->RtcResetFlag = TRUE;
      AmiTsehob->EfiHobGuidType.Name = AmiTseSetupGuid;
      MemCpy(&AmiTsehob->AmiTseData, &AmiTseData, sizeof(AMITSESETUP));

      Size = sizeof(AMITSESETUP);
      Status = ReadVariablePpi->GetVariable(
                 ReadVariablePpi, 
                 L"AMITSESetup",
                 &AmiTseSetupGuid, 
                 NULL,
                 &Size, 
                 &AmiTseData
                );
      PEI_TRACE((EFI_D_INFO, PeiServices, "TSE Setup:%r\n", Status));
      // Check if password has been applied or not.
      if (!EFI_ERROR(Status)/* && ( 0 != MemCmp(&AmiTseData, &AmiTsehob->AmiTseData, sizeof(AMITSESETUP)))*/) {
        MemCpy(&AmiTsehob->AmiTseData, &AmiTseData, sizeof(AMITSESETUP));
      }
    }
  }
  PEI_TRACE((EFI_D_INFO, PeiServices, "OemRtcResetHook End\n"));
  return TRUE;                                                                                                                                                      //RTC_PWR_STS  bit will remain set until the software clears it by writing a 0 back to this bit position. The PCH code will take care of  this.
  }
   
  //No CmosClearSource detected
  PEI_TRACE((EFI_D_INFO, PeiServices, "OemRtcResetHook No CmosClearSource detected\n"));
  return FALSE;
}
//Lizhq 20140821 Reserve Password after clear CMOS.}
