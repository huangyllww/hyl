/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Print Routines
 *
 * Contains all functions related to IDS Debug Print
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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

#ifndef _AMD_IDS_DEBUG_PRINT_LOCAL_H_
#define _AMD_IDS_DEBUG_PRINT_LOCAL_H_

/// return status for debug print
typedef enum {
  IDS_DEBUG_PRINT_SUCCESS = 0,      ///< success
  IDS_DEBUG_PRINT_BUFFER_OVERFLOW,      ///< Bufer overflow
} IDS_DEBUG_PRINT_STATUS;

typedef BOOLEAN (*PF_AMD_IDS_DEBUG_PRINT_SUPPORT) (VOID);
typedef BOOLEAN (*PF_AMD_IDS_DEBUG_PRINT_FILTER) (UINT64 *Filter);
typedef VOID (*PF_AMD_IDS_DEBUG_PRINT_PRINT) (CHAR8 *Buffer, UINTN BufferSize);
typedef VOID (*PF_AMD_IDS_DEBUG_CONSTRUCTOR) (VOID);

/// Debug print Hw layer service class
typedef struct _AMD_IDS_DEBUG_PRINT_INSTANCE {
  PF_AMD_IDS_DEBUG_PRINT_SUPPORT  support;    ///Check if support
  PF_AMD_IDS_DEBUG_CONSTRUCTOR    constructor;  ///constructor, will be called prior to driver execution
  PF_AMD_IDS_DEBUG_PRINT_FILTER  customfilter;  ///Get if any customize filters
  PF_AMD_IDS_DEBUG_PRINT_PRINT  print;  ///Print data to Hw layer
} AMD_IDS_DEBUG_PRINT_INSTANCE;

#endif //_AMD_IDS_DEBUG_PRINT_LOCAL_H_

