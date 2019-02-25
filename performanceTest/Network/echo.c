#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
	int sock;
	int len;
	int SERVER_PORT = 1234;
	struct sockaddr_in server_address, client_address;
	char buffer[32];

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SERVER_PORT);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("could not create socket\n");
		return 1;
	}

	if ((bind(sock, (struct sockaddr *)&server_address, sizeof(server_address))) < 0)
	{
		printf("could not bind socket\n");
		return 1;
	}

	// socket address used to store client address
	
	int client_address_len = 0;

	while (true) {
		len = recvfrom(sock, buffer, 32, 0,(struct sockaddr *)&client_address, &client_address_len);
		buffer[len] = '\0';
		sendto(sock, buffer, len, 0, (struct sockaddr *)&client_address, sizeof(client_address));
	}

	return 0;
}