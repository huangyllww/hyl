//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2003, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//

#include "IpmiSensorReading.h"
#include "SensorReadingProtocol.h"
#include <TseSetupTransfer.h> //<lvych00120160908>
//
// Identify driver entry point
//
EFI_DRIVER_ENTRY_POINT (InitializeIpmiSensorReading);

typedef struct _EFI_IPMI_SENSOR_DATABASE_STRUCT {
	IPMI_SENSOR_RECORD_STRUCT       *Sdr;
	UINT8                           RecordType;        
} EFI_IPMI_SENSOR_DATABASE_STRUCT;

//Define Module Private data
EFI_IPMI_SENSOR_DATABASE_STRUCT       SDRDatabase[256];
#if defined(GET_SDR_DATA_STYLE) && (GET_SDR_DATA_STYLE == 2)  //<lvych00120160908+>
EFI_GUID gIpmiSensorProtocolGuid = { 0x1e81084c, 0x9f0, 0x4a03, { 0xa1, 0xef, 0x76, 0x8c, 0x2c, 0xdf, 0xbd, 0x50 } };
#elif defined(GET_SDR_DATA_STYLE) && (GET_SDR_DATA_STYLE == 1)
Tse_Setup_transfer *pTseSetuptransferInterface = NULL;
IPMI_GET_SDR_REQUEST                    GetSdrCommandDataEx;
#endif //<lvych00120160908->
CHAR16 UnitTbl[][24] = {
		L"unspecified", {0xB0, L"C"}, {0xB0, L"F"}, {0xB0, L"K"}, L"Volts", L"Amps", L"Watts", L"Joules", L"Couloms", L"VA", /* 0 ~ 9 */
		L"Nits", L"lumen", L"lux", L"Candela", L"kPa", L"PSI", L"Newton", L"CFM", L"RPM", L"Hz", /* 10 ~ 19 */
		L"microsecond", L"millisecond", L"second", L"miniute", L"hour", L"day", L"week", L"mil", L"inches", L"feet", /* 20 ~ 29 */
		L"cu in", L"cu feet", L"mm", L"cm", L"m", L"cu cm", L"cu m", L"liters", L"fluid ounce", L"radians", /* 30 ~ 39 */
		L"steradians", L"revolutions", L"cycles", L"gravities", L"ounce", L"pound", L"ft-lb", L"oz-in", L"gauss", L"gilberts",/* 40 ~ 49*/
		L"henry", L"milliherry", L"farad", L"microfarad", L"ohms", L"siemens", L"mole", L"becquerel", L"PPM(parts/million)", L"reserved",/* 50 ~ 59 */
		L"Decibels", L"DbA", L"DbC", L"gray", L"sievert", L"color temp deg K", L"bit", L"kilobit", L"megabit", L"gigabit", /* 60 ~ 69 */
		L"byte", L"kilobyte", L"megabyte", L"gigabyte", L"word(data)", L"dword", L"qword", L"line(re.mem. line)", L"hit", L"miss",/* 70 ~ 79 */
		L"retry", L"reset", L"overrun / overflow", L"underrun", L"collision", L"packets", L"messages", L"characters", L"error", L"correctable error", /* 80 ~ 89 */
		L"uncorrectable error", /* 90 */
};

EFI_IPMI_TRANSPORT         	    *mIpmiTransport;

UINT8	SensorFound=0;
EFI_GUID gEfiSetupGuid = SETUP_GUID;
EFI_GUID gEfiSensorReadingProtocolGuid=EFI_SENSOR_READING_PROTOCOL_GUID;

INT16 POW(INT16 x, INT16 y)
{
	INT16 val;
	val = 1;
	
	while(y-->0) {
		val = val*x;
	};
	
	return val;
}

