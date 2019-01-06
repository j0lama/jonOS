/*Based on GCC Glibc implementation implementation*/

#include "stddef.h"

void memset(void * dest, register int c, register size_t len)
{
	register unsigned char *ptr = (unsigned char*)dest;
  	while (len-- > 0)
    	*ptr++ = c;
}

void memcpy(void * dest, const void * src, register size_t len)
{
    register unsigned char *d = (unsigned char*)dest;
    register unsigned char *s = (unsigned char*)src;
    while (len-- > 0) {
        *d++ = *s++;
    }
}

int memcmp(const void * buffer1, const void * buffer2, size_t nLength)
{
	const unsigned char *p1 = (const unsigned char *) buffer1;
	const unsigned char *p2 = (const unsigned char *) buffer2;
	
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
