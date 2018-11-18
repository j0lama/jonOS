#include "stdint.h"
#include "uart.h"
#include "interrupt.h"
#include "timer.h"
#include "gpu.h"

extern volatile uint32_t __binary_function;

void main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	/*Declare as unused*/
	(void) r0;
	(void) r1;
	(void) atags;

	uart_init();
	gpu_init();
	interrupts_init();
	timer_init();


	uart_puts("Hello from UART\r\n");
	timer(5);
	set_background_color(RED);
	console_puts("Hola que esta pasando aqui\nEsto tiene pinta de funcionar\nAhora a porbar\tmas cosas xD");

	while (1);
	/*
	uint8_t code [] = "\x70\x40\x2D\xE9\x0C\x50\x9F\xE5\x0C\x40\x9F\xE5\x05\x00\xA0\xE1\x34\xFF\x2F\xE1\xFC\xFF\xFF\xEA\xB8\x7F\x00\x00\x48\x82\x00\x00";

	uint8_t i = 0;
	void (*f)() = (void(*)()) &__binary_function;
	uint8_t * value = (uint8_t *) &__binary_function;

	uart_init();

	for(i = 0; i < 32; i++)
	{
		*(volatile uint8_t*)(value + i) = code[i];
	}

	f();*/

}

