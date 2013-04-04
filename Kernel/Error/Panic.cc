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

#include "LinkerHelper.h"

#include <CoreSystem/MachineInstructions.h>

LINKER_SYMBOL(PanicDrivers, PanicDriver*);
LINKER_SYMBOL(PanicDriversLength, uint32_t);

void panic(const char* message, ...)
{
	Interrupts::CPUState state;
	va_list args;
	va_start(args, message);
	panic_state(message, &state, args);
	va_end(args);
}

void panic_state(const char* message, Interrupts::CPUState* cpuState, va_list args)
{
	// Prevent any futher interrupts from waking up the kernel
	DisableInterrupts();
	
	uint64_t timestamp = TimeStampCounter() >> 24;
	uint32_t count = PanicDriversLength/sizeof(PanicDriver);

	for (uint32_t i = 0; i < count; i++) {
		PanicDrivers[i](timestamp, message, cpuState, args);
	}
	
	// Halt the kernel
	while(true)
		Halt();
}