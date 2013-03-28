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

#include "Multiboot/Multiboot.h"
#include "Logging/Logging.h"
#include "Memory/PhyMem.h"
#import "Memory/kalloc.h"
#import "VM/VM.h"
#import "Interrupts/Interrupts.h"

#import "KernelInfo.h"
#import "Bootstrap.h"

#include <CoreSystem/MachineInstructions.h>


// This is the initial heap we use until we've
// got a real heap.
//
// Note: Heap is still valid, after boot up
char StartupHeap[4*1024];

extern "C" void KernelInitialize(uint32_t magic, struct Multiboot* header)
{	
	LoggingInitialize();
	
	// This will also make a temporary bootstrap mapping
	MultibootAdjust(header, KERNEL_LOAD_ADDRESS);
	
	LogVerbose("Magic %x, header: %p", magic, header);
	MultibootInitializePhyMem(header, KERNEL_LOAD_ADDRESS);
	_PhyMemMarkUsedRange(KernelOffset, KernelLength);
	_PhyMemMarkUsedRange(KernelBootstrapOffset, KernelBootstrapLength);
	BootstrapPhyMemInitialize();
	LogPhyMem();
	
	KallocInitialize(StartupHeap, sizeof(StartupHeap));
	
	VM::Initialize();
	BootstrapRelease();
	
	InterruptsInitialize();
	
	LogInfo("booted");
	LogPhyMem();


	for(;;) {
		LogInfo("Interrupts %i", interruptCount);
		Halt();
		if (interruptCount > 50) {
			for(uint32_t j = 0; j < kUInt32Max - 1; j++) {
			}

			LogInfo("Done");
		}
	}
}
