/** @file OemPasswordPolicy.c
    Function file for Tse hook functionalities.

**/

//----------------------------------------------------------------------
// Include(s)
//----------------------------------------------------------------------
#include "AmiLib.h"
#include "AmiDxeLib.h"
#include "TseCommon.h"
//#include <Guid/SetupVariable.h>
#include <Setup.h>
#include "Timer.h"
#include "commonoem.h"
#include "Include/Protocol/AMIPostMgr.h"
#include "Include/Protocol/EsaTseInterfaces.h"
#include <Library/HobLib.h>  //<lvych001-20170811 Revised bios recovery update bios maybe fail when setup screenlock.>
//<Wangbn001-20191029 BIOS password policy+>
#include <TseSetupTransfer.h>//zanggy-20201217-Support PasswordComplexityMode takes effect immediatley in setup+>
//#include <OemSetupVarInCache.h>
//<Wangbn001-20191029 BIOS password policy->
//<Wangbn001-20191102-password four Consecutive Chars check+>
#include <Protocol/FlashProtocol.h>
//<Wangbn001-20191102-password four Consecutive Chars check->
#include "minisetup.h"
#include "OemPasswordPolicy.h"

//----------------------------------------------------------------------
// External Function/Variable Declaration
//----------------------------------------------------------------------
extern VOID GetAMITSEVariable(VOID **mSysConf, UINT8 **setup, UINTN *VarSize);
extern VOID EfiStrCpy(IN CHAR16 *Destination, IN CHAR16 *Source);
extern BOOLEAN IsPasswordSupportNonCaseSensitive();
extern EFI_STATUS InitEsaTseInterfaces (void);
extern VOID MouseInit(VOID);
UINT8 GetPasswordReportInboxcolor(VOID);
VOID SetPwdKeyboardLayout(VOID);
VOID ResetPwdKeyboardLayout(VOID);
VOID SetPwdTimeOut( EFI_EVENT Event, BOOLEAN *timeout ); 

//----------------------------------------------------------------------
// Internal Function Declaration
//----------------------------------------------------------------------
EFI_STATUS RecodePasswordDataLocal(VOID);

//----------------------------------------------------------------------
// Global Variable Declaration
//----------------------------------------------------------------------
extern EFI_RUNTIME_SERVICES     *gRT;
extern EFI_BOOT_SERVICES        *gBS;
extern EFI_SYSTEM_TABLE         *gST;
extern ESA_INTERFACES_FOR_TSE   *gEsaInterfaceForTSE;
extern Tse_Setup_transfer       *pTseSetuptransferInterface; 
static SCREEN_LOCK_DATA         gScreenLockData={0};

//Mizl-20220317-SGMTB#0143674-PasswordComplexMode+>

typedef union {
  struct {
    UINT8      PwdComplex_Uper:1;
    UINT8      PwdComplex_Lower:1;
    UINT8      PwdComplex_Number:1;
    UINT8      PwdComplex_Other:1;
    UINT8      MasterVersion:4;
  } Field;
  UINT8 Value;
} PWD_COMPLEX_MODE;
//Mizl-20220317-SGMTB#0143674-PasswordComplexMode->
//----------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------
//<Wangbn001-20191102-password four Conseutive Chars check+>
VOID PasswordEncodeHook( CHAR16 *Password, UINTN MaxSize);
extern BOOLEAN TSEPwdSavetoNvram(VOID);
UINTN
EfiStrLen (
  IN CHAR16   *String
  );

#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)
#if defined(OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS) && (OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS)

#if !defined(OEM_TSE_PSW_HASH_DATA_LOCATION)
#define OEM_TSE_PSW_HASH_DATA_LOCATION 1
#endif

#if !defined(OEM_TSE_PSW_NUMBER_HASH_MAX) || !(OEM_TSE_PSW_NUMBER_HASH_MAX)
#define OEM_TSE_PSW_NUMBER_HASH_MAX      1
#elif (OEM_TSE_PSW_NUMBER_HASH_MAX > 3)
#undef OEM_TSE_PSW_NUMBER_HASH_MAX
#define OEM_TSE_PSW_NUMBER_HASH_MAX   3
#endif

#ifndef OEM_TSE_PSW_HASH_CHARS_COUNT
#define OEM_TSE_PSW_HASH_CHARS_COUNT      4
#elif (OEM_TSE_PSW_HASH_CHARS_COUNT > SETUP_PASSWORD_LENGTH)
#undef OEM_TSE_PSW_HASH_CHARS_COUNT
#define OEM_TSE_PSW_HASH_CHARS_COUNT      SETUP_PASSWORD_LENGTH
#endif


#define OEM_TSE_PSW_VAR_GUID  {0x99d3288d, 0xef03, 0x455d, {0x96, 0x95, 0x8a, 0xd6, 0x35, 0x91, 0xd, 0xea}}
EFI_GUID    gOemTsePswVarGuid = OEM_TSE_PSW_VAR_GUID;

/// SHA-1 digest size in bytes.
///
#define SHA1_DIGEST_SIZE    20
///
/// SHA-256 digest size in bytes
///
#define SHA256_DIGEST_SIZE  32

#pragma pack(push, 1)
typedef struct
{
    UINT8  PswCount;
    UINT8  FirstNumber;
}OEM_TSE_PSW_INFO;

typedef struct
{
#if TSE_HASH_PASSWORD
#if TSE_PWD_ENCRYPT_USING_SHA256
    UINT8 PswData[SHA256_DIGEST_SIZE];
#else
    UINT8 PswData[SHA1_DIGEST_SIZE];
#endif
#else
    UINT16 PswData[SETUP_PASSWORD_LENGTH];
#endif
}OEM_TSE_PSW_DATA;

#define OEM_TSE_PSW_FV_RECORD_SIGNATURE   0x55AA

typedef struct
{
    UINT8   PswDataCount;
    OEM_TSE_PSW_DATA  FullPswData;
//  followed data is : OEM_TSE_PSW_DATA[PswDataCount]
}OEM_TSE_PSW_NVRAM_RECORD_HEADER;

//16 bytes region descriptor
typedef struct
{
    UINT32  Offset;
    UINT32  Size;
    UINT8   MaxRecordLimit;
    UINT8   Reserved[7];
}OEM_TSE_PSW_FV_REC_REGION_DESC;

//64 bytes FV descriptor
typedef struct
{
    EFI_GUID    Signature;
    UINT32      Size;
    UINT8       Reserved[12];
    OEM_TSE_PSW_FV_REC_REGION_DESC    AdminPswRegionDesc;
    OEM_TSE_PSW_FV_REC_REGION_DESC    UserPswRegionDesc;
}OEM_TSE_PSW_FV_DESC;

//8 bytes record header descriptor + one PswData(OEM_TSE_PSW_DATA)
typedef struct
{
    UINT16  Signature;      //0x55AA
    UINT8   Flag;           //0xff - valid record, Other - Record deleted
    UINT8   PswDataCount;   //How many data(OEM_TSE_PSW_DATA) followed the header
    UINT8   Reserved[4];
    OEM_TSE_PSW_DATA  FullPswData;
//  followed data is : OEM_TSE_PSW_DATA[PswDataCount]
}OEM_TSE_PSW_FV_RECORD_HEADER;

#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 0)
typedef OEM_TSE_PSW_NVRAM_RECORD_HEADER OEM_TSE_PSW_RECORD_HEADER;
#elif (OEM_TSE_PSW_HASH_DATA_LOCATION == 1)
typedef OEM_TSE_PSW_FV_RECORD_HEADER OEM_TSE_PSW_RECORD_HEADER;
#endif

#define TSE_PSW_REC_SIZE(mRec) (sizeof(OEM_TSE_PSW_RECORD_HEADER) + ((UINTN)mRec.PswDataCount)*sizeof(OEM_TSE_PSW_DATA))
#define TSE_PSW_REC_SIZE_P(pRec) (sizeof(OEM_TSE_PSW_RECORD_HEADER) + ((UINTN)pRec->PswDataCount)*sizeof(OEM_TSE_PSW_DATA))

#pragma pack(pop)


//temp string area to save the plaintext password
CHAR16 gTmpAdminPlaintextPsw[PASSWORD_MAX_SIZE+1] = {0};
CHAR16 gTmpUserPlaintextPsw[PASSWORD_MAX_SIZE+1] = {0};
#endif
#endif

//<lvych001-20170502 Improve code+>
// VOID RefreshSetup(){
//   if (gApp != NULL) 
//     gApp->CompleteRedraw = TRUE;
// }
extern VOID RefreshSetup();
//<lvych001-20170502 Improve code->

//zanggy-20201217-Support PasswordComplexityMode takes effect immediatley in setup+>
UINT8 GetPasswordComplexityMode(){
    EFI_STATUS  Status;
    SETUP_DATA          *SetupData;
    UINTN               SetupDataSize = sizeof(SETUP_DATA);
    EFI_GUID            EfiSetupGuid = SETUP_GUID; 
    Tse_Setup_transfer  *TseSetuptransferInterface = NULL;
    EFI_GUID            TseTransferGuid = TSE_TRANSFER_GUID;

    if(pTseSetuptransferInterface == NULL)
    {
        Status = pBS->LocateProtocol(&TseTransferGuid, NULL, &pTseSetuptransferInterface);
        if(EFI_ERROR(Status))
        {
          //default is PASSWORD_SENSITIVE_LEVEL
          return PASSWORD_SENSITIVE_LEVEL;
        }
    }

    pTseSetuptransferInterface->VarGetBuffer(L"Setup",EfiSetupGuid,(UINT8**)&SetupData,&SetupDataSize);
    return SetupData->PasswordComplexityMode;

}
//zanggy-20201217-Support PasswordComplexityMode takes effect immediatley in setup->

UINT8 GetPasswordRetries(){
    EFI_STATUS  Status;
    SETUP_DATA          SetupData;
    UINTN               SetupDataSize;
    EFI_GUID gEfiSetupGuid = SETUP_GUID; 
    UINT8   Retries = 3;

    SetupDataSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
    if (!EFI_ERROR(Status)) Retries = SetupData.PasswordRetries;
    
    return Retries;
}

//<Wangyia-20200812 Support Password Repeat +>
UINT8 GetPasswordRepeatNumbers(){
    EFI_STATUS          Status;
    SETUP_DATA          SetupData;
    UINTN               SetupDataSize;
    EFI_GUID            gEfiSetupGuid = SETUP_GUID; 
    UINT8               Repeats = 3;

    SetupDataSize = sizeof(SETUP_DATA);
    Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
    if (!EFI_ERROR(Status)) Repeats = SetupData.PasswordRepeatNumbers;
    
    return Repeats;
}
//<Wangyia-20200812 Support Password Repeat ->

//<lvych006-20160310 Administrator and User Password Policy +>
#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)
BOOLEAN PreProcessConInAvailability(VOID)
{
  RecodePasswordDataLocal();
  return FALSE;
}

