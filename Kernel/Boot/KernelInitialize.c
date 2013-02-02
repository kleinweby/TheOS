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
#import "Memory/VirtMem.h"

#import "KernelInfo.h"

// This is the initial heap we use until we've
// got a real heap.
//
// Note: Heap is still valid, after boot up
char StartupHeap[4*1024];

void KernelInitialize(uint32_t magic, struct Multiboot* header)
{	
	LoggingInitialize();
	
	MultibootAdjust(header, KERNEL_LOAD_ADDRESS);
	
	LogVerbose("Magic %x, header: %p", magic, header);
	MultibootInitializePhyMem(header, KERNEL_LOAD_ADDRESS);
	_PhyMemMarkUsedRange(KernelOffset, KernelLength);
	LogPhyMem();
	
	KallocInitialize(StartupHeap, sizeof(StartupHeap));
	
	VirtMemInitialize();
}

//
// Map a continues space in page directory
// used during startup
//
extern uint32_t alloc_page() __attribute__ ((section (".startup")));
void MapContinues(uint32_t* pageDirectory, uint32_t paddr, uint32_t vaddr, uint32_t size) __attribute__ ((section (".startup")));
void MapContinues(uint32_t* pageDirectory, uint32_t paddr, uint32_t vaddr, uint32_t size)
{
	// Sanitise paddr, vaddr and size
	paddr = paddr & 0xFFFFF000;
	vaddr = vaddr & 0xFFFFF000;
	size = (size + 0x1000) & 0xFFFFF000;
	
	uint32_t tableIndex = (vaddr >> 22) & 0x3FF;
	uint32_t entryIndex = (vaddr >> 12) & 0x3FF;
	
	for (; tableIndex < 1024 && size > 0; tableIndex++) {
		// Add table when needed
		if (!(pageDirectory[tableIndex] & (1 << 0))) {
			uint32_t tableAddr = alloc_page();
			
			pageDirectory[tableIndex] = tableAddr | (1<<1) | (1 << 0);
		}
		
		uint32_t* table = (uint32_t*)(pageDirectory[tableIndex] & 0xFFFFF000);
		
		for (; entryIndex < 1024 && size > 0; entryIndex++) {
			// paddr | writable | present
			table[entryIndex] = paddr | (1 << 1) | (1 << 0);
			
			size -= 0x1000;
			paddr += 0x1000;
			vaddr += 0x1000;
		}
	}
}
