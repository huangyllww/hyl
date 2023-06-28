#ifndef __OEM_SETUP_IPMI_H__
#define __OWM_SETUP_IPMI_H__

#include "Token.h"

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/IPMITransportProtocol.h>
#include <Protocol/Variable.h>

#include <IpmiNetFnChassisDefinitions.h>

#include <AmiDxeLib.h>
#define  EFI_TPL_CALLBACK 8

#define SET_SYSTEM_BOOT_OPTIONS_COMMAND			0x8
#define GET_SYSTEM_BOOT_OPTIONS_COMMAND			0x9
	#define ONE_OF_TWO_SELECTOR						0x61
		#define ONE_OF_TWO_COUNT						0x15
	#define ONE_OF_FOUR_SELECTOR					0x62
		#define ONE_OF_FOUR_COUNT						0x0
	#define ONE_OF_SIXTEEN_SELECTOR					0x63
		#define ONE_OF_SIXTEEN_COUNT					0x2

typedef struct {
	UINT8 selectorid;
	UINT8 groupid;
	UINT8 optlength;
	UINT8 optcount;
	UINT8 mask;
	UINT8 datalength;//IPMI Response & Request Data Length
} IPMI_OPTION_TABLE;

#define IPMI_BOOT_GUID  \
    {0x842680f2, 0x1a9c, 0x48e6, 0xa4, 0x33, 0xbe, 0x9a, 0xcb, 0xd, 0xd4, 0x38}

#define SETUP_GUID { 0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 }

#define EFI_BDS_EVENT_BEFORE_SETUP_GUID  \
  {0xdbc9fd21, 0xfad8, 0x45b0, 0x9e, 0x78, 0x27, 0x15, 0x88, 0x67, 0xcc, 0x93};

#define OEM_NVRAM_UPDATE_GUID \
  {0xaa264f75, 0x4b9e, 0x4421, 0xbb, 0x36, 0x96, 0x51, 0x8a, 0x46, 0xc4, 0x4e};

//Machong Notify OemIPMI Module
#define RTC_POWER_LOSS_PROTOCOL_GUID \
  {0x37531bb8, 0x35c3, 0x41f2, 0x8c, 0x5a, 0xb2, 0x91, 0x85, 0xa8, 0xf0, 0xf1}
//Machong Notify OemIPMI Module
#endif
