.386p

echoch macro ascii
    mov ah,2
    mov dl,ascii
    int 21h
endm

assume cs:code,ds:data,ss:stacks

data segment
    busnum dw 0000h  ;总线号0 - 00FFh
    devnum dw 0014h  ;设备号0 - 001Fh
    funnum dw 0000h  ;功能号0 - 0007h
    regnum dw 0020h  ;寄存器0 - 00FFh

    sm_base   dw 0000h ;存放smbus 基地址
    sm_devnum dw 0000h   ;a0/a2/a4/a6
    sm_regnum dw 0000h   ;

    bufferlen = 128
    buffer db bufferlen dup(0) ;存放spd  128-byte的信息
    buffer1 db 'smbus base address :'
    buffer2 db 'device index :'
    putin db 64
data ends

stacks segment
    db 128 dup(0)
stacks ends

code segment use16
start:
            mov ax,data
            mov ds,ax
            
            call pci_read
            and ax,0fffeh                   ;1111_1111_1111_1110,最后一位1 :表示端口方式
            mov sm_base,ax                  ;把基地址保存到sm_base

;-----------------------打印smbus 的基地址------------
            lea dx,buffer1      ;mov dx,offset buffer1
            mov cx,20
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
            echoch 0dh  ;换行
            echoch 0ah
;--------------------读smbus  设备的SPD ----------------

        ;
    nextd:
            mov sm_regnum,0000h
            lea dx,buffer2      ;mov dx,offset buffer2
            mov cx,14
            mov ah,40h    
            int 21h
        ;
            lea dx,putin
            mov ah,10
            int 21h
            mov si,dx
            mov ah,ds:[si+2]
            sub ah,37h
            shl ah,1
            shl ah,1
            shl ah,1
            shl ah,1
            mov al,ds:[si+3]
            sub al,30h
            cmp al,10
            jb mv
            sub al,7
        mv: add al,ah
            mov ah,0
            
            mov sm_devnum,ax   ;设备a0

;            mov ax,sm_devnum
;            push ax
;            shr al,4 
;            call toascii
;            echoch al
;            pop ax
;            call toascii 
;            echoch al    
;            echoch ' '
        ;
            call sm_read                    ;读128-byte 的spd，存入buffer , 并显示buffer
            echoch 0dh                      ;换行
            echoch 0ah
        ;    add sm_devnum,2                 ;设备号a0/a2/a4/a6
        ;    cmp sm_devnum,0a8h
        ;    jl nextd                        ;小于，循环
;----------------------------------------
;--------------结束，返回DOS -------------
    notfind:
            mov ah,4ch
            int 21h


pci_read:                                   ;计算配置寄存器，读取SM BASE ADDRESS
            ;protect register
            push ebx     
            push dx
            ;clear
            xor eax,eax
            xor ebx,ebx
            ;enable
            add eax,1h                      
            shl eax,31                      ;(eax)=80000000h
            ;bus number
            mov ebx,ds:[00]
            and ebx,0ffh
            shl ebx,16
            add eax,ebx                     ;(eax)=80000000h
            ;device number
            xor ebx,ebx
            mov ebx,ds:[02]                 
            and ebx,0ffh
            shl ebx,11
            add eax,ebx                     ;(eax)=8000F800h
            ;function number
            xor ebx,ebx
            mov ebx,ds:[04]
            and ebx,0ffh
            shl ebx,8
            add eax,ebx                     ;(eax)=8000FB00h
            ;register
            xor ebx,ebx
            mov ebx,ds:[06]
            and ebx,0ffh
            add eax,ebx                     ;(eax)=8000FB20h
            ;read IO
            mov dx,0cf8h
            out dx,eax
            mov dx,0cfch
            in eax,dx
            ;resume register
            pop dx
            pop ebx    
            ret

iodelay:                                   ;延时
            push cx
            mov cx,0ffffh
    delay:
            loop delay
            mov cx,0ffffh
    delay1:
            loop delay1
            pop cx
            ret

sm_read:
            push dx
            push ax

            mov ax,data                     ;设置目的地址(buffer 的地址)
            mov es,ax                       ;段地址为数据段地址
            lea di,buffer       ;mov di,offset buffer            ;偏移地址为缓冲区的偏移地址
    nextch:
            ;----------smbus 访问规范--------
            call iodelay
            mov dx,sm_base                  ;SMbus 的基地址
            add dx,00h                      ;status register
            mov al,0feh                     
            out dx,al
            call iodelay                    ;延时

            mov dx,sm_base
            add dx,04h                      ;slave address register
            mov ax,sm_devnum                ;从设备地址:a0/a2/a4/a6  ,
            or al,01h                       ;末位:1 - 表示读
            out dx,al
            call iodelay

            mov dx,sm_base
            add dx,03h                      ;command register
            mov ax,sm_regnum                ;寄存器索引
            out dx,al  
            call iodelay
            inc sm_regnum

            mov dx,sm_base
            add dx,02h                      ;control register
            mov al,48h                      ;设置读写模式:字节(48h)、字(4ch)、块(54h)
            out dx,al
            call iodelay
            call iodelay

            mov dx,sm_base
            add dx,00h
            in al,dx
            cmp al,04h                      ;判断读结果
            jz enderr                       ;读出错,设备不存在,返回

            mov dx,sm_base
            add dx,05h                      ;data0 register
            in al,dx                        ;回读数据
            mov es:[di],al                  ;数据保存到buffer 中
            inc di
            cmp sm_regnum,7fh
            ja endsm                        ;128-byte  读完
            jmp nextch
    endsm:                                  ;读完结束，打印buffer 后，结束
            call print_buffer
            pop ax
            pop dx
            ret
    enderr:                                 ;读出错结束，直接退出
            pop ax
            pop dx          
            ret

print_buffer:
            push ax
            push ds
            push si
            push cx
            push bp
            ;
            mov ax,data                      ;设置源地址(buffer 的地址)
            mov ds,ax
            lea si,buffer       ;mov si,offset buffer
            mov cx,bufferlen
            cld
    nextline3:
            dec cx
            echoch 0dh  
            echoch 0ah
            mov bp,16  
    nextch3:    
            lodsb                            ;加载一个字节到al
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
            pop bp                              ;出栈顺序与入栈顺序相反
            pop cx
            pop si
            pop ds
            pop ax
            ret

toascii:
            and al,0fh                          ;高四位清零
            add al,90h                          ;1001_xxxx
            daa
            adc al,40h
            daa
            ret

code ends
end start