EFI_STATUS RecodePasswordDataLocal(VOID)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    EFI_GUID mAmiTseOemPortingVar2Guid =  OEM_PORT_VARIABEL2;
    RECORD_PASSWORD_DATA            RecodeSetupData;
    UINTN                           RecodeSetupDataSize;   
    UINT32          Attributes;
    UINT16 *RecodePassword=NULL;
    BOOLEAN UserUpdateFlag=FALSE,AdminUpdateFlag=FALSE;

    UINT8 *setup = NULL;
    AMITSESETUP *mSysConf = NULL;
    UINTN VarSize = 0;
    UINT32 PasswordInstalled = AMI_PASSWORD_NONE;    
    UINT8                      NoOfRepeats;
    
    SETUP_DEBUG_TSE( "RecodePasswordDataLocal.\n");
    GetAMITSEVariable(&mSysConf,&setup,&VarSize);
    if ( ( mSysConf != NULL ) && ( VarSize == sizeof(AMITSESETUP) ) )
    {    
        MemSet( &RecodeSetupData, sizeof (RECORD_PASSWORD_DATA), 0);
        RecodeSetupDataSize       = sizeof (RECORD_PASSWORD_DATA);
        Status = gRT->GetVariable ( L"OemPwdVar", &mAmiTseOemPortingVar2Guid, &Attributes, &RecodeSetupDataSize, &RecodeSetupData);
        if (EFI_ERROR(Status)) {

            Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;  
        }
        PasswordInstalled = PasswordCheckInstalled();
        
        if ( EfiCompareMem( RecodeSetupData.UserPassword, mSysConf->UserPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16) ) ){
            MemCpy(RecodeSetupData.UserPassword, mSysConf->UserPassword,((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16));
            if(PasswordInstalled&AMI_PASSWORD_USER) UserUpdateFlag = TRUE;
        }
        
        if ( EfiCompareMem( RecodeSetupData.AdminPassword, mSysConf->AdminPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16) ) ){
            MemCpy(RecodeSetupData.AdminPassword, mSysConf->AdminPassword,((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16));
            if(PasswordInstalled&AMI_PASSWORD_ADMIN) AdminUpdateFlag = TRUE;
        }

        NoOfRepeats = GetPasswordRepeatNumbers();
        if(UserUpdateFlag||AdminUpdateFlag){
            if(UserUpdateFlag){
                RecodePassword = (UINT16*)&RecodeSetupData.PasswordData[RecodeSetupData.RecodeIndex];
                MemSet( RecodePassword, ((UINT8)OEM_SETUP_PASSWORD_LENGTH) * sizeof(CHAR16), 0 );//liutz001_20190628_Repeat password can use issue
                MemCpy(RecodePassword,RecodeSetupData.UserPassword,((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16));

                if(RecodeSetupData.RecodeTotalNum < SETUP_PASSWORD_MAX_RECODE_NUM){
                    RecodeSetupData.RecodeTotalNum++;
                }
                
                if(RecodeSetupData.RecodeIndex < SETUP_PASSWORD_MAX_RECODE_NUM - 1){
                    RecodeSetupData.RecodeIndex++;
                }else{
                    RecodeSetupData.RecodeIndex=0;
                }
            }
            
            if(AdminUpdateFlag){
                RecodePassword = (UINT16*)&RecodeSetupData.PasswordData[RecodeSetupData.RecodeIndex];
                MemSet(RecodePassword, ((UINT8)OEM_SETUP_PASSWORD_LENGTH) * sizeof(CHAR16), 0 );//Oem_liutz001_20190628_Repeat password can use issue
                MemCpy(RecodePassword,RecodeSetupData.AdminPassword,((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16));
                

                if(RecodeSetupData.RecodeTotalNum < SETUP_PASSWORD_MAX_RECODE_NUM){
                    RecodeSetupData.RecodeTotalNum++;
                }
                
                if(RecodeSetupData.RecodeIndex < SETUP_PASSWORD_MAX_RECODE_NUM - 1){
                    RecodeSetupData.RecodeIndex++;
                }else{
                    RecodeSetupData.RecodeIndex=0;
                }
            }
            
            Status = gRT->SetVariable( L"OemPwdVar", \
                              &mAmiTseOemPortingVar2Guid, \
                              Attributes, \
                              sizeof (RECORD_PASSWORD_DATA), \
                              &RecodeSetupData);
            
        }
        SETUP_DEBUG_TSE( "RecodePasswordDataLocal Total:%d,Index:%d,User:%d,Admin:%d.\n",RecodeSetupData.RecodeTotalNum,RecodeSetupData.RecodeIndex,UserUpdateFlag,AdminUpdateFlag);

    }
    
    if(setup)
        MemFreePointer( (VOID **)&setup );
    else
        MemFreePointer( (VOID **)&mSysConf );
    
    return Status;
}
#endif

//<huangjin001-20190521 Fix can't set 32 bits HDD password when Admin/user password installed + >
BOOLEAN CheckPasswordRepeatLocal(CHAR16*Password,UINTN PwdLength)
{
    EFI_STATUS          Status = EFI_SUCCESS;
    BOOLEAN RepeatStatus=FALSE;
    EFI_GUID mAmiTseOemPortingVar2Guid =  OEM_PORT_VARIABEL2;
    RECORD_PASSWORD_DATA            RecodeSetupData;
    UINTN                           RecodeSetupDataSize;
    UINTN   index,ii;
    CHAR16 *RealPassword=NULL,*TempEncoded=NULL;
    UINT8   PasswordRepeat = 0;
    UINT16  PasswordTotal = 0;

    SETUP_DEBUG_TSE( "CheckPasswordRepeatLocal.\n");

    if ( Password == NULL ||PwdLength == 0)
        return RepeatStatus;

    TempEncoded = EfiLibAllocateZeroPool( ((UINT8)OEM_SETUP_PASSWORD_LENGTH+1) * sizeof(CHAR16) );
    if ( TempEncoded == NULL )
        return RepeatStatus;

    MemSet( TempEncoded, ((UINT8)OEM_SETUP_PASSWORD_LENGTH+1) * sizeof(CHAR16), 0 );
    EfiStrCpy(TempEncoded,Password);    
    
    PasswordEncode(TempEncoded,SETUP_PASSWORD_LENGTH*sizeof(CHAR16));//liutz001_20190628_Repeat password can use issue

    RecodeSetupDataSize       = sizeof (RECORD_PASSWORD_DATA);
    Status           = gRT->GetVariable ( L"OemPwdVar", &mAmiTseOemPortingVar2Guid, NULL, &RecodeSetupDataSize, &RecodeSetupData);
    if (EFI_ERROR(Status)) return RepeatStatus;
    
    PasswordRepeat = GetPasswordRepeatNumbers();
    index = RecodeSetupData.RecodeIndex;
    PasswordTotal = RecodeSetupData.RecodeTotalNum;
    
    if ((index == 0) && (RecodeSetupData.RecodeTotalNum == SETUP_PASSWORD_MAX_RECODE_NUM))
        index = SETUP_PASSWORD_MAX_RECODE_NUM - 1;
    if (index > 0)
        index--;

    for(; ((PasswordRepeat > 0) && (PasswordTotal > 0)); PasswordRepeat--, PasswordTotal--){
        RealPassword = (CHAR16*)&RecodeSetupData.PasswordData[index];
        for(ii=0;ii<OEM_SETUP_PASSWORD_LENGTH;ii++){
            if(ii%10==0) SETUP_DEBUG_TSE( "\n");
            SETUP_DEBUG_TSE( "%x,%x  ",TempEncoded[ii],RealPassword[ii]);
        }
        if( (!EfiCompareMem(TempEncoded,RealPassword,(UINT8)OEM_SETUP_PASSWORD_LENGTH * sizeof(CHAR16)))||
            (!EfiCompareMem(Password,RealPassword,(UINT8)OEM_SETUP_PASSWORD_LENGTH * sizeof(CHAR16))))
        {
            RepeatStatus = TRUE;
            break;
        }
        if ((index == 0) && (RecodeSetupData.RecodeTotalNum == SETUP_PASSWORD_MAX_RECODE_NUM))
            index = SETUP_PASSWORD_MAX_RECODE_NUM - 1;
        else
            index--;
    }
    MemFreePointer( (VOID **)&TempEncoded );
    return RepeatStatus;
}
//<huangjin001-20190521 Fix can't set 32 bits HDD password when Admin/user password installed - >
//Mizl-20220317-SGMTB#0143674-PasswordComplexMode +>
BOOLEAN
CheckPwdComplex(IN UINT8 ComplexMode)
{
    UINT8   iComplexCount=0,iValidComplex=0;

    for (iComplexCount = 0; iComplexCount < 8; iComplexCount++)
    {
        /* code */
        if((ComplexMode & (1 << iComplexCount)) == (1 << iComplexCount))
            iValidComplex++;
    }
    
    if((iValidComplex >PWD_COMPLEX_MODE_COUNT))
        return TRUE;
    
    return FALSE;
}
//Mizl-20220317-SGMTB#0143674-PasswordComplexMode ->

BOOLEAN CheckPasswordComplexityLocal(CHAR16*Password,UINTN PwdLength)
{
    BOOLEAN AuthenticateStatus = FALSE;
    // UINTN   Upper=0,Lower=0,Number=0,Other=0,TypeNum=0;//Mizl-20220317-SGMTB#0143674-PasswordComplexMode>
    UINT16  Index=0;
    PWD_COMPLEX_MODE    PasswordComplexMode;//Mizl-20220317-SGMTB#0143674-PasswordComplexMode>

    if ( Password == NULL || PwdLength == 0)
        return TRUE;
    
    PasswordComplexMode.Value=0;//Mizl-20220317-SGMTB#0143674-PasswordComplexMode>

    for(Index=0;Index < PwdLength;Index++){
        if((Password[Index]>=L'0')&&(Password[Index]<=L'9')) PasswordComplexMode.Field.PwdComplex_Number=1;
        else if((Password[Index]>=L'a')&&(Password[Index]<=L'z')) PasswordComplexMode.Field.PwdComplex_Lower=1;
        else if((Password[Index]>=L'A')&&(Password[Index]<=L'Z')) PasswordComplexMode.Field.PwdComplex_Uper=1;
        else PasswordComplexMode.Field.PwdComplex_Other=1;
    }
    //<xuyj1-20180829-add password complex and simple select+>
    if(CheckPwdComplex(PasswordComplexMode.Value)){
         AuthenticateStatus = TRUE;
     }else{
         AuthenticateStatus = FALSE;
     }
     
     if(IsPasswordSupportNonCaseSensitive ()){
         if((PasswordComplexMode.Field.PwdComplex_Uper || PasswordComplexMode.Field.PwdComplex_Lower) && PasswordComplexMode.Field.PwdComplex_Number && PasswordComplexMode.Field.PwdComplex_Other){
             AuthenticateStatus = TRUE;
         }
     }
     //<xuyj1-20180829-add password complex and simple select->
    return AuthenticateStatus;
}


#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)
//<Wangbn001-20191029 BIOS password policy, password windows freeze function+>
VOID OemReportInBox(
        UINTN PasswordLength,
        CHAR16 *OrgBoxStr,
        UINTN CurrXPos,
        UINTN CurrYPos,
        BOOLEAN bWaitForReturn,
        BOOLEAN bDisplayOff
        )
{
  CHAR16 *BoxStr=NULL;
  EFI_INPUT_KEY Key;
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR16 *TempStr=NULL;
  
  
  //Draw Box
  //EIP-70175 Set Password ReportInbox color
  //DrawBox( CurrXPos, CurrYPos,  PasswordLength, 1, EFI_BACKGROUND_BLUE | EFI_WHITE );
  DrawBox( CurrXPos, CurrYPos,  PasswordLength, 1, GetPasswordReportInboxcolor());
  
  TempStr=EfiLibAllocateZeroPool((PasswordLength+1)*sizeof(CHAR16));
  MemFillUINT16Buffer(TempStr, PasswordLength, L' ');
  if(PasswordLength)
      TempStr[PasswordLength] = 0x0;
  
  DrawString( CurrXPos, CurrYPos, TempStr);
  
  //Draw Box string
  //Draw Box string
  if(OrgBoxStr != NULL)
  {
      BoxStr = SkipEscCode(OrgBoxStr);

      DrawString( CurrXPos, CurrYPos, BoxStr);
  }
   
  SetPwdKeyboardLayout();//EIP-88912
  TSEStringReadLoopEntryHook();

  FlushLines( CurrYPos - 2, CurrYPos + 2 );
  MouseStop();
  DoRealFlushLines();
  MouseRefresh();

  if(bWaitForReturn || bDisplayOff)
  {

    Key.UnicodeChar = 0;
    if(bWaitForReturn){
      do
      {
        gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
      } while( Key.UnicodeChar != CHAR_CARRIAGE_RETURN );
    }
    if(bDisplayOff){
      DrawString( CurrXPos, CurrYPos, TempStr);
    }

  }

  TSEStringReadLoopExitHook();
  ResetPwdKeyboardLayout();//EIP-88912

  MemFreePointer((VOID **)&BoxStr);
  MemFreePointer((VOID **)&TempStr);
}

VOID ApplyOemPasswordPolicy(
    UINTN TotalRetries,
    UINTN *NoOfRetries,  
    UINTN PasswordLockTime,
    UINTN PasswordLength,
    UINTN CurrXPos,
    UINTN CurrYPos
)
{
    CHAR16 tString[100];
    volatile BOOLEAN tTimeOut = FALSE;
    UINTN TimeOutValue=0;
    EFI_EVENT timer;
    EFI_STATUS Status;
    EFI_TPL CurrentTpl = 0;
    UINTN NoOfMinutes=0;
    UINTN NoOfSeconds=0;
    EFI_INPUT_KEY Key;
    UINTN  nCount = 0;
    CHAR16 *wsPswLockTime = NULL;
    CHAR16 *wsPswErrAttempts = NULL;

    if (1 == *NoOfRetries){
        wsPswLockTime = HiiGetString( gHiiHandle, STRING_TOKEN(STR_OEM_PSW_LOCK_TIME) );

        SetPasswordLock(1,(UINT16)PasswordLockTime);

        CurrentTpl = gBS->RaiseTPL( TPL_HIGH_LEVEL ); //we store the current TPL
        gBS->RestoreTPL( TPL_APPLICATION); //setting the TPL level as application Level TPL
        while(PasswordLockTime){//SGPZT23195#PasswdPolicy(Timeout-Count)

            NoOfMinutes = PasswordLockTime/60;
            NoOfSeconds = PasswordLockTime%60;

            TimeOutValue = 1;
            Swprintf(tString, wsPswLockTime, NoOfMinutes, NoOfSeconds);

            OemReportInBox( PasswordLength, tString, CurrXPos, CurrYPos, FALSE,FALSE);

            if(TimeOutValue)
            {
                timer = NULL;
                Status = TimerCreateTimer( &timer, (EFI_EVENT_NOTIFY)SetPwdTimeOut, (VOID *)&tTimeOut, TimerRelative, TimeOutValue * TIMER_ONE_SECOND, EFI_TPL_NOTIFY );
            }
            
            do{               
              gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
            }while ( ! tTimeOut );
     
            if(TimeOutValue)
                TimerStopTimer( &timer );

            PasswordLockTime--;
            
            tTimeOut = FALSE;
            
        };
        //SGPZT23195#PasswdPolicy(Timeout-Count)
        
        gBS->RaiseTPL( TPL_HIGH_LEVEL ); // Raising the TPL before reverting back as TPL may be changes in between
        gBS->RestoreTPL( CurrentTpl );
        
        //SetPasswordLock(0,0);SGPZT23195#PasswdPolicy(Timeout-Count)
        OemReportInBox( PasswordLength, tString, CurrXPos, CurrYPos, FALSE,TRUE);
        //*NoOfRetries = TotalRetries + 1;
    }else{
        wsPswErrAttempts = HiiGetString( gHiiHandle, STRING_TOKEN(STR_OEM_ERROR_PSWD_ATTEMPTS) );
        Swprintf(tString, wsPswErrAttempts, (*NoOfRetries)-1);
/*
        if(*NoOfRetries > 2)
            Swprintf(tString, L"Invalid Password,%d attempts left", (*NoOfRetries)-1);
        else
            Swprintf(tString, L"Invalid Password,1 attempt left");
*/
//<Wangbn003-20191029 Fix can't reset system by Ctrl+Alt+Del if wrong password entered when check password during POST+>
        CurrentTpl = gBS->RaiseTPL( TPL_HIGH_LEVEL ); //we store the current TPL
        gBS->RestoreTPL( TPL_APPLICATION); //setting the TPL level as application Level TPL
//<Wangbn003-20191029 Fix can't reset system by Ctrl+Alt+Del if wrong password entered when check password during POST->
        OemReportInBox( PasswordLength, tString, CurrXPos, CurrYPos, TRUE,TRUE);
//<Wangbn003-20191029 Fix can't reset system by Ctrl+Alt+Del if wrong password entered when check password during POST+>
        gBS->RaiseTPL( TPL_HIGH_LEVEL ); // Raising the TPL before reverting back as TPL may be changes in between
        gBS->RestoreTPL( CurrentTpl );
//<Wangbn003-20191029 Fix can't reset system by Ctrl+Alt+Del if wrong password entered when check password during POST->
    }
    if(wsPswLockTime != NULL) MemFreePointer((VOID**)&wsPswLockTime);
    if(wsPswErrAttempts != NULL) MemFreePointer((VOID**)&wsPswErrAttempts);
}

//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   BCDToDec
//
// Description: Converts a BCD value to a Decimal value.
//
// Input:       
//      IN UINT8 BCD -- BCD value
//
// Output:      
//      UINT8 - decimal value
//
// Notes:       
//      Only for 2 digit BCD.
//
//----------------------------------------------------------------------------
//<AMI_PHDR_END>
UINT8 BCDToDec(UINT8 BCD)
{
    UINT8 FirstDigit = BCD & 0xf;
  UINT8 SecondDigit = BCD >> 4;

    return SecondDigit * 10  + FirstDigit;
}


//<AMI_PHDR_START>
//----------------------------------------------------------------------------
// Procedure:   ReadRtcIndex
//
// Description: Read the RTC value at the given Index.
//
// Input:       Index       RTC Index
//
// Output:      RTC Value read from the provided Index
//
// Notes:       Here is the control flow of this function:
//              1. Read port 0x70 (RTC Index Register) to get bit 7.
//                  Bit 7 is the NMI bit-it should not be changed.
//              2. Output 0x70 with the Index and NMI bit setting.
//              3. Read 0x71 for Data. Getting Dec when appropriate.
//              4. Return the Data.
//----------------------------------------------------------------------------
//<AMI_PHDR_END>

UINT8 ReadRtcIndex(IN UINT8 Index)
{
    UINT8 volatile Value=0;

    do {
        IoWrite8(CMOS_BANK0_INDEX, 0x0A | 0x80);
        Value = IoRead8(CMOS_BANK0_DATA);   
    } while (Value & 0x80); 

    IoWrite8(CMOS_BANK0_INDEX, Index | 0x80);

    // Read register
    Value = IoRead8(CMOS_BANK0_DATA);               
    if (Index <= 9) { 
        Value = BCDToDec(Value);  
    }

    return (UINT8)Value;
}

BOOLEAN CheckPasswordLock(UINTN *LeftLockTime)
{
  EFI_STATUS              Status = EFI_SUCCESS;
  EFI_GUID                mAmiTseOemPortingVar2Guid =  OEM_PORT_VARIABEL2;
  PASSWORD_LOCK_DATA      RecodeSetupData;
  UINTN                   RecodeSetupDataSize;
  UINT8                   TimeHour;
  UINT8                   TimeMinute;
  UINT8                   TimeSecond;
  UINTN                   TotalTime;
  UINT8                   rTimeHour;
  UINT8                   rTimeMinute;
  UINT8                   rTimeSecond;
  UINTN                   rTotalTime;
  UINTN                   Temp;
  BOOLEAN                 LockStatus=FALSE;

  
  RecodeSetupDataSize       = sizeof (PASSWORD_LOCK_DATA);
  Status           = gRT->GetVariable ( L"OemPwdLockVar", &mAmiTseOemPortingVar2Guid, NULL, &RecodeSetupDataSize, &RecodeSetupData);
  if (!EFI_ERROR(Status)){
    if(RecodeSetupData.PasswordLock == 1){
      
      TimeHour = ReadRtcIndex(4);
      TimeMinute = ReadRtcIndex(2);
      TimeSecond = ReadRtcIndex(0);
      TotalTime = TimeHour*3600+TimeMinute*60+TimeSecond;
      
      rTimeHour = RecodeSetupData.Hour;
      rTimeMinute = RecodeSetupData.Minute;
      rTimeSecond = RecodeSetupData.Second;
      rTotalTime = rTimeHour*3600+rTimeMinute*60+rTimeSecond;
      
      if(TotalTime<rTotalTime)
        TotalTime +=86400;
      
      Temp = TotalTime - rTotalTime;

      if(Temp < RecodeSetupData.PLockTime){
        *LeftLockTime = RecodeSetupData.PLockTime - Temp;
      }
      LockStatus = TRUE;//SGPZT23195#PasswdPolicy(Timeout-Count)
    }
  }
  return LockStatus;
}

EFI_STATUS SetPasswordLock(UINT8 LockData,UINT16 LockTime)
{
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_GUID                  mAmiTseOemPortingVar2Guid =  OEM_PORT_VARIABEL2;
  PASSWORD_LOCK_DATA        RecodeSetupData;
  UINTN                     RecodeSetupDataSize;
  BOOLEAN                   LockStatus=FALSE;
  UINT32                    Attributes;
  
  if((LockData == 1) && (LockTime == 0))
    return Status;

  MemSet( &RecodeSetupData, sizeof (PASSWORD_LOCK_DATA), 0);
  RecodeSetupDataSize       = sizeof (PASSWORD_LOCK_DATA);
  Status = gRT->GetVariable ( L"OemPwdLockVar", &mAmiTseOemPortingVar2Guid, &Attributes, &RecodeSetupDataSize, &RecodeSetupData);
  if (EFI_ERROR(Status)) {
      Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;  
  }
  
  RecodeSetupData.PasswordLock = LockData;
  RecodeSetupData.PLockTime = LockTime;
  if(LockData == 1){
    RecodeSetupData.Hour = ReadRtcIndex(4);
    RecodeSetupData.Minute = ReadRtcIndex(2);
    RecodeSetupData.Second = ReadRtcIndex(0);
  }

  Status = gRT->SetVariable( L"OemPwdLockVar", \
                    &mAmiTseOemPortingVar2Guid, \
                    Attributes, \
                    sizeof (PASSWORD_LOCK_DATA), \
                    &RecodeSetupData);
  
  return Status;
  
}

EFI_STATUS GetPasswordLockTime(
        UINTN *LockTime)
{
    EFI_STATUS                Status = EFI_SUCCESS;
    EFI_GUID                  mAmiTseOemPortingVar2Guid =  OEM_PORT_VARIABEL2;
    PASSWORD_LOCK_DATA        RecodeSetupData;
    UINTN                     RecodeSetupDataSize;
    BOOLEAN                   LockStatus=FALSE;
    UINT32                    Attributes;

    MemSet( &RecodeSetupData, sizeof (PASSWORD_LOCK_DATA), 0);
    RecodeSetupDataSize       = sizeof (PASSWORD_LOCK_DATA);
    Status = gRT->GetVariable ( L"OemPwdLockVar", &mAmiTseOemPortingVar2Guid, &Attributes, &RecodeSetupDataSize, &RecodeSetupData);
    if (EFI_ERROR(Status)) {
        *LockTime = 0;
        return Status;  
    }
    *LockTime = RecodeSetupData.PLockTime;
     
    return Status;
}

EFI_STATUS GetFixedPasswordLockTime(
        UINTN *LockTime)
{
    EFI_STATUS                Status = EFI_SUCCESS;
    SETUP_DATA                SetupData;
    UINTN                     SetupDataSize = sizeof(SETUP_DATA);
    EFI_GUID                  gEfiSetupGuid = SETUP_GUID;

    Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
    if (EFI_ERROR(Status)) {
        //SGPZT23195#PasswdPolicy(Timeout-Count)+>
        //*LockTime = PASSWORD_LOCK_TIME * 60;
        *LockTime = PASSWORD_LOCK_TIME ;
        //SGPZT23195#PasswdPolicy(Timeout-Count)->

        return Status;  
    }
    //SGPZT23195#PasswdPolicy(Timeout-Count)+>
    //*LockTime = SetupData.PasswordLockTime * 60;
    *LockTime = SetupData.PasswordLockTime;
    //SGPZT23195#PasswdPolicy(Timeout-Count)->

    return Status;
}
//<lvych001-20180511 Add bios password lock function.->
#endif //defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)

//<lvych007-20160415 BIOS screen lock function +>
#if defined(BIOS_SETUP_SCREEN_LOCK_SUPPORT)&& (BIOS_SETUP_SCREEN_LOCK_SUPPORT == 1)

VOID OemBeforeLegacyBootLaunch(VOID)
{
  StopScreenLockTimer();
}

VOID OemAfterLegacyBootLaunch(VOID)
{
  StartScreenLockTimer();
}

VOID OemBeforeEfiBootLaunch(VOID)
{
  StopScreenLockTimer();
}

VOID OemAfterEfiBootLaunch(VOID)
{
  StartScreenLockTimer();
}

CONTROL_ACTION DetectKeysPress(AMI_EFI_KEY_DATA key)
{
  if(!(((key.KeyState.KeyShiftState & (~SHIFT_STATE_VALID))==0) && (key.Key.ScanCode==0) && (key.Key.UnicodeChar==0))){
    gScreenLockData.LockCount = 0;
  }

  return ControlActionUnknown;
}

//----------------------------------------------------------------------------
// Procedure:   IsPasswordChange
// Input:       VOID
//
// Output:      VOID
//
//----------------------------------------------------------------------------
BOOLEAN IsPasswordChange(AMITSESETUP *TseVar)
{
    EFI_STATUS            Status = EFI_SUCCESS;
  EFI_GUID        SetupGuid = AMITSESETUP_GUID;
  EFI_GUID        gTseTransferGuid = TSE_TRANSFER_GUID;  
    AMITSESETUP       *AMITSEData = NULL;
    UINTN           AMITSEDataSize=0;
    BOOLEAN         UserUpdateFlag=FALSE, AdminUpdateFlag=FALSE;

    if(pTseSetuptransferInterface == NULL)
    {
        Status = pBS->LocateProtocol(&gTseTransferGuid, NULL, &pTseSetuptransferInterface);
        if(EFI_ERROR(Status))
        {
            return FALSE;
        }
    }

  pTseSetuptransferInterface->VarGetBuffer(L"AMITSESetup",SetupGuid,(UINT8**)&AMITSEData,&AMITSEDataSize);
    
  if ( EfiCompareMem( AMITSEData->UserPassword, TseVar->UserPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16) ) )
  {
      UserUpdateFlag = TRUE;
  }
    
  if ( EfiCompareMem( AMITSEData->AdminPassword, TseVar->AdminPassword, SETUP_PASSWORD_LENGTH * sizeof(CHAR16) ) )
  {
      AdminUpdateFlag = TRUE;
  }
  
  if(UserUpdateFlag||AdminUpdateFlag)
  {
      return TRUE;
  }
  
    return FALSE;
}

