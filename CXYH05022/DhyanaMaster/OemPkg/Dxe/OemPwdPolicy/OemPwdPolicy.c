#include <Efi.h>
#include <token.h>
#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Dxe.h>
#include "Library\UefiLib.h"
#include "Library\HobLib.h"

#include "AMIVfr.h"
#include "OemRtcResetHook.h"


typedef enum  
{
  AMI_PASSWORD_NONE = 0,
  AMI_PASSWORD_USER,
  AMI_PASSWORD_ADMIN,
  AMI_PASSWORD_ANY
}AMI_POLICY_PRIVATE;

EFI_GUID            AmiTseSetupGuid = AMITSESETUP_GUID;

/**
GC_TODO: ClearPwdByGPIO - Clear PWD Once Flag IsPWDClear Is Set By GPIO

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/

EFI_STATUS
ClearPwdByGPIO()
{
  EFI_STATUS          Status=EFI_SUCCESS;
  VOID                *pwdHobList;
  OEM_INFO_DATA_HOB   *pOemInfoHob = NULL;
  EFI_GUID            OemInfoDataHobGuid = OEM_INFO_DATA_HOB_GUID;
  BOOLEAN             IsPWDClear;
  UINTN               Size = sizeof(AMITSESETUP);
  UINT32              Attributes;
  AMITSESETUP         AmiTseData;
  // Get oem information from HOB
  pwdHobList = GetHobList ();
  ASSERT (pwdHobList != NULL);
    
  IsPWDClear = FALSE;
  pOemInfoHob = GetNextGuidHob (&OemInfoDataHobGuid, pwdHobList);
  if(pOemInfoHob != NULL) IsPWDClear = pOemInfoHob->IsPWDClear;
  TRACE((TRACE_DXE_CORE,"Clear Flag:%d\n", IsPWDClear));
  if(!IsPWDClear)
    return EFI_NOT_READY;
  
  Status = pRS->GetVariable (L"AMITSESetup", 
                  &AmiTseSetupGuid, 
                  &Attributes,
                  &Size,
                  &AmiTseData);
  if (EFI_ERROR(Status))
    return Status;
  //SetMem
  MemSet(AmiTseData.AdminPassword, SETUP_PASSWORD_LENGTH * sizeof (CHAR16), 0);
  MemSet(AmiTseData.UserPassword, SETUP_PASSWORD_LENGTH * sizeof (CHAR16), 0);

  // Save the password in NVRAM
  Status = pRS->SetVariable (L"AMITSESetup", 
                   &AmiTseSetupGuid, 
                   Attributes,
                   Size,
                   &AmiTseData);
  return Status;
}


UINT32 PasswordInstalledCheck(AMITSESETUP *mSysConf)
{
  UINT32    Installed = AMI_PASSWORD_NONE;
  CHAR16    *TestPassword = NULL;
  
  if (mSysConf == NULL)
    return Installed;
  
  TestPassword = Malloc((SETUP_PASSWORD_LENGTH + 1)*sizeof(CHAR16));
  if (TestPassword != NULL) {
    MemSet( TestPassword, (SETUP_PASSWORD_LENGTH + 1)*sizeof(CHAR16), 0 );
    if ( ! MemCmp( TestPassword, mSysConf->UserPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16) ) )
      Installed |= AMI_PASSWORD_USER;
    if ( ! MemCmp( TestPassword, mSysConf->AdminPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16) ) )
      Installed |= AMI_PASSWORD_ADMIN;
    Installed ^= AMI_PASSWORD_ANY;
    
    pBS->FreePool(TestPassword);
    TestPassword = NULL;
  }

  return Installed;
}
/**
GC_TODO: RestorePwdOnceNvramLoadDefault

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
RestorePwdOnceNvramLoadDefault()
{
  EFI_STATUS          Status = EFI_SUCCESS;
  BOOLEAN              ResetConfig = FALSE;

  EFI_GUID            AmiTseSetupGuid = AMITSESETUP_GUID;
  UINT32              Attributes;
  UINTN               Size = sizeof(AMITSESETUP);
  AMITSESETUP         AmiTseData;

  VOID               *HobList;
  EFI_HOB_GUID_TYPE  *GuidHob;
  AMI_TSE_HOB        *AmiTsehob = NULL;
  UINT32              PasswordInstalled = 0;


  HobList = GetHobList ();
  ASSERT (HobList != NULL);
  
  GuidHob = GetNextGuidHob (&AmiTseSetupGuid, HobList);
  if (GuidHob != NULL) {
    AmiTsehob = (AMI_TSE_HOB *) GuidHob;
    ResetConfig = AmiTsehob->RtcResetFlag;
    if(ResetConfig) PcdSetBool(PcdCmosClear, TRUE); //<lvych003-20170408 Improve show clear cmos message behavior>
  }
    
  TRACE((TRACE_DXE_CORE,"Hob:0x%x  ResetConfig:%d\n", GuidHob, ResetConfig));
  if (!ResetConfig)
    return EFI_NOT_READY;
  
  // Get the password information from HOB
  Status = pRS->GetVariable(L"AMITSESetup", 
                   &AmiTseSetupGuid, 
                   &Attributes,
                   &Size,
                   &AmiTseData);
        
  if (EFI_ERROR(Status)) 
    return Status;
  
  PasswordInstalled = PasswordInstalledCheck(&AmiTsehob->AmiTseData);
  TRACE((TRACE_DXE_CORE,"PasswordInstalledCheck:%d\n", PasswordInstalled));
  if (PasswordInstalled == 0) 
    return EFI_NOT_FOUND;
  
  AmiTsehob->AmiTseData.AMISilentBoot = AmiTseData.AMISilentBoot;
  // Save the password in NVRAM
  Status = pRS->SetVariable (L"AMITSESetup", 
                       &AmiTseSetupGuid, 
                       Attributes,
                       Size,
                       &AmiTsehob->AmiTseData);
  
  return Status;
}

#if defined(LOAD_DEFAULTS_ON_CMOS_CLEAR) && (LOAD_DEFAULTS_ON_CMOS_CLEAR == 1)
/**
GC_TODO: ClearPwdOnceNvramLoadDefault - Clear Password When NVRAM Load Default

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS
ClearPwdOnceNvramLoadDefault()
{
  EFI_STATUS    Status;
  EFI_GUID      AmiTseSetupGuid = AMITSESETUP_GUID;
  AMITSESETUP   AmiTseData;
  UINTN         VariableSize =0;
  UINT32        Attributes = 0;

  VariableSize = sizeof(AMITSESETUP);
  if(!PcdGetBool(PcdFirstBoot))
    return EFI_NOT_READY;
  
  Status = pRS->GetVariable ( L"AMITSESetup", \
                                  &AmiTseSetupGuid, \
                                  &Attributes, \
                                  &VariableSize, \
                                  &AmiTseData );
	if(EFI_ERROR(Status)) 
    Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  
  AmiTseData.AMISilentBoot = DEFAULT_QUIET_BOOT;
  
  MemSet(AmiTseData.AdminPassword, SETUP_PASSWORD_LENGTH * sizeof (CHAR16), 0);
  MemSet(AmiTseData.UserPassword, SETUP_PASSWORD_LENGTH * sizeof (CHAR16), 0);
  
  Status = pRS->SetVariable(
                      L"AMITSESetup",
                      &AmiTseSetupGuid,
                      Attributes,/*EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,*/
                      sizeof (AMITSESETUP),
                      &AmiTseData
                      ); 
  
  return Status;
}
#endif
/**
GC_TODO: PassWordClearEvent - Called When gEfiVariableWriteArchProtocolGuid Installed

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
VOID EFIAPI OemPwdPolicyEvent (
/*  IN EFI_EVENT Event, 
  IN VOID *Context*/)
{
  EFI_STATUS        Status=EFI_SUCCESS;

#if defined(LOAD_DEFAULTS_ON_CMOS_CLEAR) && (LOAD_DEFAULTS_ON_CMOS_CLEAR == 1)
  Status = ClearPwdOnceNvramLoadDefault();
  DEBUG((EFI_D_ERROR, "[OemDbg]ClearPwdOnceNvramLoadDefault:%r\r\n\n",Status));
#endif

#if defined(RESTORE_PASSWORD_ON_CMOS_CLEAR) && (RESTORE_PASSWORD_ON_CMOS_CLEAR == 1)
  Status = RestorePwdOnceNvramLoadDefault();
  DEBUG((EFI_D_ERROR, "[OemDbg]RestorePwdOnceNvramLoadDefault:%r\r\n\n",Status));
#endif

#if defined(BIOS_PASSWORD_CLEAR_PIN_SUPPORT)&& (BIOS_PASSWORD_CLEAR_PIN_SUPPORT == 1)           
  Status = ClearPwdByGPIO();
  DEBUG((EFI_D_ERROR, "[OemDbg]ClearPwdByGPIO:%r\r\n\n",Status));
#endif
/*
  if(Event) {
    pBS->CloseEvent(Event);
    Event=NULL; 
  }
  TRACE((TRACE_DXE_CORE,"Call back: return\n"));*/

  return;
}

