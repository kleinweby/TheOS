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

#include "Interrupts/Timer.h"

#include <CoreSystem/MachineInstructions.h>
#include "Logging/Logging.h"

namespace Timer {
namespace X86 {

const uint16_t kPICInterruptNumber = 0x20;
GlobalPtr<Timer> LocalTimer;

void Initialize()
{
	LocalTimer = new PIT();
}

Ptr<Timer> GetLocalTimer()
{
	return LocalTimer;
}

PIT::PIT()
{
	//
	// 0x43 is the config port for the PIT
	//
	// Config value:
	//  bit 0 - 0 binary format
	//  bit 1-3 - 0 One shot counter
	//  bit 4-5 - 11b first LSB then MSB
	//  bit 6-7 - 0 Channel 0
	//
	outb(0x43, 0x30);
}

PIT::~PIT()
{
	// Disable the timer
	this->setTicks(0);
	// Remove the handler
	this->setHandler(NULL);
}

Interrupts::Handler PIT::getHandler() const
{
	return Interrupts::GetHandler(kPICInterruptNumber);
}

void PIT::setHandler(Interrupts::Handler handler)
{
	Interrupts::SetHandler(kPICInterruptNumber, handler);
}

uint32_t PIT::getTicks() const
{
	// Can we read from port 0x40?
	return 0;
}

void PIT::setTicks(uint32_t ticks)
{
	if (ticks > kUInt16Max) {
		LogWarning("PIT asked to interrupt in %d ticks, clapped to %d", ticks, kUInt16Max);
		ticks = kUInt16Max;
	}

	outb(0x40, ticks & 0xFF);
	outb(0x40, (ticks >> 8) &0xFF);
}

}
}
