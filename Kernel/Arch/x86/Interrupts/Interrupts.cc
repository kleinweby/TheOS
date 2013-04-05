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

#include "Interrupts.h"

#include <CoreSystem/MachineInstructions.h>
#include "Logging/Logging.h"
#include "Error/Panic.h"

namespace Interrupts {
namespace X86 {

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

static_assert(sizeof(struct _IDTEntry) == 8, "IDT Entry was packed wrong");

struct _IDTHeader {
  uint16_t size;
  uint32_t address;
} __attribute__((packed));

static_assert(sizeof(struct _IDTHeader) == 6, "IDT Header was packed wrong");

struct TSS
{
   uint32_t prev_tss; 
   uint32_t esp0; // Kernel stack
   uint32_t ss0;  // kernel stack segment
   uint32_t esp1;
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         
   uint32_t cs;        
   uint32_t ss;        
   uint32_t ds;        
   uint32_t fs;       
   uint32_t gs;         
   uint32_t ldt;      
   uint16_t trap;
   uint16_t iomap_base;
} __attribute__((packed));

struct GDTEntry {
	uint16_t limitLow;
	uint32_t baseLow:24;
	uint8_t access;
	uint8_t limitHigh:4;
	uint8_t flags:4;
	uint8_t baseHigh;
} __attribute__((packed));

static_assert(sizeof(struct GDTEntry) == 8, "GDT Entry was packed wrong");

extern "C" struct GDTEntry GDTEntryTSS;
TSS tss;

extern "C" void InterruptsLOADTSS();

static const uint8_t kInterruptsCount = 48;

Handler Handlers[kInterruptsCount];

struct _IDTEntry IDT[kInterruptsCount];
extern "C" struct _IDTHeader InterruptsIDTHeader = {.address = (uint32_t)&IDT, .size = sizeof(IDT)};

extern "C" void InterruptsLoadIDT();

// All the trampolins from the nasm file
extern "C" void InterruptsTrampolin0();
extern "C" void InterruptsTrampolin1();
extern "C" void InterruptsTrampolin2();
extern "C" void InterruptsTrampolin3();
extern "C" void InterruptsTrampolin4();
extern "C" void InterruptsTrampolin5();
extern "C" void InterruptsTrampolin6();
extern "C" void InterruptsTrampolin7();
extern "C" void InterruptsTrampolin8();
extern "C" void InterruptsTrampolin9();
extern "C" void InterruptsTrampolin10();
extern "C" void InterruptsTrampolin11();
extern "C" void InterruptsTrampolin12();
extern "C" void InterruptsTrampolin13();
extern "C" void InterruptsTrampolin14();
extern "C" void InterruptsTrampolin15();
extern "C" void InterruptsTrampolin16();
extern "C" void InterruptsTrampolin17();
extern "C" void InterruptsTrampolin18();
extern "C" void InterruptsTrampolin32();
extern "C" void InterruptsTrampolin33();
extern "C" void InterruptsTrampolin34();
extern "C" void InterruptsTrampolin35();
extern "C" void InterruptsTrampolin36();
extern "C" void InterruptsTrampolin37();
extern "C" void InterruptsTrampolin38();
extern "C" void InterruptsTrampolin39();
extern "C" void InterruptsTrampolin40();
extern "C" void InterruptsTrampolin41();
extern "C" void InterruptsTrampolin42();
extern "C" void InterruptsTrampolin43();
extern "C" void InterruptsTrampolin44();
extern "C" void InterruptsTrampolin45();
extern "C" void InterruptsTrampolin46();
extern "C" void InterruptsTrampolin47();

static void InterruptsInstallIDTEntry(uint32_t number, void (*tramp)(), 
	uint16_t selector, enum GateType type, uint8_t storageSegment, uint8_t privilegLevel)
{
    struct _IDTEntry *entry = &IDT[number];

	//memset(entry, 0, sizeof(struct _IDTEntry));

    entry->handler_low = reinterpret_cast<uint32_t>(tramp)&0xFFFF;
    entry->handler_high = reinterpret_cast<uint32_t>(tramp) >> 16;

    entry->selector = selector;
    entry->storageSegment = storageSegment;
    entry->_unused_zero = 0;
    entry->isPresent = 1;
	entry->privilegLevel = privilegLevel;
	entry->type = type;
}

void Initialize()
{
	LogTrace("Remap PIC interrupts");
	//
	// In real mode a few irq's are on places where
	// exceptions lay, so we nee to remap this
	//
	
	// Initialize master PIC
	outb(0x20, 0x11); // Init
	outb(0x21, 0x20); // Interrupt number
	outb(0x21, 0x04); // IRQ 2 is slave
	outb(0x21, 0x01); // ??
	 
	// Initialize slave PIC
	outb(0xa0, 0x11); // Ini
	outb(0xa1, 0x28); // Interrupt number
	outb(0xa1, 0x02); // IRQ 2 is slave
	outb(0xa1, 0x01); // ??
 
 	LogTrace("Setup IDT");
 
 #define INSTALL_TRAMPOLIN(n) InterruptsInstallIDTEntry(n, InterruptsTrampolin##n, 0x08, kGate32BitInterruptType, 0, 0);

	INSTALL_TRAMPOLIN(0);
	INSTALL_TRAMPOLIN(1);
	INSTALL_TRAMPOLIN(2);
	INSTALL_TRAMPOLIN(3);
	INSTALL_TRAMPOLIN(4);
	INSTALL_TRAMPOLIN(5);
	INSTALL_TRAMPOLIN(6);
	INSTALL_TRAMPOLIN(7);
	INSTALL_TRAMPOLIN(8);
	INSTALL_TRAMPOLIN(9);
	INSTALL_TRAMPOLIN(10);
	INSTALL_TRAMPOLIN(11);
	INSTALL_TRAMPOLIN(12);
	INSTALL_TRAMPOLIN(13);
	INSTALL_TRAMPOLIN(14);
	INSTALL_TRAMPOLIN(15);
	INSTALL_TRAMPOLIN(16);
	INSTALL_TRAMPOLIN(17);
	INSTALL_TRAMPOLIN(18);
 
	INSTALL_TRAMPOLIN(32);
	INSTALL_TRAMPOLIN(33);
	INSTALL_TRAMPOLIN(34);
	INSTALL_TRAMPOLIN(35);
	INSTALL_TRAMPOLIN(36);
	INSTALL_TRAMPOLIN(37);
	INSTALL_TRAMPOLIN(38);
	INSTALL_TRAMPOLIN(39);
	INSTALL_TRAMPOLIN(40);
	INSTALL_TRAMPOLIN(41);
	INSTALL_TRAMPOLIN(42);
	INSTALL_TRAMPOLIN(43);
	INSTALL_TRAMPOLIN(44);
	INSTALL_TRAMPOLIN(45);
	INSTALL_TRAMPOLIN(46);
	INSTALL_TRAMPOLIN(47);
 
	// Load IDT
 	InterruptsLoadIDT();
  
	// Mask all irqs
	SetIRQMask(kUInt32Max);

	// Setup TSS
	GDTEntryTSS.access = 0xE9; 
	GDTEntryTSS.flags = 0x4;
	GDTEntryTSS.baseLow =   reinterpret_cast<uint32_t>(&tss)        & 0xFFFFFF;
	GDTEntryTSS.baseHigh = (reinterpret_cast<uint32_t>(&tss) >> 24) & 0xFF;
	GDTEntryTSS.limitLow =   sizeof(tss)        & 0xFFFF;
	GDTEntryTSS.limitHigh = (sizeof(tss) >> 16) & 0xF;

	tss.esp0 = 0x0;
	tss.ss0 = 0x10;
	tss.iomap_base = sizeof(tss);

	// Load tss
	InterruptsLOADTSS();
}

void Enable()
{
	EnableInterrupts();
}

void Disable()
{
	DisableInterrupts();
}

void SetExceptionHandler(uint16_t exceptionNumber, Handler handler)
{
	Handlers[exceptionNumber] = handler;
}

Handler GetExceptionHandler(uint16_t exceptionNumber)
{
	return Handlers[exceptionNumber];
}

void SetIRQHandler(uint16_t irqNumber, Handler handler)
{
	Handlers[0x20 + irqNumber] = handler;
}

Handler GetIRQHandler(uint16_t irqNumber)
{
	return Handlers[0x20 + irqNumber];
}

void MaskIRQ(uint16_t irqNumber)
{
	uint8_t port = 0x21;
	uint8_t mask;

	if (irqNumber >= 8) {
		port = 0xA1;
		irqNumber -= 8;
	}

	outb(port, inb(port) & (1 << irqNumber));
}

void UnmaskIRQ(uint16_t irqNumber)
{
	uint8_t port = 0x21;
	uint8_t mask;

	if (irqNumber >= 8) {
		port = 0xA1;
		irqNumber -= 8;
	}

	outb(port, inb(port) & ~(1 << irqNumber));
}

uint32_t GetIRQMask()
{
	return (uint32_t)(inb(0xA1) << 8 | inb(0x21));
}

void SetIRQMask(uint32_t mask)
{
	outb(0x21,  mask       & 0xFF);
	outb(0xA1, (mask >> 8) & 0xFF);
}

void SetKernelStack(uint32_t stack)
{
	tss.esp0 = stack;
}

uint32_t GetKernelStack()
{
	return tss.esp0;
}

// The Halt CPU State
// ==================

// A very little stack.
// TODO: at the moment, when the cpu wakes up from the halt
// this stack will be used
const uint32_t kHaltCPUStackSize = sizeof(CPUState) + 4096;
uint8_t HaltCPUStack[kHaltCPUStackSize];

void HaltCPU() __attribute__((naked));

void HaltCPU()
{
	__asm__ volatile ("1: hlt; jmp 1;");
	while (1) {
		Halt();
	}
}

// The Interrupts handler
// ======================

extern "C" const CPUState* InterruptsHandler(const CPUState* ptr)
{
	const CPUState* newState;
	LogInfo("Interrupt");

// Debug print of interrupt state	
#if 0
	LogInfo("  edi = %x", ptr->edi);
  	LogInfo("  esi = %x", ptr->esi);
	LogInfo("  edp = %x", ptr->ebp);
	LogInfo("  ebx = %x", ptr->ebx);
	LogInfo("  edx = %x", ptr->edx);
	LogInfo("  ecx = %x", ptr->ecx);
	LogInfo("  eax = %x", ptr->eax);

	LogInfo("    # = %x", ptr->interruptNumber);
	LogInfo(" code = %x", ptr->errorCode);

	LogInfo("  eip = %x", ptr->eip);
	LogInfo("   cs = %x", ptr->cs);
	LogInfo("eflags= %x", ptr->eflags);
	LogInfo("  esp = %x", ptr->esp);
	LogInfo("   ss = %x", ptr->ss);
#endif

	Handler h = Handlers[ptr->interruptNumber];

	if (ptr->interruptNumber == 14) {
		uint32_t cr2;
  __asm__ volatile( "mov %%cr2,%0"
                  : "=a"(cr2));
		LogWarning("Pagefault %x at %x", cr2, ptr->eip);
	}

	if (h) {
		newState = h(ptr);
	}
	else {
		panic("No handler present for interrupt #%x", ptr->interruptNumber);
		newState = NULL;
	}

	if (ptr->interruptNumber < 0x20) {
		panic("Exception");
	}

	if (ptr->interruptNumber >= 0x20)
		outb(0x20,0x20);
	// TODO: EOI for second pic

	// TODO when ptr is NULL use halt cpu state
	if (newState == NULL) {
		newState = reinterpret_cast<CPUState*>(OFFSET(&HaltCPUStack[kHaltCPUStackSize - 1], -sizeof(CPUState)));
		{
			CPUState* state = const_cast<CPUState*>(newState);
			state->edi = 0xBADBEEF;
			state->esi = 0xBADBEEF;
			state->ebp = 0xBADBEEF;
			state->ebx = 0xBADBEEF;
			state->edx = 0xBADBEEF;
			state->ecx = 0xBADBEEF;
			state->eax = 0xBADBEEF;

			state->interruptNumber = 0x0;
			state->errorCode = 0x0;

			state->eip = reinterpret_cast<uint32_t>(&HaltCPU);
			state->cs = 0x8; // Run in ring 0
			state->eflags = 0x202;
			// ESP will not be switches by cpu
			// so we need to install this state into the stack
			state->esp = reinterpret_cast<uint32_t>(&HaltCPUStack[kHaltCPUStackSize - 1]);
			state->ss = 0x10;
		}
	}

#if 0
	LogInfo("  edi = %x", newState->edi);
  	LogInfo("  esi = %x", newState->esi);
	LogInfo("  edp = %x", newState->ebp);
	LogInfo("  ebx = %x", newState->ebx);
	LogInfo("  edx = %x", newState->edx);
	LogInfo("  ecx = %x", newState->ecx);
	LogInfo("  eax = %x", newState->eax);

	LogInfo("    # = %x", newState->interruptNumber);
	LogInfo(" code = %x", newState->errorCode);

	LogInfo("  eip = %x", newState->eip);
	LogInfo("   cs = %x", newState->cs);
	LogInfo("eflags= %x", newState->eflags);
	LogInfo("  esp = %x", newState->esp);
	LogInfo("   ss = %x", newState->ss);
#endif

	return newState;
}

}
}
