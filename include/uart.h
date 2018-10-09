#ifndef UART_H
#define UART_H

#include "stdint.h"

/*Funtions to access the UART (GPIO 14 as TX and GPIO 15 RX)*/

void uart_init();
void uart_putc(uint8_t c);
uint8_t uart_getc();
void uart_puts(char* str);

#endif