/**
GC_TODO: PWDClear : Clear Pass World Function
@param SystemTable

@retval EFI_SUCCESS   - GC_TODO: add retval description
@retval EFI_NOT_FOUND - GC_TODO: add retval description
**/
EFI_STATUS OemPwdPolicy(
  IN  EFI_SYSTEM_TABLE *SystemTable
)
{ 
  EFI_STATUS  Status = EFI_SUCCESS;

//  EFI_EVENT   Event;
//  VOID        *Registration;

  //<Yaosr002-20200819 Add the start time of each page. +>
#if defined(REPORT_POST_START_TIME) && (REPORT_POST_START_TIME == 1)
  UINT16  Year;
  UINT8   Month, Day, Hour, Minute, Second;
#endif

#if defined(REPORT_POST_START_TIME) && (REPORT_POST_START_TIME == 1)
  Year = ReadRtcIndex(0x09);
  Month = ReadRtcIndex (0x08);
  Day = ReadRtcIndex (0x07);
  Hour = ReadRtcIndex (0x04);
  Minute = ReadRtcIndex (0x02);
  Second = ReadRtcIndex (0x00);
  DEBUG ((EFI_D_ERROR, "OemDbg:DXE start time: 20%.2d/%.2d/%.2d %.2d:%.2d:%.2d\n", Year, Month, Day, Hour, Minute, Second));
#endif
  //<Yaosr002-20200819 Add the start time of each page. ->
/*
  Status = RegisterProtocolCallback(
            &gEfiVariableWriteArchProtocolGuid,
            OemPwdPolicyEvent,
            NULL, &Event, &Registration
            );
  TRACE((TRACE_DXE_CORE,"Register OemPwdPolicyEvent call back:%r\n",Status));
*/
  OemPwdPolicyEvent();

  return Status;
}
