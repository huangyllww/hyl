;-----------------------------------------------------------
;功能: 通过SMbus 读取内存的SPD 信息
;时间: 2015/09
;环境: DOS + MASM5
;说明: 代码中涉及的smbus规范是基于Intel平台
;---------------------------------------------------------
;功能: 显示ascii 表示的字符
echoch macro ascii
    mov ah,2
    mov dl,ascii
    int 21h
    endm
;---------------------------------------------------------
.386P
;-------------------- data segment ---------------------------------
dseg segment use16
    busnum dw 0000h  ;总线号0 - 00FFh
    devnum dw 001fh  ;设备号0 - 001Fh
    funnum dw 0003h  ;功能号0 - 0003h
    regnum dw 0020h  ;寄存器0 - 00FFh
    ;
    sm_base   dw 0000h ;存放smbus 基地址
    sm_devnum dw 0000h   ;a0/a2/a4/a6
    sm_regnum dw 0000h   ;
    ;
    bufferlen = 128
    buffer db bufferlen dup(0) ;存放spd  128-byte的信息
    buffer1 db 'smbus base address :',0
    buffer2 db 'device index :'
dseg ends
;-------------------- data segment end ----------------------------------
;-------------------- code segment ---------------------------------
cseg segment use16
    assume cs:cseg, ds:dseg
start:
    mov ax,dseg
    mov ds,ax
    ;----------------------扫描PCI ---------------------
    mov si ,offset buffer1	
	mov bl,0fh		
	call print_string
    call pci_read
    mov ax,4c00h
    int 21h
pci_read proc   ;根据busnum,devnum,funnum计算出configure addresss,使用IO读取offset regnum 的内容
    ;protect register
    push ebx
    push dx
    ;clear
    xor eax,eax
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
    push eax
    call print_eax
    pop eax
    mov dx,0cf8h
    out dx,eax
    mov dx,0cfch
    in eax,dx
    call print_eax
    pop dx
    pop ebx
    ret
pci_read endp
;----------------------------------------------
;

print_eax:
	MOV ebp,EAX
	and eax,0f0000000h
	mov cl,28
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f000000h
	mov cl,24
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f00000h
	mov cl,20
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f0000h
	mov cl,16
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f000h
	mov cl,12
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f00h
	mov cl,8
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0f0h
	mov cl,4
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	mov eax,ebp
	and eax,0fh
	call number_conver_ascii
	MOV DL, AL                           ;输出字符
    MOV AH, 02H
 	INT 21H
	 ret

print_string:
                mov cl,ds:[si]
                mov ch,0
                jcxz print_string_ret   ;条件转移 当cx=0 跳转             
                mov al,ds:[si]
                ; mov bl,00001111b    ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
                call print_char_color
                inc si
                jmp print_string
        print_string_ret:
			ret
	number_conver_ascii:
				cmp al,10
				jb  conver0_9
				jnb convera_f
				
			conver0_9:
				add al,30h
				jmp conver_end
			convera_f:
				add al,57h
				jmp conver_end
			conver_end:
			ret
	print_blank:
			mov al,' '
			mov bl,0fh
			call print_char_color
		ret	
	print_colon:
			mov al,':'
			mov bl,0fh
			call print_char_color
		ret	
    
;;---------------------------------------------------
;;  ---打印一个彩色字符---
;;para：al:输出字符，bl：输出字符颜色
;;ret：DH:行(Y坐标)，DL:列(X坐标)
;;---------------------------------------------------
    print_char_color:
            mov ah,03h       ;读取光标信息
            mov bh,00h       ;第0页
            int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)

            mov ah,09h          ;在光标位置显示字符
			; mov al,31h          ;字符 ASCII
			; mov bl,0fb        ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
			mov bh,00h          ;第0页
			mov cx,01h          ;字符重复个数
			int 10h

			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)

			add dl,1         ;将光标向后移动一列

			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			; mov dh,00h       ;dh:行号
			; mov dl,00h       ;dl:列号
			int 10h
        ret
;;---------------------------------------------------
;;  ---换行---
;;para:	null
;;ret：	null
;;打印函数---------------------------------------------------		
	print_nextline:
			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)

			add dh,1         ;DH:行(Y坐标)向下移动一行
			mov dl,0		 ;DL:列(X坐标) 移动到行首

			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			; mov dh,00h       ;dh:行号
			; mov dl,00h       ;dl:列号
			int 10h
		ret
;-------------------------------------------

iodelay proc     ;延时函数
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
cseg ends
;------------- code segment end --------------------------
    end start