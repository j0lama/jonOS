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

void bzero(void * dest, register size_t len)
{
    memset(dest, 0, len);
}
