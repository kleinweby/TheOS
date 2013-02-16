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

#import "Backend.h"

// Use platform independet parts
using namespace VM::Backend;

namespace VM {
namespace Backend {
namespace X86 {

class KernelContext : public Context {
public:
	KernelContext();
	~KernelContext();
};

GlobalPtr<KernelContext> KernelBackendContext;

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

static inline page_t EntryGetPAddr(uint32_t* entry)
{
	return (page_t)(*entry & 0xFFFFF000);
}

static inline void EntrySetPAddr(uint32_t* entry, page_t page)
{
	// Preserve the options
	*entry = (uint32_t)page | (*entry&0xFFF);
}

static inline VMBackendMapOptions EntryGetOptions(uint32_t* entry)
{
	return *entry & 0xFF;
}

enum {
	VMBackendOptionPresent = (1 << 0)
};

static inline void EntrySetOptions(uint32_t* entry, VMBackendMapOptions options)
{
	// Preserve paddr
	*entry = (*entry & 0xFFFFF000) | options;
}

void Initialize()
{
	KernelBackendContext = new KernelContext();
}

Context::Context(VMBackendMapOptions options, bool initialize) : VM::Backend::Context(options)
{		
	if (!PhyMemAlloc(&this->paddrPageDirectory)) {
		return;
	}
	
	if (initialize) {
		this->beginAccess();
	
		// Clear new page directory
		for (uint32_t i = 0; i < 1024; i++) {
			this->pageDirectory->entries[i] = 0;
		}
	
		// TODO: insert kernel directory
	
		this->endAccess();
	}
}

bool Context::map(page_t paddr, pointer_t vaddr, VMBackendMapOptions options)
{
	assert(vaddr > (pointer_t)0x0); // Don't map 0x0
	assert(paddr != kPhyInvalidPage);
	
	if (options == 0)
		options = this->defaultOptions;
	
	bool success = true;
	
	this->beginAccess();
	
	uint32_t tableIndex = tableIndexFromAddress(vaddr);
	
	// Table is not present, must create it now
	if (EntryGetOptions(&this->pageDirectory->entries[tableIndex]) & VMBackendOptionPresent) {
	}
	
	PageTable* table = OFFSET(this->pageTablesBase, tableIndex * sizeof(PageTable));
	
	PageTableEntry* entry = &table->entries[entryIndexFromAddress(vaddr)];
	
	if (EntryGetOptions(entry) & VMBackendOptionPresent) {
		success = false;
	}
	else {
		EntrySetPAddr(entry, paddr);
		// Always add the present option
		EntrySetOptions(entry, options|VMBackendOptionPresent);
	}
	
	this->endAccess();
	
	return success;
}

bool Context::unmap(pointer_t vaddr)
{
	assert(vaddr > (pointer_t)0x0);
	
	bool success = true;
	
	this->beginAccess();
	
	uint32_t tableIndex = tableIndexFromAddress(vaddr);
	
	// Table is not present,so cannot unmap
	if (EntryGetOptions(&this->pageDirectory->entries[tableIndex]) & VMBackendOptionPresent) {
		success=false;
	}
	else {
		PageTable* table = OFFSET(this->pageTablesBase, tableIndex * sizeof(PageTable));
		
		PageTableEntry* entry = &table->entries[entryIndexFromAddress(vaddr)];
	
		if (!(EntryGetOptions(entry) & VMBackendOptionPresent)) {
			success = false;
		}
		else {
			// Just clear the whole thing, no need to do something
			// bity here, as it doesnt matter as long as the present bit
			// is cleared
			entry = 0;
		}
	}
	
	this->endAccess();
		
	return success;
}

page_t Context::translate(pointer_t vaddr)
{
	assert(vaddr >= 0x0);
		
	page_t page = kPhyInvalidPage;
	
	this->beginAccess();
	
	uint32_t tableIndex = tableIndexFromAddress(vaddr);
	
	if (EntryGetOptions(&this->pageDirectory->entries[tableIndex]) & VMBackendOptionPresent) {
		PageTable* table = OFFSET(this->pageTablesBase, tableIndex * sizeof(PageTable));
		
		if (EntryGetOptions(&table->entries[entryIndexFromAddress(vaddr)]) & VMBackendOptionPresent)
			page = EntryGetPAddr(&table->entries[entryIndexFromAddress(vaddr)]);
	}
	
	this->endAccess();
	
	return page;
}

bool Context::makeAccessible()
{
	// Find a spot to temporarly put our context
	pointer_t base = (pointer_t)0xF0000000;
	for (; base < (pointer_t)0xFFC00000 &&
		 KernelBackendContext->translate(base) != kPhyInvalidPage;
	 	base = OFFSET(base, 0x400000));
				
	assert(base != (pointer_t)0xFFC00000); // Whoops, nothing free?!
			
	KernelBackendContext->mapPageDirectory(this->paddrPageDirectory, base);
		
	// Save the current base
	this->pageTablesBase = (PageTable*)base;
	this->pageDirectory = OFFSET((PageDirectory*)base, 0x3FE000);
	
	return true;
}

void Context::mapPageDirectory(page_t paddr, pointer_t vaddr)
{
	PageDirectoryEntry* entry = &this->pageDirectory->entries[tableIndexFromAddress(vaddr)];
			
	EntrySetPAddr(entry, paddr);
	EntrySetOptions(entry, VMBackendOptionPresent|VMBackendOptionWriteable);
			
	// Now invalidate the entries
	for (pointer_t ptr = vaddr;
	     ptr < OFFSET(vaddr, 0x400000); 
	     ptr = OFFSET(ptr, kPhyMemPageSize)) {
		invalidatePage(ptr);
	}
}

void Context::endAccessible()
{
	// Simply declare non present
	KernelBackendContext->pageDirectory->entries[tableIndexFromAddress(this->pageTablesBase)] = 0;
		
	// Now invalidate the entries
	for (pointer_t ptr = this->pageTablesBase;
	     ptr < OFFSET(this->pageTablesBase, 0x400000); 
	     ptr = OFFSET(ptr, kPhyMemPageSize)) {
		invalidatePage(ptr);
	}
			
	// And clear the base
	this->pageTablesBase = NULL;
	this->pageDirectory = NULL;
}

void Context::activate()
{
	__asm__ __volatile__ (	"mov %0, %%eax\n"
	"mov %%eax, %%cr3\n" :: "m" (this->paddrPageDirectory));
}

KernelContext::KernelContext() : Context(VMBackendOptionGlobal|VMBackendOptionWriteable, false)
{	
	this->pageTablesBase = (PageTable*)0xFFC00000;
	this->pageDirectory = (PageDirectory*)0xFFFFF000;
	
	// temporary to insert context into itself
	BooststrapMap((uint32_t)this->paddrPageDirectory, (uint32_t)this->pageDirectory, 0x1000);
	
	PageDirectory* d = this->pageDirectory;

	// Now we preallocate all tables above KERNEL_LOAD_ADDRESS so that these can be copied
	// into the other contexts but we can keep mapping pages
	for (uint32_t i = (KERNEL_LOAD_ADDRESS >> 22 & 0x03FF);
		 i < 1023; // Not 1024, because the pd is mapped there
		 i++) {
		page_t page;
		assert(PhyMemAlloc(&page));
		
		EntrySetPAddr(&d->entries[i], page);
		EntrySetOptions(&d->entries[i], VMBackendOptionWriteable|VMBackendOptionGlobal|VMBackendOptionPresent);
	}
	
	// Add pd as last entry
	EntrySetPAddr(&d->entries[1023], this->paddrPageDirectory);
	EntrySetOptions(&d->entries[1023], VMBackendOptionWriteable|VMBackendOptionGlobal|VMBackendOptionPresent);
		
	// Now make the page tables accibile
	// Now the pd itself is at 0xFFFFE000, and the other tables can be accessed before
	BoostrapMapPageDirectory((uint32_t)this->paddrPageDirectory, 
							 (uint32_t)this->pageTablesBase);
}

KernelContext::~KernelContext()
{
	panic("Try to destruct kernel context!");
}


} // namespace X86
} // namespace Backend
} // namespace VM
