/*************************************************************************\
*  Module Name: iotool.c
*
*  Descriprtion:
*
*  Date:
\**************************************************************************/

#include "typedef.h"
#include "astiolib.h"

VOID SetReg (USHORT usPORT, UCHAR jMask, UCHAR jData)
{
    UCHAR jtemp=jData;  
  
    jtemp = (UCHAR) IoRead8 ((SHORT)usPORT);    
    IoWrite8 ((SHORT)usPORT, jtemp & jMask | jData);  	
  
}

VOID SetReg2 (USHORT usPORT, UCHAR jData)
{
  
	IoWrite8 ((SHORT)usPORT, jData);  	
  
}

UCHAR GetReg (USHORT usPORT, UCHAR jMask)
{
	USHORT tmpPORT, tmpValue;

	
	tmpPORT = usPORT & 0xFFFE;
	tmpValue = IoRead16 (tmpPORT);
	
	if(usPORT & 0x1)
		return ((UCHAR)(tmpValue>>8) & jMask);
	else
		return ((UCHAR)tmpValue & jMask);  	
}

UCHAR GetReg2 (USHORT usPORT)
{
     
	USHORT tmpPORT, tmpValue;

	
	tmpPORT = usPORT & 0xFFFE;
	tmpValue = IoRead16 (tmpPORT);
	
	if(usPORT & 0x1)
		return ((UCHAR)(tmpValue>>8));
	else
		return ((UCHAR)tmpValue);  		
}

VOID SetIndexReg (USHORT usPORT, UCHAR jIndex, UCHAR jMask, UCHAR jData)
{
    UCHAR jtemp=jData;  
  
//    IoWrite16 ((SHORT)usPORT++, (SHORT)jIndex);
//    jtemp = (UCHAR) IoRead16 ((SHORT)usPORT);    
//    IoWrite16 ((SHORT)usPORT, ((SHORT)jtemp & (SHORT)jMask) | (SHORT)jData);  	

	  IoWrite8 ((SHORT)usPORT++, jIndex);
	  jtemp = IoRead8 ((SHORT)usPORT); 
	  IoWrite8 ((SHORT)usPORT, (jtemp & jMask) | jData);
  
}

VOID SetIndexReg2 (USHORT usPORT, UCHAR jIndex, UCHAR jData)
{
  
    IoWrite8 ((SHORT)usPORT++, jIndex);
    IoWrite8 ((SHORT)usPORT, jData);  	
  
}



UCHAR GetIndexReg (USHORT usPORT, UCHAR jIndex, UCHAR jMask)
{
     
//    IoWrite16 ((SHORT)usPORT++, (SHORT)jIndex);
        
//    return ( ((UCHAR)inp ((SHORT)usPORT)) & jMask );  
	  IoWrite8(usPORT++, jIndex);
	  return (IoRead8(usPORT) & jMask);		
  
}

UCHAR GetIndexReg2 (USHORT usPORT, UCHAR jIndex)
{
     
//    IoWrite16 ((SHORT)usPORT++, (SHORT)jIndex);
        
//    return ( ((UCHAR)inp ((SHORT)usPORT)) );
	  IoWrite8(usPORT++, jIndex);
	  return (IoRead8(usPORT));	
  
}

VOID Delay15us (VOID)
{
	
    USHORT value;
	
    value = GetReg (0x61, 0xFF);

    if (value & 0x10)
        while (GetReg (0x61, 0xFF) & 0x10) ;
    else
        while (!(GetReg (0x61, 0xFF) & 0x10)) ;
    
}
