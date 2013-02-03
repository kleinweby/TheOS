;  Copyright (c) 2011-2013, Christian Speich
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
[section .bootstrap]

[extern Bootstrap]
[extern BootstrapPageDirectory]

[global BootstrapASMEntry]
BootstrapASMEntry:
	; setup temporary stack
	mov esp, BootstrapStack
	; Save mb magic and header
	push ebx
	push eax

	; Setup the temporary mapping
	call Bootstrap
	
	; Now activate paging
	mov eax, [BootstrapPageDirectory]
	mov cr3, eax
	mov eax, cr0
	or  eax, 0x80000000
	mov cr0, eax
	
	; Restore mb magic and header
	pop eax
	pop ebx
	
	; Done with the lowlevel bootstrap
	jmp BootstrapASMHighEntry

; Stack used during bootstrap
times 0x400 db 0x0
BootstrapStack:

[section .text]

[extern KernelInitialize]

BootstrapASMHighEntry:
	; Setup real stack
	mov esp, KernelStack
	
	; Load gdt
	; This btw. don't use the phys address
	lgdt [GDTHeader]
	
	mov cx, 0x10
	mov ds, cx
	mov es, cx
	mov fs, cx
	mov gs, cx
	mov ss, cx
	
	; Flush the change
	jmp 0x08:.flush
.flush:

	; Push the multiboot header pointer onto the stack
	; so that the KernelInitialize function get it as
	; argument
	push ebx
	push eax

	; Jump to the kernel written in c
	call KernelInitialize
	; When the c-part returns we just loop
	; and halt the cpu
.loop:
	hlt
	jmp .loop

[section .rodata]
GDTHeader:
	; Size of the gdt
	dw GDTEnd - GDT - 1
	; Phys address of the gdt
	dd GDT
GDT:
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
GDTEnd:

[section .bss]
; Our real kernel stack (16kb)
resb 0x4000
KernelStack:
