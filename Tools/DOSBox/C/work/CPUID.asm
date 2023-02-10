.586p

assume cs:code,ds:data

data segment

	db 128 dup(0)
	
data ends

code segment use16
start:
	mov eax,data
	mov ds,eax
	mov eax,0
	cpuid
	
	mov ds:[0],ebx
	mov ds:[4],edx
	mov ds:[8],ecx
	
	mov eax,1
	cpuid
	
	mov ax,4c00h
	int 21h
code ends
end start