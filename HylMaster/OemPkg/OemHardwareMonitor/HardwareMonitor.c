//BIOS Change History                                                       
//---------------------------------------------------------------------------- 
//Who        When        Rel    Description                           
//---------  ----------  -----  ---------------------------------------------- 
//Machong	 2013/05/20  0.03   Refresh

/************************************************************************
 * Oem HWMonitor Module                                               *
 ************************************************************************/

#include "HardwareMonitor.h"
#include "HardwareMonitorVarType.h"
#include "IpmiSensorReading\SensorReadingProtocol.h"

#define COMMAND_DATA_SIZE        20
#define RESPONSE_DATA_SIZE       20  

#define STRING_TOKEN( x ) x

#define SENSOR_NAME_STRING(x) STR_SENSOR_NAME_##x##_STRING
#define SENSOR_VALUE_STRING(x) STR_SENSOR_VALUE_##x##_STRING

#if APTIO_4
EFI_GUID  gEfiRedirSensorProtocolGuid   = EFI_SM_SENSOR_REDIR_PROTOCOL_GUID;
EFI_GUID  gAdvancedFormSetGuid = ADVANCED_FORM_SET_GUID;


EFI_HII_STRING_PROTOCOL     *mHiiString=NULL;
EFI_HII_DATABASE_PROTOCOL   *mHiiDatabase=NULL;

EFI_HANDLE HWImageHandle=NULL;
EFI_HII_HANDLE HWHiiHandle=NULL;
#endif
SENSOR_READING_PROTOCOL *mSensorReadingProtocol=NULL;

BOOLEAN SensorInitialized=FALSE;
UINT8 SensorFounded;
UINT8 gUpdateIdx=0;

//Machong 2014/11/19 Optimizing Token display using Sensor Value Flags {
UINT8 gSensorOK[SENSOR_ENTRY_NUMBER] = {0};
//Machong 2014/11/19 Optimizing Token display using Sensor Value Flags {

VOID
HWInitString (
  IN EFI_HII_HANDLE HiiHandle,
  IN STRING_REF     StrRef,
  IN CHAR16         *sFormat,
  IN ...
  )
{
    CHAR16 s[1024];
    va_list ArgList = va_start(ArgList,sFormat);
    Swprintf_s_va_list(s,sizeof(s),sFormat,ArgList);
    va_end(ArgList);
#if EFI_SPECIFICATION_VERSION>0x20000
    HiiLibSetString(HiiHandle, StrRef, s);
#else
	pHii->NewString(pHii, L"   ", HiiHandle, &StrRef, s);
#endif
}

EFI_GUID gEfiSensorReadingProtocolGuid = EFI_SENSOR_READING_PROTOCOL_GUID;

