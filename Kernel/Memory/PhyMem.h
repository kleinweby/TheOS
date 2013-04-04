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

#pragma once

#include <CoreSystem/CommonTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Describes a physical page
//
typedef void* page_t;

static const uint32_t kPhyMemPageSize = 4 * 1024 /* 4 KiB */;
static const page_t kPhyInvalidPage = (void*)0xFFFFFFFF;
static const uint32_t kPhyPageMask = 0xFFFFF000;

//
// Initializes the phy mem subsystem
//
void PhyMemInitialize();

//
// Initializetion routines. Be careful when using those
// ====================================================
//
void _PhyMemMarkFree(page_t page);
void _PhyMemMarkUsed(page_t page);
void _PhyMemMarkUsedRange(page_t address, size_t size);
void _PhyMemMarkFreeRange(page_t address, size_t size);

//
// Print phy mem layout
//
void LogPhyMem();

// Alloc an physical memory page and returns the address of it.
// 
// When the allocation fails the address will be undefined.
// 
// Passing NULL as address pointer will cause a panic.
// 
// @param address Pointer to an pointer_t value that will contain the
//                address of the allocated page.
//                Note: 0x0 is an valid page address too. For error
//                checking use the return value.
// @return Returns true if the allocation succeeded. false otherwise.
// 					
bool PhyMemAlloc(page_t* address);

#ifdef __cplusplus
}
#endif
