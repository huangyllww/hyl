//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
// $Header: /AptioV/SRC/AMIDebugRxPkg/Include/Library/ser_xp.h 7     1/05/16 7:38p Sudhirv $
//
// $Revision: 7 $
//
// $Date: 1/05/16 7:38p $
//*****************************************************************
//*****************************************************************
//
//	ser_xp.h
//
//
// Revision History
// ----------------
// $Log: /AptioV/SRC/AMIDebugRxPkg/Include/Library/ser_xp.h $
// 
// 7     1/05/16 7:38p Sudhirv
// [TAG]			EIP249624
// [Category]   	Bug Fix
// [Severity]		Normal
// [Symptom]  	Setting Breakpoint in SMM Code from DXE phase when SMM
// Entry\Exit is ON causes the Host-Target to lose sync
// [RootCause]	Setting Breakpoint in SMM Code from DXE phase when SMM
// Entry\Exit is ON causes the Host-Target to lose sync
// [Solution]		handle the memory access case
// [Files]			AMIDebugRxPkg.sdl
// AMIDebugRxPkg.dec
// AMIDebugRxPkg.cif
// AMIpeidebug.h
// AMIpeidebugX64.h
// ser_acc.h
// ser_xp.h
// 
// 6     2/27/15 12:59a Jekekumarg
// EIP-205972-[AMIDebugger] Ramjet- Halt at SMM entry hangs at s3nvramsave
// module
// 
// 5     2/11/15 12:56p Jekekumarg
// Fixed the Issue EIP-203977-SMM Entry / Exit not Booting to Shell. It
// Hangs in Post Screen
// 
// 4     12/22/14 1:03a Sudhirv
// [TAG]	EIP196769
// [Category]	Bug Fix
// [Symptom:]	Local variables can display in DXE but PEI phase still fail,
// global variable still cannot display on both phases
// [Files]	AMIpeidebug.h
// AMIpeidebugX64.h
// ser_xp.h
// IncludeLibrary.cif
// 
// 3     6/24/14 6:05p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0010 Label
// 
// 2     2/20/14 5:01p Sudhirv
// Updated for AMI Debug for UEFI 3.01.0008 INT Label
// 
// 1     11/02/12 10:07a Sudhirv
// AMIDebugRxPkg modulepart for AMI Debug for UEFI 3.0
// 
// 3     8/08/12 3:58p Sudhirv
// Updated after Binaries prepared.
// 
// 2     8/03/12 6:32p Sudhirv
// Updated before making binaries.
//
// 1     7/15/12 8:12p Sudhirv
// AMIDebugRx Module for Aptio 5
//
//*****************************************************************

#include "token.h"
#ifndef EFIx64
#include <Library\AMIPeiDebug.h>
#else
#include <Library\AMIPeiDebugX64.h>
#endif

//; Serial Port settings
#define BAUD_RATE		57600

//; Sync flags
#define START_FLAG	0x07e
#define STOP_FLAG	0x07f
#define ESC_CODE	0x07d

//; Timings
#define HANDSHAKE_MAX_RETRIES	10
#if SERIAL_DEBUGGER_SUPPORT
#define DATA_TO_ACK_SMM_DELAY	50000
#define DATA_TO_ACK_DELAY		50000
#else
#define DATA_TO_ACK_SMM_DELAY	1000
#define DATA_TO_ACK_DELAY		60000
#endif
#define DATA_TO_HSACK_DELAY		500
#define SEND_DATA_MAX_RETRIES	10
#define SEND_DATA_MIN_RETRIES	0
//#define MAX_PKT_SIZE			1 * 582
#define MAX_PKT_SIZE			1 * GLOBALBUF_SIZE
#define MAX_EXPTN_PKT_SIZE		32

//; States
#define STATE_IDLE			    0x1
#define STATE_SENDING			0x2
#define STATE_RCVING			0x3
#define STATE_RCVING_ACK		0x4
#define STATE_DATA_READY		0x5
#define STATE_PKT_SENT			0x6

//; Commands
#define CMD_ACK				0x81
#define CMD_NACK			0x82
#define CMD_BUSY			0x83
#define CMD_DATA			0x4
#define CMD_HANDSHAKE		0x85
#define CMD_HANDSHAKE_ACK	0x86
#define CMD_EXCPTN			0xfe
#define CMD_ALIVE 			0xfd


#define WAIT_FOREVER		0x0ffff

#define SUCCESS				0
#define FAILED				1
#define TIMEOUT				2

typedef struct {
	UINT8 PktId;
	UINT8 PktNum;
	UINT8 Cmd;
	UINT8 SubCmd;
}SerXPortPktHdr_T;


typedef struct {
	UINT8 PktId;
	UINT8 PktNum;
	UINT8 Cmd;
	UINT8 SubCmd;
	UINT16 Size;
	UINT8 MiscData[256];
}SerXPortPktHdrConsole_T;


typedef struct {
	UINT16	m_OffHigh;
	UINT16	m_Reserved;
	UINT16	m_SegSel;
	UINT16	m_OffLow;
}IDTTableEntry_T;

