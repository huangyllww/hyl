
#include "NvramLib.h"
#include "string.h"
#include "stdlib.h"
#include "Efi.h"
#include "Wrapper.h"
#include "Library/BaseMemoryLib.h"
#include "Library/BaseLib.h"

#define _EFI 1

//////////////////////// NVRAM.h /////////////////////////////////////////////////
#define FLASH_EMPTY_BYTE ((UINT8)FlashEmpty)
#define FLASH_EMPTY_FLAG FLASH_EMPTY_BYTE
#define FLASH_EMPTY_NEXT FlashEmptyNext
#define FLASH_EMPTY_SIGNATURE ((UINT32)FlashEmpty)
#define FLASH_EMPTY_SIZE ((VAR_SIZE_TYPE)FlashEmpty)
#define NVRAM_GUIDS_ADDRESS NvramGuidsAddress
#define NVRAM_END NvramEnd

#define NVRAM_FLAG_VALID 0x80
#define NVRAM_FLAG_RUNTIME 1
#define NVRAM_FLAG_ASCII_NAME 2
#define NVRAM_FLAG_GUID 4
#define NVRAM_FLAG_DATA_ONLY 8

#define ALL_FLAGS (NVRAM_FLAG_VALID | NVRAM_FLAG_RUNTIME |\
				   NVRAM_FLAG_ASCII_NAME | NVRAM_FLAG_GUID|\
				   NVRAM_FLAG_DATA_ONLY)

#define NVAR_SIGNATURE ('N'+('V'<<8)+(('A'+('R'<<8))<<16))//NVAR

extern UINT8 *pEndOfVars;
extern UINT8 *pFirstVar;

extern UINTN FlashEmpty;
extern UINT32 FlashEmptyNext;
extern UINT32 NvramGuidsOffset;
extern EFI_GUID* NvramGuidsAddress;
extern UINT8* NvramEnd;
extern UINTN NvramAddress;
extern UINTN NvramSize;

typedef UINT16 VAR_SIZE_TYPE;
#define NEXT_OFFSET (EFI_FIELD_OFFSET(NVAR,size)+sizeof(VAR_SIZE_TYPE))
#define NEXT_SIZE 3
#define FLAG_SIZE 1
#define FLAG_OFFSET (NEXT_OFFSET + NEXT_SIZE)
#define MAX_NVRAM_VARIABLE_SIZE ((1<<(sizeof(VAR_SIZE_TYPE)<<3))-1)
#pragma pack(push)
#pragma pack(1)
typedef struct{
//TODO: get rid of bit fields!!!
	UINT32 signature;
	VAR_SIZE_TYPE size;
	UINT32 next:24;
	UINT32 flags:8;
//  guid and name are there only if NVRAM_FLAG_DATA_ONLY is not set
//	UINT8 guid;
//	CHAR8* or CHAR16*: name
//  VOID* data
}NVAR;
#pragma pack(pop)

BOOLEAN NVisVariable(NVAR *pVar);

EFI_GUID* NVGetGuid(NVAR* pVar);
NVAR* NVGetDataNvar(NVAR *pVar);
VOID* NVGetData(NVAR* pVar, UINTN NameLength, UINTN* pDataSize);
BOOLEAN NVisValid(NVAR* pVar);
NVAR* NVDetextNext(NVAR* pVar);
NVAR* NVGetNext(NVAR* pVar);
VOID* NVGetName(NVAR* pVar);
BOOLEAN VarEq(NVAR* pNvar, CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize);
VOID* FindVariable(CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize);
EFI_STATUS NVGetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data, NVAR **pNvar
);
EFI_STATUS NVGetNextVariableName(
	IN OUT UINTN *VariableNameSize,
	IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
);
BOOLEAN IsNvramBlank();

//////////////////////// New Code /////////////////////////////////////////////////
#ifndef NULL
#define NULL 0
#endif
//#define guidcmp(g1,g2) MemCmp(g1,g2,sizeof(EFI_GUID))
#define EFI_FIELD_OFFSET(TYPE,Field) ((UINTN)(&(((TYPE *) 0)->Field)))
//#define memcpy memcpy
//#define MemCmp memcmp
#define Malloc malloc

