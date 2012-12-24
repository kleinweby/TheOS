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

#include <CoreSystem/String.h>

static char numberDefinitions[] = "0123456789ABCDEF";
static char *trueString = "true";
static char *falseString = "false";

bool isDigit(char c) {
	if (c >= '0' && c <= '9')
		return true;
	
	return false;
}

size_t strlen(char const* string)
{
	size_t length = 0;
	
	while (*string != '\0') {
		string++;
		length++;
	}
	
	return length;
}

void formatNumber(VPrintfPutChar putchar, uint32_t number, uint8_t base, uint32_t minLength, bool useWhitespacePadding)
{		
	// We only support base 2 to 16
	if (base < 2 || base > 16)
		return;
	
	char buffer[25];
	char* tempString = buffer;
	uint32_t usedLength = 0;
	
	// First we put the number in in reverse to
	// calculate the needed space and the number
	// itself
	do {
		uint8_t remainder;
		
		remainder = number % base;
		number = number / base;
		
		*tempString = numberDefinitions[remainder];
		
		usedLength++;
		tempString++;
	} while(number > 0);
	
	// Now we know the needed length and
	// have the number in a reverse format aviaible
	
	// Now padd the thing.
	for (; usedLength < minLength; usedLength++) {
		if (useWhitespacePadding)
			*tempString = ' ';
		else
			*tempString = '0';
		tempString++;
	}
	
	tempString--;
	
	// Now swap the thing around
	for (uint8_t i = 0; i < usedLength; i++, tempString--) {
		putchar(*tempString);
	}

	return;
}

void snprintf(char *string, size_t maxStringSize, char const* format, ...)
{
	va_list args;
	
	va_start(args, format);
	vsnprintf(string, maxStringSize, format, args);
	va_end(args);
}

void pprintf(VPrintfPutChar putchar, char const* format, ...)
{
	va_list args;
	
	va_start(args, format);
	vpprintf(putchar, format, args);
	va_end(args);
}

void vsnprintf(char *string, size_t maxStringSize, char const* format, va_list args)
{
	#pragma unused(string, maxStringSize, format, args)
	// void putchar(char c)
	// {
	// 	if (maxStringSize > 0) {
	// 		*string = c;
	// 		string++;
	// 		maxStringSize--;
	// 	}
	// }
	// 
	// vpprintf(putchar, format, args);
	// 
	// // We used all aviable buffer
	// // but wee need to isnert an \0
	// // so replace the last char
	// if (maxStringSize == 0) {
	// 	// Go one back
	// 	string--;
	// 	maxStringSize++;
	// }
	// 
	// *string = '\0';
}

void putstr(VPrintfPutChar putchar, char* str) {
	while (*str != '\0') {
		putchar(*str);
		str++;
	}
}

void vpprintf(VPrintfPutChar putchar, char const* format, va_list args)
{	
	while (*format != '\0') {
		// We have a format specifier here
		// so we need to evaluate it.
		if (*format == '%') {
			format++;
			
			if (*format == '%') {
				putchar('%');
			}
			// It is in deed an format
			else {
				bool useWhitespacePadding = true;
				uint32_t minLength = 0;
				int32_t precision = -1;
				
				// Read the length specifier
				// if there is any.
				while (isDigit(*format)) {
					// An leading 0 means we should padd with
					// 0 instand of whitespaces (if it is an numeric
					// format)
					if (minLength == 0 && *format == '0') {
						useWhitespacePadding = false;
					} else {
						minLength = minLength * 10 + ((uint32_t)*format - '0');
					}
					format++;
				}
				
				// The precision is specified
				if (*format == '.') {
					format++;
					precision = 0;
					
					while (isDigit(*format)) {
						precision = precision * 10 + (*format - '0');
						format++;
					}
				}
				
				// Now we've parsed away the formatting options
				// let look at the format
				switch(*format) {
					// Pointer
					case 'p':
						minLength = 8;
						useWhitespacePadding = false;
						putchar('*');
					// Hex number
					case 'X':
					case 'x':
					{
						uint32_t val = va_arg(args, uint32_t);
						putchar('0');
						putchar('x');
						formatNumber(putchar, val, 16, minLength, useWhitespacePadding);
						break;
					}
					// Signed integer
					case 'd':
					case 'i':
					{
						int32_t val = va_arg(args, int32_t);
						if (val < 0) {
							putchar('-');
							val = -val;
						}
						
						formatNumber(putchar, (uint32_t)val, 10, minLength, useWhitespacePadding);
						break;
					}
					// Unsinged integer
					case 'u':
					{
						uint32_t val = va_arg(args, uint32_t);
						formatNumber(putchar, val, 10, minLength, useWhitespacePadding);
						break;
					}
					// Boolean
					case 'B':
					{
						uint32_t val = va_arg(args, uint32_t);
						
						if (val == true) {
							putstr(putchar, trueString);
						}
						else {
							putstr(putchar, falseString);
						}
						break;
					}
					// String
					case 's':
					{
						char* str = va_arg(args, char*);

						putstr(putchar, str);
						break;
					}
					default:
						// Don't know the format. so we ignore it.
						break;
				}
			}
		}
		else {
			putchar(*format);
		}
		
		format++;
	}
}
