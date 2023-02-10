;-----------------------------------------------------------
;功能: 通过SMbus 读取内存的SPD 信息
;时间: 2015/09
;环境: DOS + MASM5
;说明: 代码中涉及的smbus规范是基于Intel平台
;---------------------------------------------------------
;功能: 显示ascii 表示的字符
;---------------------------------------------------------
 
.386  ;eax ,ebx 等寄存器需添加.386.model

;-------------------- data segment ---------------------------------
stack segment use16
    db   64 dup(0)
stack ends
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
    buffer1 db 'smbus base address :',2 dup(0)
    buffer2 db 'end :',2 dup(0) 
    smbus_base  db 'smbus base address is:',2 dup(0) 

   
data ends
;-------------------- data segment end ----------------------------------
;-------------------- code segment --------------------------------- 
assume cs:cseg, ds:data, ss:stack
cseg segment use16



  start:
    mov ax,data
    mov ds,ax
    mov ax,stack  
     mov ss,ax
    ;----------------------扫描PCI ---------------------
    mov busnum,0000h
    mov devnum,0000h
    mov funnum,0000h
    mov regnum,0000h
nextreg:
    call pci_read     ;读取pci 配置空间的前4 个字节
    cmp ax,0ffffh     ;判断设备是否存在
    jz nextfun        ;不存在，跳到下一个fun
    ;
    add regnum,08h    ;读class code
    call pci_read
    and eax,0ffffff00h
    cmp eax,0c050000h ;根据class code 判断SMbus Controller( 0c0500 )
    jz find           ;是SMbus Controller
nextfun:
    mov regnum,0000h
    inc funnum
    cmp funnum,0007h
    ja nextdev        ;funnum 大于 7,跳到下一个dev
    jmp nextreg
nextdev:
    mov regnum,0000h
    mov funnum,0000h
    inc devnum
    cmp devnum,001fh
    ja nextbus      ;devnum 大于 1fh，跳到下一个bus
    jmp nextreg
nextbus:
    mov regnum,0000h
    mov funnum,0000h
    mov devnum,0000h
    inc busnum
    cmp busnum,0005h
    ja notfind        ;busnum 大于5，没找到SMbus Controller --结束
    jmp nextreg

    ;--------------------找到SMbus  Controller-------------------
find:
    sub regnum,08h
    add regnum,20h  ;SMbus 的基地址在pci 配置空间中的偏移地址
    call pci_read   ;读取SMbus 的基地址
    and ax,0fffeh   ;1111_1111_1111_1110,最后一位1 :表示端口方式
    mov sm_base,ax  ;把基地址保存到sm_base
    ;
    ;-----------------------打印smbus 的基地址------------
    mov dx,offset buffer1
    mov cx,20
    mov ah,40h
    int 21h
    ;
    mov ax,sm_base
    shr ax,8
    push ax
    shr al,4
    call toascii
    echo al
    pop ax
    call toascii
    echo al
    ;
    mov ax,sm_base
    push ax
    shr al,4
    call toascii
    echo al
    pop ax
    call toascii
    echo al
    echo 0dh  ;换行
        echo 0ah
    ;--------------------读smbus  设备的SPD ----------------
    mov sm_devnum,00a0h   ;设备a0
    ;
nextd:
    mov sm_regnum,0000h
    mov dx,offset buffer2
    mov cx,14
    mov ah,40h
    int 21h
    ;
    mov ax,sm_devnum
    push ax
    shr al,4
    call toascii
    echo al
    pop ax
    call toascii
    echo al
    echo ' '
    ;
    call sm_read      ;读128-byte 的spd，存入buffer , 并显示buffer
    echo 0dh        ;换行
        echo 0ah
        add sm_devnum,2   ;设备号a0/a2/a4/a6
        cmp sm_devnum,0a8h
        jl nextd          ;小于，循环
        ;----------------------------------------
    ;--------------结束，返回DOS -------------
notfind:
    mov ah,4ch
    int 21h
;---------------------------------------------
;------------------子程序----------------
;----------------------------------------
;功能: 通过smbus IO registers 读取spd 信息，并存入buffer
;入口:
;
sm_read proc
    push dx
    push ax
    ;
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
    ;
    mov dx,sm_base
    add dx,04h       ;slave address register
    mov ax,sm_devnum ;从设备地址:a0/a2/a4/a6  ,
    or al,01h        ;末位:1 - 表示读
    out dx,al
    call iodelay
    ;
    mov dx,sm_base
    add dx,03h       ;command register
    mov ax,sm_regnum ;寄存器索引
    out dx,al
    call iodelay
    inc sm_regnum
    ;
    mov dx,sm_base
    add dx,02h       ;control register
    mov al,48h       ;设置读写模式:字节(48h)、字(4ch)、块(54h)
    out dx,al
    call iodelay
    call iodelay
    ;
    mov dx,sm_base
    add dx,00h
    in al,dx
    cmp al,04h       ;判断读结果
    jz enderr        ;读出错,设备不存在,返回
    ;
    mov dx,sm_base
    add dx,05h      ;data0 register
    in al,dx        ;回读数据
    mov es:[di],al  ;数据保存到buffer 中
    inc di
    cmp sm_regnum,7fh
    ja endsm        ;128-byte  读完
    jmp nextch
endsm:                    ;读完结束，打印buffer 后，结束
    call print_buffer
    pop ax
    pop dx
    ret
enderr:                 ;读出错结束，直接退出
    pop ax
    pop dx
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
pci_read proc
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
    ;read IO
    mov dx,0cf8h
    out dx,eax
    mov dx,0cfch
    in eax,dx
    ;resume register
    pop dx
    pop ebx
    ret
pci_read endp
;----------------------------------------------
;功能:打印buffer的内容(以ascii 形式)
;
print_buffer proc
    push ax
    push ds
    push si
    push cx
    push bp
    ;
    mov ax,data      ;设置源地址(buffer 的地址)
    mov ds,ax
    mov si,offset buffer
        mov cx,bufferlen
        cld
nextline3:
    dec cx
        echo 0dh
        echo 0ah
        mov bp,16
nextch3:
    lodsb            ;加载一个字节到al
    ;
    push ax
    shr al,4
    call toascii
    echo al
    pop ax
    call toascii
    echo al
    echo ' '
    ;
    dec bp
        jz nextline3
    loop nextch3
    ;
    pop bp     ;出栈顺序与入栈顺序相反
    pop cx
    pop si
    pop ds
    pop ax
    ret
print_buffer endp
;----------------------------------------
;功能:把al 的低4位转成ascii码,并存入al
;入口: al
;出口: al
toascii proc
    and al,0fh  ;高四位清零
    add al,90h  ;1001_xxxx
    daa
    adc al,40h
    daa
    ret
toascii endp
;-------------------------------------------
cseg ends
;------------- code segment end --------------------------
    end start