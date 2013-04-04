//
// Copyright (c) 2012, Christian Speich
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "Panic.h"

extern "C" {
#include <CoreSystem/MachineInstructions.h>
#include <CoreSystem/String.h>
}

static void SerialWrite(uint16_t base, char chr) {
  while ((inb(base+5)&0x20)==0);
  outb(base,(uint8_t)chr);
}

static void SerialPutchar(char chr) {
	SerialWrite(0x3F8, chr);
}

void PanicDriverSerial(uint64_t timestamp, const char* message, Interrupts::CPUState* cpuState, va_list args)
{	
	pprintf(SerialPutchar, "\033[0;37m[%10d]\033[1;31m[F] Panic\033[0m\n", (uint32_t)timestamp);
	pprintf(SerialPutchar, "Message:");
	vpprintf(SerialPutchar, message, args);
	
	if (cpuState) {
		pprintf(SerialPutchar, "CPU State:\n");
		pprintf(SerialPutchar, "    eax =  %08x\n", cpuState->eax);
		pprintf(SerialPutchar, "    ebx =  %08x\n", cpuState->ebx);
		pprintf(SerialPutchar, "    ecx =  %08x\n", cpuState->ecx);
		pprintf(SerialPutchar, "    edx =  %08x\n", cpuState->edx);
		pprintf(SerialPutchar, "    ebp =  %08x\n", cpuState->ebp);
		pprintf(SerialPutchar, "    esi =  %08x\n", cpuState->esi);
		pprintf(SerialPutchar, "    edi =  %08x\n", cpuState->edi);
		pprintf(SerialPutchar, "    eip = %p\n", cpuState->eip);
		pprintf(SerialPutchar, "     cs =  %08x\n", cpuState->cs);
		pprintf(SerialPutchar, " eflags =  %08x\n", cpuState->eflags);
		pprintf(SerialPutchar, "    esp =  %08x\n", cpuState->esp);
		pprintf(SerialPutchar, "    ss  =  %08x\n\n", cpuState->ss);
		
		pprintf(SerialPutchar, "Backtrace:\n");
	}
}

PanicRegisterDriver(PanicDriverSerial);
