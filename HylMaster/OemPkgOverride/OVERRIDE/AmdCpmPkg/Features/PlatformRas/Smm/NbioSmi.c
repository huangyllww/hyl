/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include "RasSmmPciLib.h"
#include "AmdRas.h"
#include "AmdPlatformRasSmm.h"
#include "Protocol/FchSmmPeriodicalDispatch2.h"
#include <AmdCpmSmm.h>
#include "Token.h"
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
//SGEZT#23035:PCIe Leaky Buckey +>
extern PCIE_PORT_ERROR_PROFILE     *PcieErrorProfile;
BOOLEAN SkipErrorReport = TRUE;

#define PCI_VENDORID_REG            (0x00)
#define PCI_DEVICEID_REG            (0x02)
#define PCI_COMMAND_REG             (0x04)
#define PCI_CLASS_CODE_0_REG        (0x09)
#define PCI_CLASS_CODE_1_REG        (0x0A)
#define PCI_CLASS_CODE_2_REG        (0x0B)
#define PCI_PRIMARY_BUS_REG         (0x18)
#define PCI_SECONDARY_BUS_REG       (0x19)
#define PCIE_SLOT_NUMBER_SHIFT      (19) //SGPZ#23493-DmesgSlotInfoError

#define PCI_BRIDGE_CONTROL_REG      (0x3E)
  #define PCI_BRIDGECTRL_SERR_EN      (BIT1)

#define PCI_SEC_STATUS_REG          (0x1E)

//SGEZT#23035:PCIe Leaky Buckey ->

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT32                                     mNbioSmiCount = 0;
UINT32                                     mNbioSmiCoolOff = 0;
UINT64                                     mNbioTscLast = 0;
SMI_MODE                                   mNbioSmiMode = INTERRUPT_MODE;
EFI_HANDLE                                 mNbioPollingModeHandle = NULL;
FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL      *mAmdNbioPeriodicalDispatch = NULL;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
NbioLogError (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );

EFI_STATUS
NbioClrStatus (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );

EFI_STATUS
NbioResetCounter (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );

BOOLEAN
NbioSearchErr (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  );


EFI_STATUS
PcieErrorScan (
  UINT32    NbioGlobalStatusHi,
  UINT8     DieBusNum
  );

//SGEZT#23035:PCIe Leaky Buckey +>
EFI_STATUS
ParsePCIeError (
  PCIE_PORT_PROFILE *PciePortProfile,
  IN UINT32         DieBusNum
  );
//SGEZT#23035:PCIe Leaky Buckey ->

EFI_STATUS
SataErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  );

BOOLEAN
FindNbioError (
  VOID
  );

VOID 
ProcessNbioRasSmiSource (
  VOID
  );

EFI_STATUS
RasSmmRegisterNbioPeriodicSmi (
  VOID
  );

SCAN_STATUS
STATIC
RasDevSmmStsClr (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  );

VOID
FillPcieInfo (
  PCI_ADDR      PciPortAddr,
  PCIE_ERROR_SECTION *PcieErrorSection  
  );
  
VOID
STATIC
PcieDevErrScanOnDevice (
  PCI_ADDR       Device,
  BOOLEAN        RootStatusSet
  );

VOID
STATIC
PcieDevErrScanOnFunction (
  PCI_ADDR       Function,
  BOOLEAN        RootStatusSet
  );

