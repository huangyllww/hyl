






echonch macro ascii
	mov ah,2
	mov dl,ascii
	int 21h
	endm
	
	.386p
	
dseg segment use16
	busnum dw 0000h
	devnum dw oo1fh
	funnum dw 0007h
	regnum dw 00ffh
	
	sm_base dw 0000h
	sm_devnum dw 0000h
	sm_regnum dw 0000h
	
	bufferlen = 128
	buffer db bufferlen dup(0)
	buffer1 db 'smbus base address :'
	buffer2 db 'device index :'
dseg ends


cseg segment use16
	assume cs:cseg, ds:dseg
start:
	mov ax,dseg
	mov ds,ax
	
	mov busnum,0000h
	mov devnum,0000h
	mov funnum,0000h
	mov regnum,0000h
nextreg:
	call pci_read
	cmp ax,0ffffh
	jz nextfun
	;
	add regnum,08h
	call pci_read
	and eax,0ffffff00h
	cmp eax,0c050000h
	jz find
nextfun:
	mov regnum,0000h
	inc funnum
	cmp funnum,0007h
	ja nextdev
	jmp nextreg
nextdev:
	mov regnum,0000h
	mov funnum,0000h
	inc devnum
	cmp devnum,001fh
	ja nextbus
	jmp nextreg
nextbus:
	mov regnum,0000h
	mov funnum,0000h
	mov devnum,0000h
	inc busnum
	cmp busnum,0005h
	ja notfind
	jmp nextreg
	
	
find:
	sub regnum,08h
	add regnum,20h
	call pci_read
	and ax,0fffeh
	mov sm_base,ax
	;
	;
	mov dx,offset buffer1
	mov,cx,20
	mov ah,40h
	int 21h
	;
	mov ax,sm_base
	shr ax,8
	push ax
	shr al,4
	call toascii
	echoch al
	pop ax
	call toascii
	echoch al
	;
	mov ax,sm_base
	push ax
	shr al,4
	call toascii
	echoch al
	pop ax
	call toascii
	echoch al
	echoch odh
		echoch oah
	;
	mov sm_devnum,00a0h
	;
nextd:
	mov sm_regnum,0000h
	mov dx,offset buffer2
	moc cx,14
	mov ah,40h
	int 21h
	;
	mov ax,sm_devnum
	push ax
	shr al,4
	call toascii
	echoch al
	pop ax
	call toascii
	echoch al
	echoch ' '
	;
	call sm_read
	echoch 0dh
		echoch oah
		add sm_devnum,2
		cmp sm_devnum,0a8h
		jl nextd
		;
	;
notfind:
	mov ah,4ch
	int 21h
;
;
;
;
;
;
sm_read proc
	push dx
	push ax
	;
	mov ax,dseg
	mov es,ax
	mov di,offset buffer
nextch:
	;
	call iodelay
	mov dx,sm_base
	add dx,00h
	mov al,0feh
	out dx,al
	call iodelay
	;
	mov dx,sm_base
	add dx,04h
	mov ax,sm_devnum
	or al,01h
	out dx,al
	call iodelay
	;
	mov dx,sm_base
	add dx,03h
	mov ax,sm_regnum
	out dx,al
	call iodelay
	inc sm_regnum
	;
	mov dx,sm_base
	add dx,02h
	mov al,48h
	out dx,al
	call iodelay
	call iodelay
	;
	mov dx,sm_base
	add dx,00h
	in al,dx
	cmp al,04h
	jz enderr
	;
	mov dx,sm_base
	add dx,05h
	in al,dx
	mov es:[di],al
	inc di
	cmp sm_regnum,7fh
	ja endsm
	jmp nextch
endsm:
	call print_buffer
	pop ax
	pop dx
	ret
enderr:
	pop ax
	pop dx
	ret
sm_read endp
;
;
;
iodelay proc
	push cx
	mov cx,0ffffh
delay:
	loop delay
	mov cx,0ffffh
delay1:
	loop delay1
	pop cx
	ret
iodelay    endp
;
;
;
;
;
pci_read proc
	;protect register
	push ebx
	push dx
	;clear
	xor eax，eax
	xor ebx,ebx
	;enable
	add eax,1h
	shl eax,31
	;bus number
	mov ebx,ds:[00]
	and ebx,0ffh
	shl ebx,16
	add eax,ebx
	;device number
	xor ebx,ebx
	mov ebx,ds:[02]
	and ebx,0ffh
	shl ebx,11
	add eax,ebx
	;function number
	xor ebx,ebx
	mov ebx,ds:[04]
	and ebx,0ffh
	shl ebx,8
	add eax,ebx
	;register
	xor ebx,ebx
	mov ebx,ds:[06]
	and ebx,0ffh
	add eax,ebx
	;read IO
	mov dx,0cf8h
	out dx,eax
	mov dx,0cfch
	in eeax,dx
	;resume register
	pop dx
	pop ebx
	ret
pci_read endp
;
;
;
print_buffer proc
	push ax
	push ds
	push si
	push cx
	push bp
	;
	mov ax,dseg
	mov ds,ax
	mov si,offset buffer
		mov cx,bufferlen
		cld
nextline3:
	dec cx
		echoch 0dh
		echoch 0ah
		mov bp,16
nextch3:
	lodsb
	;
	push ax
	shr al,4
	call toascii
	echoch al
	pop ax
	call toascii
	echoch al
	echoch ' '
	;
	dec bp
		jz nextline3
	loop nextch3
	;
	pop bp
	pop cx
	pop si
	pop ds
	pop ax
	ret
print_buffer endp
;
;
;
;
toascii proc
	and al,0fh
	add al,90h
	daa
	adc al,40h
	daa
	ret
toascii endp
;
cseg ends
;
	end start