VOID FlashAreaWriteEnable(VOID* pStartAddress, UINTN Size){}
VOID FlashAreaWriteDisable(VOID* pStartAddress, UINTN Size){}
BOOLEAN FlashAreaErase(VOID* pStartAddress, UINTN Size){
	memset(pStartAddress,(UINT8)FlashEmpty,Size);
	return TRUE;
}
BOOLEAN NvramFlashProgram(UINT8* pStart, UINTN Size, VOID* pDataPtr){
	memcpy(pStart,pDataPtr,Size);
	return TRUE;
}

//////////////////////// NVRAMRead.c /////////////////////////////////////////////////
BOOLEAN IsNvramBlank() {return ((NVAR*)pFirstVar)->signature==FlashEmpty;}

EFI_GUID* NVGetGuid(NVAR* pVar){
	if(pVar->flags&NVRAM_FLAG_GUID) return (EFI_GUID*)(pVar+1);
	else return NVRAM_GUIDS_ADDRESS-*(UINT8*)(pVar+1);
}

NVAR* NVGetDataNvar(NVAR *pVar)
{
	for(//TODO: check for VAR validity
		;pVar->next!=FLASH_EMPTY_NEXT
		;pVar = (NVAR*)((UINT8*)pVar + pVar->next)
	);
	return pVar;
}

VOID* NVGetData(NVAR* pVar, UINTN NameLength, UINTN* pDataSize){
	//TODO: What should we do here if pVar we just found is bad????
	if (pVar->next==FLASH_EMPTY_NEXT){
		UINTN DataOffset = sizeof(NVAR)+NameLength+(pVar->flags&NVRAM_FLAG_GUID ? sizeof(EFI_GUID) : 1);
		if (pDataSize) *pDataSize = pVar->size-DataOffset;
		return (INT8*)pVar+DataOffset;
	}
	else{
		pVar = NVGetDataNvar(pVar);
		if (pDataSize) *pDataSize = pVar->size-sizeof(NVAR);
		return pVar+1;
	}
}

BOOLEAN NVisValid(NVAR* pVar){
	return	((pVar->flags^~FLASH_EMPTY_FLAG)&NVRAM_FLAG_VALID) &&
			!(pVar->flags&NVRAM_FLAG_DATA_ONLY);
}

BOOLEAN NVisVariable(NVAR *pVar)
{
	return //TODO: Check pVar<pEndOfVars && pVar>=pFirstVar
		pVar->signature==NVAR_SIGNATURE
	&&	pVar->flags!=FLASH_EMPTY_FLAG
	&&	!(pVar->flags&~ALL_FLAGS)
	&&	pVar->size!=FLASH_EMPTY_SIZE && pVar->size>sizeof(NVAR)
	&&	(pVar->next==FLASH_EMPTY_NEXT||pVar->next<(UINTN)(pEndOfVars-pFirstVar))
	;
}


NVAR* NVDetextNext(NVAR* pVar)
{
	if (!pVar || pVar >= (NVAR*)pEndOfVars) return NULL;
	if (NVisVariable(pVar))
	{
		pVar = (NVAR*)((UINT8*)pVar+pVar->size);
		if (pVar >= (NVAR*)pEndOfVars) return NULL;
	}
	if (!NVisVariable(pVar))
	{
		//TODO: This is temporary!!! Change it!!!
		//To speed it up let's scan 0x100 bytes at max
		NVAR *pBorder = (NVAR*)((UINT8*)pVar+0x100);
		for(
			; pVar<(NVAR*)pEndOfVars && !NVisVariable(pVar)
			; pVar=(NVAR*)((UINT8*)pVar+1)
		) if (pVar>pBorder) return NULL;
		if (pVar >= (NVAR*)pEndOfVars) return NULL;
	}
	return pVar;
}


NVAR* NVGetNext(NVAR* pVar){
	do{
		pVar = NVDetextNext(pVar);
	}while(pVar && !NVisValid(pVar));
	return pVar;
}

VOID* NVGetName(NVAR* pVar){
	return (INT8*)(pVar+1)+(pVar->flags&NVRAM_FLAG_GUID ? sizeof(EFI_GUID) : 1);
}

