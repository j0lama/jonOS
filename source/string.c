#include "stdint.h"
#include "stddef.h"
#include <stdlib.h>

char *uint2hex(uint32_t value)
{
	static char buffer[11];
	uint8_t offset;
	uint8_t c;
	offset=8;
	while(offset--)
	{
		c = '0' + (value & 0xF);
		if(c>=58) /*If its letter*/
			c+=7;
		buffer[2+offset] = c;
		value = value >> 4; /*Next digit*/
	}
	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[10] = 0;
	return buffer;
}

char *char2hex(uint8_t value)
{
	static char buffer[3];
	uint8_t offset;
	uint8_t c;
	offset=2;
	while(offset--)
	{
		c = '0' + (value & 0xF);
		if(c>=58) /*If its letter*/
			c+=7;
		buffer[offset] = c;
		value = value >> 4; /*Next digit*/
	}
	buffer[2] = 0;
	return buffer;
}

char *uint2dec(uint32_t value)
{
	/* Biggest number is 4294967295 (10 digits) */
	static char buffer[11];
	uint8_t offset = 10;

	while(value || (offset == 10))
	{
		offset--;
		buffer[offset] = 48 + (value % 10);;
		value = value/10;
	}
	buffer[10] = 0;
	return &buffer[offset];
}

size_t strlen (const char *string)
{
	size_t nResult = 0;
	while (*string++)
		nResult++;
	return nResult;
}

void strcpy(char * dest, const char * src)
{
	memcpy(dest, src, strlen(src)+1);
}