SCAN_STATUS
STATIC
PcieDevErrScanCallback (
  PCI_ADDR             Device,
  RAS_PCI_SCAN_DATA    *ScanData
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          T A B L E    D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
//A PCI-E port device/function loopup table.
PCIE_PORT_LIST gPciePortList[] = {
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {1, 6},
    {1, 7},
    {2, 1},
    {3, 1},
    {3, 2},
    {3, 3},
    {3, 4},
    {3, 5},
    {3, 6},
    {3, 7},
    {4, 1}
};

//SGEZT#23035:PCIe Leaky Buckey +>
typedef struct
{
  UINT32  GenErrorDataErrorSeverity;
  UINT32  PcieErrStatusErrorSeverity;
}APEI_ERROR_SEVERITY;
//SGEZT#23035:PCIe Leaky Buckey ->

/*********************************************************************************
 * Name: AmdNbioRasSmiCallback
 *
 * Description
 *   NBIO RAS Error handle SMI call back function entry
 *   Perform NBIO Error handling
 *
 *
 * Arguments:
 *   DispatchHandle  : The handle of this callback, obtained when registering
 *   DispatchContext : Pointer to the FCH_SMM_APURAS_REGISTER_CONTEXT
 *
 * Returns:
 *   None
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdNbioRasSmiCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  )
{

  RAS_NBIO_ERROR_INFO   RasNbioErrorInfo;
  UINT32                NbioGlobalStatusLo;
  UINT32                NbioGlobalStatusHi;
  BOOLEAN               RasThresholdPeriodicSmiEn;

  RasThresholdPeriodicSmiEn = mPlatformApeiData->PlatRasPolicy.RasThresholdPeriodicSmiEn;
// moved down to reduce debug messages
//  if (mNbioSmiMode == INTERRUPT_MODE) {
//    DEBUG ((EFI_D_ERROR, "NBIO SMI Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
//  } else {
//    DEBUG ((EFI_D_ERROR, "POLLING_MODE NBIO RAS Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
//  }
  
  //Check NBIO Global Error Status
  RasSmnRead ((UINT32)MiscRegisterContext->Bus, RAS_GLOBAL_STATUS_LO, &NbioGlobalStatusLo);
  RasSmnRead ((UINT32)MiscRegisterContext->Bus, RAS_GLOBAL_STATUS_HI, &NbioGlobalStatusHi);

  if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) == 0)) {
    return EFI_SUCCESS;
  }

  //Error found!!
  RasNbioErrorInfo.NbioGlobalStatusLo = NbioGlobalStatusLo;
  RasNbioErrorInfo.NbioGlobalStatusHi = NbioGlobalStatusHi;
  RasNbioErrorInfo.Die = MiscRegisterContext->Die;
  RasNbioErrorInfo.DieBusNumber = MiscRegisterContext->Bus;

  if (mNbioSmiMode == INTERRUPT_MODE) {
    DEBUG ((EFI_D_ERROR, "NBIO SMI Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
  } else {
    DEBUG ((EFI_D_ERROR, "POLLING_MODE NBIO RAS Error handler: Die %x, Bus %x\n", MiscRegisterContext->Die, MiscRegisterContext->Bus));
  }
  DEBUG ((EFI_D_RAS|EFI_D_ERROR, "=================================================================================\n"));
  DEBUG ((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]Global Status Lo : 0x%08x  Hi : 0x%08x\n", __FUNCTION__,NbioGlobalStatusLo,NbioGlobalStatusHi));

  if (RasThresholdPeriodicSmiEn) {
    ProcessNbioRasSmiSource ();
  }

  //Scan Error Reporting Status by Type (UNCORR, CORR, UCP)
  //Right now only report first finding. make it loop for all errors.
  if ((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) != 0) {
    RasNbioErrorInfo.TypeId = NBIO_GRP_UNCORR;
    if (NbioSearchErr (&RasNbioErrorInfo)) {
      NbioLogError (&RasNbioErrorInfo);
      NbioClrStatus (&RasNbioErrorInfo);
    }

    RasNbioErrorInfo.TypeId = NBIO_GRP_CORR;
    if (NbioSearchErr (&RasNbioErrorInfo)) {
      NbioLogError (&RasNbioErrorInfo);
      NbioClrStatus (&RasNbioErrorInfo);
      NbioResetCounter (&RasNbioErrorInfo);
    }

    RasNbioErrorInfo.TypeId = NBIO_GRP_UCP;
    if (NbioSearchErr (&RasNbioErrorInfo)) {
      NbioLogError (&RasNbioErrorInfo);
      NbioClrStatus (&RasNbioErrorInfo);
      NbioResetCounter (&RasNbioErrorInfo);
    }
  }

  if ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) != 0) {
    //PCI-E error detection
    //Scan active PCI-E port
    PcieErrorScan (NbioGlobalStatusHi, MiscRegisterContext->Bus);

    //SATA error detection
    SataErrorScan (NbioGlobalStatusHi, MiscRegisterContext->Bus);

    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}

BOOLEAN
CheckSataEnabled (
  UINT8 DieBusNum
  )
{
  PCI_ADDR     Nbif1Addr;
  PCI_ADDR     SataAddr;
  UINT8        SecondaryBus;

  DEBUG ((EFI_D_ERROR, "NBIO Bus: %x\n", DieBusNum));
  Nbif1Addr.AddressValue = MAKE_SBDFO (0, DieBusNum, 8, 1, 0);
  if (RasPciIsDevicePresent (Nbif1Addr.AddressValue)) {
    SecondaryBus = PciRead8 (Nbif1Addr.AddressValue | 0x19);
    DEBUG ((EFI_D_ERROR, "NBIF1 secondary Bus: %x\n", SecondaryBus));
    if (SecondaryBus == 0) {
      //The secondary PCI bus has not assigned, it shouldn't be 0
      return FALSE;
    }
    SataAddr.AddressValue = MAKE_SBDFO (0, SecondaryBus, 0, 2, 0);
    if (RasPciIsDevicePresent (SataAddr.AddressValue)) {
      DEBUG ((EFI_D_ERROR, "SATA Controller Found !!!\n"));
      return TRUE;
    }
  }
  return FALSE;
}

EFI_STATUS
SataErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  UINT32       SataPortRasSts;
  UINT32       PortNum;

  //Check NBIF1 status
  if (0 != (NbioGlobalStatusHi & BIT17)) {
    //check if SataRasSupport and SATA enabled.
    if ( mPlatformApeiData->PlatRasPolicy.FchSataRasSupport && CheckSataEnabled (DieBusNum)) {
      DEBUG ((EFI_D_ERROR, "Scan SATA Error: Bus %x\n", DieBusNum));
      for ( PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++ ) {
        RasSmnRead (DieBusNum, FCH_SMN_SATA_CONTROL_SLOR + PortNum * 0x20 + 0x88, &SataPortRasSts);
        if ((SataPortRasSts & SATA_PAR_ERR_STS) == 0) {
          continue;
        }
        // Log Sata error

        //
        //Clear error status
        //
        RasSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG130 + PortNum * 0x80, 0xFFFFFFFF, 0xFFFFFFFF);
        RasSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG110 + PortNum * 0x80, 0xFFFFFFFF, 0x00);
        RasSmnRW (DieBusNum, FCH_SMN_SATA_CONTROL_SLOR + PortNum * 0x20 + 0x88, 0xFFFFFFFF, 0x00);
      }
    }
  }
  return Status;
}

EFI_STATUS
PcieErrorScan (
  UINT32 NbioGlobalStatusHi,
  UINT8 DieBusNum
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINT32                PcieGlobalStatus;
  UINT16                PciePortIndex;
  PCIE_PORT_PROFILE     *PciePortProfileInstance;
  //SGEZT#23035:PCIe Leaky Buckey +>
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
  UINT64                TscCurrent;
  UINT64                TimeSinceTscBoot;
  UINT64                LeakyInterval;
  UINT64                LeakyLoops;
  UINT64                LeakyDropOut;
  
  LeakyInterval = mPlatformApeiData->PlatRasPolicy.PcieLeakyBucketInterval * 60 * 60 * 1000;
  LeakyDropOut = mPlatformApeiData->PlatRasPolicy.PcieLeakyBucketDropOutCnt;
#endif
  //SGEZT#23035:PCIe Leaky Buckey ->

  PcieGlobalStatus = NbioGlobalStatusHi & 0xFFFF;

  PciePortProfileInstance = mPlatformApeiData->AmdPciePortMap->PciPortNumber;

  //Search active PCI-E port for error only
  for (PciePortIndex = 0; PciePortIndex < mPlatformApeiData->AmdPciePortMap->PortCount; PciePortIndex++, PciePortProfileInstance++) {
    if (PciePortProfileInstance->NbioBusNum != DieBusNum) {
      //find next
      continue;
    }
    if ((PcieGlobalStatus & (1 << PciePortProfileInstance->PciPhysicalPortNumber)) != 0) {
      DEBUG ((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]PCI-E Error detected at 0x%02x:0x%02x.0x%02x, Physical Port: %0d\n", __FUNCTION__,\
                                                                        DieBusNum,gPciePortList[PciePortProfileInstance->PciPortNumber].Device,\
                                                                        gPciePortList[PciePortProfileInstance->PciPortNumber].Function,PciePortProfileInstance->PciPhysicalPortNumber));
      
      //SGEZT#23035:PCIe Leaky Buckey +>
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
      SkipErrorReport = TRUE; //default is not report error to OS
      if (PcieErrorProfile[PciePortIndex].RpPciAddr == PciePortProfileInstance->RpPciAddr) //if not equal, always not report error; should be equal
      {
        PcieErrorProfile[PciePortIndex].TotalErrors += 1;
        TscCurrent = AsmReadMsr64(TSC);
        TimeSinceTscBoot = ConvertToMilliseconds((TscCurrent - PcieErrorProfile[PciePortIndex].TscBoot));
        if (LeakyInterval == 0)
          LeakyLoops = 0;
        else
          LeakyLoops = TimeSinceTscBoot / LeakyInterval;
        if ((LeakyLoops * LeakyDropOut >= (UINT64)PcieErrorProfile[PciePortIndex].TotalErrors) || //drop is more then total errors, reset error counter
              (TimeSinceTscBoot >= (24 *60 * 60 * 1000))) //over 24 hours, reset error counter
        {
          PcieErrorProfile[PciePortIndex].TotalErrors = 0;
          PcieErrorProfile[PciePortIndex].TscBoot = TscCurrent;
          SkipErrorReport = TRUE;
          DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]reset error counter caused by errors need drop is more than actual errors or time over 24 hours\n",__FUNCTION__));
        }
        else
        {
          if ((PcieErrorProfile[PciePortIndex].TotalErrors - (UINT16)(LeakyLoops * LeakyDropOut)) < mPlatformApeiData->PlatRasPolicy.McaPcieErrThreshCount) //not over McaPcieErrThreshCount
          {
            SkipErrorReport = TRUE;
            DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]not over 24 hours, error will be count\n",__FUNCTION__));
          }
          else //error over McaPcieErrThreshCount
          {
            PcieErrorProfile[PciePortIndex].TotalErrors = 0;
            PcieErrorProfile[PciePortIndex].TscBoot = TscCurrent;
            SkipErrorReport = FALSE;
            DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]error over (%d + drop count), error will be report\n", __FUNCTION__, mPlatformApeiData->PlatRasPolicy.McaPcieErrThreshCount));
          }
        }
      }
      DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]LeakyLoops %x LeakyDropOut %x TotalErrors %x SkipErrorReport %x\n", __FUNCTION__,LeakyLoops, LeakyDropOut, PcieErrorProfile[PciePortIndex].TotalErrors, SkipErrorReport));
#endif      
      //SGEZT#23035:PCIe Leaky Buckey ->

      //PCI-E error detected
      Status = ParsePCIeError (PciePortProfileInstance,DieBusNum);
      if (Status == EFI_OUT_OF_RESOURCES) {
        DEBUG ((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]PCI-E Error log Out of Resource!!!\n",__FUNCTION__));
      }
    }
  }

  return Status;
}

EFI_STATUS
RasSmmStsClr(
  PCI_ADDR          PciPortAddr
)
{
  UINT16        AerCapPtr;
  UINT16        SecPcieExtCapPtr;
  UINT8         PcieCapPtr;
  UINT32        PcieUncorrStatus;
  UINT32        PcieCorrStatus;
  UINT32        PcieRootStatus;
  UINT16        PcieDevStatus;
  UINT32        PcieLnkStatus;
  UINT32        RasRetryCounter;

  
  //Clear Status register
  DEBUG ((EFI_D_ERROR, "[RAS]Error Handle ClrSts @ Bus: 0x%x, Dev: 0x%x, Func: 0x%x, DLActive=%d\n", PciPortAddr.Address.Bus, PciPortAddr.Address.Device, PciPortAddr.Address.Function, (PciRead16(PciPortAddr.AddressValue + 0x6A) & BIT13 ? 1 : 0) ));

  
  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  SecPcieExtCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, SEC_PCIE_EXT_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);
  
  RasRetryCounter = mPlatformApeiData->PlatRasPolicy.RasRetryCnt;
  if (AerCapPtr != 0) {
    PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR); 
    PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
    PcieRootStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
    while ((PcieUncorrStatus !=0)||(PcieCorrStatus !=0) || (PcieRootStatus !=0)) {
      if (RasRetryCounter == 0) {
        break;
      }  else {
        RasRetryCounter--; 
      }
      DEBUG ((EFI_D_ERROR, "[RAS] PCIE UnCorr Error Status : 0x%08x\n", PcieUncorrStatus));
      PciWrite32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR, PcieUncorrStatus);
      PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
      
      DEBUG ((EFI_D_ERROR, "[RAS] PCIE Corr Error Status : 0x%08x\n", PcieCorrStatus));
      PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR, PcieCorrStatus);
      PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
         
      if (RasGetPcieDeviceType (PciPortAddr) == PcieDeviceRootComplex) {
        DEBUG ((EFI_D_ERROR, "[RAS] PCIE Root Error Status : 0x%08x\n", PcieRootStatus));
        PciWrite32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR, PcieRootStatus);
        PcieRootStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_ROOT_STATUS_PTR);
      }
    }
  }
  RasRetryCounter = mPlatformApeiData->PlatRasPolicy.RasRetryCnt;
  if (PcieCapPtr != 0) {
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    while ((PcieDevStatus & 0xF) != 0) {
      if (RasRetryCounter == 0) {
        break;
      } else {
        RasRetryCounter--; 
      }
    DEBUG ((EFI_D_ERROR, "[RAS] PCIE Device Status : 0x%08x\n", PcieDevStatus));
    PciWrite16(PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR, PcieDevStatus);
    PcieDevStatus = PciRead16 (PciPortAddr.AddressValue + PcieCapPtr + PCIE_DEVICE_STATUS_PTR);
    }
  }
  RasRetryCounter = mPlatformApeiData->PlatRasPolicy.RasRetryCnt;
  if (SecPcieExtCapPtr !=0 ) {
    PcieLnkStatus = PciRead32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
    while (PcieLnkStatus != 0) {
      if (RasRetryCounter == 0) {
        break;
      } else {
        RasRetryCounter--; 
      }
      DEBUG((EFI_D_ERROR, "[RAS] PCIE Lane Error Status : 0x%08x\n", PcieLnkStatus));
      PciWrite32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR, PcieLnkStatus);
      PcieLnkStatus = PciRead32 (PciPortAddr.AddressValue + SecPcieExtCapPtr + PCIE_LANE_ERR_STATUS_PTR);
    }
  }
  return EFI_SUCCESS;

}

SCAN_STATUS
STATIC
RasDevSmmStsClr (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  )
{
    SCAN_STATUS             ScanStatus;
    PCIE_DEVICE_TYPE        DeviceType;
    ScanStatus = SCAN_SUCCESS;
    DEBUG ((EFI_D_ERROR, "[RAS] RasDevSmmStsClr for Device = %d:%d:%d\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
    ScanStatus = SCAN_SUCCESS;
    DeviceType = RasGetPcieDeviceType (Device);
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device type = 0x%x\n", DeviceType));
    switch (DeviceType) {
    case  PcieDeviceRootComplex:
    case  PcieDeviceDownstreamPort:
    case  PcieDeviceUpstreamPort:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device root port found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case  PcieDevicePcieToPcix:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device PCIE to PCIx found\n"));
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case  PcieDeviceEndPoint:
    case  PcieDeviceLegacyEndPoint:
      DEBUG ((EFI_D_ERROR, "[RAS] PCI-E endpoint found\n"));
      RasSmmStsClr (Device);
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    default:
      break;
    }
    return ScanStatus;
}

//SGEZT#23035:PCIe Leaky Buckey +>
EFI_STATUS
CheckAPEIErrorTable()
{
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE  *PcieErrStatusBlk=mPlatformApeiData->AmdPcieAerErrBlk;
  
  if(SkipErrorReport == TRUE)
    return EFI_SUCCESS;

  if ((sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + PcieErrStatusBlk->DataLength + sizeof (GENERIC_PCIE_AER_ERR_ENTRY)) > MAX_ERROR_BLOCK_SIZE)
    return EFI_OUT_OF_RESOURCES;
  
  return EFI_SUCCESS;
}

VOID
RasReinitErrBlkSts (
  IN OUT       EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE *ErrStatusBlk
  )
{
  if ((ErrStatusBlk->BlockStatus.CorrectableErrorValid | ErrStatusBlk->BlockStatus.UncorrectableErrorValid) == 0)
  {
    DEBUG ((DEBUG_ERROR, "[RAS] Error Block Reset!!\n"));
    //OS already recorded the previous error and invalid the valid bits, reset buffer.
    DEBUG ((DEBUG_ERROR, "[RAS] Error Block Buffer Size to clear: 0x%x\n", ErrStatusBlk->DataLength + sizeof (EFI_ACPI_6_0_GENERIC_ERROR_STATUS_STRUCTURE)));
    ZeroMem (ErrStatusBlk, ErrStatusBlk->DataLength + sizeof (EFI_ACPI_6_0_GENERIC_ERROR_STATUS_STRUCTURE));

    //Reinit header
    ErrStatusBlk->ErrorSeverity = ERROR_NONE;
    ErrStatusBlk->RawDataOffset = sizeof (EFI_ACPI_6_0_GENERIC_ERROR_STATUS_STRUCTURE);
  }
  DEBUG ((DEBUG_ERROR, "[RAS] Error Block Data Entry Count: 0x%x, Length: 0x%0x\n", ErrStatusBlk->BlockStatus.ErrorDataEntryCount, ErrStatusBlk->DataLength));
}


VOID
UpdateGenErrStsBlkSeverity(
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE *ErrStatusBlk,
  APEI_ERROR_SEVERITY        ErrorSeverity
)
{
    switch (ErrorSeverity.GenErrorDataErrorSeverity)
    {
    case ERROR_RECOVERABLE:
        if (ErrStatusBlk->ErrorSeverity != ERROR_SEVERITY_FATAL) {
          ErrStatusBlk->ErrorSeverity = ERROR_RECOVERABLE;
        }
      break;
    case ERROR_SEVERITY_FATAL:
      // If error type Uncorrectable AND multiple uncorrectable errors
      ErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_FATAL;
      break;
    case ERROR_SEVERITY_CORRECTED:
      if (ErrStatusBlk->ErrorSeverity > ERROR_SEVERITY_CORRECTED) {
        ErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_CORRECTED;
      }
      break;
    }
}

EFI_STATUS
LogPCIeError
(
  IN  APEI_ERROR_SEVERITY ErrorServerity,
  IN  PCI_ADDR      PciPortAddr
)
{
  EFI_STATUS          Status = EFI_SUCCESS;
  UINT16              AerCapPtr;
  UINT8               PcieCapPtr;
  UINT32              Index;
  UINT32              ErrDataEntryCount;
  PCIE_DEVICE_TYPE    PortType;
  PCI_ADDR            PciAddr;
  UINT8               DevType=RasGetPcieDeviceType (PciPortAddr);
  UINT32              MaxIndex = 0;

  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *PcieErrStatusBlk,*TempPcieErrStatusBlk;
  GENERIC_PCIE_AER_ERR_ENTRY                  *GenPcieAerErrEntry,*TempGenPcieAerErrEntry;
  PCIE_ERROR_SECTION                          *TempPcieErrorSection;
  EFI_GUID                                    PcieErrSectionType = PCIE_SECT_GUID;
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
  UINT8                                       fSeparateAERLog=mPlatformApeiData->PlatRasPolicy.SeparateAERLog;
#endif
  /**
  Note:Allocate Temp Memory Buffer
  Patch Reserved For Only Filter OS Message Solution
  **/
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (PCIE_ERROR_SECTION),
                    &TempPcieErrorSection
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE),
                    &TempPcieErrStatusBlk
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (GENERIC_PCIE_AER_ERR_ENTRY),
                    &TempGenPcieAerErrEntry
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  /**
  Note:Update PcieErrStatusBlk & GenPcieAerErrEntry Data
  **/
  //Get PcieErrStatusBlk Pointer
  switch (ErrorServerity.GenErrorDataErrorSeverity) 
  {
    case ERROR_SEVERITY_FATAL:
      //Find uncorrectable error block
      switch(DevType)
      {
        case PcieDeviceRootComplex:
          PcieErrStatusBlk = mPlatformApeiData->AmdPcieAerUnErrBlk;
          *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
          *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;
          break;
        case PcieDeviceEndPoint:
        case PcieDeviceLegacyEndPoint:
          PcieErrStatusBlk = mPlatformApeiData->AmdPcieDevAerUnErrBlk;
          if(PciPortAddr.Address.Function == 0)
          {
            *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
            *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;
          }
          break;
        default:
          PcieErrStatusBlk = mPlatformApeiData->AmdPcieBridgeAerUnErrBlk;
          *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
          *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;
          break;
      }
      CopyMem (TempPcieErrStatusBlk, PcieErrStatusBlk, sizeof(EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));
      //Set BlockStatus
      if (TempPcieErrStatusBlk->BlockStatus.UncorrectableErrorValid) 
      {
        TempPcieErrStatusBlk->BlockStatus.MultipleUncorrectableErrors = 1;
      } else 
      {
        TempPcieErrStatusBlk->BlockStatus.UncorrectableErrorValid = 1;
      }
      break;
    case ERROR_RECOVERABLE:
    case ERROR_SEVERITY_CORRECTED:
      //Find correctable error block
      switch(DevType)
      {
        case PcieDeviceRootComplex:
          PcieErrStatusBlk = mPlatformApeiData->AmdPcieAerErrBlk;
          *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
          *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;
          break;
        case PcieDeviceEndPoint:
        case PcieDeviceLegacyEndPoint:
          PcieErrStatusBlk = mPlatformApeiData->AmdPcieDevAerErrBlk;
          if(PciPortAddr.Address.Function == 0)
          {
            *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
            *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;
          }
          break;
        default:
          PcieErrStatusBlk = mPlatformApeiData->AmdPcieBridgeAerErrBlk;
          *((UINT32*)(&PcieErrStatusBlk->BlockStatus)) = 0;
          *((UINT32*)(&PcieErrStatusBlk->DataLength)) = 0;
          break;
      }
            
      CopyMem (TempPcieErrStatusBlk, PcieErrStatusBlk, sizeof(EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));
      //Set BlockStatus
      if (TempPcieErrStatusBlk->BlockStatus.CorrectableErrorValid) {
        TempPcieErrStatusBlk->BlockStatus.MultipleCorrectableErrors = 1;
      } else {
        TempPcieErrStatusBlk->BlockStatus.CorrectableErrorValid = 1;
      }
      break;
    default:
      return EFI_UNSUPPORTED;
  }

  //Get GenPcieAerErrEntry
  GenPcieAerErrEntry = (GENERIC_PCIE_AER_ERR_ENTRY *) ((UINTN ) PcieErrStatusBlk + sizeof (EFI_ACPI_6_0_GENERIC_ERROR_STATUS_STRUCTURE) + PcieErrStatusBlk->DataLength);
  
  ErrDataEntryCount = TempPcieErrStatusBlk->BlockStatus.ErrorDataEntryCount;

  /**
  Note:Init Temp Memory Buffer(TempPcieErrStatusBlk && TempGenPcieAerErrEntry)
  **/
  RasReinitErrBlkSts(TempPcieErrStatusBlk);

  CopyMem (TempGenPcieAerErrEntry, GenPcieAerErrEntry, sizeof(GENERIC_PCIE_AER_ERR_ENTRY));


  /**
  Note:Parse Data
  **/
  PciAddr.AddressValue = PciPortAddr.AddressValue;

  AerCapPtr = RasFindPcieExtendedCapability (PciAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciAddr.AddressValue, PCIE_CAP_ID);



  DEBUG ((EFI_D_ERROR, "  Bus 0x%x Dev: 0x%x Func: 0x%x PCIE Cap Ptr : 0x%x\n", PciAddr.Address.Bus, PciAddr.Address.Device, PciAddr.Address.Function, PcieCapPtr));
  
  PortType = RasGetPcieDeviceType (PciAddr);
  TempPcieErrorSection->Validation.Value = 0xEF;
  TempPcieErrorSection->PortType = (UINT32)PortType;
  TempPcieErrorSection->Revision = 0x02;
  TempPcieErrorSection->CommandStatus = PciRead32 (PciAddr.AddressValue + PCI_COMMAND_REG);

  TempPcieErrorSection->DeviceId.VendorId = PciRead16 (PciAddr.AddressValue + PCI_VENDORID_REG);
  TempPcieErrorSection->DeviceId.DeviceId = PciRead16 (PciAddr.AddressValue + PCI_DEVICEID_REG);
  TempPcieErrorSection->DeviceId.ClassCode[0] = PciRead8 (PciAddr.AddressValue + PCI_CLASS_CODE_0_REG);
  TempPcieErrorSection->DeviceId.ClassCode[1] = PciRead8 (PciAddr.AddressValue + PCI_CLASS_CODE_1_REG);
  TempPcieErrorSection->DeviceId.ClassCode[2] = PciRead8 (PciAddr.AddressValue + PCI_CLASS_CODE_2_REG);
  TempPcieErrorSection->DeviceId.Function = (UINT8)PciAddr.Address.Function;
  TempPcieErrorSection->DeviceId.Device = (UINT8)PciAddr.Address.Device;
  TempPcieErrorSection->DeviceId.Segment = (UINT8)PciAddr.Address.Segment;


  // bus number information to uniquely identify the root port or bridge. Default values for both the bus numbers is zero.
  TempPcieErrorSection->DeviceId.PrimaryBus = ((PortType <= PcieDeviceLegacyEndPoint))? (UINT8)PciAddr.Address.Bus : PciRead8 (PciAddr.AddressValue + PCI_PRIMARY_BUS_REG);
  TempPcieErrorSection->DeviceId.SecondaryBus = ((PortType <= PcieDeviceLegacyEndPoint))? 0x00 : PciRead8 (PciAddr.AddressValue + PCI_SECONDARY_BUS_REG);

  if (PcieCapPtr != 0) 
  {
    //SGPZ#23493-DmesgSlotInfoError +>
    //TempPcieErrorSection->DeviceId.Slot = (UINT16) ((PciRead32 (PciAddr.AddressValue + PcieCapPtr + PCIE_SLOT_CAP_REGISTER) >> PCIE_SLOT_NUMBER_SHIFT) << 3);
    TempPcieErrorSection->DeviceId.Slot = (UINT16) (PciRead32 (PciAddr.AddressValue + PcieCapPtr + PCIE_SLOT_CAP_REGISTER) >> PCIE_SLOT_NUMBER_SHIFT);
    //SGPZ#23493-DmesgSlotInfoError ->
  }
  TempPcieErrorSection->BridgeCtrlStatus = (UINT32) (PciRead16 (PciAddr.AddressValue + PCI_BRIDGE_CONTROL_REG)) << 16 | (UINT32) (PciRead16 (PciAddr.AddressValue + PCI_SEC_STATUS_REG));

  if (PcieCapPtr != 0) 
  {
    // Check PCIE CAP version
    MaxIndex = ((PciRead16 (PciAddr.AddressValue + PcieCapPtr + 2) & 0xF) < 2)? 9 : 15;
    for (Index = 0; Index < MaxIndex; Index++) 
    {
      TempPcieErrorSection->CapabilityStructure.CapabilityData[Index] = PciRead32 (PciAddr.AddressValue + (PcieCapPtr + (4 * Index)));
    }
  }
  if (AerCapPtr != 0) 
  {
    for (Index = 0; Index < 24; Index++) 
    {
      TempPcieErrorSection->AerInfo.AerInfoData[Index] = PciRead32 (PciAddr.AddressValue + (AerCapPtr + (4 * Index)));
    }
  }

  CopyGuid ((EFI_GUID*)&TempGenPcieAerErrEntry->GenErrorDataEntry.SectionType, &PcieErrSectionType);
  TempGenPcieAerErrEntry->GenErrorDataEntry.Revision = GENERIC_ERROR_REVISION;
  TempGenPcieAerErrEntry->GenErrorDataEntry.Flags = 0x01;
  TempGenPcieAerErrEntry->GenErrorDataEntry.ErrorDataLength = sizeof (PCIE_ERROR_SECTION);

  TempGenPcieAerErrEntry->GenErrorDataEntry.ErrorSeverity = ErrorServerity.GenErrorDataErrorSeverity;

  TempPcieErrStatusBlk->ErrorSeverity = ErrorServerity.PcieErrStatusErrorSeverity;

  TempPcieErrStatusBlk->BlockStatus.ErrorDataEntryCount++;
  TempPcieErrStatusBlk->DataLength += sizeof (GENERIC_PCIE_AER_ERR_ENTRY);

  UpdateGenErrStsBlkSeverity(TempPcieErrStatusBlk, ErrorServerity);
  
  
  CopyMem (&TempGenPcieAerErrEntry->PcieAerErrorSection, TempPcieErrorSection, sizeof(PCIE_ERROR_SECTION));

  mAmiHygonElogProtocol->SendElogEventPcie((UINT8*)TempGenPcieAerErrEntry); // AMI PORTING ELog 
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
  //If Only Support BMC AER Log,SKIP OS Log
  if(!fSeparateAERLog)
  {
#endif
    //Patch Reserved For Only Filter OS Message Solution
    CopyMem(GenPcieAerErrEntry,TempGenPcieAerErrEntry,sizeof(GENERIC_PCIE_AER_ERR_ENTRY));
    CopyMem(PcieErrStatusBlk,TempPcieErrStatusBlk,sizeof(EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE));
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)      
  }
