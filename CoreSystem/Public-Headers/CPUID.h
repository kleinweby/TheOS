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


#ifndef _CPUID_H_
#define _CPUID_H_

#include <CoreSystem/CommonTypes.h>

static inline bool isCPUIDSupported() {
  // See http://wiki.osdev.org/CPUID
  return YES;
}

typedef enum {
  kCPUID_Vendor,
  kCPUID_Features,
  kCPUID_TLB,
  kCPUID_Serial,

  kCPUID_IntelExtended = 0x80000000,
  kCPUID_IntelFeatues,
  kCPUID_IntelBrand,
  kCPUID_IntelBrandMore,
  kCPUID_IntelBrandEnd
} CPUIDRequestCode;

enum CPUFeatureECX {
  kCPUFeatureSSE3         = 1 << 0, 
  kCPUFeaturePCLMUL       = 1 << 1,
  kCPUFeatureDTES64       = 1 << 2,
  kCPUFeatureMONITOR      = 1 << 3,  
  kCPUFeatureDS_CPL       = 1 << 4,  
  kCPUFeatureVMX          = 1 << 5,  
  kCPUFeatureSMX          = 1 << 6,  
  kCPUFeatureEST          = 1 << 7,  
  kCPUFeatureTM2          = 1 << 8,  
  kCPUFeatureSSSE3        = 1 << 9,  
  kCPUFeatureCID          = 1 << 10,
  kCPUFeatureFMA          = 1 << 12,
  kCPUFeatureCX16         = 1 << 13, 
  kCPUFeatureETPRD        = 1 << 14, 
  kCPUFeaturePDCM         = 1 << 15, 
  kCPUFeatureDCA          = 1 << 18, 
  kCPUFeatureSSE4_1       = 1 << 19, 
  kCPUFeatureSSE4_2       = 1 << 20, 
  kCPUFeaturex2APIC       = 1 << 21, 
  kCPUFeatureMOVBE        = 1 << 22, 
  kCPUFeaturePOPCNT       = 1 << 23, 
  kCPUFeatureAES          = 1 << 25, 
  kCPUFeatureXSAVE        = 1 << 26, 
  kCPUFeatureOSXSAVE      = 1 << 27, 
  kCPUFeatureAVX          = 1 << 28,
};

enum CPUFeatureEDX {
  kCPUFeatureFPU          = 1 << 0,  
  kCPUFeatureVME          = 1 << 1,  
  kCPUFeatureDE           = 1 << 2,  
  kCPUFeaturePSE          = 1 << 3,  
  kCPUFeatureTSC          = 1 << 4,  
  kCPUFeatureMSR          = 1 << 5,  
  kCPUFeaturePAE          = 1 << 6,  
  kCPUFeatureMCE          = 1 << 7,  
  kCPUFeatureCX8          = 1 << 8,  
  kCPUFeatureAPIC         = 1 << 9,  
  kCPUFeatureSEP          = 1 << 11, 
  kCPUFeatureMTRR         = 1 << 12, 
  kCPUFeaturePGE          = 1 << 13, 
  kCPUFeatureMCA          = 1 << 14, 
  kCPUFeatureCMOV         = 1 << 15, 
  kCPUFeaturePAT          = 1 << 16, 
  kCPUFeaturePSE36        = 1 << 17, 
  kCPUFeaturePSN          = 1 << 18, 
  kCPUFeatureCLF          = 1 << 19, 
  kCPUFeatureDTES         = 1 << 21, 
  kCPUFeatureACPI         = 1 << 22, 
  kCPUFeatureMMX          = 1 << 23, 
  kCPUFeatureFXSR         = 1 << 24, 
  kCPUFeatureSSE          = 1 << 25, 
  kCPUFeatureSSE2         = 1 << 26, 
  kCPUFeatureSS           = 1 << 27, 
  kCPUFeatureHTT          = 1 << 28, 
  kCPUFeatureTM1          = 1 << 29, 
  kCPUFeatureIA64         = 1 << 30,
  kCPUFeaturePBE          = 1 << 31
};

/** issue a single request to CPUID. Fits 'intel features', for instance
 *  note that even if only "eax" and "edx" are of interest, other registers
 *  will be modified by the operation, so we need to tell the compiler about it.
 */
static inline void CPUID(CPUIDRequestCode code, uint32_t *a, uint32_t *d) {
  __asm__ volatile("cpuid":"=a"(*a),"=d"(*d):"a"(code):"ecx","ebx");
}
 
/** issue a complete request, storing general registers output as a string
 */
/*
  Fetches a string from cpuid and stores eax, ebx, ecx and edx in buffer[0-3].

  Note: for some strings you it seems that you need to swap buffer[2] and buffer[3].
 */
static inline void CPUIDString(CPUIDRequestCode code, uint32_t buffer[4]) {
  __asm__ volatile("cpuid":"=a"(*buffer),"=b"(*(buffer+1)),
               "=c"(*(buffer+2)),"=d"(*(buffer+3)):"a"(code));
}

#endif /* _CPUID_H_ */