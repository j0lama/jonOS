/*This module is a layer for the USPi library that include high level network functions*/

#include "network.h"
#include "uspi.h"
#include "uspienv.h"
#include "uspios.h"
#include "uart.h"
#include "stdint.h"
#include "stddef.h"
#include "uspienv/macros.h"
#include "uspienv/types.h"
#include "gpu.h"
#include "string.h"
#include "stdlib.h"

#define ARP_TABLE_LEN	16


typedef struct IPHeader
{
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t length;
    uint16_t identifier;
    uint16_t flags:4, offset:12;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t ip_src;
    uint32_t ip_dst;
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


/*ARP Table structure and functions*/
typedef struct ARPTable
{
	uint8_t IP[ARP_TABLE_LEN][IP_ADDRESS_SIZE];
	uint8_t MAC[ARP_TABLE_LEN][MAC_ADDRESS_SIZE];
	uint8_t position;
	Boolean full;
} ARPTable;
ARPTable arp_table;

uint8_t * getMACAddress(uint8_t IPAddress[])
{
	uint8_t i, limit;
	if(arp_table.full == TRUE)
		limit = ARP_TABLE_LEN;
	else
		limit = arp_table.position;

	for(i = 0; i < limit; i++)
	{
		if(memcmp(arp_table.IP[i], IPAddress, IP_ADDRESS_SIZE) == 0)
			return arp_table.MAC[i];
	}
	return NULL;
}

void setMACAddress(uint8_t IPAddress[], uint8_t MACAddress[])
{
	memcpy(arp_table.IP[arp_table.position], IPAddress, IP_ADDRESS_SIZE);
	memcpy(arp_table.MAC[arp_table.position], MACAddress, MAC_ADDRESS_SIZE);
	arp_table.position++;
	if(arp_table.position >= 16)
	{
		arp_table.position = 0;
		arp_table.full = TRUE;
	}
}


static const uint8_t BroadcastMAC[] = BROADCAST_MAC;

/*Auxiliary functions for debugging prupose*/
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

void printARPTable()
{
	uint8_t i, limit;
	if(arp_table.full == TRUE)
		limit = ARP_TABLE_LEN;
	else
		limit = arp_table.position;

	console_puts("\n\n ARP Table\n ");

	for(i = 0; i < limit; i++)
	{
		printIP(arp_table.IP[i]);
		console_puts(" -> ");
		printMAC(arp_table.MAC[i]);
		console_puts("\n ");
	}
	console_puts("\n");
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

	/*Init the ARP Table to zeros*/
	bzero((void *)&arp_table, sizeof(ARPTable));

	return 0;
}

Boolean validARPRequest(uint8_t * frame)
{
	ARPFrame * arp_request;
	arp_request = (ARPFrame *) frame;
	/*Check if its a ARP Reply packet*/
	if (   arp_request->Ethernet.ProtocolType != BE(ETHERNET_ARP)
	    || arp_request->ARP.HardwareType != BE(HTYPE)
	    || arp_request->ARP.ProtocolType != BE(IPV4)
	    || arp_request->ARP.HardwareLength != MAC_ADDRESS_SIZE
	    || arp_request->ARP.ProtocolLength != IP_ADDRESS_SIZE
	    || arp_request->ARP.OPCode != BE(ARP_REQUEST))
		return FALSE;
	return TRUE;
}

Boolean validARPReplay(uint8_t * frame)
{
	ARPFrame * arp_reply;
	arp_reply = (ARPFrame *) frame;
	/*Check if its a ARP Reply packet*/
	if (   arp_reply->Ethernet.ProtocolType != BE(ETHERNET_ARP)
	    || arp_reply->ARP.HardwareType != BE(HTYPE)
	    || arp_reply->ARP.ProtocolType != BE(IPV4)
	    || arp_reply->ARP.HardwareLength != MAC_ADDRESS_SIZE
	    || arp_reply->ARP.ProtocolLength != IP_ADDRESS_SIZE
	    || arp_reply->ARP.OPCode != BE(ARP_REPLY))
		return FALSE;
	return TRUE;
}

/*Function that gets the IP header checksum*/
void calcChecksum(uint16_t longitud, uint8_t *datos, uint8_t *checksum) {
	uint16_t word16;
	uint32_t sum=0;
	uint32_t i;

	for (i=0; i<longitud; i=i+2) /* Make 16 bit words out of every two adjacent 8 bit words in the packet and add them up*/
	{
	    word16 = (datos[i]<<8) + datos[i+1];
	    sum += (uint32_t)word16;       
	}
	while (sum>>16) /* Take only 16 bits out of the 32 bit sum and add up the carries */
	    sum = (sum & 0xFFFF)+(sum >> 16);
	sum = ~sum; /* One's complement the result */  
	checksum[0] = sum >> 8;
	checksum[1] = sum & 0xFF;
	return;
}


void applyMask(uint8_t * IP, uint8_t * mask, uint8_t * IPMasked){
	uint8_t i;
	for(i = 0; i < IP_ADDRESS_SIZE; i++)
		IPMasked[i] = IP[i]&mask[i];
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

/*ARP fucntions*/
int ARPRequest(uint8_t IPAddress[], uint8_t * DestMAC)
{
	uint8_t buffer[USPI_FRAME_BUFFER_SIZE];
	uint32_t buffer_length;
	ARPFrame arp_frame;
	uint8_t * MACAddressTable;

	/*Check if the IP is cached in the ARP Table*/
	MACAddressTable = getMACAddress(IPAddress);
	if(MACAddressTable != NULL) /*If is cached, return the MAC Address*/
	{
		memcpy(DestMAC, MACAddressTable, MAC_ADDRESS_SIZE);
		return 0;
	}

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

	console_puts("\n\n ARP Request sent");
	dumpPacket((uint8_t *)&arp_frame, sizeof(arp_frame));

	/*Wait to the ARP Replay*/
	while(1)
	{
		if(!recvFrame((void *)buffer, &buffer_length)) /*While no frame is available continue waiting for one*/
			continue;

		console_puts("\n\n Packet size: ");
		console_puts(uint2dec(buffer_length));

		if(buffer_length <= 60)
		{
			dumpPacket(buffer, buffer_length);
		}

		/*Check if its a ARP Reply packet*/
		if(validARPReplay(buffer) == FALSE)
		{
			console_puts("\n\n Invalid ARP Reply packet");
			continue;
		}
		else
			break; /*Valid ARP Reply*/
	}

	/*Cache the MAC Address*/
	setMACAddress(IPAddress, ((ARPFrame*)buffer)->ARP.MACAddressSender);

	/*Copy the MAC Address replayed*/
	memcpy(DestMAC, ((ARPFrame*)buffer)->ARP.MACAddressSender, MAC_ADDRESS_SIZE);

	return 0;
}

int ARPReplay(void * buffer)
{
	ARPFrame * arp_frame;

	/*Check if its a ARP Request packet*/
	if(validARPRequest(buffer) == FALSE)
		return 1; /*Invalid ARP Request packet*/

	/*Building the ARP Replay message*/
	arp_frame = (ARPFrame *) buffer;
		
	if (memcmp (arp_frame->ARP.ProtocolAddressTarget, netConfiguration.IPAddress, IP_ADDRESS_SIZE) != 0)
		return 1; /*ARP Request is not for us*/

	/* Prepare reply packet */
	/*Ethernet layer*/
	memcpy(arp_frame->Ethernet.MACReceiver, arp_frame->ARP.MACAddressSender, MAC_ADDRESS_SIZE);
	memcpy(arp_frame->Ethernet.MACSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE);

	/*ARP layer*/
	arp_frame->ARP.OPCode = BE(ARP_REPLY);
	memcpy(arp_frame->ARP.MACAddressTarget, arp_frame->ARP.MACAddressSender, MAC_ADDRESS_SIZE);
	memcpy(arp_frame->ARP.ProtocolAddressTarget, arp_frame->ARP.ProtocolAddressSender, IP_ADDRESS_SIZE);
	memcpy(arp_frame->ARP.MACAddressSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE);
	memcpy(arp_frame->ARP.ProtocolAddressSender, netConfiguration.IPAddress, IP_ADDRESS_SIZE);

	/*Send the ARP Request*/
	sendFrame((const void *)arp_frame, sizeof( *arp_frame));

	return 0; /*ARP Replay sent correctly*/
}

int sendEthernet(uint8_t DestMAC[], void * frame, uint32_t frame_length)
{
	uint8_t buffer[USPI_FRAME_BUFFER_SIZE];
	/*Building the Ethernet header*/
	memcpy(((EthernetHeader*)buffer)->MACReceiver, DestMAC, MAC_ADDRESS_SIZE);
	memcpy(((EthernetHeader*)buffer)->MACSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE);
	((EthernetHeader*)buffer)->ProtocolType = BE(ETHERNET_ARP);

	memcpy(buffer + ETHERNET_HEADER_SIZE, frame, frame_length); /*Copy the frame to the final structure*/
	return sendFrame(buffer, frame_length + ETHERNET_HEADER_SIZE);
}

typedef struct IPHeader
{
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t length;
    uint16_t identifier;
    uint16_t flags:4, offset:12;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t ip_src;
    uint32_t ip_dst;
} PACKED IPHeader;

/*Simple IP implementation with no options field*/
int sendIP(uint8_t DestIP[], void * segment, uint32_t segment_lenght)
{
	uint8_t buffer[IP_DATAGRAM_SIZE];
	uint8_t IPSrcMask[IP_ADDRESS_SIZE];
	uint8_t IPDestMask[IP_ADDRESS_SIZE];
	uint8_t DestMAC[MAC_ADDRESS_SIZE];
	IPHeader * ip_header;

	ip_header = (IPHeader *) buffer;
	/*Building the IP Header*/
	applyMask(netConfiguration.IPAddress, netConfiguration.SubnetMask, IPSrcMask); /* Applying mask to source IP */
	applyMask(DestIP, netConfiguration.SubnetMask, IPDestMask); /* Applying mask to source IP */

	/*If IPSrcMask and IPDestMask are equal, it means that are in the same subnet
	  and the ARP Request is for the DestIP. If not, the ARP Request is for the Gateway*/
	if(memcmp(IPSrcMask, IPDestMask, IP_ADDRESS_SIZE) == 0) /* Same network */
		ARPRequest(DestIP, DestMAC);
	else
		ARPRequest(netConfiguration.GatewayAddress, DestMAC); /*Diferent network*/

	ip_header->version_ihl = IP_IHL_VERSION; /* Setting version 4 and IHL 5 */
	ip_header->type_of_service = IP_TOS; /* Basic ToS */
	ip_header->identifier = BE(IP_ID); /* IP identifier */
	ip_header->ttl = IP_TTL; /* Setting the Time To Live */


	/*Copy the segment with the IP header*/
	memcpy(buffer + IP_HEADER_SIZE, segment, segment_lenght);

	/*Send to the Ethernet layer*/
	return sendEthernet(DestMAC, buffer, segment_lenght + IP_HEADER_SIZE);
}




//void sendMessage(uint8_t DestMAC[], void * msg)
//{
//	uint8_t buffer[USPI_FRAME_BUFFER_SIZE];
//	int len = strlen ((const char *) msg);
//	memcpy((buffer+sizeof(EthernetHeader)), msg, len);
//	sendEthernet(DestMAC, buffer, len+sizeof(EthernetHeader));
//}

PACKET_TYPE readPacket(void * buffer, size_t * buffer_length)
{
	uint8_t frame[USPI_FRAME_BUFFER_SIZE];
	while(1)
	{
		if(!recvFrame(frame, buffer_length)) /*While no frame is available continue waiting for one*/
			continue;

		if(ARPReplay(frame) == 0) /*If the frame is a ARP Request still waiting for a new one*/
			continue;

		break; /*No ARP Frame received*/
	}

	/*Identify the packet*/
	return UNKNOWN_PACKET;
}