/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPM structures and definitions
 *
 * Contains AMD CPM CPU/APU Option Definitions
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

#ifndef _OPTION_CPU_INSTALL_H_
#define _OPTION_CPU_INSTALL_H_

#ifdef AMD_CPM_CPU_FAMILY_0x14_ON_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ON
    #define AMD_CPM_PCIE_GPP0_NAME_ON               AMD_CPM_PCIE_GPP0_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ON
    #define AMD_CPM_PCIE_GPP1_NAME_ON               AMD_CPM_PCIE_GPP1_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ON
    #define AMD_CPM_PCIE_GPP2_NAME_ON               AMD_CPM_PCIE_GPP2_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ON
    #define AMD_CPM_PCIE_GPP3_NAME_ON               AMD_CPM_PCIE_GPP3_NAME_1
  #endif

  CPU_REVISION_ITEM CpuRevisionTableON[] = {
    { 0x0FFF0F00, 0x00510F00, CPM_CPU_REVISION_ID_ON, CPM_PCIE_REVISION_ID_ON},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameON[] = {
    {CPM_PCIE_REVISION_ID_ON, 4, 0, 0x20, AMD_CPM_PCIE_GPP0_NAME_ON},
    {CPM_PCIE_REVISION_ID_ON, 5, 0, 0x21, AMD_CPM_PCIE_GPP1_NAME_ON},
    {CPM_PCIE_REVISION_ID_ON, 6, 0, 0x22, AMD_CPM_PCIE_GPP2_NAME_ON},
    {CPM_PCIE_REVISION_ID_ON, 7, 0, 0x23, AMD_CPM_PCIE_GPP3_NAME_ON},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_ON           CpuRevisionTableON,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ON          PcieBridgeNameON,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ON
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ON
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_TN_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_TN
    #define AMD_CPM_PCIE_GFX0_NAME_TN               AMD_CPM_PCIE_GFX0_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_TN
    #define AMD_CPM_PCIE_GFX1_NAME_TN               AMD_CPM_PCIE_GFX1_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_TN
    #define AMD_CPM_PCIE_GPP0_NAME_TN               AMD_CPM_PCIE_GPP0_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_TN
    #define AMD_CPM_PCIE_GPP1_NAME_TN               AMD_CPM_PCIE_GPP1_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_TN
    #define AMD_CPM_PCIE_GPP2_NAME_TN               AMD_CPM_PCIE_GPP2_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_TN
    #define AMD_CPM_PCIE_GPP3_NAME_TN               AMD_CPM_PCIE_GPP3_NAME_1
  #endif

  CPU_REVISION_ITEM CpuRevisionTableTN[] = {
    { 0x0FFF0F00, 0x00610F00, CPM_CPU_REVISION_ID_TN, CPM_PCIE_REVISION_ID_TN},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameTN[] = {
    {CPM_PCIE_REVISION_ID_TN, 2, 0, 0x10, AMD_CPM_PCIE_GFX0_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 3, 0, 0x11, AMD_CPM_PCIE_GFX1_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 4, 0, 0x20, AMD_CPM_PCIE_GPP0_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 5, 0, 0x21, AMD_CPM_PCIE_GPP1_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 6, 0, 0x22, AMD_CPM_PCIE_GPP2_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 7, 0, 0x23, AMD_CPM_PCIE_GPP3_NAME_TN},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_TN           CpuRevisionTableTN,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_TN          PcieBridgeNameTN,
#else
  #define OPTION_CPM_CPU_REVISION_ID_TN
  #define OPTION_CPM_PCIE_BRIDGE_NAME_TN
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_KV_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_KV
    #define AMD_CPM_PCIE_GFX0_NAME_KV               AMD_CPM_PCIE_GFX0_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_KV
    #define AMD_CPM_PCIE_GFX1_NAME_KV               AMD_CPM_PCIE_GFX1_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_KV
    #define AMD_CPM_PCIE_GPP0_NAME_KV               AMD_CPM_PCIE_GPP0_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_KV
    #define AMD_CPM_PCIE_GPP1_NAME_KV               AMD_CPM_PCIE_GPP1_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_KV
    #define AMD_CPM_PCIE_GPP2_NAME_KV               AMD_CPM_PCIE_GPP2_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_KV
    #define AMD_CPM_PCIE_GPP3_NAME_KV               AMD_CPM_PCIE_GPP3_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_KV
    #define AMD_CPM_PCIE_GPP4_NAME_KV               AMD_CPM_PCIE_GPP4_NAME_2
  #endif

  CPU_REVISION_ITEM CpuRevisionTableKV[] = {
    { 0x0FFF0F00, 0x00630F00, CPM_CPU_REVISION_ID_KV, CPM_PCIE_REVISION_ID_KV},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameKV[] = {
    {CPM_PCIE_REVISION_ID_KV, 2, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 2, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_KV},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_KV           CpuRevisionTableKV,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KV          PcieBridgeNameKV,
#else
  #define OPTION_CPM_CPU_REVISION_ID_KV
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KV
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_CZ_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_CZ
    #define AMD_CPM_PCIE_GFX0_NAME_CZ               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_CZ
    #define AMD_CPM_PCIE_GFX1_NAME_CZ               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_CZ
    #define AMD_CPM_PCIE_GFX2_NAME_CZ               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_CZ
    #define AMD_CPM_PCIE_GFX3_NAME_CZ               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_CZ
    #define AMD_CPM_PCIE_GFX4_NAME_CZ               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_CZ
    #define AMD_CPM_PCIE_GPP0_NAME_CZ               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_CZ
    #define AMD_CPM_PCIE_GPP1_NAME_CZ               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_CZ
    #define AMD_CPM_PCIE_GPP2_NAME_CZ               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_CZ
    #define AMD_CPM_PCIE_GPP3_NAME_CZ               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_CZ
    #define AMD_CPM_PCIE_GPP4_NAME_CZ               AMD_CPM_PCIE_GPP4_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableCZ[] = {
    { 0x0FFF0F00, 0x00660F00, CPM_CPU_REVISION_ID_CZ, CPM_PCIE_REVISION_ID_CZ},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameCZ[] = {
    {CPM_PCIE_REVISION_ID_CZ, 3, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_CZ},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_CZ           CpuRevisionTableCZ,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_CZ          PcieBridgeNameCZ,
#else
  #define OPTION_CPM_CPU_REVISION_ID_CZ
  #define OPTION_CPM_PCIE_BRIDGE_NAME_CZ
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x16_KB_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_KB
    #define AMD_CPM_PCIE_GPP0_NAME_KB               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_KB
    #define AMD_CPM_PCIE_GPP1_NAME_KB               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_KB
    #define AMD_CPM_PCIE_GPP2_NAME_KB               AMD_CPM_PCIE_GPP2_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_KB
    #define AMD_CPM_PCIE_GPP3_NAME_KB               AMD_CPM_PCIE_GPP3_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_KB
    #define AMD_CPM_PCIE_GFX_NAME_KB                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableKB[] = {
    { 0x0FFF0F00, 0x00700F00, CPM_CPU_REVISION_ID_KB, CPM_PCIE_REVISION_ID_KB},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameKB[] = {
    {CPM_PCIE_REVISION_ID_KB, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 4, 0x22, AMD_CPM_PCIE_GPP2_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 5, 0x23, AMD_CPM_PCIE_GPP3_NAME_KB},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_KB           CpuRevisionTableKB,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KB          PcieBridgeNameKB,
#else
  #define OPTION_CPM_CPU_REVISION_ID_KB
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KB
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x16_ML_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ML
    #define AMD_CPM_PCIE_GPP0_NAME_ML               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ML
    #define AMD_CPM_PCIE_GPP1_NAME_ML               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ML
    #define AMD_CPM_PCIE_GPP2_NAME_ML               AMD_CPM_PCIE_GPP2_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ML
    #define AMD_CPM_PCIE_GPP3_NAME_ML               AMD_CPM_PCIE_GPP3_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_ML
    #define AMD_CPM_PCIE_GFX_NAME_ML                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableML[] = {
    { 0x0FFF0F00, 0x00730F00, CPM_CPU_REVISION_ID_ML, CPM_PCIE_REVISION_ID_ML},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameML[] = {
    {CPM_PCIE_REVISION_ID_ML, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 4, 0x22, AMD_CPM_PCIE_GPP2_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 5, 0x23, AMD_CPM_PCIE_GPP3_NAME_ML},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_ML           CpuRevisionTableML,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ML          PcieBridgeNameML,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ML
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ML
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x16_NL_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_NL
    #define AMD_CPM_PCIE_GPP0_NAME_NL               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_NL
    #define AMD_CPM_PCIE_GPP1_NAME_NL               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_NL
    #define AMD_CPM_PCIE_GFX_NAME_NL                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableNL[] = {
    { 0x0FFF0F00, 0x00760F00, CPM_CPU_REVISION_ID_NL, CPM_PCIE_REVISION_ID_NL},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameNL[] = {
    {CPM_PCIE_REVISION_ID_NL, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_NL},
    {CPM_PCIE_REVISION_ID_NL, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_NL},
    {CPM_PCIE_REVISION_ID_NL, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_NL},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_NL           CpuRevisionTableNL,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_NL          PcieBridgeNameNL,
#else
  #define OPTION_CPM_CPU_REVISION_ID_NL
  #define OPTION_CPM_PCIE_BRIDGE_NAME_NL
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x21_AM_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_AM
    #define AMD_CPM_PCIE_GPP0_NAME_AM               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_AM
    #define AMD_CPM_PCIE_GPP1_NAME_AM               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_AM
    #define AMD_CPM_PCIE_GFX_NAME_AM                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableAM[] = {
    { 0x0FFF0F00, 0x01200F00, CPM_CPU_REVISION_ID_AM, CPM_PCIE_REVISION_ID_AM},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameAM[] = {
    {CPM_PCIE_REVISION_ID_AM, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_AM},
    {CPM_PCIE_REVISION_ID_AM, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_AM},
    {CPM_PCIE_REVISION_ID_AM, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_AM},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_AM           CpuRevisionTableAM,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_AM          PcieBridgeNameAM,
#else
  #define OPTION_CPM_CPU_REVISION_ID_AM
  #define OPTION_CPM_PCIE_BRIDGE_NAME_AM
#endif

#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_ST_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_ST
    #define AMD_CPM_PCIE_GFX0_NAME_ST               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_ST
    #define AMD_CPM_PCIE_GFX1_NAME_ST               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_ST
    #define AMD_CPM_PCIE_GFX2_NAME_ST               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_ST
    #define AMD_CPM_PCIE_GFX3_NAME_ST               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_ST
    #define AMD_CPM_PCIE_GFX4_NAME_ST               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ST
    #define AMD_CPM_PCIE_GPP0_NAME_ST               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ST
    #define AMD_CPM_PCIE_GPP1_NAME_ST               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ST
    #define AMD_CPM_PCIE_GPP2_NAME_ST               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ST
    #define AMD_CPM_PCIE_GPP3_NAME_ST               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_ST
    #define AMD_CPM_PCIE_GPP4_NAME_ST               AMD_CPM_PCIE_GPP4_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableST[] = {
    { 0x0FFF0F00, 0x00670F00, CPM_CPU_REVISION_ID_ST, CPM_PCIE_REVISION_ID_ST},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameST[] = {
    {CPM_PCIE_REVISION_ID_ST, 3, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_ST},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_ST           CpuRevisionTableST,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ST          PcieBridgeNameST,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ST
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ST
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_BR_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_BR
    #define AMD_CPM_PCIE_GFX0_NAME_BR               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_BR
    #define AMD_CPM_PCIE_GFX1_NAME_BR               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_BR
    #define AMD_CPM_PCIE_GFX2_NAME_BR               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_BR
    #define AMD_CPM_PCIE_GFX3_NAME_BR               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_BR
    #define AMD_CPM_PCIE_GFX4_NAME_BR               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_BR
    #define AMD_CPM_PCIE_GPP0_NAME_BR               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_BR
    #define AMD_CPM_PCIE_GPP1_NAME_BR               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_BR
    #define AMD_CPM_PCIE_GPP2_NAME_BR               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_BR
    #define AMD_CPM_PCIE_GPP3_NAME_BR               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_BR
    #define AMD_CPM_PCIE_GPP4_NAME_BR               AMD_CPM_PCIE_GPP4_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableBR[] = {
    { 0x0FFF0F00, 0x00660F50, CPM_CPU_REVISION_ID_BR, CPM_PCIE_REVISION_ID_BR},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameBR[] = {
    {CPM_PCIE_REVISION_ID_BR, 3, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_BR},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_BR           CpuRevisionTableBR,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_BR          PcieBridgeNameBR,
#else
  #define OPTION_CPM_CPU_REVISION_ID_BR
  #define OPTION_CPM_PCIE_BRIDGE_NAME_BR
#endif
//PS-473+ --->
#ifdef HYGON_CPM_CPU_FAMILY_0x18_SL1_SUPPORT
  #ifndef HYGON_CPM_PCIE_GFX0_NAME_ZP
    #define HYGON_CPM_PCIE_GFX0_NAME_ZP               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX1_NAME_ZP
    #define HYGON_CPM_PCIE_GFX1_NAME_ZP               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX2_NAME_ZP
    #define HYGON_CPM_PCIE_GFX2_NAME_ZP               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX3_NAME_ZP
    #define HYGON_CPM_PCIE_GFX3_NAME_ZP               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX4_NAME_ZP
    #define HYGON_CPM_PCIE_GFX4_NAME_ZP               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX5_NAME_ZP
    #define HYGON_CPM_PCIE_GFX5_NAME_ZP               AMD_CPM_PCIE_GFX5_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX6_NAME_ZP
    #define HYGON_CPM_PCIE_GFX6_NAME_ZP               AMD_CPM_PCIE_GFX6_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GFX7_NAME_ZP
    #define HYGON_CPM_PCIE_GFX7_NAME_ZP               AMD_CPM_PCIE_GFX7_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP0_NAME_ZP
    #define HYGON_CPM_PCIE_GPP0_NAME_ZP               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP1_NAME_ZP
    #define HYGON_CPM_PCIE_GPP1_NAME_ZP               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP2_NAME_ZP
    #define HYGON_CPM_PCIE_GPP2_NAME_ZP               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP3_NAME_ZP
    #define HYGON_CPM_PCIE_GPP3_NAME_ZP               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP4_NAME_ZP
    #define HYGON_CPM_PCIE_GPP4_NAME_ZP               AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP5_NAME_ZP
    #define HYGON_CPM_PCIE_GPP5_NAME_ZP               AMD_CPM_PCIE_GPP5_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP6_NAME_ZP
    #define HYGON_CPM_PCIE_GPP6_NAME_ZP               AMD_CPM_PCIE_GPP6_NAME_4
  #endif
  #ifndef HYGON_CPM_PCIE_GPP7_NAME_ZP
    #define HYGON_CPM_PCIE_GPP7_NAME_ZP               AMD_CPM_PCIE_GPP7_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableHygonZP[] = {
    { 0x0FF00F00, 0x00900F00, CPM_CPU_REVISION_ID_HYGON_ZP, CPM_PCIE_REVISION_ID_HYGON_ZP},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameHygonZP[] = {
    {CPM_PCIE_REVISION_ID_ZP, 1, 1, 0x10, HYGON_CPM_PCIE_GFX0_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 2, 0x11, HYGON_CPM_PCIE_GFX1_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 3, 0x12, HYGON_CPM_PCIE_GFX2_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 4, 0x13, HYGON_CPM_PCIE_GFX3_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 5, 0x14, HYGON_CPM_PCIE_GFX4_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 6, 0x15, HYGON_CPM_PCIE_GFX5_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 7, 0x16, HYGON_CPM_PCIE_GFX6_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 2, 1, 0x17, HYGON_CPM_PCIE_GFX7_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 1, 0x20, HYGON_CPM_PCIE_GPP0_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 2, 0x21, HYGON_CPM_PCIE_GPP1_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 3, 0x22, HYGON_CPM_PCIE_GPP2_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 4, 0x23, HYGON_CPM_PCIE_GPP3_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 5, 0x24, HYGON_CPM_PCIE_GPP4_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 6, 0x25, HYGON_CPM_PCIE_GPP5_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 7, 0x26, HYGON_CPM_PCIE_GPP6_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 4, 1, 0x27, HYGON_CPM_PCIE_GPP7_NAME_ZP},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_HYGON_ZP           CpuRevisionTableHygonZP,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_HYGON_ZP          PcieBridgeNameHygonZP,
#else
  #define OPTION_CPM_CPU_REVISION_ID_HYGON_ZP
  #define OPTION_CPM_PCIE_BRIDGE_NAME_HYGON_ZP
#endif
// <--- PS-473+
  
#ifdef AMD_CPM_CPU_FAMILY_0x17_ZP_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_ZP
    #define AMD_CPM_PCIE_GFX0_NAME_ZP               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_ZP
    #define AMD_CPM_PCIE_GFX1_NAME_ZP               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_ZP
    #define AMD_CPM_PCIE_GFX2_NAME_ZP               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_ZP
    #define AMD_CPM_PCIE_GFX3_NAME_ZP               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_ZP
    #define AMD_CPM_PCIE_GFX4_NAME_ZP               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX5_NAME_ZP
    #define AMD_CPM_PCIE_GFX5_NAME_ZP               AMD_CPM_PCIE_GFX5_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX6_NAME_ZP
    #define AMD_CPM_PCIE_GFX6_NAME_ZP               AMD_CPM_PCIE_GFX6_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX7_NAME_ZP
    #define AMD_CPM_PCIE_GFX7_NAME_ZP               AMD_CPM_PCIE_GFX7_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ZP
    #define AMD_CPM_PCIE_GPP0_NAME_ZP               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ZP
    #define AMD_CPM_PCIE_GPP1_NAME_ZP               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ZP
    #define AMD_CPM_PCIE_GPP2_NAME_ZP               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ZP
    #define AMD_CPM_PCIE_GPP3_NAME_ZP               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_ZP
    #define AMD_CPM_PCIE_GPP4_NAME_ZP               AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP5_NAME_ZP
    #define AMD_CPM_PCIE_GPP5_NAME_ZP               AMD_CPM_PCIE_GPP5_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP6_NAME_ZP
    #define AMD_CPM_PCIE_GPP6_NAME_ZP               AMD_CPM_PCIE_GPP6_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP7_NAME_ZP
    #define AMD_CPM_PCIE_GPP7_NAME_ZP               AMD_CPM_PCIE_GPP7_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableZP[] = {
    { 0x0FFF0F00, 0x00800F00, CPM_CPU_REVISION_ID_ZP, CPM_PCIE_REVISION_ID_ZP},
    0
  };

  PCIE_BRIDGE_NAME PcieBridgeNameZP[] = {
    {CPM_PCIE_REVISION_ID_ZP, 1, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 6, 0x15, AMD_CPM_PCIE_GFX5_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 7, 0x16, AMD_CPM_PCIE_GFX6_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 2, 1, 0x17, AMD_CPM_PCIE_GFX7_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 6, 0x25, AMD_CPM_PCIE_GPP5_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 7, 0x26, AMD_CPM_PCIE_GPP6_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 4, 1, 0x27, AMD_CPM_PCIE_GPP7_NAME_ZP},
    0
  };

  #define OPTION_CPM_CPU_REVISION_ID_ZP           CpuRevisionTableZP,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ZP          PcieBridgeNameZP,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ZP
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ZP
#endif
