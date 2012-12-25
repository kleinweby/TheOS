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

//
// Abstract
// =======
//
// This file provides structues uses for multiboo
//

#ifndef _MULTIBOOT_MULTIBOOT_H_
#define _MULTIBOOT_MULTIBOOT_H_

#include "Error/Assert.h"

#include <CoreSystem/CommonTypes.h>

struct Multiboot {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	char* bootdevice; // Seems wrong type
	char* cmdline; // Seems wrong type
	uint32_t mods_count;
	struct MultibootModule* mods_addr;
	uint32_t syms[4];
	uint32_t mmap_length;
	struct MultibootMMapEntry* mmap_addr;
	uint32_t drives_length;
	pointer_t drives_addr;
	pointer_t config_table;
	char* boot_loader_name;
	uint32_t apm_table;
	uint32_t vbe_control_info;
	uint32_t vbe_mode_info;
	uint16_t vbe_mode;
	uint16_t vbe_interface_seg;
	uint16_t vbe_interface_off;
	uint16_t vbe_interface_len;
} __attribute__ ((packed));

struct MultibootMMapEntry {
	uint32_t size;
	uint64_t base_address;
	uint64_t length;
	uint32_t type;
} __attribute__ ((packed));

struct MultibootModule {
	pointer_t startAddress;
	pointer_t endAddress;
	char* name;
	uint32_t __reversed;
} __attribute__ ((packed));

//
// Adjust Multiboot structure for a given offset
//
void _MultibootAdjust(struct Multiboot* multiboot, offset_t offset);
#define MultibootAdjust(multiboot, offset) multiboot = (pointer_t)((uint32_t)multiboot + offset); _MultibootAdjust(multiboot, offset)

//
// Initialize PhyMem from informations of the multiboot structure
//
void MultibootInitializePhyMem(struct Multiboot* multiboot);

#endif // _MULTIBOOT_MULTIBOOT_H_
