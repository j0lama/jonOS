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
void printMAC(uint8_t MACAddress[])
{
	uint8_t i;
	console_puts(char2hex(MACAddress[0]));
	for(i = 1; i < 6; i++)
	{
		console_puts(":");
		console_puts(char2hex((uint32_t)MACAddress[i]));
	}
}

void dumpPacket(uint8_t * packet, uint32_t size)
{
	console_puts("\n ");
	int i = 0;
	int j = 0;
	while(size--)
	{
		if(i == 16)
		{
			console_puts("\n ");
			i = 0;
		}
		console_puts(char2hex(packet[j]));
		console_puts(" ");
		i++;
		j++;
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
	uint8_t buffer[USPI_FRAME_BUFFER_SIZE];
	uint32_t buffer_length;
	ARPFrame arp_frame;
	ARPFrame * arp_reply;
	/*Building the Ethernet header*/
	memcpy(arp_frame.Ethernet.MACReceiver, BroadcastMAC, MAC_ADDRESS_SIZE);
	memcpy(arp_frame.Ethernet.MACSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE);
	arp_frame.Ethernet.ProtocolType = BE(ETHERNET_ARP);
	//Building the ARP Request message
	arp_frame.ARP.HardwareType = BE(HTYPE);
	arp_frame.ARP.ProtocolType = BE(IPV4);
	arp_frame.ARP.HardwareLength = MAC_ADDRESS_SIZE;
	arp_frame.ARP.ProtocolLength = IP_ADDRESS_SIZE;
	arp_frame.ARP.OPCode = BE(ARP_REQUEST); /*Request*/
	memcpy(arp_frame.ARP.MACAddressSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE); /*Our MAC*/
	memcpy(arp_frame.ARP.ProtocolAddressSender, netConfiguration.IPAddress, IP_ADDRESS_SIZE); /*Our IP*/
	memcpy(arp_frame.ARP.MACAddressTarget, BroadcastMAC, MAC_ADDRESS_SIZE);
	memcpy(arp_frame.ARP.ProtocolAddressTarget, IPAddress, IP_ADDRESS_SIZE); /*Requested IP*/

	/*Send the ARP Request*/
	sendFrame((const void *)&arp_frame, sizeof(arp_frame));

	console_puts("\n\n ARP Request sended");
	dumpPacket((uint8_t *)&arp_frame, sizeof(arp_frame));

	/*Wait to the ARP Replay*/
	while(1)
	{
		if(!recvFrame((void *)buffer, &buffer_length)) /*While no frame is available continue waiting for one*/
			continue;

		console_puts("\n\n Packet size: ");
		console_puts(uint2dec(buffer_length));

		arp_reply = (ARPFrame *) buffer;

		if(buffer_length <= 60)
		{
			dumpPacket((uint8_t *)arp_reply, buffer_length);
		}

		/*Check if its a ARP Reply packet*/
		if (   arp_reply->Ethernet.ProtocolType != BE(ETHERNET_ARP)
		    || arp_reply->ARP.HardwareType != BE(HTYPE)
		    || arp_reply->ARP.ProtocolType != BE(IPV4)
		    || arp_reply->ARP.HardwareLength != MAC_ADDRESS_SIZE
		    || arp_reply->ARP.ProtocolLength != IP_ADDRESS_SIZE
		    || arp_reply->ARP.OPCode != BE(ARP_REPLY))
		{
			console_puts("\n\n Invalid ARP Reply packet");
			continue;
		}

		console_puts("\n\n IP Replayed: ");
		printIP(arp_reply->ARP.ProtocolAddressSender);
		console_puts("\n\n MAC Replayed: ");
		printMAC(arp_reply->ARP.MACAddressSender);


	}

	/*Copy the frame to a ARPFrame structure*/

	return 0;
}