//<lvych002-20180928 Revised access level is always unchange when enter user password unlock screen in bios setup.+>
VOID UpdateSysAccessStatus( UINT32 PasswordType )
{
  EFI_STATUS      Status = EFI_SUCCESS;
  EFI_GUID        sysAccessGuid = SYSTEM_ACCESS_GUID;
  EFI_GUID        gTseTransferGuid = TSE_TRANSFER_GUID;
  SYSTEM_ACCESS   *sysAccessData;
  UINTN           sysAccessSize = sizeof(SYSTEM_ACCESS);


  if(pTseSetuptransferInterface == NULL)
  {
      Status = pBS->LocateProtocol(&gTseTransferGuid, NULL, &pTseSetuptransferInterface);
      if(EFI_ERROR(Status))
        return;
  }

  pTseSetuptransferInterface->VarGetBuffer(L"SystemAccess",sysAccessGuid,(UINT8**)&sysAccessData,&sysAccessSize);

  if ( PasswordType == AMI_PASSWORD_USER )
    sysAccessData->Access = SYSTEM_PASSWORD_USER;
  else
    sysAccessData->Access = SYSTEM_PASSWORD_ADMIN;

}
//<lvych002-20180928 Revised access level is always unchange when enter user password unlock screen in bios setup.->

//----------------------------------------------------------------------------
// Procedure: ProcessScreenLock
// Input:   VOID
//
// Output:    VOID
//
//----------------------------------------------------------------------------
extern UINT8  PasswordEventFlag; //<bieyb-20190112  Fix watchdog will be reopened after back from lock screen>

VOID ProcessScreenLock(VOID)
{
  EFI_STATUS      Status = EFI_SUCCESS;
  UINT32        PasswordType = 0;
  UINTN         NoOfRetries;
  UINTN         Index;
  EFI_INPUT_KEY   Key;
//<lvych006-20160310 Administrator and User Password Policy +>
#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)
  NoOfRetries = GetPasswordRetries();
//<lvych006-20160310 Administrator and User Password Policy ->
#endif //<lvych006-20160310 Administrator and User Password Policy>
  if(gScreenLockData.PasswordInstalled)
  { 
    if(IsPasswordChange(&gScreenLockData.TSEVar) == FALSE){
      ClearScreen( EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY );   //<witty111-20160416 +> 
      Status = InitEsaTseInterfaces ();
      if (!EFI_ERROR (Status))
      {
        PasswordType = gEsaInterfaceForTSE->CheckSystemPassword (AMI_PASSWORD_NONE, &NoOfRetries, NULL);
        gPasswordType = PasswordType; // setting the gPasswordType in Esa Boot Only.
        MouseInit();
      }
      else{
        PasswordEventFlag = 1; //<bieyb-20190112  Fix watchdog will be reopened after back from lock screen>
        PasswordType = CheckSystemPassword( AMI_PASSWORD_NONE, &NoOfRetries, NULL);
        PasswordEventFlag = 0; //<bieyb-20190112  Fix watchdog will be reopened after back from lock screen>
      }
      UpdateSysAccessStatus(PasswordType);//<lvych002-20180928 Revised access level is always unchange when enter user password unlock screen in bios setup.>
      if(AMI_PASSWORD_NONE == PasswordType)
      {
        while(1)
        {
          //Patch
          //Ctl-Alt-Del is not recognized by core unless a
          //ReadKeyStroke is issued
          gBS->WaitForEvent( 1, gST->ConIn->WaitForKey, &Index );
          gST->ConIn->ReadKeyStroke( gST->ConIn, &Key );
        }
      }
      RefreshSetup();
    }
  }

}

VOID CheckForScreenLock (EFI_EVENT Event, VOID *Context)
{
  EFI_STATUS Status;
  
  TimerStopTimer( &gScreenLockData.LockEvent );
  gScreenLockData.LockCount++;
  if(gScreenLockData.LockTime == 0) return; //<lvych00220160602>
  if(gScreenLockData.LockCount > gScreenLockData.LockTime){
    gScreenLockData.LockCount = 0;
        ProcessScreenLock();
  }
  Status = StartScreenLockTimer();
}

EFI_STATUS StartScreenLockTimer(){

  EFI_STATUS Status = EFI_SUCCESS;

  if(gScreenLockData.LockInit !=1)
    return EFI_UNSUPPORTED;
  
  if(gScreenLockData.LockEvent != NULL)
    return Status; // timer already created.
  Status = TimerCreateTimer( &gScreenLockData.LockEvent, CheckForScreenLock, NULL, TimerRelative, ONE_SECOND_TIMER, TPL_CALLBACK );
  // Signal it once immediately.
  //if (!EFI_ERROR(Status)) gBS->SignalEvent( gScreenLock );
  return Status;
  
}

EFI_STATUS StopScreenLockTimer(){

  EFI_STATUS Status = EFI_SUCCESS;

  if(gScreenLockData.LockInit !=1)
    return EFI_UNSUPPORTED;
  gScreenLockData.LockCount = 0;
  Status = TimerStopTimer( &gScreenLockData.LockEvent );
  return Status;
  
}

