#ifndef __WRITE_CACHE_H__
#define __WRITE_CACHE_H__


#include <IndustryStandard/AmiAtaAtapi.h>
#include <Base.h>


#define     VWC_ENABLE            0x02
#define     VWC_DISABLE           0x82
#define ZeroMemory(Buffer,Size) pBS->SetMem(Buffer,Size,0)


#endif
