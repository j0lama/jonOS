#ifndef KERNEL_CALLS_H
#define KERNEL_CALLS_H

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


/*********/
/* CACHE */
/*********/

/* void cleanCaches() */
#define cleanCaches ((void(*)(void)) 0xa088)


/*******/
/* GPU */
/*******/

/* void console_puts(char *text) */
#define console_puts ((void(*))((char*)) 0x9c10)

/* void set_foreground_color(unsigned short int color) */
#define set_foreground_color ((void(*))((unsigned short int)) 0x96f8)

/* void set_background_color(unsigned short int color) */
#define set_background_color ((void(*))((unsigned short int)) 0x973c)


/********************/
/* Input Output (IO)*/
/********************/

/* void io_write(uint32_t addr, uint32_t data) */
#define io_write ((void(*))((uint32_t)(uint32_t)) 0x8390)

/* uint32_t io_read(uint32_t addr) */
#define io_read ((uint32_t(*))((uint32_t)) 0x83c0)


/**********/
/* MALLOC */
/**********/

/* void * alloc_m(size_t size) */
#define alloc_m ((void*(*))((size_t)) 0x9dd4)

/* void * calloc_m(size_t count, size_t size) */
#define calloc_m ((void*(*))((size_t)(size_t)) 0x9ed0)

/* void free_m(void * block) */
#define free_m ((void(*))((void*)) 0x9f38)


/***********/
/* NETWORK */
/***********/

typedef struct networkConfig
{
	uint8_t IPAddress[4];
	uint8_t GatewayAddress[4];
	uint8_t SubnetMask[4];
	uint8_t MACAddress[6];
} networkConfig;

#define netConfiguration ((networkConfig) 0x246b8)

/* int sendUDP(uint8_t DestIP[], uint16_t DestPort, void * message, uint32_t message_length) */
#define sendUDP ((int(*)(uint8_t*)(uint16_t)(void*)(uint32_t)) 0xb548)

/* int recv(uint16_t port, void * buffer, size_t buffer_length) */
#define recv ((int(*))((uint16_t)(void*)(size_t)) 0xb6f0)

/* void printIP(uint8_t IPAddress[]) */
#define printIP ((void(*))((uint8_t*)) 0xa280)

/* void printMAC(uint8_t MACAddress[]) */
#define printMAC ((void(*))((uint8_t*)) 0xa314)

/* void printARPTable() */
#define printARPTable ((void(*))((void)) 0xa47c)

/* void dumpPacket(uint8_t * packet, uint32_t size) */
#define dumpPacket ((void(*))((uint8_t*)(uint32_t)) 0xa3a8)


/**********/
/* STDLIB */
/**********/

/* void memset(void * dest, register int c, register size_t len) */
#define memset ((void(*))((void*)(register int)(register size_t)) 0x816c)

/* void memcpy(void * dest, const void * src, register size_t len) */
#define memcpy ((void(*))((void*)(const void*)(size_t)) 0x81b8)

/*int memcmp(const void * buffer1, const void * buffer2, size_t nLength)*/
#define memcmp ((int(*))((const void*)(const void*)(size_t)) 0x8214)

/* void bzero(void * dest, register size_t len) */
#define bzero ((void(*))((void*)(register size_t)) 0x82a0)

/* int atoi(char * str) */
#define atoi ((int(*))((char*)) 0x82d0)


/**********/
/* STRING */
/**********/

/* char *uint2hex(uint32_t value) */
#define uint2hex ((char*(*))((uint32_t)) 0xb86c)

/* char *uint2dec(uint32_t value) */
#define uint2dec ((char*(*))((uint32_t)) 0xba00)

/* char *char2hex(uint8_t value) */
#define char2hex ((char*(*))((uint8_t)) 0xb94c)

/* size_t strlen (const char *string) */
#define strlen ((size_t(*))((const char*)) 0xbad4)


/********/
/* UART */
/********/

/* void uart_putc(uint8_t c) */
#define uart_putc ((void(*))((uint8_t)) 0x850c)

/* uint8_t uart_getc() */
#define uart_getc ((uint8_t(*))((void)) 0x8560)

/* void uart_puts(char* str) */
#define uart_puts ((void(*)(char*)) 0x85a4)


/********************/
/* Integer division */
/********************/

/* int idiv(uint32_t N, uint32_t D) */
#define idiv ((int(*)(uint32_t)(uint32_t)) 0xbd58)

#endif