//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **

//**********************************************************************
// $Header: /Alaska/Tools/template.c 6     1/13/10 2:13p Felixp $
//
// $Revision: 6 $
//
// $Date: 1/13/10 2:13p $
//**********************************************************************
// Revision History
// ----------------
// $Log: /Alaska/Tools/template.c $
// 
// 6     1/13/10 2:13p Felixp
// 
//**********************************************************************
//<AMI_FHDR_START>
//
// Name:  <This File's Name>
//
// Description: 
//
//<AMI_FHDR_END>
//**********************************************************************

#include <SetupText.h>
#include <Protocol/AmiSmbios.h>
UINT32
MyReadaddr32 (
  IN  UINT32     Address
  )
{
    return *(( UINT32*) (Address));
}
EFI_GUID gAmiSmbiosProtocolGuid = { 0x5e90a50d, 0x6955, 0x4a49, { 0x90, 0x32, 0xda, 0x38, 0x12, 0xf8, 0xe8, 0xe5 } };
VOID MySmBiosInit(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{   EFI_STATUS      Status;
    AMI_SMBIOS_PROTOCOL     *gSmbiosProtocol;
    UINT16              Type0DataSize = sizeof(SMBIOS_BIOS_INFO);
    SMBIOS_BIOS_INFO  *Type0;
    UINT8                    *Type0DataPtr ;
    UINT8                     myBiosVendor;                     // String number
    UINT8                     myBiosVersion;                    // String number
    UINT8                     myBiosReleaseDate;                // String number
    UINT8                     mySystemBiosMajorRelease;
    UINT8                     mySystemBiosMinorRelease;
    UINT8                     myECFirmwareMajorRelease;
    UINT8                     myECFirmwareMinorRelease;
 
    
    Status = pBS->LocateProtocol(&gAmiSmbiosProtocolGuid, NULL, &gSmbiosProtocol);  
    Status = gSmbiosProtocol->SmbiosReadStrucByType( 
                                                            0,
                                                            1,
                                                            &Type0DataPtr,
                                                            &Type0DataSize);  //readtype0
    ASSERT_EFI_ERROR(Status);
    Type0=  ((SMBIOS_BIOS_INFO*)Type0DataPtr);
    DEBUG((DEBUG_ERROR, "Hyl:*TYPE0:%x \n", *Type0)); 
    DEBUG((DEBUG_ERROR, "Hyl:TYPE0:%x \n", Type0)); 
    DEBUG((DEBUG_ERROR, "Hyl:&TYPE0:%x \n", &Type0)); 
    Type0++;
    DEBUG((DEBUG_ERROR, "Hyl:*TYPE0:%x \n", *Type0));
    DEBUG((DEBUG_ERROR, "Hyl:TYPE0:%x \n",Type0)); 
    DEBUG((DEBUG_ERROR, "Hyl:&TYPE0:%x \n", &Type0)); 
  
    myBiosVendor=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->BiosVendor;
    myBiosVersion=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->BiosVersion;
    myBiosReleaseDate=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->BiosReleaseDate;
    mySystemBiosMajorRelease=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->SystemBiosMajorRelease;
    mySystemBiosMinorRelease=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->SystemBiosMinorRelease;
    myECFirmwareMajorRelease=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->ECFirmwareMajorRelease;
    myECFirmwareMinorRelease=  ((SMBIOS_BIOS_INFO*)Type0DataPtr)->ECFirmwareMinorRelease;
    DEBUG((DEBUG_ERROR, "Hyl:myBiosVendor:%x \n", myBiosVendor));      
    DEBUG((DEBUG_ERROR, "Hyl:myBiosVersion:%x \n", myBiosVersion));
    DEBUG((DEBUG_ERROR, "Hyl:myBiosReleaseDate:%x\n ", myBiosReleaseDate));
    DEBUG((DEBUG_ERROR, "Hyl:mySystemBiosMajorRelease:%x \n ", mySystemBiosMajorRelease));      
    DEBUG((DEBUG_ERROR, "Hyl:mySystemBiosMinorRelease:%x\n ", mySystemBiosMinorRelease));
    DEBUG((DEBUG_ERROR, "Hyl:myECFirmwareMajorRelease:%x \n", myECFirmwareMajorRelease));
    DEBUG((DEBUG_ERROR, "Hyl:myECFirmwareMinorRelease:%x \n", myECFirmwareMinorRelease));
}


/*
UINT8
MyReadaddr8 (
  IN  UINT32     Address
  )
{
    return *(( UINT8*) (Address));
}
MyReadaddr32 (
  IN  UINT32     Address
  )
{
    return *(( UINT32*) (Address));
}
VOID MySmBiosInit(EFI_HII_HANDLE HiiHandle, UINT16 Class)
{
   
    CHAR16  Type0Data[50]={0};
    CHAR16  Type1Data[50]={0};
    UINT32 realaddress;
    UINT32 smaddr;
    UINT32 SmbiosTableaddr;
    UINT32 SmbiosLength;
    UINT32 Dataindex;
   

            realaddress=0xf0000;   //find 0xf0000-0xfffff _SM_
            while(realaddress<0xfffff)
            {
                
                
               if(MyReadaddr32(realaddress)==0x5f4d535f)//find_SM_
               {               
                   DEBUG((DEBUG_ERROR, "Hyl:SmBiosaddr find _SM_ success "));   
                                
                   if((MyReadaddr32(realaddress+16)==0x494d445f)&&(MyReadaddr8(realaddress+20)==0x5f))//find_DMI_
                              {    
                                   smaddr=realaddress;
                                   DEBUG((DEBUG_ERROR, "Hyl:SmBiosaddr find _DMI_success "));                   
                                   break;
                              } 
                   
               }
               realaddress = realaddress + 4;
          
            } 
           
            SmbiosTableaddr=MyReadaddr32(smaddr+0x18); //EPSTABLE adde
            SmbiosLength=  ((MyReadaddr32(SmbiosTableaddr) )&0x0000ff00 )>>8;
            realaddress=SmbiosTableaddr+SmbiosLength;
            Dataindex=0;
            while(1)
            {           
                
                if(  MyReadaddr8(realaddress)==0&&MyReadaddr8(realaddress+1)==0) 
                {
                    break;
                }
                if( MyReadaddr8(realaddress)>0) 
                {
                    Type0Data[Dataindex]=MyReadaddr8(realaddress);                  
                    Dataindex++;
                }
                if( MyReadaddr8(realaddress)==0) 
                         {
                                       Type0Data[Dataindex]=0x2e ;
                                       Dataindex++;
                         }
                
                realaddress++;
            }

            InitString( HiiHandle,
                                                 STRING_TOKEN(STR_SMBIOS_VALUE0),
                                                 L"SMBIOSTYPE0:%s" ,Type0Data);
            Dataindex=0;
            SmbiosLength=  ((MyReadaddr32(realaddress+2) )&0x0000ff00 )>>8;
            realaddress=realaddress+SmbiosLength+2;
            while(1)
                        {
                           
                            
                            if(  MyReadaddr8(realaddress)==0&&MyReadaddr8(realaddress+1)==0) 
                            {
                                break;
                            }
                            if( MyReadaddr8(realaddress)>0) 
                            {
                                Type1Data[Dataindex]=MyReadaddr8(realaddress) ;
                                Dataindex++;
                            }
                            if( MyReadaddr8(realaddress)==0) 
                             {
                                Type1Data[Dataindex]=0x2e ;
                                Dataindex++;
                              }

                            realaddress++;
                            
                        }


             InitString( HiiHandle,
                                                 STRING_TOKEN(STR_SMBIOS_VALUE1),
                                                 L"SMBIOSTYPE1:%s" ,Type1Data);
             DEBUG((DEBUG_ERROR, "Hyl:InitSetupText exit\n ")); 
             DEBUG((DEBUG_ERROR, "====================================\n" ));
             
}
            
            
            */


//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2010, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **        
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
