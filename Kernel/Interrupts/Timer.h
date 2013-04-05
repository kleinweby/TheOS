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

namespace Timer {


//
// Initialize the Timer subsystem
//
void Initialize();

//
// Timers will always be oneshot as they are
// intended for scheduling purposes
//
class Timer : public KObject {
public:
	virtual ~Timer();

	// Gets the current set handler
	virtual Interrupts::Handler getHandler() const = 0;

	// Sets the handler for timer interrupts
	virtual void setHandler(Interrupts::Handler handler) = 0;

	// Get the remaining ticks until the timer fires
	// TODO: Implement some sort of universal time thingy here
	virtual uint32_t getTicks() const = 0;

	// Set the ticks until the timer fires
	// Note: will enable the timer
	// Setting a value of 0 results in disabling the timer
	virtual void setTicks(uint32_t ticks) = 0;

	// Enable the timer
	virtual void enable() = 0;

	// Disable the timer
	virtual void disable() = 0;
};

}

#include_next "Interrupts/Timer.h"

namespace Timer {

// Import the platform timer namespace
namespace Native = __PLATFORM__;

//
// Use this Timer object for scheduling timer interrupts
//
// It will always be local to the current cpu
//
using Native::GetLocalTimer;

}

