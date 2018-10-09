#include "stdint.h"
#include "uart.h"

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	// Declare as unused
	(void) r0;
	(void) r1;
	(void) atags;

	uint8_t value = 0;
 
	uart_init();
	uart_puts("Me voy a cagar en Dios como no funcione!\r\n");
 
	while (1)
	{
		value = uart_getc();
		if (value == 'n')
			uart_puts("Next\r\n");
		else if (value == 'b')
			uart_puts("Back\r\n");
		else
			uart_puts("Unknown\r\n");
	}
}

