/*Based on GCC Glibc implementation implementation*/

#include "stddef.h"
#include "stdint.h"

void memset(void * dest, register int c, register size_t len)
{
	register uint8_t *ptr = (uint8_t*)dest;
  	while (len-- > 0)
    	*ptr++ = c;
}

/* Auxiliary functions to speed-up the memcpy function if size is 4*N or 2*N */

void __memcpy_mod4__(void * dest, const void * src, register size_t len)
{
	register uint32_t *d = (uint32_t*)dest;
    register uint32_t *s = (uint32_t*)src;
    while (len-- > 0) {
        *d++ = *s++;
    }
}

void __memcpy_mod2__(void * dest, const void * src, register size_t len)
{
	register uint16_t *d = (uint16_t*)dest;
    register uint16_t *s = (uint16_t*)src;
    while (len-- > 0) {
        *d++ = *s++;
    }
}

void __memcpy_modN__(void * dest, const void * src, register size_t len)
{
	register uint8_t *d = (uint8_t*)dest;
    register uint8_t *s = (uint8_t*)src;
    while (len-- > 0) {
        *d++ = *s++;
    }
}

void memcpy(void * dest, const void * src, register size_t len)
{
	if(len % 4 == 0)
		__memcpy_mod4__(dest, src, len/4);
	else if(len % 2 == 0)
		__memcpy_mod2__(dest, src, len/2);
	else
		__memcpy_modN__(dest, src, len);
}

int memcmp(const void * buffer1, const void * buffer2, size_t nLength)
{
	const uint8_t *p1 = (const uint8_t *) buffer1;
	const uint8_t *p2 = (const uint8_t *) buffer2;
	
	while (nLength--)
	{
		if (*p1 != *p2)
			return 1;
		p1++;
		p2++;
	}

	return 0;
}

void bzero(void * dest, register size_t len)
{
    memset(dest, 0, len);
}

int atoi(char * str)
{
	int i = 0;
	int res = 0;
	int negative = 1;
	if(str[0] == '-')
	{
		negative = -1;
		i = 1;
	}
	for(; str[i] != '\0'; i++)
	{
		res = 10*res + str[i] - '0';
	}

	return res*negative;
}
