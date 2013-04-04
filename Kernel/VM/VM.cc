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

#include "VM.h"
#include "VM/Backend.h"
#include "Context.h"
#include "FixedStore.h"
#include "Layer.h"
#include "Region.h"
#include "KernelInfo.h"
#include "Logging/Logging.h"

namespace VM {

GlobalPtr<Context> KernelContext;

void SetupKernelContext();

void Initialize()
{
	Backend::Initialize();
	
	SetupKernelContext();
}

void SetupKernelContext()
{
	KernelContext = new Context(Backend::GetKernelContext());
	Ptr<Layer> layer;
	Ptr<Region> region;
	
	// TODO: we should make different regions, for text, data, rodata etc.
	// Create a layer with the parts the bootloader loaded for us
	layer = new Layer(new FixedStore(KernelOffset, KernelLength/kPhyMemPageSize));
	// And create a region in the kernel context
	region = new Region(layer, (offset_t)KernelOffset + KERNEL_LOAD_ADDRESS, Permission::Read | Permission::Write | Permission::Execute, KernelContext);
	// We need to fault this manually, as the fault handling code would not be present
	region->fault();

	// VGA
	layer = new Layer(new FixedStore((page_t)0xB8000, 16*1024));
	region = new Region(layer, 0xC00B8000, Permission::Read | Permission::Write, KernelContext);
	region->fault();

	ActivateContext(KernelContext);
}

void ActivateContext(Ptr<Context> context)
{
	context->getBackend()->activate();
}

} // namespace VM