//<lvych002-20161210 Revised str copy maybe overflow +>
CHAR16 *
EFIAPI
AsciiStrnToUnicodeStr (
  OUT     CHAR16                    *Destination,
  IN      CONST CHAR8               *Source,
  IN      UINTN                     Length
  )
{
  CHAR16                            *ReturnValue;

  if (Length == 0) {
    return Destination;
  }
  
  ASSERT (Destination != NULL);

  //
  // ASSERT Source is less long than PcdMaximumAsciiStringLength
  //
  ASSERT (AsciiStrSize (Source) != 0);

  //
  // Source and Destination should not overlap
  //
  ASSERT ((UINTN) ((CHAR8 *) Destination - Source) > AsciiStrLen (Source));
  ASSERT ((UINTN) (Source - (CHAR8 *) Destination) >= (AsciiStrSize (Source) * sizeof (CHAR16)));


  ReturnValue = Destination;
  while ((*Source != '\0') && (Length > 0)) {
    *(Destination++) = (CHAR16) *(Source++);
    Length--;
  }
  
  //
  // End the Destination with a NULL.
  //
  *Destination = '\0';
  
  //
  // ASSERT Original Destination is less long than PcdMaximumUnicodeStringLength
  //
  ASSERT (StrSize (ReturnValue) != 0);

  return ReturnValue;
}
//<lvych002-20161210 Revised str copy maybe overflow ->

//<lvych001-20161209 Revised HWM temperature display is not normal +>
VOID TraceSensorData(EFI_IPMI_SENSOR_DATABASE_STRUCT *SDRData, UINT8 RawData)
{
  UINTN     i;
  UINT8     *p;
  CHAR16    oSensorName[30];
  
  DEBUG ((-1,"\n"));
  DEBUG ((-1,"**************************RecordId:%d**************************\n",SDRData->Sdr->SensorType1.RecordId));
  AsciiStrnToUnicodeStr (oSensorName,(CHAR8 *)SDRData->Sdr->SensorType1.AsciiIdString,16);
  oSensorName[16]= '\0';
  DEBUG ((-1,"Name:%s         \n",oSensorName));
  DEBUG ((-1,"RecordType:%d   \n",SDRData->RecordType));
  DEBUG ((-1,"RawData:%d      \n",RawData));
  DEBUG ((-1,"Linearization:%d\n",SDRData->Sdr->SensorType1.Linearization));
  DEBUG ((-1,"MLo:%d          \n",SDRData->Sdr->SensorType1.MLo));
  DEBUG ((-1,"Toleremce:%d    \n",SDRData->Sdr->SensorType1.Toleremce));
  DEBUG ((-1,"MHi:%d          \n",SDRData->Sdr->SensorType1.MHi));  
  DEBUG ((-1,"BLo:%d          \n",SDRData->Sdr->SensorType1.BLo));
  DEBUG ((-1,"AccuracyLow:%d  \n",SDRData->Sdr->SensorType1.AccuracyLow));
  DEBUG ((-1,"BHi:%d          \n",SDRData->Sdr->SensorType1.BHi));
  DEBUG ((-1,"AccuracyExp:%d  \n",SDRData->Sdr->SensorType1.AccuracyExp));  
  DEBUG ((-1,"AccuracyHi:%d   \n",SDRData->Sdr->SensorType1.AccuracyHi));
  DEBUG ((-1,"BExp:%d         \n",SDRData->Sdr->SensorType1.BExp));
  DEBUG ((-1,"RExp:%d         \n",SDRData->Sdr->SensorType1.RExp));
  p = (UINT8*)SDRData->Sdr;
  for(i=0;i<0x80;i++,p++){
    if((i!=0)&&(i%0x10==0))
      DEBUG ((-1,"\n"));
    DEBUG ((-1,"%02X ",*p));
  }
  DEBUG ((-1,"\n"));
}
//<lvych001-20161209 Revised HWM temperature display is not normal ->
//----------------------------------------------------------------------------
// Name:  CalculateSensorReading
//----------------------------------------------------------------------------
EFI_STATUS
CalculateSensorReading(
	UINT16 SensorNumber,
	UINT8 RawData,
	CHAR16 *Val)
{
	INT16 M, B, K1, K2, L, CommonFactor, PureVal;
	INTN StrLength, Idx, Compl;
	
	L=0;
	CommonFactor=0;
	Idx=0;
	Compl=0;
			
//<lvych001-20161209 Revised HWM temperature display is not normal +>		
	if (SDRDatabase[SensorNumber].RecordType == 0x1) {
	  
		//TraceSensorData(&SDRDatabase[SensorNumber],RawData);
		
		L = SDRDatabase[SensorNumber].Sdr->SensorType1.Linearization;
	
		M = (SDRDatabase[SensorNumber].Sdr->SensorType1.MHi) << 8 | SDRDatabase[SensorNumber].Sdr->SensorType1.MLo;
		if (M & BIT09)
			M = (BIT10-M)*-1; 
		
		B = (SDRDatabase[SensorNumber].Sdr->SensorType1.BHi) << 8 | SDRDatabase[SensorNumber].Sdr->SensorType1.BLo;
		if (B & BIT09)
			B = (BIT10-B)*-1;
		
		K1 = SDRDatabase[SensorNumber].Sdr->SensorType1.BExp;
		if (K1 & BIT03)
			K1 = (BIT04-K1) * -1;
			
		K2 = SDRDatabase[SensorNumber].Sdr->SensorType1.RExp;
		if (K2 & BIT03)
			K2 = (BIT04-K2) * -1;
		
		if(K2 < 0 || (K1+K2) < 0)
		{
			CommonFactor = MAX(ABS(K2), ABS(K1+K2));
		}
	} else if (SDRDatabase[SensorNumber].RecordType == 0x2) {
#if 0		
		M = (SDRDatabase[SensorNumber].Sdr->SensorType2.MHi) << 8 | SDRDatabase[SensorNumber].Sdr->SensorType2.MLo;
		if (M & BIT09)
			M = (BIT10-M)*-1; 
		
		B = (SDRDatabase[SensorNumber].Sdr->SensorType2.BHi) << 8 | SDRDatabase[SensorNumber].Sdr->SensorType2.BLo;
		if (B & BIT09)
			B = (BIT10-B)*-1;
		
		K1 = SDRDatabase[SensorNumber].Sdr->SensorType2.BExp;
		if (K1 & BIT03)
			K1 = (BIT04-K1)*-1;
			
		K2 = SDRDatabase[SensorNumber].Sdr->SensorType2.RExp;
		if (K2 & BIT03)
			K2 = (BIT04-K2)*-1;
		
		if(K2 < 0 || (K1+K2) < 0)
		{
			CommonFactor = MAX(ABS(K2), ABS(K1+K2));
		}
#endif
		return  EFI_INVALID_PARAMETER;		
	}
//<lvych001-20161209 Revised HWM temperature display is not normal ->	
	if(L == 0) {		
		PureVal = M * RawData * POW(10, K2 + CommonFactor) + B * POW(10, K1+K2+CommonFactor);
		UnicodeSPrint(Val, 29, L"%d", PureVal);
		StrLength = StrLen(Val);
		Idx = StrLength;
		
		if(StrLength < CommonFactor + 1) {
			Compl = CommonFactor + 1 - StrLength;
			StrLength = CommonFactor + 1;
		}
		

		if(CommonFactor) {
			do {
				Val[Idx + Compl + 1] = Val[Idx];
				Idx--;
			} while( Idx>=0 && Idx >= (StrLength - CommonFactor - Compl)); // Bull of shit here, Ugly!!!

			if(Compl){
				for(Idx = 0; Idx<= Compl; Idx++)
				Val[Idx] = L'0';
			}
			Val[StrLength - CommonFactor] = L'.';                // Only a dot added
		}
	} else
		return  EFI_INVALID_PARAMETER;
	
	return EFI_SUCCESS;
}


