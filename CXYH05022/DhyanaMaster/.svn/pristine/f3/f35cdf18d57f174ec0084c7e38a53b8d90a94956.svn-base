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

/** @file CpuSetup.c
    CPU Setup Routines

**/

#include <Setup.h>
#include <AmiDxeLib.h>
#include <Protocol\AmiCpuInfo.h>
#include <AMIHobs.h>
#include <CacheSubClass.h>
#include <CpuBoard.h>
#include "Protocol/FabricTopologyServices.h"

// CPU
#define MSR_PSTATE_LIMIT    0x0C0010061
#define MSR_PSTATE_0        0x0C0010064
//#define	HYGON_MSR_NB_CFG      0x0C001001F
#define  PCI_ADDRESS(bus, dev, func, reg) \
            ((UINT64) ( (((UINT8)bus) << 16) + (((UINT8)dev) << 11) + \
            (((UINT8)func) << 8) + ((UINT8)reg) )) & 0x00000000ffffffff

EFI_GUID gAmiCpuInfoProtocolGuid   = AMI_CPU_INFO_PROTOCOL_GUID;
EFI_HII_HANDLE gHiiHandle;

UINT64 ReadMsr(UINT32 Msr);
extern WriteMsr();
extern  CPULib_CpuID();

static EFI_GUID gSetupGuid = SETUP_GUID;

/**
    Read the PCI Register

    @param Address :(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) \
        + (((UINT32)func) << 16) + ((UINT32)reg) )
    @param ExtAccess : TRUE for extended access, FALSE is for normal access

    @retval UINT32 return value from the PCI Register.

**/
UINT32 NbReadPCI32(
    IN UINT32 Address,
    IN BOOLEAN  ExtAccess
)
{
    UINT32  Value32;
    UINT32  AddrCf8 = (1 << 31) + (Address & 0xFFFF00) + (Address & 0xFC);

    if (ExtAccess) //If extended config Access requested then add the extended config address  bit (24 to 27)
        AddrCf8 = AddrCf8 | 0x1000000;

    IoWrite32(0xCF8, AddrCf8);
    Value32 = IoRead32(0xCFC);
    return Value32;
}

/**
    Initialize CPU Setup information.

        
    @param Value Cache associativity type
      
         
    @retval UINTN Return value or 0 if it is out of defined range  

**/

UINTN GetAIndex (
    IN EFI_CACHE_ASSOCIATIVITY_DATA Value )
{
    if (Value >= EfiCacheAssociativityDirectMapped
            && Value <= EfiCacheAssociativity64Way)
        return (UINTN)Value;
    return 0;
}

/**
    Initialize CPU Setup information.

        
    @param Event 
    @param Context 

         
    @retval VOID

**/

VOID 
InitCPUInfo(
    IN  EFI_EVENT       Event, 
    IN  VOID            *Context
)
{
	EFI_STATUS                      Status;
	AMI_CPU_INFO                    *GetCpuInfo = NULL;
	AMI_CPU_INFO_PROTOCOL           *Cpu;
    UINT8                           CurSocket;
    UINT8                           CurCPU;
    UINT8                           a = 0;
    UINT8                           i;
    UINT8                           StringIndex;
    STRING_REF                      pCacheName;
    REGISTER_TABLE                  Reg;
    UINT8                           ExtFamily;
    UINT8                           BaseFamily;
    UINT8                           ExtModel;
    UINT8                           BaseModel;
    UINT8                           Stepping;
    UINT16                          CacheSize = 0;

    // ===================================================
    // The following intializes string arrays to allow
    // a simple algorithm for setting values. To support
    // additional nodes, simply modify MAX_NODES and
    // add additional string values.
	UINT16	sProcessorName[] = {
		STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE),
		STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE3),
	};
	UINT16	sNumberOfCores[] = {
		STRING_TOKEN(STR_PROCESSOR_CORE_VALUE),
		STRING_TOKEN(STR_PROCESSOR_CORE_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_CORE_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_CORE_VALUE3),
	};
	UINT16	sProcessorSpeed[] = {
		STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE),
		STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE3),
	};
	UINT16	sMicrocodeVersion[] = {
		STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE),
		STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE3),
	};

