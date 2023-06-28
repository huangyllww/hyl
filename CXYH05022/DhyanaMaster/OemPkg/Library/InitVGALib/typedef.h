/*************************************************************************\
*  Module Name: typedef.h
*
*  Descriprtion:
*
*  Date:
\**************************************************************************/
/* Watcom C type define */
#include <PiPei.h>
#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>

#define	VOID	void
#define ULONG   UINT32
#define USHORT  UINT16
#define UCHAR   UINT8
#define LONG    INT32
#define	SHORT   INT16			
#define CHAR    CHAR8
#define BYTE	UINT8
#define BOOL	BOOLEAN

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE 
#define	FALSE	0
#endif
