/* $NoKeywords:$ */
/**
 * @file
 *
 * ma.h
 *
 * ARDK common header file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
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
* ***************************************************************************
*
*/

#ifndef _MA_H_
#define _MA_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */


#define MAX_CS_PER_CHANNEL      8   ///< Max CS per channel
/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/** MARDK Structure*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT8    Loads;                  ///< Number of Data Loads
  UINT32   AddrTmg;                ///< Address Timing value
  UINT32   Odc;                    ///< Output Driver Compensation Value
} PSCFG_ENTRY;

/** MARDK Structure*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT8    Loads;                  ///< Number of Data Loads
  UINT32   AddrTmg;                ///< Address Timing value
  UINT32   Odc;                    ///< Output Driver Compensation Value
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_PSCFG_ENTRY;

/** MARDK Structure for RDIMMs*/
typedef struct {
  UINT16   Speed;                  ///< Dram speed in MHz
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT32   AddrTmg;                ///< Address Timing value
  UINT16   RC2RC8;                   ///< RC2 and RC8 value  //High byte: 1st pair value, Low byte: 2nd pair value
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_R_PSCFG_ENTRY;

/** MARDK Structure*/
typedef struct {
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT32   PhyRODTCSLow;          ///< Fn2_9C 180
  UINT32   PhyRODTCSHigh;         ///< Fn2_9C 181
  UINT32   PhyWODTCSLow;          ///< Fn2_9C 182
  UINT32   PhyWODTCSHigh;         ///< Fn2_9C 183
  UINT8    Dimms;                  ///< Number of Dimms
} ADV_PSCFG_ODT_ENTRY;

/** MARDK Structure for Write Levelization ODT*/
typedef struct {
  UINT16   DIMMRankType;         ///< Bitmap of Ranks   //Bit0-3:DIMM0(1:SR, 2:DR, 4:QR, 0:No Dimm, 0xF:Any), Bit4-7:DIMM1, Bit8-11:DIMM2, Bit12-16:DIMM3
  UINT8    PhyWrLvOdt[MAX_CS_PER_CHANNEL / 2];            ///< WrLvOdt (Fn2_9C_0x08[11:8]) Value for each Dimm
  UINT8    Dimms;                ///< Number of Dimms
} ADV_R_PSCFG_WL_ODT_ENTRY;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

AGESA_STATUS
MemAGetPsCfgDef (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  );

UINT16
MemAGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  );

#endif  /* _MA_H_ */