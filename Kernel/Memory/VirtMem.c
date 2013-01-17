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

#include "VirtMem.h"

#import "Logging/Logging.h"
#import "Memory/PhyMem.h"
#import "KernelInfo.h"
#import <CoreSystem/CommonTypes.h>

typedef struct {
	/**
	  Set isPresent to 1 when the page table is present and
	  in memory.
	 */
	uint32_t isPresent:1;
	/**
	  Set isWriteable to 1 when the memory of the page table mapps
	  is writeable.
	 */
	uint32_t isWriteable:1;
	// The page directory is accecable by the userland
	uint32_t user:1;
	uint32_t writeThrough:1;
	uint32_t cacheDisabled:1;
	uint32_t wasAccessed:1;
	uint32_t __unused:1;
	uint32_t pageSize:1;
	uint32_t __unused2:1;
	
	uint32_t userData:3;
	uint32_t pageTableAddress:20;
} __attribute__ ((packed)) PageDirectoryEntry;

union PageDirectory {
	PageDirectoryEntry entries[1024];
	uint32_t rawEntries[1024];
};

typedef struct {
	// The page table is present in memory
	uint32_t present:1;
	// The page table is writeable
	uint32_t writeable:1;
	// The page table is accecable by the userland
	uint32_t user:1;
	uint32_t writeThrough:1;
	uint32_t cacheDisabled:1;
	uint32_t accessed:1;
	uint32_t dirty:1;
	uint32_t __unused:1;
	uint32_t global:1;
	
	uint32_t userData:3;
	uint32_t physicalAddress:20;
} __attribute__ ((packed)) PageTableEntry;

typedef union {
	PageTableEntry entries[1024];
	uint32_t rawEntries[1024];
} PageTable;

static inline uint32_t tableIndexFromAddress(pointer_t address)
{
	return ((uint32_t)address >> 22 & 0x03FF);
}

static inline uint16_t entryIndexFromAddress(pointer_t address)
{
	return ((uint32_t)address >> 12 & 0x03FF);
}

static inline void invalidatePage(pointer_t address)
{
	__asm__ __volatile__ ("invlpg %0"::"m" (address));
}

struct {
	// The index the following entries should start
	uint32_t firstIndex;
	// Entries of the kernel, and mapped in every
	// process
	PageTable* tables[1024-(KERNEL_LOAD_ADDRESS >> 22 & 0x03FF)];
} KernelDirectory = {.firstIndex =  (KERNEL_LOAD_ADDRESS >> 22 & 0x03FF)};

void VirtMemInitialize()
{
	// We need 1024-firstIndex tables allocated now, to
	// alway insert them
	// TODO: this is quite memory intensive, maybe we should figure out a way to
	// lessen that. But thisway we avoid, changing all processes, when we
	// ad a directory entry afterwards
	for (uint32_t i = KernelDirectory.firstIndex; i < 1024; i++) {
		PhyMemAlloc((pointer_t*)&KernelDirectory.tables[i - KernelDirectory.firstIndex]);
		//memset(KernelDirectory.tables[i - KernelDirectory.firstIndex], 0, sizeof(PageTable));
	}
	LogVerbose("Allocated %d kernel page directories, starting at %d. (%dkib)", sizeof(KernelDirectory.tables)/sizeof(PageTable*), KernelDirectory.firstIndex, sizeof(KernelDirectory.tables)/sizeof(PageTable*) * sizeof(PageTable)/1024);
	
	LogVerbose("Create initial mapping...");
	
}
