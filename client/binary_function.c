#include "libkernel.h"

int echoServer()
{
	void(*bzero)(void *, register size_t) = solveFunction(BZERO);
	int*(*recv)(uint16_t, void *, size_t) = solveFunction(RECV);
	void(*console_puts)(uint8_t *) = solveFunction(CONSOLE_PUTS);
	uint8_t buffer[32];

	while(1)
	{
		bzero(buffer, 32);
		recv(0, buffer, 32);
		console_puts(buffer);
		if(buffer[1] == 'e' && buffer[2] == 'n' && buffer[3] == 'd')
			break;
	}
	return 0;
}