BOOLEAN VarEq(NVAR* pNvar, CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize){
	UINT8 *pVarGuid, *pNameStart, *pN;
	pVarGuid=(UINT8*)NVGetGuid(pNvar);
	pN=(UINT8*)NVGetName(pNvar);
	pNameStart=pN;
	if (pNvar->flags&NVRAM_FLAG_ASCII_NAME){
		while(*pN && *pN==*sName){pN++; sName++;}
		if (*pN!=*sName) return FALSE;
		pN++;
	}
	else{
		CHAR16 *sN=(CHAR16*)pN;
		while(*sN && *sN==*sName){sN++; sName++;}
		if (*sN!=*sName) return FALSE;
		pN=(UINT8*)++sN;
	}
	if (CompareGuid((CONST GUID  *)pVarGuid,(CONST GUID  *)pGuid)){
		if (pNameSize) *pNameSize=pN-pNameStart;
		return TRUE;
	}
	return FALSE;
}

VOID* FindVariable(CHAR16* sName, EFI_GUID* pGuid, UINTN* pNameSize){
	NVAR* pNvar = (NVAR*)pFirstVar;
	//TODO: We assume that the first variable is always valid; Is it ok???
	if (!NVisVariable(pNvar)) return NULL;
	if (!NVisValid(pNvar)) pNvar = NVGetNext(pNvar);
	for(; pNvar; pNvar=NVGetNext(pNvar)){
		if (VarEq(pNvar,sName,pGuid,pNameSize)) return pNvar;
	}
	return NULL;
}

EFI_STATUS NVGetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data,
	IN NVAR **ppNvar OPTIONAL
)
{
	UINTN NameSize;
	NVAR* pNvar;
	UINT8* pN;
	UINTN Size;
	if (!VariableName || !VendorGuid || !DataSize || !Data && *DataSize) return EFI_INVALID_PARAMETER;

	pNvar = (NVAR*)FindVariable(VariableName,VendorGuid,&NameSize);
	if (!pNvar) return EFI_NOT_FOUND;
	if (Attributes){
		*Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_NON_VOLATILE;
		if (pNvar->flags&NVRAM_FLAG_RUNTIME) *Attributes|=EFI_VARIABLE_RUNTIME_ACCESS;
	}
	// Data
	pN = (UINT8*)NVGetData(pNvar,NameSize,&Size);
	if (*DataSize<Size){ *DataSize=Size; return EFI_BUFFER_TOO_SMALL;}
	*DataSize=Size;
	memcpy(Data,pN,Size);
	if (ppNvar) *ppNvar = pNvar;
	return EFI_SUCCESS;
}

EFI_STATUS NVGetNextVariableName(
	IN OUT UINTN *VariableNameSize,
	IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
//TODO: Review to see if it can be fixed
//Disable pLastVar optimization since we will be in trouble after SetVirtualAddressMap
//	static NVAR* pLastVar = NULL;
	UINTN NameSize;
	NVAR* pNvar;
	UINT8 *pN, *pNameStart;
	EFI_GUID* pVarGuid;
	if (!VariableNameSize || !VariableName || !VendorGuid) return EFI_INVALID_PARAMETER;
	if (VariableName[0]==0){
		pNvar = (NVAR*)pFirstVar;
		if(!NVisVariable(pNvar))return EFI_NOT_FOUND;
	}
	else{
//Disable pLastVar optimization. See comments above for details.
/*		if (pLastVar && VarEq(pLastVar,VariableName,VendorGuid,NULL)) pNvar=pLastVar;
		else*/{
			pNvar=(NVAR*)FindVariable(VariableName,VendorGuid,&NameSize);
			if (!pNvar) return EFI_NOT_FOUND;
		}
		pNvar=NVGetNext(pNvar);
		if (!pNvar) return EFI_NOT_FOUND;
	}
	pN = pNameStart = (UINT8*)NVGetName(pNvar);
	if (pNvar->flags&NVRAM_FLAG_ASCII_NAME){
		while(*pN++);
		NameSize = (pN - pNameStart)*2;
	}
	else{
		CHAR16* sN=(CHAR16*)pN;
		while(*sN++);
		pN=(UINT8*)sN;
		NameSize = pN - pNameStart;
	}
	if (NameSize>*VariableNameSize){
		*VariableNameSize=NameSize;
		return EFI_BUFFER_TOO_SMALL;
	}
	*VariableNameSize=NameSize;
	pVarGuid=NVGetGuid(pNvar);
	if (pNvar->flags&NVRAM_FLAG_ASCII_NAME) while((*VariableName++=*pNameStart++) != 0);
	else{
		CHAR16* sN = (CHAR16*)pNameStart;
		while((*VariableName++=*sN++) != 0);
	}
	memcpy(VendorGuid,pVarGuid,sizeof(EFI_GUID));
//Disable pLastVar optimization. See comments above for details.
//	pLastVar = pNvar;
	return EFI_SUCCESS;
}
//////////////////////// NVRAMDXE.c /////////////////////////////////////////////////
#define ALL_VARIABLE_ATTRIBUTES (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
INT16 NextGuid;
BOOLEAN Runtime = FALSE;


