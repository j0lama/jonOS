#include "io.h"

/*Used inline directive to avoid call a function and use the stack*/

/*Write data in addr (memory address)*/
void io_write(uint32_t addr, uint32_t data)
{
	*(volatile uint32_t*)addr = data;
}

/*Read memory*/
uint32_t io_read(uint32_t addr)
{
	return *(volatile uint32_t*)addr;
}