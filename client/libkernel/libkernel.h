#ifndef KERNEL_CALLS_H
#define KERNEL_CALLS_H
#include "stdint.h"
#include "stddef.h"


/* 	UART
	Funtions to use the UART
	By default the kernel initialize the UART
*/
#define uart_init ((void(*))()0x80ec)
#define uart_putc ((void(*))(uint8_t)0x81b0)
#define uart_getc ((uint8_t(*))()0x8204)
#define uart_puts ((void(*)(char*))0x8248)

/*	MALLOC
	Functions to allocate and free chuncks of memory
*/
#define malloc ((void*(*))(size_t)0x83a4)
#define free ((void(*))(void *)0x8498)

/* NETWORK */
#define sendUDP ((int(*))((uint8_t*)(uint16_t)(void*)(uint32_t))0xb178)

/* SCREEN */
#define console_puts ((void(*)(char*))0x8bd4)



/*	VARIABLES
	Auxiliary variables for tests
*/
#define msg ((char *) 0x7fb8)


#endif