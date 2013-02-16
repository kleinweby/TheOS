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

#import "Utils/Object.h"

DECLARE_CLASS(VMContext);

#import "VMSection.h"
#import "VMBackend.h"


DEFINE_CLASS(VMContext, Object,
	// The backend for managing mappings
	VMBackendContext backend;
	
	// All our sections
	// ordered to their base
	VMSection* sections;
);

//
// Create a new VMContext
//
OBJECT_RETURNS_RETAINED
VMContext VMContextCreate();

//
// Create a new VMContext with a given backend
//
OBJECT_RETURNS_RETAINED
VMContext VMContextCreateWithBackend(VMBackendContext backend);

//
// Adds a section to this context at a given offset.
//
void VMContextAddSection(VMContext context, VMSection section, offset_t offset);

//
// Removes a section from this context
//
void VMContextRemoveSection(VMContext context, VMSection section);

//
// Switches to the given context
//
void VMContextSwitch(VMContext context);
