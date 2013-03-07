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

#import "Layer.h"
#import "Region.h"
#import "Store.h"
#import "PageFault.h"
#import "Backend.h"
#import "Context.h"

namespace VM {

Layer::Layer(Ptr<Layer> _parent)
{
	this->store = NULL;
	this->parent = _parent;
}

Layer::Layer(Ptr<Store> _store)
{
	this->parent = NULL;
	this->store = _store;
}

Layer::~Layer()
{
	
}

bool Layer::handleFault(uint32_t vaddr, RegionPermission permissions, Ptr<Region> region)
{
	Ptr<Backend::Context> backend = region->getContext()->getBackend();
	
	if (this->store) {
		bool reuse = true;
		
		// We need write permissions, but the store does not allow us to write directly
		if ((permissions & RegionPermission::Write) && !(this->store->isWriteable(vaddr)))
			reuse = false;
		
		page_t paddr;
		
		if (reuse)
			paddr = this->store->getPageAddress(vaddr);
		else
			panic("Not implemented");
		
		backend->map(paddr, (pointer_t)(vaddr + region->getOffset()), 0);
	}
	
	if (this->parent) {
		
	}
	return false;
}

size_t Layer::getSize()
{
	if (this->parent) {
		return this->parent->getSize();
	}
	
	return this->store->getSize();
}

size_t Layer::getRealSize()
{
	size_t size = 0x0;
	
	for (size_t i = 0; i < this->getSize()/kPhyMemPageSize; i++) {
		if (this->pages[i] != kPhyInvalidPage)
			size += kPhyMemPageSize;
	}
	
	return size;
}

} // namespace VM