#endif
  Status = gSmst->SmmFreePool(TempPcieErrorSection);
  Status = gSmst->SmmFreePool(TempGenPcieAerErrEntry);
  Status = gSmst->SmmFreePool(TempPcieErrStatusBlk);

  return EFI_SUCCESS;
}
//SGEZT#23035:PCIe Leaky Buckey ->

EFI_STATUS
ParsePCIeError (
  PCIE_PORT_PROFILE *PciePortProfile,
  IN UINT32         DieBusNum
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  PCI_ADDR      PciPortAddr;
  UINT16        AerCapPtr;
  UINT16        SecPcieExtCapPtr;
  UINT8         PcieCapPtr;
  UINT32        PcieUncorrStatus=0;
  UINT32        PcieUncorrMask;
  UINT32        PcieUncorrSeverity;
  UINT32        PcieCorrStatus=0;
  UINT32        PcieCorrMask=0;
  UINT32		    SlotCapabilities;
  UINT32		    SlotControl;
  UINT32		    SlotStatus;
  BOOLEAN       IsRootPortError = FALSE;
  BOOLEAN       RootErrStatusSet = FALSE;
  UINT32        RootErrStatus;
  UINT8         PwrRsrCfg;
  RAS_PCI_SCAN_DATA  ScanData;
  APEI_ERROR_SEVERITY ErrorServerity;




  PciPortAddr.AddressValue = 0;
  PciPortAddr.Address.Bus = PciePortProfile->NbioBusNum;
  PciPortAddr.Address.Device = gPciePortList[PciePortProfile->PciPortNumber].Device;
  PciPortAddr.Address.Function = gPciePortList[PciePortProfile->PciPortNumber].Function;
  
  if(EFI_ERROR (CheckAPEIErrorTable))
  {
    RasSmmStsClr(PciPortAddr);
    return EFI_OUT_OF_RESOURCES;
  }

  AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
  SecPcieExtCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, SEC_PCIE_EXT_CAP_ID);
  PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);

  DEBUG ((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]Die %d Physical Port %d PCIE Cap Ptr : 0x%x\n",__FUNCTION__,PciePortProfile->NbioDieNum, PciePortProfile->PciPhysicalPortNumber, PcieCapPtr));

  ErrorServerity.GenErrorDataErrorSeverity=ERROR_NONE;

  if (RasPciErrCheck(PciPortAddr, &RootErrStatusSet, &RootErrStatus)) {
    IsRootPortError = TRUE;
  }

  DEBUG ((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]IsRootPortError = %d\n",__FUNCTION__,IsRootPortError));

  if (!IsRootPortError && RootErrStatusSet) 
  {
    RasSmmStsClr (PciPortAddr);
    
    //Scan and log device error
    ScanData.Buffer = (void *)&RootErrStatusSet;
    ScanData.RasScanCallback = PcieDevErrScanCallback;
    RasPciScanSecondaryBus (PciPortAddr, &ScanData);
  }else
  { 
    /**
    Note:
    **/
    if (AerCapPtr == 0)
    {
      RasSmmStsClr(PciPortAddr);
      return EFI_NOT_FOUND;
    }

    //Un-Correctable error
    PcieUncorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
    PcieUncorrMask = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_MASK_PTR);
    PcieUncorrSeverity = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_UNCORR_SEVERITY_PTR);

    DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]UC Status:%x Mask:%x Severity:%x\r\n",__FUNCTION__,PcieUncorrStatus,PcieUncorrMask,PcieUncorrSeverity));

    if (PcieUncorrStatus & ~PcieUncorrMask) 
    {
      if (PcieUncorrStatus & PcieUncorrSeverity) 
      {
        ErrorServerity.GenErrorDataErrorSeverity = ERROR_SEVERITY_FATAL;
        ErrorServerity.PcieErrStatusErrorSeverity = ERROR_SEVERITY_FATAL;
      }else 
      {
        ErrorServerity.GenErrorDataErrorSeverity = ERROR_RECOVERABLE;
        ErrorServerity.PcieErrStatusErrorSeverity = ERROR_RECOVERABLE;
      }
      
      //Check Hotplug Device
      //Completion Timeout Status
      if(PcieUncorrStatus & BIT14)
      {		

        SlotCapabilities = PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x14);
        if (SlotCapabilities & BIT6)
        {			        
          //Hotplug capable
          SlotControl = PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x18);
          if (SlotControl & BIT10)
          {				
            // //Power off
            ErrorServerity.GenErrorDataErrorSeverity = ERROR_NONE;
            ErrorServerity.PcieErrStatusErrorSeverity = ERROR_NONE;
          } else {
              SlotStatus = PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x1A);
              if (!(SlotStatus & BIT6))
              {				
                // //Presence Detect = Slot Empty
                ErrorServerity.GenErrorDataErrorSeverity = ERROR_NONE;
                ErrorServerity.PcieErrStatusErrorSeverity = ERROR_NONE;
              }
            }							  
          }			  
        }
    }else 
    {
      //Correctable error
    	PcieCorrStatus = PciRead32 (PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_STATUS_PTR);
     	PcieCorrMask = PciRead32(PciPortAddr.AddressValue + AerCapPtr + PCIE_CORR_MASK_PTR);

      DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]Correctd Status:%x Mask:%x\r\n",__FUNCTION__,PcieCorrStatus,PcieCorrMask));

    	if (0 != (PcieCorrStatus & ~PcieCorrMask)) 
      {
        ErrorServerity.GenErrorDataErrorSeverity = ERROR_SEVERITY_CORRECTED;
        ErrorServerity.PcieErrStatusErrorSeverity = ERROR_SEVERITY_CORRECTED;
    	}
    }
 //SGEZT#23035:PCIe Leaky Buckey +>
 #if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
    if((SkipErrorReport == FALSE) || (ErrorServerity.GenErrorDataErrorSeverity == ERROR_SEVERITY_FATAL)) 
    {
#endif
 //SGEZT#23035:PCIe Leaky Buckey ->

      Status = LogPCIeError(ErrorServerity,PciPortAddr);
      DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]LogPCIeError:%r\r\n",__FUNCTION__,Status));
 //SGEZT#23035:PCIe Leaky Buckey +>
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)        
    }
