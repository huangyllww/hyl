/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM structures and definitions
 *
 * Contains AMD CPM Platform Interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Include
 * @e \$Revision$   @e \$Date$
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

#ifndef _AMD_CPM_PLATFORM_H_
#define _AMD_CPM_PLATFORM_H_

// Platform BIOS should redefine the following CPU SUPPORT defines in AmdCpmPkg\Addendum\Oem\*Board*\AmdCpmPlatform.h.
// Then, copy AmdCpmPkg\Addendum\Oem\*Board*\AmdCpmPlatform.h to AmdCpmPkg\AmdCpmPlatform.h before building BIOS.
  #define AMD_CPM_CPU_FAMILY_0x14_ON_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_TN_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_KV_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_CZ_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x16_KB_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x16_ML_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x16_NL_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x21_AM_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_ST_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_BR_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x17_ZP_SUPPORT
  #define HYGON_CPM_CPU_FAMILY_0x18_SL1_SUPPORT
#endif