//This function has been slightly modified from the original BIOS version
BOOLEAN NVCompact(NVAR* pSkip, INTN Size){
	UINT8* pN = (UINT8*)pFirstVar;
	UINT8* pMStart = (UINT8*)Malloc(NvramSize), *pM=pMStart, *pMFirstVar = pM;
	UINTN GuidCount = 0;
	NVAR* pNvar = (NVAR*)pN;
	NVAR* pMvar = (NVAR*)pM;
	BOOLEAN ok=FALSE;
	EFI_GUID *pMguid = (EFI_GUID*)(pMStart+NvramGuidsOffset), *pMFirstGuid = pMguid;
	if (!NVisValid((NVAR*)pN)) return TRUE; //NVRAM area is empty
	for(;pN ;pN=(UINT8*)NVGetNext(pNvar)){
		UINTN NameSize,DataSize;
		UINT8 *pDest;
		EFI_GUID *pGuid;
		UINTN guid=0;
		pNvar = (NVAR*)pN;
		pMvar = (NVAR*)pM;
		if (pNvar==pSkip) continue;
		*pMvar = *pNvar; pMvar->next = FLASH_EMPTY_NEXT;
		for(pGuid=pMFirstGuid;pGuid>pMFirstGuid-GuidCount && !CompareGuid((CONST GUID  *)pGuid,(CONST GUID  *)NVGetGuid(pNvar));pGuid--) guid++;
		if (GuidCount>255 && guid>GuidCount){
			pMvar->flags |= NVRAM_FLAG_GUID;
			pM=(UINT8*)(pMvar+1);
			*(EFI_GUID*)pM = *NVGetGuid(pNvar);
			pM += sizeof(EFI_GUID);
		}
		else{
			pMvar->flags&=~NVRAM_FLAG_GUID;
			if (guid==GuidCount){
				GuidCount++;
				*pMguid-- = *NVGetGuid(pNvar);
			}
			pM = (UINT8*)(pMvar+1);
			*pM++ = (UINT8)guid;
		}
		pN=(UINT8*)NVGetName(pNvar);
		pDest = pM;
		if (pMvar->flags&NVRAM_FLAG_ASCII_NAME) while((*pM++=*pN++) != 0);
		else{
			CHAR16 *sM=(CHAR16*)pM, *sN=(CHAR16*)pN;
			while((*sM++=*sN++) != 0);
			pM = (UINT8*)sM; pN = (UINT8*)sN;
		}
		NameSize=pM-pDest;
		pN = (UINT8*)NVGetData(pNvar,NameSize,&DataSize);
		memcpy(pM,pN,DataSize);
		pM+=DataSize;
		pMvar->size=(VAR_SIZE_TYPE)(pM-(UINT8*)pMvar);
	}
	if ((UINT8*)pEndOfVars - (UINT8*)pFirstVar <= pM-pMFirstVar+Size ) return TRUE; //no need to compact
	//program flash part
	FlashAreaWriteEnable((VOID*)NvramAddress,NvramSize);
	if ((ok=FlashAreaErase((VOID*)NvramAddress,NvramSize)) != 0){
		pEndOfVars = (UINT8*)pFirstVar+(pM-pMFirstVar);
		NextGuid = (INT16)(pMFirstGuid - pMguid);
		//program GUIDs
		if ((ok=NvramFlashProgram((UINT8*)((EFI_GUID*)NVRAM_END-NextGuid),NextGuid*sizeof(EFI_GUID),(UINT8*)(pMguid+1))) != 0){
			//program Data
			ok = NvramFlashProgram((UINT8*)pFirstVar,(UINT8*)pEndOfVars-(UINT8*)pFirstVar,pMFirstVar);
		}
	}
	FlashAreaWriteDisable((VOID*)NvramAddress,NvramSize);
	free(pMStart);
	return ok;
}


