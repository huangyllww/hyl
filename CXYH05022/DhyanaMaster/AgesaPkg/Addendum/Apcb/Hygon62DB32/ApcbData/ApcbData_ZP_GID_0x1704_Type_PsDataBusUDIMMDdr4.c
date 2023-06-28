
/* $NoKeywords:$ */
/**
 * @file
 *
 * @e \$Revision:$   @e \$Date:$
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
 *
 ***************************************************************************/

#include "MyPorting.h"
#include "ApcbV2.h"

#include <MiscMemDefines.h>
#include <APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

//UDIMM DATABUS DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER  ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_UDIMM_DDR4_DATA_BUS, (sizeof(APCB_TYPE_HEADER) + sizeof (PSCFG_DATABUS_ENTRY_D4) * 30), 0, 0, 0 };  // SizeOfType will be fixed up by tool
PSCFG_DATABUS_ENTRY_D4 DataBusUdimmDdr4[] = {
  {1, DDR1333,                                                   V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_80, RTTWR_OFF, RTTPRK_OFF, 0x1E, 0x1E, 0xA},
  {1, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667 + DDR2933, V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
  {1, DDR3200,                                                   V1_2, DIMM_SR,           NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
  {2, DDR1333,                                                   V1_2, NP,                DIMM_SR,           RTTNOM_OFF,RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18},
  {2, DDR1333,                                                   V1_2, NP,                DIMM_DR,           RTTNOM_OFF,RTTWR_80,  RTTPRK_240, 0x3F, 0x3F, 0x18},
  {2, DDR1333,                                                   V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_80, RTTWR_OFF, RTTPRK_OFF, 0x1E, 0x1E, 0xA},
  {2, DDR1333,                                                   V1_2, DIMM_SR,           DIMM_SR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {2, DDR1333,                                                   V1_2, DIMM_DR,           DIMM_DR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {2, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, NP,                DIMM_SR,           RTTNOM_OFF,RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18},
  {2, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, NP,                DIMM_DR,           RTTNOM_OFF,RTTWR_80,  RTTPRK_240, 0x3F, 0x3F, 0x18},
  {2, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
  {2, DDR1600 + DDR1866 + DDR2133 + DDR2400,                     V1_2, DIMM_SR,           DIMM_SR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {2, DDR1600 + DDR1866 + DDR2133 + DDR2400,                     V1_2, DIMM_DR,           DIMM_DR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {2, DDR2933,                                                   V1_2, NP,                DIMM_SR,           RTTNOM_OFF,RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18},
  {2, DDR2933,                                                   V1_2, DIMM_SR,           NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
//HYGON_DX Config
  {0x81, DDR1333,                                                   V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_80, RTTWR_OFF, RTTPRK_OFF, 0x1E, 0x1E, 0xA},
  {0x81, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667 + DDR2933, V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
  {0x81, DDR3200,                                                   V1_2, DIMM_SR,           NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
  {0x82, DDR1333,                                                   V1_2, NP,                DIMM_SR,           RTTNOM_OFF,RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18},
  {0x82, DDR1333,                                                   V1_2, NP,                DIMM_DR,           RTTNOM_OFF,RTTWR_80,  RTTPRK_240, 0x3F, 0x3F, 0x18},
  {0x82, DDR1333,                                                   V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_80, RTTWR_OFF, RTTPRK_OFF, 0x1E, 0x1E, 0xA},
  {0x82, DDR1333,                                                   V1_2, DIMM_SR,           DIMM_SR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {0x82, DDR1333,                                                   V1_2, DIMM_DR,           DIMM_DR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {0x82, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, NP,                DIMM_SR,           RTTNOM_OFF,RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18},
  {0x82, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, NP,                DIMM_DR,           RTTNOM_OFF,RTTWR_80,  RTTPRK_240, 0x3F, 0x3F, 0x18},
  {0x82, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, DIMM_SR + DIMM_DR, NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
  {0x82, DDR1600 + DDR1866 + DDR2133 + DDR2400,                     V1_2, DIMM_SR,           DIMM_SR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {0x82, DDR1600 + DDR1866 + DDR2133 + DDR2400,                     V1_2, DIMM_DR,           DIMM_DR,           RTTNOM_34, RTTWR_120, RTTPRK_240, 0x3F, 0x3F, 0x18},
  {0x82, DDR2933,                                                   V1_2, NP,                DIMM_SR,           RTTNOM_OFF,RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18},
  {0x82, DDR2933,                                                   V1_2, DIMM_SR,           NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0xE},
 };
        
        
        
APCB_TYPE_DATA_END_SIGNATURE();

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}


