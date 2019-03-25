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
#define ANY_PORT 0

#define CLEANCACHES 0

/*******/
/* GPU */
/*******/
#define CONSOLE_PUTS 1
#define SET_FOREGROUND_COLOR 2
#define SET_BACKGROUND_COLOR 3

/********************/
/* Input Output (IO)*/
/********************/
#define IO_WRITE 4
#define IO_READ 5

/**********/
/* MALLOC */
/**********/
#define ALLOC_M 6
#define CALLOC_M 7
#define FREE_M 8

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
#define NETCONFIGURATION 9
#define SENDUDP 10
#define RECV 11
#define PRINTIP 12
#define PRINTMAC 13
#define PRINTARPTABLE 14
#define DUMPPACKET 15

/**********/
/* STDLIB */
/**********/
#define MEMSET 16
#define MEMCPY 17
#define MEMCMP 18
#define BZERO 19
#define ATOI 20

/**********/
/* STRING */
/**********/
#define UINT2HEX 21
#define UINT2DEC 22
#define CHAR2HEX 23
#define STRLEN 24
#define STRCPY 25
#define STRCMP 26

/********/
/* UART */
/********/
#define UART_PUTC 27
#define UART_GETC 28
#define UART_PUTS 29

/********************/
/* Integer division */
/********************/
#define IDIV 30


/* Function used to solve all function address */
/* void * solveFunction(const char * name) */
#define solveFunction ( (void*(*)(const char *)) 0x85a0)
void * solveFunctionInt(int pos)
#define solveFunctionInt ( (void*(*)(int)) 0x8670)


#endif