VOID InitNV(){
	NVAR *pNvar = (NVAR*)pFirstVar;
	NVAR *pLastVar = pNvar;
	NextGuid=0;
	if (!NVisVariable(pNvar))
	{
		//TODO: We assume that the first variable is always valid; Is it ok???
		pEndOfVars = pFirstVar;
		return;
	}
	for(;pNvar;pNvar=NVDetextNext(pNvar)){
		if (NVisValid(pNvar) && !(pNvar->flags&NVRAM_FLAG_GUID)){
			INT16 guid = *(UINT8*)(pNvar+1);
			if (guid>NextGuid) NextGuid=guid;
		}
		pLastVar = pNvar;
	}
	NextGuid++;
	pEndOfVars = (UINT8*)pLastVar+pLastVar->size;
	if (pNvar>=(NVAR*)NVRAM_END){
		pEndOfVars = (UINT8*)NVRAM_END;
		NVCompact(NULL,0);
	}
}

EFI_STATUS NVSetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	IN UINT32 Attributes, IN UINTN DataSize,
	IN VOID *Data, NVAR **ppNvar OPTIONAL
)
{
	UINTN NameSize = 0,Size = 0;
	NVAR* pNvar;
	BOOLEAN ok=FALSE;
	NVAR Var; //= {FLASH_EMPTY_SIGNATURE,0,FLASH_EMPTY_NEXT,NVRAM_FLAG_VALID};
	UINT8* p;
	INT16 guid=0;

	//Var = {FLASH_EMPTY_SIGNATURE,0,FLASH_EMPTY_NEXT,NVRAM_FLAG_VALID};
	Var.signature = FLASH_EMPTY_SIGNATURE;
	Var.size = 0;
	Var.next = FLASH_EMPTY_NEXT;
	Var.flags = NVRAM_FLAG_VALID;

	//TODO: Failure recovery (NVCompact? skip?) old or new value (in case of filaure old value should stay)
	if (
		!VariableName || VariableName[0]==0 || !VendorGuid
	   ||( Attributes & ~ALL_VARIABLE_ATTRIBUTES)
	   ||(Attributes & EFI_VARIABLE_RUNTIME_ACCESS) && !(Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS )
	   || DataSize & !Data
	) return EFI_INVALID_PARAMETER;
	pNvar = (ppNvar && *ppNvar) ? *ppNvar : (NVAR*)FindVariable(VariableName,VendorGuid,&NameSize);
	if (!DataSize || !(Attributes & EFI_VARIABLE_BOOTSERVICE_ACCESS)){
		if (pNvar){ //delete
			UINT8 flag = (UINT8)pNvar->flags^NVRAM_FLAG_VALID;//invert validity bit
			FlashAreaWriteEnable((VOID*)NvramAddress,NvramSize);
			ok=NvramFlashProgram((UINT8*)pNvar+FLAG_OFFSET,FLAG_SIZE,&flag);
			FlashAreaWriteDisable((VOID*)NvramAddress,NvramSize);
			if (!ok) return EFI_DEVICE_ERROR;
			else
			{
				if (ppNvar) *ppNvar = NULL;
				return EFI_SUCCESS;
			}
		}
		else return EFI_NOT_FOUND;
	}
	Size=sizeof(NVAR);
	if (!pNvar){
		//create new
		CHAR16* s;
		EFI_GUID* pGuid;
		if (Attributes & EFI_VARIABLE_RUNTIME_ACCESS) Var.flags |= NVRAM_FLAG_RUNTIME;
		for(pGuid=NVRAM_GUIDS_ADDRESS;pGuid>NVRAM_GUIDS_ADDRESS-NextGuid && !CompareGuid((CONST GUID  *)pGuid,(CONST GUID  *)VendorGuid);pGuid--) guid++;
		if (NextGuid>255 && guid>NextGuid){ Var.flags |= NVRAM_FLAG_GUID; Size+=sizeof(EFI_GUID);}
		else Size+=1;
		//alalize name
		for(s=VariableName;*s&&!*((CHAR8*)s+1);s++) ;
		if (!*s){
			Var.flags |= NVRAM_FLAG_ASCII_NAME;
			Size+=s-VariableName+1+DataSize;
		}
		else{
			while(*s++);
			Size+=(UINT8*)s-(UINT8*)VariableName+DataSize;
		}
	}else{
        UINT8 *OldData;
        UINTN OldDataSize;
        if (!(pNvar->flags & NVRAM_FLAG_RUNTIME)!=!(Attributes & EFI_VARIABLE_RUNTIME_ACCESS)) return EFI_INVALID_PARAMETER;
        //if we used ppNvar, NameSize is not initialized. Let's call VarEq to calculate the NameSize
        if (ppNvar && *ppNvar) VarEq(pNvar,VariableName,VendorGuid,&NameSize);
        OldData = NVGetData(pNvar,NameSize,&OldDataSize);
        if (OldDataSize==DataSize){
            memcpy(OldData,Data,DataSize);
            if (ppNvar) *ppNvar = pNvar;
            return EFI_SUCCESS;
        }
        //update existing
		Var.flags |= NVRAM_FLAG_DATA_ONLY;
		Size+=DataSize;
	}
	if (Size>MAX_NVRAM_VARIABLE_SIZE) return EFI_OUT_OF_RESOURCES;
	if ((UINT8*)pEndOfVars + Size > NVRAM_END - sizeof(EFI_GUID)*(NextGuid+1) - sizeof(NVAR)){
		// No Flash erase operations in runtime
		if (Runtime) return EFI_OUT_OF_RESOURCES;
		if (pNvar){
			EFI_GUID* pGuid;
			Var.flags = pNvar->flags;
			//if we used ppNvar, NameSize is not initialized. Let's call VarEq to calculate the NameSize
			if (ppNvar && *ppNvar) VarEq(pNvar,VariableName,VendorGuid,&NameSize);
			Size+=NameSize;
			if (!NVCompact(pNvar,Size)) return EFI_DEVICE_ERROR;
			pNvar = NULL;
			for(pGuid=NVRAM_GUIDS_ADDRESS;pGuid>NVRAM_GUIDS_ADDRESS-NextGuid && !CompareGuid((CONST GUID  *)pGuid,(CONST GUID  *)VendorGuid);pGuid--) guid++;
			if (NextGuid>255 && guid>NextGuid){ Var.flags |= NVRAM_FLAG_GUID; Size+=sizeof(EFI_GUID);}
			else Size+=1;
		}
		else if (!NVCompact(NULL,0)) return EFI_DEVICE_ERROR;
		if ((UINT8*)pEndOfVars + Size > NVRAM_END - sizeof(EFI_GUID)*(NextGuid+1) - sizeof(NVAR)) return EFI_OUT_OF_RESOURCES;
	}
	Var.size=(VAR_SIZE_TYPE)Size;
	FlashAreaWriteEnable((VOID*)NvramAddress,NvramSize);
	p = (UINT8*)pEndOfVars;
	p+=sizeof(NVAR);
	//Program Var at pEndOfVars
	if ((ok=NvramFlashProgram(pEndOfVars,sizeof(NVAR),&Var)) != 0){
		if (!pNvar){
			//Program Guid
			if (Var.flags&NVRAM_FLAG_GUID){
				//program the whole guid
				ok=NvramFlashProgram(p,sizeof(EFI_GUID),VendorGuid);
				p+=sizeof(EFI_GUID);
			}else{
				//program the whole guid at NVRAM_GUIDS_ADDRESS - NextGuid
				if (guid==NextGuid && ((ok=NvramFlashProgram((UINT8*)(NVRAM_GUIDS_ADDRESS-NextGuid),sizeof(EFI_GUID),VendorGuid)) != 0))
						NextGuid++;
				//program guid(1 byte) at pEndOfVars+sizeof(NVAR);
				ok=NvramFlashProgram(p++,1,&guid);
			}
			if (ok){
				//Program name and data
				if (Var.flags&NVRAM_FLAG_ASCII_NAME){
					UINT8* q;
					for(q=(UINT8*)VariableName;*(CHAR16*)q && ok ;q+=2,p++)
						ok = NvramFlashProgram(p,1,q);
					if (ok) ok = NvramFlashProgram(p++,1,q); //zero at end
				}
				else{
					//ok = NvramFlashProgram(p,NameSize,VariableName);
					UINT16* q;
					for(q=VariableName;*q && ok ;q++,p+=2)
						ok = NvramFlashProgram(p,2,q);
					if (ok){
						ok = NvramFlashProgram(p,2,q); //zero at end
						p+=2;
					}
				}
			}
			pNvar = (NVAR*)pEndOfVars;
		}
		if (ok){
			//Program data
			ok=NvramFlashProgram(p,DataSize,Data);
			p+=DataSize;
		}
		if (ok)//write a signature
		{
			UINT32 Sig = NVAR_SIGNATURE;
			ok=NvramFlashProgram(pEndOfVars,sizeof(Sig),&Sig);
			if((Var.flags&NVRAM_FLAG_DATA_ONLY) && ok){
				//if this is update operation, set pNext;
				NVAR *pN=NVGetDataNvar(pNvar);
				Var.next=(UINT32)((UINT8*)pEndOfVars-(UINT8*)pN);
				ok=NvramFlashProgram((UINT8*)pN+NEXT_OFFSET,NEXT_SIZE,(UINT8*)&Var+NEXT_OFFSET);
			}
		}
	}
	FlashAreaWriteDisable((VOID*)NvramAddress,NvramSize);
	pEndOfVars=p;
	if (ok)
	{
		if (ppNvar) *ppNvar = pNvar;
		return EFI_SUCCESS;
	}
 	else
	{
		return EFI_DEVICE_ERROR;
	}
}
//////////////////////// Token.c /////////////////////////////////////////////////
// Constants
//UINTN FlashEmpty;
//UINT32 FlashEmptyNext;
// Variables
UINTN NvramSize;
UINT32 NvramGuidsOffset;
EFI_GUID* NvramGuidsAddress;
UINT8* NvramEnd;
UINTN gNvramAddress;
UINT8 *pEndOfVars;
UINT8 *pFirstVar;
//////////////////////// New code /////////////////////////////////////////////////
VOID NvramLibInit(char* Buffer, UINTN Size, UINT8 FlashErasePolarity){
	UINTN Address = (UINTN)Buffer;
	//FlashEmpty = (UINTN)(-FlashErasePolarity);
	//FlashEmptyNext = (FlashErasePolarity ? 0xffffff : 0);
	//NvramSize = Size;
	NvramGuidsOffset = (UINT32)(Size-sizeof(EFI_GUID));
	NvramGuidsAddress = (EFI_GUID*)(Address + NvramGuidsOffset);
	NvramEnd = (UINT8*)(Address + Size);
	gNvramAddress = Address;
	pEndOfVars = NvramEnd;
	pFirstVar = (UINT8*)gNvramAddress;
	InitNV();
}

