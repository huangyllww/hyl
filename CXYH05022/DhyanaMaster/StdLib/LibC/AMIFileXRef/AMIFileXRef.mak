#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**              5555 Oak brook Pkwy, Norcorss, GA 30093             **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************

#**********************************************************************
# $Header: /Projects/AMIRescue/Aptio V/eModules/StdLib/LibC/AMIFileXRef/AMIFileXRef.mak $
#

Prepare : LoadAmiStdLibFileXRef


LoadAmiStdLibFileXRef :  $(BUILD_DIR)/Token.mak
	$(ECHO) \
"#ifndef __STDLIB_FILEXREF_H__$(EOL)\
#define __STDLIB_FILEXREF_H__$(EOL)\
#define STRINGAIZE(A) #A$(EOL)\
#define END_OF_LINK {0,{0}}$(EOL)\
#define MAX_FILE_LENGTH 128$(EOL)\
struct _filexref$(EOL)\
{$(EOL)\
	char Filename[MAX_FILE_LENGTH];$(EOL)\
	EFI_GUID ResourceGUID;$(EOL)\
};$(EOL)\
#define STDLIB_FILEXREF {$(AMI_STDLIB_FILEXREF_ELINK)END_OF_LINK}$(EOL)\
#endif$(EOL) "\
>$(BUILD_DIR)/AmiStdLibFileXRef.h	


#**********************************************************************
#**********************************************************************
#**                                                                  **
#**        (C)Copyright 1985-2011, American Megatrends, Inc.         **
#**                                                                  **
#**                       All Rights Reserved.                       **
#**                                                                  **
#**             5555 Oakbrook Pkwy, Norcross, Georgia 30093          **
#**                                                                  **
#**                       Phone: (770)-246-8600                      **
#**                                                                  **
#**********************************************************************
#**********************************************************************
