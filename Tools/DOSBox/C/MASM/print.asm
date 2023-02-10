 .386p
 ASSUME cs:code,ds:data ss:STACK
  
data segment use16
	S_Month db	3,0,3 dup(0)
	DayIn	db	'please input day(eg:01):$'
data ends
STACK SEGMENT
	db 256	dup(0)
STACK ENDS
code segment use16
    
start:
	mov ax,data
	mov ds,ax	;初始化
		mov ah, 02H

	lea dx ,DayIn
	mov ah, 09H
	int 21H
code ends
end start

