/** @file

  This file contains Oem RTC initialization Hooks implementation
  
  $Revision:0A
  $Date:  Lizhq 2018/07/21
  
  Copyright (C) 2018 Dawning Information Industry Co.,LTD.All rights reserved.<BR>
  
**/

#include <AmiLib.h>
#include <AmiDxeLib.h>
#include <Token.h>
#include <Protocol/RealTimeClock.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#define RTC_INIT_SECOND 0
#define RTC_INIT_MINUTE 0
#define RTC_INIT_HOUR   0
#define RTC_INIT_DAY    1
#define RTC_INIT_MONTH  1

/**
  Called by gEfiRealTimeClockArchProtocolGuid protocol notify.

  Restore default date if RTC power status bit was set..

  @param  Event    Event triggered.
  @param  Context  Parameter passed with the event.

**/
VOID EFIAPI RestoreDefaultDate (
    IN EFI_EVENT Event, 
    IN VOID *Context)
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_TIME            EfiTime = {0};
  BOOLEAN             CmosClear = FALSE;
  
  CmosClear = PcdGetBool(PcdCmosClear);
  DEBUG((EFI_D_INFO, __FUNCTION__" Start CMOS Clear flag:0x%x\n", CmosClear));
  if(CmosClear) {
    pRS->GetTime(&EfiTime, NULL);
    
    EfiTime.Second = RTC_INIT_SECOND;
    EfiTime.Minute = RTC_INIT_MINUTE;
    EfiTime.Hour   = RTC_INIT_HOUR;
    EfiTime.Day    = RTC_INIT_DAY;
    EfiTime.Month  = RTC_INIT_MONTH;
    EfiTime.Year   = DEFAULT_TIME_PROJECT_YEAR;  //DEFAULT_TIME_PROJECT_YEAR should be defined by project SDL
    EfiTime.Nanosecond  = 0;
    EfiTime.TimeZone = EFI_UNSPECIFIED_TIMEZONE;
    EfiTime.Daylight = 0;
    pRS->SetTime(&EfiTime);
  }
            
  if(Event) {
    pBS->CloseEvent(Event);
    Event=NULL; 
  }
	DEBUG((EFI_D_INFO, __FUNCTION__" End\n"));
}

/**
  The module Entry Point of install gEfiRealTimeClockArchProtocolGuid protocol notify.

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Other          Some error occurs when executing this entry point.

**/
EFI_STATUS OemRTCArchProtocolNotifyEntryPoint (
    IN EFI_HANDLE       ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  
  EFI_EVENT   Event;
  VOID        *Registration;
  DEBUG((EFI_D_INFO, __FUNCTION__" Start \n"));

  InitAmiLib(ImageHandle, SystemTable);
  
  Status = RegisterProtocolCallback(
            &gEfiRealTimeClockArchProtocolGuid,
            RestoreDefaultDate,
            NULL, &Event, &Registration
            );
  DEBUG((EFI_D_INFO, __FUNCTION__" End:%r \n", Status));
  
  return Status;
}
