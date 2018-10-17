/*Example of a function using the jonOS's libraries*/
void function()
{
	void (* uart_puts)(char *) = (void(*)(char * )) 0x0000000000008248;
	char (* msg) = (char *) 0x7fb8;
	while (1)
	{
		uart_puts(msg);
	}
}