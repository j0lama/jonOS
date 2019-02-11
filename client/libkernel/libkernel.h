#ifndef KERNEL_CALLS_H
#define KERNEL_CALLS_H
#include "stdint.h"
#include "stddef.h"


/*********/
/* CACHE */
/*********/

/* void cleanCaches() */
#define cleanCaches ((void(*)(void)) )


/*******/
/* GPU */
/*******/

/* void console_puts(char *text) */
#define console_puts ((void(*))((char*)) )

/* void set_foreground_color(unsigned short int color) */
#define set_foreground_color ((void(*))((unsigned short int)) )

/* void set_background_color(unsigned short int color) */
#define set_background_color ((void(*))((unsigned short int)) )


/********************/
/* Input Output (IO)*/
/********************/

/* void io_write(uint32_t addr, uint32_t data) */
#define io_write ((void(*))((uint32_t)(uint32_t)) )

/* uint32_t io_read(uint32_t addr) */
#define io_read ((uint32_t(*))((uint32_t)) )


/**********/
/* MALLOC */
/**********/

/* void * alloc_m(size_t size) */
#define alloc_m ((void*(*))((size_t)) )

/* void * calloc_m(size_t count, size_t size) */
#define calloc_m ((void*(*))((size_t)(size_t)) )

/* void free_m(void * block) */
#define free_m ((void(*))((void*)) )


/***********/
/* NETWORK */
/***********/

typedef struct networkConfig
{
	uint8_t IPAddress[4];
	uint8_t GatewayAddress[4];
	uint8_t SubnetMask[4];
	uint8_t MACAddress[MAC_ADDRESS_SIZE];
} networkConfig;

#define netConfiguration ((networkConfig) 0x246c8)

/* int sendUDP(uint8_t DestIP[], uint16_t DestPort, void * message, uint32_t message_length) */
#define sendUDP ((int(*)(uint8_t*)(uint16_t)(void*)(uint32_t)) )

/* int recv(uint16_t port, void * buffer, size_t buffer_length) */
#define recv ((int(*))((uint16_t)(void*)(size_t)) )

/* void printIP(uint8_t IPAddress[]) */
#define printIP ((void(*))((uint8_t*)) )

/* void printMAC(uint8_t MACAddress[]) */
#define printMAC ((void(*))((uint8_t*)) )

/* void printARPTable() */
#define printARPTable ((void(*))((void)) )

/* void dumpPacket(uint8_t * packet, uint32_t size) */
#define dumpPacket ((void(*))((uint8_t*)(uint32_t)))


/**********/
/* STDLIB */
/**********/

/* void memset(void * dest, register int c, register size_t len) */
#define memset ((void(*))((void*)(register int)(register size_t)) )

/* void memcpy(void * dest, const void * src, register size_t len) */
#define memcpy ((void(*))((void*)(const void*)(size_t)) )

/*int memcmp(const void * buffer1, const void * buffer2, size_t nLength)*/
#define memcmp ((int(*))((const void*)(const void*)(size_t)) )

/* void bzero(void * dest, register size_t len) */
#define bzero ((void(*))((void*)(register size_t)) )

/* int atoi(char * str) */
#define atoi ((int(*))((char*)) )


/**********/
/* STRING */
/**********/

/* char *uint2hex(uint32_t value) */
#define uint2hex ((char*(*))((uint32_t)) )

/* char *uint2dec(uint32_t value) */
#define uint2dec ((char*(*))((uint32_t)) )

/* char *char2hex(uint8_t value) */
#define char2hex ((char*(*))((uint8_t)) )

/* size_t strlen (const char *string) */
#define strlen ((size_t(*))((const char*)) )


/********/
/* UART */
/********/

/* void uart_putc(uint8_t c) */
#define uart_putc ((void(*))((uint8_t)) )

/* uint8_t uart_getc() */
#define uart_getc ((uint8_t(*))((void)) )

/* void uart_puts(char* str) */
#define uart_puts ((void(*)(char*)) )


/********************/
/* Integer division */
/********************/

/* int idiv(uint32_t N, uint32_t D) */
#define idiv ((int(*)(uint32_t)(uint32_t)) )


/**********/
/* STDDEF */
/**********/

#define NULL ((void *) 0)

typedef unsigned int size_t;


/**********/
/* STDINT */
/**********/

typedef unsigned char uint8_t;

typedef unsigned short uint16_t;

typedef unsigned int uint32_t;

typedef unsigned long long uint64_t;

typedef short int16_t;

typedef int int32_t;

typedef long int64_t; 

#endif