#ifndef IO_H
#define IO_H

#include "stdint.h"

/*Basics functions for access the memory*/

void io_write(uint32_t addr, uint32_t data);
uint32_t io_read(uint32_t addr);

#endif