#endif
 //SGEZT#23035:PCIe Leaky Buckey ->
    // Clear Root Port error status
    RasSmmStsClr(PciPortAddr);

    if (ErrorServerity.GenErrorDataErrorSeverity != ERROR_SEVERITY_FATAL) 
      return EFI_SUCCESS;

    // Trigger action - Warm reset
    IoWrite8 (0xCD6, 0x10);
    PwrRsrCfg = IoRead8 (0xCD7);
    PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
    IoWrite8 (0xCD7, PwrRsrCfg);
    IoWrite8(0xCF9, 0x2);
    IoWrite8(0xCF9, 0x6);
    CpuDeadLoop();
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RasSmmRegisterNbioSmi (
  VOID
  )
{
  EFI_STATUS                        Status;
  FCH_SMM_APURAS_DISPATCH_PROTOCOL  *AmdApuRasDispatch;
  FCH_SMM_APURAS_REGISTER_CONTEXT   ApuRasRegisterContext;
  EFI_HANDLE                        ApuRasHandle = NULL;
  UINT8                             Die;
  UINT8                             DieBusNum;
  UINT16                            VendorID;
  UINTN                             PciAddress;

  //
  //  Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmApuRasDispatchProtocolGuid,
                  NULL,
                  &AmdApuRasDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  //Register Error handler per Die.
  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if ((VendorID != AMD_VENDOR_ID) && (VendorID != HYGON_VID)) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);

    DEBUG ((EFI_D_ERROR, "[RAS] Install NBIO Error SMI callback handler : Die %x, Bus %x\n", Die, DieBusNum));

    //NBIO Error handle registry.
    ApuRasRegisterContext.Bus = DieBusNum;
    ApuRasRegisterContext.Die = Die;
    ApuRasRegisterContext.Order = 0x80;

    Status = AmdApuRasDispatch->Register (
                              AmdApuRasDispatch,
                              AmdNbioRasSmiCallback,
                              &ApuRasRegisterContext,
                              &ApuRasHandle
                              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

BOOLEAN
NbioSearchErr (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  UINT32 Index;
  UINT32 NbioParityErrorSts;
  UINT32 BaseAddr;
  UINT32 MaxGroupId;

  BaseAddr = 0;
  MaxGroupId = 0;
  switch (RasNbioErrorInfo->TypeId) {
  case NBIO_GRP_UNCORR:
    BaseAddr = PARITY_ERROR_STATUS_UNCORR_GRP;
    MaxGroupId = MAX_UNCORR_GRP_NUM;
    break;
  case NBIO_GRP_CORR:
    BaseAddr = PARITY_ERROR_STATUS_CORR_GRP;
    MaxGroupId = MAX_CORR_GRP_NUM;
    break;
  case NBIO_GRP_UCP:
    BaseAddr = PARITY_ERROR_STATUS_UCP_GRP;
    MaxGroupId = MAX_UCP_GRP_NUM;
  }

  for (Index = 0; Index < MaxGroupId; Index++) {
    RasSmnRead ((UINT32)RasNbioErrorInfo->DieBusNumber,
                BaseAddr + (Index * 4),
                &NbioParityErrorSts);
    if (NbioParityErrorSts != 0) {
      RasNbioErrorInfo->GroupId = (UINT8)Index;
      RasNbioErrorInfo->NbioParityErrorStsAddr = BaseAddr + (Index * 4);
      RasNbioErrorInfo->NbioParityErrorSts = NbioParityErrorSts;
      return TRUE;
    }
  }
  return FALSE;
}


VOID
UpdateNbioErrDataEntry (
  IN       GENERIC_NBIO_ERR_ENTRY   *ErrDataEntry,
  IN       RAS_NBIO_ERROR_INFO      *RasNbioErrorInfo
  )
{
  EFI_GUID     NbioErrSectionType = NBIO_ERROR_SECT_GUID;

  CopyGuid ((EFI_GUID*)&ErrDataEntry->GenErrorDataEntry.SectionType, &NbioErrSectionType);
  if ((RasNbioErrorInfo->NbioGlobalStatusLo & BIT0) == 1) {
    ErrDataEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_CORRECTED;
  } else {
    ErrDataEntry->GenErrorDataEntry.ErrorSeverity = ERROR_SEVERITY_FATAL;
  }
  ErrDataEntry->GenErrorDataEntry.Revision = GENERIC_ERROR_REVISION;
  ErrDataEntry->GenErrorDataEntry.Flags = 0x01;
  ErrDataEntry->GenErrorDataEntry.ErrorDataLength = sizeof (AMD_NBIO_ERROR_RECORD);

  ErrDataEntry->NbioErrorSection.ValidationBits.Value = NBIO_VALID_BIT_MAP;
  ErrDataEntry->NbioErrorSection.BusId = RasNbioErrorInfo->DieBusNumber;
  ErrDataEntry->NbioErrorSection.ErrorSource.Field.Nbio = 1;
  ErrDataEntry->NbioErrorSection.ErrorType.Value = (UINT8) (RasNbioErrorInfo->NbioGlobalStatusLo & 0x0000000F);
  ErrDataEntry->NbioErrorSection.GroupId = RasNbioErrorInfo->GroupId;
  ErrDataEntry->NbioErrorSection.GroupType = RasNbioErrorInfo->TypeId;
  ErrDataEntry->NbioErrorSection.ParityErrSts = RasNbioErrorInfo->NbioParityErrorSts;
}

EFI_STATUS
NbioLogError (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  EFI_STATUS                                    Status;
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE      *NbioErrStatusBlk;
  GENERIC_NBIO_ERR_ENTRY                        *ErrDataEntry;
  UINT32                                        ErrDataEntryCount;

  Status = EFI_SUCCESS;

  NbioErrStatusBlk    = mPlatformApeiData->AmdNbioErrBlk;
  ErrDataEntryCount = NbioErrStatusBlk->BlockStatus.ErrorDataEntryCount;

  // Provide only one record per error report
  *((UINT32*)(&NbioErrStatusBlk->BlockStatus)) = 0;
  *((UINT32*)(&NbioErrStatusBlk->DataLength)) = 0;


  ErrDataEntry      = (GENERIC_NBIO_ERR_ENTRY *) ((UINTN ) NbioErrStatusBlk + sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + NbioErrStatusBlk->DataLength);

  if ((sizeof (EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE) + NbioErrStatusBlk->DataLength + sizeof (GENERIC_NBIO_ERR_ENTRY)) > MAX_ERROR_BLOCK_SIZE) {
    return EFI_OUT_OF_RESOURCES;
  }

  UpdateNbioErrDataEntry (ErrDataEntry, RasNbioErrorInfo);

  if ((RasNbioErrorInfo->NbioGlobalStatusLo & BIT0) == 1) {
    NbioErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_CORRECTED;
    if (NbioErrStatusBlk->BlockStatus.CorrectableErrorValid) {
      NbioErrStatusBlk->BlockStatus.MultipleCorrectableErrors = 1;
    } else {
      NbioErrStatusBlk->BlockStatus.CorrectableErrorValid = 1;
    }
  } else {
    NbioErrStatusBlk->ErrorSeverity = ERROR_SEVERITY_FATAL;
    if (NbioErrStatusBlk->BlockStatus.UncorrectableErrorValid) {
      NbioErrStatusBlk->BlockStatus.MultipleUncorrectableErrors = 1;
    } else {
      NbioErrStatusBlk->BlockStatus.UncorrectableErrorValid = 1;
    }
  }

  NbioErrStatusBlk->BlockStatus.ErrorDataEntryCount++;
  NbioErrStatusBlk->DataLength += sizeof (GENERIC_NBIO_ERR_ENTRY);

  mAmiHygonElogProtocol->SendElogEventNbio((UINT8*)RasNbioErrorInfo); // AMI PORTING ELog
  return EFI_SUCCESS;
}

EFI_STATUS
NbioClrStatus (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  RasSmnWrite ((UINT32)RasNbioErrorInfo->DieBusNumber,
               RasNbioErrorInfo->NbioParityErrorStsAddr,
               &RasNbioErrorInfo->NbioParityErrorSts);

  return EFI_SUCCESS;
}

EFI_STATUS
NbioResetCounter (
  RAS_NBIO_ERROR_INFO *RasNbioErrorInfo
  )
{
  UINT32        ParityCounterAddr;
  UINT32        MaxGroupId;
  UINT32        ParityCounterReset;

  ParityCounterAddr = 0;
  MaxGroupId = 0;
  switch (RasNbioErrorInfo->TypeId) {
  case NBIO_GRP_CORR:
    ParityCounterAddr = PARITY_COUNTER_CORR_GRP;
    MaxGroupId = MAX_CORR_GRP_NUM;
    break;
  case NBIO_GRP_UCP:
    ParityCounterAddr = PARITY_COUNTER_UCP_GRP;
    MaxGroupId = MAX_UCP_GRP_NUM;
  }

  if (RasNbioErrorInfo->GroupId > MaxGroupId) {
    return EFI_INVALID_PARAMETER;
  }

  ParityCounterAddr += (RasNbioErrorInfo->GroupId * 4);
  ParityCounterReset = BIT31;

  RasSmnWrite ((UINT32)RasNbioErrorInfo->DieBusNumber,
               ParityCounterAddr,
               &ParityCounterReset);

  return EFI_SUCCESS;
}

BOOLEAN
FindNbioError (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINTN                             PciAddress;
  UINT8                             Die;
  UINT8                             DieBusNum;
  UINT16                            VendorID;
  UINT32                            NbioGlobalStatusLo;
  UINT32                            NbioGlobalStatusHi;
  FCH_SMM_APURAS_REGISTER_CONTEXT   ApuRasRegisterContext;
  BOOLEAN                           ErrorFound;

  ErrorFound = FALSE;
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - FindNbioError Entry\n"));
  for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Process Die: 0x%02x, ", Die));

    PciAddress = ((Die + 24) << 15) + 0x000;
    VendorID = PciRead16 (PciAddress);
    if (VendorID != AMD_VENDOR_ID && VendorID != HYGON_VID) {
      continue;
    }

    PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
    DieBusNum = PciRead8 (PciAddress);
    DEBUG ((DEBUG_ERROR, "DieBusNum: 0x%02x.\n", DieBusNum));
    
    //Check NBIO Global Error Status
    RasSmnRead ((UINT32)DieBusNum, RAS_GLOBAL_STATUS_LO, &NbioGlobalStatusLo);
    RasSmnRead ((UINT32)DieBusNum, RAS_GLOBAL_STATUS_HI, &NbioGlobalStatusHi);

    if (((NbioGlobalStatusLo & RAS_GLOBAL_STATUS_LO_MASK) == 0) && ((NbioGlobalStatusHi & RAS_GLOBAL_STATUS_HI_MASK) == 0)) {
      continue;
    }
    
    ApuRasRegisterContext.Die = Die;
    ApuRasRegisterContext.Bus = DieBusNum;
    Status = AmdNbioRasSmiCallback (NULL, &ApuRasRegisterContext);
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - FindNbioError: Error Found in Die: 0x%02x\n", Die));
    ErrorFound = TRUE;
  }
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - FindNbioError Exit\n"));

  return ErrorFound;
}



