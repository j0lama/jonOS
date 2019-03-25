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

#define ANY_PORT 0


#define BZERO "bzero"
#define RECV "recv"
#define SENDUDP "sendUDP"
#define STRLEN "strlen"


/* Function used to solve all function address */
/* void * solveFunction(const char * name) */
#define solveFunction ( (void*(*)(const char *)) 0x85a0)
#define solveFunctionInt ( (void*(*)(int)) 0x8670)


#endif