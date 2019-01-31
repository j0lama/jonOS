#ifndef MALLOC_H
#define MALLOC_H
#include "stddef.h"

void dinamic_mem_init();
uint8_t * getBRK();
void * alloc_m(size_t size);
void * calloc_m(size_t count, size_t size);
void free_m(void * block);

#endif