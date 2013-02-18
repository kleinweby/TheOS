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

#import <CoreSystem/CommonTypes.h>

#import "Utils/KObject.h"
#import "VM/PageFault.h"

namespace VM {

class Store;
class Region;

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
	
public:
	//
	// Construct a new layer with a parent
	//
	Layer(Ptr<Layer> parent);
	
	//
	// Construct a new layer with a store
	//
	Layer(Ptr<Store> store);
	
	//
	// Ask the layer to handle a fault in the region at
	// address.
	//
	// Note: mapping should only allow the faulted type.
	// So when a read fault occour, only map the page
	// readonly if it makes sense. For zero filled pages
	// it would make no sense, as we expect writes soon.
	//
	bool handleFault(pointer_t vaddr, FaultType type, Region region);
};

} // namespace VM
