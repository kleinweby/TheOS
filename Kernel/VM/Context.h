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
#include "VM/Backend.h"
#include "Utils/Dictionary.h"

namespace VM {

class Region;

class Context : public KObject {
	// The backend we use to do the actuall mapping
	Ptr<Backend::Context> backend;
	
	Dictionary<uint32_t, Ptr<Region>> regions;

protected:
	void addRegion(Ptr<Region> region);
	void removeRegion(Ptr<Region> region);

	friend class Region;
public:
	///
	/// @brief Create a new context with a private backend
	///
	Context();
	
	///
	/// Create a new context with a given backend
	///
	Context(Ptr<Backend::Context> backend);
	
	///
	/// Copy a context.
	///
	Context(Ptr<Context>& context);
	
	//
	// Destructor
	//
	virtual ~Context();
	
	///
	/// Get the backend used to map.
	///
	Ptr<Backend::Context> getBackend() const;
};
	
} // namespace VM
