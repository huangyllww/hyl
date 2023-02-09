/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD HSTI DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 18:23:05 +0800 (Fri, 30 Jan 2015) $
 *
 */
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
#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Guid/EventGroup.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Hsti.h>
#include <Library/HstiLib.h>
#include <Library/MemoryAllocationLib.h>
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV1.h>
#include <Library/AmdPspMboxLibV1.h>
#include <Protocol/AdapterInformation.h>
#include <AmdHsti.h>

#define  HSTI_AMD_V1_IMPLEMENTATIONID           L"AMD HSTI Implementation V1"
//#define AMD_HSTI_CS_SYSTEM_SUPPORT
/**
  AMD HSTI driver's entry point.

  Create AMD gAdapterInfoPlatformSecurityGuid protocol

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
**/
EFI_STATUS
EFIAPI
AmdHstiV1Entry (
  IN  EFI_HANDLE           ImageHandle,
  IN  EFI_SYSTEM_TABLE     *SystemTable
  )
{
  ADAPTER_INFO_PLATFORM_SECURITY   *Hsti;
  UINTN                            HstiSize;
  UINT8                            *HstiData;
  EFI_STATUS                       Status;
  UINT32                           HSTIState;

  //Allocate Pool for hold ADAPTER_INFO_PLATFORM_SECURITY
  HstiSize = sizeof (ADAPTER_INFO_PLATFORM_SECURITY) + 3 * HSTI_AMD_FEATUERS_SIZE_IN_BYTES + HSTI_AMD_ERROR_STRING_SIZE;
  Hsti = AllocateZeroPool (HstiSize);
  if (Hsti == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Initialize HSTI table static filed
  //
  Hsti->Version              = PLATFORM_SECURITY_VERSION_VNEXTCS;
  Hsti->Role                 = PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE;
  StrCpy (Hsti->ImplementationID, HSTI_AMD_V1_IMPLEMENTATIONID);
  Hsti->SecurityFeaturesSize = HSTI_AMD_FEATUERS_SIZE_IN_BYTES;
  //
  // Requirement
  //
  HstiData = (UINT8 *) (Hsti + 1);
  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_0] = HSTI_AMD_CRYPTO_STRENGTH;

  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_1] = HSTI_AMD_FWCODE_PROTECT_PROTECT_SPI +
                                            HSTI_AMD_FWCODE_PROTECT_SIGNED_FW_CHECK;

  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_2] = HSTI_AMD_SECURE_FW_UPDATE_DFT_TESTKEY +
                                            HSTI_AMD_SECURE_FW_UPDATE_CHECK_TESTKEY_IN_PRODUCTION +
                                            HSTI_AMD_SECURE_FW_UPDATE_ROLLBACK_CHECK;

  HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_3] = HSTI_AMD_SECUREBOOT_BACKDOOR_INLINE_PROMPT_CHECK +
                                            HSTI_AMD_SECUREBOOT_BACKDOOR_MANUFACTURE_CHECK;
  //CS System
  #ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] = HSTI_AMD_FEATURE_PSP_SECURE_EN +
                                              HSTI_AMD_FEATURE_PSP_PLATFORM_SECURE_BOOT_EN +
                                              HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON;
  #else
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] = HSTI_AMD_FEATURE_PSP_SECURE_EN +
                                              HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON;
  #endif

  //CS System
  #ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_5] = HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN +
                                              HSTI_AMD_CSM_DISABLE_ON_CS_SYSTEM;
  #else
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_5] = HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN;
  #endif

  //
  // Implementation
  //
  HstiData += HSTI_AMD_FEATUERS_SIZE_IN_BYTES;

  //CS System
  #ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] = HSTI_AMD_FEATURE_PSP_SECURE_EN +
                                              HSTI_AMD_FEATURE_PSP_PLATFORM_SECURE_BOOT_EN +
                                              HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON;
  #else
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] = HSTI_AMD_FEATURE_PSP_SECURE_EN +
                                              HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON;
  #endif

  //
  // Verification
  //
  HstiData += HSTI_AMD_FEATUERS_SIZE_IN_BYTES;

  //Send Mailbox to PSP to query HSTI related status
  HSTIState = 0;
  PspMboxBiosQueryHSTIState (&HSTIState);

  if (HSTIState & HSTISTATE_PSP_SECURE_EN) {
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] |= HSTI_AMD_FEATURE_PSP_SECURE_EN;
  }
#ifdef AMD_HSTI_CS_SYSTEM_SUPPORT
  if (HSTIState & HSTISTATE_PSP_PLATFORM_SECURE_EN) {
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] |= HSTI_AMD_FEATURE_PSP_PLATFORM_SECURE_BOOT_EN;
  }
#endif
  if (HSTIState & HSTISTATE_PSP_DEBUG_LOCK_ON) {
    HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4] |= HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON;
  }

  //
  // Initialize error string
  //

  //
  // Install HSTI table.
  //
  Status = HstiLibSetTable ((VOID *) Hsti, HstiSize);
  // Use HstiLibSetFeaturesVerified call to make sure gAdapterInfoPlatformSecurityGuid has been set
  HstiLibSetFeaturesVerified (PLATFORM_SECURITY_ROLE_PLATFORM_REFERENCE, HSTI_AMD_V1_IMPLEMENTATIONID, HSTI_AMD_FEATURE_BYTE_INDEX_4, HstiData[HSTI_AMD_FEATURE_BYTE_INDEX_4]);

  return Status;
}

