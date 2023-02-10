;-----------------------------------------------------------
;功能: 通过SMbus 读取内存的SPD 信息
;时间: 2015/09
;环境: DOS + MASM5
;说明: 代码中涉及的smbus规范是基于Intel平台
;---------------------------------------------------------
;功能: 显示ascii 表示的字符
;---------------------------------------------------------
.386P
;-------------------- data segment ---------------------------------
data segment use16
    busnum dw 0000h  ;总线号0 - 00FFh
    devnum dw 001fh  ;设备号0 001Fh
    funnum dw 0003h  ;功能号0-  - 0003h
    regnum dw 0020h  ;寄存器0 - 00FFh
    sm_base   dw 0000h ;存放smbus 基地址
    sm_devnum dw 0000h   ;a0/a2/a4/a6
    sm_regnum dw 0000h   ;
    bufferlen = 128
    buffer db bufferlen dup(0) ;存放spd  128-byte的信息
    buffer1 db 'smbus base address :',2 dup(0)段 
    buffer2 db 'end :',2 dup(0)段 
    smbus_base  db 'smbus base address is:',2 dup(0) 

   
data ends
;-------------------- data segment end ----------------------------------
;-------------------- code segment ---------------------------------
cseg segment use16
    assume cs:cseg, ds:data
start:
    mov ax,data
    mov ds,ax
    ;----------------------扫描PCI ---------------------
    mov sm_devnum,00a0h   ;设备a0
    mov ax,0003h
    int 10h
    call print_smbase;输出提示字符 sm_base addres:
    call pci_read;读取smbase 到eax
    push ax
    call print_ax;显示sm_base addres
    pop ax
    and ax,0fffeh   ;1111_1111_1111_1110,最后一位1 :表示端口方式
    mov sm_base,ax  ;把基地址保存到sm_base
    call print_nextline
    call sm_read;配置 从设备，偏移地址， 读取格式等
notfind:
    mov ax,4c00h
    int 21h
;---------------------------------------------
;------------------子程序----------------
;----------------------------------------
;功能: 通过smbus IO registers 读取spd 信息，并存入buffer
;入口:
;
sm_read proc   ;读取128字节
    push dx
    push ax
    mov ax,data           ;设置目的地址(buffer 的地址)
    mov es,ax             ;段地址为数据段地址
    mov di,offset buffer  ;偏移地址为缓冲区的偏移地址
nextch:
    ;----------smbus 访问规范--------
    call iodelay
    mov dx,sm_base  ;SMbus 的基地址
    add dx,00h      ;status register
    mov al,0feh     ;
    out dx,al
    call iodelay    ;延时
    mov dx,sm_base
    add dx,04h       ;slave address register
    mov ax,sm_devnum ;从设备地址:a0/a2/a4/a6 ，sm_devnum=a0h
    or al,01h        ;末位:1 - 表示读
    out dx,al
    call iodelay
    mov dx,sm_base
    add dx,03h       ;command register
    mov ax,sm_regnum ;寄存器索引
    out dx,al
    call iodelay
    inc sm_regnum
    mov dx,sm_base
    add dx,02h       ;control register
    mov al,48h       ;设置读写模式:字节(48h)、字(4ch)、块(54h)
    out dx,al
    call iodelay
    call iodelay;
    mov dx,sm_base
    add dx,00h
    in al,dx;
    mov dx,sm_base
    add dx,05h      ;data0 register
    in al,dx        ;回读数据
    push al
    call print_al
    call print_blank
    call print_blank
    pop al
    mov es:[di],al  ;数据保存到buffer 中
    inc di
    cmp sm_regnum,07fh
    ja endsm        ;128-byte  读完
    jmp nextch
endsm:                    ;读完结束，打印buffer 后，结束
    pop ax
    pop dx
    call print_nextline
    call print_nextline
    call print_nextline
    mov ax,4c00h
    int 21h
    ret
sm_read endp
;--------------------------------------------------
;功能:延时,等待外设把数据准备好
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
;---------------------------------------------
;功能: 根据eax中的地址读取pci的配置空间,并存入eax
;入口: busnum、devnum、funnum、regnum
;出口: eax
;
pci_read: ;根据busnum,devnum,funnum计算出configure addresss,使用IO读取offset regnum 的内容
    ;protect register
    push ebx  ;保存数据
    push dx
    ;clear
    xor eax,eax   ;运算最快的清零
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
    in eax,dx
    ;resume register
    pop dx
    pop ebx
ret

;----------------------------------------------


	print_string:
                mov cl,ds:[si]
                mov ch,0
                jcxz print_string_ret   ;条件转移 当cx=0 跳转             
                mov al,ds:[si]
                 mov bl,0fh   ;颜色：[7]:闪烁、[6,5,4]:背景R,G,B、[3]:高亮、[2,1,0]:前景R,G,B
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
			mov bh,00h          ;第0页
			mov cx,01h          ;字符重复个数
			int 10h
			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
			add dl,1         ;将光标向后移动一列
			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			int 10h
        ret
;;---------------------------------------------------
;;  ---换行---
;;para:	null
;;ret：	null
;;---------------------------------------------------		
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
print_al:
	and eax,0f0h
	mov cl,4
	shr eax,cl
	call number_conver_ascii
	MOV DL, AL            
    MOV AH, 02H
	int 21h 
	and al,0fh
	call number_conver_ascii
	MOV DL, AL                           ;输出字符
    MOV AH, 02H
 	INT 21H
	 ret
print_ax:
    mov ebp,eax
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
    print_smbase:
        mov si,offset buffer1
        mov bl ,0f0h
        call print_string
    ret
      print_end:
        mov si,offset buffer2
        mov bl ,0f0h
        call print_string
    ret
    
;-------------------------------------------
mov_lie:
            mov ah,03h       ;读取光标信息
            mov bh,00h       ;第0页
            int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
            mov ah,09h          ;在光标位置显示字符
			mov bh,00h          ;第0页
			mov cx,01h          ;字符重复个数
			int 10h
			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
			add dh,4        ;将光标向后移动一列
			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			int 10h
        ret
mov_hang:
            mov ah,03h       ;读取光标信息
            mov bh,00h       ;第0页
            int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
            mov ah,09h          ;在光标位置显示字符
			mov bh,00h          ;第0页
			mov cx,01h          ;字符重复个数
			int 10h
			mov ah,03h       ;读取光标信息
			mov bh,00h       ;第0页
			int 10h          ;CH:光标的起始行、CL:光标的终止行、DH:行(Y坐标)，DL:列(X坐标)
			add dl,8        ;将光标向后移动一列
			mov ah,02h       ;置光标
			mov bh,00h       ;第0页
			int 10h
        ret

cseg ends
;------------- code segment end --------------------------
    end start