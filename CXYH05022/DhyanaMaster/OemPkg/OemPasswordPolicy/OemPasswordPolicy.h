/** @file OemPasswordPolicy.h
    Header file for string functionalities.

**/

#ifndef _OEM_PASSWORD_POLICY_H_
#define	_OEM_PASSWORD_POLICY_H_

#include "token.h"
#include "AMIVfr.h"

#define PCH_TCO_BASE                           0x60
#define R_PCH_TCO2_STS                         0x06
#define B_PCH_TCO2_STS_INTRD_DET               BIT0
#define VAR_EQ_ADMIN_DECIMAL_NAME              L"65"
#define VAR_EQ_USER_DECIMAL_NAME               L"66"
#define ONE_SECOND_TIMER  (10 * 1000 * 1000)

//<lvych007-20160415 BIOS screen lock function +>
#define ONE_SECOND_TIMER  (10 * 1000 * 1000)
typedef struct _SCREEN_LOCK_DATA{
  UINT8  LockInit;
  UINT16 LockTime;
  UINT16 LockCount;
  UINT32 PasswordInstalled;
  AMITSESETUP TSEVar;
  EFI_EVENT LockEvent;
} SCREEN_LOCK_DATA;
//<lvych007-20160415 BIOS screen lock function ->

#pragma pack(push)
#pragma	pack(1)
//<Wangbn001-20191030 Improved the code for password lock function to reduce NVRAM write+>
typedef struct _PASSWORD_LOCK_DATA
{
    UINT16 PLockTime;   //uint minute
    UINT8  PasswordLock;
    UINT8  Hour;
    UINT8  Minute;
    UINT8  Second;
}PASSWORD_LOCK_DATA;
//<Wangbn001-20191030 Improved the code for password lock function to reduce NVRAM write->

typedef struct _RECORD_PASSWORD_DATA
{
    UINT16 RecodeTotalNum;
    UINT16 RecodeIndex;
    UINT16 UserPassword[SETUP_PASSWORD_LENGTH];
    UINT16 AdminPassword[SETUP_PASSWORD_LENGTH];
    UINT16 PasswordData[SETUP_PASSWORD_MAX_RECODE_NUM][OEM_SETUP_PASSWORD_LENGTH];//liutz001_20190628_Repeat password can use issue
}RECORD_PASSWORD_DATA;

#pragma pack(pop)

#if defined(BIOS_SETUP_SCREEN_LOCK_SUPPORT)&& (BIOS_SETUP_SCREEN_LOCK_SUPPORT == 1) //<lvych00720160415+>
EFI_STATUS StartScreenLockTimer();
EFI_STATUS StopScreenLockTimer();
#endif //defined(BIOS_SETUP_SCREEN_LOCK_SUPPORT)&& (BIOS_SETUP_SCREEN_LOCK_SUPPORT == 1) //<lvych00720160415+>

#if defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1) //<lvych00620160310+>
UINT8 GetPasswordRetries();
UINT8 GetPasswordRepeatNumbers();
UINT8 GetPasswordComplexityMode();
BOOLEAN CheckPasswordComplexityLocal(CHAR16*Password,UINTN PwdLength);
BOOLEAN CheckPasswordRepeatLocal(CHAR16*Password,UINTN PwdLength);
//<lvych001-20180511 Add bios password lock function.+>
BOOLEAN CheckPasswordLock(UINTN *LeftLockTime);
EFI_STATUS SetPasswordLock(UINT8 LockData,UINT16 LockTime);
EFI_STATUS GetPasswordLockTime(
        UINTN *LockTime);
VOID OemReportInBox(
        UINTN PasswordLength,
        CHAR16 *OrgBoxStr,
        UINTN CurrXPos,
        UINTN CurrYPos,
        BOOLEAN bWaitForReturn,
        BOOLEAN bDisplayOff
        );
VOID ApplyOemPasswordPolicy(
    UINTN TotalRetries,
    UINTN *NoOfRetries,  
    UINTN PasswordLockTime,
    UINTN PasswordLength,
    UINTN CurrXPos,
    UINTN CurrYPos
);

EFI_STATUS GetFixedPasswordLockTime(
        UINTN *LockTime);

//<lvych001-20180511 Add bios password lock function.->
#endif //defined(OEM_SETUP_PASSWORD_POLICY_SUPPORT)&& (OEM_SETUP_PASSWORD_POLICY_SUPPORT == 1) //<lvych00620160310->


#endif 