// Goal... limit the system on average to 5 interrupts per 1000ms or less
// If we exceed the limit, enter periodic polling mode
//
// If we are in periodic polling mode, and no errors occur during the 1000ms
// time scale, return to interrupt mode.
//
// mNbioTscLast = <TSC>
// mNbioSmiMode = Interrupt (default)
// mNbioSmiCount = 0 (default)
// 
// SMI_threshold = PcdCpmSmiThreshold (default: 5 interrupts)
// SMI_scale = PcdCpmSmiScale (default: 1000 ms)
// 
// SmiRate (SmiPeriod) = SMI_scale / SMI_threshold
// mNbioSmiCoolOff = 0;
// 
// TimeSinceLastSmi (Uninitilized)
VOID 
ProcessNbioRasSmiSource (
  VOID
  )
{
  UINT64                  TimeSinceLastSmi;
  UINT32                  SmiRate;
  UINT64                  TscCurrent;
  UINT32                  Data32;
  BOOLEAN                 McaErrThreshEn;
  #if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
  UINT16                  McaMemErrThreshCount; //<Chendl001-20180626 Memory Leaky Bucket Design>
  #else
  UINT16                  McaErrThreshCount;
  #endif
  UINT32                  SmiThreshold;
  UINT32                  SmiScale;
  EFI_STATUS              Status;

  if (mNbioSmiMode != INTERRUPT_MODE)
    return;

  mNbioSmiCount++;
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Entry: Before Leaky bucket process => mNbioSmiCount = 0x%08x\n", mNbioSmiCount));

  McaErrThreshEn = mPlatformApeiData->PlatRasPolicy.McaErrThreshEn;
  #if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
  McaMemErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaMemErrThreshCount; //<Chendl001-20180626 Memory Leaky Bucket Design>

  #else
  McaErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
  #endif
  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;
  SmiScale = mPlatformApeiData->PlatRasPolicy.RasSmiScale;
  //Ex: SmiRate = (SmiScale: 1000, unit: ms)/(SmiThreshold: 5 interrupts) = 200ms 
  SmiRate = SmiScale/SmiThreshold;   

  // Leak events out of the leaky bucket based on the time since the last SMI
  TscCurrent = AsmReadMsr64(TSC);
  TimeSinceLastSmi = ConvertToMilliseconds((TscCurrent - mNbioTscLast));
  while ((TimeSinceLastSmi > SmiRate) && (mNbioSmiCount > 0)) {
    mNbioSmiCount--;
    TimeSinceLastSmi-=SmiRate;
  }
  
  // Save the current TSC to the last TSC
  mNbioTscLast = TscCurrent;
  DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]   INTERRUPT_MODE: After Leaky bucket process => mNbioSmiCount = 0x%08x\n", mNbioSmiCount));
  if (mNbioSmiCount > SmiThreshold) {
    // This event caused the Leaky bucket to overflow, enable polled mode via periodic SMIs
    DEBUG ((DEBUG_ERROR, "  INTERRUPT_MODE: mNbioSmiCount: 0x%04x > SmiThreshold: 0x%04x => Leaky bucket overflow, Start switching to POLLING_MODE\n", 
                         mNbioSmiCount, SmiThreshold));

    // Try to turn on periodic SMI's at rate of SmiRate                  
    Status = RasSmmRegisterNbioPeriodicSmi ();
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Exit: Register AmdNbioPeriodicSmiCallback failed: 0x%08x, cannot switch to POLLING_MODE.\n", Status));
      return;
    }

    mNbioSmiMode = POLLING_MODE;
    mNbioSmiCoolOff = SmiThreshold;
    
    //Disable SMI generation for NBIO SMI
    Data32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC);  
    Data32 &= ~(BIT10 | BIT11);                             //Disable HW assertion SMI
    MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC, Data32);

    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Exit: Successfully switched to POLLING_MODE\n"));
  } else {
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] INTERRUPT_MODE - Exit: Leaky bucket has not overflowed\n"));
  }

  return;
}

