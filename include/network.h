#ifndef NETWORK_H
#define NETWORK_H
#include "stdint.h"

#define ETHERNET_ARP 0x806
#define IPV4 0x800
#define HTYPE 1
#define ARP_REQUEST	1
#define ARP_REPLY 2
#define BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
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
int ARPRequest(uint8_t IPAddress[]/*, uint8_t DestMAC[]*/);

#endif