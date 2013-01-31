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

#import "VMLinearObject.h"

#import "Error/Assert.h"
#import "PhyMem.h"
#import "kalloc.h"

DEFINE_CLASS(VMLinearObject, VMObject,
	offset_t phyBase;
);

static void VMLinearObjectAddToContextImpl(VMObject object, VMContext context);
static void VMLinearObjectRemoveFromContextImpl(VMObject object, VMContext context);
static char* VMLinearObjectDescriptionImpl(VMObject object);

VMLinearObject VMLinearObjectCreate(VMSection section, offset_t base, offset_t phyBase, size_t size)
{
	assert(section);
	assert(base > 0);
	assert(size % kPhyMemPageSize == 0);
	
	VMLinearObject obj = kalloc(sizeof(struct _VMLinearObject));
	
	if (!obj)
		return NULL;
	
	if (!VMObjectInitialize((void*)obj, section, base, size, VMLinearObjectAddToContextImpl,
		VMLinearObjectRemoveFromContextImpl, VMLinearObjectDescriptionImpl,
		VMObjectDeallocImpl))
		return NULL;
	
	obj->phyBase = phyBase;
	
	return obj;
}

static void VMLinearObjectAddToContextImpl(VMObject _object, VMContext context)
{
	VMLinearObject object = (VMLinearObject)_object;
	
	#pragma unused(object, context)
}

static void VMLinearObjectRemoveFromContextImpl(VMObject _object, VMContext context)
{
	VMLinearObject object = (VMLinearObject)_object;
	
	#pragma unused(object, context)
}

static char* VMLinearObjectDescriptionImpl(VMObject _object)
{
	VMLinearObject object = (VMLinearObject)_object;
	#pragma unused(object)
	return NULL;
}
