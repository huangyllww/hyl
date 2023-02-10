/**
 * @file
 *
 * Required OEM hooks for CCX initialization
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
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
#include <Uefi.h>
#include <Library/PcdLib.h>

//DM1-277 >>>
UINT32 ApSyncMemRead32(void)
{
    volatile UINT32 *pDataPtr = (UINT32 *)(UINTN)(PcdGet32(PcdApSyncFlagMemAddress));
    return (*(volatile UINT32 *)pDataPtr);
}

void ApSyncMemWrite32(UINT32 Data32)
{
    volatile UINT32 *pDataPtr = (UINT32 *)(UINTN)(PcdGet32(PcdApSyncFlagMemAddress));
    *(volatile UINT32 *)pDataPtr = Data32;
}
//DM1-277 <<<

/*--------------------------------------------------------------------------------------*/
/**
 *  Hook to store the vector that all APs should jump to out of reset to a non-volatile,
 *  shared location.
 *
 *  @param[in]     ApInitAddress     Address of the code that AP should jump to
 *  @param[in,out] ContentToRestore  The current value in the non-volatile storage
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
SaveApInitVector (
  IN       UINT32  ApInitAddress,
  IN OUT   UINT32  *ContentToRestore
  )
{
    if(ContentToRestore != NULL) {
        *ContentToRestore = ApSyncMemRead32();
    }
    ApSyncMemWrite32(ApInitAddress);
}

/*--------------------------------------------------------------------------------------*/
/**
 *  Hook to restore the initial content of the non-volatile storage location.
 *
 *  @param[in]     ContentToRestore  The value to restore
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
RestoreContentVector (
  IN       UINT32  ContentToRestore
  )
{
    ApSyncMemWrite32(ContentToRestore);
}


/*--------------------------------------------------------------------------------------*/
/**
 *  Save AP Sync flag address
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
SaveApSyncFlag (
  IN       UINT32  ApSyncFlagAddress,
  IN OUT   UINT32  *ContentToRestore
  )
{
    if(ContentToRestore != NULL) {
        *ContentToRestore = ApSyncMemRead32();
    }
    ApSyncMemWrite32(ApSyncFlagAddress);
}

/*--------------------------------------------------------------------------------------*/
/**
 *  Restor AP Sync flag
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
RestoreContentApSyncFlag (
  IN       UINT32  ContentToRestore
  )
{
    ApSyncMemWrite32(ContentToRestore);
}

/*--------------------------------------------------------------------------------------*/
/**
 *  Get AP Sync flag Address
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID GetApSyncFlagAddress (
  OUT      UINT32  *FlagAddress
  )
{
	*FlagAddress = ApSyncMemRead32();
}

