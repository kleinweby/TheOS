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

#import "Utils/KObject.h"
#import "Memory/PhyMem.h"

namespace VM {
namespace Backend {
	
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
void Initialize();

class Context : public KObject {
private:
	// Access controll
	int32_t accessCounter;
protected:
	// Default options when mapping
	VMBackendMapOptions defaultOptions;
	
	// Constructor
	Context(VMBackendMapOptions defaultOptions);
	
	//
	// Primitive Methods that have to be implemented
	// by the arch specify subclass.
	// Note: virtual public methods also need to be implemented
	//
	
	// Called by beginAccess/endAccess when needed
	virtual bool makeAccessible() = 0;
	virtual void endAccessible() = 0;
public:
	//
	// Create a new backend context.
	//
	static Ptr<Context> create(VMBackendMapOptions defaultOptions);
	
	//
	// Destructor
	//
	virtual ~Context();
	
	//
	// Maps a given vaddr to paddr.
	//
	// This will fail when...
	//  ... vaddr already is mapped
	//  ... context is kernel context and vaddr < KERNEL_LOAD_ADDRESS
	//  ... context is not the kernel context and vaddr >= KERNEL_LOAD_ADDRESS
	//
	// @param vaddr The virtual address that will be mapped
	// @param paddr The destination physical addr
	// @param options The options for this mapping. (0=default)
	// @returns YES if successfull, otherwise NO
	//
	virtual bool map(page_t paddr, pointer_t vaddr, VMBackendMapOptions options) = 0;
	
	//
	// Unmaps a given vaddr in the context.
	//
	// This will fail when...
	//  ... vaddr already is not mapped
	//  ... context is kernel context and vaddr < KERNEL_LOAD_ADDRESS
	//  ... context is not the kernel context and vaddr >= KERNEL_LOAD_ADDRESS
	//
	// @param vaddr The virtual address that will be mapped
	// @returns YES if successfull, otherwise NO
	//
	virtual bool unmap(pointer_t vaddr) = 0;

	//
	// Translates a vaddr to the paddr in the context.
	//
	// @param vaddr The virtual address that will be translated
	// @returns Returns the page address or kPhyInvalidPage if there is no such mapping.
	//
	virtual page_t translate(pointer_t vaddr) = 0;
	
	//
	// Switches to a this context
	//
	virtual void activate() = 0;
	
	//
	// Begin accessing the context
	//
	// This will perpare the context to be accessed.
	// (Aquire locks, map needed structures etc.)
	//
	// NOTE: Calling this method is purly optional,
	// all methods will call beginAccess when not
	// explictly done by you.
	//
	void beginAccess();
	
	//
	// End access
	// Call this for every time ou called beginAccess()
	//
	void endAccess();
};

//
// Returns the context used by the kernel.
//
// Note: only addresses above KERNEL_LOAD_ADDRESS
// can be mapped in this, and will be avaiable in
// all other contexts.
//
GlobalPtr<Context> GetKernelContext();

}
}
