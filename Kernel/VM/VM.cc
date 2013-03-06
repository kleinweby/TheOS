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

#import "VM.h"
#import "Backend.h"
#import "Context.h"
#import "FixedStore.h"
#import "Layer.h"
#import "Region.h"
#import "KernelInfo.h"
#import "Logging/Logging.h"

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
	region = new Region(layer, KERNEL_LOAD_ADDRESS, KernelContext);
	// We need to fault this manually, as the fault handling code would not be present
	region->fireFault(FaultType::Write);
	region->fireFault(FaultType::Execute);
}

} // namespace VM