void	SerXPortInitGblData (VOID *);
void	SerInitGblVar (VOID *);
void	XportCopyIntoRAM(VOID*, VOID*, UINTN Size);
UINTN 	InitXLayer( UINT32 IDTAddr);
void	PatchIDT(UINT16, IDTTableEntry_T *);
void	InitSerPort (UINT16, UINT32);
void	EnableSerIRQ (UINT16);
void	ClearAllSerInt (UINT16);
UINT8	GetIRQ (UINT16);
void	SerPortIrqHndlr ();
void	DummySerPortIrqHndlr ();
void	SendEOI (void);
VOID	SerPortHndlr ();
UINTN	SendData(VOID*,UINT8*,UINT16);
void	CopyBufIntoPktBuf(VOID*, VOID*, UINTN);
UINTN	SendDataNoAck(UINT8*,UINT16);
UINTN	RcvData(VOID*,UINT8*,UINT16*,UINT16);
UINT16	AMISendPkt(VOID*,UINT16);
UINTN	RcvPkt(VOID*,UINT8, UINT16);
//Debug - To Boot without host Sudhir
UINTN	RcvPktHS(UINT8, UINT16);
VOID	SendAck(VOID*,UINT8);
UINT16	PktSync(VOID *);
UINTN	SendBuf(VOID *, UINT8*, UINT16);
VOID	FillChecksum(UINT8*, UINT16);
UINTN	VerifyChecksum(UINT8*,UINT16);
VOID	SendAliveSignal(VOID*pData);
//Using new function for now will update the old one to this
UINTN	SendException(UINT8,UINT8*,UINT16);
VOID	ExptnHndlr(VOID*);
UINT32	IsXportInListenMode();
UINT16 	InitPort();
UINT16	RestorePort();
void 	SetupInterruptHandler(IDTTableEntry_T *IDTAddr);
BOOLEAN GetFlowCtrl(void);
UINT8 WaitForDataInIp(UINT16, UINT8 *);
void EnableBreakpointOnIO();
void DisableBreakpointOnIO();
VOID Stall (UINTN Usec);

VOID *
EFIAPI
CopyMem (
  OUT VOID       *DestinationBuffer,
  IN CONST VOID  *SourceBuffer,
  IN UINTN       Length
  );

void	AddDelay();

UINT64 DbgGetCpuTimer();
#ifdef EFIx64
UINT32 	GetCPUInfo0(VOID);
UINT32 	GetCPUInfo4(VOID);
UINT32 	GetCPUInfo8(VOID);
UINT32 	GetVersion(VOID);
UINT32 	GetFeature(VOID);
UINT64	DbgGetCpuTmr();
#endif

UINTN GetSMMDbgDataBaseAddress();

BOOLEAN
EFIAPI
SaveAndSetDebugTimerInterrupt (
  IN BOOLEAN                EnableStatus
  );

UINT32 EFIAPI GetApicTimerInitCount(VOID);
VOID InitializeDebugTimer(VOID);

VOID DBGWriteChkPort(UINT8);
void SaveTmrHndlr (VOID *);

#ifdef EFIx64
void DebugWriteChkPort(UINT8 DbgChkPoint);
#endif

#ifndef EFIx64
void 	InitPortData(SerXPortGblData_T *SerData);
#else
void 	InitPortData(SerXPortGblData_Tx64 *SerData);
#endif

UINT32 GetTimeOutFactor(UINT16 TimeoutUs);
UINT32 IsDbgrInCAR();
UINTN SendHandShakeToHost(VOID*);
UINTN SendTargetInfoToHost(VOID*);
VOID GetBuildTime(void*);
UINTN GetDbgDataBaseAddress();
UINTN GetSMMDbgDataBaseAddress();


#if USB_DEBUG_TRANSPORT
VOID	PatchTimerIDT(UINT8 IntNum, IDTTableEntry_T *IDT);
int		InitUSBPort();
void 	RestoreUSBPort(VOID *Base);
#endif

#ifndef EFIx64
extern	AMI_PEI_TRANSPORT_PROTOCOL AMITransportProtocol;
#else
extern	AMI_PEI_TRANSPORT_PROTOCOLx64 AMITransportProtocol;
#endif

//xApic Timer related Function Definitions
VOID 
EFIAPI 
SendApicEoi(
	VOID
	);

// SerialPortLib Function Definitions
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  );

UINT8
SerialPortReadRegister (
  UINTN  Offset
  );

UINT8
SerialPortWriteRegister (
  UINTN  Offset,
  UINT8  Value
  );
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
//**                                                             **//
//**         (C)Copyright 2014, American Megatrends, Inc.        **//
//**                                                             **//
//**                     All Rights Reserved.                    **//
//**                                                             **//
//**   5555 Oakbrook Pkwy, Building 200,Norcross, Georgia 30093  **//
//**                                                             **//
//**                     Phone (770)-246-8600                    **//
//**                                                             **//
//*****************************************************************//
//*****************************************************************//
//*****************************************************************//
