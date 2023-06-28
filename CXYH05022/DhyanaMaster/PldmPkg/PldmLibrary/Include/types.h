/*
types.h	- commonly used type definitions
*/

#ifndef __TYPES_H__
#define __TYPES_H__


typedef unsigned char			UINT8;

typedef short					INT16;
typedef unsigned short			UINT16;
typedef UINT16 CHAR16;
//typedef unsigned char CHAR8;
typedef int BOOL;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned short USHORT;
typedef unsigned char BYTE;
typedef BOOLEAN bool;

#ifdef __cplusplus
//extern "C" {
#endif
int getch();
#ifdef __cplusplus
//}
#endif

#define far

//#define TEXT_LINE_TOP    " 浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様?"
//#define TEXT_LINE_MID    " 麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様?"
//#define TEXT_LINE_END    " 藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様?"

//#define TEXT_LINE_SEP    " ?------------------------------------------------------------------------- ?"
//#define TEXT_V           " ?"

#define TEXT_LINE_TOP    " +---------------------------------------------------------------------------+ "
#define TEXT_LINE_MID    TEXT_LINE_TOP
#define TEXT_LINE_END    TEXT_LINE_TOP
#define TEXT_LINE_SEP    " | ------------------------------------------------------------------------- | "
#define TEXT_V           " | "

#define huge
#define __attribute__(x)

#endif 						// __TYPES__
