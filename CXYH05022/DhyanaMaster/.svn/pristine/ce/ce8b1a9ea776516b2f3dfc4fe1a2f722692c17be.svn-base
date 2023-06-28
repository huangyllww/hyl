#include <Setup.h>
#include <ppi\ReadOnlyVariable.h>
#include <ppi\ReadOnlyVariable2.h>
#include <Library\PcdLib.h>
#include <Library\DebugLib.h>
#include <Hob.h>
//#include "PCIeTopyConfig.h"
#include "NVMeTopy.c"
#include "AutoNVMePortingList.h"
#include <Protocol/RedirFruProtocol.h>
 NVME_CONFIG   *NvmeConfigList[] = {AUTO_NVME_PORTING_LIST NULL};
 CHAR8         *NvmeConfigNameList[] = {AUTO_NVME_PORTING_LIST_NAMES NULL};
 //Hugc_20220121_Modify Hotplug address by FRU Setting>>>
  //follow R-4ES-R-U2 string setting
 #define SLIMLINE_X8_1_CPU0      1        //SLIMLINE_X8_1_CPU0
 #define RISERA_SLIMLINE_X8_2    2      //Riser A -> R-4ES-R-U2 -> SLIMLINE_X8_2
 #define RISERA_SLIMLINE_X8_3    3     //Riser A -> R-4ES-R-U2 -> SLIMLINE_X8_3
 #define SLIMLINE_X8_2_CPU0      4       //SLIMLINE_X8_2_CPU0
  
 #define RISERB_SLIMLINE_X8_2    8      //Riser B -> R-4ES-R-U2 -> SLIMLINE_X8_2
 #define RISERB_SLIMLINE_X8_3    9        //Riser B -> R-4ES-R-U2 -> SLIMLINE_X8_3
 #define RISERC_SLIMLINE_X8_1      12      //Riser C SLIMLINE X8 P1
 #define RISERC_SLIMLINE_X8_0      13      //Riser C SLIMLINE X8 P0
 #define RISERA_RETIMER_OFFSET   12
 #define RISER_BC_RETIMER_OFFSET 8
  
  
  UINT8 GetFRUInfo (
          IN CONST EFI_PEI_SERVICES                   **PeiServices
    )
  {
 //     CONST EFI_PEI_SERVICES **PeiServices;
      EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
      CHAR16                               LastFruBaseBoardExtra1[256] = {0};
      EFI_STATUS                          Status = EFI_SUCCESS;
      UINTN                               Size;
      UINT32                              Attribute;
      
 //     PeiServices = GetPeiServicesTablePointer ();
      Status = (*PeiServices)->LocatePpi(
                  PeiServices,
                  &gEfiPeiReadOnlyVariable2PpiGuid,
                  0, 
                  NULL,
                  &ReadOnlyVariable
                  );
            
      Size = sizeof(LastFruBaseBoardExtra1);
      Status = ReadOnlyVariable->GetVariable (ReadOnlyVariable,L"LastFruBaseBoardExtra1", &gEfiRedirFruProtocolGuid, &Attribute, &Size, LastFruBaseBoardExtra1);
      DEBUG ((EFI_D_INFO, " GetFRUInfo LastFruBaseBoardExtra1: Status: %r\n", Status));
      if (Status == EFI_SUCCESS){
          DEBUG ((EFI_D_INFO, "LastFruBaseBoardExtra1: %s\n", LastFruBaseBoardExtra1));
          if ( StrCmp(LastFruBaseBoardExtra1,L"F001") == 0 )  {
              return 1;
          }else if ( StrCmp(LastFruBaseBoardExtra1,L"F002") == 0 )  {
              return 2;
          }else if ( StrCmp(LastFruBaseBoardExtra1,L"F003") == 0 ) {
              return 3;
          }else if ( StrCmp(LastFruBaseBoardExtra1,L"F004") == 0 )
          {
              return 4;
          }else if (StrCmp(LastFruBaseBoardExtra1,L"F005") == 0 )
          {
              return 5;
          }
      }
    return 0;
  }
 //Hugc_20220121_Modify Hotplug address by FRU Setting<<< 