EFI_STATUS HWMonitorCallbackFunction(
    EFI_HII_HANDLE HiiHandle, 
    UINT16 Class, 
    UINT16 SubClass, 
    UINT16 Key)
{
  EFI_STATUS Status;
//  UINT8      SensorData;
  CHAR16     SensorName[30]; //BMC Engineer sometimes may violate the specification, so we should be careful
  CHAR16     SensorUnit[24];
  CHAR16	 SensorValue[30];
  UINT8		 SensorIndex;
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
  UINT16	 NextRecordId=0;
  UINT8	 	 RecordType=0;
#endif //<lvych00120160908->
  CALLBACK_PARAMETERS         *CallbackParameters = GetCallbackParameters();
  
  //
  // Check for the key and Return if Key value does not match
  //
  if ( Key != SENSOR_VALUE_KEY ) {
	  DEBUG ((-1,"HWMonitor: HWMonitorCallbackFunction is called with \
              Wrong Key Value. Returning EFI_UNSUPPORTED\n"));
      return EFI_UNSUPPORTED;
  }

  //
  // Get the call back parameters and verify the action
  //
  if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING ) {
      DEBUG ((-1,"HWMonitorCallbackFunction: CallbackParameters->Action != \
              EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
      return  EFI_SUCCESS;
  }
//Machong optimize Reading Sensor Data Progress {
  if(mSensorReadingProtocol == NULL) {
    Status = gBS->LocateProtocol(&gEfiSensorReadingProtocolGuid,NULL, &mSensorReadingProtocol);
    if(EFI_ERROR(Status)) return  Status;
  }
  
  if(Key == SENSOR_VALUE_KEY)
  {
	  if(!SensorInitialized) {
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
	      Status = mSensorReadingProtocol->GetNextSDRData(FALSE,&RecordType,&NextRecordId);
	      if(EFI_ERROR(Status)) return  Status;
	      //skip not type1 data
	      if((RecordType!=1) && (NextRecordId != 0xFFFF)) return  Status; //<lvych001-20180331 Revised hardware monitor information shows incomplete.>

		  mSensorReadingProtocol->GetSensorName(mSensorReadingProtocol, gUpdateIdx, SensorName);
		  SensorName[16] = L'\0';
		  HWInitString(HiiHandle, \
				  STRING_TOKEN(SENSOR_NAME_STRING(1) + 2 * gUpdateIdx), \
				  SensorName
				  );

		  if(NextRecordId == 0xFFFF){
			  SensorFounded = mSensorReadingProtocol->GetSensorFound(mSensorReadingProtocol);
			  if(SensorFounded > SENSOR_ENTRY_NUMBER)
				  SensorFounded = SENSOR_ENTRY_NUMBER;
			  //may be lost some data need update
			  for(SensorIndex=gUpdateIdx+1; SensorIndex<SensorFounded; SensorIndex++) {
				  mSensorReadingProtocol->GetSensorName(mSensorReadingProtocol, SensorIndex, SensorName);
				  SensorName[16] = L'\0';
				  HWInitString(HiiHandle, \
						  STRING_TOKEN(SENSOR_NAME_STRING(1) + 2 * SensorIndex), \
						  SensorName
				   	   	  );
			  }
			  SensorInitialized = TRUE;
		  }
#else
		  SensorFounded = mSensorReadingProtocol->GetSensorFound(mSensorReadingProtocol);
		  if(SensorFounded > SENSOR_ENTRY_NUMBER)
			  SensorFounded = SENSOR_ENTRY_NUMBER;
		  
		  for(SensorIndex=0; SensorIndex<SensorFounded; SensorIndex++) {
			  mSensorReadingProtocol->GetSensorName(mSensorReadingProtocol, SensorIndex, SensorName);
			  SensorName[16] = L'\0';
			  HWInitString(HiiHandle, \
					  STRING_TOKEN(SENSOR_NAME_STRING(1) + 2 * SensorIndex), \
					  SensorName
			   	   	  );
		  }
		  SensorInitialized = TRUE;	
		  return EFI_SUCCESS;
#endif //<lvych00120160908->
	  }
	  gBS->SetMem(SensorUnit, 24, 0);
	  //Now Update Sensor Value String Token
	  Status = mSensorReadingProtocol->GetSensorData(mSensorReadingProtocol, gUpdateIdx, SensorValue);
	  if(!EFI_ERROR(Status)) {
		  Status = mSensorReadingProtocol->GetSensorUnit(mSensorReadingProtocol, gUpdateIdx, SensorUnit);
		  HWInitString(HiiHandle, \
			  STRING_TOKEN(SENSOR_VALUE_STRING(1) + 2 * gUpdateIdx), \
			  L"%s %s", SensorValue,SensorUnit\
			  );
		  //Flag the sensor we succeeded to read
		  gSensorOK[gUpdateIdx] = 1;
		  
	  } else if(gSensorOK[gUpdateIdx] != 1){
		  HWInitString(HiiHandle, \
			  STRING_TOKEN(SENSOR_VALUE_STRING(1) + 2 * gUpdateIdx), \
			  L"Not Available"\
			  );
	  } 
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
	  if(!SensorInitialized){
		  gUpdateIdx++;
	  }else{
		  gUpdateIdx = (gUpdateIdx + 1) % SensorFounded;
	  }
#else
	  gUpdateIdx = (gUpdateIdx + 1) % SensorFounded;
#endif	 //<lvych00120160908->
  }

  return EFI_SUCCESS;
}

EFI_STATUS InitSensorCallbackFunction(
    EFI_HII_HANDLE HiiHandle, 
    UINT16 Class, 
    UINT16 SubClass, 
    UINT16 Key)
{
  EFI_STATUS Status;
  CALLBACK_PARAMETERS         *CallbackParameters = GetCallbackParameters();
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
  UINT16	 NextRecordId=0;
  UINT8	 	 RecordType=0;
#endif //<lvych00120160908->
  Status = EFI_SUCCESS;
  DEBUG ((-1, "InitSensor: InitSensorCallbackFunction() Entrance\n"));

  //
  // Check for the key and Return if Key value does not match
  //
  if ( Key != VIEW_HW_MONITOR_FORM_KEY ) {
	  DEBUG ((-1,"InitSensor: Callback function is called with \
              Wrong Key Value. Returning EFI_UNSUPPORTED\n"));
      return EFI_UNSUPPORTED;
  }

  //
  // Get the call back parameters and verify the action
  //
  if ( CallbackParameters->Action != EFI_BROWSER_ACTION_CHANGING ) {
	  DEBUG ((-1,"InitSensor: CallbackParameters->Action != \
              EFI_BROWSER_ACTION_CHANGING... so return EFI_SUCCESS\n"));
      return  EFI_SUCCESS;
  }
  
  //Machong optimize Reading Sensor Data Progress {
    if(mSensorReadingProtocol == NULL) {
      Status = gBS->LocateProtocol(&gEfiSensorReadingProtocolGuid,NULL, &mSensorReadingProtocol);
      if(EFI_ERROR(Status)) {
      //<Lizhq037-20170106 Can not enter Hardware Monitor control panel if BMC was disabled. +>
        HWInitString(HiiHandle, STRING_TOKEN(STR_GET_BMC_SDR_VALUE), L"Sensor Not Found");
        return  EFI_SUCCESS;
      //<Lizhq037-20170106 Can not enter Hardware Monitor control panel if BMC was disabled. ->
      }
#if defined(GET_SDR_DATA_STYLE) && (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
      Status = mSensorReadingProtocol->GetNextSDRData(TRUE,&RecordType,&NextRecordId);
#else
      Status = mSensorReadingProtocol->InitSDRData(mSensorReadingProtocol);    
#endif //<lvych00120160908->
      if(EFI_ERROR(Status)) return  Status;
    }
  
  return EFI_SUCCESS;
}
