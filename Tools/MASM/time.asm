ASSUME cs:CODE, ds:DATA, ss:STACK 

DATA SEGMENT
	S_Year 	db 	5,0,5 dup(0)
	S_Month db	3,0,3 dup(0)
	S_Day 	db	3,0,3 dup(0)
	YearIn	db	'please input year(eg:2020):$'
	MonthIn	db	'please	input month(eg:05):$'
	DayIn	db	'please input day(eg:01):$'
	WrongInfo db 'your input is illegal,please input again!$'
	ReaptInfo db 'Would you like to check again?(Y/n)	$'
	year 	dw	0
	month 	db	0
	day 	db	0
	Max_Day db	0,31,28,31,30,31,30,31,31,30,31,30,31
	tmp		dw	0
	week1	db	'Monday$'
	week2	db	'Tuesday$'
	week3	db	'Wednesday$'
	week4	db	'Thursday$'
	week5	db	'Friday$'
	week6	db	'Saturday$'
	week7	db	'Sunday$'
	hint	db	' is $'

DATA ENDS

STACK SEGMENT
	db 256	dup(0)
STACK ENDS

CODE SEGMENT

NEXTLINE PROC NEAR	;输出换行
	mov ah, 02H
	mov	dl, 0DH
	int 21H
	mov dl, 0AH
	int 21H
	ret
NEXTLINE ENDP

MAIN PROC
START:
	mov ax,DATA
	mov ds,ax	;初始化
INPUTYEAR:
	call NEXTLINE
	lea dx, YearIn	;提示输入year
	mov ah, 09H
	int 21H
	lea dx, S_Year 	;读入year
	mov ah, 0AH
	int 21H

	call NEXTLINE	;换行
	;检查Year是否合法
	mov cl, S_Year+1	;将实际长度保存到cx中
	mov ch, 0
	cmp cx, 4		;检查长度，同时计算year的值
	jnz WRONGYEAR
	mov year, 0
	mov si, 2
CHECKYEAR:
	;判断是否是数字
	mov dl, S_Year[si]
	cmp dl, '0'
	jl	WRONGYEAR
	cmp dl, '9'
	jg	WRONGYEAR
	;是数字，计算year
	mov ax, year
	mov bx, 10
	mul bx
	mov dl, S_Year[si]
	sub dl, '0'
	mov dh, 0
	add ax, dx
	mov year, ax
	inc si
	loop CHECKYEAR
	;year计算完毕
	jp	INPUTMONTH

WRONGYEAR:
	lea dx, WrongInfo	;输出错误信息
	mov ah, 09H
	int 21H
	call NEXTLINE		;换行
	jp	INPUTYEAR		;重新输入

INPUTMONTH:
	lea dx, MonthIn		;提示输入month
	mov ah, 09H
	int 21H
	lea dx, S_Month 	;读入month
	mov ah, 0AH
	int 21H

	call NEXTLINE		;换行
	;检查Month是否合法
	mov cl, S_Month+1	;将实际长度保存到cx中
	mov ch, 0
	cmp cx, 2			;检查长度，同时计算month的值
	jnz WRONGMONTH
	mov month, 0
	mov si, 2
CHECKMONTH:
	;判断是否是数字
	mov dl, S_Month[si]
	cmp dl, '0'
	jl	WRONGMONTH
	cmp dl, '9'
	jg	WRONGMONTH
	;是数字，计算month
	mov al, month
	mov bl, 10
	mul bl
	sub dl, '0'
	add al, dl
	mov month, al
	inc si
	loop CHECKMONTH
	;month计算完毕
	;判断month是否在合法范围内
	mov dl, month
	cmp dl, 1
	jl	WRONGMONTH
	cmp dl, 12
	jg	WRONGMONTH
	jmp INPUTDAY

WRONGMONTH:
	lea dx, WrongInfo	;输出错误信息
	mov ah, 09H
	int 21H
	call NEXTLINE		;换行
	jmp	INPUTMONTH		;重新输入

INPUTDAY:
	lea dx, DayIn	;提示输入day
	mov ah, 09H
	int 21H
	lea dx, S_Day 	;读入day
	mov ah, 0AH
	int 21H

	call NEXTLINE	;换行
	;检查day是否合法
	mov cl, S_Day+1	;将实际长度保存到cx中
	mov ch, 0
	cmp cx, 2		;检查长度，同时计算day的值
	jnz WRONGDAY_TMP
	mov si, 2
	mov day, 0
CHECKDAY:
	
	;判断是否是数字
	mov dl, S_Day[si]
	cmp dl, '0'
	jl	WRONGDAY_TMP
	cmp dl, '9'
	jg	WRONGDAY_TMP
	;是数字，计算day
	mov al, day
	mov bl, 10
	mul bl
	sub dl, '0'
	add al, dl
	mov day, al
	inc si
	loop CHECKDAY
	;day计算完毕
	;检查day是否在合法范围内

	mov dl, day
	cmp dl, 1
	jl	WRONGDAY
	mov cl, month
	mov ch, 0
	mov si, cx
	cmp dl, Max_Day[si]	;检查有没有该月份最大的一天
	jg	CHECKLEAP		;检查是不是闰月

	jmp	INPUTEND		;输入结束

WRONGDAY_TMP:
	jmp WRONGDAY

