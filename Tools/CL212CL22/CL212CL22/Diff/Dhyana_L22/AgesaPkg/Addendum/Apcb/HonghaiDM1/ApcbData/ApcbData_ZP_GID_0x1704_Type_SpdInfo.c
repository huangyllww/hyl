
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
//This file includes code originally published under the following license.


/** @file
  This module produces the EFI_PEI_S3_RESUME_PPI.
  This module works with StandAloneBootScriptExecutor to S3 resume to OS.
  This module will excute the boot script saved during last boot and after that,
  control is passed to OS waking up handler.

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials are licensed and made available under the terms and conditions
  of the BSD License which accompanies this distribution.  The
  full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "MyPorting.h"
#include "ApcbV2.h"

#include <MiscMemDefines.h>
#include <Apcb.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER  ApcbTypeHeader = {
  APCB_GROUP_MEMORY, APCB_MEM_TYPE_SPD_INFO, (sizeof (APCB_TYPE_HEADER) + sizeof (PSP_SPD_STRUCT)), 0, 0, 0
};                                                                                                                                                     // SizeOfType will be fixed up by tool
///PSP_SPD_STRUCT is too big, redefine myPSP_SPD_STRUCT

PSP_SPD_STRUCT  memPspSpd = {
  32,                                 // < TotalDimms - Total number of DIMMs in the system
  {
        //SpdValid, Socket, umc, dimm
    RDIMM_INFO (TRUE, 0, 0, 0),
    RDIMM_INFO (FALSE, 0, 0, 1),
    RDIMM_INFO (TRUE, 0, 1, 0),
    RDIMM_INFO (FALSE, 0, 1, 1),
    RDIMM_INFO (TRUE, 0, 2, 0),
    RDIMM_INFO (FALSE, 0, 2, 1),
    RDIMM_INFO (TRUE, 0, 3, 0),
    RDIMM_INFO (FALSE, 0, 3, 1),
    RDIMM_INFO (TRUE, 0, 4, 0),
    RDIMM_INFO (FALSE, 0, 4, 1),
    RDIMM_INFO (TRUE, 0, 5, 0),
    RDIMM_INFO (FALSE, 0, 5, 1),
    RDIMM_INFO (TRUE, 0, 6, 0),
    RDIMM_INFO (FALSE, 0, 6, 1),
    RDIMM_INFO (TRUE, 0, 7, 0),
    RDIMM_INFO (FALSE, 0, 7, 1),
    RDIMM_INFO (TRUE, 1, 0, 0),
    RDIMM_INFO (FALSE, 1, 0, 1),
    RDIMM_INFO (TRUE, 1, 1, 0),
    RDIMM_INFO (FALSE, 1, 1, 1),
    RDIMM_INFO (TRUE, 1, 2, 0),
    RDIMM_INFO (FALSE, 1, 2, 1),
    RDIMM_INFO (TRUE, 1, 3, 0),
    RDIMM_INFO (FALSE, 1, 3, 1),
    RDIMM_INFO (TRUE, 1, 4, 0),
    RDIMM_INFO (FALSE, 1, 4, 1),
    RDIMM_INFO (TRUE, 1, 5, 0),
    RDIMM_INFO (FALSE, 1, 5, 1),
    RDIMM_INFO (TRUE, 1, 6, 0),
    RDIMM_INFO (FALSE, 1, 6, 1),
    RDIMM_INFO (TRUE, 1, 7, 0),
    RDIMM_INFO (FALSE, 1, 7, 1),
  },
};

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}



