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

#import <CoreSystem/CommonTypes.h>

//
// Bootstrap
// =========
//
// Bootstrap is responsible for setting up the temporary
// higher-half kernel mechanism.
//
// For that we create a temporary mapping which will later
// replaced by the VM-subsystem.
//

#define BOOTSTRAP_SECTION  __attribute__ ((section (".bootstrap")))

//
// This temporarly maps paddr->vaddr for size.
//
// Note: during bootstrap the offset between paddr and
// vaddr should always be KERNEL_LOAD_ADDRESS as we're
// not able to manage the vspace.
//
void BooststrapMap(uint32_t paddr, uint32_t vaddr, uint32_t size) BOOTSTRAP_SECTION;

//
// This registers the pages used by bootstrap to the pmem subsystem
//
void BootstrapPhyMemInitialize() BOOTSTRAP_SECTION;

//
// This releases all the resources used by the bootstrap mechanism.
// (Except for the this method it self)
//
void BootstrapRelease();
