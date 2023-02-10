.586p

assume cs:code,ds:data

; 给出存储空间
data segment
	
	tiptable	dd tip1,tip2,tip3,tip4,tip5,tip6
	tip1		db 'Please enter the Bus number:(Hex)',0
	tip2		db 'Then enter the Device number:(Hex)',0
	tip3		db 'Then enter the Function number:(Hex)',0
	tip4 		db 'Subsystem Vendor ID:',0
	tip5		db 'Subsystem ID:',0
	tip6		db 'Press enter/c to exit/continue.',0
	address		dd 0
	information	db 0,0,0,0

data ends

code segment use16

start:
;主程序
	mov eax,data
	mov ds,eax
	mov eax,0b800h
	mov es,eax

s:
;初始化
	mov eax,8000002ch
	mov dword ptr address[0],eax
	mov edx,0
	
;清屏
	call clear
	
;第一行，提示输入Bus
	mov dh,0
	mov dl,0
	mov ebx,0
	mov ebx,tiptable[ebx]
	call show
	
;第二行，输入处理，可退格
	mov edi,160*1+0
	call getstr
	
;获取Bus
	mov edi,160*1+0
	mov ecx,16;获取数字左移，*16*16*16*16，2的16次方
	call getin

;第三行，提示输入Device
	mov dh,2
	mov dl,0
	mov ebx,4
	mov ebx,tiptable[ebx]
	call show
	
;第四行，输入处理，可退格
	mov edi,160*3+0
	call getstr
	
;获取Device
	mov edi,160*3+0
	mov ecx,11;获取数字左移，*16*16*8，2的11次方
	call getin
	
;第五行，提示输入Function
	mov dh,4
	mov dl,0
	mov ebx,8
	mov ebx,tiptable[ebx]
	call show
	
;第六行，输入处理，可退格
	mov edi,160*5+0
	call getstr
	
;获取Function
	mov edi,160*5+0
	mov ecx,8;获取数字左移，*16*16，2的8次方
	call getin
	
;空间访问
	mov dx,0cf8h
	mov dword ptr eax,address[0]
	out dx,eax
	
	mov dx,0cfch
	in eax,dx
	mov dword ptr information[0],eax
	mov eax,0
	
;第七行，显示VID
	mov dh,6
	mov dl,0
	mov ebx,12
	mov ebx,tiptable[ebx]
	call show
	
	mov esi,1
	add edi,2
	call showID

;显示DID
	mov dh,6
	mov dl,40
	mov ebx,16
	mov ebx,tiptable[ebx]
	call show
	
	mov esi,3
	add edi,2
	call showID
	
;第八行，提示继续/退出
	mov dh,7
	mov dl,0
	mov ebx,20
	mov ebx,tiptable[ebx]
	call show
	
	mov ah,0
	int 16h
	
	mov bh,1ch
	cmp ah,bh;若为Enter的扫描码，结束
	je ok
	jmp s

ok:
	mov ax,4c00h
	int 21h
ret
;主程序结束	

;清屏模块
clear:
	push edi
	push ecx
	
	mov edi,0
	mov ecx,2000
	
s1:
	mov byte ptr es:[160*0+edi],' '
	add edi,2
	loop s1
	
	pop ecx
	pop edi
	ret
;清屏模块结束

;显示模块，根据传入的tip地址显示提示
show:
	push eax
	push ebx
	push ecx
	
	mov edi,0
	mov al,160
	mov ah,0
	mul dh
	mov di,ax
	add dl,dl
	mov dh,0
	add di,dx
	
shows:
	mov cl,ds:[ebx]
	mov ch,0
	jcxz showok
	mov byte ptr es:[edi],cl
	inc bx
	add edi,2
	jmp shows
	
showok:
	pop ecx
	pop ebx
	pop eax
	ret
;显示模块结束

;键盘输入处理
getstr:
	push eax
	push ebx
	
getstrs:
	mov ah,0
	int 16h
	cmp al,20h
	jb nochar
	
	mov es:[edi],al
	add edi,2
	jmp getstrs
	
nochar:
	cmp ah,0eh
	je backspace
	cmp ah,1ch
	je ent
	jmp getstrs
	
backspace:
	sub edi,2
	mov byte ptr es:[edi],' '
	
	jmp getstrs
	
ent:
	pop ebx
	pop eax
	ret
;键盘输入处理结束

;获取信息模块
getin:
	push eax
	push ebx
	
	mov eax,0
	mov ebx,0
	mov al,es:[edi]
	mov bl,es:[edi+2]
	
	;对大写字母、小写字母、数字进行不同处理，小写字母减去57h，大写字母减去37h，数字减去30h
	mov dl,41h
	;此处由于下面ax在比较之前已经减去了7h，故对比对象dh不为61h(a的ASC码)，而是5ah
	mov dh,5ah
	cmp al,dl
	jb num1
	sub eax,7h
	cmp al,dh
	jb num1
	sub eax,20h
	
num1:
	sub eax,30h
	
	;同上
	cmp bl,dl
	jb num2
	sub ebx,7h
	cmp bl,dh
	jb num2
	sub ebx,20h
	
num2:
	sub ebx,30h
	;处理完毕
	
	;
	mov edx,16
	mul edx
	add eax,ebx
	
	;乘法移位
s2:
	mov edx,2;注意重新赋值，乘法会改变edx
	mul edx
	loop s2
	
	add dword ptr address[0],eax
	
	pop ebx
	pop eax
	ret
;获取信息模块结束

;信息显示模块
showID:
	push eax
	push ebx
	
	mov al,information[esi]
	mov ah,0
	
	;除以16以分离两数（低位商即原数值的高位，高位余数即原数值的低位）
	mov bl,16
	div bl
	
	;对大写字母、数字进行不同处理
	mov bl,10
	cmp al,bl
	jb num3
	add al,7h
	
num3:
	add al,30h
	
	cmp ah,bl
	jb num4
	add ah,7h
	
num4:
	add ah,30h
	;处理完毕
	
	mov byte ptr es:[edi],al
	mov byte ptr es:[edi+2],ah
	
	mov al,information[esi-1]
	mov ah,0
	
	;同上
	mov bl,16
	div bl
	
	mov bl,10
	cmp al,bl
	jb num5
	add al,7h
	
num5:
	add al,30h
	
	cmp ah,bl
	jb num6
	add ah,7h
	
num6:
	add ah,30h

	mov byte ptr es:[edi+4],al
	mov byte ptr es:[edi+6],ah

	pop ebx
	pop eax
	ret
;信息显示模块结束

code ends

end start