EFI_STATUS
EFIAPI
NVMeConfigListConstructor (
    IN       EFI_PEI_FILE_HANDLE  FileHandle,
    IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                          Status;
  UINTN                               Index;
  SETUP_DATA                          SetupData;
  UINTN                               VariableSize;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *ReadOnlyVariable;
  EFI_GUID                            SetupGuid= SETUP_GUID;
  OEM_NVME_CONFIG_LIST_HOB            *NVMeConfigListHob;
  NVME_CONFIG                         *pNVMeConfigList;
  EFI_GUID                            OemNvmeConfigListHobGuid = OEM_NVME_CONFIG_LIST_HOB_GUID;

  //Locate Setup Variable
  Status = (*PeiServices)->LocatePpi(
              PeiServices,
              &gEfiPeiReadOnlyVariable2PpiGuid,
              0, 
              NULL,
              &ReadOnlyVariable
              );
        
  VariableSize = sizeof(SETUP_DATA);
  Status = ReadOnlyVariable->GetVariable(ReadOnlyVariable, L"Setup", &SetupGuid,
                                  NULL, &VariableSize, &SetupData);
  if (Status != EFI_SUCCESS){
  	DEBUG ((EFI_D_INFO, "[OemDbg Dxio]: Locate BIOS Setup Variable Fail\n"));
    return Status;
  }

  Status = (*PeiServices)->CreateHob(PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof(OEM_NVME_CONFIG_LIST_HOB), (void **)&NVMeConfigListHob);
  if(EFI_ERROR(Status))return Status;
  DEBUG ((EFI_D_INFO, "[OemDbg Dxio]Create HOB Status:%r\n",Status));
  NVMeConfigListHob->Header.Name = OemNvmeConfigListHobGuid;
  (*PeiServices)->CopyMem(NVMeConfigListHob->NVMeConfigList, (VOID *) &gNVMeConfigListInstalled, sizeof(gNVMeConfigListInstalled)); 
  
  DEBUG ((EFI_D_INFO, "[OemDbg Dxio]sizeof(gNVMeConfigListInstalled):%x\n",sizeof(gNVMeConfigListInstalled)));

  pNVMeConfigList = NVMeConfigListHob->NVMeConfigList;
  for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
  {
      DEBUG ((EFI_D_INFO, "Before update: Index %d: Die %d StartLane %d EndLane %d SlaveAddress %d\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
  }
  DEBUG ((EFI_D_INFO, "[OemDbg Dxio]SetupData.NVME_DEFAUL:%x\n",SetupData.NVME_DEFAULT));
  //Hugc_20220121_Modify Hotplug address by FRU Setting>>>
   // if (SetupData.NVME_DEFAULT == 1)
   //     return Status;
    if (SetupData.NVME_DEFAULT == 1){
         if(GetFRUInfo(PeiServices) == 2){
            
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/(sizeof(NVME_CONFIG)*2); Index++)
            {
                pNVMeConfigList[Index * 2].SlaveAddress = 0xFF;
                pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0xFF;   
                if(Index == SLIMLINE_X8_1_CPU0){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       //CPU0 P1 phyA0=CH0:0x40_0, phyA1=CH0:0x40_1(J135)
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if((Index == RISERA_SLIMLINE_X8_2)||(Index == RISERA_SLIMLINE_X8_2 + RISERA_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x44;       // CPU0 P2 phyA1=CH0:0x44_0,phyA2=CH0:0x44_1(riserA)
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x44;
                }
                if((Index == RISERA_SLIMLINE_X8_3)||(Index == RISERA_SLIMLINE_X8_3 + RISERA_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       // CPU0 P2 phyA3=CH0:0x42_0,phyA4=CH0:0x42_1(riserA)
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }
                if(Index == SLIMLINE_X8_2_CPU0){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x46;       //CPU0 P3 phyA0=CH0:0x46_1,phyA1=CH0:0x46_0(J41), 
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x46; 
                }
                if(Index == 6){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       // CPU1 P0 phyA0=CH1:0x40_0,phyA1=CH1:0x40_1(J43),   
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if(Index == 7){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       // CPU1 P0 phyA2=CH1:0x42_1, phyA3=CH1:0x42_0(J44),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }
                if((Index == 10)||(Index == 10 + RISER_BC_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x46;       //CPU1 P2 phyA0=CH1:0x46_1,phyA1=CH1:0x46_0(riserB) 
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x46; 
                }
                if((Index == 11)||(Index == 11 + RISER_BC_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x44;       //  CPU1 P2 phyA2=CH1:0X44_1,phyA3=CH1:0X44_0(riserB),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x44; 
                }                
            }
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
            {
                DEBUG ((EFI_D_INFO, "After follow FRU F002 update : Index %d: Die %d StartLane %d EndLane %d SlaveAddress %x\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
            }   
            
            
        }else if(GetFRUInfo(PeiServices) == 1){
            
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/(sizeof(NVME_CONFIG)*2); Index++)
            {
                pNVMeConfigList[Index * 2].SlaveAddress = 0xFF;
                pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0xFF;   
                 if((Index == RISERA_SLIMLINE_X8_2)||(Index == RISERA_SLIMLINE_X8_2 + RISERA_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       // CPU0 P2 phyA0=CH0:0x42_0,phyA1=CH0:0x42_1(riserA)
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42;
                }
                if((Index == RISERA_SLIMLINE_X8_3)||(Index == RISERA_SLIMLINE_X8_3 + RISERA_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       // CPU0 P2 phyA2=CH0:0x40_0,phyA3=CH0:0x40_1(riserA)
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if(Index == 6){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       // CPU1 P0 phyA0=CH1:0x40_0,phyA1=CH1:0x40_1(J43),   
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if(Index == 7){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       // CPU1 P0 phyA2=CH1:0x42_1, phyA3=CH1:0x42_0(J44),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }
                if((Index == 10)||(Index == 10 + RISER_BC_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x46;       //CPU1 P2 phyA0=CH1:0x46_1,phyA1=CH1:0x46_0(riserB) 
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x46; 
                }
                if((Index == 11)||(Index == 11 + RISER_BC_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x44;       //  CPU1 P2 phyA2=CH1:0X44_1,phyA3=CH1:0X44_0(riserB),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x44; 
                }
            }
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
            {
                DEBUG ((EFI_D_INFO, "After follow FRU F001 update : Index %d: Die %d StartLane %d EndLane %d SlaveAddress %x\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
            }   
        
            
        }  else if(GetFRUInfo(PeiServices) == 3){
            
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/(sizeof(NVME_CONFIG)*2); Index++)
            {
                pNVMeConfigList[Index * 2].SlaveAddress = 0xFF;
                pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0xFF;   
                if(Index == 1){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       // CPU0 P1 phyA0=CH1:0x40_0,phyA1=CH1:0x40_1(J135),   
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if(Index == 4){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       // CPU0 P3 phyA0=CH1:0x42_1, phyA1=CH1:0x42_0(J41),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }
                if((Index == 6)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       //CPU1 P0 phyA0=CH1:0x46_1,phyA1=CH1:0x46_0(J43) 
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if((Index == 7)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       //  CPU1 P0 phyA2=CH1:0X44_1,phyA3=CH1:0X44_0(J44),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }

            }
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
            {
                DEBUG ((EFI_D_INFO, "After follow FRU F003 update : Index %d: Die %d StartLane %d EndLane %d SlaveAddress %x\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
            }   
                  
        }else if(GetFRUInfo(PeiServices) == 4){
            
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/(sizeof(NVME_CONFIG)*2); Index++)
            {
                pNVMeConfigList[Index * 2].SlaveAddress = 0xFF;
                pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0xFF;   
                if(Index == 1){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       // CPU0 P1 phyA0=CH1:0x40_0,phyA1=CH1:0x40_1(J135),   
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if(Index == 4){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       // CPU0 P3 phyA0=CH1:0x42_1, phyA1=CH1:0x42_0(J41),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }
                if((Index == 6)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x44;       //CPU1 P0 phyA0=CH1:0x46_1,phyA1=CH1:0x46_0(J43) 
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x44; 
                }
                if((Index == 7)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x46;       //  CPU1 P0 phyA2=CH1:0X44_1,phyA3=CH1:0X44_0(J44),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x46; 
                }

            }
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
            {
                DEBUG ((EFI_D_INFO, "After follow FRU F004 update : Index %d: Die %d StartLane %d EndLane %d SlaveAddress %x\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
            }   
                  
        }else if(GetFRUInfo(PeiServices) == 5){
            
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/(sizeof(NVME_CONFIG)*2); Index++)
            {
                pNVMeConfigList[Index * 2].SlaveAddress = 0xFF;
                pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0xFF;   
                if(Index == 4){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       // CPU0 P3 phyA0=CH1:0x42_1, phyA1=CH1:0x42_0(J41),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if((Index == 6)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x40;       //CPU1 P0 phyA0=CH1:0x46_1,phyA1=CH1:0x46_0(J43) 
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x40; 
                }
                if((Index == 7)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x42;       //  CPU1 P0 phyA2=CH1:0X44_1,phyA3=CH1:0X44_0(J44),  
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x42; 
                }
                if((Index == 12)||(Index == 12 + RISER_BC_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x44;       // CPU1 P3 phyA0=CH1:0x44_0,phyA1=CH1:0x44_1(RiserC),   
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x44; 
                }
                if((Index == 13)||(Index == 13 + RISER_BC_RETIMER_OFFSET)){
                    pNVMeConfigList[Index * 2].SlaveAddress = 0x46;       // CPU1 P3 phyA2=CH1:0x46_0,phyA3=CH1:0x46_1(RiserC),   
                    pNVMeConfigList[Index * 2 + 1].SlaveAddress = 0x46; 
                }                

            }
            for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
            {
                DEBUG ((EFI_D_INFO, "After follow FRU F005 update : Index %d: Die %d StartLane %d EndLane %d SlaveAddress %x\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
            }   
                  
        }
        

        return Status;      
    }
    
  //Hugc_20220121_Modify Hotplug address by FRU Setting<<<
  
  pNVMeConfigList = NVMeConfigListHob->NVMeConfigList;
  for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/(sizeof(NVME_CONFIG)*2); Index++)
  {
      pNVMeConfigList[Index * 2].SlaveAddress = SetupData.NVME_HP_ADDR[Index];
      pNVMeConfigList[Index * 2 + 1].SlaveAddress = SetupData.NVME_HP_ADDR[Index];
      if((Index == 2) || (Index ==3)){
               pNVMeConfigList[(Index+RISERA_RETIMER_OFFSET) * 2].SlaveAddress = SetupData.NVME_HP_ADDR[Index];
               pNVMeConfigList[(Index+RISERA_RETIMER_OFFSET) * 2 + 1].SlaveAddress = SetupData.NVME_HP_ADDR[Index]; //RiserA retimer
           }
           if((Index == 8) || (Index ==9)||(Index == 10)|(Index == 11)){
               pNVMeConfigList[(Index+RISER_BC_RETIMER_OFFSET) * 2].SlaveAddress = SetupData.NVME_HP_ADDR[Index];
               pNVMeConfigList[(Index+RISER_BC_RETIMER_OFFSET) * 2 + 1].SlaveAddress = SetupData.NVME_HP_ADDR[Index]; //RiserB/RiserC retimer
           }
  }
  for(Index=0; Index < sizeof(gNVMeConfigListInstalled)/sizeof(NVME_CONFIG); Index++)
  {
      DEBUG ((EFI_D_INFO, "After update: Index %d: Die %d StartLane %d EndLane %d SlaveAddress %x\n", Index, pNVMeConfigList[Index].DieNumber, pNVMeConfigList[Index].StartLane, pNVMeConfigList[Index].EndLane, pNVMeConfigList[Index].SlaveAddress));
  }
  return Status;
}
