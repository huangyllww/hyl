;-----------------------------------------------------------
;����: ͨ��SMbus ��ȡ�ڴ��SPD ��Ϣ
;ʱ��: 2015/09
;����: DOS + MASM5
;˵��: �������漰��smbus�淶�ǻ���Intelƽ̨
;---------------------------------------------------------
;����: ��ʾascii ��ʾ���ַ�
;---------------------------------------------------------
 
.386  ;eax ,ebx �ȼĴ��������.386.model

;-------------------- data segment ---------------------------------
stack segment use16
    db   64 dup(0)
stack ends
data segment use16
    busnum dw 0000h  ;���ߺ�0 - 00FFh
    devnum dw 001fh  ;�豸��0 001Fh
    funnum dw 0003h  ;���ܺ�0-  - 0003h
    regnum dw 0020h  ;�Ĵ���0 - 00FFh
    sm_base   dw 0000h ;���smbus ����ַ
    sm_devnum dw 0000h   ;a0/a2/a4/a6
    sm_regnum dw 0000h   ;
    bufferlen = 128
    buffer db bufferlen dup(0) ;���spd  128-byte����Ϣ
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
    ;----------------------ɨ��PCI ---------------------
    mov busnum,0000h
    mov devnum,0000h
    mov funnum,0000h
    mov regnum,0000h
nextreg:
    call pci_read     ;��ȡpci ���ÿռ��ǰ4 ���ֽ�
    cmp ax,0ffffh     ;�ж��豸�Ƿ����
    jz nextfun        ;�����ڣ�������һ��fun
    ;
    add regnum,08h    ;��class code
    call pci_read
    and eax,0ffffff00h
    cmp eax,0c050000h ;����class code �ж�SMbus Controller( 0c0500 )
    jz find           ;��SMbus Controller
nextfun:
    mov regnum,0000h
    inc funnum
    cmp funnum,0007h
    ja nextdev        ;funnum ���� 7,������һ��dev
    jmp nextreg
nextdev:
    mov regnum,0000h
    mov funnum,0000h
    inc devnum
    cmp devnum,001fh
    ja nextbus      ;devnum ���� 1fh��������һ��bus
    jmp nextreg
nextbus:
    mov regnum,0000h
    mov funnum,0000h
    mov devnum,0000h
    inc busnum
    cmp busnum,0005h
    ja notfind        ;busnum ����5��û�ҵ�SMbus Controller --����
    jmp nextreg

    ;--------------------�ҵ�SMbus  Controller-------------------
find:
    sub regnum,08h
    add regnum,20h  ;SMbus �Ļ���ַ��pci ���ÿռ��е�ƫ�Ƶ�ַ
    call pci_read   ;��ȡSMbus �Ļ���ַ
    and ax,0fffeh   ;1111_1111_1111_1110,���һλ1 :��ʾ�˿ڷ�ʽ
    mov sm_base,ax  ;�ѻ���ַ���浽sm_base
    ;
    ;-----------------------��ӡsmbus �Ļ���ַ------------
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
    echo 0dh  ;����
        echo 0ah
    ;--------------------��smbus  �豸��SPD ----------------
    mov sm_devnum,00a0h   ;�豸a0
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
    call sm_read      ;��128-byte ��spd������buffer , ����ʾbuffer
    echo 0dh        ;����
        echo 0ah
        add sm_devnum,2   ;�豸��a0/a2/a4/a6
        cmp sm_devnum,0a8h
        jl nextd          ;С�ڣ�ѭ��
        ;----------------------------------------
    ;--------------����������DOS -------------
notfind:
    mov ah,4ch
    int 21h
;---------------------------------------------
;------------------�ӳ���----------------
;----------------------------------------
;����: ͨ��smbus IO registers ��ȡspd ��Ϣ��������buffer
;���:
;
sm_read proc
    push dx
    push ax
    ;
    mov ax,data           ;����Ŀ�ĵ�ַ(buffer �ĵ�ַ)
    mov es,ax             ;�ε�ַΪ���ݶε�ַ
    mov di,offset buffer  ;ƫ�Ƶ�ַΪ��������ƫ�Ƶ�ַ
nextch:
    ;----------smbus ���ʹ淶--------
    call iodelay
    mov dx,sm_base  ;SMbus �Ļ���ַ
    add dx,00h      ;status register
    mov al,0feh     ;
    out dx,al
    call iodelay    ;��ʱ
    ;
    mov dx,sm_base
    add dx,04h       ;slave address register
    mov ax,sm_devnum ;���豸��ַ:a0/a2/a4/a6  ,
    or al,01h        ;ĩλ:1 - ��ʾ��
    out dx,al
    call iodelay
    ;
    mov dx,sm_base
    add dx,03h       ;command register
    mov ax,sm_regnum ;�Ĵ�������
    out dx,al
    call iodelay
    inc sm_regnum
    ;
    mov dx,sm_base
    add dx,02h       ;control register
    mov al,48h       ;���ö�дģʽ:�ֽ�(48h)����(4ch)����(54h)
    out dx,al
    call iodelay
    call iodelay
    ;
    mov dx,sm_base
    add dx,00h
    in al,dx
    cmp al,04h       ;�ж϶����
    jz enderr        ;������,�豸������,����
    ;
    mov dx,sm_base
    add dx,05h      ;data0 register
    in al,dx        ;�ض�����
    mov es:[di],al  ;���ݱ��浽buffer ��
    inc di
    cmp sm_regnum,7fh
    ja endsm        ;128-byte  ����
    jmp nextch
endsm:                    ;�����������ӡbuffer �󣬽���
    call print_buffer
    pop ax
    pop dx
    ret
enderr:                 ;�����������ֱ���˳�
    pop ax
    pop dx
    ret
sm_read endp
;--------------------------------------------------
;����:��ʱ,�ȴ����������׼����
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
;����: ����eax�еĵ�ַ��ȡpci�����ÿռ�,������eax
;���: busnum��devnum��funnum��regnum
;����: eax
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
;����:��ӡbuffer������(��ascii ��ʽ)
;
print_buffer proc
    push ax
    push ds
    push si
    push cx
    push bp
    ;
    mov ax,data      ;����Դ��ַ(buffer �ĵ�ַ)
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
    lodsb            ;����һ���ֽڵ�al
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
    pop bp     ;��ջ˳������ջ˳���෴
    pop cx
    pop si
    pop ds
    pop ax
    ret
print_buffer endp
;----------------------------------------
;����:��al �ĵ�4λת��ascii��,������al
;���: al
;����: al
toascii proc
    and al,0fh  ;����λ����
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