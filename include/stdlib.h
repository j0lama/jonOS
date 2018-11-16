#ifndef STDLIB_H
#define STDLIB_H

#include "stddef.h"

void memset(void * dest, register int c, register size_t len);
void memcpy(void * dest, const void * src, register size_t len);
void bzero(void * dest, register size_t len);

#endif