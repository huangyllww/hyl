/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH ACPI lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
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
****************************************************************************
*/
#ifndef _FCH_ACPILIB_H_
#define _FCH_ACPILIB_H_
///
/// RSDP - ACPI 2.0 table RSDP
///
typedef struct _RSDP_HEADER {
  UINT64               Signature;                      ///< RSDP signature "RSD PTR"
  UINT8                CheckSum;                       ///< checksum of the first 20 bytes
  UINT8                OEMID[6];                       ///< OEM ID
  UINT8                Revision;                       ///< 0 for APCI 1.0, 2 for ACPI 2.0
  UINT32               RsdtAddress;                    ///< physical address of RSDT
  UINT32               Length;                         ///< total length of RSDP (including extended part)
  UINT64               XsdtAddress;                    ///< physical address of XSDT
  UINT8                ExtendedCheckSum;               ///< chechsum of whole table
  UINT8                Reserved[3];                    ///< Reserved
} RSDP_HEADER;

///
/// DESCRIPTION_HEADER - ACPI common table header
///
typedef struct _DESCRIPTION_HEADER {
  UINT32               Signature;                      ///< ACPI signature (4 ASCII characters)
  UINT32               Length;                         ///< Length of table, in bytes, including header
  UINT8                Revision;                       ///< ACPI Specification minor version #
  UINT8                CheckSum;                       ///< To make sum of entire table == 0
  UINT8                OemId[6];                       ///< OEM identification
  UINT8                OemTableId[8];                  ///< OEM table identification
  UINT32               OemRevision;                    ///< OEM revision number
  UINT32               CreatorId;                      ///< ASL compiler vendor ID
  UINT32               CreatorRevision;                ///< ASL compiler revision number
} DESCRIPTION_HEADER;

///
/// _AcpiRegWrite - ACPI MMIO register R/W structure
///
typedef struct _ACPI_REG_WRITE {
  UINT8        MmioBase;               /// MmioBase: Index of Fch block (For instance GPIO_BASE:0x01 SMI_BASE:0x02)
  UINT8        MmioReg;                /// MmioReg      : Register index
  UINT8        DataAndMask;            /// DataANDMask  : AND Register Data
  UINT8        DataOrMask;             /// DataOrMask   : Or Register Data
} ACPI_REG_WRITE;

VOID*     AcpiLocateTable (IN UINT32 Signature);
VOID      AcpiSetTableCheckSum (IN VOID *TablePtr);
UINT8     AcpiGetTableCheckSum (IN VOID *TablePtr);
UINT8     GetByteSum (IN VOID *DataPtr, IN UINT32 Length);

#endif