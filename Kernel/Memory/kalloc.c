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

#import "kalloc.h"

#import "Error/Assert.h"
#import "Logging/Logging.h"

// kalloc is loosly based on http://g.oswego.edu/dl/html/malloc.html

//
// The Heap will be one contigious space.
//

//
// Flags are the lower 3 bits of the size
//
typedef size_t ChunkFlags;
static const ChunkFlags kChunkFree = (1 << 0);
static const ChunkFlags kChunkUsed = (1 << 1);

static const size_t ChunkFlagsMask = 0x7;

// This chunk structure will be placed at the beginning of
// every free chunk.
//
// Therefore the minimum freeable size is sizeof(FreeChunk) = 16bytes (on 32bit)
// which means the minimum allocatable size is also 16bytes (12 usable)
typedef struct FreeChunk FreeChunk;
struct FreeChunk {
	size_t size; // Lower 4 bits are flags
	FreeChunk* prev;
	FreeChunk* next;
};

//
// Header for allocated chunk
//
typedef struct {
	size_t size; // Lower 4 bits are flags
} UsedChunk;

// Helper Functions
static inline size_t ChunkSize(void* c) {
	UsedChunk* chunk = c;
	return chunk->size & ~ChunkFlagsMask;
}

// Base chunk, never allocated
FreeChunk* heapStart;
// Last chunk in heap, never allocated
FreeChunk* heapEnd;

void KAllocInitialize(void* ptr, size_t size)
{
	FreeChunk* chunk=0;
	
	heapStart = ptr;
	heapEnd = OFFSET(ptr, size - sizeof(FreeChunk));
	chunk = OFFSET(ptr, sizeof(FreeChunk));
	
	heapStart->size = sizeof(FreeChunk) | kChunkFree;
	heapEnd->size = sizeof(FreeChunk) | kChunkFree;
	chunk->size = size - 2*sizeof(FreeChunk) | kChunkFree;
	
	heapStart->prev = heapEnd->next = NULL;
	heapStart->next = chunk;
	chunk->next = heapEnd;
	heapEnd->prev = chunk;
	chunk->prev = heapStart;
}

void* kalloc(size_t size)
{
	FreeChunk* chunk = heapStart;
	
	// Go one further, we never allocate the heap chuck
	chunk = chunk->next;
	
	// Adjust size for our overhead
	size += sizeof(UsedChunk);
	
	// Align our size
	if (size & 0x7)
		size = (size & (size_t)~0x7) + 8;

	// Currently use the first fit strategy
	// this is not really optimal, we'll use
	// bins later one
	while(chunk != heapEnd) {
		assert(chunk);
		assert(chunk->size & kChunkFree);
		// Do we fit exaclty?
		if (ChunkSize(chunk) == size) {
			// Adjust free chaining
			chunk->prev->next = chunk->next;
			if (chunk->next)
				chunk->next->prev = chunk->prev;
			
			break; // Now transform our chunk
		}
		// Do we fit, and can at leas put FreeChunk it the remainder?
		else if (ChunkSize(chunk) >= size + sizeof(FreeChunk)) {
			FreeChunk* c = OFFSET(chunk, size);

			c->size = chunk->size - size | kChunkFree;
			c->prev = chunk->prev;
			c->next = chunk->next;
			chunk->prev->next = c;
			if (chunk->next)
				chunk->next->prev = c;
			chunk->size = size;
			break; // Now transform our chunk
		}
		
		chunk = chunk->next;
	}
	
	if (chunk == heapStart || chunk == heapEnd)
		return NULL;
	
	// Clear free flag
	chunk->size &= ~kChunkFree;
	// Set used flag
	chunk->size |= kChunkUsed;
	
	assert(chunk != heapStart && chunk != heapEnd);
	return OFFSET(chunk, sizeof(UsedChunk));
}

void free(void* ptr)
{
	// Adjust pointer
	ptr = OFFSET(ptr, -sizeof(UsedChunk));
	
	UsedChunk* chunk = ptr;
	assert(chunk->size & kChunkUsed);
	
	FreeChunk* next;
	
	// Find free next Chunk
	next = ptr+ChunkSize(chunk);
	while (next->size & kChunkUsed)
		next = next + ChunkSize(next);
	
	// Found the next free chunk
	// Is that adjacent at our end?
	if (next == OFFSET(ptr, ChunkSize(chunk)) && next != heapEnd) {
		// Maybe we're adjacent to the chunk before?
		if (OFFSET(next->prev, ChunkSize(next->prev)) == ptr && next->prev != heapStart) {
			// Woah great, just conceal us :)
			// expand the size
			next->prev->size += ChunkSize(next) + ChunkSize(next);
			// Remove the next node from the list
			next->prev->next = next->next;
			next->next->prev = next->prev;
		}
		// Well at least we can conceal with the chunk behind
		else {
			FreeChunk* c = ptr;
			// Mark free
			c->size &= ~ChunkFlagsMask;
			c->size |= kChunkFree;
			
			// Expand size
			c->size += ChunkSize(next);
			
			// Add to chain, remove next
			next->prev->next = c;
			c->prev = next->prev;
			next->next->prev = c;
			c->next = next->next;
		}
	}
	// Are we adjacent at our front?
	else if (OFFSET(next->prev, ChunkSize(next->prev)) == ptr && next->prev != heapStart) {
		// Only need to expand the chunk, that's easy!
		next->prev->size += ChunkSize(chunk);
	}
	// No one loves us, we're alone
	else {
		FreeChunk* c = ptr;
		// Mark free
		c->size &= ~ChunkFlagsMask;
		c->size |= kChunkFree;
		
		// Add us to chaining
		next->prev->next = c;
		c->prev = next->prev;
		next->prev = c;
		c->next = next;
	}
}
