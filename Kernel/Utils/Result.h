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

#import <CoreSystem/CommonTypes.h>
#import "Utils/KObject.h"
#import "Error/Error.h"

enum class ResultType : short {
	Success,
	Error,
	TryAgain
};

template<class T = Error>
class Result {
private:
	// Disallow dynamic instances
	void* operator new(size_t) = delete;
	void* operator new[](size_t) = delete;
	void operator delete(void *) = delete;
	void operator delete[](void*) = delete;
protected:
	ResultType resultType;
	// Use kobject here to be able to store a
	// waiting object here
	Ptr<KObject> object;
	
public:
	Result() : Result(ResultType::Success) {}
	Result(ResultType _resultType)
	{
		this->resultType = _resultType;
	}
	
	Result(Ptr<T> _object) : Result(ResultType::Success)
	{
		this->object = _object;
	}
	
	Result(bool succeeded) : Result(succeeded ? ResultType::Success : ResultType::Error) {}
	
	operator bool() const
	{
		return this->isSuccess();
	}
	
	inline bool isSuccess() const
	{
		return resultType == ResultType::Success;
	}
	
	inline bool isError() const
	{
		return resultType == ResultType::Error;
	}
	
	inline bool isTryAgain() const
	{
		return resultType == ResultType::TryAgain;
	}
	
	operator T() const
	{
		return (T)this->object;
	}
	
	operator Ptr<Error>() const
	{
		return (Ptr<Error>)this->object;
	}
};