#if CACHE_INFO_MODEL == 0
	UINT16	sL1DataCache[] = {
		STRING_TOKEN(STR_PROCESSOR_L1_DCACHE_VALUE),
		STRING_TOKEN(STR_PROCESSOR_L1_DCACHE_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_L1_DCACHE_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_L1_DCACHE_VALUE3),
	};
	UINT16	sL1InstructionCache[] = {
		STRING_TOKEN(STR_PROCESSOR_L1_ICACHE_VALUE),
		STRING_TOKEN(STR_PROCESSOR_L1_ICACHE_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_L1_ICACHE_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_L1_ICACHE_VALUE3),
	};
	UINT16	sL2DataCache[] = {
		STRING_TOKEN(STR_PROCESSOR_L2_CACHE_VALUE),
		STRING_TOKEN(STR_PROCESSOR_L2_CACHE_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_L2_CACHE_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_L2_CACHE_VALUE3),
	};
	UINT16	sL3DataCache[] = {
		STRING_TOKEN(STR_PROCESSOR_L3_CACHE_VALUE),
		STRING_TOKEN(STR_PROCESSOR_L3_CACHE_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_L3_CACHE_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_L3_CACHE_VALUE3),
	};
#else
    UINT16  sL1DataCache[] = {
        STRING_TOKEN(STR_PROCESSOR_L1_DCACHE_VALUE),
        STRING_TOKEN(STR_PROCESSOR_L1_DCACHE_VALUE1)
    };
    UINT16  sL2DataCache[] = {
        STRING_TOKEN(STR_PROCESSOR_L2_CACHE_VALUE),
        STRING_TOKEN(STR_PROCESSOR_L2_CACHE_VALUE1)
    };
    UINT16  sL3DataCache[] = {
        STRING_TOKEN(STR_PROCESSOR_L3_CACHE_VALUE),
        STRING_TOKEN(STR_PROCESSOR_L3_CACHE_VALUE1)
    };
#endif

	UINT16	sProcessorFamily[] = {
		STRING_TOKEN(STR_PROCESSOR_FAMILY_VALUE),
		STRING_TOKEN(STR_PROCESSOR_FAMILY_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_FAMILY_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_FAMILY_VALUE3),
	};

	UINT16	sProcessorModel[] = {
		STRING_TOKEN(STR_PROCESSOR_MODEL_VALUE),
		STRING_TOKEN(STR_PROCESSOR_MODEL_VALUE1)
//		STRING_TOKEN(STR_PROCESSOR_MODEL_VALUE2),
//		STRING_TOKEN(STR_PROCESSOR_MODEL_VALUE3),
	};
	
    UINT16  sProcessorStepping[] = {
        STRING_TOKEN(STR_PROCESSOR_STEPPING_VALUE),
        STRING_TOKEN(STR_PROCESSOR_STEPPING_VALUE1)
//      STRING_TOKEN(STR_PROCESSOR_STEPPING_VALUE2),
//      STRING_TOKEN(STR_PROCESSOR_STEPPING_VALUE3),
    };
    CHAR16 *sAssociativity[] = {
        L"??-way  ",
        L"??-way  ", 
        L"??-way  ",
        L"Direct  ",    // EfiCacheAssociativityDirectMapped = 3
        L"2-way   ",    // EfiCacheAssociativity2Way = 4
        L"4-way   ",    // EfiCacheAssociativity4Way = 5
        L"Full-way",    // EfiCacheAssociativityFully = 6
        L"8-way   ",    // EfiCacheAssociativity8Way = 7
        L"16-way  ",    // EfiCacheAssociativity16Way = 8
        L"??-way  ",    // 9
        L"??-way  ",    // A
		L"32-way  ",    // EfiCacheAssociativity32Way = B
		L"48-way  ",    // EfiCacheAssociativity48Way = C
		L"64-way  "     // EfiCacheAssociativity64Way = D
    };

