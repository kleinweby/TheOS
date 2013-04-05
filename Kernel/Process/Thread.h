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
#include "Interrupts/Interrupts.h"
#include <CoreSystem/CommonTypes.h>

namespace Process {

class Process;

enum class ThreadState : unsigned short
{
	//
	// Running means that the thread is activly
	// scheduled
	//
	Running,
	//
	// Blocked means that the thread waits on some
	// resource.
	//
	Blocked,
	//
	// Suspended means it was suspended explicitly
	//
	Suspended,
	//
	// Terminated means the thread exited or was killed
	//
	Terminated
};

class Thread : public KObject
{
	Interrupts::CPUState cpuState;
	// Weak pointer to holding process
	Process* process;
	// The state this thread is in
	ThreadState state;
public:
	Thread(uint32_t entryPoint, size_t stackSize, Ptr<Process> process);
	virtual ~Thread();

	// Saves the cpu state of this thread
	void setCPUState(const Interrupts::CPUState* state);
	// Gets the cpu state of this thread
	const Interrupts::CPUState* getCPUState() const;

	// Sets the current state of this thread
	void setState(ThreadState state);
	// Gets the current state of this threead
	ThreadState getState() const;
};

}
