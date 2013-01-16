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

#ifndef __LOGGING_H__
#define __LOGGING_H__

#include <CoreSystem/CommonTypes.h>
#include <CoreSystem/Integers.h>
#include <CoreSystem/VariadicArguments.h>

//
// Inizalize the logging subsystem.
//
void LoggingInitialize();

//
// Logging
// ========
// 

typedef enum {
	kLogLevelFatal,
	kLogLevelError,
	kLogLevelWarning,
	kLogLevelInfo,
	kLogLevelVerbose,
	kLogLevelTrace
} LogLevel;

static LogLevel CurrentLogLovel = kLogLevelTrace;

//
// This is the default Log function which will relay the format to the log providers
// Note: These does not filter the messages based on the loglevel, this is done in the
//     macros provided.
//
void _Log(const char* function, const char* filename, uint32_t line, LogLevel logLevel, const char* format, ...) __attribute__ ((format (printf, 5, 6)));
void _Log_va(const char* function, const char* filename, uint32_t line, LogLevel logLevel, const char* format, va_list args);

//
// The Macros provided for use
//

#define _LogMacro(logLevel, format,ARGS...) if (logLevel <= CurrentLogLovel) { _Log(__FUNCTION__, __FILE__, __LINE__, logLevel, format, ##ARGS); }

#define LogFatal(format,ARGS...) _LogMacro(kLogLevelFatal, format,##ARGS)
#define LogError(format,ARGS...) _LogMacro(kLogLevelError, format,##ARGS)
#define LogWarning(format,ARGS...) _LogMacro(kLogLevelWarning, format,##ARGS)
#define LogInfo(format,ARGS...) _LogMacro(kLogLevelInfo, format,##ARGS)
#define LogVerbose(format,ARGS...) _LogMacro(kLogLevelVerbose, format,##ARGS)
#define LogTrace(format,ARGS...) _LogMacro(kLogLevelTrace, format,##ARGS)

//
// Log Provider
// =============
//

typedef void(*LogDriverLog)(const char* function, const char* filename, uint32_t line, uint64_t timestamp, LogLevel logLevel, const char* format, va_list args);

typedef struct LogDriver {
	char const* name;
	LogDriverLog log;
} LogDriver;

//
// Use this macro on the top level to staticly register a log provider at compile time
//
#define LoggingRegisterDriver(_name, driver) LogDriver LogDriver_##_name __attribute__ ((section (".LoggingDrivers"))) = {.name = #_name, .log = &driver}

#endif /* __LOGGING_H__ */
