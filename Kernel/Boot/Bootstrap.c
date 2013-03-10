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

#import "Bootstrap.h"
#import "Memory/PhyMem.h"
#import "Logging/Logging.h"

uint32_t* BootstrapPageDirectory BOOTSTRAP_SECTION;
void Bootstrap() BOOTSTRAP_SECTION;
uint32_t BootstrapAllocNextPage BOOTSTRAP_SECTION;
uint32_t BootstrapAllocBeginPage BOOTSTRAP_SECTION;
uint32_t BootstrapAllocPage() BOOTSTRAP_SECTION;

uint32_t BootstrapAllocFirstPage;
uint32_t BootstrapAllocLastPage;

extern void _KernelBootstrapOffset;
extern void _KernelBootstrapLength;
extern void _KernelOffset;
extern void _KernelLength;

void Bootstrap()
{
	BootstrapAllocNextPage = (uint32_t)&_KernelOffset + (uint32_t)&_KernelLength;
	
	// Advance to next page
	if (BootstrapAllocNextPage & 0xFFF)
		BootstrapAllocNextPage = (BootstrapAllocNextPage+0x1000)&0xFFFFF000;
	
	BootstrapAllocBeginPage = BootstrapAllocNextPage;
	BootstrapPageDirectory = (uint32_t*)(BootstrapAllocPage());
	
	// Identity map pd
	BooststrapMap((uint32_t)BootstrapPageDirectory,
				  (uint32_t)BootstrapPageDirectory,
				  0x1000);
	// Identity map bootstrap
	BooststrapMap((uint32_t)&_KernelBootstrapOffset,
				  (uint32_t)&_KernelBootstrapOffset,
				  (uint32_t)&_KernelBootstrapLength);
	// Map higher half
	BooststrapMap((uint32_t)&_KernelOffset,
				  (uint32_t)&_KernelOffset + KERNEL_LOAD_ADDRESS,
				  (uint32_t)&_KernelLength);
				  	
	// Map somthing where the kernel context later will be
	// so that the needed table gets allocated which we can not do
	// later
	BooststrapMap((uint32_t)BootstrapPageDirectory, 0xFFFFF000, 0x1000);
}

void BooststrapMap(uint32_t paddr, uint32_t vaddr, uint32_t _size)
{
	// Sanitise paddr, vaddr and size
	paddr = paddr & 0xFFFFF000;
	vaddr = vaddr & 0xFFFFF000;
	
	uint32_t tableIndex = (vaddr >> 22) & 0x3FF;
	uint32_t entryIndex = (vaddr >> 12) & 0x3FF;
	int32_t size = (int32_t)_size;
	
	for (; tableIndex < 1024 && size > 0; tableIndex++) {
		// Add table when needed
		if (!(BootstrapPageDirectory[tableIndex] & (1 << 0))) {
			uint32_t tableAddr = BootstrapAllocPage();
			
			BootstrapPageDirectory[tableIndex] = tableAddr | (1<<1) | (1 << 0);
			
			// Also identity map the table
			BooststrapMap(tableAddr, tableAddr, 0x1000);
		}
		
		uint32_t* table = (uint32_t*)(BootstrapPageDirectory[tableIndex] & 0xFFFFF000);
		
		for (; entryIndex < 1024 && size > 0; entryIndex++) {
			// paddr | writable | present
			table[entryIndex] = paddr | (1 << 1) | (1 << 0);
			
			size -= 0x1000;
			paddr += 0x1000;
			vaddr += 0x1000;
		}
	}
}

uint32_t BootstrapAllocPage()
{
	uint32_t page = BootstrapAllocNextPage;
	BootstrapAllocNextPage += 0x1000;
	
	for (uint32_t* p = (uint32_t*)page; p < (uint32_t*)page + 0x1000; p++)
		*p = 0;
	
	return page;
}

void BootstrapPhyMemInitialize()
{
	BootstrapAllocFirstPage = BootstrapAllocBeginPage;
	BootstrapAllocLastPage = BootstrapAllocNextPage;
	
	_PhyMemMarkUsedRange((page_t)BootstrapAllocFirstPage, BootstrapAllocLastPage-BootstrapAllocFirstPage);
}

void BoostrapMapPageDirectory(uint32_t paddr, uint32_t vaddr)
{
	uint32_t tableIndex = (vaddr >> 22) & 0x3FF;
	
	BootstrapPageDirectory[tableIndex] = paddr | (1<<1) | (1 << 0);
}

void BootstrapRelease()
{
	_PhyMemMarkFreeRange((page_t)BootstrapAllocFirstPage, BootstrapAllocLastPage-BootstrapAllocFirstPage);
}
