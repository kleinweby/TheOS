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
//

#pragma once

#include <CoreSystem/CommonTypes.h>

#include "LinkerHelper.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// A string used as kernel version
//
// This may be a full version string, or
// is equal to KernelGitVersion when a full version
// could not be produced
extern char* KernelVersion;

//
// A short git hash identifier, describing the build
// version of this theos kernel
//
extern char* KernelGitVersion;

//
// Some basic information about the compile-time
// kernel layout
//
LINKER_SYMBOL(KernelOffset, pointer_t);
LINKER_SYMBOL(KernelLength, offset_t);
// Note: the bootstrap section will be deleted
// when the vm subsystem is up and running
LINKER_SYMBOL(KernelBootstrapOffset, pointer_t);
LINKER_SYMBOL(KernelBootstrapLength, offset_t);
// The Text Section
LINKER_SYMBOL(KernelTextOffset, pointer_t);
LINKER_SYMBOL(KernelTextLength, offset_t);
// The Data Section
LINKER_SYMBOL(KernelDataOffset, pointer_t);
LINKER_SYMBOL(KernelDataLength, offset_t);
// The .rodata section
LINKER_SYMBOL(KernelRODataOffset, pointer_t);
LINKER_SYMBOL(KernelRODataLength, offset_t);

#ifdef __cplusplus
}
#endif
