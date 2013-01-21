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

//
// Internal opaque object structure. Do NOT use
// it in any way.
//
struct _Object {	
	//
	// Count how many objects hold a reference to this
	// retainable.
	//
	int32_t retainCount;
	//
	// Is called when retain Count reaches zero
	//
	void (*Dealloc)(void* ptr);
};

//
// Call this to declare that a class with the name A
// exists
//
#define DECLARE_CLASS(A) typedef struct _##A* A __attribute__((NSObject))

//
// Call this (usually in the implementation) to define your
// class and this member variables
//
#define DEFINE_CLASS(A, S, x)\
struct _##A {\
	struct _##S super;\
	x\
}

//
// Some helper methods to declare ownership, so
// that it can be automaticly tested.
//

//
// Attributes an function that returns an object
// with rc +1
//
// Object creation methods should have this
//
#define OBJECT_RETURNS_RETAINED __attribute__((ns_returns_retained))

//
// Attributes an function that returns an object with rc -1
//
#define OBJECT_CONSUME __attribute__((ns_consumed))

//
// Declares a gerneral object class
//
DECLARE_CLASS(Object);

//
// Call this at the beginning of the object creation
// it will set the retaincount to 1.
//
// Dealloc will be called when rc reaches 0, after that the object
// will be freed (don't do that in Dealloc).
//
bool ObjectInit(void* object, void (*Dealloc)(void* ptr)) __attribute__((nonnull));

//
// Increases the retain count by 1.
//
// Note: this will crash when the retain count is < 1
//
OBJECT_RETURNS_RETAINED
Object _Retain(Object object) __attribute__((nonnull));

//
// Releases the reference to this object.
//
void _Release(Object OBJECT_CONSUME object) __attribute__((nonnull));

//
// Typecorrect retain and release
//
#define Retain(...) ((__typeof(__VA_ARGS__)) _Retain((void*)(__VA_ARGS__)))
#define Release(...) _Release((void *)(__VA_ARGS__))
