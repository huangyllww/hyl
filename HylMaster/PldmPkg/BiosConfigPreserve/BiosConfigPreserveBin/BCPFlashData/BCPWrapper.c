//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2016, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093  			**
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************

/** @file 
EsrtConfigPreserve.c

Responsible for preserving setup configuration in ESRT flash update

*/

//************************************************************************
//					Include Section
//************************************************************************
#include "Token.h"


/**
Returns ENABLE_FV_BCP_SUPPORT token value

@param void

@retval BOOLEAN

**/

BOOLEAN FVDataAccessType ()
{
#ifdef ENABLE_FV_BCP_SUPPORT
    return ENABLE_FV_BCP_SUPPORT;
#endif 
    
    return 0;
}

/**
Returns FLASHDATA_SIZE_BCP token value

@param void

@retval BOOLEAN

**/

UINTN FVFlashDataSize ()
{
#ifdef FLASHDATA_SIZE_BCP
    return FLASHDATA_SIZE_BCP;
#endif 
    
    return 0;
}

/**
Returns FLASH_BLOCK_SIZE token value

@param void

@retval BOOLEAN

**/

UINTN FVFlashBlockSize ()
{
#ifdef FLASH_BLOCK_SIZE
    return FLASH_BLOCK_SIZE;
#endif 
    
    return 0;
}

/**
Returns FV_BCP_OFFSET token value

@param void

@retval BOOLEAN

**/

UINTN FVBCPVolumeOffset ()
{
#ifdef FV_BCP_OFFSET
    return FV_BCP_OFFSET;
#endif 
    
    return 0;
}

/**
Returns FV_BCP_SIZE token value

@param void

@retval BOOLEAN

**/

UINTN FVBCPVolumeSize ()
{
#ifdef FV_BCP_SIZE
    return FV_BCP_SIZE;
#endif 
    
    return 0;
}

//************************************************************************
//************************************************************************
//**                                                                  	**
//**        (C)Copyright 2016, American Megatrends, Inc.            	**
//**                                                                  	**
//**                       All Rights Reserved.                       	**
//**                                                                  	**
//**  5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093              **
//**                                                                  	**
//**                       Phone: (770)-246-8600                      	**
//**                                                                  	**
//************************************************************************
//************************************************************************
