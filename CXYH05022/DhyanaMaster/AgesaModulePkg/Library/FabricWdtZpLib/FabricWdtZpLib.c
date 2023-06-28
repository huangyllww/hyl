/* $NoKeywords:$ */
/**
 * @file
 *
 *Fabric watchdog timer library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
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


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AMD.h"
#include <FabricRegistersZP.h>
#include "Library/IdsLib.h"
#include <Library/FabricWdtLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FABRICWDTZPLIB_FABRICWDTZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT32 DfZpWdtCntSelDecode [] =
{
  31,
  127,
  255,
  511,
  1023,
  2047,
  4095,
  16383
};
#define NumberOfDfWdtCntSel (sizeof (DfZpWdtCntSelDecode) / sizeof (DfZpWdtCntSelDecode[0]))

UINT64 DfZpWdtBaseSelDecode [] =
{
  1310000,
  1280,
  10,
  0
};
#define NumberOfDfWdtBaseSel (sizeof (DfZpWdtBaseSelDecode) / sizeof (DfZpWdtBaseSelDecode[0]))

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FabricGetCcmWdtInfo
 *
 *  @param[in, out] DfCcmTimeout        DF CCM WDT time in ns
 *
 *  @retval TRUE  WDT is enabled
 *  @retval FALSE WDT is disabled
 *
 */
BOOLEAN
FabricGetCcmWdtInfo (
  IN       UINT64 *DfCcmTimeout
  )
{
  BOOLEAN                DfWdtEn;
  DF_GLBL_CTRL_REGISTER  DfGlblCtrl;

  DfGlblCtrl.Value = FabricRegisterAccRead (0, 0, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG, FABRIC_REG_ACC_BC);
  ASSERT (DfGlblCtrl.Field.WDTBaseSel < NumberOfDfWdtBaseSel);
  ASSERT (DfGlblCtrl.Field.CCMWDTCntSel < NumberOfDfWdtCntSel);
  if (DfGlblCtrl.Field.WDTBaseSel < 3) {
    DfWdtEn = TRUE;
    *DfCcmTimeout = (UINT64) (DfZpWdtBaseSelDecode[DfGlblCtrl.Field.WDTBaseSel] * DfZpWdtCntSelDecode[DfGlblCtrl.Field.CCMWDTCntSel]);
  } else {
    DfWdtEn = FALSE;
    *DfCcmTimeout = 0;
  }
  return DfWdtEn;
}