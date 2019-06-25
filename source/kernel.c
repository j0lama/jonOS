#include "stdint.h"
#include "stdlib.h"
#include "uart.h"
#include "interrupt.h"
#include "timer.h"
#include "string.h"
#include "framebuffer.h"
#include "gpu.h"
#include "network.h"
#include "uspios.h"
#include "malloc.h"
#include "cache.h"
#include "address_solver.h"

extern volatile uint32_t __address_solver;
extern volatile uint32_t __heap_start;

void main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	/*Declare as unused*/
	(void) r0;
	(void) r1;
	(void) atags;

	/* Network configuration */
	uint8_t IPAddress[] = {123, 123, 123, 123};
	uint8_t Gateway[] = {123, 123, 123, 1};
	uint8_t SubnetMask[] = {255, 255, 255, 0};
	uint8_t msgPayloadLen[32];
	uint8_t * payload = NULL;
	int payloadLen = 0;
	int (*f)();

	/* Init UART */
	uart_init();
	/* Init Interruption manager */
	//interrupts_init();

	/* Init timer */
	//timer_init();

	/*Init dinamic memory*/
	dinamic_mem_init();

	/* Init address solver */
	init_address_solver();

	/* Init network stack */
	int ret = networkInit(IPAddress, Gateway, SubnetMask);

	/* Init GPU and screen */
	gpu_init(CHAR_SIZE_BIG);

	if(ret != 0)
		console_puts(" Error in network\n\n");

	/* Debug message from UART */
	uart_puts("Hello from UART\r\n");

	/* Setting up the screen colors */
	set_foreground_color(YELLOW);

	/* Printing system information */
	console_puts(" Welcome to jonOS\n\n");
	//console_puts(" Screen base address: ");
	//console_puts(uint2hex(framebuffer.screenbase));
	console_puts("\n\n Screen dimensions: ");
	console_puts(uint2dec(framebuffer.x));
	console_puts("x");
	console_puts(uint2dec(framebuffer.y));
	console_puts("\n\n Heap base address: ");
	console_puts(uint2hex((uint32_t)&__heap_start));
	console_puts("\n\n Payload base address: ");
	console_puts(uint2hex((uint32_t)&__address_solver));
	/* Printing network configuration */
	console_puts("\n\n\n Network configuration");
	console_puts("\n\n IP address: ");
	printIP(netConfiguration.IPAddress);
	console_puts("\n\n Gateway address: ");
	printIP(netConfiguration.GatewayAddress);
	console_puts("\n\n Subnet Mask: ");
	printIP(netConfiguration.SubnetMask);
	console_puts("\n\n MAC address: ");
	printMAC(netConfiguration.MACAddress);

	/* Setting up the screen colors */
	set_foreground_color(WHITE);
	int value = 0;
	/* Loop that waits for the payload */
	while(1)
	{
		/* Clean the buffer */
		bzero(msgPayloadLen, 32);
		/* Receive the payload length*/
		recv(ANY_PORT, msgPayloadLen, 32);

		/* Converts to number the payload length */
		payloadLen = atoi((char *)msgPayloadLen);

		/*Alloc memory for the payload*/
		payload = alloc_m(payloadLen);

		/* Cleaning cache */
		cleanCaches();

		/* Receive the payload */
		recv(ANY_PORT, payload, payloadLen);

		/* Converts the payload to a function*/
		f = (void(*)(uint8_t*,size_t)) payload;

		/* Print the fucntion address */
		console_puts("\n\n Client Payload at ");
		console_puts(uint2hex((uint32_t)f));
		console_puts("\n\n");

		/* Execute the payload */
		value = f();
		console_puts("\n\n Payload return: ");
		console_puts(uint2dec(value));

		/*Free the memory*/
		free_m(payload);
	}

}