VOID InstallScreenLockHandlers( VOID )
{
  EFI_STATUS      Status;
  SETUP_DATA          SetupData;
  UINTN               SetupDataSize;
  EFI_GUID      gEfiSetupGuid = SETUP_GUID;
  EFI_GUID        AmiTseSetupguid = AMITSESETUP_GUID;
    AMITSESETUP     TSEVar;
  UINTN         TseSize = 0;

  SETUP_DEBUG_TSE("InstallScreenLockHandlers.\n");
  //Init
  SetupDataSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable ( L"Setup", &gEfiSetupGuid, NULL, &SetupDataSize, &SetupData);
  if (!EFI_ERROR(Status))
  { 
    gScreenLockData.LockTime = SetupData.ScreenLockTime * 60;
  }
  else
  {
    gScreenLockData.LockTime = 0;
  }

  gScreenLockData.PasswordInstalled = PasswordCheckInstalled();
  MemSet( &gScreenLockData.TSEVar.AdminPassword, ((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16), 0 );
  MemSet( &gScreenLockData.TSEVar.UserPassword, ((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16), 0 );
  TseSize = sizeof (AMITSESETUP); 
    Status = gRT->GetVariable ( L"AMITSESetup", \
                                &AmiTseSetupguid, \
                                NULL, \
                                &TseSize, \
                                &TSEVar );
  if(!EFI_ERROR(Status))
  {
    if( gScreenLockData.PasswordInstalled & AMI_PASSWORD_ADMIN )
    {
      MemCpy(&gScreenLockData.TSEVar.AdminPassword,&TSEVar.AdminPassword,((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16));
    }
    
    if( gScreenLockData.PasswordInstalled & AMI_PASSWORD_USER )
    {
      MemCpy(&gScreenLockData.TSEVar.UserPassword,&TSEVar.UserPassword,((UINT8)SETUP_PASSWORD_LENGTH) * sizeof(CHAR16));
    }
  }
  
  if(gScreenLockData.PasswordInstalled && gScreenLockData.LockTime)
  {
    gScreenLockData.LockInit = 1; //<lvych00220160602+>   
    gScreenLockData.LockCount = 0; //<lvych00220160602->    
    gScreenLockData.LockEvent = NULL;
    StartScreenLockTimer();
  }
}
#endif
//<lvych007-20160415 BIOS screen lock function >

#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)
#if defined(OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS) && (OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS)
VOID ProcessEnterSetupForTsePswPolicy( VOID )
{
    MemSet(gTmpAdminPlaintextPsw, sizeof(gTmpAdminPlaintextPsw), 0);
    MemSet(gTmpUserPlaintextPsw, sizeof(gTmpUserPlaintextPsw), 0);
}

//free the allocated memory
EFI_STATUS  Oem_FreePswRecArray(IN OUT OEM_TSE_PSW_RECORD_HEADER **pPswRecArray, IN UINT32 Count)
{
    if(pPswRecArray == NULL || Count == 0)
    {
        return EFI_SUCCESS;
    }
    while(Count--)
    {
        if(*pPswRecArray != NULL)
        {
            MemFreePointer((VOID**)pPswRecArray++);
        }
    }
    return EFI_SUCCESS;
}

#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 0)
//Get the password hash datas (OEM_TSE_PSW_RECORD_HEADER) from NVRAM
//Input:    PswType - AMI_PASSWORD_ADMIN, AMI_PASSWORD_USER
//          pPswRecArray - the array to save the pointer that point to hash data (OEM_TSE_PSW_RECORD_HEADER)
//          pPswCount - the array size
//Output:   pPswRecArray - the pointer that point to hash data (OEM_TSE_PSW_RECORD_HEADER)
//          pPswCount - number of pointer of hash data (OEM_TSE_PSW_RECORD_HEADER) stored in pPswRecArray
//                      the output number is <= input array size
//return:   EFI_SUCCESS
//          EFI_INVALID_PARAMETER - one or more input parameter is invalid
EFI_STATUS Oem_GetPreviousHashedPswArrayNvram(IN UINT32 PswType, IN OUT OEM_TSE_PSW_RECORD_HEADER **pPswRecArray, IN OUT UINT32 *pPswCount)
{
    UINTN                   Size = 0;
    OEM_TSE_PSW_INFO      *pTsePswInfo = NULL;
    OEM_TSE_PSW_RECORD_HEADER    *pTsePswRec = NULL;
    UINT32                  mPswCount = 0;
    UINT8                   VarNumber = 0;
    CHAR16                  VarName[50] = {0};
    UINT32                  Index;
    
//check input parameters
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(pPswCount == NULL || *pPswCount == 0 || pPswRecArray == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }

//Step1, get stored data information variable, "OEMTSEPSWINFOA" for administor, "OEMTSEPSWINFOU" for user
    pTsePswInfo = (OEM_TSE_PSW_INFO*)VarGetNvramName(PswType == AMI_PASSWORD_ADMIN?L"OEMTSEPSWINFOA":L"OEMTSEPSWINFOU", &gOemTsePswVarGuid, NULL, &Size);
    if(pTsePswInfo == NULL)
    {
        //not found the information variable, that means, no any password installed yet
        return EFI_NOT_FOUND;
    }
//Step2, Check the information variable is valid or not, and wether any password installed or not.
    if(Size != sizeof(OEM_TSE_PSW_INFO) || pTsePswInfo->PswCount == 0)
    {
        MemFreePointer( (VOID *)&pTsePswInfo);
        return EFI_NOT_FOUND;
    }
//Step3, Get hash data record from last to first, the max number of the record is limit to minimum of the record count and array size.
    for(Index = pTsePswInfo->PswCount; (Index > 0) && (mPswCount < *pPswCount);Index--)
    {
        VarNumber = (pTsePswInfo->FirstNumber + Index - 1)%OEM_TSE_PSW_NUMBER_HASH_MAX;
        Size = 0;
        Swprintf(VarName, PswType == AMI_PASSWORD_ADMIN?L"OEMTSEPSWA%d":L"OEMTSEPSWU%d", VarNumber);
        pTsePswRec = (OEM_TSE_PSW_RECORD_HEADER*)VarGetNvramName(VarName, &gOemTsePswVarGuid, NULL, &Size);
        if(pTsePswRec != NULL)    //success to get nvram variable
        {
            //the nvram variable size must >= NVRAM header size(sizeof(OEM_TSE_PSW_NVRAM))
            //also must >= header(sizeof(OEM_TSE_PSW_NVRAM)) + data size(PswDataCount * sizeof(OEM_TSE_PSW_DATA))
            if(  Size >= sizeof(OEM_TSE_PSW_RECORD_HEADER)
              && Size == (((UINTN)pTsePswRec->PswDataCount) * sizeof(OEM_TSE_PSW_DATA)  + sizeof(OEM_TSE_PSW_RECORD_HEADER))
              )
            {
                //valid record found, save the memory location(pointer) into array.
                pPswRecArray[mPswCount] = pTsePswRec;
                mPswCount++;
            }
            else
            {
                MemFreePointer((VOID**)&pTsePswRec);
            }

        }
    }
//step4, finished read the hash data record, free the allocated temp memory
    MemFreePointer( (VOID **)&pTsePswInfo);
    if(mPswCount == 0)
    {
        //no any record found
        return EFI_NOT_FOUND;
    }
    *pPswCount = mPswCount;
    return EFI_SUCCESS;
}

//Encode the specified password and save into NVRAM
//Input:
//          PswType - AMI_PASSWORD_ADMIN, AMI_PASSWORD_USER
//          PlaintextPsw - current plaintext password
//          PswLen - the length of current plaintext password string, may equal to EfiStrLen(PlaintextPsw)
//Output:
//          None
//Return:
//          EFI_SUCCESS - Successfully save the data
//          EFI_INVALID_PARAMETER
EFI_STATUS  Oem_SaveTsePswNvram(UINT32 PswType, CHAR16 *PlaintextPsw, UINTN PswLen)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINTN                   Size = 0;
    UINT16                  mEncodedData[SETUP_PASSWORD_LENGTH] = {0};
    //OEM_TSE_PSW_DATA      mPswData;
    OEM_TSE_PSW_INFO      *pTsePswInfo = NULL;
    OEM_TSE_PSW_RECORD_HEADER    *pTsePswRec = NULL;
    OEM_TSE_PSW_DATA      *pTsePswData = NULL;
    UINT8                   VarNumber = 0;
    CHAR16                  VarName[50] = {0};
    UINTN                   Index;
    
//check input parameters
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(*PlaintextPsw == 0 || PswLen == 0)
    {
        return EFI_SUCCESS;
    }
    PswLen = MIN(PswLen, SETUP_PASSWORD_LENGTH);
//Get current password data information variable
    pTsePswInfo = (OEM_TSE_PSW_INFO*)VarGetNvramName(PswType == AMI_PASSWORD_ADMIN?L"OEMTSEPSWINFOA":L"OEMTSEPSWINFOU", &gOemTsePswVarGuid, NULL, &Size);
    if(pTsePswInfo != NULL && Size != sizeof(OEM_TSE_PSW_INFO))
    {
        //variable is not valid, free current and create new later
        MemFreePointer((VOID**)&pTsePswInfo);
    }
    if(pTsePswInfo == NULL)
    {
        //current information variable not found or invalid, create new
        pTsePswInfo = EfiLibAllocateZeroPool( sizeof(OEM_TSE_PSW_INFO) );
        if(pTsePswInfo == NULL)
        {
            return EFI_OUT_OF_RESOURCES;
        }
    }
    
    //encode current whole password, and check if there is same password data exist in nvram or not
    //MemSet(&mPswData, sizeof(mPswData), 0);
    //MemCpy(mPswData.PswData, PlaintextPsw, PswLen * sizeof(CHAR16));
    //PasswordEncodeHook( mPswData.PswData, sizeof(mPswData.PswData));
    MemSet(mEncodedData, sizeof(mEncodedData), 0);
    MemCpy(mEncodedData, PlaintextPsw, PswLen * sizeof(CHAR16));
    PasswordEncodeHook(mEncodedData, sizeof(mEncodedData));
    
    if(pTsePswInfo->PswCount)
    {
        //check with all previous password data
        for(Index = 0; Index < pTsePswInfo->PswCount; Index++)
        {
            VarNumber = (pTsePswInfo->FirstNumber + Index)%OEM_TSE_PSW_NUMBER_HASH_MAX;
            Swprintf(VarName, PswType == AMI_PASSWORD_ADMIN?L"OEMTSEPSWA%d":L"OEMTSEPSWU%d", VarNumber);
            Size = 0;
            pTsePswRec = (OEM_TSE_PSW_RECORD_HEADER*)VarGetNvramName(VarName, &gOemTsePswVarGuid, NULL, &Size);
            if(pTsePswRec != NULL)    //success to get nvram variable
            {
                //the nvram variable size must >= NVRAM header size(sizeof(OEM_TSE_PSW_NVRAM))
                //also must >= header(sizeof(OEM_TSE_PSW_NVRAM)) + data size(PswDataCount * sizeof(OEM_TSE_PSW_DATA))
                if( Size >= sizeof(OEM_TSE_PSW_RECORD_HEADER) && Size >= (pTsePswRec->PswDataCount * sizeof(OEM_TSE_PSW_DATA)  + sizeof(OEM_TSE_PSW_RECORD_HEADER)))
                {
                    //valid record found
                    if(MemCmp(mEncodedData, pTsePswRec->FullPswData.PswData, sizeof(OEM_TSE_PSW_DATA)) == 0)
                    {
                        //already have same password data in NVRAM, skip save it and return success.
                        MemFreePointer((VOID**)&pTsePswInfo);
                        return EFI_SUCCESS;
                    }
                }
                MemFreePointer((VOID**)&pTsePswRec);

            }
        }
    }
    
//allocate memory for new record
    Size = (PswLen > OEM_TSE_PSW_HASH_CHARS_COUNT)?(PswLen - OEM_TSE_PSW_HASH_CHARS_COUNT + 1):1;
    Size = Size * sizeof(OEM_TSE_PSW_DATA);
    Size += sizeof(OEM_TSE_PSW_RECORD_HEADER);

    pTsePswRec = NULL;
    pTsePswRec = EfiLibAllocateZeroPool( Size );
    if(pTsePswRec == NULL)
    {
        MemFreePointer((VOID**)&pTsePswInfo);
        return EFI_OUT_OF_RESOURCES;
    }

    pTsePswRec->PswDataCount = (UINT8)((PswLen > OEM_TSE_PSW_HASH_CHARS_COUNT)?(PswLen - OEM_TSE_PSW_HASH_CHARS_COUNT + 1):1);
    MemCpy(pTsePswRec->FullPswData.PswData, mEncodedData, sizeof(OEM_TSE_PSW_DATA));    //save current encoded whole password.

//save hash data into record memory
    pTsePswData = (OEM_TSE_PSW_DATA*)((UINTN)pTsePswRec + sizeof(OEM_TSE_PSW_RECORD_HEADER));
    for(Index = 0; Index + OEM_TSE_PSW_HASH_CHARS_COUNT <= MAX(PswLen, OEM_TSE_PSW_HASH_CHARS_COUNT); Index++, pTsePswData++)
    {
        //encode the password four chars by four chars and save into the record memory
        MemSet(mEncodedData, sizeof(mEncodedData), 0);
        MemCpy(mEncodedData, PlaintextPsw + Index, (MIN(PswLen-Index, OEM_TSE_PSW_HASH_CHARS_COUNT))*sizeof(UINT16));
        PasswordEncodeHook( mEncodedData, sizeof(mEncodedData));
        MemCpy(pTsePswData->PswData, mEncodedData, sizeof(OEM_TSE_PSW_DATA));
    }
//write record into NVRAM
    VarNumber = (pTsePswInfo->FirstNumber + pTsePswInfo->PswCount)%OEM_TSE_PSW_NUMBER_HASH_MAX;
    Swprintf(VarName, PswType == AMI_PASSWORD_ADMIN?L"OEMTSEPSWA%d":L"OEMTSEPSWU%d", VarNumber);
    Status = VarSetNvramName(VarName, &gOemTsePswVarGuid, EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, pTsePswRec, Size);
    if(!EFI_ERROR(Status))
    {
        //adjust password count and first variable number.
        pTsePswInfo->PswCount++;
        if(pTsePswInfo->PswCount > OEM_TSE_PSW_NUMBER_HASH_MAX)
        {
            pTsePswInfo->PswCount = OEM_TSE_PSW_NUMBER_HASH_MAX;
            pTsePswInfo->FirstNumber = (pTsePswInfo->FirstNumber + 1)%OEM_TSE_PSW_NUMBER_HASH_MAX;
        }
        //save password information variable
        Status = VarSetNvramName( PswType == AMI_PASSWORD_ADMIN?L"OEMTSEPSWINFOA":L"OEMTSEPSWINFOU",
                                &gOemTsePswVarGuid, 
                                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE, 
                                pTsePswInfo,
                                sizeof(OEM_TSE_PSW_INFO)
                                );
    }
    MemFreePointer( (VOID **)&pTsePswInfo);
    MemFreePointer( (VOID**)&pTsePswRec);
    return Status;
}
#endif  //#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 0)

#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 1)

#define OEM_TSE_PSW_FV_EMPTY_PAD  0xFF
#define OEM_TSE_PSW_FV_EMPTY_PAD16    ((OEM_TSE_PSW_FV_EMPTY_PAD << 8) | OEM_TSE_PSW_FV_EMPTY_PAD)
#define OEM_TSE_PSW_FV_EMPTY_PAD32    ((OEM_TSE_PSW_FV_EMPTY_PAD16 << 16) | OEM_TSE_PSW_FV_EMPTY_PAD16)
#define OEM_TSE_PSW_FV_EMPTY_PAD64    ((OEM_TSE_PSW_FV_EMPTY_PAD32 << 32) | OEM_TSE_PSW_FV_EMPTY_PAD32)


#define BLOCK_ADDR(adr) ( (UINTN)(adr) -  ((UINTN)(adr)%FLASH_BLOCK_SIZE) )
#define BLOCK_NUMBER(adr, Size) ( (Size)?(((BLOCK_ADDR((adr)+(Size)-1) - BLOCK_ADDR(adr))/FLASH_BLOCK_SIZE)+1):0 )
#define BLOCK_STARTADDR(adr) BLOCK_ADDR(adr)
#define BLOCK_ENDADDR(adr) (BLOCK_ADDR(adr) + FLASH_BLOCK_SIZE - 1)

//check the buffer if any byte is not equal to Data
//Input:    Buffer - the buffer to check
//          Size - how many bytes in the buffer to check
//          Data - The data to check with buffer
//Output:
//          NULL - all data in buffer is equal to Data
//          not NULL - pointer to the address that is not equal to Data
UINT8* CheckBuffer8(IN UINT8*  Buffer, IN UINTN Size, IN UINT8 Data)
{
    if(Buffer == NULL || Size == 0) 
    {
        return NULL;
    }
    while(Size)
    {
        if(*Buffer != Data)
        {
            return Buffer;
        }
        Buffer++;
        Size--;
    }
    return NULL;
}

//check the buffer if any byte is not equal to Data
//for huge buffer check
//Input:    Buffer - the buffer to check
//          Size - how many bytes in the buffer to check
//          Data - The data to check with buffer
//Output:
//          NULL - all data in buffer is equal to Data
//          not NULL - pointer to the address that is not equal to Data
UINT8* CheckBuffer(UINT8*  Buffer, UINTN Size, UINT8 Data)
{
    UINT8   *pTmp = NULL;
    UINTN   SizeQword;
    UINT64  DataQWord;
    if(Buffer == NULL || Size == 0)
    {
        return NULL;
    }

//check the pre bytes not aligned to UINT64(8 bytes) at the beganing of the buffer
    if(((UINTN)Buffer) & 0x07)
    {
        if((pTmp = CheckBuffer8(Buffer, sizeof(UINT64) - (((UINTN)Buffer) & 0x07), Data)) != NULL)
        {
            return pTmp;
        }
        else
        {
            Buffer += sizeof(UINT64) - (((UINTN)Buffer) & 0x07);
            Size -= sizeof(UINT64) - (((UINTN)Buffer) & 0x07);
        }
    }
    SizeQword = Size & (~0x07);     //How many QWORD to check
    Size = Size & 0x07;

//check the remain bytes aligned to UINT64(8 bytes)
    if(SizeQword)
    {
        DataQWord = ((UINT64)(Data) << 24) | ((UINT64)(Data) << 16) | ((UINT64)(Data) << 8) | (UINT64)(Data);
        DataQWord = LShiftU64(DataQWord, 32) | DataQWord;
        while(SizeQword)
        {
            if(*(UINT64*)(Buffer) != DataQWord)
            {
                return CheckBuffer8(Buffer, sizeof(UINT64), Data);
            }
            Buffer += sizeof(UINT64);
            SizeQword -= sizeof(UINT64);
        }
    }
//check the remain bytes not aligned to UINT64(8 bytes) at the end of the buffer
    return CheckBuffer8(Buffer, Size, Data);
}

//Locate AmiFlashProtocol
//Input:    NONE
//Output:   AMI_FLASH_PROTOCOL * - AmiFlashProtocol
//          NULL - AmiFlashProtocol not located
FLASH_PROTOCOL *GetFlashProtocol()
{
    EFI_STATUS  Status;
    static FLASH_PROTOCOL *mFlash = NULL;
    if(mFlash == NULL)
    {
        Status = pBS->LocateProtocol(&gFlashProtocolGuid, NULL, &mFlash);
        if(EFI_ERROR(Status))
        {
            return NULL;
        }
    }
    return mFlash;
}

//Check the FV descriptor is valid or not
//will check the FV size and Signature
//Input:    pTsePswFvDesc - the FV descriptor
//Output:   NONE
//Return:
//          TRUE - the FV is valid
//          FALSE - The FV is invalid
BOOLEAN Oem_CheckTsePswFv(IN OEM_TSE_PSW_FV_DESC *pTsePswFvDesc)
{
    if(pTsePswFvDesc == NULL)
    {
        return FALSE;
    }
//check if the FV desc is empty or not
    if(CheckBuffer8((UINT8*)pTsePswFvDesc, sizeof(OEM_TSE_PSW_FV_DESC), OEM_TSE_PSW_FV_EMPTY_PAD) == NULL)
    {
        return FALSE;
    }
//check FV size
    if(pTsePswFvDesc->Size != OEM_TSE_PSW_FV_SIZE)
    {
        return FALSE;
    }
//check FV signature (GUID) is valid or not
    if(MemCmp(&gOemTsePswVarGuid, &pTsePswFvDesc->Signature, sizeof(EFI_GUID)))
    {
        return FALSE;
    }
//FV desc check passed, FV desc is valid
    return TRUE;
}

//check the region descriptor(Admin & User) in FV desc is valid or not
//will check the region offset, region size
//caller must ensure the FV desc is valid(use Oem_CheckTsePswFv to check FV desc before invoke Oem_CheckTsePswFvRecRegion)
//Input:    pTsePswFvDesc - the FV descriptor
//Output:   NONE
//Return:   TRUE - region descriptor(Admin & User) in FV desc is valid
//          FALSE - region descriptor(Admin or User) in FV desc is invalid
BOOLEAN Oem_CheckTsePswFvRecRegion(IN OEM_TSE_PSW_FV_DESC *pTsePswFvDesc)
{
    if(pTsePswFvDesc == NULL)
    {
        return FALSE;
    }
//Check Offset and Size is 0xFFFFFFFF or not
    if(pTsePswFvDesc->AdminPswRegionDesc.Offset == OEM_TSE_PSW_FV_EMPTY_PAD32 || pTsePswFvDesc->AdminPswRegionDesc.Size == OEM_TSE_PSW_FV_EMPTY_PAD32)
    {
        return FALSE;
    }
    if(pTsePswFvDesc->UserPswRegionDesc.Offset == OEM_TSE_PSW_FV_EMPTY_PAD32 || pTsePswFvDesc->UserPswRegionDesc.Size == OEM_TSE_PSW_FV_EMPTY_PAD32)
    {
        return FALSE;
    }
//the Offset and size not empty
//Check the region size, must <= FV.size - sizeof(FV desc)
    if((pTsePswFvDesc->AdminPswRegionDesc.Size + pTsePswFvDesc->UserPswRegionDesc.Size) > (pTsePswFvDesc->Size - sizeof(OEM_TSE_PSW_FV_DESC)))
    {
        return FALSE;
    }
//check the admin region offset
    if(pTsePswFvDesc->AdminPswRegionDesc.Offset < sizeof(OEM_TSE_PSW_FV_DESC) || (pTsePswFvDesc->AdminPswRegionDesc.Offset + sizeof(OEM_TSE_PSW_FV_RECORD_HEADER)) > pTsePswFvDesc->Size)
    {
        return FALSE;
    }
//check the admin region size
    if((pTsePswFvDesc->AdminPswRegionDesc.Size + sizeof(OEM_TSE_PSW_FV_DESC)) > pTsePswFvDesc->Size)
    {
        return FALSE;
    }
//check the user region offset
    if(pTsePswFvDesc->UserPswRegionDesc.Offset < sizeof(OEM_TSE_PSW_FV_DESC) || (pTsePswFvDesc->UserPswRegionDesc.Offset + sizeof(OEM_TSE_PSW_FV_RECORD_HEADER)) > pTsePswFvDesc->Size)
    {
        return FALSE;
    }
//check the user region size
    if((pTsePswFvDesc->UserPswRegionDesc.Size + sizeof(OEM_TSE_PSW_FV_DESC)) > pTsePswFvDesc->Size)
    {
        return FALSE;
    }
//check passed
    return TRUE;
}

//check record flag is valid/(==0xFF) or not/deleted (!=0xFF)
//Input:    pTsePswFvRec - the record to check
//Output:   NONE
//Return:   TRUE - record flag is valid
//          FALSE - flag is not valid
BOOLEAN Oem_CheckTsePswFvRecFlag(IN OEM_TSE_PSW_FV_RECORD_HEADER *pTsePswFvRec)
{
    if(pTsePswFvRec == NULL)
    {
        return FALSE;
    }
    if(pTsePswFvRec->Flag != 0xFF)
    {
        return FALSE;
    }
    return TRUE;
}
//check record signature is valid(==0x55aa) or not (!=0x55aa)
//Input:    pTsePswFvRec - the record to check
//Output:   NONE
//Return:   TRUE - record signature is valid
//          FALSE - signature is not valid
BOOLEAN Oem_CheckTsePswFvRecSignature(IN OEM_TSE_PSW_FV_RECORD_HEADER *pTsePswFvRec)
{
    if(pTsePswFvRec == NULL)
    {
        return FALSE;
    }
    if(pTsePswFvRec->Signature != OEM_TSE_PSW_FV_RECORD_SIGNATURE)
    {
        return FALSE;
    }
    return TRUE;
}

//check record data count is valid(!=0xFF) or not (==0xFF)
//Input:    pTsePswFvRec - the record to check
//Output:   NONE
//Return:   TRUE - record data count is valid
//          FALSE - data count is not valid
BOOLEAN Oem_CheckTsePswFvRecDataCount(OEM_TSE_PSW_FV_RECORD_HEADER *pTsePswFvRec)
{
    if(pTsePswFvRec == NULL)
    {
        return FALSE;
    }
    if(pTsePswFvRec->PswDataCount == OEM_TSE_PSW_FV_EMPTY_PAD)
    {
        return FALSE;
    }
    return TRUE;
}

//write the data to flash ROM
//Input:    FlashAddress - address of the flash ROM(MMIO)
//          Size - how many bytes write to flash
//          DataBuffer - the data that will write to flash
//Output:   NONE
//Return:   EFI_SUCCESS - write successfully
//          != EFI_SUCCESS - some error occured
EFI_STATUS  Oem_TsePswFvFlashWrite(
    IN  VOID *FlashAddress, 
    IN  UINTN Size, 
    IN  VOID *DataBuffer
)
{
    EFI_STATUS              Status;
    UINT8       tBuffer[16];
//    EFI_TPL                 OldTpl;
//    AMI_FLASH_PROTOCOL      *mFlash = GetFlashProtocol();
    FLASH_PROTOCOL            *mFlash = GetFlashProtocol();
    if(Size == 0)
    {
//no data need to write, just return success
        return EFI_SUCCESS;
    }
    if(DataBuffer == NULL)
    {
//need write data, but buffer is invalid
        return EFI_INVALID_PARAMETER;
    }
    if(mFlash == NULL)
    {
//Get FlashProtocol failed
        return EFI_NOT_FOUND;
    }
//write flash
//    OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
    //Status = mFlash->DeviceWriteEnable();
    Status = mFlash->DeviceWriteEnable();
    mFlash->Read((UINT8*)FlashAddress,16,tBuffer);
    if(!EFI_ERROR(Status))
    {
        Status = mFlash->Write(FlashAddress, Size, DataBuffer);
        mFlash->DeviceWriteDisable();
    }
//    pBS->RestoreTPL(OldTpl);
    return Status;
}

VOID *
EfiLibAllocatePool (
  IN  UINTN   AllocationSize
  );

//erase the specified flash ROM area(any address, any size), may be not aligned to FLASH_BLOCK_SIZE
//will preserve the data not belong to the specified area but may erased during block erase
//Input:    FlashAddress - address of the flash ROM(MMIO)
//          Size - how many bytes to erase
//Output:   NONE
//Return:   EFI_SUCCESS - erase successfully
//          != EFI_SUCCESS - some error occured
EFI_STATUS  Oem_TsePswFvFlashErase(
    IN VOID *FlashAddress, 
    IN UINTN Size
)
{
    EFI_STATUS              Status;
    EFI_TPL                 OldTpl;
    UINT8                   *pBlockBuffer = NULL;
    UINTN                   mStartAddress;
    UINTN                   mEndAddress;
    UINTN                   mBlockAddress, mStartBlockAddress, mEndBlockAddress;
    UINTN                   mBlocks;
    UINTN                   mRestorePreBytes = 0, mRestoreRestBytes = 0;
    UINTN                   Index;

//    AMI_FLASH_PROTOCOL      *mFlash = GetFlashProtocol();
    FLASH_PROTOCOL      *mFlash = GetFlashProtocol();
    if(Size == 0)
    {
//erase size is 0, just return success
        return EFI_SUCCESS;
    }
    if(mFlash == NULL)
    {
//Get FlashProtocol failed
        return EFI_NOT_FOUND;
    }
    mStartAddress = (UINTN)FlashAddress;
    mEndAddress = mStartAddress + Size - 1;

    if((mStartAddress%FLASH_BLOCK_SIZE) || ((mEndAddress+1)%FLASH_BLOCK_SIZE))
    {
//Start address or End address is not aligned to FLASH_BLOCK_SIZE, need preserve some data
        pBlockBuffer = EfiLibAllocatePool(FLASH_BLOCK_SIZE);
        if(pBlockBuffer == NULL)
        {
            return EFI_OUT_OF_RESOURCES;
        }
    }

//prepare to erase block
    mBlockAddress = mStartBlockAddress = BLOCK_ADDR(mStartAddress);
    mEndBlockAddress = BLOCK_ADDR(mEndAddress);
    mBlocks = BLOCK_NUMBER(mStartAddress, Size);    //how many blocks to erase
    for(Index = 0; Index < mBlocks; Index++, mBlockAddress+=(Index*FLASH_BLOCK_SIZE))
    {
        mRestorePreBytes = mRestoreRestBytes = 0;
        if(mBlockAddress == mStartBlockAddress)
        {
//it's first Block, preserve some data
            mRestorePreBytes = mStartAddress - mBlockAddress;
            if(CheckBuffer((UINT8*)mBlockAddress, mRestorePreBytes, OEM_TSE_PSW_FV_EMPTY_PAD) == NULL)
            {
//no need preserve because these data is empty
                mRestorePreBytes = 0;
            }
            else
            {
                MemCpy(pBlockBuffer, (VOID*)mBlockAddress, mRestorePreBytes);
            }
        }
        if(mBlockAddress == mEndBlockAddress)
        {
//it's last block
            mRestoreRestBytes = BLOCK_ENDADDR(mEndAddress) - mEndAddress;
            if(CheckBuffer((UINT8*)mEndAddress + 1, mRestoreRestBytes, OEM_TSE_PSW_FV_EMPTY_PAD) == NULL)
            {
//no need preserve because these data is empty
                mRestoreRestBytes = 0;
            }
            else
            {
                MemCpy(pBlockBuffer + (mEndAddress - mBlockAddress) + 1 , (UINT8*)mEndAddress + 1, mRestoreRestBytes);
            }
        }
//Erase block
        OldTpl = pBS->RaiseTPL(TPL_HIGH_LEVEL);
        Status = mFlash->DeviceWriteEnable();
        if(!EFI_ERROR(Status))
        {
            Status = mFlash->Erase((VOID*)mBlockAddress, FLASH_BLOCK_SIZE);
            mFlash->DeviceWriteDisable();
        }
        pBS->RestoreTPL(OldTpl);
        if(EFI_ERROR(Status))
        {
//Erase block failed
            break;
        }
        if(mRestorePreBytes)
        {
//restore pre data after erase block\r\n"));
            Status = Oem_TsePswFvFlashWrite((VOID*)mBlockAddress, mRestorePreBytes, pBlockBuffer);
        }
        if(mRestoreRestBytes)
        {
//restore rest data after erase block
            Status = Oem_TsePswFvFlashWrite((VOID*)(mEndAddress + 1), mRestoreRestBytes, pBlockBuffer + (mEndAddress - mBlockAddress) + 1);
        }
    }
//Erase end
    if(pBlockBuffer != NULL)
    {
        MemFreePointer((VOID**)&pBlockBuffer);
    }
    return Status;
}

//rebuild the FV header and FV data
//Input:
//          pTsePswFvDescPara - Optional, use the data specified by this pointer to rebuild the FV
//                          If the pTsePswFvDesc is NULL, then use default setting to rebuild the FV
//Return:
//          EFI_SUCCESS - Successfully
//          EFI_INVALID_PARAMETER - the data specified by pTsePswFvDesc has invalid data
//          EFI_DEVICE_ERROR - failed to locate Flash protocol
//          Other - flash access error occured
EFI_STATUS  Oem_RebuildTsePswFv(IN OPTIONAL OEM_TSE_PSW_FV_DESC *pTsePswFvDescPara)
{
    EFI_STATUS              Status;
    OEM_TSE_PSW_FV_DESC   mTsePswFvDesc;

    if(pTsePswFvDescPara == NULL)
    {
        MemSet(&mTsePswFvDesc, sizeof(OEM_TSE_PSW_FV_DESC), OEM_TSE_PSW_FV_EMPTY_PAD);
        MemCpy(&mTsePswFvDesc.Signature, &gOemTsePswVarGuid, sizeof(EFI_GUID));
        mTsePswFvDesc.Size = OEM_TSE_PSW_FV_SIZE;
        mTsePswFvDesc.AdminPswRegionDesc.Offset = sizeof(OEM_TSE_PSW_FV_DESC);
        mTsePswFvDesc.AdminPswRegionDesc.Size = (OEM_TSE_PSW_FV_SIZE - sizeof(OEM_TSE_PSW_FV_DESC)) >> 1; //(OEM_TSE_PSW_FV_SIZE - Fv_Header)/2
        mTsePswFvDesc.AdminPswRegionDesc.MaxRecordLimit = OEM_TSE_PSW_NUMBER_HASH_MAX;
        mTsePswFvDesc.UserPswRegionDesc.Offset = mTsePswFvDesc.AdminPswRegionDesc.Offset + mTsePswFvDesc.AdminPswRegionDesc.Size;
        mTsePswFvDesc.UserPswRegionDesc.Size = mTsePswFvDesc.AdminPswRegionDesc.Size;
        mTsePswFvDesc.UserPswRegionDesc.MaxRecordLimit = OEM_TSE_PSW_NUMBER_HASH_MAX;
        pTsePswFvDescPara = &mTsePswFvDesc;
    }
    else if(!Oem_CheckTsePswFv(pTsePswFvDescPara))
    {
        return EFI_INVALID_PARAMETER;
    }
    
//check the FV in ROM is empty or not, 
//even if the SPI erase procedure will check the FV is empty or not, but CheckBuffer() procedure is faster(QWord by Qword) than it(Byte by Byte)
    if(CheckBuffer((VOID*)OEM_TSE_PSW_FV_BASE, OEM_TSE_PSW_FV_SIZE, OEM_TSE_PSW_FV_EMPTY_PAD) != NULL)
    {
//erase the FV
        Status = Oem_TsePswFvFlashErase((VOID*)OEM_TSE_PSW_FV_BASE, pTsePswFvDescPara->Size);
        if(EFI_ERROR(Status))
        {
            return Status;
        }
    }
//write FV desc
    Status = Oem_TsePswFvFlashWrite((VOID*)OEM_TSE_PSW_FV_BASE, sizeof(OEM_TSE_PSW_FV_DESC), pTsePswFvDescPara);
    return Status;
}

//Get the record in the FV
//caller must ensure FV desc is valid (check by Oem_CheckTsePswFv()) before invoke this function
//caller must ensure region desc is valid (check by Oem_CheckTsePswFvRecRegion()) before invoke this function
//Input:
//          PswType - Password type, must be AMI_PASSWORD_ADMIN or AMI_PASSWORD_USER
//          pTsePswFvDesc - FV descriptor address
//          pCurrentTsePswFvRec - current record address
//              if pCurrentTsePswFvRec is NULL, then the first record (starting of the record region) will return
//              else the next record after current record will return
//Return:
//          OEM_TSE_PSW_RECORD_HEADER* - the next record address
//              NULL - no any record found
OEM_TSE_PSW_RECORD_HEADER* Oem_TsePswFvGetNextRecord(IN UINT32 PswType, OEM_TSE_PSW_FV_DESC *pTsePswFvDesc, OEM_TSE_PSW_RECORD_HEADER *pCurrentTsePswFvRec)
{
    OEM_TSE_PSW_FV_REC_REGION_DESC    *pTsePswFvRecRegion = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTsePswFvRec = NULL;
    
    if((PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER) || pTsePswFvDesc == NULL)
    {
        return NULL;
    }
    if(pTsePswFvDesc == NULL)
    {
        return NULL;
    }
    pTsePswFvRecRegion = (PswType == AMI_PASSWORD_ADMIN)?(&pTsePswFvDesc->AdminPswRegionDesc):(&pTsePswFvDesc->UserPswRegionDesc);
    if(pCurrentTsePswFvRec == NULL)
    {
//Current record is NULL, get first record(starting of the record region)
        pTsePswFvRec = (OEM_TSE_PSW_FV_RECORD_HEADER*)(((UINTN)pTsePswFvDesc) + pTsePswFvRecRegion->Offset);
    }
    else
    {
//Current record is NOT NULL, check if it is a record
        if(!Oem_CheckTsePswFvRecSignature(pCurrentTsePswFvRec) || !Oem_CheckTsePswFvRecDataCount(pCurrentTsePswFvRec))
        {
//Current record signature is not valid , or data count is not valid
            return NULL;
        }
//get next record address = current record address + current record size
        pTsePswFvRec = (OEM_TSE_PSW_FV_RECORD_HEADER*)(((UINTN)pCurrentTsePswFvRec) + TSE_PSW_REC_SIZE_P(pCurrentTsePswFvRec));
    }
    if((UINTN)pTsePswFvRec >= (UINTN)pTsePswFvDesc + pTsePswFvRecRegion->Offset + pTsePswFvRecRegion->Size)
    {
// Next record address exceeding the region limit
        return NULL;
    }
    if(!Oem_CheckTsePswFvRecSignature(pTsePswFvRec))
    {
//next record signature invalid
        return NULL;
    }
    if(!Oem_CheckTsePswFvRecDataCount(pTsePswFvRec))
    {
//next record DataCount invalid
        return NULL;
    }
    if((UINTN)pTsePswFvRec + TSE_PSW_REC_SIZE_P(pTsePswFvRec) > (((UINTN)pTsePswFvDesc) + (UINTN)pTsePswFvRecRegion->Offset + (UINTN)pTsePswFvRecRegion->Size))
    {
//Next record size invalid
        return NULL;
    }
//it's record
    return pTsePswFvRec;
}

//Get the valid record(not deleted) in the FV
//caller must ensure FV desc is valid (check by Oem_CheckTsePswFv()) before invoke this function
//caller must ensure region desc is valid (check by Oem_CheckTsePswFvRecRegion()) before invoke this function
//Input:
//          PswType - Password type, must be AMI_PASSWORD_ADMIN or AMI_PASSWORD_USER
//          pTsePswFvDesc - FV descriptor address
//          pCurrentTsePswFvRec - current record address
//              if pCurrentTsePswFvRec is NULL, then the first valid record will return
//              else the next record after current record will return
//Return:
//          OEM_TSE_PSW_RECORD_HEADER* - the next record address
//              NULL - no any record found
OEM_TSE_PSW_RECORD_HEADER* Oem_TsePswFvGetNextRecordWithValidFlag(IN UINT32 PswType, OEM_TSE_PSW_FV_DESC *pTsePswFvDesc, OEM_TSE_PSW_RECORD_HEADER *pCurrentTsePswFvRec)
{
    OEM_TSE_PSW_RECORD_HEADER         *pTmpTsePswFvRec = NULL;
    pTmpTsePswFvRec = pCurrentTsePswFvRec;
    do
    {
        pTmpTsePswFvRec = Oem_TsePswFvGetNextRecord(PswType, pTsePswFvDesc, pTmpTsePswFvRec);
        if((pTmpTsePswFvRec != NULL) && Oem_CheckTsePswFvRecFlag(pTmpTsePswFvRec))
        {
//found a record and flag is valid
            break;
        }
    }while(pTmpTsePswFvRec != NULL);
    return pTmpTsePswFvRec;
}

//Get the record region information
//caller must ensure FV desc is valid (check by Oem_CheckTsePswFv()) before invoke this function
//caller must ensure region desc is valid (check by Oem_CheckTsePswFvRecRegion()) before invoke this function
//Input:
//          pTsePswFvDesc - FV descriptor address
//          PswType - Password type, must be AMI_PASSWORD_ADMIN or AMI_PASSWORD_USER
//Output:   pRecordCount - Total record number (deleted and not deleted record) in the region
//          pValidRecordCount - Total valid record number(not deleted) in the region
//          pValidRecordMaxSize - max valid record's size in the region
//          pValidRecordTotalSize - Total valid record size
//          pFirstTsePswFvRec - The first record(deleted or not deleted record) in the region
//          pLastTsePswFvRec - The Last record(deleted or not deleted record) in the region
//          pFirstValidTsePswFvRec - The First valid record(not deleted) in the region
//          pLastValidTsePswFvRec - The Last valid record(not deleted) in the region
//Return:
//          EFI_SUCCESS - get region information succefully
//          Other - some error occured
EFI_STATUS  Oem_GetTsePswFvRecordRegionInfo(
        IN  OEM_TSE_PSW_FV_DESC   *pTsePswFvDesc,
        IN  UINT32                  PswType,
        OUT OPTIONAL UINTN          *pRecordCount,
        OUT OPTIONAL UINTN          *pValidRecordCount,
        OUT OPTIONAL UINTN          *pValidRecordMaxSize,
        OUT OPTIONAL UINTN          *pValidRecordTotalSize,
        OUT OPTIONAL OEM_TSE_PSW_RECORD_HEADER **pFirstTsePswFvRec,
        OUT OPTIONAL OEM_TSE_PSW_RECORD_HEADER **pLastTsePswFvRec,
        OUT OPTIONAL OEM_TSE_PSW_RECORD_HEADER **pFirstValidTsePswFvRec,
        OUT OPTIONAL OEM_TSE_PSW_RECORD_HEADER **pLastValidTsePswFvRec
)
{
    OEM_TSE_PSW_FV_REC_REGION_DESC    *pTsePswFvRecRegion = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTsePswFvRec = NULL, *pTmpFirstTsePswFvRec = NULL, *pTmpLastTsePswFvRec = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTmpFirstValidTsePswFvRec = NULL, *pTmpLastValidTsePswFvRec = NULL;
    UINTN                               mRecordCount = 0, mValidRecordCount = 0, mValidRecordMaxSize = 0, mValidRecordTotalSize = 0;

    if(pTsePswFvDesc == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return EFI_INVALID_PARAMETER;
    }
    pTsePswFvRecRegion = (PswType == AMI_PASSWORD_ADMIN)?(&pTsePswFvDesc->AdminPswRegionDesc):(&pTsePswFvDesc->UserPswRegionDesc);
//check how many valid record and get first valid record address.
    while((pTsePswFvRec = Oem_TsePswFvGetNextRecord(PswType, pTsePswFvDesc, pTsePswFvRec)) != NULL)
    {
//record found
        if(pTmpFirstTsePswFvRec == NULL)
        {
            pTmpFirstTsePswFvRec = pTsePswFvRec;
//save first record
        }
        if(Oem_CheckTsePswFvRecFlag(pTsePswFvRec))
        {
//record flag is valid
            if(pTmpFirstValidTsePswFvRec == NULL)
            {
//Save first valid record
                pTmpFirstValidTsePswFvRec = pTsePswFvRec;
            }
            pTmpLastValidTsePswFvRec = pTsePswFvRec;
            mValidRecordCount++;
            mValidRecordTotalSize += TSE_PSW_REC_SIZE_P(pTsePswFvRec);
            mValidRecordMaxSize = MAX(mValidRecordMaxSize, TSE_PSW_REC_SIZE_P(pTsePswFvRec));
        }
        pTmpLastTsePswFvRec = pTsePswFvRec;
        mRecordCount++;
    }
    if(pRecordCount != NULL) *pRecordCount = mRecordCount;
    if(pValidRecordCount != NULL) *pValidRecordCount = mValidRecordCount;
    if(pValidRecordMaxSize != NULL ) *pValidRecordMaxSize = mValidRecordMaxSize;
    if(pValidRecordTotalSize != NULL) *pValidRecordTotalSize = mValidRecordTotalSize;
    if(pFirstTsePswFvRec != NULL) *pFirstTsePswFvRec = pTmpFirstTsePswFvRec;
    if(pLastTsePswFvRec != NULL) *pLastTsePswFvRec = pTmpLastTsePswFvRec;
    if(pFirstValidTsePswFvRec != NULL) *pFirstValidTsePswFvRec = pTmpFirstValidTsePswFvRec;
    if(pLastValidTsePswFvRec != NULL) *pLastValidTsePswFvRec = pTmpLastValidTsePswFvRec;

    return EFI_SUCCESS;
}

//cleanup the specified record region
//drop the deleted record
//drop some old valid record if remain size(region size - total valid record size) is smaller than RequireSize
//Rebuild the FV desc if FV desc is empty/invalid or record region is invalid
//Input:
//          PswType - Password type, indicate which record region to cleanup, must be AMI_PASSWORD_ADMIN or AMI_PASSWORD_USER
//          RequireSize - 0: just drop the deleted record
//                       >0: drop some old valid record if remain size is smaller than RequireSize
//Output:   NONE
//Return:
//          EFI_SUCCESS - success
//          Other - some error occured
EFI_STATUS  Oem_TsePswFvCleanupRegion(IN UINT32 PswType, IN UINTN RequireSize)
{
    EFI_STATUS                          Status;
    OEM_TSE_PSW_FV_DESC               *pTsePswFvDesc = (OEM_TSE_PSW_FV_DESC *)OEM_TSE_PSW_FV_BASE;
    OEM_TSE_PSW_FV_REC_REGION_DESC    *pTsePswFvRecRegion = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTsePswFvRec = NULL, *pTmpTsePswFvRec = NULL;
    UINT8                               *pTsePswFvRecSaveBuffer = NULL;
    UINTN                               mSize = 0;
    UINT8                               *pBlockBuffer = NULL;
    UINTN                               mPswCount = 0;
    UINTN                               mRecordTotalSize = 0;
    
    if(!Oem_CheckTsePswFv(pTsePswFvDesc) || !Oem_CheckTsePswFvRecRegion(pTsePswFvDesc))
    {
//FV invalid or record region invalid, rebuild FV
        Status = Oem_RebuildTsePswFv(NULL);
//rebuild FV will erase all data in region, therefor, no need do any other cleanup action
        return Status;
    }
    pTsePswFvRecRegion = (PswType == AMI_PASSWORD_ADMIN)?(&pTsePswFvDesc->AdminPswRegionDesc):(&pTsePswFvDesc->UserPswRegionDesc);
    RequireSize = MIN(RequireSize, pTsePswFvRecRegion->Size);
    pTmpTsePswFvRec = NULL;

//get total valid record number and size
    Status = Oem_GetTsePswFvRecordRegionInfo( pTsePswFvDesc, 
                                                PswType,
                                                NULL,               //pRecordCount
                                                &mPswCount,         //pValidRecordCount
                                                NULL,               //pValidRecordMaxSize
                                                &mRecordTotalSize,  //pValidRecordTotalSize
                                                NULL,               //pFirstTsePswFvRec
                                                NULL,               //pLastTsePswFvRec
                                                &pTmpTsePswFvRec,   //pFirstValidTsePswFvRec
                                                NULL                //pLastValidTsePswFvRec
                                              );
    if(EFI_ERROR(Status))
    {
        return Status;
    }

//drop old extra valid record if total record number is lager than limit
//drop some old valid record if remain size is smaller than RequireSize
    while(((mPswCount > pTsePswFvRecRegion->MaxRecordLimit) || ((mRecordTotalSize + RequireSize) > pTsePswFvRecRegion->Size)) && pTmpTsePswFvRec != NULL)
    {
        mRecordTotalSize -= TSE_PSW_REC_SIZE_P(pTmpTsePswFvRec);
        pTmpTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pTmpTsePswFvRec);
        if(mPswCount)
        {
            mPswCount--;
        }
    }

//there are some valid record in region, save these record
    if(mPswCount && mRecordTotalSize && pTmpTsePswFvRec != NULL)
    {
//save all valid record into new buffer
        pTsePswFvRecSaveBuffer = EfiLibAllocatePool(mRecordTotalSize);
        if(pTsePswFvRecSaveBuffer == NULL)
        {
            return EFI_OUT_OF_RESOURCES;
        }
        MemSet(pTsePswFvRecSaveBuffer, mRecordTotalSize, OEM_TSE_PSW_FV_EMPTY_PAD);

        while(pTmpTsePswFvRec != NULL)
        {
            if( (mSize + TSE_PSW_REC_SIZE_P(pTmpTsePswFvRec)) > mRecordTotalSize )
            {
//no space to save current record
                break;
            }
            MemCpy(pTsePswFvRecSaveBuffer + mSize, pTmpTsePswFvRec, TSE_PSW_REC_SIZE_P(pTmpTsePswFvRec));
            mSize += TSE_PSW_REC_SIZE_P(pTmpTsePswFvRec);
            pTmpTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pTmpTsePswFvRec);
        }
    }

    pTsePswFvRec = (OEM_TSE_PSW_RECORD_HEADER*)((UINTN)pTsePswFvDesc + pTsePswFvRecRegion->Offset);
    Status = EFI_SUCCESS;
    if(CheckBuffer((UINT8*)pTsePswFvRec, pTsePswFvRecRegion->Size, OEM_TSE_PSW_FV_EMPTY_PAD))
    {
//Erase record region
        Status = Oem_TsePswFvFlashErase(pTsePswFvRec, pTsePswFvRecRegion->Size);
    }
    if(!EFI_ERROR(Status) && pTsePswFvRecSaveBuffer != NULL && mSize)
    {
//restore preserved record data
        Status = Oem_TsePswFvFlashWrite(pTsePswFvRec, mSize, pTsePswFvRecSaveBuffer);
    }
    if(pTsePswFvRecSaveBuffer != NULL)
    {
        MemFreePointer((VOID**)&pTsePswFvRecSaveBuffer);
    }
    return Status;
}

