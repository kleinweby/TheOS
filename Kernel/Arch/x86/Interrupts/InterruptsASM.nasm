;
;  Copyright (c) 2012, Christian Speich
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
;

[BITS 32]

[section .text]

[extern InterruptsIDTHeader]

[global InterruptsLoadIDT]
InterruptsLoadIDT:
	; Load the interrupt descriptor table as described by the header
	lidt [InterruptsIDTHeader]

	; Return to the caller
	ret

[global InterruptsLOADTSS]
InterruptsLOADTSS:
	; Load the index of our TSS structure - The index is
    ; 0x28, as it is the 5th selector and each is 8 bytes
    ; long, but we set the bottom two bits (making 0x2B)
    ; so that it has an RPL of 3, not zero.
	mov ax, 0x2B
	
	; Load the task descriptor
	ltr ax
	
	; Return
	ret

[extern InterruptsHandler]
InterruptsTrampolinCommon:
	; CPU-Zustand sichern
    push ebp
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax
	
	; Call Interrupts Handler with cpu state struct
	push esp
	call InterruptsHandler
	; Interruptshandler returns the state to restore
	mov esp, eax
	
	pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    pop ebp
	; Pop error + interrupt number
	add esp, 8
	iret

%macro TrampolinStub 1
[global InterruptsTrampolin%1]
InterruptsTrampolin%1
	; Push invalid error no
	push dword 0
	; Push Interrupt Number
	push dword %1
	jmp InterruptsTrampolinCommon
%endmacro

%macro TrampolinStubWithErrno 1
[global InterruptsTrampolin%1]
InterruptsTrampolin%1
	; Push Interrupt Number
	push dword %1
	jmp InterruptsTrampolinCommon
%endmacro

; Exceptions
TrampolinStub 0
TrampolinStub 1
TrampolinStub 2
TrampolinStub 3
TrampolinStub 4
TrampolinStub 5
TrampolinStub 6
TrampolinStub 7
TrampolinStubWithErrno 8
TrampolinStub 9
TrampolinStubWithErrno 10
TrampolinStubWithErrno 11
TrampolinStubWithErrno 12
TrampolinStubWithErrno 13
TrampolinStubWithErrno 14
TrampolinStub 15
TrampolinStub 16
TrampolinStubWithErrno 17
TrampolinStub 18
 
; IRQs
TrampolinStub 32
TrampolinStub 33
TrampolinStub 34
TrampolinStub 35
TrampolinStub 36
TrampolinStub 37
TrampolinStub 38
TrampolinStub 39
TrampolinStub 40
TrampolinStub 41
TrampolinStub 42
TrampolinStub 43
TrampolinStub 44
TrampolinStub 45
TrampolinStub 46
TrampolinStub 47

 
; Others: e.g. syscall

