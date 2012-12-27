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

#include "Multiboot.h"

#include "Memory/PhyMem.h"
#include "Logging/Logging.h"

void _MultibootAdjust(struct Multiboot* multiboot, offset_t offset)
{
	LogTrace("Adjusting multiboot structure by %x", offset);
	#define ADJUST(a) (a) = OFFSET(a, offset)
	
	ADJUST(multiboot->bootdevice);
	ADJUST(multiboot->cmdline);
	ADJUST(multiboot->mods_addr);
	ADJUST(multiboot->mmap_addr);
	ADJUST(multiboot->drives_addr);
	ADJUST(multiboot->config_table);
	ADJUST(multiboot->boot_loader_name);
	
	struct MultibootModule* module = multiboot->mods_addr;
	for (uint32_t i = 0; i < multiboot->mods_count; i++) {
		ADJUST(module[i].startAddress);
		ADJUST(module[i].endAddress);
		ADJUST(module[i].name);
	}
	
	#undef ADJUST
}

void MultibootInitializePhyMem(struct Multiboot* multiboot)
{
	PhyMemInitialize();
	
	LogTrace("Account available ram reported by multiboot");
	struct MultibootMMapEntry* entry;
	
	entry  = multiboot->mmap_addr;

	while ((uint32_t)entry < (uint32_t)multiboot->mmap_addr + multiboot->mmap_length) {
		if (entry->type == 1) {
			for (pointer_t address = (pointer_t)(uint32_t)entry->base_address; 
			     address < (pointer_t)(uint32_t)(entry->base_address + entry->length);
			     address += kPhyMemPageSize) {
				_PhyMemMarkFree(address);	
			}
		}
		
		entry = (struct MultibootMMapEntry*)((uint32_t)entry + entry->size + 4);
	}
	
	LogTrace("Account phys pages used by multiboot structure");
	_PhyMemMarkUsedRange(multiboot, sizeof(struct Multiboot));
}