//----------------------------------------------------------------------------
// Name:  ParseSDRRepository
//----------------------------------------------------------------------------
EFI_STATUS
ParseSDRRepository (
  VOID )
{
    EFI_STATUS                          Status;
    UINT16                              ReserveId=0;
    UINT8                               ReserveIdSize = sizeof (ReserveId);
    IPMI_GET_SDR_REQUEST                GetSdrCommandData={0};
    UINT8                               GetSdrResponseData[0xFF];
    UINT8                               GetSdrResponseDataSize=sizeof (GetSdrResponseData);
    IPMI_SENSOR_RECORD_STRUCT           *SdrRecord=NULL;
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 2)  //<lvych00120160908+>
    EFI_IPMI_SENSOR_PROTOCAL 			*mIpmiSensorProtocol=NULL;	
	UINT16								SensorNum = 0;
#endif //<lvych00120160908->
    SetMem((VOID *)SDRDatabase, sizeof(EFI_IPMI_SENSOR_DATABASE_STRUCT) * 256, 0);
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 2)  //<lvych00120160908+>
  //
  // This is BMC SDR records.
  //
    Status = gBS->LocateProtocol(&gIpmiSensorProtocolGuid,NULL, &mIpmiSensorProtocol);
  if(!EFI_ERROR(Status)) {
	  mIpmiSensorProtocol->GetIpmiSensorData((void*)SDRDatabase,&SensorNum);
	  SensorFound = (UINT8)SensorNum;
	  return Status;
  }
