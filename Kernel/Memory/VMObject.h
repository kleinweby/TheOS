//
// Copyright (c) 2012, Christian Speich
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

typedef struct VMObject VMObject;

#import "Memory/VMSection.h"

struct VMObject {
	offset_t base; // Relative to the section
	size_t size;
	
	// The section we're in
	VMSection* section;
	
	// We don't need to keep track of the contexts
	// we're added, the section knows about them
	
	//
	// Members
	//
	void (*addToContext)(VMObject* object, VMContext* context);
	void (*removeFromContext)(VMObject* object, VMContext* context);
	
	char* (*description)(VMObject* object);
	
	void (*dealloc)(VMObject* object);
};

bool VMObjectInitialize(VMObject* obj,
		VMSection* section,
		offset_t base,
		size_t size,
		void (*addToContextImpl)(VMObject* object, VMContext* context),
		void (*removeFromContextImpl)(VMObject* object, VMContext* context),
		char* (*descriptionImpl)(VMObject* object),
		void (*deallocImpl)(VMObject* object));
