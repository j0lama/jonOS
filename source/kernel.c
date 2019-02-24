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

extern volatile uint32_t __binary_function;
extern volatile uint32_t __heap_start;

void main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	/*Declare as unused*/
	(void) r0;
	(void) r1;
	(void) atags;

	/* Network configuration */
	uint8_t IPAddress[] = {192, 168, 1, 123};
	uint8_t Gateway[] = {192, 168, 1, 1};
	uint8_t SubnetMask[] = {255, 255, 255, 0};
	uint8_t msgPayloadLen[32];
	uint8_t * payload = NULL;
	int ntimes = 0; /* Number of executions */
	int payloadLen = 0;
	void (*f)(uint8_t*,size_t);
	char *msg = "jon";

	/* Init UART */
	uart_init();
	/* Init Interruption manager */
	//interrupts_init();

	/* Init timer */
	//timer_init();

	/*Init dinamic memory*/
	dinamic_mem_init();

	/* Init network stack */
	int ret = networkInit(IPAddress, Gateway, SubnetMask);

	/* Init GPU and screen */
	gpu_init(CHAR_SIZE_MEDIUM);

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
	//console_puts("\n\n Screen dimensions: ");
	//console_puts(uint2dec(framebuffer.x));
	//console_puts("x");
	//console_puts(uint2dec(framebuffer.y));
	//console_puts("\n\n Heap base address: ");
	//console_puts(uint2hex((uint32_t)&__heap_start));
	//console_puts("\n\n Payload base address: ");
	//console_puts(uint2hex((uint32_t)&__binary_function));
	///* Printing network configuration */
	//console_puts("\n\n\n Network configuration");
	//console_puts("\n\n IP address: ");
	//printIP(netConfiguration.IPAddress);
	//console_puts("\n\n Gateway address: ");
	//printIP(netConfiguration.GatewayAddress);
	//console_puts("\n\n Subnet Mask: ");
	//printIP(netConfiguration.SubnetMask);
	//console_puts("\n\n MAC address: ");
	//printMAC(netConfiguration.MACAddress);

	/* Setting up the screen colors */
	set_foreground_color(WHITE);

	/* Loop that waits for the payload */
	while(1)
	{
		/* Clean the buffer */
		bzero(msgPayloadLen, 32);
		/* Receive the payload length*/
		recv(ANY_PORT, msgPayloadLen, 32);

		/* Converts to number the payload length */
		payloadLen = atoi((char *)msgPayloadLen);

		/* Print the lenght */
		//console_puts("\n\n payloadLen: ");
		//console_puts(uint2dec((uint32_t)payloadLen));

		/* Receive ntimes value */
		/* Clean the buffer */
		bzero(msgPayloadLen, 32);
		/* Receive the payload length*/
		recv(ANY_PORT, msgPayloadLen, 32);
		/* Converts to number the payload length */
		ntimes = atoi((char *)msgPayloadLen);
		/* Print the lenght */
		//console_puts("\n\n Ntimes: ");
		//console_puts(uint2dec((uint32_t)ntimes));


		/*Alloc memory for the payload*/
		payload = alloc_m(payloadLen);

		/* Cleaning cache */
		cleanCaches();

		/* Receive the payload */
		recv(ANY_PORT, payload, payloadLen);

		/* Converts the payload to a function*/
		f = (void(*)(uint8_t*,size_t)) payload;

		/* Print the fucntion address */
		console_puts("\n\n Function address: ");
		console_puts(uint2hex((uint32_t)f));

		/* Send the start flag*/
		uart_puts("S");

		/* Execute the function */
		while(ntimes--)
			f((uint8_t *)msg, 3);

		/* Send the end flag*/
		uart_puts("E");

		console_puts("\n\n Done");

		/*Free the memory*/
		free_m(payload);
		//sendUDP(IPAddressPC, 12345, answer, strlen(answer));
		//console_puts("\n\n Answer sent");
	}

}

