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

#import "Object.h"

#import "Error/Assert.h"
#import "Memory/kalloc.h"

bool ObjectInit(void* _obj, void (*Dealloc)(void* ptr))
{
	assert(_obj != NULL);
	assert(Dealloc != NULL);
	
	Object obj = _obj;
	
	obj->retainCount = 1;
	obj->Dealloc = Dealloc;
	
	return true;
}

Object _Retain(Object obj)
{
	assert(obj != NULL);
	
	int32_t rc;
		
	rc = __sync_add_and_fetch(&obj->retainCount, 1);
	
	//
	// The retaincount must be greater than 1 because
	// before it would be >=1 and we added one, so
	// it must be > 1 now
	//
	assert(rc > 1);
	
	return obj;
}

void _Release(Object obj)
{
	assert(obj != NULL);
		
	int32_t rc;
		
	rc = __sync_sub_and_fetch(&obj->retainCount, 1);
		
	//
	// < 0 Would mean double free
	//
	assert(rc >= 0);
	
	if (rc == 0) {
		obj->Dealloc(obj);
		free(obj);
	}
}
