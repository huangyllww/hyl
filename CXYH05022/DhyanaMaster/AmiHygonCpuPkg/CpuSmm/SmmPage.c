//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************

//**********************************************************************
// $Header: $
//
// $Revision: $
//
// $Date: $
//**********************************************************************

//<AMI_FHDR_START>
//---------------------------------------------------------------------------
//
// Name:		SmmPage.c
//
// Description:	Smm Page Table code
//
//---------------------------------------------------------------------------
//<AMI_FHDR_END>
#include <Token.h>
#include "CpuSmm.h"

extern  CPU_SMM_PRIVATE_DATA    gCpuSmmInt;

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: SmmPageTableInit
//
// Description:
//  Allocates memory and creates identity mapping
//
// Input:   
//  VOID
//
// Output:
//   EFI_STATUS
//      EFI_OUT_OF_RESOURCES - can't allocate memory for Page table
//      EFI_SUCCESS - Page table created
//
// Modified:    
//
// Referrals: 
//
// Notes:      
//  To save space, only memory which uses SMM_PAGING_MAX_ADDRESS_LINES mapped
//             
//---------------------------------------------------------------------------
// <AMI_PHDR_END>

EFI_STATUS
EFIAPI
SmmPageTableInit(
  VOID
)
{
    UINT32  NumPages;
    UINT8   NumAddrLines = (UINT8)gEfiCpuHob->SizeOfMemorySpace;
    VOID*   SmmPageTableStart;
    
    // !TODO - may not be enough SMM_PAGING_MAX_ADDRESS_LINES
    if (NumAddrLines > SMM_PAGING_MAX_ADDRESS_LINES) NumAddrLines = SMM_PAGING_MAX_ADDRESS_LINES;

    NumPages = GetPageTableNumPages(NumAddrLines);
    SmmPageTableStart = AllocatePages(NumPages);
    if(!SmmPageTableStart)  return EFI_OUT_OF_RESOURCES;
    
    FillPageTable(NumAddrLines, SmmPageTableStart);
    gCpuSmmInt.SmmPageTableStart = SmmPageTableStart;
    return  EFI_SUCCESS;
}

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: GetPageDirEntry
//
// Description:
//  Returns Page Directory pointer for given address
//
// Input:   
//  IN LMODE_2MPAGE_TRANSL	*ReqPhysAddr - pointer to requested address
//  IN OUT PAGE_MAP_AND_DIRECTORY_POINTER  **PageDirEntry - pointer to entry pointer
//
// Output:
//   VOID
//
// Modified:    
//
// Referrals: 
//
// Notes:      
//             
//---------------------------------------------------------------------------
// <AMI_PHDR_END>

VOID
EFIAPI
GetPageDirEntry(
  IN LMODE_2MPAGE_TRANSL	*ReqPhysAddr,
  IN OUT PAGE_MAP_AND_DIRECTORY_POINTER  **PageDirEntry
)
{
    UINT64                          *TablePtr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *Pml4Entry;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PdpEntry;

    // Get PML4 address
    TablePtr = (UINT64*)(AsmReadCr3() & (UINT64)~0xfff);

    (UINT64*)Pml4Entry = TablePtr + ReqPhysAddr->Bits.PML4Offset * 1;
    // Get PDP entry
    TablePtr = (UINT64*)(Pml4Entry->Bits.PageTableBaseAddress << 12);
    (UINT64*)PdpEntry = TablePtr + ReqPhysAddr->Bits.PDPOffset * 1;
    // Get PD entry
    TablePtr = (UINT64*)(PdpEntry->Bits.PageTableBaseAddress << 12);
    (UINT64*)(*PageDirEntry) = TablePtr + ReqPhysAddr->Bits.PageDirOffset * 1;

}

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: GlobEnableMmioMap
//
// Description:
//  Disables/Enables memory mapping for MMIO addresses - 16Mb of ROM and 
//   FCH ACPI MMIO space
//
// Input:   
//  VOID
//
// Output:
//   VOID
//
// Modified:    
//
// Referrals: 
//
// Notes:      
//  Working with 2M pages
//             
//---------------------------------------------------------------------------
// <AMI_PHDR_END>

VOID
EFIAPI
GlobEnableMmioMap(
  IN    PAGE_STATE  PageState
)
{
    LMODE_2MPAGE_TRANSL             ReqPhysAddr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PageDirEntry;
    UINT64                          PageStart;
//#### EIP 143623
    UINT64                          PcieBase;
    
    PcieBase = AsmReadMsr64(0xC0010058);
    if((PcieBase & 1)== 1){
        PcieBase = PcieBase & (UINT64)0xFFF00000;
    }else{
        PcieBase = (UINT64)(PcdGet64 (PcdPciExpressBaseAddress));
    }
/*
    ReqPhysAddr.Uint64 = (UINT64)SB_MMIO_BASE;           // FCH MMIO

    GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
    // Set page state
    PageDirEntry->Bits.Present = PageState;
    
    // ROM region
    for(PageStart = ROM_START_ADDR; PageStart < (UINT64)ADDR_4G;){
*/
    for(PageStart = PcieBase; PageStart < (UINT64)ADDR_4G;){
        ReqPhysAddr.Uint64 = PageStart;
        GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
        // Set page state
        PageDirEntry->Bits.Present = PageState;
        PageStart += (UINT64)MMIO_PAGE_SIZE;
    }
}

// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: DisableMmioMap
//
// Description:
//  Disables memory mapping for address in CR2 - PF exception
//
// Input:   
//  VOID
//
// Output:
//   VOID
//
// Modified:    
//
// Referrals: 
//
// Notes:      
//             
//---------------------------------------------------------------------------
// <AMI_PHDR_END>

VOID
EFIAPI
DisableMmioMap(
    VOID
)
{
    LMODE_2MPAGE_TRANSL             ReqPhysAddr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PageDirEntry;
    
    ReqPhysAddr.Uint64 = (UINT64)AsmReadCr2();

    GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
    // Disable page
    PageDirEntry->Bits.Present = DisablePage;
}


// <AMI_PHDR_START>
//---------------------------------------------------------------------------
//
// Name: EnableMmioMap
//
// Description:
//  Enables memory mapping for address in CR2 - PF exception
//
// Input:   
//  VOID
//
// Output:
//   VOID
//
// Modified:    
//
// Referrals: 
//
// Notes:      
//             
//---------------------------------------------------------------------------
// <AMI_PHDR_END>


VOID
EFIAPI
EnableMmioMap(
    VOID
)
{
    LMODE_2MPAGE_TRANSL             ReqPhysAddr;
    PAGE_MAP_AND_DIRECTORY_POINTER  *PageDirEntry;
    
    ReqPhysAddr.Uint64 = (UINT64)AsmReadCr2();

    GetPageDirEntry(&ReqPhysAddr, &PageDirEntry);
    // Enable page
    PageDirEntry->Bits.Present = EnablePage;
}

//*************************************************************************
//*************************************************************************
//**                                                                     **
//**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
//**                                                                     **
//**                       All Rights Reserved.                          **
//**                                                                     **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
//**                                                                     **
//**                       Phone: (770)-246-8600                         **
//**                                                                     **
//*************************************************************************
//*************************************************************************
