#include "stdint.h"
#include "uart.h"

extern volatile uint8_t __binary_function;

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	/*Declare as unused*/
	(void) r0;
	(void) r1;
	(void) atags;

	uint32_t i = 0;
	void (*f)() = (void(*)()) __binary_function;
	uint8_t * value = (uint8_t *) __binary_function;

	uart_init();

	for(i = 0; i < 186; i++)
	{
		*(value + i) = uart_getc();
		uart_putc(*(value + i));
	}

	f();

}