#endif //<lvych00120160908->
    //
    //
    // Get Reservation ID for Get SDR Info command
    //
    Status = mIpmiTransport->SendIpmiCommand (
				mIpmiTransport,
				IPMI_NETFN_STORAGE,
				BMC_LUN,
				IPMI_STORAGE_RESERVE_SDR_REPOSITORY,
				NULL,
				0,
				(UINT8 *) &ReserveId,
				(UINT8 *) &ReserveIdSize );

    //
    //  Fill Get SDR command Data
    //
    GetSdrCommandData.ReservationId = ReserveId; // Reservation ID
    GetSdrCommandData.RecordId = 0; // Record ID
    GetSdrCommandData.RecordOffset = 0; // Record Offset
    GetSdrCommandData.BytesToRead = 0xff; // Bytes to read

    while ( GetSdrCommandData.RecordId != 0xFFFF ) {

    	GetSdrResponseDataSize = sizeof (GetSdrResponseData);

        //
        // Clear the GetSdrResponseData
        //
        ZeroMem (&GetSdrResponseData[0], GetSdrResponseDataSize);

    	//
    	// Get SDR Info command to Get FRU DATA
    	//
    	Status = mIpmiTransport->SendIpmiCommand (
    				mIpmiTransport,
    				IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    IPMI_STORAGE_GET_SDR,
                    (UINT8 *) &GetSdrCommandData,
     		        sizeof (IPMI_GET_SDR_REQUEST),
                    (UINT8 *) &GetSdrResponseData,
                    (UINT8 *) &GetSdrResponseDataSize );
    	
    	if ( EFI_ERROR (Status) ) {
    		return Status;
    	}
    	
    	SdrRecord = (IPMI_SENSOR_RECORD_STRUCT *) (GetSdrResponseData + sizeof (UINT16)); //Exclude (UINT16) NextRecordId variable and assign
		//
		// Got the required SDR AsciiIdString in ASCII format
		// Convert it to Unicode format
		//
        if ( ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordType == 0x1 ) { //Type 01
    		SDRDatabase[SensorFound].Sdr = AllocateRuntimeCopyPool (sizeof(IPMI_SDR_RECORD_STRUCT_1), (UINT8 *)SdrRecord);
    		ASSERT (SDRDatabase[SensorFound].Sdr != NULL);
    		SDRDatabase[SensorFound].RecordType = 0x1;
    		SensorFound++;
    	} /*else if  ( ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordType == 0x2 ) { //Type 02
    		SDRDatabase[SensorFound].Sdr = AllocateRuntimeCopyPool (sizeof(EFI_IPMI_SDR_RECORD_STRUCT_2), (UINT8 *)SdrRecord);
    		ASSERT (SDRDatabase[SensorFound].Sdr != NULL);
    		SDRDatabase[SensorFound].RecordType = 0x2;
    		SensorFound++;
    	}*/ //Remove Sensors whose SDR Recore type = 2
    	
    	//
    	// Update the RecordId for the GET SDR command
    	//
    	if ( GetSdrCommandData.RecordId == ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->NextRecordId ) {
    		break;
    	} else {
    		GetSdrCommandData.RecordId = ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->NextRecordId; // Record ID
    	}
    } //while ( GetSdrCommandData.RecordId != 0xFFFF )
		return EFI_SUCCESS;
}

