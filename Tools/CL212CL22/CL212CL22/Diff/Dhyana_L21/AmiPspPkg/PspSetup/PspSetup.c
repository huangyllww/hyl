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

/** @file PspSetup.c
    Psp Setup Routines

**/

#include <AmiDxeLib.h>
#include <Setup.h>
#include <AmdPspDirectory.h>
#include <Library/BaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <CpuCspLib.h>

// PSP L1 directory - Fixed region
PSP_DIRECTORY_ENTRY FirmwareVersionL1[]={
{PSP_FW_RECOVERY_BOOT_LOADER, 0, 0},	//Type 0x03
{SMU_OFFCHIP_FW, 0, 0},                 //Type 0x08
{AGESA_BOOT_LOADER_0, 0, 0},            //Type 0x30
{PSP_DIR_LV2, 0, 0},                    //Type 0x40
};

UINTN FirmwareVersionCountL1 = sizeof(FirmwareVersionL1)/sizeof(PSP_DIRECTORY_ENTRY);

// PSP L2 directory - Updatable region
PSP_DIRECTORY_ENTRY FirmwareVersionL2[]={
{PSP_FW_BOOT_LOADER, 0, 0},             //Type 0x01
{SMU_OFFCHIP_FW, 0, 0},                 //Type 0x08
{AGESA_BOOT_LOADER_0, 0, 0},            //Type 0x30
};

UINTN FirmwareVersionCountL2 = sizeof(FirmwareVersionL2)/sizeof(PSP_DIRECTORY_ENTRY);



/**
    Returns base address of Family18 PSP Entries

    @param
    @param

    @retval 
**/

EFI_STATUS
 GetF17PspEntryBase()
{
    UINT32  PspDir, BiosDir;
    VOID    *RomPtr;
    UINTN   i;
    UINTN   j;

    if(GetDirBase(&PspDir, &BiosDir) != TRUE){
        return EFI_NOT_FOUND;
    }
    RomPtr = (VOID *)(UINTN)PspDir;

    if (((PSP_DIRECTORY_HEADER*)RomPtr)->PspCookie != PSP_DIRECTORY_HEADER_SIGNATURE){
        
        return EFI_NOT_FOUND;
    }
    
    for (i = 0; i < FirmwareVersionCountL1; i++)
    {
        for (j = 0; j < ((PSP_DIRECTORY*)RomPtr)->Header.TotalEntries; j++)
        {
            if (((PSP_DIRECTORY*)RomPtr)->PspEntry[j].Type == FirmwareVersionL1[i].Type)
            {
                
                FirmwareVersionL1[i].Location = ((PSP_DIRECTORY*)RomPtr)->PspEntry[j].Location;
                break;
            }
        }
    }
    
    RomPtr = (VOID*)(FirmwareVersionL1[3].Location);
    
    if (((PSP_DIRECTORY_HEADER*)RomPtr)->PspCookie != PSP_LV2_DIRECTORY_HEADER_SIGNATURE){
           
           return EFI_NOT_FOUND;
       }       
    
    for (i = 0; i < FirmwareVersionCountL2; i++)
    {
        for (j = 0; j < ((PSP_DIRECTORY*)RomPtr)->Header.TotalEntries; j++)
        {
            if (((PSP_DIRECTORY*)RomPtr)->PspEntry[j].Type == FirmwareVersionL2[i].Type)
            {
                
                FirmwareVersionL2[i].Location = ((PSP_DIRECTORY*)RomPtr)->PspEntry[j].Location;
                break;
            }
        }
    }

    return  EFI_SUCCESS;
}


/**
    Initializes PSP FW Version String.

    @param HiiHandle Handle to HII database
    @param Class Indicates the setup class

    @retval VOID
**/

VOID InitPspStrings (
    IN EFI_HII_HANDLE   HiiHandle,
    IN UINT16           Class )
{
    EFI_STATUS                          Status = EFI_SUCCESS;
    UINT8                               *p;

    

    if (Class == ADVANCED_FORM_SET_CLASS) {
       
            Status = GetF17PspEntryBase();
            if (Status != EFI_SUCCESS)
                return;
            TRACE((-1,"\n InitPspStrings Psp Level 1 Fixed Region\n"));
            //PSP directory Level 1 Fixed region
            p = (UINT8 *)(FirmwareVersionL1[0].Location + 0x60);//PSP Recovery BootLoader Version
            InitString(HiiHandle, STRING_TOKEN(STR_PSP_REC_VER_VALUE), L"%d.%d.%d.%d", *(p+3), *(p+2), *(p+1), *p);
                       
            p = (UINT8 *)(FirmwareVersionL1[1].Location + 0x60);//SMU FW Version
            InitString(HiiHandle, STRING_TOKEN(STR_SMU_FW_VER_VALUE_L1), L"%d.%d.%d", *(p+2), *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL1[2].Location + 0x60);//ABL Version
            InitString(HiiHandle, STRING_TOKEN(STR_ABL_VER_VALUE_L1), L"%d.%d.%d.%d", *(p+3), *(p+2), *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL1[2].Location + 0xA0);//APCB Version
            InitString(HiiHandle, STRING_TOKEN(STR_APCB_VER_VALUE_L1), L"%d.%d", *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL1[2].Location + 0xA4);//APOB Version
            InitString(HiiHandle, STRING_TOKEN(STR_APOB_VER_VALUE_L1), L"%d.%d", *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL1[2].Location + 0xA8);//APPB Version
            InitString(HiiHandle, STRING_TOKEN(STR_APPB_VER_VALUE_L1), L"%d.%d", *(p+1), *p);

            //PSP directory Level 2 Updatable region
            p = (UINT8 *)(FirmwareVersionL2[0].Location + 0x60);//PSP BootLoader Version
            InitString(HiiHandle, STRING_TOKEN(STR_PSP_BOT_VER_VALUE), L"%d.%d.%d.%d", *(p+3), *(p+2), *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL2[1].Location + 0x60);//SMU FW Version
            InitString(HiiHandle, STRING_TOKEN(STR_SMU_FW_VER_VALUE_L2), L"%d.%d.%d", *(p+2), *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL2[2].Location + 0x60);//ABL Version
            InitString(HiiHandle, STRING_TOKEN(STR_ABL_VER_VALUE_L2), L"%d.%d.%d.%d", *(p+3), *(p+2), *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL2[2].Location + 0xA0);//APCB Version
            InitString(HiiHandle, STRING_TOKEN(STR_APCB_VER_VALUE_L2), L"%d.%d", *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL2[2].Location + 0xA4);//APOB Version
            InitString(HiiHandle, STRING_TOKEN(STR_APOB_VER_VALUE_L2), L"%d.%d", *(p+1), *p);

            p = (UINT8 *)(FirmwareVersionL2[2].Location + 0xA8);//APPB Version
            InitString(HiiHandle, STRING_TOKEN(STR_APPB_VER_VALUE_L2), L"%d.%d", *(p+1), *p);
        
        
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
