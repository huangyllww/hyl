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
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/BaseSocLogicalIdXlatLib.h>
#include "cpuRegisters.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_BASECORELOGICALIDX86LIB_BASECORELOGICALIDX86LIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the raw family / model / stepping for the executing core
 *
 * @returns       Raw CPU ID for the executing core
 */
UINT32
BaseGetRawIdOnExecutingCore (
  VOID
  )
{
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );

  return CpuId.EAX_Reg;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the FamilyId for the executing core
 *
 * @returns       FamilyId for the executing core
 */
UINT32
GetFamilyId (
  VOID
  )
{
  UINT32 RawId;
  UINT32 BaseFamilyId;
  UINT32 ExtFamilyId;

  RawId = BaseGetRawIdOnExecutingCore();
  BaseFamilyId = (RawId & CPUID_BASE_FAMILY_MASK) >> CPUID_BASE_FAMILY_OFFSET;
  ExtFamilyId  = (RawId & CPUID_EXT_FAMILY_MASK)  >> CPUID_EXT_FAMILY_OFFSET;

  return (UINT32)(BaseFamilyId + ExtFamilyId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the ModelId for the executing core
 *
 * @returns       ModelId for the executing core
 */
UINT32
GetModelId (
  VOID
  )
{
    UINT32 RawId;
    UINT32 BaseModelId;
    UINT32 ExtModelId;

    RawId = BaseGetRawIdOnExecutingCore();
    BaseModelId = (RawId & CPUID_BASE_MODEL_MASK)  >> CPUID_BASE_MODEL_OFFSET;
    ExtModelId = (RawId & CPUID_EXT_MODEL_MASK)   >> CPUID_EXT_MODEL_OFFSET;
    
    return (UINT32)(BaseModelId + (ExtModelId << 4));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the SteppingId for the executing core
 *
 * @returns       SteppingId for the executing core
 */
UINT32
GetSteppingId (
  VOID
  )
{
    UINT32 RawId;
    UINT32 SteppingId;

    RawId = BaseGetRawIdOnExecutingCore();
    SteppingId = RawId & CPUID_STEPPING_MASK;

    return SteppingId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the PkgType for the executing core
 *
 * @returns       PkgType for the executing core
 */
UINT32
GetPkgType (
  VOID
  )
{
    CPUID_DATA CpuId;
    UINT32     PkgType;

    AsmCpuid (
        AMD_CPUID_FMF,
        &(CpuId.EAX_Reg),
        &(CpuId.EBX_Reg),
        &(CpuId.ECX_Reg),
        &(CpuId.EDX_Reg)
        );
    
    PkgType = (CpuId.EBX_Reg & CPUID_PKG_TYPE_MASK) >> CPUID_PKG_TYPE_OFFSET;
    
    return PkgType;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the logical family / revision for the executing core
 *
 * @param[out]    LogicalId   Executing core's logical faily and logical revision
 *
 * @retval        TRUE        Executing core's logical ID was successfully found
 * @retval        FALSE       The executing core's family is unknown
 */
BOOLEAN
BaseGetLogicalIdOnExecutingCore (
     OUT SOC_LOGICAL_ID  *LogicalId
  )
{
  return (BaseSocConvertRawToLogical (BaseGetRawIdOnExecutingCore (), LogicalId));
}
