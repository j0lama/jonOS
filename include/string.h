#ifndef STRING_H
#define STRING_H
#include "stddef.h"

char *uint2hex(uint32_t value);
char *char2hex(uint8_t value);
char *uint2dec(uint32_t value);
size_t strlen (const char *string);
void strcpy(char * dest, const char * src);
int strcmp(const char * str1, const char * str2);

#endif