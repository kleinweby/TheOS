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

#include "PhyMem.h"

#include "Logging/Logging.h"

static const uint32_t kPageMask = ~(kPhyMemPageSize - 1);
static const uint32_t kFreeBitmapPlanes = 4ULL*1024ULL*1024ULL*1024ULL /* 4GB */ / kPhyMemPageSize /* Page size */ / 32 /* 32 pages per 32bit uint_t value */;
static uint32_t FreeBitmap[kFreeBitmapPlanes];

static inline uint32_t PageNumberFromAddress(pointer_t address)
{
	return (uint32_t)address/kPhyMemPageSize;
}

static inline uint32_t PlaneNumberFromAddress(pointer_t address)
{
	return PageNumberFromAddress(address) >> 5;
}

static inline uint32_t BitNumberInPlaneFromAddress(pointer_t address)
{
	return PageNumberFromAddress(address) & 0x1F;
}

static inline pointer_t AddressFromPlaneAndBit(uint32_t planeNumber, uint32_t bitNumber)
{
	return (pointer_t)(((planeNumber << 5) | bitNumber) * kPhyMemPageSize);
}

void PhyMemInitialize()
{
	// Nothing is free
	for (uint32_t i = 0; i < kFreeBitmapPlanes; i++)
		FreeBitmap[i] = 0;
	
	LogInfo("PhyMem initialized");
}

void LogPhyMem()
{
	for (uint32_t i = 0; i < kFreeBitmapPlanes; i++) {
		char f[33];
		
		for (uint8_t j = 0; j < 32; j++)
			f[j] = (FreeBitmap[i]&(1 << j)) ? 'F' : '_';
		
		f[32] = '\0';
		
		LogVerbose("PhyMem: %010x: %s", i * kPhyMemPageSize * 32, f);
		
		// Collapse uniform
		{
			uint32_t j = i+1;
			for (; j < kFreeBitmapPlanes && FreeBitmap[i] == FreeBitmap[j]; j++);
			
			// Collapse 5 consequend rows
			if (j - i >= 3) {
				LogVerbose("PhyMem:             : %s", f);
				i = j - 2;
			}
		}
	}
}

void _PhyMemMarkFree(pointer_t page)
{
	uint32_t planeNumber = PlaneNumberFromAddress(page);

	if (planeNumber > kFreeBitmapPlanes)
		return;

	FreeBitmap[planeNumber] |= 1 << BitNumberInPlaneFromAddress(page);
}

void _PhyMemMarkUsed(pointer_t page)
{
	uint32_t planeNumber = PlaneNumberFromAddress(page);

	if (planeNumber > kFreeBitmapPlanes)
		return;

	FreeBitmap[planeNumber] &= ~(1 << BitNumberInPlaneFromAddress(page));
}

void _PhyMemMarkUsedRange(pointer_t address, size_t size)
{
	for(uint32_t addr = ((uint32_t)address & kPageMask); 
	    addr < (uint32_t)address + size;
		addr += kPhyMemPageSize) {
		_PhyMemMarkUsed((pointer_t)addr);
	}
}
