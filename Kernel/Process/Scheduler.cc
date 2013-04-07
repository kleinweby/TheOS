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

#include "Process/Scheduler.h"
#include "Logging/Logging.h"

#include <CoreSystem/MachineInstructions.h>

namespace Process {

GlobalPtr<Scheduler> GlobalScheduler;

const Interrupts::CPUState* schedule_helper(const Interrupts::CPUState* state)
{
	return GlobalScheduler->schedule(state);
}

void Initialize()
{
	GlobalScheduler = new Scheduler();
}

void TakeOff()
{
	LogInfo("Take Off");
	GlobalScheduler->timer->enable();

	// This shoul normally not loop, the interrupt
	// subsystem has its own halt process/state
	while(1) Halt();
}

Ptr<Thread> GetCurrentThread()
{
	return GlobalScheduler->getCurrentThread();
}

SchedulerItem::~SchedulerItem() 
{
}

Scheduler::Scheduler()
{
	this->timer = Timer::GetLocalTimer();
	this->timer->setHandler(schedule_helper);
}
Scheduler::~Scheduler()
{
}

void Scheduler::removeThreadFromScheduling(Ptr<Thread> thread)
{
	if (this->nextItem != NULL) {
		if (this->nextItem->thread == thread) {
			this->nextItem = this->nextItem->next;
		}
		else {
			Ptr<SchedulerItem> item = this->nextItem;

			while (item->next) {
				if (item->next->thread == thread) {
					item->next = item->next->next;
				}

				item = item->next;
			}
		}
	}
}

void Scheduler::addThreadToScheduling(Ptr<Thread> thread)
{
	if (this->nextItem == NULL) {
		this->nextItem = new SchedulerItem(thread);
	}
	Ptr<SchedulerItem> item = this->nextItem;

	while (item->next) item = item->next;

	item->next = new SchedulerItem(thread);
}

void Scheduler::threadStateDidChange(Ptr<Thread> thread)
{
	this->removeThreadFromScheduling(thread);
	this->addThreadToScheduling(thread);
}

const Interrupts::CPUState* Scheduler::schedule(const Interrupts::CPUState* state)
{
	// A thread ran, so save it's state
	if (state && this->currentThread) {
		this->currentThread->setCPUState(state);
		this->addThreadToScheduling(this->currentThread);
		this->currentThread = NULL;
	}

	// We have an item to run
	if (this->nextItem) {
		Ptr<SchedulerItem> item = this->nextItem;

		this->nextItem = this->nextItem->next;

		this->timer->setTicks(kUInt16Max);
		return item->thread->getCPUState();
	}
	else {
		LogInfo("Schedule Halt");
		this->timer->setTicks(kUInt16Max);
		return NULL;
	}
}

Ptr<Thread> Scheduler::getCurrentThread() const
{
	return this->currentThread;
}

}

