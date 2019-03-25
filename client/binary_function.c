#include "libkernel.h"

int echoServer()
{
	uint8_t buffer[32];
	uint8_t IPAddressDest[] = {192, 168, 1, 222};
	void(*bzero)(void *, register size_t) = solveFunction(BZERO);
	int*(*recv)(uint16_t, void *, size_t) = solveFunction(RECV);
	int*(*sendUDP)(uint8_t *, uint16_t, void *, uint32_t) = solveFunction(SENDUDP);
	size_t*(*strlen)(const char *) = solveFunction(STRLEN);

	while(1)
	{
		bzero(buffer, 32);
		recv(ANY_PORT, buffer, 32);
		sendUDP(IPAddressDest, 8888, buffer, (uint32_t)strlen((char *)buffer));
	}
}