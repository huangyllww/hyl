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

/** @file
    Gathers CPU Information
**/

#include <Library/PeiServicesLib.h>
#include <Library/BaseLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include "PlatformPeiSystemInformation.h"
#include "Pei.h"

#include <CcxRegistersZp.h>


/**
    Outputs CPU information to early text out.

    @param  **PeiServices - pointer to the PEI services.
    @param  *This - pointer to PPI AMI_PEI_TEXT_OUT

    @retval VOID
**/

VOID
InitializeCpuData (
    IN  CONST EFI_PEI_SERVICES  **PeiServices,
    IN  AMI_PEI_TEXT_OUT    *This
)
{
    EFI_STATUS  Status;
    UINT32      EAX_Reg, EBX_Reg, ECX_Reg ,EDX_Reg, RegValue, CurFID, CurDID, CurrPstate;
    INT32       Index;
    UINTN       NumberOfProcessors, TotalNumberOfDie, TscFrequency;
    UINT8       ExtFamily, BaseFamily, ExtModel, BaseModel, CoreCount, Stepping, PkgType;
    CHAR8       PkgName[10];
    CHAR8       TextString[160];
    CHAR8       CPUNameString[48];
    UINT8       msr_count, dword_count;
    UINT32      ProcessorNameMsr=0x80000002, DwordValue=0;
    BOOLEAN     IsSmtEnabled;
    AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI          *FabricTopologyServicesPpi;
    
    // Get Number of processors and dies
    Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gAmdFabricTopologyServicesPpiGuid,
                               0,
                               NULL,
                               &FabricTopologyServicesPpi
                               );
    FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, &TotalNumberOfDie, NULL);
    
    // Get Processor info
    AsmCpuid(0x1, &EAX_Reg, &EBX_Reg, NULL, NULL);
    ExtFamily = (UINT8)((EAX_Reg >> 20) & 0xFF);
    BaseFamily = (UINT8)((EAX_Reg >> 8) & 0xF);
    ExtModel = (UINT8)((EAX_Reg >> 16) & 0xF);
    BaseModel = (UINT8)((EAX_Reg >> 4) & 0xF);
    Stepping = (UINT8)(EAX_Reg & 0xF);
    CoreCount = (UINT8)((EBX_Reg >> 16) & 0xFF);
    
    // Check if SMT enabled
    AsmCpuid(0x8000001E, NULL, &EBX_Reg, NULL, NULL);
    IsSmtEnabled = (((EBX_Reg >> 8) & 0xFF) == 0x1) ? TRUE : FALSE;
    
    // Get Package Type
    AsmCpuid(0x80000001, NULL, &EBX_Reg, NULL, NULL);
    PkgType = (EBX_Reg >> 28) & 0x0F;
    
    switch (PkgType) {
    case 2:
        // AM4
        AsciiSPrint(PkgName, sizeof (PkgName), "AM4");
        break;
    case 1:
        // SP4
        AsciiSPrint(PkgName, sizeof (PkgName), "SP4");
        break;
    case 3:
        // SP4r2
        AsciiSPrint(PkgName, sizeof (PkgName), "SP4r2");
        break;
    case 4:
        // SP3
        AsciiSPrint(PkgName, sizeof (PkgName), "SP3");
        break;
    case 7:
        // SP3r2
        AsciiSPrint(PkgName, sizeof (PkgName), "SP3r2");
        break;
        
    case 6:
        // DM1
        AsciiSPrint(PkgName, sizeof (PkgName), "DM1");
        break;
        
    default:
        AsciiSPrint(PkgName, sizeof (PkgName), "SP3");
        break;
    }

    // Get Frequency
    CurrPstate = (UINT32)(AsmReadMsr64(MSR_PSTATE_STS) & 0x7);
    RegValue = (UINT32)AsmReadMsr64(MSR_PSTATE_0 + CurrPstate);
    //Extract the FID and DID.
    CurFID = (UINT8)RegValue & 0xFF;
    CurDID = (UINT8)(RegValue >> 8) & 0x3F;
    // to avoid divide by zero
    if(CurDID == 0)   CurDID = 1;
    // Freq = (FID/DID)*200 MHz
    TscFrequency = (UINTN)DivU64x32(MultU64x64(CurFID, 200), CurDID);
    
    //Get Processor Name String
    for(msr_count=0;msr_count < 3;msr_count++)
    {
        AsmCpuid((ProcessorNameMsr+msr_count), &EAX_Reg, &EBX_Reg, &ECX_Reg, &EDX_Reg);
        for(dword_count=0;dword_count < 4;dword_count++){
            if(dword_count==0)
                DwordValue=EAX_Reg;
            else if(dword_count==1)
                DwordValue=EBX_Reg;
            else if(dword_count==2)
                DwordValue=ECX_Reg;
            else if(dword_count==3)
                DwordValue=EDX_Reg;

            CPUNameString[(msr_count*16)+(dword_count*4)+0]=DwordValue;
            CPUNameString[(msr_count*16)+(dword_count*4)+1]=DwordValue >> 8;
            CPUNameString[(msr_count*16)+(dword_count*4)+2]=DwordValue >> 16;
            CPUNameString[(msr_count*16)+(dword_count*4)+3]=DwordValue >> 24;
        }
    }
    // Remove trailing spaces from Processor Name String
    for(Index = 47; Index > 0; Index--){
        if(CPUNameString[Index]==0) continue;
        if(CPUNameString[Index]== 0x20) {
            CPUNameString[Index] = 0;
        } else {
            break;
        }
    }
    // Print info
    AsciiSPrint (TextString, 160, " CPU Info: %a %dMHz\n", CPUNameString, TscFrequency);
    Status = This->WriteString(This, 0, TextString);
    AsciiSPrint (TextString, 160, " Processor Family:%xh Model:%02xh Stepping:%c%d Package:%a\n", 
                ExtFamily + BaseFamily, (ExtModel << 4) + BaseModel, 'A'+BaseModel , Stepping, PkgName);
    Status = This->WriteString(This, 0, TextString);
    AsciiSPrint (TextString, 160, " Processors:%d  Dies:%d  Cores:%d  Threads:%d\n", 
                NumberOfProcessors,
                TotalNumberOfDie,
                IsSmtEnabled?((CoreCount / 2)* NumberOfProcessors):(CoreCount * NumberOfProcessors) ,
                (CoreCount * NumberOfProcessors)
                );
    Status = This->WriteString(This, 0, TextString);
    Status = This->SetAttribute(This, 0x7, 0, 0);
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
