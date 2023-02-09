//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file PlatformPeiSystemInfoDIMM.c

    Gathers DIMM information
*/

#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include "PlatformPeiSystemInformation.h"
#include "Pei.h"


#define MSR_SYS_CFG     0x0C0010010
#define MSR_TOP_MEM     0x0C001001A
#define MSR_TOP_MEM2    0x0C001001D

#define UMC0_CH_REG_BASE            (0x00050000)
#define UMC1_CH_REG_BASE            (0x00150000)
#define IOHC_NB_SMN_INDEX_2_BIOS    (0x00B8)
#define IOHC_NB_SMN_DATA_2_BIOS     (0x00BC)


/**
    Reads SMN register
    
    @param  IohcBus - Root bus number for Die to read from
    @param  SmnAddress - SMN address
    @param  Value - pointer to output value
    
*/

VOID
SmnRead2 (
  IN    UINT32  IohcBus,
  IN    UINT32  SmnAddress,
  IN    volatile UINT32  *Value
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress;
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_INDEX_2_BIOS;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_DATA_2_BIOS;
  *Value = PciRead32 (PciAddress);
}

/**
    Gets the DIMM's data and output to serial/video

    @param  **PeiServices - pointer to the PEI services.
    @param  *This - pointer to PPI AMI_PEI_TEXT_OUT

    @retval EFI_SUCCESS
    
*/

EFI_STATUS
InitializeDimmData (
    IN  CONST EFI_PEI_SERVICES  **PeiServices,
	IN  AMI_PEI_TEXT_OUT	    *This
)
{
    EFI_STATUS  Status;
    UINT64      Tom1, Tom2, SystemRam;
    UINT8       DieCount;
    UINT32      Die, DieBus;
    UINT32      DramConfigValue;
    UINT32      MemClkFreqReg, MaxMemClkFreqReg, MaxMemSpeed;
    CHAR8       TextString[160];
    
    DEBUG((EFI_D_INFO,"[PEI System Information] InitializeDimmData \n"));

    // Get Total Memory Size
    if (AsmReadMsr64(MSR_SYS_CFG) & BIT21) {
        Tom2 = AsmReadMsr64(MSR_TOP_MEM2);
        Tom2 = RShiftU64(Tom2, 20) - 0x1000;
    } else {
        Tom2 = 0;
    }
        
    Tom1 = AsmReadMsr64(MSR_TOP_MEM);
    Tom1 = RShiftU64(Tom1, 20);
    
    SystemRam = RShiftU64((Tom1 + Tom2), 10);
    
    // Get Max Memory Speed
    DieCount = PciRead8((0x18 << 15) + (0x01 << 12) + 0x204);
    MaxMemClkFreqReg = 0;    
    for(Die=0; Die < DieCount; Die++){
        // Get DieBus number
        DieBus = PciRead8(((0x18 + Die)<< 15) + (0x0 << 12) + 0x84 );
        // Get DramConfig register value
        SmnRead2(DieBus, UMC0_CH_REG_BASE + 0x200, &DramConfigValue);
        MemClkFreqReg = DramConfigValue & 0xFF;
        if (MemClkFreqReg > MaxMemClkFreqReg)
            MaxMemClkFreqReg = MemClkFreqReg;
        SmnRead2(DieBus, UMC1_CH_REG_BASE + 0x200, &DramConfigValue);
        MemClkFreqReg = DramConfigValue & 0xFF;
        if (MemClkFreqReg > MaxMemClkFreqReg)
            MaxMemClkFreqReg = MemClkFreqReg;
    }
    
    switch (MaxMemClkFreqReg) {
    case 0x14:
        // 667MHz
        MaxMemSpeed = 1333;
        break;
    case 0x18:
        // 800MHz
        MaxMemSpeed = 1600;
        break;
    case 0x1C:
        // 933MHz
        MaxMemSpeed = 1866;
        break;
    case 0x20:
        // 1067MHz
        MaxMemSpeed = 2133;
        break;
    case 0x24:
        // 1200MHz
        MaxMemSpeed = 2400;
        break;
    case 0x28:
        // 1333MHz
        MaxMemSpeed = 2667;
        break;
    case 0x2C:
        // 1467MHz
        MaxMemSpeed = 2933;
        break;
    case 0x30:
        // 1600MHz
        MaxMemSpeed = 3200;
        break;
    default:
        // 1200MHz
        MaxMemSpeed = 2400;
        break;
    }
    
    // Print Info
    AsciiSPrint (TextString, 160, " Memory Info: Memory Size: %dGB", SystemRam);
    Status = This->WriteString(This, 0, TextString);
    AsciiSPrint (TextString, 160, " Memory Speed: %d MT/s\n", MaxMemSpeed);
    Status = This->WriteString(This, 0, TextString);
    Status = This->SetAttribute(This, 0x7, 0, 0);
      
    return EFI_SUCCESS;
}

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2018, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**         5555 Oakbrook Pkwy, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
