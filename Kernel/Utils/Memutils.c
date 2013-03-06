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

#import "Memutils.h"

int memcmp(const void *_s1, const void *_s2, size_t n)
{
	const char *s1 = _s1;
	const char *s2 = _s2;
	
	for (size_t i = 0; i < n; i++, s1++, s2++) {
		if (*s1 < *s2)
			return -1;
		else if (*s1 > *s2)
			return 1;
	}
	
	return 0;
}

void *memset(void *_b, int c, size_t len)
{
	int *b = _b;
	
	for (size_t i = 0; i < len/sizeof(c); i++, b++)
		*b = c;
	
	return _b;
}

// void *memmove(void *s1, const void *s2, size_t n)
// {
// 	
// }

void *memcpy(void *restrict _s1, const void *restrict _s2, size_t n)
{
	char *s1 = _s1;
	const char *s2 = _s2;
	
	for (size_t i = 0; i < n; i++, s1++, s2++) {
		*s1 = *s2;
	}
	
	return _s1;
}
