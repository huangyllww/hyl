/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD SOC BIST Library
 *
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
#include "Uefi.h"
#include <Library/BaseLib.h>
#include "AMD.h"
#include "Filecode.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Ppi/NbioSmuServicesPpi.h>

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_PEISOCBISTZPLIB_PEISOCBISTZPLIB_FILECODE

 /*---------------------------------------------------------------------------------------*/
 /**
  *
  * Reads the status of CCX BIST
  *
  * @param[in]      CcxNumber              Ccx number to read BIST data from
  * @param[in]      SystemDieCount         System based die number
  * @param[in]      PeiServices            Pointer to PEI services
  *
  *
  * @retval         AGESA_ALERT            A BIST error has occurred
  * @retval         AGESA_SUCCESS          No BIST errors have occured
  */
AGESA_STATUS
ReadCcxBistData (
  IN       UINTN            CcxNumber,
  IN       UINTN            SystemDieCount,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8                         BistEn;
  UINT8                         ColdResetMBistEn;
  UINT8                         BistDone;
  UINT8                         BistPF;
  UINT32                        BistStatus;
  UINT32                        BistEnFuse;
  UINT32                        ColdResetMBistEnFuse;
  EFI_STATUS                    CalledStatus;
  PEI_AMD_NBIO_SMU_SERVICES_PPI *NbioSmuServices;


  CalledStatus = (**PeiServices).LocatePpi (
                                 PeiServices,
                                 &gAmdNbioSmuServicesPpiGuid,
                                 0,
                                 NULL,
                                 &NbioSmuServices
                                 );

  ASSERT (CalledStatus == EFI_SUCCESS);

  NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieCount, 0x5D0CC, &BistEnFuse);
  BistEn = (BistEnFuse >> 4) & 0x1;

  NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieCount, 0x5D0DC, &ColdResetMBistEnFuse);
  ColdResetMBistEn = (ColdResetMBistEnFuse >> 25) & 0x1;

  // Check BIST status only if BIST is fuse enabled
  if ((BistEn == 0x1) || (ColdResetMBistEn == 0x1)){
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieCount, 0x18080064 | (CcxNumber << 22), &BistStatus);

    BistDone = (BistStatus >> 9) & 0x1;
    if (BistDone == 0x1) {
      BistPF = (BistStatus >> 8) & 0x1;
      if (BistPF == 0) {
        return AGESA_ALERT;
      }
    }
  }
  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
 /**
  *
  * Reads the status of non-CCX BIST
  *
  * @param[in]      DieNumber              Die number to read data from
  * @param[out]     BistData               Value of BIST status register
  * @param[in]      PeiServices            Pointer to PEI services
  *
  * @retval         AGESA_ALERT            A BIST error has occurred
  * @retval         AGESA_SUCCESS          No BIST errors have occured
  */
AGESA_STATUS
ReadNonCcxBistData (
  IN       UINTN            DieNumber,
       OUT UINTN            *BistData,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8                         RunMBist;
  UINT32                        RunMBistFuse;
  EFI_STATUS                    CalledStatus;
  PEI_AMD_NBIO_SMU_SERVICES_PPI *NbioSmuServices;

  CalledStatus = (**PeiServices).LocatePpi (
                                 PeiServices,
                                 &gAmdNbioSmuServicesPpiGuid,
                                 0,
                                 NULL,
                                 &NbioSmuServices
                                 );

  ASSERT (CalledStatus == EFI_SUCCESS);

  NbioSmuServices->SmuRegisterRead (NbioSmuServices, DieNumber, 0x5D258, &RunMBistFuse);
  RunMBist = (RunMBistFuse >> 6) & 0x1;

  if (RunMBist == 0x1) {
    NbioSmuServices->SmuRegisterRead (NbioSmuServices, DieNumber, 0x03810038, BistData);

    if (*BistData != 0) {
      return AGESA_ALERT;
    }
  }

  return AGESA_SUCCESS;
}