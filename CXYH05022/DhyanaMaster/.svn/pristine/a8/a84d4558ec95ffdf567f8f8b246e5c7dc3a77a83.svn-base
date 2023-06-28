;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************
;*************************************************************************

;*************************************************************************
; $Header: $
;
; $Revision:  $
;
; $Date:  $
;*************************************************************************
;<AMI_FHDR_START>
;
; Name: SmmReloc.asm
;
; Description:  Code for relocating SMM Base
;
;<AMI_FHDR_END>
;*************************************************************************


SMM_BASE_SAVE_OFFS  EQU 0FF00h

EXTERNDEF      gSmmRelocSize:WORD
EXTERNDEF      gNewSmmBaseOffs:WORD
EXTERNDEF      gRelocDoneOffs:WORD
EXTERNDEF      SmmRelocStart:BYTE

.code
SmmRelocStart   LABEL   BYTE

;<AMI_PHDR_START>
;----------------------------------------------------------------------------
; Procedure:	SmmRelocEntry
;
; Description:	Code executed by each Core to relocate SMM base.
;
; Input:
;   None
;
; Output:
;   None
;
; Modified:
;
; Referrals:
;
; Notes:
;   NewSmmBase is updated by init procedure before launch individual Core.
;
;----------------------------------------------------------------------------
;<AMI_PHDR_END>

SmmRelocEntry PROC
                                        ; cs = 0x3000
    DB 66h, 0b8h                        ; mov eax,  gNewSmmBase
NewSmmBase    DD   ?
    DB 0bbh                             ;  mov bx, SMM_BASE_SAVE_OFFS
    DW  SMM_BASE_SAVE_OFFS
    DB 66h, 2eh, 89h, 07h               ;  mov cs:[bx], eax
    DB 0b0h, 01                         ;  mov al, 1
    DB 2eh, 0a2h                        ;  mov cs:[RelocDone], al     ;set flag
    DW 8000h + RelocDone - SmmRelocEntry
    
    rsm
    jmp $
SmmRelocEntry ENDP

RelocDone  DD  0

gSmmRelocSize   DW  $ - SmmRelocEntry
gNewSmmBaseOffs DW  NewSmmBase - SmmRelocEntry
gRelocDoneOffs  DW  RelocDone - SmmRelocEntry
END
;*************************************************************************
;*************************************************************************
;**                                                                     **
;**        (C)Copyright 1985-2019, American Megatrends, Inc.            **
;**                                                                     **
;**                       All Rights Reserved.                          **
;**                                                                     **
;**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093           **
;**                                                                     **
;**                       Phone: (770)-246-8600                         **
;**                                                                     **
;*************************************************************************