VOID 
ProcessNbioPeriodicSMI (
  VOID
  )
{
  UINT32                  Data32;
  UINT32                  SmiThreshold;
  BOOLEAN                 McaErrThreshEn;
  #if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
  UINT16                  McaMemErrThreshCount; //<Chendl001-20180626 Memory Leaky Bucket Design>

  #else
  UINT16                  McaErrThreshCount;
  #endif
  EFI_STATUS              Status = EFI_SUCCESS;
  UINTN                   PciAddress;
  UINT8                   Die;
  UINT8                   DieBusNum;
  UINT16                  VendorID;
  
  McaErrThreshEn = mPlatformApeiData->PlatRasPolicy.McaErrThreshEn;
  #if defined(MEMORY_LEAKY_BUCKET_SUPPORT) && (MEMORY_LEAKY_BUCKET_SUPPORT == 1)
  McaMemErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaMemErrThreshCount; //<Chendl001-20180626 Memory Leaky Bucket Design>

  #else
  McaErrThreshCount = mPlatformApeiData->PlatRasPolicy.McaErrThreshCount;
  #endif
  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;

  if (mNbioSmiMode == POLLING_MODE) {
    DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Entry\n"));
    if (FindNbioError()) {
      // An error was detected, reset the threshold to max
      mNbioSmiCoolOff = SmiThreshold;
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]  POLLING_MODE - Exit: An error was detected, reset the mNbioSmiCoolOff to max: 0x%08x\n", mNbioSmiCoolOff));
    } else {
      mNbioSmiCoolOff--;
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]   POLLING_MODE: No Error, mNbioSmiCoolOff-- = 0x%08x\n", mNbioSmiCoolOff));
      if (mNbioSmiCoolOff == 0) {
        // If we go one full leaky bucket time scale with no errors, return to interrupt mode
        DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI]   POLLING_MODE: mNbioSmiCoolOff == 0x%08x => Go one full leaky bucket time scale with no errors => Start returning to INTERRUPT_MODE.\n", 
                                mNbioSmiCoolOff));
    
        // Try to turn off periodic SMI's for PFEH polling
        if (mNbioPollingModeHandle == NULL) {
          DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: cannot locate mNbioPollingModeHandle, fail to return to INTERRUPT_MODE\n"));
          return;
        }
        Status = mAmdNbioPeriodicalDispatch->UnRegister (
                                           mAmdNbioPeriodicalDispatch, 
                                           mNbioPollingModeHandle);
        if (EFI_ERROR(Status)) {
          DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: cannot UnrRgister AmdNbioPeriodicSmiCallback, fail to return to INTERRUPT_MODE\n"));
          return;
        }

        mNbioPollingModeHandle = NULL;
        mNbioSmiMode = INTERRUPT_MODE;
        mNbioSmiCount  = 0;
        
        //Enable SMI generation for NBIO SMI
        for (Die = 0; Die < MAX_DIE_SUPPORT; Die++) {
          PciAddress = ((Die + 24) << 15) + 0x000;
          VendorID = PciRead16 (PciAddress);
          if (VendorID != AMD_VENDOR_ID && VendorID != HYGON_VID) {
            continue;
          }
        
          PciAddress = ((Die + 24) << 15) + (DF_CFGADDRESSCNTL_FUNC << 12) + DF_CFGADDRESSCNTL_OFFSET;
          DieBusNum = PciRead8 (PciAddress);

          RasSmnRW (DieBusNum, RAS_GLOBAL_STATUS_LO, 0xfffffeff, BIT8);   //Clear HWSMI_STS
          RasSmnRW (DieBusNum, IOHC_INTERRUPT_EOI, 0xfffffffe, BIT0);   //Clear SMI_EOI
        }

        MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0x84, BIT21);  //Clear BIT21: ApuRasSmi
        Data32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC);
        Data32 &= ~(BIT10 | BIT11);
        Data32 |= BIT10;
        MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0xAC, Data32); //Enable HW assertion SMI
        MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + 0x84, BIT21);  //Clear BIT21: ApuRasSmi

        DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: Successfully return to INTERRUPT_MODE\n"));
      } else {
      DEBUG ((DEBUG_ERROR, "[RAS - NBIO SMI] POLLING_MODE - Exit: No error duration time is less than SMI time scale\n"));
      }
    }
  }
  return;
}

