#ifndef NETWORK_H
#define NETWORK_H
#include "stdint.h"
#include "stddef.h"

#define ETHERNET_ARP 0x806
#define IPV4 0x800
#define HTYPE 1
#define ARP_REQUEST	1
#define ARP_REPLY 2
#define BROADCAST_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define MAC_ADDRESS_SIZE	6
#define IP_ADDRESS_SIZE		4
#define IP_HEADER_SIZE	20
#define UDP_HEADER_SIZE	8
#define ETHERNET_HEADER_SIZE 14
#define IP_IHL_VERSION 0x45
#define IP_TOS 0x00 /*This can be changed to modify the priority*/
#define IP_ID 1
#define IP_TTL 0xEF
#define UDP_DEFAULT_PORT 1337

/*Other protocol identifiers*/
/*https://en.wikipedia.org/wiki/List_of_IP_protocol_numbers*/
#define ICMP_PROTOCOL_ID 0x1
#define UDP_PROTOCOL_ID 0x11
#define TCP_PROTOCOL_ID 0x6
/*https://tools.ietf.org/html/rfc3692#section-2.1*/
#define CUSTOM_PROTOCOL_ID 0xFC
#define IP_DATA_PROTOCOL UDP_PROTOCOL_ID
#define IP_DATAGRAM_SIZE USPI_FRAME_BUFFER_SIZE - ETHERNET_HEADER_SIZE

#define ANY_PORT 0


typedef struct networkConfig
{
	uint8_t IPAddress[4];
	uint8_t GatewayAddress[4];
	uint8_t SubnetMask[4];
	uint8_t MACAddress[MAC_ADDRESS_SIZE];
} networkConfig;

networkConfig netConfiguration;

int networkInit();
int ARPRequest(uint8_t IPAddress[], uint8_t * DestMAC);
//PACKET_TYPE recv(void * buffer, size_t * buffer_length);

/*TEST*/
int sendIP(uint8_t DestIP[], void * segment, uint32_t segment_lenght);
int sendUDP(uint8_t DestIP[], uint16_t DestPort, void * message, uint32_t message_length);
int recv(uint16_t port, void * buffer, size_t buffer_length);

/*Debugging functions*/
void printIP(uint8_t IPAddress[]);
void printMAC(uint8_t MACAddress[]);
void printARPTable();
void dumpPacket(uint8_t * packet, uint32_t size);

#endif