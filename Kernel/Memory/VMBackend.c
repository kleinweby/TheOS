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

#import "VMBackend.h"

#import "Logging/Logging.h"
#import "Memory/PhyMem.h"
#import "Memory/Kalloc.h"
#import "Error/Assert.h"
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

typedef union {
	PageDirectoryEntry entries[1024];
	uint32_t rawEntries[1024];
} PageDirectory;

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

DEFINE_CLASS(VMBackendContext, Object,
	VMBackendMapOptions defaultOptions;

	// Pointer to the page directory (vaddr)
	PageDirectory* pageDirectory;

	// Access controll
	int32_t accessCounter;
	// Base when the page tables can be accessed
	PageTable* pageTablesBase;
);

static void VMBackendContextDealloc(void* object);

// These ensure that the context can be accesses (e.g. the tables are mapped)
static void VMBackendContextBeginAccess(VMBackendContext context);
static void VMBackendContextEndAccess(VMBackendContext context);

//
// The Kernel Context
// ==================
//

// We staticly allocate this in the binary cause we always
// need this, so avoid the hassle with dynamic allocation.
// We'll jsut grap the physical pages that back the tables
// later, but this is easy
struct {
	// The index the following entries should start
	uint32_t firstIndex;
	// Entries of the kernel, and mapped in every
	// process
	PageTable* tables[1024-(KERNEL_LOAD_ADDRESS >> 22 & 0x03FF)];
} KernelDirectory = {.firstIndex =  (KERNEL_LOAD_ADDRESS >> 22 & 0x03FF)};

static VMBackendContext VMKernelBackendContext;
static void VMBackendCreateKernelContext();

//
// Initialization
// ==============
//

void VMBackendInitialize()
{
	VMBackendCreateKernelContext();
	LogVerbose("Create initial mapping...");
}

//
// The Kernel Context
// ==================
//
VMBackendContext VMBackendGetKernelContext()
{
	return VMKernelBackendContext;
}

static void VMBackendCreateKernelContext()
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
}

//
// Backend Contexts
// ================
//

VMBackendContext VMBackendContextCreate(VMBackendMapOptions defaultOptions)
{
	VMBackendContext context = kalloc(sizeof(struct _VMBackendContext));
	
	if (context == NULL)
		return NULL;
	
	if (!ObjectInit(context, VMBackendContextDealloc)) {
		free(context);
		return NULL;
	}
	
	context->defaultOptions = defaultOptions;
	
	return context;
}

void VMBackendContextDealloc(void* _context)
{
	VMBackendContext context = _context;
	
	#pragma unused(context)
}

bool VMBackendContextMap(VMBackendContext context, pointer_t vaddr, page_t paddr, VMBackendMapOptions options)
{
	assert(context != NULL);
	assert(vaddr > (pointer_t)0x0); // Don't map 0x0
	
	if (options == 0)
		options = context->defaultOptions;
	
	#pragma unused(paddr)
	
	return true;
}

bool VMBackendContextUnmap(VMBackendContext context, pointer_t vaddr)
{
	assert(context != NULL);
	assert(vaddr > (pointer_t)0x0);
	
	return true;
}

page_t VMBackendContextTranslate(VMBackendContext context, pointer_t vaddr)
{
	assert(context != NULL);
	
	page_t page = kPhyInvalidPage;
	
	VMBackendContextBeginAccess(context);
	
	uint32_t tableIndex = tableIndexFromAddress(vaddr);
	
	if (context->pageDirectory->entries[tableIndex].isPresent) {
		PageTable* table = OFFSET(context->pageTablesBase, tableIndex * sizeof(PageTable));
		
		if (table->entries[entryIndexFromAddress(vaddr)].present)
			page = (page_t)(table->entries[entryIndexFromAddress(vaddr)].physicalAddress << 12);
	}
	
	VMBackendContextEndAccess(context);
	
	return page;
}

static void VMBackendContextBeginAccess(VMBackendContext context)
{
	int32_t accessCounter;
		
	accessCounter = __sync_add_and_fetch(&context->accessCounter, 1);
	
	// It had to be >=0 before, so now it must be >= 1
	assert(accessCounter >= 1);
	
	// We're the first, so make it accible
	if (accessCounter == 1) {
		if (context == VMKernelBackendContext) {
			// Page directory will be mapped as last
			// page table.
			context->pageTablesBase = (PageTable*)0xFFC00000;
		}
		else {
			// Find a spot to temporarly put our context
			pointer_t base = (pointer_t)0xF0000000;
			for (; base < (pointer_t)0xFFC00000 &&
				 VMBackendContextTranslate(VMKernelBackendContext, base) != kPhyInvalidPage;
			 	base = OFFSET(base, 0x400000));
				
			assert(base != (pointer_t)0xFFC00000); // Whoops, nothing free?!
			
			// Map the kernel directory here.
			PageDirectoryEntry* entry = &VMKernelBackendContext->pageDirectory->entries[tableIndexFromAddress(base)];
			
			entry->isWriteable = 1;
			entry->user = 0;
			entry->pageSize = 0;
			entry->pageTableAddress = (uint32_t)(VMBackendContextTranslate(VMKernelBackendContext,
			                          context->pageDirectory)) >> 12;
			entry->isPresent = 1;
			
			// Now invalidate the entries
			for (pointer_t ptr = base;
			     ptr < OFFSET(base, 0x400000); 
			     ptr = OFFSET(ptr, kPhyMemPageSize)) {
				invalidatePage(ptr);
			}
			
			// Save the current base
			context->pageTablesBase = base;
		}
	}
}

static void VMBackendContextEndAccess(VMBackendContext context)
{
	int32_t accessCounter;
		
	accessCounter = __sync_sub_and_fetch(&context->accessCounter, 1);
	
	// It had to be >=1 before, so now it must be >=0
	assert(accessCounter >= 0);
	
	// We're the first, so make it accible
	if (accessCounter == 0) {
		if (context == VMKernelBackendContext) {
			// Just clear the base
			context->pageTablesBase = NULL;
		}
		else {
			// Simply declare non present
			VMKernelBackendContext->pageDirectory->entries[tableIndexFromAddress(context->pageTablesBase)].isPresent = 0;
			
			// Now invalidate the entries
			for (pointer_t ptr = context->pageTablesBase;
			     ptr < OFFSET(context->pageTablesBase, 0x400000); 
			     ptr = OFFSET(ptr, kPhyMemPageSize)) {
				invalidatePage(ptr);
			}
			
			// And clear the base
			context->pageTablesBase = NULL;
		}
	}
}
