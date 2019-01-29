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
    uint16_t flags_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t ip_src[IP_ADDRESS_SIZE];
    uint8_t ip_dst[IP_ADDRESS_SIZE];
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

Boolean validUDPHeader(uint16_t port, UDPHeader * header)
{
	/*This function only checks the dest_port assuming that checksum its unused*/
	if(header->dest_port != BE(port))
		return FALSE;
	return TRUE;
}


Boolean validIPHeader(IPHeader * ip_header)
{
	IPHeader header_aux; /*Used to check the IP Header checksum*/
	memcpy(&header_aux, ip_header, IP_HEADER_SIZE);
	header_aux.checksum = 0x0000;
	calcChecksum(IP_HEADER_SIZE, (uint8_t *) &header_aux, (uint8_t *) &header_aux.checksum);

	/*This function onlu checks the unvariable fields of the IP header*/
	if(	ip_header->version_ihl != IP_IHL_VERSION /*Check the */
		|| ip_header->checksum != header_aux.checksum /*Checksum*/
		|| memcmp(ip_header->ip_dst, netConfiguration.IPAddress, IP_ADDRESS_SIZE) != 0) /*IP Dest*/
		return FALSE;
	return TRUE;
}

Boolean validEthernetHeader(EthernetHeader * eth_header)
{
	if(	memcmp(eth_header->MACReceiver, netConfiguration.MACAddress, MAC_ADDRESS_SIZE) /*Check if dest_mac is the Raspberry Pi MAC*/
		|| eth_header->ProtocolType != BE(IPV4)) /*Check if level 3 protocol is IP protocol*/
		return FALSE;
	return TRUE;
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

	/*Wait to the ARP Replay*/
	while(1)
	{
		if(!recvFrame((void *)buffer, &buffer_length)) /*While no frame is available continue waiting for one*/
			continue;

		/*Check if its a ARP Reply packet*/
		if(validARPReplay(buffer) == FALSE)
		{
			/*Invalid ARP Reply packet*/
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

int sendEthernet(uint8_t DestMAC[], void * frame, uint32_t frame_length, uint16_t ProtocolType)
{
	uint8_t buffer[USPI_FRAME_BUFFER_SIZE];
	/*Building the Ethernet header*/
	memcpy(((EthernetHeader*)buffer)->MACReceiver, DestMAC, MAC_ADDRESS_SIZE);
	memcpy(((EthernetHeader*)buffer)->MACSender, netConfiguration.MACAddress, MAC_ADDRESS_SIZE);
	((EthernetHeader*)buffer)->ProtocolType = BE(ProtocolType);

	memcpy(buffer + ETHERNET_HEADER_SIZE, frame, frame_length); /*Copy the frame to the final structure*/
	return sendFrame(buffer, frame_length + ETHERNET_HEADER_SIZE);
}

/*Simple IP implementation with no options field*/
int sendIP(uint8_t DestIP[], void * segment, uint32_t segment_lenght)
{
	uint8_t buffer[IP_DATAGRAM_SIZE];
	uint8_t IPSrcMask[IP_ADDRESS_SIZE];
	uint8_t IPDestMask[IP_ADDRESS_SIZE];
	uint8_t DestMAC[MAC_ADDRESS_SIZE];
	uint32_t len = ((USPI_FRAME_BUFFER_SIZE - IP_HEADER_SIZE)/8)*8;
	uint16_t offset = 0;
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
	ip_header->protocol = IP_DATA_PROTOCOL; /* Setting the data protocol */
	ip_header->checksum = 0x0000; /* Setting by default the checksum to 0 */

	/* Copy IPSrc and DestIP to the header */
	memcpy(ip_header->ip_src, netConfiguration.IPAddress, IP_ADDRESS_SIZE); /* Our IP */
	memcpy(ip_header->ip_dst, DestIP, IP_ADDRESS_SIZE); /* DestIP */

	/* IP fragmentation*/
	while(segment_lenght + IP_HEADER_SIZE > USPI_FRAME_BUFFER_SIZE)
	{
		segment_lenght -= len;
		ip_header->length = BE(len + IP_HEADER_SIZE); /* Setting the IP packet length */
		ip_header->flags_offset = BE((offset & 0x1fff) | 0x2000); /*Setting Flags (DF = 0, MF = 1) and offset*/
		/*Copy the fragment*/
		memcpy(buffer + IP_HEADER_SIZE, (uint8_t *)segment + offset, len);
		offset += ((USPI_FRAME_BUFFER_SIZE - IP_HEADER_SIZE)/8); /*Increment the offset*/

		/* Calculate the checksum */
		calcChecksum(IP_HEADER_SIZE, (uint8_t *)ip_header, (uint8_t *)&ip_header->checksum);

		/*Send the fragmented package*/
		sendEthernet(DestMAC, buffer, len + IP_HEADER_SIZE, IPV4);
	}

	ip_header->length = BE(segment_lenght + IP_HEADER_SIZE); /*Last or unique fragment size*/
	ip_header->flags_offset = BE(offset & 0x1fff); /*Flags (DF = 0, MF = 0)*/
	/* Calculate the checksum */
	calcChecksum(IP_HEADER_SIZE, (uint8_t *)ip_header, (uint8_t *)&ip_header->checksum);
	/*Copy the segment with the IP header*/
	memcpy(buffer + IP_HEADER_SIZE, (uint8_t *)segment + offset, segment_lenght);

	/*Send to the Ethernet layer*/
	return sendEthernet(DestMAC, buffer, segment_lenght + IP_HEADER_SIZE, IPV4);
}

int sendUDP(uint8_t DestIP[], uint16_t DestPort, void * message, uint32_t message_length)
{
	uint8_t buffer[UDP_HEADER_SIZE + message_length];
	UDPHeader * udp_header;
	udp_header = (UDPHeader *)buffer;
	/*Building the UDP header*/
	udp_header->source_port = BE(UDP_DEFAULT_PORT);
	udp_header->dest_port = BE(DestPort);
	udp_header->length = BE(UDP_HEADER_SIZE + message_length);
	/*Checksum is zero because its unused*/
	udp_header->checksum = 0x0000;

	/*Copy the message*/
	memcpy(buffer + UDP_HEADER_SIZE, message, message_length);

	/*Send to the IP layer*/
	return sendIP(DestIP, buffer, UDP_HEADER_SIZE + message_length);
}


int recv(uint16_t port, void * buffer, size_t buffer_length)
{
	/*Alloc static memory for the buffer and the headers (UDP/IP/ETHERNET)*/
	uint8_t frame[buffer_length + UDP_HEADER_SIZE + IP_HEADER_SIZE + ETHERNET_HEADER_SIZE];
	UDPHeader * udp_header;
	IPHeader * ip_header;
	EthernetHeader * eth_header;
	size_t frameSize = buffer_length + UDP_HEADER_SIZE + IP_HEADER_SIZE + ETHERNET_HEADER_SIZE;

	while(1)
	{
		if(!recvFrame(frame, &frameSize)) /*While no frame is available continue waiting for one*/
			continue;

		if(ARPReplay(frame) == 0) /*If the frame is a ARP Request still waiting for a new one*/
			continue;

		eth_header = (EthernetHeader *) (frame);
		if(validEthernetHeader(eth_header) == FALSE) /*Invalid Ethernet header*/
			continue;

		ip_header = (IPHeader *) (frame + ETHERNET_HEADER_SIZE);
		if(validIPHeader(ip_header) == FALSE) /*Invalid IP header*/
			continue;

		udp_header = (UDPHeader *) (frame + ETHERNET_HEADER_SIZE + IP_HEADER_SIZE);
		if(validUDPHeader(port, udp_header) == FALSE) /*Invalid dest_port*/
			continue;

		/*Copy the UDP message*/
		memcpy(buffer, frame + UDP_HEADER_SIZE + IP_HEADER_SIZE + ETHERNET_HEADER_SIZE, buffer_length);

		break; /*No ARP Frame received*/
	}

	return 0;
}