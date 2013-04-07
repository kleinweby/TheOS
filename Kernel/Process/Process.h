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

#include "Utils/KObject.h"
#include "Process/Thread.h"
#include "VM/Context.h"

namespace Process {

typedef uint32_t pid_t;

class Process : public KObject
{
	friend class Thread;
	// The vm context used when running one of the threads
	Ptr<VM::Context> vmContext;
	// The parent of this process
	// Only for the process with pid=0 a NULL is acceptable here
	Ptr<Process> parent;
	// The globale unique identifier of this process
	pid_t pid;
protected:
	Ptr<Thread> thread;
public:
	Process();
	virtual ~Process();

	// Gets the vm context accosiated with this parent
	Ptr<VM::Context> getVMContext() const;

	// Gets the pid of this process
	pid_t getPID() const;

	// Sets the parent process
	void setParent(Ptr<Process> parent);
	// Gets parent process
	Ptr<Process> getParent() const;
};

}
