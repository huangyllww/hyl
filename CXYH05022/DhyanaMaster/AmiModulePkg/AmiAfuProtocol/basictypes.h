//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2014, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
/** @file 
basictypes.h

This file collects basic data types.

*/

#ifndef	_BASIC_TYPES_H_
#define	_BASIC_TYPES_H_

#include <Token.h>
//#include <nativetypes.h>	// Define platform native types

#ifndef NULL
#define	NULL	0
#endif

// D2S MARCO
/**
 *	The macro expands the parameter to literal string.
 */
#define     D2SL2(x)        #x

/**
 *	The macro expands the parameter to literal string.
 */
#define     COMMAND(x)      D2SL2(x)


typedef	char			CHAR;
typedef	BOOLEAN			BOOL;

// Unsigned types
typedef	unsigned char		BYTE;
typedef	unsigned short		WORD;
typedef	unsigned long		DWORD;
typedef unsigned __int64	QWORD;

// Pointer types
typedef void *			PVOID;

typedef char *			PCHAR;
typedef short *			PSHORT;
typedef int *			PINT;

typedef unsigned char *		PBYTE;
typedef unsigned short *	PWORD;
typedef unsigned int *		PDWORD;
typedef UINTN			PHY_ADDR_T;


// Bitmap Macro
#ifndef BIT00
#define BIT00	0x00000001
#endif
#ifndef BIT01
#define BIT01	0x00000002
#endif
#ifndef BIT02
#define BIT02	0x00000004
#endif
#ifndef BIT03
#define BIT03	0x00000008
#endif
#ifndef BIT04
#define BIT04	0x00000010
#endif
#ifndef BIT05
#define BIT05	0x00000020
#endif
#ifndef BIT06
#define BIT06	0x00000040
#endif
#ifndef BIT07
#define BIT07	0x00000080
#endif
#ifndef BIT08
#define BIT08	0x00000100
#endif
#ifndef BIT09
#define BIT09	0x00000200
#endif
#ifndef BIT10
#define BIT10	0x00000400
#endif
#ifndef BIT11
#define BIT11	0x00000800
#endif
#ifndef BIT12
#define BIT12	0x00001000
#endif
#ifndef BIT13
#define BIT13	0x00002000
#endif
#ifndef BIT14
#define BIT14	0x00004000
#endif
#ifndef BIT15
#define BIT15	0x00008000
#endif
#ifndef BIT16
#define BIT16	0x00010000
#endif
#ifndef BIT17
#define BIT17	0x00020000
#endif
#ifndef BIT18
#define BIT18	0x00040000
#endif
#ifndef BIT19
#define BIT19	0x00080000
#endif
#ifndef BIT20
#define BIT20	0x00100000
#endif
#ifndef BIT21
#define BIT21	0x00200000
#endif
#ifndef BIT22
#define BIT22	0x00400000
#endif
#ifndef BIT23
#define BIT23	0x00800000
#endif
#ifndef BIT24
#define BIT24	0x01000000
#endif
#ifndef BIT25
#define BIT25	0x02000000
#endif
#ifndef BIT26
#define BIT26	0x04000000
#endif
#ifndef BIT27
#define BIT27	0x08000000
#endif
#ifndef BIT28
#define BIT28	0x10000000
#endif
#ifndef BIT29
#define BIT29	0x20000000
#endif
#ifndef BIT30
#define BIT30	0x40000000
#endif
#ifndef BIT31
#define BIT31	0x80000000
#endif

#define IN
#define OUT

#endif	// _BASIC_TYPES_H_
