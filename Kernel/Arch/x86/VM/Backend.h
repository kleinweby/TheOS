//
// Copyright (c) 2013, Christian Speich
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

#import "Memory/VMBackend.h"
#import "Boot/Bootstrap.h"

namespace VM {
namespace Backend {
namespace X86 {

typedef uint32_t PageDirectoryEntry;

typedef struct {
	PageDirectoryEntry entries[1024];
} PageDirectory;

typedef uint32_t PageTableEntry;

typedef struct {
	PageTableEntry entries[1024];
} PageTable;

void Initialize();

// The Context used for paging on X86
class Context : public VM::Backend::Context {
protected:
	// Base when the page tables can be accessed
	PageTable* pageTablesBase;
	PageDirectory* pageDirectory;
	
	// the page directory (paddr)
	page_t paddrPageDirectory;

	void mapPageDirectory(page_t paddr, pointer_t vaddr);

	// Overrided from abstract superclass
	virtual bool map(page_t paddr, pointer_t vaddr, VMBackendMapOptions options);
	virtual bool unmap(pointer_t vaddr);
	virtual page_t translate(pointer_t vaddr);
	virtual void activate();
	virtual bool makeAccessible();
	virtual void endAccessible();
	
	// Call with false, to not initialize die pd. This
	// is only used in the KernelContext Subclass
	Context(VMBackendMapOptions options, bool initialize);
public:
	Context(VMBackendMapOptions options) : Context(options, true) {}
};

extern GlobalPtr<Context> KernelContext;

} // namespace X86
} // namespace Backend
} // namespace VM
