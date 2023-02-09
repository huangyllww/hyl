/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/ApcbZpLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspFlashUpdateLib.h>
#include <Library/AmdPspApobLib.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmReadyToBoot.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        LIBRARY_APCBLIB_APCBLIB_FILECODE

#define APCB_SIGNATURE  0x42435041ul

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern EFI_GUID gSmmApcbRtShadowAddressProtocolGuid;

/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */
APCB_HEADER *mApcbSmmRtShadowCopyProtocol = NULL;
APCB_HEADER **mApcbSmmRtShadowCopyPtr = NULL;
BOOLEAN     mInSmm = FALSE;
BOOLEAN     mAtRuntime = FALSE;
UINT64      mApcbEntryAddress = 0;
UINT32      mApcbEntrySize = 0;
EFI_SMM_SYSTEM_TABLE2          *mSmst = NULL;
UINT8                     mApcbInstance = 0;

/*
 * Function to get ApcbShadowCopy Address, return NULL if fail
*/
VOID *
GetApcbShadowCopy (
  )
{
  VOID * ApcbData;

  ApcbData = NULL;
  if (mAtRuntime) {
    ApcbData = *mApcbSmmRtShadowCopyPtr;
  } else {
    ApcbData = (VOID *) (UINTN) PcdGet64 (PcdApcbShadowAddress);
  }
  ASSERT (ApcbData != NULL);
  ASSERT ((*(UINT32 *)ApcbData) == APCB_SIGNATURE);
  IDS_HDT_CONSOLE_PSP_TRACE ("GetApcbShadowCopy %x\n", ApcbData);

  return ApcbData;
}

