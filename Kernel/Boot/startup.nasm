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

; We are here in an pre paging, pre gdt stage
; we are located at 0x1000 and we need to set up
; gdt to map 0xC0000000 to 0x0. (Higher half kernel)
startup:
	; Load a fake gdt which will add 0x40000000 to all
	; addresses; this will result in it ending up
	; at 0x0.
	lgdt [fake_gdt_header]
	; Mak this working
	mov cx, 0x10
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx

	; Jumpt to the higher half of the kernel
	jmp 0x08:_startup_higherhalf

fake_gdt_header:
	; Size of the fake gdt
	dw fake_gdt_end - fake_gdt - 1
	; Phys address of the fake gdt
	dd fake_gdt

fake_gdt:
	; 0 is mapped to 0
	dd 0, 0
	; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40
	; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92, granularity 0xCF
	db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40
fake_gdt_end:

[section .text]
; We are now in the position to resolve 0xC0000000 correctly

[extern KernelInitialize]

_startup_higherhalf:
	; Setup the kernel stack
	mov esp, kernel_stack
	
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

[global enable_gdt_identity]
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