#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
//
//----------------------------------------------------------------------------
// Name:  HWGetNextSDRData
//----------------------------------------------------------------------------
EFI_STATUS
HWGetNextSDRData (
    BOOLEAN InitFlag,
    UINT8 *RecordType,
    UINT16  *NextRecordId
)
{
    EFI_STATUS                          Status;
    EFI_STATUS                          Status1 = EFI_SUCCESS;
    UINT16                              ReserveId=0;
    UINT8 				                ReserveIdSize = sizeof (ReserveId);
    UINT8				                GetSdrResponseData[0xFF];
    UINT8				                GetSdrResponseDataSize=sizeof (GetSdrResponseData);
    IPMI_SENSOR_RECORD_STRUCT           *SdrRecord=NULL;
    EFI_GUID                            gTseTransferGuid = TSE_TRANSFER_GUID;  
	HM_V_DATA						    HMSetup;
    HM_V_DATA                           *pHMSetup;
	UINTN               			    VariableSize;

    
    if(pTseSetuptransferInterface == NULL){
        Status1 = gBS->LocateProtocol(&gTseTransferGuid, NULL, &pTseSetuptransferInterface);
        ASSERT( !EFI_ERROR( Status1 ));
    }
    if(!EFI_ERROR(Status1))
    {
        VariableSize = sizeof(HMSetup);
        Status1 = pTseSetuptransferInterface->VarGetBuffer(L"HWMonitor",gEfiSetupGuid,(UINT8**)&pHMSetup,&VariableSize);
        ASSERT( !EFI_ERROR( Status1 ));
    }
    
    *NextRecordId = 0xFFFF;
    *RecordType = 0;
    if(InitFlag){
		SensorFound  = 0;  
		SetMem((VOID *)SDRDatabase, sizeof(EFI_IPMI_SENSOR_DATABASE_STRUCT) * 256, 0);
		SetMem((VOID *)&GetSdrCommandDataEx, sizeof(IPMI_GET_SDR_REQUEST), 0);
		
		//
		//
		// Get Reservation ID for Get SDR Info command
		//
		Status = mIpmiTransport->SendIpmiCommand (
					mIpmiTransport,
					IPMI_NETFN_STORAGE,
					BMC_LUN,
					IPMI_STORAGE_RESERVE_SDR_REPOSITORY,
					NULL,
					0,
					(UINT8 *) &ReserveId,
					(UINT8 *) &ReserveIdSize );
	
		//
		//  Fill Get SDR command Data
		//
		GetSdrCommandDataEx.ReservationId = ReserveId; // Reservation ID
		GetSdrCommandDataEx.RecordId = 0; // Record ID
		GetSdrCommandDataEx.RecordOffset = 0; // Record Offset
		GetSdrCommandDataEx.BytesToRead = 0xff; // Bytes to read
        
        if(!EFI_ERROR(Status1))
        {
            pHMSetup->SensorFound = SENSOR_ENTRY_NUMBER;
        }

    }
    
    if( GetSdrCommandDataEx.RecordId != 0xFFFF ) {

    	GetSdrResponseDataSize = sizeof (GetSdrResponseData);

        //
        // Clear the GetSdrResponseData
        //
        ZeroMem (&GetSdrResponseData[0], GetSdrResponseDataSize);

    	//
    	// Get SDR Info command to Get FRU DATA
    	//
    	Status = mIpmiTransport->SendIpmiCommand (
    				mIpmiTransport,
    				IPMI_NETFN_STORAGE,
                    BMC_LUN,
                    IPMI_STORAGE_GET_SDR,
                    (UINT8 *) &GetSdrCommandDataEx,
     		        sizeof (IPMI_GET_SDR_REQUEST),
                    (UINT8 *) &GetSdrResponseData,
                    (UINT8 *) &GetSdrResponseDataSize );
    	
    	if ( EFI_ERROR (Status) ) {
    		return Status;
    	}
    	
    	SdrRecord = (IPMI_SENSOR_RECORD_STRUCT *) (GetSdrResponseData + sizeof (UINT16)); //Exclude (UINT16) NextRecordId variable and assign
		//
		// Got the required SDR AsciiIdString in ASCII format
		// Convert it to Unicode format
		//
        if ( ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordType == 0x1 ) { //Type 01
    		SDRDatabase[SensorFound].Sdr = AllocateRuntimeCopyPool (sizeof(IPMI_SDR_RECORD_STRUCT_1), (UINT8 *)SdrRecord);
    		ASSERT (SDRDatabase[SensorFound].Sdr != NULL);
    		SDRDatabase[SensorFound].RecordType = 0x1;
    		SensorFound++;
    	} /*else if  ( ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordType == 0x2 ) { //Type 02
    		SDRDatabase[SensorFound].Sdr = AllocateRuntimeCopyPool (sizeof(EFI_IPMI_SDR_RECORD_STRUCT_2), (UINT8 *)SdrRecord);
    		ASSERT (SDRDatabase[SensorFound].Sdr != NULL);
    		SDRDatabase[SensorFound].RecordType = 0x2;
    		SensorFound++;
    	}*/ //Remove Sensors whose SDR Recore type = 2
    	
    	//
    	// Update the RecordId for the GET SDR command
    	//

    	if ( GetSdrCommandDataEx.RecordId != ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->NextRecordId ) {
    		GetSdrCommandDataEx.RecordId = ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->NextRecordId; // Record ID
    		*NextRecordId = GetSdrCommandDataEx.RecordId;
    	}
        *RecordType = ((EFI_IPMI_GET_SDR_RESPONSE_WITH_RECORD_HDR*)GetSdrResponseData)->RecordHeader.RecordType;
    }
    
    if(*NextRecordId==0xFFFF){
        if(!EFI_ERROR(Status1))
        {
              pHMSetup->SensorFound = SensorFound;
        }
		  VariableSize = sizeof(HMSetup);
		  Status = gRT->GetVariable( L"HWMonitor", &gEfiSetupGuid, NULL,
		                      &VariableSize, &HMSetup );
		  
        if(HMSetup.SensorFound != SensorFound){
		  HMSetup.SensorFound = SensorFound;
		  
		  Status = gRT->SetVariable( L"HWMonitor", &gEfiSetupGuid,
						EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,   // Attributes
						VariableSize,
						&HMSetup);
        }
    }
    
    return EFI_SUCCESS;
}
#endif //<lvych00120160908->

