#ifndef NETWORK_H
#define NETWORK_H
#include "stdint.h"

#define MAC_ADDRESS_SIZE	6
#define IP_ADDRESS_SIZE		4
#define IPHEADER_SIZE	20
#define UDPHEADER_SIZE	8

typedef struct networkConfig
{
	uint8_t IPAddress[4];
	uint8_t GatewayAddress[4];
	uint8_t SubnetMask[4];
	uint8_t MACAddress[MAC_ADDRESS_SIZE];
} networkConfig;

networkConfig netConfiguration;

int networkInit();
void printIP(uint8_t IPAddress[]);

#endif