EFI_STATUS SceGetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	OUT UINT32 *Attributes OPTIONAL,
	IN OUT UINTN *DataSize, OUT VOID *Data
)
{
	return NVGetVariable(
		VariableName, VendorGuid, Attributes, DataSize, Data, NULL
	);
}

EFI_STATUS GetNextVariableName(
	IN OUT UINTN *VariableNameSize,
	IN OUT CHAR16 *VariableName, IN OUT EFI_GUID *VendorGuid
)
{
	return NVGetNextVariableName(VariableNameSize,VariableName,VendorGuid);
}

EFI_STATUS SetVariable(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data
)
{
	return NVSetVariable(
		VariableName, VendorGuid, Attributes, DataSize, Data, NULL
	);
}

//Additional output parameter NvramDataPtr
//On exit it will contain adrress within NVRAM buffer at which
//variable data has been placed.
EFI_STATUS SetVariableEx(
	IN CHAR16 *VariableName, IN EFI_GUID *VendorGuid,
	IN UINT32 Attributes, IN UINTN DataSize, IN VOID *Data, OUT VOID **NvramDataPtr
)
{
	NVAR *Nvar = NULL;
	EFI_STATUS Status;

	Status = NVSetVariable(
		VariableName, VendorGuid, Attributes, DataSize, Data, &Nvar
	);
	if (!EFI_ERROR(Status) && NvramDataPtr){
		UINTN NameSize;
		//This function compares NVAR record with Name/GUID pair
		//We don't need this
		//We are calling this routine just to get NameSize
		VarEq(Nvar,VariableName,VendorGuid,&NameSize);
		*NvramDataPtr = NVGetData(Nvar,NameSize,NULL);
	}
	return Status;
}

VOID NvramCompact(){ NVCompact(NULL,0); } 

BOOLEAN IsPolarityCorrect() {return ((NVAR*)pEndOfVars)->signature == FlashEmpty;}