//delete record(write record flag to 0x00)
//caller must ensure the record address ia valid
//Input:
//          pTsePswFvRec - the record to be deleted
//Output:   NONE
//Return:
//          EFI_SUCCESS - success
//          Other - some error occured
EFI_STATUS  Oem_TsePswFvRecordDelete(OEM_TSE_PSW_RECORD_HEADER* pTsePswFvRec)
{
    UINT8   Data8 = 0;
    return Oem_TsePswFvFlashWrite(&pTsePswFvRec->Flag, 1, &Data8);  //mask current record
}

//create new record in region
//Rebuild the FV desc if FV desc is empty/invalid or record region is invalid
//Input:
//          PswType - Password type, must be AMI_PASSWORD_ADMIN or AMI_PASSWORD_USER
//          pNewTsePswFvRec - the new record data will write to region
//Output:   NONE
//Return:
//          EFI_SUCCESS - success
//          Other - some error occured
EFI_STATUS  Oem_TsePswFvRecordCreateNew(IN UINT32 PswType, IN OEM_TSE_PSW_RECORD_HEADER* pNewTsePswFvRec)
{
    EFI_STATUS                          Status;
    OEM_TSE_PSW_FV_DESC               *pTsePswFvDesc = (OEM_TSE_PSW_FV_DESC *)OEM_TSE_PSW_FV_BASE;
    OEM_TSE_PSW_FV_REC_REGION_DESC    *pTsePswFvRecRegion = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTsePswFvRec = NULL, *pTmpTsePswFvRec = NULL, *pLastTsePswFvRec = NULL;
    UINTN                               mPswCount = 0;

    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(pNewTsePswFvRec == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(!Oem_CheckTsePswFvRecSignature(pNewTsePswFvRec) || !Oem_CheckTsePswFvRecFlag(pNewTsePswFvRec) || !Oem_CheckTsePswFvRecDataCount(pNewTsePswFvRec))
    {
        return EFI_INVALID_PARAMETER;
    }
    if(!Oem_CheckTsePswFv(pTsePswFvDesc) || !Oem_CheckTsePswFvRecRegion(pTsePswFvDesc))
    {
//FV invalid or record region invalid, rebuild FV
        Status = Oem_RebuildTsePswFv(NULL);
        if(EFI_ERROR(Status))
        {
//rebuild FV failed
            return Status;
        }
    }
    pTsePswFvRecRegion = (PswType == AMI_PASSWORD_ADMIN)?(&pTsePswFvDesc->AdminPswRegionDesc):(&pTsePswFvDesc->UserPswRegionDesc);
//check the new record size, if it is lager than region size, do not write to region
    if(TSE_PSW_REC_SIZE_P(pNewTsePswFvRec) > pTsePswFvRecRegion->Size)
    {
        return EFI_UNSUPPORTED;
    }
//check how many valid record and get first valid record address.
    Status = Oem_GetTsePswFvRecordRegionInfo( pTsePswFvDesc, 
                                                PswType,
                                                NULL,               //pRecordCount
                                                &mPswCount,         //pValidRecordCount
                                                NULL,               //pValidRecordMaxSize
                                                NULL,               //pValidRecordTotalSize
                                                NULL,               //pFirstTsePswFvRec
                                                &pLastTsePswFvRec,  //pLastTsePswFvRec
                                                &pTmpTsePswFvRec,   //pFirstValidTsePswFvRec
                                                NULL                //pLastValidTsePswFvRec
                                              );
    if(EFI_ERROR(Status))
    {
        return Status;
    }
//Check if need delete old extra valid record
    while(mPswCount && (mPswCount >=  pTsePswFvRecRegion->MaxRecordLimit) && pTmpTsePswFvRec != NULL)
    {
        pTsePswFvRec = pTmpTsePswFvRec;
        if((mPswCount-1) && ((mPswCount-1) >=  pTsePswFvRecRegion->MaxRecordLimit))     //reduce read record
        {
//save next record address before delete current
            pTmpTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pTmpTsePswFvRec);
        }
//delete record
        Status = Oem_TsePswFvRecordDelete(pTsePswFvRec);  //mask current record
        mPswCount--;
    }

    if(pLastTsePswFvRec != NULL)
    {
//last valid record found, set next useable record address to the address after this record
        pTmpTsePswFvRec = (OEM_TSE_PSW_FV_RECORD_HEADER*)(((UINTN)pLastTsePswFvRec) + TSE_PSW_REC_SIZE_P(pLastTsePswFvRec));
    }
    else 
    {
//last record NOT found, set next useable record address to record region start
        pTmpTsePswFvRec = (OEM_TSE_PSW_RECORD_HEADER*)(((UINTN)pTsePswFvDesc) + pTsePswFvRecRegion->Offset);
    }
    
//check if there is enough space from this address to save the record
    if(  (((UINTN)pTmpTsePswFvRec + TSE_PSW_REC_SIZE_P(pNewTsePswFvRec)) > (((UINTN)pTsePswFvDesc) + pTsePswFvRecRegion->Offset + pTsePswFvRecRegion->Size))
       || CheckBuffer8((UINT8*)pTmpTsePswFvRec, TSE_PSW_REC_SIZE_P(pNewTsePswFvRec), OEM_TSE_PSW_FV_EMPTY_PAD)
      )
    {
//new record space is not empty or remain space is too small, need clean region
        Status = Oem_TsePswFvCleanupRegion(PswType, TSE_PSW_REC_SIZE_P(pNewTsePswFvRec));
//get last record again after rebuild FV or region cleanup
        pTsePswFvRec = pLastTsePswFvRec = NULL;
        while((pTsePswFvRec = Oem_TsePswFvGetNextRecord(PswType, pTsePswFvDesc, pTsePswFvRec)) != NULL)
        {
            pLastTsePswFvRec = pTsePswFvRec;
        }
    }
//check the space again after rebuild or cleanup
    if(pLastTsePswFvRec != NULL)
    {
        pTmpTsePswFvRec = (OEM_TSE_PSW_FV_RECORD_HEADER*)(((UINTN)pLastTsePswFvRec) + TSE_PSW_REC_SIZE_P(pLastTsePswFvRec));
    }
    else 
    {
        pTmpTsePswFvRec = (OEM_TSE_PSW_RECORD_HEADER*)(((UINTN)pTsePswFvDesc) + pTsePswFvRecRegion->Offset);
    }
    if(   (((UINTN)pTmpTsePswFvRec + TSE_PSW_REC_SIZE_P(pNewTsePswFvRec)) > (((UINTN)pTsePswFvDesc) + pTsePswFvRecRegion->Offset + pTsePswFvRecRegion->Size))
       || CheckBuffer8((UINT8*)pTmpTsePswFvRec, TSE_PSW_REC_SIZE_P(pNewTsePswFvRec), OEM_TSE_PSW_FV_EMPTY_PAD)
      )
    {
//new record space is still not empty or remain space is too small, there may be some error
        return EFI_DEVICE_ERROR;
    }
//write record to region
    Status = Oem_TsePswFvFlashWrite((VOID*)pTmpTsePswFvRec, TSE_PSW_REC_SIZE_P(pNewTsePswFvRec), (VOID*)pNewTsePswFvRec);
    return Status;
}