EFI_STATUS
EFIAPI
AmdNbioPeriodicSmiCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{

  ProcessNbioPeriodicSMI ();

  return EFI_SUCCESS;
}

EFI_STATUS
RasSmmRegisterNbioPeriodicSmi (
  VOID
  )
{
  EFI_STATUS                               Status;
  FCH_SMM_PERIODICAL_REGISTER_CONTEXT      PeriodicalRegisterContext; 
  UINT32                                   SmiRate;
  UINT32                                   SmiThreshold;
  UINT32                                   SmiScale;

  SmiThreshold = mPlatformApeiData->PlatRasPolicy.RasSmiThreshold;
  SmiScale = mPlatformApeiData->PlatRasPolicy.RasSmiScale;

  //
  // Periodic Timer SMI Registration
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmPeriodicalDispatch2ProtocolGuid,
                    NULL,
                    &mAmdNbioPeriodicalDispatch
                    );
  
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //SmiRate = (SmiScale: 1000, unit: ms)/(SmiThreshold: 5 interrupts) = 200ms
  SmiRate = SmiScale/SmiThreshold;

  //SmiTrig0, In FchSmmPeriodicalDispatcher.c:
  //  if (SmiTickInterval == LONG_TIMER_SMI_INTERVAL) then {FCH_SMI_REG98 |= BIT29}
  //  FCH_SMI_REG98[29] = 1 => SmiTimer to be SmiLongTimer register and long timer runs at 1 ms unit time.
  PeriodicalRegisterContext.SmiTickInterval  = LONG_TIMER_SMI_INTERVAL;
  
  //SmiTimer, FCH_SMI_REG96 = (UINT16) (Period / SmiTickInterval) & 0x7FFF;
  PeriodicalRegisterContext.Period           = SmiRate * LONG_TIMER_SMI_INTERVAL;
  
  //SmiTimer, FCH_SMI_REG96 |= SMI_TIMER_ENABLE
  PeriodicalRegisterContext.StartNow         = 1;

  mNbioPollingModeHandle = NULL;
  Status = mAmdNbioPeriodicalDispatch->Register (
                                     mAmdNbioPeriodicalDispatch,
                                     AmdNbioPeriodicSmiCallback,
                                     &PeriodicalRegisterContext,
                                     &mNbioPollingModeHandle
                                     );

  return Status;
}