CHECKLEAP:
	mov dh, month
	cmp dh, 2
	jne WRONGDAY		;如果不是2月就肯定不对
	;判断是不是闰年if(year%4==0 && year%100!=0 || year%400==0) 是闰年
	mov ax, year
	mov dx, 0
	mov bx, 400
	div bx
	cmp dx, 0
	je	ISLEAP 			;可以被400整除说明是闰月
	mov ax, year
	mov dx, 0
	mov bx, 4
	div bx
	cmp dx, 0
	jne ISNOTLEAP		;不能被4整除说明不是闰月
	mov ax, year
	mov dx, 0
	mov bx, 100
	div bx
	cmp dx, 0
	je 	ISNOTLEAP		;能被100整除说明不是闰月
ISLEAP:
	mov dl, day
	cmp dl, 29			
	jg	WRONGDAY		;比29还大
	jmp	INPUTEND		;输入结束

ISNOTLEAP:
WRONGDAY:
	lea dx, WrongInfo	;输出错误信息
	mov ah, 09H
	int 21H
	call NEXTLINE		;换行
	jmp	INPUTDAY		;重新输入

INPUTEND:				;输入合法，开始使用基姆拉尔森公式计算答案
	mov dl, month
	cmp dl, 1
	je	ADDMONTH
	cmp	dl, 2
	je 	ADDMONTH
	jmp CALCULATE

ADDMONTH:
	mov al, month
	add al, 12
	mov month, al
	mov ax, year
	sub ax, 1
	mov year, ax

CALCULATE:

	mov bl, day
	mov bh, 0			;bx := day
	add bl, month
	add bl, month 		;bx += 2*month

	mov al, month
	add al, 1
	mov ah, al
	add al, ah 
	add al, ah 			;al := 3*(month+1)

	mov ah, 0
	mov cl, 5
	div cl
	mov ah, 0			;ax := al/5
	add bx, ax			;bx += ax

	add bx, year 		;bx += year
	mov ax, year
	mov dx, 0
	mov cx, 4
	div cx
	add bx, ax			;bx += year/4
	mov ax, year
	mov dx, 0
	mov cx, 100
	div cx
	sub bx, ax			;bx -= year/100
	mov ax, year
	mov dx, 0
	mov cx, 400
	div cx
	add bx, ax			;bx += year/400

	add bx, 1			;bx += 1
	mov ax, bx
	mov dx, 0
	mov cx, 7
	div cx				;bx %= 7
	mov bx, dx

	;基姆拉尔森公式计算完毕

OUTPUT:
	call NEXTLINE
	mov al, S_Year+1
	add al, 2
	mov ah, 0
	mov si, ax	
	mov S_Year[si], '$'	;加上字符串终结符$
	lea dx, S_Year+2	;输出年份
	mov ah, 09H
	int 21H

	mov dl, '\'
	mov ah, 02H
	int 21H

	mov al, S_Month+1
	add al, 2
	mov ah, 0
	mov si, ax	
	mov S_Month[si], '$'
	lea dx, S_Month+2
	mov ah, 09H
	int 21H

	mov dl, '\'
	mov ah, 02H
	int 21H

	mov al, S_Day+1
	add al, 2
	mov ah, 0
	mov si, ax	
	mov S_Day[si], '$'
	lea dx, S_Day+2
	mov ah, 09H
	int 21H

	lea	dx,	hint
	mov ah,	09H
	int 21H

	mov al, bl
	cmp al, 0
	jnz D1
	lea dx, week7
	mov ah, 09H
	int 21H
	jmp	D7

INPUTYEAR_TMP1:
	call NEXTLINE
	jmp INPUTYEAR

D1:	
	cmp al, 1
	jnz D2
	lea dx, week1
	mov ah, 09H
	int 21H
	jmp	 D7
D2:
	cmp al, 2
	jnz D3
	lea dx, week2
	mov ah, 09H
	int 21H
	jmp	D7 
D3:
	cmp al, 3
	jnz D4
	lea dx, week3
	mov ah, 09H
	int 21H
	jmp	D7 
D4:
	cmp al, 4
	jnz D5
	lea dx, week4
	mov ah, 09H
	int 21H
	jmp	D7 
D5:
	cmp al, 5
	jnz D6
	lea dx, week5
	mov ah, 09H
	int 21H
	jmp	D7 
D6:
	cmp al, 6
	jnz D7
	lea dx, week6
	mov ah, 09H
	int 21H
D7:
	call NEXTLINE
	call NEXTLINE

	lea dx, ReaptInfo	;输出重复查询信息
	mov ah, 09H
	int 21H

	mov ah, 01H			;输入字符
	int 21H

	cmp al, 'Y'
	jz INPUTYEAR_TMP1	;跳转到中继点1
	cmp al, 'y'
	jz INPUTYEAR_TMP1 	;跳转到中继点1

	call printweek1
	mov ax, 4C00H		;程序结束
	int 21H


	printweek1:
		mov dx,offset week1
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	printweek2:
		mov dx,offset week2
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	printweek3:
		mov dx,offset week3
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	printweek4:
		mov dx,offset week4
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	printweek5:
		mov dx,offset week5
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	printweek6:
		mov dx,offset week6
		mov ah,09h						;显示字符串，'$'结尾
		int 21h
	ret
	printweek7:
		mov dx,offset week7
		mov ah,09h						;显示字符串，'$'结尾
		int 21h


MAIN ENDP

CODE ENDS
END START
	