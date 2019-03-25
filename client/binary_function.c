#include "libkernel.h"

int echoServer()
{
	uint8_t buffer[32];
	void(*bzero)(void *, register size_t) = solveFunctionInt(19);
	int*(*recv)(uint16_t, void *, size_t) = solveFunctionInt(11);
	void(*console_puts)(uint8_t *) = solveFunctionInt(1);

	while(1)
	{
		bzero(buffer, 32);
		recv(0, buffer, 32);
		console_puts(buffer);
	}
	return 0;
}