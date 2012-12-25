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

//
// Abstract
// =======
//
// This file provides commong types used in the system
//


#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <CoreSystem/Integers.h>

typedef uint32_t size_t;
static const size_t kSizeMax = kUInt32Max;

typedef uint32_t offset_t;
static const offset_t kOffsetMax = kUInt32Max;

typedef void* pointer_t;
static const pointer_t NULL = (pointer_t)0;

static inline pointer_t OFFSET(pointer_t ptr, offset_t off) {
	return (pointer_t)((uint32_t)ptr + off);
}

typedef uint8_t bool;

static const bool true = (bool)1;
static const bool false = (bool)0;

static const bool YES = (bool)1;
static const bool NO = (bool)0;

#endif /* COMMON_TYPES_H */