EFI_STATUS HWGetSensorData (
  SENSOR_READING_PROTOCOL *This,
  UINT8 RecordNumber,
  CHAR16 *SensorData
)
{	
  EFI_STATUS                        Status;
  UINT8				    Command[10];
  UINT8                             Response[20];
  UINT32                            ResponseSize;
  
  UINT8				    Retry=0;

  if (SDRDatabase[RecordNumber].RecordType != 0x1) {
	  SensorData[0] = L'\0';
	  return EFI_NOT_FOUND;
  }
  
  Command[0] = SDRDatabase[RecordNumber].Sdr->SensorType1.SensorNumber;
  
  Status = EFI_SUCCESS;
  
  do {
      Status = mIpmiTransport->SendIpmiCommand (
	mIpmiTransport,
	EFI_SM_SENSOR_EVENT,
	BMC_LUN,
	0x2d,//Get Sensor Reading Command
	(UINT8*) &Command,
	sizeof(UINT8),
	(UINT8*) &Response,
	(UINT8*) &ResponseSize
       );
       Retry++;
       MicroSecondDelay (IPMI_KCS_DELAY_PER_RETRY);  //Default delay is 5ms
  } while(EFI_ERROR(Status) && Retry<IPMI_SENSOR_READING_RETRY); //BMC doesn't respond in time sometimes
  
  if(!EFI_ERROR(Status)) {
	  if(Response[1] & 0x20)
	      return EFI_NOT_FOUND;
	  
	  Status = CalculateSensorReading(RecordNumber, Response[0], SensorData);//*SensorData = Response[0];
	  if(EFI_ERROR(Status))
		 SensorData[0] = L'\0';
  }
  else
	  SensorData[0] = L'\0';
  return Status;
}

EFI_STATUS HWGetSensorName (
  SENSOR_READING_PROTOCOL *This,
  UINT8 RecordNumber,
  CHAR16 *SensorName
)
{
//<lvych002-20161210 Revised str copy maybe overflow +>
    UINTN Length;
	if(RecordNumber > 256)
		return  EFI_NOT_FOUND;
	
	if(NULL == SensorName)
		return  EFI_INVALID_PARAMETER;
	
	if(SDRDatabase[RecordNumber].RecordType == 0x1) { //Type 0x1
	  Length = (SDRDatabase[RecordNumber].Sdr->SensorType1.IdStringLength & 0x1F);
	  AsciiStrnToUnicodeStr (SensorName,(CHAR8 *)SDRDatabase[RecordNumber].Sdr->SensorType1.AsciiIdString,Length); 
	} else if(SDRDatabase[RecordNumber].RecordType == 0x2) { //Type 0x2
      Length = (SDRDatabase[RecordNumber].Sdr->SensorType2.IdStringLength & 0x1F);
	  AsciiStrnToUnicodeStr (SensorName,(CHAR8 *)SDRDatabase[RecordNumber].Sdr->SensorType2.AsciiIdString,Length);
	} else 
		return EFI_NOT_FOUND;
//<lvych002-20161210 Revised str copy maybe overflow ->	
	return EFI_SUCCESS;
}

