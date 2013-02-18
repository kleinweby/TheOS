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
#import "Error/Assert.h"

//
// Ptr and GlobalPtr
// =================
//
// (GLobal)Ptr are used for simple ref count
// mmgt of kernel objects. They MUST never be
// dynamicly allocated and and ensure a objects
// presence during the current scope.
//
// GlobalPtr is Ptr without the con- and 
// destructors to make it usable in global/static
// scope.
//
//

template <class T>
class GlobalPtr {
private:
	// Disallow dynamic instances
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;
	void operator delete(void *) = delete;
	void operator delete[](void*) = delete;
protected:
	// Reference to the object
	T* object;
public:
	
	void operator=(T* other)
	{
		if (this->object != NULL)
			this->object->Release();
		
		if (other != NULL)
			other->Retain();
		this->object = other;
	}
	
	void operator=(GlobalPtr<T> other)
	{
		if (this->object != NULL)
			this->object->Release();
		if (*other != NULL)
			other->Retain();
		this->object = *other;
	}
	
	T* operator*() const
	{
		return this->object;
	}
	
	T* operator->() const
	{
		return this->object;
	}
};

template <class T>
class Ptr : public GlobalPtr<T> {
public:
	Ptr()
	{
		this->object = NULL;
	}
	
	Ptr(T* obj)
	{
		obj->Retain();
		this->object = obj;
	}

	//
	// We need to use a template constructor here
	// for the following case:
	//
	// B is subclass of A
	// Ptr<B> test1 = ...
	// Ptr<A> test2 = test1;
	// Without the last step would not produce a
	// compatible cast.
	//
	template <class TOther>
	Ptr(GlobalPtr<TOther> const& obj) : Ptr(*obj)
	{
	}
	
	~Ptr()
	{
		this->object->Release();
	}
};

class KObject {
private:
	//
	// Count how many objects hold a reference to this
	// retainable.
	//
	int32_t retainCount;
	
public:
	void* operator new   (size_t);
	void* operator new[] (size_t);
	void  operator delete   (void*);
	void  operator delete[] (void*);
	
	KObject()
	{
		this->retainCount = 0;
	}
	
	void Retain()
	{
		int32_t rc;
		
		rc = __sync_add_and_fetch(&this->retainCount, 1);
	
		//
		// The rc may be 1 after this, as newly created objects
		// have an rc = 0, and the rc system only kicks in when
		// first assigned to the Ptr
		//
		assert(rc >= 1);
	}
	
	void Release()
	{
		int32_t rc;
		
		rc = __sync_sub_and_fetch(&this->retainCount, 1);
		
		//
		// < 0 Would mean double free
		//
		assert(rc >= 0);
	
		if (rc == 0) {
			delete this;
		}
	}
};
