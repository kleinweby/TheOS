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

#pragma once

#include <CoreSystem/CommonTypes.h>

#include "Utils/KObject.h"
#include "VM/Permission.h"

namespace VM {

class Context;
class Layer;

enum class RegionType : unsigned short {
	//
	// On copy the regions underlaying data
	// can be modified by both regions
	// without the other region seeing the changes
	//
	Private,
	//
	// On copy, when one region changes the
	// underlaying data, both regions see these
	// changes.
	//
	Shared
};

class Region : public KObject {

protected:
	Context* context; // The parent context for this region (weak to avoid cycles)
	Ptr<Layer> layer; // The layer providing content for this region
	
	// The offset in the context (start address)
	offset_t offset;
	// The size (same accross contexts)
	size_t size;

	// The type of this region
	RegionType type;
	// The permissions of this region
	Permission permissions;
public:
	//
	// Default constructor
	//
	// @param layer The layer that will provide the content
	// @param offset The offset in the context.
	// @param context The context the region will be added to
	//
	Region(Ptr<Layer> layer, offset_t offset, Permission permissions, Ptr<Context> context);
	
	//
	// Copy constructor
	//
	// @param region Region to copy.
	// @param offset The offset in the context.
	// @param context The context the region will be added to
	//
	Region(Ptr<Region>& region, offset_t offset, Permission permissions, Ptr<Context> context);
	
	//
	// Destructor
	//
	virtual ~Region();
	
	//
	// Set the region type.
	// Obviously this will only affect region copies
	// after the type was set.
	//
	void setType(RegionType type);
	RegionType getType() const;
	
	//
	// Get the offset in the context
	//
	offset_t getOffset() const;
	
	//
	// Get the size in the offset
	//
	size_t getSize() const;
	
	//
	// Get the context
	//
	Ptr<Context> getContext() const;
	
	//
	// Removes the region from the context
	//
	void removeFromContext();
	
	//
	// Handle a page fault at address
	//
	bool handleFault(uint32_t vaddr, Permission permissions);
	
	//
	// You can call this at any time, to cause a fault
	// on the whole region
	//
	bool fault(Permission permissions);
	bool fault();
};

} //namespace VM
