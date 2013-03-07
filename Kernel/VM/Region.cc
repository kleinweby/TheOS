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

#import "Region.h"
#import "Context.h"
#import "Layer.h"

namespace VM {

// Default constructor
Region::Region(Ptr<Layer> _layer, offset_t _offset, Ptr<Context> _context)
{
	this->type = RegionType::Private;
	this->layer = _layer;
	this->offset = _offset;
	this->context = _context;
	this->size = _layer->getSize();
	
	// We can not call this here, until we get retained by the context
	//this->context->addRegion(this);
}
	
// Copy constructor
Region::Region(Ptr<Region>& _region, offset_t _offset, Ptr<Context> _context)
{	
	this->offset = _offset;
	this->context = _context;
	this->size = _region->size;
	this->type = _region->type;
	
	// We can not call this here, until we get retained by the context
	//this->context->addRegion(this);
}

Region::~Region()
{
}

void Region::setType(RegionType _type)
{
	this->type = _type;
}

RegionType Region::getType() const
{
	return this->type;
}

offset_t Region::getOffset() const
{
	return this->offset;
}

size_t Region::getSize() const
{
	return this->size;
}

Ptr<Context> Region::getContext() const
{
	return this->context;
}

void Region::removeFromContext()
{
	this->context->removeRegion(this);
}

bool Region::handleFault(uint32_t vaddr, RegionPermission _permissions)
{
	assert(this->offset <= vaddr && vaddr < this->offset+this->size);
	// TODO: check permissions

	return this->layer->handleFault(vaddr - this->offset, _permissions, this);
}

bool Region::fault(RegionPermission _permissions)
{
	bool success = true;

	for (uint32_t vaddr = this->offset; vaddr < this->offset+this->size; vaddr += kPhyMemPageSize) {
		if (!this->handleFault(vaddr, _permissions))
			success = false;
	}
	
	return success;
}

} // namespace VM
