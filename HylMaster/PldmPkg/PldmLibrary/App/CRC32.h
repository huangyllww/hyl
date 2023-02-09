#ifndef _CRC32_H_
#define _CRC32_H_

#ifdef _WIN32
//#include "stdafx.h"
#endif
#include "types.h"


typedef unsigned long ULONG;
typedef unsigned char *LPBYTE;
typedef unsigned int UINT;

void CalculateCRC_32(const LPBYTE Buffer, UINT Size, UINT32 *CRC);

#endif /* #ifndef _CRC32_H_ */
