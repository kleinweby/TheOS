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
#include "Utils/Result.h"
#include "VM/Region.h"
#include "Memory/PhyMem.h"

namespace VM {

class Store;

class Layer : public KObject {
	//
	// Lookup for pages not provided by this
	// layer will be:
	// Store -> Parent -> FAIL
	//
	// This way, a non root layer can get a
	// swap storage to page out if needed
	//
	Ptr<Layer> parent;
	Ptr<Store> store;
	
	//
	// Addresses for the phy pages by this
	// layer.
	// TODO: maybe a map would be better here?
	// probbably depends on the ratio covered by
	// this layer.
	//
	page_t* pages;
public:
	///
	/// Construct a new layer with a parent
	///
	Layer(Ptr<Layer> parent);
	
	///
	/// Construct a new layer with a store
	///
	Layer(Ptr<Store> store);
	
	//
	// Destructor
	//
	virtual ~Layer();
	
	///
	/// Ask the layer to handle a fault in the region at
	/// address.
	///
	/// @param permissions the permissions the layer should map
	///
	bool handleFault(uint32_t vaddr, Permission permissions, Ptr<Region> region);
	
	///
	/// Gets the size of this layer
	///
	virtual size_t getSize() const;
	
	///
	/// Get the real size of this layer
	/// This means only the phy pages directly hold by this layer.
	///
	virtual size_t getRealSize() const;
};

} // namespace VM
