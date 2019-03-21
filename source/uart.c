#include "uart.h"
#include "stdint.h"
#include "io.h"
#include "gpio.h"

/* Loop delay times in a way that the compiler won't optimize away*/
static inline void delay(int32_t times)
{
	for(int i = 0; i < times; i++)
		asm volatile("nop");
}

void uart_init()
{
	// Disable UART0.
	io_write(UART0_CR, 0x00000000);
	// Setup the GPIO pin 14 && 15.
 
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	io_write(GPPUD, 0x00000000);
	delay(150);
 
	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
	io_write(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
 
	// Write 0 to GPPUDCLK0 to make it take effect.
	io_write(GPPUDCLK0, 0x00000000);
 
	// Clear pending interrupts.
	io_write(UART0_ICR, 0x7FF);
 
	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 115200.
 
	// Divider = 3000000 / (16 * 115200) = 1.627 = ~1.
	io_write(UART0_IBRD, 1);
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	io_write(UART0_FBRD, 40);
 
	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	io_write(UART0_LCRH, (1 << 4) | (1 << 5) | (1 << 6));
 
	// Mask all interrupts.
	io_write(UART0_IMSC, (1 << 1) | (1 << 4) | (1 << 5) | (1 << 6) |
	                       (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10));
 
	// Enable UART0, receive & transfer part of UART.
	io_write(UART0_CR, (1 << 0) | (1 << 8) | (1 << 9));
}

void uart_putc(uint8_t c)
{
	// Wait for UART to become ready to transmit.
	while ( io_read(UART0_FR) & (1 << 5) ) { }
	io_write(UART0_DR, c);
}
 
uint8_t uart_getc()
{
    // Wait for UART to have received something.
    while ( io_read(UART0_FR) & (1 << 4) ) { }
    return io_read(UART0_DR);
}
 
void uart_puts(char* str)
{
	while(*str != '\0')
		uart_putc((uint8_t)*str++);
}