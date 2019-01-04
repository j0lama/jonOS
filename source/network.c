/*This module is a layer for the USPi library that include high level network functions*/

#include "network.h"
#include "uspi.h"
#include "uspienv.h"
#include "uspios.h"
#include "uart.h"
#include "stdint.h"
#include "uspienv/macros.h"
#include "uspienv/types.h"
#include "gpu.h"
#include "string.h"
#include "stdlib.h"


typedef struct IPHeader
{
    uint8_t version:4, header_length:4;
    uint8_t type_of_service;
    uint16_t length;
    uint16_t identifier;
    uint16_t flags:4, offset:12;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;  //2 Byte
    uint32_t ip_src;        //4 Byte
    uint32_t ip_dst;        //4 Byte
} PACKED IPHeader;

typedef struct UDPHeader
{
	uint16_t source_port;
	uint16_t dest_port;
	uint16_t length;
	uint16_t checksum;
} PACKED UDPHeader;

typedef struct EthernetHeader
{
	uint8_t	MACReceiver[MAC_ADDRESS_SIZE];
	uint8_t	MACSender[MAC_ADDRESS_SIZE];
	uint16_t ProtocolType;
} PACKED EthernetHeader;

typedef struct ARPPacket
{
	uint16_t HardwareType;
	uint16_t ProtocolType;
	uint8_t	HardwareLength;
	uint8_t	ProtocolLength;
	uint16_t OPCode;
	uint8_t	MACAddressSender[MAC_ADDRESS_SIZE];
	uint8_t	ProtocolAddressSender[IP_ADDRESS_SIZE];
	uint8_t	MACAddressTarget[MAC_ADDRESS_SIZE];
	uint8_t	ProtocolAddressTarget[IP_ADDRESS_SIZE];
} PACKED ARPPacket;

typedef struct ARPFrame
{
	EthernetHeader Ethernet;
	ARPPacket	ARP;
} PACKED ARPFrame;

static const uint8_t BroadcastMAC[] = BROADCAST_MAC;

/*Auxiliary functions for debbugging prupose*/
void printIP(uint8_t IPAddress[])
{
	uint8_t i;
	console_puts(uint2dec(IPAddress[0]));
	for(i = 1; i < 4; i++)
	{
		console_puts(".");
		console_puts(uint2dec((uint32_t)IPAddress[i]));
	}
}

int networkInit(uint8_t IPAddress[], uint8_t GatewayAddress[], uint8_t SubnetMask[])
{
	if (!USPiEnvInitialize()) /*Error initializing the enviroment required by USPi*/
	{
		uart_puts("Error initializing the enviroment required by USPi\r\n");
		return 1;
	}
	if (!USPiInitialize())
	{
		uart_puts("Error initializing the USPi lib\r\n");
		USPiEnvClose(); /*CLose the enviroment*/
		return 1;
	}
	if (!USPiEthernetAvailable()) /*Check if ethernet device is available*/
	{
		uart_puts("Ethernet device is not found\r\n");
		USPiEnvClose (); /*CLose the enviroment*/
		return 1;
	}
	/*Network configuration setup*/
	memcpy(netConfiguration.IPAddress, IPAddress, 4); /*Copy the IP Address. Must be a valid IP in the LAN*/
	memcpy(netConfiguration.GatewayAddress, GatewayAddress, 4);
	memcpy(netConfiguration.SubnetMask, SubnetMask, 4);
	USPiGetMACAddress(netConfiguration.MACAddress); /*Get the MAC address of our Raspberry*/

	unsigned nTimeout = 0;
	while (!USPiEthernetIsLinkUp()) /*Wait until the Ethernet link is up*/
	{
		MsDelay(100);
		if (++nTimeout < 40)
			continue;
		nTimeout = 0;
		uart_puts("Link is down\r\n");
	}
	return 0;
}

int sendFrame(const void * buffer, uint32_t buffer_length)
{
	/* Returns 0 on failure */
	return USPiSendFrame(buffer, buffer_length);
}

int recvFrame(void * buffer, uint32_t * buffer_length)
{
	/* Returns 0 if no frame is available or on failure */
	return USPiReceiveFrame (buffer, buffer_length);
}

int ARPRequest(uint8_t IPAddress[]/*, uint8_t DestMAC[]*/)
{
	ARPFrame arp_frame;
	/*Building the Ethernet header*/
	memcpy(arp_frame.Ethernet.MACReceiver, BroadcastMAC, MAC_ADDRESS_SIZE);
	memcpy(arp_frame.Ethernet.MACSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE);
	arp_frame.Ethernet.ProtocolType = ETHERNET_ARP;
	//Building the ARP Request message
	arp_frame.ARP.HardwareType = HTYPE;
	arp_frame.ARP.ProtocolType = IPV4;
	arp_frame.ARP.HardwareLength = MAC_ADDRESS_SIZE;
	arp_frame.ARP.ProtocolLength = IP_ADDRESS_SIZE;
	arp_frame.ARP.OPCode = ARP_REQUEST; /*Request*/
	memcpy(arp_frame.ARP.MACAddressSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE); /*Our MAC*/
	memcpy(arp_frame.ARP.ProtocolAddressSender, netConfiguration.IPAddress, IP_ADDRESS_SIZE); /*Our IP*/
	bzero(arp_frame.ARP.MACAddressTarget, MAC_ADDRESS_SIZE);
	memcpy(arp_frame.ARP.ProtocolAddressTarget, IPAddress, IP_ADDRESS_SIZE); /*Requested IP*/

	/*Send the ARP Request*/
	sendFrame((const void *)&arp_frame, sizeof(arp_frame));
	return 0;
}