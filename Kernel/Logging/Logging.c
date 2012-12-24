/*
Copyright (c) 2012, Christian Speich
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <CoreSystem/MachineInstructions.h>

#include "Logging.h"
#include "LinkerHelper.h"

LINKER_SYMBOL(LoggingProviders, LogProvider*);
LINKER_SYMBOL(LoggingProvidersLength, uint32_t);

void LoggingInitialize()
{
	LogInfo("Logging initialized.");
}

void _Log(const char* function, const char* filename, uint32_t line, LogLevel logLevel, const char* format, ...)
{
	va_list args;
	
	va_start(args, format);
	_Log_va(function, filename, line, logLevel, format, args);
	va_end(args);
}

void _Log_va(const char* function, const char* filename, uint32_t line, LogLevel logLevel, const char* format, va_list args)
{
	// Use some hackish way to get a simple timestamp
	uint64_t timestamp = TimeStampCounter() >> 24;
	uint32_t count = LoggingProvidersLength/sizeof(LogProvider);

	for (uint32_t i = 0; i < count; i++) {
		LoggingProviders[i](function, filename, line, timestamp, logLevel, format, args);
	}
}
