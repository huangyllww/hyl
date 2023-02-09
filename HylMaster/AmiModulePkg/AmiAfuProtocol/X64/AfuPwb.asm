	.code

;---------------------------------------------------
; VOID PortWriteByte (
;	IN	WORD		Port,
;	IN	BYTE		Value,
;	IN	pUC2RegBuffer	RegBuffer
; );
;---------------------------------------------------
PortWriteByte	PROC
	;-- Backup non-volatile registers
	push		rbp
	push		rdi
	push		rsi
	push		rbx

	;-- Set registers
	push		rcx				; Port
	push		rdx				; Value
	mov		rbp, r8
	mov		edi, DWORD PTR [rbp + 00h]
	mov		esi, DWORD PTR [rbp + 04h]
	mov		ebx, DWORD PTR [rbp + 0ch]
	mov		ecx, DWORD PTR [rbp + 14h]
	pop		rax				; Rax = Value
	pop		rdx				; Rdx = Port
	out		dx, al

	mov		DWORD PTR [rbp + 18h], eax
	mov		DWORD PTR [rbp + 00h], edi
	mov		DWORD PTR [rbp + 04h], esi
	mov		DWORD PTR [rbp + 0ch], ebx
	mov		DWORD PTR [rbp + 14h], ecx

	in		al, dx
	in		al, dx
	in		al, dx

	;-- Restore non-volatile registers
	pop		rbx
	pop		rsi
	pop		rdi
	pop		rbp

	ret
PortWriteByte	ENDP

;---------------------------------------------------
; UINT64 GetCpuTimer (
; );
;---------------------------------------------------
GetCpuTimer		PROC
	rdtsc
	shl		rdx, 32
	or		rax, rdx
	ret
GetCpuTimer		ENDP

	END