//Check specified password if it is already recorded
//Input:
//          PswType - Password type, must be AMI_PASSWORD_ADMIN or AMI_PASSWORD_USER
//          pTsePswFvDesc - the new record data will write to region
//          PlaintextPsw - the plaintext password 
//          PswLen - the length of the plaintext password(or the length to check)
//Output:   NONE
//Return:
//          TRUE - password already exist
//          FALSE - password not exist
BOOLEAN Oem_CheckTsePswFvRecordExist(IN OEM_TSE_PSW_FV_DESC *pTsePswFvDesc, IN UINT32 PswType, IN CHAR16 *PlaintextPsw, IN UINTN PswLen)
{
    EFI_STATUS                          Status;
    OEM_TSE_PSW_FV_REC_REGION_DESC    *pTsePswFvRecRegion = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTsePswFvRec = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pFirstValidTsePswFvRec = NULL;
    UINTN                               mPswCount = 0;
    UINT16                              mEncodedData[SETUP_PASSWORD_LENGTH] = {0};

    if(pTsePswFvDesc == NULL)
    {
        return FALSE;
    }
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return FALSE;
    }
//if password is empty, no need to check
    if(PlaintextPsw == NULL && PswLen == 0)
    {
        return FALSE;
    }
    if(*PlaintextPsw == 0 || PswLen == 0)
    {
        return FALSE;
    }
