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
#include "Interrupts/Interrupts.h"
#include "Interrupts/Timer.h"

namespace Process {

class Scheduler;

//
// Initializes the scheduler subsystem
// after that you can create threads/processes/etc
//
void Initialize();

//
// This will enabled scheduling for the current cpu
// and transfers controll to processes.
//
void TakeOff() __attribute__((noreturn));

//
// Get the current executing thread
//
Ptr<Thread> GetCurrentThread();

extern GlobalPtr<Scheduler> GlobalScheduler;

class SchedulerItem : public KObject
{
public:
	Ptr<Thread> thread;
	Ptr<SchedulerItem> next;
	SchedulerItem(Ptr<Thread> t) : thread(t), next(NULL) {}
	virtual ~SchedulerItem();
};

class Scheduler : public KObject
{
private:
	Ptr<SchedulerItem> nextItem;
	Ptr<Thread> currentThread;
	Ptr<Timer::Timer> timer;

	void removeThreadFromScheduling(Ptr<Thread> thread);
	void addThreadToScheduling(Ptr<Thread> thread);
protected:
	friend class Thread;
	friend void TakeOff();
	void threadStateDidChange(Ptr<Thread> thread);
public:
	Scheduler();
	virtual ~Scheduler();
	const Interrupts::CPUState* schedule(const Interrupts::CPUState* state);
	Ptr<Thread> getCurrentThread() const;
};

}
