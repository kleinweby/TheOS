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
#include "Memory/PhyMem.h"

namespace VM {

class Store : public KObject {
protected:
	size_t size;
public:
	///
	/// Creates a new store with the size.
	/// If the object this store represents does not
	/// fill the complete size, the remainder will
	/// be null filled.
	///
	Store(size_t size);
	virtual ~Store();
	
	///
	/// Get the size of this store
	///
	size_t getSize() const;
	
	///
	/// Checks wheter a page at vaddr is writable
	/// This way a private copy does not need to be make
	/// and the page of this store can be reused. 
	///
	/// @note areas where it is not allowed to write
	/// (due to permissons) can still be writeable at store
	/// level.
	/// The read onlyness will be enforced in VM::Region 
	///
	virtual bool isWriteable(uint32_t vaddr) const = 0;
	
	///
	/// Get the paddr for the page at address
	///
	/// @param addess is relative to the start of this store
	///
	virtual page_t getPageAddress(uint32_t vaddr) = 0;
	
	///
	/// Writes back the content held by the layer to the
	/// store underlaying it. This may be used when isWriteable
	/// returned false, and we want to make a change permanent.
	///
	/// @param vaddr address in this store
	/// @param page the phy page to write back
	///             this page is not used by this store after this
	///             call returns.
	///
	virtual void writeback(uint32_t vaddr, page_t page) = 0;
};

} // namespace VM