#if CACHE_INFO_MODEL == 0
    CHAR16 *sCacheString[] = {
        L"       L1 Data Cache",
        L"L1 Instruction Cache",
        L"            L2 Cache",
        L" L3 Cache per Socket"
    };
#else
    CHAR16 *sCacheString[] = {
        L" L1 Cache",
        L" L2 Cache",
        L" L3 Cache per Socket"
    };
#endif

    UINTN                       NumberOfInstalledProcessors;
    UINTN                       NumberOfDie;
    UINTN                       IgnoredRootBridges;
    AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
    CHAR8                       *LastBrandStringFound;

    UINTN                       SetupSize;
    SETUP_DATA                  SetupData;
    UINT32                      VarAttr;
    UINT8 						NumPhysicalCores;

    // Locate FabricTopologyServicesProtocol
    Status = pBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
    FabricTopology->GetSystemInfo (FabricTopology, &NumberOfInstalledProcessors, &NumberOfDie, &IgnoredRootBridges);

    SetupSize = sizeof(SETUP_DATA);
    Status = pRS->GetVariable(L"Setup", &gSetupGuid, &VarAttr, &SetupSize, &SetupData);
    if (!EFI_ERROR(Status)) {
        SetupData.InstalledCpus = (UINT8)NumberOfInstalledProcessors;
        pRS->SetVariable(L"Setup", &gSetupGuid, VarAttr, sizeof(SETUP_DATA), &SetupData);
    }

    if (EFI_ERROR (Status)) 
        return;

    // ===================================================
	// Find AMI_CPU_INFO_PROTOCOL protocol 
	Status = pBS->LocateProtocol (&gAmiCpuInfoProtocolGuid, NULL, &Cpu);
    ASSERT_EFI_ERROR(Status);
    if (EFI_ERROR(Status)) return;

    // ===================================================
    //  Node X Information
    //
    //  Notes: 
    //
    //  Information is obtained for core 0 of each node
    //  with the assumption that each core on one chip has 
    //  the same configuration (i.e., same cache size, 
    //  same speed, ...).
    //
    //  The CPU number of Core0 of nodeX is 
    //  calculated as:
    // 
    //      cpu# = X*(AMI_CPU_INFO.NumCores)
    //
    //  This assumes that each node has the same core count.
    // ===================================================
   
    CurCPU = 0;

    Status = pBS->AllocatePool(EfiBootServicesData, 49, &LastBrandStringFound);

    for (CurSocket=0; CurSocket<NumberOfInstalledProcessors; CurSocket++
            ,CurCPU=(GetCpuInfo->NumCores)*CurSocket)
    {
        Cpu->GetCpuInfo(Cpu, CurCPU, &GetCpuInfo);

        if(*GetCpuInfo->BrandString != 0)
        {
            MemCpy(LastBrandStringFound,GetCpuInfo->BrandString,49);
        }

        InitString( gHiiHandle, sProcessorName[CurSocket],
                L"%a", 
                LastBrandStringFound);

        if(GetCpuInfo->NumHts == 0)
        	NumPhysicalCores = GetCpuInfo->NumCores;
        else
        	NumPhysicalCores = GetCpuInfo->NumCores / 2;
        	
		if(NumPhysicalCores == 1)
		{
			InitString( gHiiHandle, sNumberOfCores[CurSocket],
				 L"One Core %d Threads",
				 GetCpuInfo->NumCores);
		}
		else
		{
			InitString( gHiiHandle, sNumberOfCores[CurSocket],
				 L"%d Cores %d Threads",
				 NumPhysicalCores,
				 GetCpuInfo->NumCores );

		}
		
		InitString( gHiiHandle, sProcessorSpeed[CurSocket],
			 L"Running @ %d MHz  %d mV", 
			 GetCpuInfo->ActualFreq, GetCpuInfo->Voltage );

        //SGEZT#25449:Version C CPUs are mixed, and CPU information displays are inconsistent+>
        //CPULib_CpuID(1, &Reg[EAX], &Reg[EBX], &Reg[ECX], &Reg[EDX]);
        Reg[EAX]=GetCpuInfo->FamModStep;
        //SGEZT#25449:Version C CPUs are mixed, and CPU information displays are inconsistent->

        ExtFamily = (UINT8)(Reg[EAX] >> 20 & 0xFF);
        BaseFamily = (UINT8)(Reg[EAX] >> 8 & 0xF);
        ExtModel = (UINT8)(Reg[EAX] >> 16 & 0xF);
        BaseModel = (UINT8)(Reg[EAX] >> 4 & 0xF);
        Stepping = (UINT8)(Reg[EAX] & 0xF);
        
        //<Lvshh001-20200730 Display BaseFamily, ExtFamily,BaseModel, ExtModel info +>
        //InitString(gHiiHandle, sProcessorFamily[CurSocket], L"Processor Family: %xh", ExtFamily + BaseFamily);
        //InitString (gHiiHandle, sProcessorFamily[CurSocket], L"Processor Family: %xh (Ext. %xh)", BaseFamily, ExtFamily);
        InitString (gHiiHandle, sProcessorFamily[CurSocket], L"Processor Family  : %xh (Ext. %xh)", BaseFamily, ExtFamily);
        //InitString(gHiiHandle, sProcessorModel[CurSocket], L"Processor Model: %xh",(ExtModel <<4) | BaseModel);
        InitString (gHiiHandle, sProcessorModel[CurSocket], L"Processor Model   : %xh (Ext. %xh)", BaseModel, ExtModel);
        //<Lvshh001-20200730 Display BaseFamily, ExtFamily,BaseModel, ExtModel info ->
    	InitString(gHiiHandle, sProcessorStepping[CurSocket], L"Processor Stepping: %xh", Stepping);
    	
#if CACHE_INFO_MODEL == 0                      
        for (i = 0; i < NUMBER_OF_CACHES; i++)
        {
            switch (GetCpuInfo->CacheInfo[i].Level)
            {   
                case EFI_CACHE_L1:
                    if (GetCpuInfo->CacheInfo[i].Type ==
                        EfiCacheTypeData)
                    {
                        // L1 data cache is per core
                        CacheSize = GetCpuInfo->CacheInfo[i].Size;
                        pCacheName = sL1DataCache[CurSocket];
                        StringIndex = 0;
                    }
                    else
                    {
                        // L1 instruction cache is per CU
                        CacheSize = GetCpuInfo->CacheInfo[i].Size;
                        pCacheName = sL1InstructionCache[CurSocket];
                        StringIndex = 1;
                    }
                    break;
                case EFI_CACHE_L2:
                    // L2 cache is per CU
                    CacheSize = GetCpuInfo->CacheInfo[i].Size;
                    pCacheName = sL2DataCache[CurSocket];
                    StringIndex = 2;
					break;
		        case EFI_CACHE_L3:
		            // L3 cache is shared by all cores		            
	                    CacheSize = GetCpuInfo->CacheInfo[i].Size;
                    pCacheName = sL3DataCache[CurSocket];
                    StringIndex = 3;
            }

            a = GetCpuInfo->CacheInfo[i].Associativity;
			if (StringIndex == 3) 
			{
				if(GetCpuInfo->CacheInfo[i].Size)
				InitString( gHiiHandle,	pCacheName,
        		        L"%s: %d MB/%s",
                        sCacheString[StringIndex],
                        CacheSize,
                        sAssociativity[GetAIndex(a)]);
				else
				InitString( gHiiHandle,	pCacheName,
        	        L"No L3 Cache Present");
			}
			else
			{
            	InitString( gHiiHandle,	pCacheName,
        		        L"%s: %d KB/%s",
                        sCacheString[StringIndex],
                        CacheSize,
                        sAssociativity[GetAIndex(a)]);
			}		
        }
#else
        for (i = 0; i < NUMBER_OF_CACHES; i++)
        {
            // The order of cache type is defined in tCache(CPU_CACHE_TABLE) of CpuBoard.c.
            // i = 0, L1 instruction cache is per CU in EDX[31:24] of CPUID 8000_0005 
            if (i == 0)
            {
                CacheSize = GetCpuInfo->CacheInfo[i].Size;
                continue;
            }
            // i = 1, L1 data cache is per Core in ECX[31:24] of CPUID 8000_0005 
            if (i == 1)
            {
                //L1 total size is L1 instruction + L1 data
                CacheSize += GetCpuInfo->CacheInfo[i].Size;
                CacheSize = CacheSize * GetCpuInfo->NumCores / ((GetCpuInfo->NumHts == 0) ? 1 : GetCpuInfo->NumHts);
                pCacheName = sL1DataCache[CurSocket];
                StringIndex = 0;
            }
            // i = 2, L2 cache is per CU in ECX[31:16] of CPUID 8000_0006
            if (i == 2)
            {
                CacheSize = GetCpuInfo->CacheInfo[i].Size * GetCpuInfo->NumCores / ((GetCpuInfo->NumHts == 0) ? 1 : GetCpuInfo->NumHts);
                pCacheName = sL2DataCache[CurSocket];
                StringIndex = 1;
            }
            // i = 3, L3 cache is shared by all cores in EDX[31:18] of CPUID 8000_0006
            // We can get it from GetCpuInfo->CacheInfo[i].Size for L3 cache.
            if (i == 3)
            {
                CacheSize = GetCpuInfo->CacheInfo[i].Size;
                pCacheName = sL3DataCache[CurSocket];
                StringIndex = 2;
            }

            if (StringIndex == 2)
            {
                if(GetCpuInfo->CacheInfo[i].Size)
                InitString( gHiiHandle, pCacheName,
                        L"%s: %d MB",
                        sCacheString[StringIndex],
                        CacheSize);
                else
                    InitString( gHiiHandle, pCacheName,
                        L"No L3 Cache Present");
            }
            else
            {
                InitString( gHiiHandle, pCacheName,
                        L"%s: %d KB",
                        sCacheString[StringIndex],
                        CacheSize);
            }
        } // for (i = 0; i < NUMBER_OF_CACHES; i++)
#endif
    	if (GetCpuInfo->MicroCodeVers != 0)
    		InitString( gHiiHandle,	sMicrocodeVersion[CurSocket],
    			        L"Microcode Patch Level: %08xh", //SGEZT#29542:Sync Microcode Path Level Info With Main Page
    			        GetCpuInfo->MicroCodeVers );
    }
    pBS->FreePool(LastBrandStringFound);
}

/**
    Initialize CPU strings.

        
    @param HiiHandle 
    @param Class 

         
    @retval VOID

**/

VOID
InitCPUStrings(
    IN EFI_HII_HANDLE HiiHandle,
    IN UINT16 Class )
{
  EFI_EVENT                       Event;
  EFI_STATUS                      Status;
  VOID                            *CpuNotifyReg;
  AMI_CPU_INFO_PROTOCOL           *Cpu;

  if (Class == ADVANCED_FORM_SET_CLASS){
    gHiiHandle = HiiHandle;
    Status = pBS->LocateProtocol (&gAmiCpuInfoProtocolGuid, NULL, &Cpu);
    if (Status == EFI_SUCCESS){
      InitCPUInfo(NULL, NULL);
    } else {
      Status = RegisterProtocolCallback(
                                        &gAmiCpuInfoProtocolGuid,
                                        InitCPUInfo,
                                        NULL,
                                        &Event,
                                        &CpuNotifyReg
                                        );
      ASSERT_EFI_ERROR(Status);
    }
  }
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