VOID
FillPcieInfo (
  PCI_ADDR      PciPortAddr,
  PCIE_ERROR_SECTION *PcieErrorSection  
  )
{
    UINT32              Index;
    UINT8               PcieCapPtr;
    UINT16              AerCapPtr;
    PCIE_DEVICE_TYPE    PortType;
    
    AerCapPtr = RasFindPcieExtendedCapability (PciPortAddr.AddressValue, PCIE_EXT_AER_CAP_ID);
    PcieCapPtr = RasFindPciCapability (PciPortAddr.AddressValue, PCIE_CAP_ID);
    
    PortType = RasGetPcieDeviceType (PciPortAddr);
    PcieErrorSection->Validation.Value = 0xEF;
    PcieErrorSection->PortType = (UINT32)PortType;
    PcieErrorSection->Revision = 0x02;
    PcieErrorSection->CommandStatus = PciRead32 (PciPortAddr.AddressValue + 0x04);

    PcieErrorSection->DeviceId.VendorId = PciRead16 (PciPortAddr.AddressValue + 0x00);
    PcieErrorSection->DeviceId.DeviceId = PciRead16 (PciPortAddr.AddressValue + 0x02);
    PcieErrorSection->DeviceId.ClassCode[0] = PciRead8 (PciPortAddr.AddressValue + 0x09);
    PcieErrorSection->DeviceId.ClassCode[1] = PciRead8 (PciPortAddr.AddressValue + 0x0A);
    PcieErrorSection->DeviceId.ClassCode[2] = PciRead8 (PciPortAddr.AddressValue + 0x0B);
    PcieErrorSection->DeviceId.Function = (UINT8)PciPortAddr.Address.Function;
    PcieErrorSection->DeviceId.Device = (UINT8)PciPortAddr.Address.Device;
    PcieErrorSection->DeviceId.Segment = (UINT8)PciPortAddr.Address.Segment;
    PcieErrorSection->DeviceId.PrimaryBus = (UINT8)PciPortAddr.Address.Bus;
    PcieErrorSection->DeviceId.SecondaryBus =  \
            ((PortType <= PcieDeviceLegacyEndPoint))? 0x00:PciRead8 (PciPortAddr.AddressValue + 0x19);
    
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Function : 0x%x\n", PcieErrorSection->DeviceId.Function));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Device : 0x%x\n", PcieErrorSection->DeviceId.Device));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.Segment : 0x%x\n", PcieErrorSection->DeviceId.Segment));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.PrimaryBus : 0x%x\n", PcieErrorSection->DeviceId.PrimaryBus));
    DEBUG ((EFI_D_ERROR, "[RAS] PcieErrorSection->DeviceId.SecondaryBus : 0x%x\n", PcieErrorSection->DeviceId.SecondaryBus));
    
    if (PcieCapPtr !=0) {
      PcieErrorSection->DeviceId.Slot = (UINT8)(PciRead32(PciPortAddr.AddressValue + PcieCapPtr + 0x14) >> 19);
      PcieErrorSection->BridgeCtrlStatus = (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x3E)) << 16 | (UINT32) (PciRead16 (PciPortAddr.AddressValue + 0x1E));

      for (Index = 0; Index < 15; Index++) {
        PcieErrorSection->CapabilityStructure.CapabilityData[Index] = \
                                            PciRead32 (PciPortAddr.AddressValue + (0x58 + (4 * Index)));
      }
    }
    
    if (AerCapPtr !=0) {
      for (Index = 0; Index < 24; Index++) {
        PcieErrorSection->AerInfo.AerInfoData[Index] = PciRead32 (PciPortAddr.AddressValue + (AerCapPtr + (4 * Index)));
      }
    }
}

VOID
STATIC
PcieDevErrScanOnFunction (
  PCI_ADDR       Function,
  BOOLEAN        RootStatusSet
  )
{
  EFI_STATUS  Status;

  UINT16      AerCapPtr;
  UINT32      PcieUncorrStatus;
  UINT32      PcieUncorrMask;
  UINT32      PcieUncorrSverity;
  BOOLEAN     PcieCorrErrorValid;
  UINT32      RootErrStatus;
  UINT8       PwrRsrCfg;
  
  APEI_ERROR_SEVERITY ErrorServerity;

  AerCapPtr = RasFindPcieExtendedCapability (Function.AddressValue, PCIE_EXT_AER_CAP_ID);

  if (!RasPciErrCheck(Function, &RootStatusSet, &RootErrStatus)) 
    return;

  if(AerCapPtr == 0)
    return;
 
  PcieUncorrStatus = PciRead32 (Function.AddressValue + AerCapPtr + PCIE_UNCORR_STATUS_PTR);
  PcieUncorrMask = PciRead32(Function.AddressValue + AerCapPtr + PCIE_UNCORR_MASK_PTR);
  PcieUncorrSverity = PciRead32 (Function.AddressValue + AerCapPtr + PCIE_UNCORR_SEVERITY_PTR);
  if (0 != (PcieUncorrStatus & ~PcieUncorrMask)) 
  {
    if (PcieUncorrStatus & PcieUncorrSverity) 
    {
      ErrorServerity.GenErrorDataErrorSeverity = ERROR_SEVERITY_FATAL;
      ErrorServerity.PcieErrStatusErrorSeverity= ERROR_SEVERITY_FATAL;
    }else 
    {
      ErrorServerity.GenErrorDataErrorSeverity = ERROR_RECOVERABLE;
      ErrorServerity.PcieErrStatusErrorSeverity= ERROR_RECOVERABLE;
    }
  } else 
  {
    PcieCorrErrorValid = PcieAerCorrErrCheck (Function, AerCapPtr);
    if (PcieCorrErrorValid) 
    {
      ErrorServerity.GenErrorDataErrorSeverity = ERROR_SEVERITY_CORRECTED;
      ErrorServerity.PcieErrStatusErrorSeverity= ERROR_SEVERITY_CORRECTED;
    }
  }
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)  
  
  if((SkipErrorReport == FALSE) || (ErrorServerity.GenErrorDataErrorSeverity == ERROR_SEVERITY_FATAL)) 
  {
#endif
    Status = LogPCIeError(ErrorServerity,Function);
    DEBUG((EFI_D_RAS|EFI_D_ERROR, "[RAS][%a]LogPCIeError:%r\r\n",__FUNCTION__,Status));
#if defined(PCIE_LEAKY_BUCKET_SUPPORT) && (PCIE_LEAKY_BUCKET_SUPPORT ==1)    
  }
#endif
  // Clear device error status
  RasSmmStsClr(Function);

  if (ErrorServerity.GenErrorDataErrorSeverity != ERROR_SEVERITY_FATAL) 
    return;

  // Trigger action - Warm reset
  IoWrite8 (0xCD6, 0x10);
  PwrRsrCfg = IoRead8 (0xCD7);
  PwrRsrCfg = PwrRsrCfg & 0xFD; //clear ToggleAllPwrGoodOnCf9
  IoWrite8 (0xCD7, PwrRsrCfg);
  IoWrite8(0xCF9, 0x2);
  IoWrite8(0xCF9, 0x6);
  CpuDeadLoop();
  
  return;
}

VOID
STATIC
PcieDevErrScanOnDevice (
  PCI_ADDR       Device,
  BOOLEAN        RootStatusSet
  )
{
  UINT8                   MaxFunc;
  UINT8                   CurrentFunc;

  MaxFunc = RasPciIsMultiFunctionDevice (Device.AddressValue) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    DEBUG ((EFI_D_ERROR, "[RAS] Checking Device: %d:%d:%d\n",
            Device.Address.Bus,
            Device.Address.Device,
            Device.Address.Function
            ));
    if (RasPciIsDevicePresent (Device.AddressValue)) {
      DEBUG ((EFI_D_ERROR, "[RAS] Scan Device Error report for Device = 0x%x:0x%x:0x%x\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function
      ));
      PcieDevErrScanOnFunction (Device,RootStatusSet);
    }else
     //if find no function on the device, will stop scan
        break;
  }
}

SCAN_STATUS
STATIC
PcieDevErrScanCallback (
  PCI_ADDR             Device,
  RAS_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_DEVICE_TYPE        DeviceType;
  BOOLEAN                 RootStatusSet;

  ScanStatus = SCAN_SUCCESS;
  DEBUG ((EFI_D_ERROR, "[RAS] PcieDevErrScanCallback for Device = %d:%d:%d\n",
          Device.Address.Bus,
          Device.Address.Device,
          Device.Address.Function
      ));
  ScanStatus = SCAN_SUCCESS;
  DeviceType = RasGetPcieDeviceType (Device);
  DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device type = 0x%x\n", DeviceType));
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device root port found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDevicePcieToPcix:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E device PCIE to PCIx found\n"));
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    DEBUG ((EFI_D_ERROR, "[RAS] PCI-E endpoint found\n"));
    RootStatusSet = *(BOOLEAN *)ScanData->Buffer;
    DEBUG ((EFI_D_ERROR, "[RAS] RootStatusSet = %x\n",RootStatusSet));
    PcieDevErrScanOnDevice (Device,RootStatusSet);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}
