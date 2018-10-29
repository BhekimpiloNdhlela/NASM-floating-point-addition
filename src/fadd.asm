; asmsyntax=nasm
;******************************************************************
;* RW244: nasm implementation of floating point addition          *
;******************************************************************

global addf

;******************************************************************
; void addf(float a, float b, float *x);                          *
;******************************************************************
%define a [ebp+8]
%define b [ebp+12]
%define x [ebp+16]

%define f1 [ebp-4] 
%define f2 [ebp-8] 
%define f3 [ebp-12] 

%define s1 [ebp-16]
%define s2 [ebp-20]
%define s3 [ebp-24]

%define e1 [ebp-28]
%define e2 [ebp-32]
%define e3 [ebp-36]

addf:
  push  ebp
  mov   ebp, esp
  sub   esp, 36

;HANDLE SPECIAL CASES
	;check if a == 0 ==> return b
	mov eax, a
	cmp eax, 0x0
	mov eax, b
	je .return
 	xor eax, eax
	;check if b == 0 ==> return a
	mov eax, b 
	cmp eax, 0x0
	mov eax, a
	je .return
	xor eax, eax 
		
;EXTRACT FOR FLOATING NUMBER a
	mov eax, a
	cmp eax, 0x0
	jz .else_a
	;sign part for a = s1	
	mov eax, a
	shr eax, 31
	mov s1, eax
	xor eax, eax
	;fractional part for a = f1
	mov eax, a
	and eax, 0x7fffff
	xor eax, 0x800000
	mov f1, eax
	xor eax, eax
	;exponent part for a = f1
	mov eax, a
	shr eax, 23
	and eax, 0xff
	mov e1, eax
	xor eax, eax
	jmp .endif_a
.else_a:
	mov dword e1, 0x0
	mov dword f1, 0x0
	xor eax, eax
.endif_a:	

;EXTRACT FOR FLOATING NUMBER b 
	mov eax, b
	cmp eax, 0x0
	jz .else_b
	;sign part for b = s1	
	shr eax, 31
	mov s2, eax
	xor eax, eax
	;fractional part for b = f1
	mov eax, b
	and eax, 0x7fffff
	xor eax, 0x800000
	mov f2, eax
	xor eax, eax
	;exponent part for b = f1
	mov eax, b
	shr eax, 23
	and eax, 0xff
	mov e2, eax
	xor eax, eax
	jmp .endif_b
.else_b:
	mov dword e2, 0x0
	mov dword f2, 0x0
.endif_b:	
	;check if a == -b || b == -a ==> return 0x0
	mov eax, e1
	mov edx, e2
	cmp eax, edx
	jne .continue_with_algorithm
	mov eax, f1
	mov edx, f2
	cmp eax , edx
	jne .continue_with_algorithm
	mov eax, s1
	mov edx, s2
	cmp eax, edx
	mov eax, 0x0
	jne .return

.continue_with_algorithm:
;EXPONENT EQAULIZATION
	mov eax, e1
	mov edx, e2
	mov ebx, f1
.while_a:
	cmp eax, edx
	jge .end_while_a
	inc eax
	shr ebx, 1
	jmp .while_a
.end_while_a:
	mov f1, ebx	
	mov ebx, f2
.while_b:
	cmp edx, eax
	jge .end_while_b
	inc edx
	shr ebx, 1
	jmp .while_b
.end_while_b:
	mov f2, ebx
	mov e1, eax
	mov e2, edx

;TAKING THE COMMON EXPONENT
	mov e3, eax
	;clear up the used registers
	xor eax, eax
	xor ebx, ebx
	xor edx, edx

;f1 = (a < 0) ? -f1 : f1
	mov eax, a
	cmp eax, 0x0
	jge .endif0
	xor eax, eax
	mov eax, f1
	neg eax
	mov f1, eax
	xor eax, eax
.endif0:
;f2 = (b < 0) ? -f2 : f2
	mov eax, b
	cmp eax, 0x0
	jge .endif1
	xor eax, eax
	mov eax, f2
	neg eax
	mov f2, eax
	xor eax, eax
.endif1:

; add f1 to f2 and get f3 i.e.) f3 = f1 + f2;
	mov eax, f1
	mov edx, f2
	add eax, edx
	mov f3, eax
	xor eax, eax
	xor edx, edx
	
;if (f3 < 0) => f3 = -f3 and s3 = 1 else s3 = 0
	cmp dword f3, 0x0
	jge .else0
	mov eax, f3
	neg eax
	mov f3, eax
	mov dword s3, 0x1
	xor eax, eax
	jmp .endif_0
.else0:
	mov dword s3, 0x0
.endif_0:

;NORMALIZE f3, s3, & e3
	mov eax, f3 	
	xor eax, 0x800000
	cmp eax, 0
	je .combine_float_parts
	
.while1:
	mov eax, f3
	xor eax, 0x800000
	cmp eax, 0
	je .while2
	
  	and eax, 0xff000000
	cmp eax, 0
	je .while2
	shr dword f3, 1
	add dword e3, 1
	jmp .while1

.while2:
  	cmp dword f3, 0
	je .combine_float_parts
	mov eax, f3
	shr eax, 23
	cmp eax, 1 
	je .combine_float_parts
	shl dword f3, 1
	sub dword e3, 1
	jmp .while2

;COMBINE s3, e3, & f3 to form a 32-bit real value r3
.combine_float_parts:
	mov ah, s3
	mov al, e3
	shl eax, 23
	xor dword f3, 0x800000
	add eax, f3
	
;RETURN *x
.return:
	mov edx, x
	mov [edx], eax

mov esp, ebp
pop ebp  
ret
