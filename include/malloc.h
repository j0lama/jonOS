#ifndef MALLOC_H
#define MALLOC_H
#include "stddef.h"

void * malloc(size_t size);
void free(void * block);

#endif