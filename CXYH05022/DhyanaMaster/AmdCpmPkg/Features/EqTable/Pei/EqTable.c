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
#include <AmdCpmPei.h>
#include <Library/DebugLib.h>
#include <Ppi/EqTablePpi.h>
#include <Library/Ppi/AmdCpmTablePpi/AmdCpmTablePpi.h>
#include "EqTable.h"

static EFI_PEI_NOTIFY_DESCRIPTOR PeiNotifyList[] = {
  { EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gAmdCpmTablePpiGuid, EqTableInitPei }
};

STATIC EFI_PEI_EQ_TABLE_PPI mEqTablePpi = {
  EQ_TABLE_PPI_REVISION,
  CoreGetCfgTable,
  CoreDumpEqCfgTable,
  CorePreprocessEqCfgTable
};

STATIC EFI_PEI_PPI_DESCRIPTOR mEqTablePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEqTablePpiGuid,
  &mEqTablePpi 
};

VOID
CorePreprocessEqCfgTable (
  IN  CONST EFI_PEI_SERVICES      **PeiServices,
  IN  HYGON_CPM_EQ_CONFIG_TABLE*  EqCfgTable
)
{
  UINT32 i;
    
  for (i=0; (EqCfgTable->EqCfgList[i].Flag&EQ_TABLE_END) != EQ_TABLE_END; i++) {

    if (EqCfgTable->EqCfgList[i].Flag & FLAG_THROW) {
      continue; 
    }
    
    //check StartLane and EndtLane
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == pcie_eq_type) {
      //check FLAG_FORCE and FLAG_SPECIFIED
      if ((EqCfgTable->EqCfgList[i].Flag & (FLAG_SPECIFIED|FLAG_FORCE)) == 0) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Flag error!!!\n", i));
      }

      if (((EqCfgTable->EqCfgList[i].Flag & FLAG_SPECIFIED) != 0) && ((EqCfgTable->EqCfgList[i].Flag&FLAG_FORCE) != 0)) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Flag error!!!\n", i));
      }
      
      if (EqCfgTable->EqCfgList[i].StartLane > EqCfgTable->EqCfgList[i].EndLane) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, StartLane~EndLane Error!!!\n", i));
      }
    } else if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == usb_eq_type) {
      
      if ((EqCfgTable->EqCfgList[i].Type  == usb3_gen1_tx_eq_level) && (EqCfgTable->EqCfgList[i].UsbCfgValue >= max_supported_level)) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, UsbCfgValue error!!!\n", i));
      }
      
      if (EqCfgTable->EqCfgList[i].Die  >= 2) {
        EqCfgTable->EqCfgList[i].Flag |= FLAG_THROW;
        DEBUG((EFI_D_ERROR, "Table Bad: Index=%x, Die number error!!!\n", i));
      }      
    }
  }
}

VOID
CoreDumpEqCfgTable (
    IN  CONST EFI_PEI_SERVICES     **PeiServices,
    IN  HYGON_CPM_EQ_CONFIG_TABLE  *EqCfgTable
)
{
  UINT32    i, j;

  if (!DebugPrintEnabled()) return;

  DEBUG((EFI_D_INFO, "EqTable_PCIE: {Flag, Type, CpuGen, Die, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, CfgValue[16]}\n"));
  DEBUG((EFI_D_INFO, "EqTable_USB:  {Flag, Type, CpuGen, Die, UsbPort, UsbCfgValue}\n"));
  for (i=0; (EqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) != EQ_TABLE_END; i++) {
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == pcie_eq_type) {
      DEBUG((EFI_D_INFO, "PCIE_EQ { 0x%x, ", EqCfgTable->EqCfgList[i].Flag));
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].Type));
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].CpuGen)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].Die)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].VidDid)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].SubSysVidDid)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].StartLane)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].EndLane)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].PerPort));
      DEBUG((EFI_D_INFO, "{ "));
      for (j=0; j < 16; j++) {
        DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].CfgValue[j]));    
      }
      DEBUG((EFI_D_INFO, "}}\n"));
    }
    if ((EqCfgTable->EqCfgList[i].Type & EQ_TYPE_MASK) == usb_eq_type) {
      DEBUG((EFI_D_INFO, "USB_EQ { 0x%x, ", EqCfgTable->EqCfgList[i].Flag));
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].Type));
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].CpuGen)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].Die)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].UsbPort)); 
      DEBUG((EFI_D_INFO, "0x%x, ", EqCfgTable->EqCfgList[i].UsbCfgValue)); 
      DEBUG((EFI_D_INFO, "}\n"));
    }
  }
}

EFI_STATUS
CoreGetCfgTable (
    IN  CONST EFI_PEI_SERVICES     **PeiServices,
    OUT HYGON_CPM_EQ_CONFIG_TABLE  **EqCfgTable
)
{
  EFI_STATUS                  Status;
  UINT32                      i;
  HYGON_CPM_EQ_CONFIG_TABLE   *pEqCfgTable;
  AMD_CPM_TABLE_PPI           *CpmTablePpiPtr;

  Status = (*PeiServices)->LocatePpi (
                            (CPM_PEI_SERVICES**)PeiServices,
                            &gAmdCpmTablePpiGuid,
                            0,
                            NULL,
                            (VOID**)&CpmTablePpiPtr
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  pEqCfgTable = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_EQ_CONFIG);
  if (pEqCfgTable == NULL) {
    return EFI_NOT_FOUND;
  }

  for (i=0; i < MAX_EQ_CFG_ITEM; i++) {
    if ((pEqCfgTable->EqCfgList[i].Flag & EQ_TABLE_END) == EQ_TABLE_END) {
      *EqCfgTable = pEqCfgTable;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
EqTableInitPei (
    IN EFI_PEI_SERVICES             **PeiServices,
    IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
    IN VOID                         *InvokePpi
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Install EqTable Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mEqTablePpiList
                           );

  return Status;
}

/**
 * Entry point of the EQ Table PEIM driver
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
EqTablePeiEntryPoint (
  IN        CPM_PEI_FILE_HANDLE   FileHandle,
  IN        CPM_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS                      Status;

  Status = (*PeiServices)->NotifyPpi(PeiServices, PeiNotifyList);

  return EFI_SUCCESS;
}
