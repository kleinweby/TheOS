;  Copyright (c) 2011-2012, Christian Speich
;  All rights reserved.
;
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
;      * Redistributions of source code must retain the above copyright
;        notice, this list of conditions and the following disclaimer.
;      * Redistributions in binary form must reproduce the above copyright
;        notice, this list of conditions and the following disclaimer in the
;        documentation and/or other materials provided with the distribution.
;
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
;  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
;  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
;  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
;  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
;  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
;  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

[BITS 32]

[section .startup]
[global startup]

[extern _KernelStartupOffset]
[extern _KernelStartupLength]
[extern _KernelOffset]
[extern _KernelLength]
[extern MapContinues]

; We are here in an pre paging, pre gdt stage
; we are located at 0x1000 and we need to set up
; gdt to map 0xC0000000 to 0x0. (Higher half kernel)
startup:
	; Use a little stack here
	mov esp, startup_stack
	and	esp, 0xFFFFFFF0
	; Save away the mb magic and header pointer
	push ebx
	push eax
	
	; Load page directory into eax
	call alloc_page
	mov edi, eax
	
	;
	; Now we have two mapping phases
	;  1 - map _KernelStartupOffset:_KernelStartupLength begining to _KernelStartupOffset
	;  2 - map _KernelOffset:_KernelLength to _KernelOffset+0xC0000000
	;
	push dword _KernelStartupLength
	push dword _KernelStartupOffset
	push dword _KernelStartupOffset
	push edi
	call MapContinues
	add esp, 4*4
	
	push dword _KernelLength
	mov eax, _KernelOffset
	add eax, 0xC0000000
	push eax
	push dword _KernelOffset
	push edi
	call MapContinues
	add esp, 4*4

	; Activate paging
	mov eax, edi
	mov cr3, eax
	mov eax, cr0
	or  eax, 0x80000000
	mov cr0, eax

	; Jumpt to the higher half of the kernel
	jmp _startup_higherhalf

; Parameters:
;  none
;
; Return:
;  eax address of new page
;
[global alloc_page]
alloc_page:
	; We use a simple and broken algorithm for now.
	; We remeber a pointer after _KernelOffset+_KernelLength
	; and just allocate behind that
	
	mov eax, [alloc_page_ptr]
	; When alloc page ptr is 0x0
	; we first have to initalize it with _KernelOffset+_KernelLength
	test eax, eax
	jnz .alloc
	mov eax, _KernelOffset
	add eax, _KernelLength
	add eax, 0x1000
	and eax, 0xFFFFF000
.alloc:
	; Now in eax is our new page
	; we now set alloc_page_tr to the new page
	add eax, 0x1000
	mov [alloc_page_ptr], eax
		
	push ecx
	mov ecx, eax
	sub eax, 0x1000
.zero_loop
	sub ecx, 4
	mov dword [ecx], 0
	cmp ecx, eax
	jg .zero_loop
	
	pop ecx

	; Return eax
	ret
; Our current allocated page pointer
alloc_page_ptr dw 0x0

; Reserve space for the page_directory used during
; startup

times 0x400 db 0x0
startup_stack:

[section .text]
; We are now in the position to resolve 0xC0000000 correctly

[extern KernelInitialize]

_startup_higherhalf:
	; Setup the kernel stack
	mov esp, kernel_stack
	
	; enable gdt
	call enable_gdt_identity
	
	; Push the multiboot header pointer onto the stack
	; so that the kernel_initialize function get it as
	; argument
	push ebx
	push eax

	; Jump to the kernel written in c
	call KernelInitialize
	; When the c-part returns we just loop
	; and halt the cpu
	hlt_loop:
	hlt
	jmp hlt_loop

enable_gdt_identity:
	; Load identity gdt
	; This btw. don't use the phys address
	lgdt [identity_gdt_header]
	
	mov cx, 0x10
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx
	
	; Flush the change
	jmp 0x08:flush
flush:
	; Return to the caller
	ret
	
identity_gdt_header:
	; Size of the fake gdt
	dw identity_gdt_end - identity_gdt - 1
	; Phys address of the fake gdt
	dd identity_gdt

[global identity_gdt]
[global identity_gdt_end]

identity_gdt:
	; 0 is mapped to 0
	dd 0, 0

; RING 0
	; code selector 0x08: base 0x0, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x0
	; data selector 0x10: base 0x0, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x0

; RING 3
	; code selector 0x08: base 0x0, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 11111010b, 11001111b, 0x0
	; data selector 0x10: base 0x0, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
	db 0xFF, 0xFF, 0, 0 , 0, 11110010b, 11001111b, 0x0

; Task Descriptor
; Set by code
[global GDTTaskDescriptor]
GDTTaskDescriptor:
	dd 0, 0
identity_gdt_end:

; Make some space for our kernel stack
[section .bss]
[global kernel_stack_start]
kernel_stack_start:
; Reserve 16kb
resb 0x4000
; Point the stack pointer after the bytes
; because the stack will grow downwards
[global kernel_stack]
kernel_stack:

