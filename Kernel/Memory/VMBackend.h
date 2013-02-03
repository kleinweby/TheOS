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

#import <CoreSystem/CommonTypes.h>

#import "Utils/Object.h"
#import "Memory/PhyMem.h"

DECLARE_CLASS(VMBackendContext);

//
// Options for a mapped page
//
typedef uint32_t VMBackendMapOptions;
enum {
	VMBackendOptionWriteable = (1 << 1),
	VMBackendOptionUser = (1 << 2),
	VMBackendOptionWriteThrough = (1 << 3),
	VMBackendOptionCacheDisabled = (1 << 4),
	// The options can only be queried.
	VMBackendOptionWasAccessed = (1 << 5),
	// The options can only be queried.
	VMBackendOptionDirty = (1 << 6),
	// TODO: page size
	// Only usable when specifing default options
	VMBackendOptionGlobal = (1 << 8)
};

//
// Init the vm backend.
//
// This will create all page tables possibly
// used by the kernel and a bare context to use during
// kernel initialization. (To be percise it will only
// be used until any other context was activated).
//
// This will generate an idenity mapping for the heap
// and the kernel binary. Anything else you need to map
// afterwards.
//
// The vaddrs 0xF0000000-0xFFFFFFFF are reserved for use
// within the vmbackend
//
void VMBackendInitialize();

//
// Returns the context used by the kernel.
//
// Note: only addresses above KERNEL_LOAD_ADDRESS
// can be mapped in this, and will be avaiable in
// all other contexts.
//
VMBackendContext VMBackendGetKernelContext();

//
// Creates a new backend context.
//
// @param defaultOptions Default options when options=0 in VMBackendContextMap
// @returns Returns the new context or NULL if an error occour.
//
OBJECT_RETURNS_RETAINED
VMBackendContext VMBackendContextCreate(VMBackendMapOptions defaultOptions);

//
// Maps a given vaddr to paddr in the context specified.
//
// This will fail when...
//  ... vaddr already is mapped
//  ... context is kernel context and vaddr < KERNEL_LOAD_ADDRESS
//  ... context is not the kernel context and vaddr >= KERNEL_LOAD_ADDRESS
//
// @param context The backend context to operate on
// @param vaddr The virtual address that will be mapped
// @param paddr The destination physical addr
// @param options The options for this mapping. (0=default)
// @returns YES if successfull, otherwise NO
//
bool VMBackendContextMap(VMBackendContext context, page_t paddr, pointer_t vaddr, VMBackendMapOptions options);

//
// Unmaps a given vaddr in the context.
//
// This will fail when...
//  ... vaddr already is not mapped
//  ... context is kernel context and vaddr < KERNEL_LOAD_ADDRESS
//  ... context is not the kernel context and vaddr >= KERNEL_LOAD_ADDRESS
//
// @param context The backend context to operate on
// @param vaddr The virtual address that will be mapped
// @returns YES if successfull, otherwise NO
//
bool VMBackendContextUnmap(VMBackendContext context, pointer_t vaddr);

//
// Translates a vaddr to the paddr in the context.
//
// @param context The backend context to operate on
// @param vaddr The virtual address that will be translated
// @returns Returns the page address or kPhyInvalidPage if there is no such mapping.
//
page_t VMBackendContextTranslate(VMBackendContext context, pointer_t vaddr);
