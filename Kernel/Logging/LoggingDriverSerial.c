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

#include <CoreSystem/String.h>
#include <CoreSystem/MachineInstructions.h>

#include "Logging.h"

static void SerialWrite(uint16_t base, char chr) {
  while ((inb(base+5)&0x20)==0);
  outb(base,(uint8_t)chr);
}

static void SerialPutchar(char chr) {
	SerialWrite(0x3F8, chr);
}

void LoggingDriverSerial(const char* function, const char* filename, uint32_t line, uint64_t timestamp,
 							LogLevel logLevel, const char* format, va_list args)
{
	#pragma unused(filename)
	#pragma unused(line)
	#pragma unused(function)
	
	char* level;

	// Print level
	switch(logLevel) {
		case kLogLevelFatal:
			level = "\033[1;31m[F]\033[0m";
			break;
		case kLogLevelError:
			level = "\033[1;31m[E]\033[0m";
			break;
		case kLogLevelWarning:
			level = "\033[1;33m[W]\033[0m";
			break;
		case kLogLevelInfo:
			level = "\033[0;34m[I]\033[0m";
			break;
		case kLogLevelVerbose:
			level = "\033[0;34m[V]\033[0m";
			break;
		case kLogLevelTrace:
			level = "\033[0;34m[T]\033[0m";
			break;
	}

	pprintf(SerialPutchar, "\033[0;37m[%10d]\033[0m%s ", (uint32_t)timestamp, level);
	vpprintf(SerialPutchar, format, args);
	SerialPutchar('\n');
}

LoggingRegisterDriver(Serial, LoggingDriverSerial);