EFI_STATUS HWGetSensorUnit(
  SENSOR_READING_PROTOCOL *This,
  UINT8 RecordNumber,
  CHAR16 *SensorUnit		
)
{
	if(RecordNumber > 256)
		return  EFI_NOT_FOUND;
	
	if(NULL == SensorUnit)
		return  EFI_INVALID_PARAMETER;

	if(SDRDatabase[RecordNumber].RecordType == 0x1) { //Type 0x1
		StrCpy (SensorUnit, UnitTbl[SDRDatabase[RecordNumber].Sdr->SensorType1.UnitType]);
	} else if(SDRDatabase[RecordNumber].RecordType == 0x2) { //Type 0x2
		StrCpy (SensorUnit, UnitTbl[SDRDatabase[RecordNumber].Sdr->SensorType2.UnitType]);;		
	} else 
		return EFI_NOT_FOUND;
	return EFI_SUCCESS;
}

UINT8 HWGetSensorFound (
  SENSOR_READING_PROTOCOL *This
)
{
	return SensorFound;
}

EFI_STATUS HWInitSDRData(SENSOR_READING_PROTOCOL *This)
{
	EFI_STATUS                            Status;
	
	HM_V_DATA								HMSetup;
	UINTN               					VariableSize;
	
	  //
	  // Read the SDR Version. This is BMC SDR records dependent.
	  //
	  Status = ParseSDRRepository();
	  
	  if(EFI_ERROR(Status)) {
		  return Status;
	  }
	  
	  VariableSize = sizeof(HMSetup);
	  Status = gRT->GetVariable( L"HWMonitor", &gEfiSetupGuid, NULL,
	                      &VariableSize, &HMSetup );
	  
	  HMSetup.SensorFound = SensorFound;
	  
	  Status = gRT->SetVariable( L"HWMonitor", &gEfiSetupGuid,
					EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,   // Attributes
					VariableSize,
					&HMSetup);
		
	  if (Status != EFI_SUCCESS) {
	      return EFI_UNSUPPORTED;
	  }
	  
	  return  EFI_SUCCESS;
}

SENSOR_READING_PROTOCOL SensorReadingProtocol = {
  HWInitSDRData,		
  HWGetSensorData,
  HWGetSensorName,
  HWGetSensorFound,
  HWGetSensorUnit,
#if defined(GET_SDR_DATA_STYLE)&& (GET_SDR_DATA_STYLE == 1) //<lvych00120160908+>
  HWGetNextSDRData
#endif //<lvych00120160908->
};

EFI_STATUS
InitializeIpmiSensorReading (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
)
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            ThisHandle;
  ThisHandle = NULL;
  
  Status = gBS->LocateProtocol (
  				&gEfiDxeIpmiTransportProtocolGuid,
  				NULL,
  				&mIpmiTransport );
  if ( EFI_ERROR (Status) ) {
  	return Status;
  }
  
  Status = gBS->InstallProtocolInterface(
			&ThisHandle,
			&gEfiSensorReadingProtocolGuid,
			EFI_NATIVE_INTERFACE,
			&SensorReadingProtocol);
  
  return EFI_SUCCESS;
}

//**********************************************************************//
//**********************************************************************//
//**                                                                  **//
//**        (C)Copyright 1985-2003, American Megatrends, Inc.         **//
//**                                                                  **//
//**                       All Rights Reserved.                       **//
//**                                                                  **//
//**             6145-F Northbelt Pkwy, Norcross, GA 30071            **//
//**                                                                  **//
//**                       Phone: (770)-246-8600                      **//
//**                                                                  **//
//**********************************************************************//
//**********************************************************************//
