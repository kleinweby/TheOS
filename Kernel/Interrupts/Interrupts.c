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

#include "Interrupts.h"

#include "Error/Assert.h"

#include <CoreSystem/CommonTypes.h>

enum GateType {
  kGate32BitTaskType = 0x5,
  kGate16BitInterruptType = 0x6,
  kGate16BitTrapType = 0x7,
  kGate32BitInterruptType = 0xE,
  kGate32BitTrapType = 0xF,
};

struct _IDTEntry {
   uint16_t handler_low;
   uint16_t selector;
   uint8_t _unused_zero; 
   uint8_t type:4;
   uint8_t storageSegment:1;
   uint8_t privilegLevel:2;
   uint8_t isPresent:1;
   uint16_t handler_high;
} __attribute__((packed));

static_assert(sizeof(struct _IDTEntry) == 8);

struct _IDTHeader {
  uint16_t size;
  uint32_t address;
} __attribute__((packed));

static_assert(sizeof(struct _IDTHeader) == 6);

struct _IDTEntry IDT[1];
struct _IDTHeader InterruptsIDTHeader = {.address = (uint32_t)&IDT, .size = sizeof(IDT)};
