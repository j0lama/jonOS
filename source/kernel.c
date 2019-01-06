#include "stdint.h"
#include "uart.h"
#include "interrupt.h"
#include "timer.h"
#include "string.h"
#include "framebuffer.h"
#include "gpu.h"
#include "network.h"
#include "uspios.h"

extern volatile uint32_t __binary_function;
extern volatile uint32_t __heap_start;



//arping 192.168.1.123 -I eno1 -c 6



void main(uint32_t r0, uint32_t r1, uint32_t atags)
{
	uint8_t IPAddress[] = {192, 168, 1, 123};
	uint8_t Gateway[] = {192, 168, 1, 1};
	uint8_t SubnetMask[] = {255, 255, 255, 0};

	uint8_t IPAddressPC[] = {192, 168, 1, 114};
	uint8_t	MACAddressReplayed[MAC_ADDRESS_SIZE];
	uint8_t MACDest[] = {0x74, 0x85, 0x2a, 0x1c, 0x68, 0xeb};
	uint8_t MACBroadcast[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	/*Declare as unused*/
	(void) r0;
	(void) r1;
	(void) atags;

	uart_init();
	//interrupts_init();
	//timer_init();

	int ret = networkInit(IPAddress, Gateway, SubnetMask);

	gpu_init(CHAR_SIZE_MEDIUM);

	if(ret != 0)
		console_puts(" Error in network\n\n");

	uart_puts("Hello from UART\r\n");
	//timer(5);
	set_foreground_color(WHITE);
	console_puts(" Welcome to jonOS\n\n");
	console_puts(" Screen base address: ");
	console_puts(uint2hex(framebuffer.screenbase));
	console_puts("\n\n Screen dimensions: ");
	console_puts(uint2dec(framebuffer.x));
	console_puts("x");
	console_puts(uint2dec(framebuffer.y));
	console_puts("\n\n Heap base address: ");
	console_puts(uint2hex((uint32_t)&__heap_start));
	console_puts("\n\n Payload base address: ");
	console_puts(uint2hex((uint32_t)&__binary_function));

	console_puts("\n\n\n Network configuration");
	console_puts("\n\n IP address: ");
	printIP(netConfiguration.IPAddress);
	console_puts("\n\n Gateway address: ");
	printIP(netConfiguration.GatewayAddress);
	console_puts("\n\n Subnet Mask: ");
	printIP(netConfiguration.SubnetMask);
	console_puts("\n\n MAC address: ");
	printMAC(netConfiguration.MACAddress);

	int flag = 0;
	while (1)
	{
		MsDelay(1000);
		ARPRequest(IPAddressPC, MACAddressReplayed);
		if(flag == 1)
		{
			set_foreground_color(GREEN);
			flag = 0;
		}
		else
		{
			set_foreground_color(RED);
			flag = 1;
		}
		console_puts("\n\n MAC Replayed: ");
		printMAC(MACAddressReplayed);
		set_foreground_color(WHITE);
	}

	while(1)
	{
		MsDelay(3000);
		sendMessage(MACBroadcast, "Arriba Espana");
		console_puts("\n\n Message sent");
	}

	uint8_t buffer[1600];
	size_t buffer_length;

	while(1) /*Test ARP Replay */
	{
		recv((void *) buffer, &buffer_length);
		console_puts("\n\n Unknown Message received");
	}
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

