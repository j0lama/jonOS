#ifndef MALLOC_H
#define MALLOC_H
#include "stddef.h"

void dinamic_mem_init();
void * alloc_m(size_t size);
void free_m(void * block);

#endif