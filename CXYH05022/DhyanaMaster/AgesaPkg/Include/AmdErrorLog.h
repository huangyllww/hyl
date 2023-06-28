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

#ifndef _AMD_ERROR_LOG_H_
#define _AMD_ERROR_LOG_H_

#define TOTAL_ERROR_LOG_BUFFERS  128

/// The return status for all AGESA public services.
///
/// Services return the most severe status of any logged error.  Status other than SUCCESS, UNSUPPORTED, and BOUNDS_CHK
/// will have log entries with more detail.
///
typedef enum {
  AMD_SUCCESS = 0,             ///< The service completed normally. Info may be logged.
  AMD_UNSUPPORTED,             ///< The dispatcher or create struct had an unimplemented function requested.
                                 ///<  Not logged.
  AMD_BOUNDS_CHK,              ///< A dynamic parameter was out of range and the service was not provided.
                                 ///< Example, memory address not installed, heap buffer handle not found.
                                 ///< Not Logged.
  // AGESA_STATUS of greater severity (the ones below this line), always have a log entry available.
  AMD_ALERT,                   ///< An observed condition, but no loss of function.  See Log.
  AMD_WARNING,                 ///< Possible or minor loss of function.  See Log.
  AMD_ERROR,                   ///< Significant loss of function, boot may be possible.  See Log.
  AMD_CRITICAL,                ///< Continue boot only to notify user.  See Log.
  AMD_FATAL,                   ///< Halt booting.  See Log, however Fatal errors pertaining to heap problems
                                 ///< may not be able to reliably produce log errors.
  AmdStatusMax                 ///< Not a status, for limit checking.
} AMD_STATUS;

// ========================================
// CPU general events            (SubFunction = 01h)
#define CPU_EVENT_NON_CCX_BIST_FAILURE      0x08000100ul
#define CPU_EVENT_CCX_BIST_FAILURE          0x08000101ul

/// An Error Log Entry.
typedef struct {
     OUT   UINT32            ErrorClass;        ///< The severity of this error, matches AGESA_STATUS.
     OUT   UINT32            ErrorInfo;         ///< The unique error identifier, zero means "no error".
     OUT   UINT32            DataParam1;        ///< Data specific to the error.
     OUT   UINT32            DataParam2;        ///< Data specific to the error.
     OUT   UINT32            DataParam3;        ///< Data specific to the error.
     OUT   UINT32            DataParam4;        ///< Data specific to the error.
} ERROR_LOG_PARAMS;

/// An Error Log Entry.
typedef struct {
     OUT   UINT32            Count;              ///< Actual Error Log entries count.
     OUT   ERROR_LOG_PARAMS  ErrorLog_Param[TOTAL_ERROR_LOG_BUFFERS];   ///< Data specific to the Error.
} ERROR_LOG_DATA_STRUCT;

#endif  // _AMD_ERROR_LOG_H_