//check if FV and record region is valid or not
    if(!Oem_CheckTsePswFv(pTsePswFvDesc) || !Oem_CheckTsePswFvRecRegion(pTsePswFvDesc))
    {
        return FALSE;
    }

    PswLen = MIN(PswLen, SETUP_PASSWORD_LENGTH);
    pTsePswFvRecRegion = (PswType == AMI_PASSWORD_ADMIN)?(&pTsePswFvDesc->AdminPswRegionDesc):(&pTsePswFvDesc->UserPswRegionDesc);
//check how many valid record and get first valid record address.
    Status = Oem_GetTsePswFvRecordRegionInfo( pTsePswFvDesc,
                                                PswType,
                                                NULL,               //pRecordCount
                                                &mPswCount,         //pValidRecordCount
                                                NULL,               //pValidRecordMaxSize
                                                NULL,               //pValidRecordTotalSize
                                                NULL,               //pFirstTsePswFvRec
                                                NULL,               //pLastTsePswFvRec
                                                &pFirstValidTsePswFvRec,   //pFirstValidTsePswFvRec
                                                NULL                //pLastValidTsePswFvRec
                                              );
    if(EFI_ERROR(Status) || mPswCount == 0 || pFirstValidTsePswFvRec == NULL)
    {
        return FALSE;
    }
//skip extra record
    while((mPswCount > pTsePswFvRecRegion->MaxRecordLimit) && pFirstValidTsePswFvRec != NULL)
    {
        pFirstValidTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pFirstValidTsePswFvRec);
        mPswCount--;
    }
//check if there any record is equal to the password
    MemSet(mEncodedData, sizeof(mEncodedData), 0);
    MemCpy(mEncodedData, PlaintextPsw, PswLen * sizeof(CHAR16));
    PasswordEncodeHook(mEncodedData, sizeof(mEncodedData));
    while(mPswCount && pFirstValidTsePswFvRec != NULL)
    {
//        if(mPswCount <= pTsePswFvRecRegion->MaxRecordLimit)
//        {
//check this record is equal or not
            if(MemCmp(mEncodedData, pFirstValidTsePswFvRec->FullPswData.PswData, sizeof(OEM_TSE_PSW_DATA)) == 0)
            {
//record equal to password
                return TRUE;
            }
//        }
        mPswCount--;
        if(mPswCount)   //reduce last unnecessory action
        {
            pFirstValidTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pFirstValidTsePswFvRec);
        }
    }
//no record equal to password
    return FALSE;
}