/*
 * Function to set ApcbShadowCopy Address
*/
VOID
SetApcbShadowCopy (
  VOID * ApcbShadowAddress
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("SetApcbShadowCopy %x\n", ApcbShadowAddress);
  if (mAtRuntime) {
    *mApcbSmmRtShadowCopyPtr = ApcbShadowAddress;
  } else {
    PcdSet64S (PcdApcbShadowAddress, (UINT64) (UINTN) ApcbShadowAddress);
  }
}
/*
 * Function to calculate APCB checksum
*/
UINT8
ApcbCalcCheckSum (
  IN       VOID             *ApcbPtr,
  IN       UINT32           Length
  )
{
  UINT8  CheckSum;
  UINT8  *DataPtr;

  CheckSum = 0;
  DataPtr = ApcbPtr;
  while (Length --) {
    CheckSum += *(DataPtr ++);
  }
  return CheckSum;
}
/*
 * Function to get the APCB data of a given type
*/
EFI_STATUS
ApcbGetType (
  IN     UINT16          GroupId,
  IN     APCB_PARAM_TYPE ApcbParamType,
  IN     UINT16          InstanceId,
  IN     UINT8           *TypeDataStream,
  IN OUT UINT32          *TypeDataSize
  )
{
  EFI_STATUS          Status;
  BOOLEAN             ApcbRecoveryFlag;
  APCB_HEADER         *ApcbHeader;
  UINT8               *ApcbEnding;
  APCB_GROUP_HEADER   *ApcbGroupHeader;
  APCB_TYPE_HEADER    *ApcbTypeHeader;
  UINT8               *ApcbData;
  UINT32              ApcbDataSize;
  AMD_CONFIG_PARAMS   StdHeader;

  IDS_HDT_CONSOLE_PSP_TRACE ("ApcbGetType G:%x T:%x I:%x Data:&%x Size:%x\n",
                              GroupId, ApcbParamType, InstanceId, TypeDataStream, TypeDataSize);
  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  ApcbData = GetApcbShadowCopy ();
  ApcbHeader = (APCB_HEADER *) (ApcbData);
  ASSERT (APCB_SIGNATURE == ApcbHeader->Signature);
  ApcbEnding = (UINT8 *) (ApcbData + ApcbHeader->SizeOfApcb - 1);
  ApcbGroupHeader = (APCB_GROUP_HEADER *) (VOID *) ((UINT8 *) ApcbHeader + sizeof (APCB_HEADER));
  ApcbTypeHeader = (APCB_TYPE_HEADER *) (VOID *) ((UINT8 *) ApcbGroupHeader + sizeof (APCB_GROUP_HEADER));
  IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tAPCB Size: 0x%04X\n", ApcbHeader->SizeOfApcb);
  while ((((UINT8 *) ApcbGroupHeader + sizeof (APCB_GROUP_HEADER) - 1) <= ApcbEnding) &&
          ((UINT8 *) ((UINTN) (VOID *) ApcbTypeHeader + ApcbTypeHeader->SizeOfType - 1) <= ApcbEnding)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tCurrent Group: 0x%04X, Current Type: 0x%04X\n", ApcbGroupHeader->GroupId, ApcbTypeHeader->TypeId);
    if (ApcbGroupHeader->GroupId == GroupId) {
      if (ApcbTypeHeader->TypeId == ApcbParamType) {
        if (ApcbTypeHeader->InstanceId == InstanceId) {
          //
          // Found match!
          //
          IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tFound TypeId: %x,  Total TypeSize: %x\n", ApcbTypeHeader->TypeId, ApcbTypeHeader->SizeOfType);
          // Check the input buffer size, it too small return the Actual type size
          ApcbDataSize = ApcbTypeHeader->SizeOfType - sizeof (APCB_TYPE_HEADER);
          if (*TypeDataSize < ApcbDataSize) {
            IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tSize too small\n");
            *TypeDataSize = ApcbDataSize;
            return EFI_BUFFER_TOO_SMALL;
          }
          *TypeDataSize = ApcbDataSize;
          LibAmdMemCopy (TypeDataStream, ApcbTypeHeader + 1, ApcbDataSize, &StdHeader);
          return EFI_SUCCESS;
        }
      } else {
        ApcbTypeHeader = (APCB_TYPE_HEADER *) ((UINTN) (CHAR8 *) ApcbTypeHeader + ApcbTypeHeader->SizeOfType);
        // Adjust alignment
        ApcbTypeHeader = (APCB_TYPE_HEADER *) ((UINT8 *) ApcbTypeHeader + ((ALIGN_SIZE_OF_TYPE - ((UINTN) (UINT8 *) ApcbTypeHeader) % ALIGN_SIZE_OF_TYPE) & (ALIGN_SIZE_OF_TYPE - 1)));
        if ((UINT8 *) ApcbTypeHeader > (UINT8 *) ((UINTN) (CHAR8 *) ApcbGroupHeader + ApcbGroupHeader->SizeOfGroup - 1)) {
          return EFI_NOT_FOUND;
        }
      }
    } else {
      ApcbGroupHeader = (APCB_GROUP_HEADER *) ((UINTN) (VOID *) ApcbGroupHeader + ApcbGroupHeader->SizeOfGroup);
      //
      // APCB Type header following APCP Group header should already be 4-bytes aligned, there's no need for
      // extra adjustment on ApcbTypeHeader pointer.
      //
      ApcbTypeHeader = (APCB_TYPE_HEADER *) (VOID *) (ApcbGroupHeader + 1);
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\n\t\tNo matching type found in APCB.\n");

  return EFI_UNSUPPORTED;
}

/**
 *  Write APCB data to BIOS Directory APCB Entry 0x60
 *
 *
 *  @retval EFI_SUCCESS       The Data save to FLASH successfully
 *  @retval Other             Some error occurs when executing this function.
 *
 **/
EFI_STATUS
AmdPspWriteBackApcbShadowCopy (
  VOID
  )
{
  APCB_HEADER                     *ApcbHeader;
  UINT8                           *ApcbData;
  EFI_STATUS Status;
  BOOLEAN ApcbRecoveryFlag;

  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspWriteBackApcbShadowCopy Enter\n");

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }
  //Check the size of APCB DRAM copy
  ApcbData = GetApcbShadowCopy ();
  ApcbHeader = (APCB_HEADER *) ApcbData;
  if (ApcbHeader->SizeOfApcb > mApcbEntrySize) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB SPI Entry too small\n");
    return EFI_UNSUPPORTED;
  }
  //Compare if any changes
  if (CompareMem (ApcbData, (VOID *) (UINTN) mApcbEntryAddress, ApcbHeader->SizeOfApcb)) {
    //Update UniqueApcbInstance when APCB content has been updated
    ApcbHeader->UniqueApcbInstance += 1;
    ApcbHeader->CheckSumByte = 0;
    ApcbHeader->CheckSumByte = (UINT8) (0x100 - ApcbCalcCheckSum (ApcbData, ApcbHeader->SizeOfApcb));
    IDS_HDT_CONSOLE_PSP_TRACE ("UniqueApcbInstance %x\n", ApcbHeader->UniqueApcbInstance);
    IDS_HDT_CONSOLE_PSP_TRACE ("Data difference between APCB buffer & SPI copy\n");
    Status = PspUpdateFlash ((UINT32) mApcbEntryAddress, ApcbHeader->SizeOfApcb, ApcbData);
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspWriteBackApcbShadowCopy Exit\n");

  return Status;
}
/**
 *
 *  Init mApcbSmmRtShadowCopy
 *
 **/
EFI_STATUS
EFIAPI
ApcbRTBCallBack (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("ApcbRTBCallBack\n");
  //Check if mApcbSmmRtShadowCopy already initialized
  //Use APCB signature to do the simple check below
  if (((APCB_HEADER *) *mApcbSmmRtShadowCopyPtr)->Signature != APCB_SIGNATURE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Copy the Apcb from SPI %x to APCB RT copy %x\n", mApcbEntryAddress, *mApcbSmmRtShadowCopyPtr);
    //Copy the Apcb from SPI to Shadow Copy
    CopyMem (*mApcbSmmRtShadowCopyPtr, (VOID *) (UINTN) mApcbEntryAddress, mApcbEntrySize);
  }
  mAtRuntime = TRUE;
  return EFI_SUCCESS;
}
/**
 *
 *  ApcbLibConstructor, initial the shadow copy of APCB data, and save the address to PCD
 *
 **/
EFI_STATUS
EFIAPI
ApcbLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  APCB_HEADER               *ApcbShadowCopy;
  BOOLEAN                   ApcbEntryExist;
  TYPE_ATTRIB               TypeAttrib;
  UINT64                    EntryDest;
  EFI_SMM_BASE2_PROTOCOL    *SmmBase2;
  EFI_STATUS                Status;
  VOID                      *Registration;
  EFI_HANDLE                Handle;
  BOOLEAN                   ApcbRecoveryFlag;

  //Locate the global data structure via PCD
  ApcbShadowCopy = NULL;
  mInSmm = FALSE;
  IDS_HDT_CONSOLE_PSP_TRACE ("ApcbLibConstructor\n");

  //Exit if BR program
  if (SocFamilyIdentificationCheck (F15_BR_RAW_ID)) {
    return EFI_SUCCESS;
  }
  //Exit service, if recovery flag set
  ApcbRecoveryFlag = FALSE;
  Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
  if (ApcbRecoveryFlag) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
    return EFI_SUCCESS;
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_SUCCESS;
  }

  //Check the existence and size of APCB  entry 0x60
  Status = AmdPspGetApobApcbInstance (&mApcbInstance);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB Instance get fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Active APCB Instance %x\n", mApcbInstance);
  ApcbEntryExist = BIOSEntryInfo (BIOS_APCB_INFO, mApcbInstance, &TypeAttrib, &mApcbEntryAddress, &mApcbEntrySize, &EntryDest);
  if (!ApcbEntryExist) {
    IDS_HDT_CONSOLE_PSP_TRACE ("APCB Entry not exist\n");
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID**) &SmmBase2);
  if (!EFI_ERROR (Status)) {
    SmmBase2->InSmm (SmmBase2, &mInSmm);
  } else {
    IDS_HDT_CONSOLE_PSP_TRACE ("gEfiSmmBase2ProtocolGuid Locate Fail\n");
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("%a\n", mInSmm ? "InSmm" : "OutSmm");
  //Check if Posttime APCB shadow already initialized
  ApcbShadowCopy = (APCB_HEADER *) (UINTN) PcdGet64 (PcdApcbShadowAddress);
  if (ApcbShadowCopy == NULL) {
    //Shadow Copy haven't init yet
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate Posttime APCB Shadow Copy with 0x%x bytes\n", mApcbEntrySize);
    Status = gBS->AllocatePool (
              EfiBootServicesData,
              mApcbEntrySize,
              (VOID **) &ApcbShadowCopy
              );
    ASSERT (ApcbShadowCopy != NULL);
    if (ApcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB Shadow Copy allocate fail\n");
      return EFI_SUCCESS;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", ApcbShadowCopy, mApcbEntrySize);

    ZeroMem (ApcbShadowCopy, mApcbEntrySize);
    //Copy the Apcb from SPI to Shadow Copy
    CopyMem (ApcbShadowCopy, (VOID *) (UINTN) mApcbEntryAddress, mApcbEntrySize);
    //Calc the checksum, and report the warning
    if (ApcbCalcCheckSum (ApcbShadowCopy, ApcbShadowCopy->SizeOfApcb) != 0) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Warning: APCB Checksum not zero\n");
    }
    PcdSet64S (PcdApcbShadowAddress, (UINT64) (UINTN) ApcbShadowCopy);
  }

  if (!mInSmm) {
    return EFI_SUCCESS;
  }

  // We are now in SMM
    // get SMM table base
  Status = SmmBase2->GetSmstLocation (SmmBase2, &mSmst);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("GetSmstLocation Fail\n");
    return EFI_SUCCESS;
  }
  //Check if Posttime APCB shadow already initialized by locate SmmProtocol
  Status = mSmst->SmmLocateProtocol (&gSmmApcbRtShadowAddressProtocolGuid, NULL, (VOID **) &mApcbSmmRtShadowCopyPtr);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate RT APCB Shadow Copy with 0x%x bytes\n", mApcbEntrySize);
    //Allocate APCB shadow for SMM RT
    Status = mSmst->SmmAllocatePool (
                     EfiRuntimeServicesData,
                     mApcbEntrySize,
                     (VOID **) &ApcbShadowCopy
                     );
    ASSERT (ApcbShadowCopy != NULL);
    if (ApcbShadowCopy == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB Shadow Copy allocate fail\n");
      return EFI_SUCCESS;
    }
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", ApcbShadowCopy, mApcbEntrySize);

    ZeroMem (ApcbShadowCopy, mApcbEntrySize);
    mApcbSmmRtShadowCopyProtocol = ApcbShadowCopy;
    mApcbSmmRtShadowCopyPtr = &mApcbSmmRtShadowCopyProtocol;
    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gSmmApcbRtShadowAddressProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mApcbSmmRtShadowCopyProtocol
                      );
  }

  //Register SMM Exit boot service callback used init mApcbSmmRtShadowCopy
  mSmst->SmmRegisterProtocolNotify (
           &gEdkiiSmmReadyToBootProtocolGuid,
           ApcbRTBCallBack,
           &Registration
           );

  return EFI_SUCCESS;
}
