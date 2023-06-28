#ifndef _AUTO_DXIO_CONFIG_H__
#define _AUTO_DXIO_CONFIG_H__

#include <Efi.h>
#include <Pei.h>
#include <Token.h>
#include <AmiLib.h>
#include <AmiPeiLib.h>
#include <Setup.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Library\Ppi\AmdCpmTablePpi\AmdCpmTablePpi.h>

#include "AutoDxioConfig.h"

typedef VOID (AUTO_DXIO_FUNCTION)(IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr);

//----------------------------------------------------------------------------
// Variable and External Declaration(s)
//----------------------------------------------------------------------------

EFI_STATUS PeiConfigDxioSettings (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi 
    );

// /**
// GC_TODO: Update DXIO Table
// @param AMD_CPM_TABLE_PPI  *CpmTablePpiPtr

// @retval VOID
// **/
// void UpdateDxioSetting(
//   IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
// );

// /**
// GC_TODO: DxioDump
// @param  CpmTablePpiPtr - GC_TODO: DXIO PPI Pointer

// @retval NULL   - GC_TODO: add retval description
// **/
// void DxioDump(
//   IN  AMD_CPM_TABLE_PPI  *CpmTablePpiPtr
// );
#endif