//Encode the specified password and save into NVRAM
//Input:
//          PswType - AMI_PASSWORD_ADMIN, AMI_PASSWORD_USER
//          PlaintextPsw - current plaintext password
//          PswLen - the length of current plaintext password string, may equal to EfiStrLen(PlaintextPsw)
//Output:
//          None
//Return:
//          EFI_SUCCESS - Successfully save the data
//          Other - some error occured
EFI_STATUS  Oem_SaveTsePswFV(UINT32 PswType, CHAR16 *PlaintextPsw, UINTN PswLen)
{
    EFI_STATUS                      Status = EFI_SUCCESS;
    OEM_TSE_PSW_FV_DESC           *pTsePswFvDesc = (OEM_TSE_PSW_FV_DESC*)OEM_TSE_PSW_FV_BASE;
    UINTN                           mSize = 0;
    UINT16                          mEncodedData[SETUP_PASSWORD_LENGTH] = {0};
    OEM_TSE_PSW_RECORD_HEADER    *pTsePswFvRec = NULL;
    OEM_TSE_PSW_DATA              *pTsePswData = NULL;
    UINTN                           Index;
    
//check input parameters
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(PlaintextPsw == NULL && PswLen == 0)
    {
        return EFI_SUCCESS;
    }
    if(*PlaintextPsw == 0 || PswLen == 0)
    {
        return EFI_SUCCESS;
    }
    PswLen = MIN(PswLen, SETUP_PASSWORD_LENGTH);
//check if this password already record
    if(Oem_CheckTsePswFvRecordExist(pTsePswFvDesc, PswType, PlaintextPsw, PswLen))
    {
        return EFI_SUCCESS;
    }

//allocate memory for new record
    mSize = (PswLen > OEM_TSE_PSW_HASH_CHARS_COUNT)?(PswLen - OEM_TSE_PSW_HASH_CHARS_COUNT + 1):1;
    mSize = mSize * sizeof(OEM_TSE_PSW_DATA);
    mSize += sizeof(OEM_TSE_PSW_RECORD_HEADER);
    
    pTsePswFvRec = NULL;
    pTsePswFvRec = EfiLibAllocatePool( mSize );
    if(pTsePswFvRec == NULL)
    {
        return EFI_OUT_OF_RESOURCES;
    }

    MemSet(mEncodedData, sizeof(mEncodedData), 0);
    MemCpy(mEncodedData, PlaintextPsw, PswLen * sizeof(CHAR16));
    PasswordEncodeHook(mEncodedData, sizeof(mEncodedData));

    MemSet(pTsePswFvRec, sizeof(OEM_TSE_PSW_RECORD_HEADER), OEM_TSE_PSW_FV_EMPTY_PAD);
//save hash data into record memory
    pTsePswFvRec->Signature = OEM_TSE_PSW_FV_RECORD_SIGNATURE;
    pTsePswFvRec->Flag = 0xFF;
    pTsePswFvRec->PswDataCount = (UINT8)((PswLen > OEM_TSE_PSW_HASH_CHARS_COUNT)?(PswLen - OEM_TSE_PSW_HASH_CHARS_COUNT + 1):1);
    MemCpy(pTsePswFvRec->FullPswData.PswData, mEncodedData, sizeof(OEM_TSE_PSW_DATA));    //save current encoded whole password.

    pTsePswData = (OEM_TSE_PSW_DATA*)((UINTN)pTsePswFvRec + sizeof(OEM_TSE_PSW_RECORD_HEADER));
    
//encode and save data 4 chars by 4 chars
    for(Index = 0; Index + OEM_TSE_PSW_HASH_CHARS_COUNT <= MAX(PswLen, OEM_TSE_PSW_HASH_CHARS_COUNT); Index++, pTsePswData++)
    {
        MemSet(mEncodedData, sizeof(mEncodedData), 0);
        MemCpy(mEncodedData, PlaintextPsw + Index, (MIN(PswLen-Index, OEM_TSE_PSW_HASH_CHARS_COUNT))*sizeof(UINT16));
        PasswordEncodeHook( mEncodedData, sizeof(mEncodedData));
        MemCpy(pTsePswData->PswData, mEncodedData, sizeof(OEM_TSE_PSW_DATA));
    }
//encode and save data end
//Full whole record data to region
    Status = Oem_TsePswFvRecordCreateNew(PswType, pTsePswFvRec);
    MemFreePointer( (VOID**)&pTsePswFvRec);
    return Status;
}

//Get the password hash datas (OEM_TSE_PSW_RECORD_HEADER) from FV
//Input:    PswType - AMI_PASSWORD_ADMIN, AMI_PASSWORD_USER
//          pPswRecArray - the array to save the pointer that point to hash data (OEM_TSE_PSW_RECORD_HEADER)
//          pPswCount - the array size
//Output:   pPswRecArray - the pointer that point to hash data (OEM_TSE_PSW_RECORD_HEADER)
//          pPswCount - number of pointer of hash data (OEM_TSE_PSW_RECORD_HEADER) stored in pPswRecArray
//                      the output number is <= input array size
//return:   EFI_SUCCESS
//          EFI_INVALID_PARAMETER - one or more input parameter is invalid
EFI_STATUS Oem_GetPreviousHashedPswArrayFV(IN UINT32 PswType, IN OUT OEM_TSE_PSW_RECORD_HEADER **pPswRecArray, IN OUT UINT32 *pPswCount)
{
    EFI_STATUS                          Status;
    OEM_TSE_PSW_FV_DESC               *pTsePswFvDesc = (OEM_TSE_PSW_FV_DESC*)OEM_TSE_PSW_FV_BASE;
    OEM_TSE_PSW_FV_REC_REGION_DESC    *pTsePswFvRecRegion = NULL;
    OEM_TSE_PSW_RECORD_HEADER         *pTsePswFvRec = NULL, *pTmpTsePswFvRec = NULL, *pLastTsePswFvRec = NULL;
    UINTN                               mPswCount = 0;
    UINTN                               Index = 0;
//check input parameters
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(pPswCount == NULL || *pPswCount == 0 || pPswRecArray == NULL)
    {
        return EFI_INVALID_PARAMETER;
    }
    if(!Oem_CheckTsePswFv(pTsePswFvDesc) || !Oem_CheckTsePswFvRecRegion(pTsePswFvDesc))
    {
//FV or record region is invalid
        return EFI_NOT_FOUND;
    }
//check how many valid record.
    Status = Oem_GetTsePswFvRecordRegionInfo( pTsePswFvDesc, 
                                                PswType,
                                                NULL,               //pRecordCount
                                                &mPswCount,         //pValidRecordCount
                                                NULL,               //pValidRecordMaxSize
                                                NULL,               //pValidRecordTotalSize
                                                NULL,               //pFirstTsePswFvRec
                                                NULL,               //pLastTsePswFvRec
                                                &pTmpTsePswFvRec,   //pFirstValidTsePswFvRec
                                                NULL                //pLastValidTsePswFvRec
                                              );
    if(EFI_ERROR(Status))
    {
        return Status;
    }
//if no valid record found
    if(mPswCount == 0 || pTmpTsePswFvRec == NULL)
    {
        *pPswCount = 0;
        return EFI_NOT_FOUND;
    }
//skip extra record, get the latest record
    while((mPswCount > *pPswCount) && pTmpTsePswFvRec != NULL)
    {
        pTmpTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pTmpTsePswFvRec);
        mPswCount--;
    }
//check again after skip extra record
    if(mPswCount == 0 || pTmpTsePswFvRec == NULL)
    {
        *pPswCount = 0;
        return EFI_NOT_FOUND;
    }
//save all record to buffer
    for(Index = 0; (Index < mPswCount) && pTmpTsePswFvRec != NULL; )
    {
        pPswRecArray[Index] = EfiLibAllocatePool(TSE_PSW_REC_SIZE_P(pTmpTsePswFvRec));
        if(pPswRecArray[Index] == NULL)
        {
//allocate buffer for record failed
            break;
        }
        MemCpy(pPswRecArray[Index], pTmpTsePswFvRec, TSE_PSW_REC_SIZE_P(pTmpTsePswFvRec));
        Index ++;
        if(Index >= mPswCount)
        {
            break;
        }
        pTmpTsePswFvRec = Oem_TsePswFvGetNextRecordWithValidFlag(PswType, pTsePswFvDesc, pTmpTsePswFvRec);
    }
    if(Index == 0)
    {
//return No record if memory allocate failed for all record
        return EFI_NOT_FOUND;
    }
    *pPswCount = (UINT32)Index;
    return EFI_SUCCESS;
}


#endif  //#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 1)


//Get the password hash data (OEM_TSE_PSW_RECORD_HEADER)
//Input:    PswType - AMI_PASSWORD_ADMIN, AMI_PASSWORD_USER
//          pPswRecArray - the array to save the pointer that point to hash data (OEM_TSE_PSW_RECORD_HEADER)
//          pPswCount - the array size
//Output:   pPswRecArray - the pointer that point to hash data (OEM_TSE_PSW_RECORD_HEADER)
//          pPswCount - number of pointer of hash data (OEM_TSE_PSW_RECORD_HEADER) stored in pPswRecArray
//                      the output number is <= input array size
//return:   EFI_SUCCESS
//          EFI_INVALID_PARAMETER - one or more input parameter is invalid
EFI_STATUS Oem_GetPreviousHashedPswArray(IN UINT32 PswType, IN OUT OEM_TSE_PSW_RECORD_HEADER **pPswRecArray, IN OUT UINT32 *pPswCount)
{
#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 0)     //nvram
    return Oem_GetPreviousHashedPswArrayNvram(PswType, pPswRecArray, pPswCount);
#elif (OEM_TSE_PSW_HASH_DATA_LOCATION == 1)   //FV
    return Oem_GetPreviousHashedPswArrayFV(PswType, pPswRecArray, pPswCount);
#endif
    return EFI_UNSUPPORTED;
}

EFI_STATUS  Oem_SaveTsePswWrapper(UINT32 PswType, UINT16 *PlaintextPsw, UINTN PswLen)
{
#if (OEM_TSE_PSW_HASH_DATA_LOCATION == 0) //NVRAM
    return Oem_SaveTsePswNvram(PswType, PlaintextPsw, PswLen);
#elif (OEM_TSE_PSW_HASH_DATA_LOCATION == 1)   //FV
    return Oem_SaveTsePswFV(PswType, PlaintextPsw, PswLen);
#endif
    return EFI_UNSUPPORTED;
}
EFI_STATUS  Oem_SaveTsePsw(VOID)
{
    Oem_SaveTsePswWrapper(AMI_PASSWORD_ADMIN, gTmpAdminPlaintextPsw, EfiStrLen (gTmpAdminPlaintextPsw));
    Oem_SaveTsePswWrapper(AMI_PASSWORD_USER, gTmpUserPlaintextPsw, EfiStrLen (gTmpUserPlaintextPsw));
    return EFI_SUCCESS;
}


//Check the specified password with previous one or more (specified by OEM_TSE_PSW_NUMBER_HASH_MAX) password 
//      if there is 4 (specified by OEM_TSE_PSW_HASH_CHARS_COUNT) Consecutive Chars same or not
//Input:
//          PswType - AMI_PASSWORD_ADMIN, AMI_PASSWORD_USER
//          PlaintextPsw - current plaintext password
//          PswLen - the length of current plaintext password string, may equal to EfiStrLen(PlaintextPsw)
//Output:
//          None
//Return:
//          TRUE - There is 4 (specified by OEM_TSE_PSW_HASH_CHARS_COUNT) Consecutive Chars same with previous password
//          FALSE
BOOLEAN Oem_CheckNewTsePasswordWithPrevious(UINT32 PswType, CHAR16 *PlaintextPsw, UINTN PswLen)
{
    EFI_STATUS              Status = EFI_SUCCESS;
    UINT16                  mEncodedData[SETUP_PASSWORD_LENGTH];
    OEM_TSE_PSW_RECORD_HEADER    *aTsePswRecArray[OEM_TSE_PSW_NUMBER_HASH_MAX];
    OEM_TSE_PSW_DATA      *pPswData = NULL;
    UINT32                  mPswCount = OEM_TSE_PSW_NUMBER_HASH_MAX;
    UINTN                   Index = 0, j, k;
    BOOLEAN                 bDataSame = FALSE;
    
    if(PswType != AMI_PASSWORD_ADMIN && PswType != AMI_PASSWORD_USER)
    {
        return FALSE;
    }
    if(PlaintextPsw == NULL || *PlaintextPsw == 0 || PswLen == 0)
    {
        return FALSE;
    }
//get previous password data
    MemSet( aTsePswRecArray, sizeof(aTsePswRecArray), 0 );
    Status = Oem_GetPreviousHashedPswArray(PswType, aTsePswRecArray, &mPswCount);
    if(EFI_ERROR(Status) || mPswCount == 0)
    {
        return FALSE;
    }

//check the password data four chars by four chars
    for(Index = 0; (Index + OEM_TSE_PSW_HASH_CHARS_COUNT <= MAX(PswLen, OEM_TSE_PSW_HASH_CHARS_COUNT)) && (!bDataSame) ; Index++)
    {
        //encode the four chars of current password
        MemSet(mEncodedData, sizeof(mEncodedData), 0);
        MemCpy(mEncodedData, PlaintextPsw + Index, (MIN(PswLen-Index, OEM_TSE_PSW_HASH_CHARS_COUNT))*sizeof(UINT16));
        PasswordEncodeHook( mEncodedData, sizeof(mEncodedData));
        
        //compare with hash data record array
        for(j = 0; (j < mPswCount) && (!bDataSame); j++)
        {
            if(aTsePswRecArray[j] == NULL)
            {
                continue;
            }
            pPswData = (OEM_TSE_PSW_DATA*)((UINTN)(aTsePswRecArray[j]) + sizeof(OEM_TSE_PSW_RECORD_HEADER));
            //PswDataCount is number of hash data in one record, each hash data is the encoded data of the four chars in previous password.
            for(k = 0; k < aTsePswRecArray[j]->PswDataCount; k++, pPswData++)
            {
                if(MemCmp(mEncodedData, pPswData->PswData, sizeof(OEM_TSE_PSW_DATA)) == 0)
                {
                    //match data found
                    bDataSame = TRUE;
                    break;
                }
            }
            
        }
    }
    
    Oem_FreePswRecArray(aTsePswRecArray, mPswCount);
    return bDataSame;
}
#endif  //#if defined(OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS) && (OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS)
#endif //#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)

//<Wangbn001-20191102-password four Consecutive Chars check->

//----------------------------------------------------------------------
// Procedure: OemSavedConfigChanges
// Description: OEM function is called after setup utility saves changes
//              based on user input. It is invoked by below hooks:
//                  SaveAndExit()
//                  SaveWithoutExit()
//                  SaveAndReset()
//              The behavior is same as AmitseNvramUpdateGuid callback event
//----------------------------------------------------------------------
//<AMI_PHDR_END>
VOID OemSavedConfigChanges(VOID)
{

    //TODO: Put your code here start
    EFI_HANDLE DummyHandle = NULL;
    //EFI_GUID OemNvramUpdateGuid = OEM_NVRAM_UPDATE_GUID;
    pBS->InstallProtocolInterface(
        &DummyHandle, &gOemNvramUpdateGuid, EFI_NATIVE_INTERFACE, NULL
    );
    //TODO: Put your code here end
//<Wangbn001-20191102-password four Consecutive Chars check+>
//if token SETUP_SAVE_PSWD_TO_NVRAM is set to 1, the password data already saved in function _FramePasswordCallback
//so skip save again
#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1)
#if defined(OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS) && (OEM_PSW_CONSECUTIVE_CHARS_CHECK_WITH_PREVIOUS)
    if(!TSEPwdSavetoNvram())
    {
        Oem_SaveTsePsw();
    }
#endif
#endif
//<Wangbn001-20191102-password four Consecutive Chars check->	
    return;
}
