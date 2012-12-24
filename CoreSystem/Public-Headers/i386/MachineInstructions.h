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

#ifndef X86_MACHINE_INSTRUCTIONS_H
#define X86_MACHINE_INSTRUCTIONS_H

#include <CoreSystem/i386/Integers.h>

static inline void outb(uint16_t port, uint8_t val )
{
    __asm__ volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

static inline uint8_t inb(uint16_t port )
{
    uint8_t ret;
    __asm__ volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

static inline void DisableInterrupts()
{
  __asm__ __volatile__ ("cli");
}

static inline void EnableInterrupts()
{
  __asm__ __volatile__ ("sti");
}

static inline void Halt()
{
  __asm__ __volatile__ ("hlt");
}

static inline uint64_t TimeStampCounter(void) {
  uint32_t lo, hi;

  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi) :: "ecx" );

  return (uint64_t)hi << 32 | lo;
}

#endif /* MACHINE_INSTRUCTIONS_H */
