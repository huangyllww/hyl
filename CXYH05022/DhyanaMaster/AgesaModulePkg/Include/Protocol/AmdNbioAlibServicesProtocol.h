/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Pstate Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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

#ifndef _NBIO_ALIB_SERVICES_PROTOCOL_H_
#define _NBIO_ALIB_SERVICES_PROTOCOL_H_

// Current PROTOCOL revision
#define AMD_ALIB_SERVICES_DXE_REVISION   0x00

///
/// Forward declaration for the NBIO_ALIB_SERVICES_PROTOCOL
///
typedef struct _DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL;


//
// Protocol Definitions
//
/**
  Returns the Alib address
  This
    A pointer to the DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL instance.
  DebugOptions
    A pointer to a pointer to store the address of ALib
**/
typedef
VOID
(EFIAPI * AMD_ALIB_SERVICES_DXE_CONFIG) (
  IN  DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL      *This,
  OUT VOID                                     **AlibAddress
);

///
/// The Dxe of ALIB services
///
struct _DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL {
  UINT32                                Revision;      ///< revision
  AMD_ALIB_SERVICES_DXE_CONFIG          AlibGetConfig; ///<
};

extern EFI_GUID gAmdNbioALibServicesProtocolGuid;

#endif