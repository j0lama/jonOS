/*
	Raspberry Pi Framebuffer documentation
	https://github.com/raspberrypi/documentation/blob/JamesH65-mailbox_docs/configuration/mailboxes/propertiesARM-VC.md
*/

#include "framebuffer.h"
#include "stdint.h"
#include "mailbox.h"
#include "uart.h"
#include "gpu.h"
#include "gpu.h"

/*VideoCore request/response codes*/
#define REQUEST_CODE 0
#define RESPONSE_SUCCESSFUL 0x80000000
#define RESPONSE_INVALID_PITCH 0x80000004
#define RESPONSE_INVALID_TAG 0x80000008

/*TAGS*/
#define END_TAG 0
#define ALLOCATE_FRAMEBUFFER 0x00040001
#define GET_RESOLUTION 0x00040003
#define GET_PITCH 0x40008
#define SET_PHYSICAL_RESOLUTION 0x00048003
#define SET_VIRTUAL_RESOLUTION 0x00048004
#define SET_DEPTH 0x00048005

extern int idiv(uint32_t N, uint32_t D);

int function(int i)
{
	return i;
}

/*	Message content

	AAAABBBBxxxxxxxxCCCCyyyyyyyy
	└┬─┘└┬─┘└───┬──┘└┬─┘└───┬──┘
	 │   │      │    │      └  Padding to specified length
	 │   │      │    └ End Tag (0x00)
	 │   │      └ Sequence of Tags
	 │   └ Buffer request/response code
	 └ Buffer Size
 */

void framebuffer_init(uint32_t  char_size)
{
	uint32_t count;
	uint32_t c;

	/* Storage space for the buffer used to pass information between the
	 * CPU and VideoCore
	 * Needs to be aligned to 16 bytes as the bottom 4 bits of the address
	 * passed to VideoCore are used for the mailbox number
	 */
	volatile uint32_t mailbox_buffer[256] __attribute__((aligned (16)));

	/* Get the display size
	Tag: 0x00040003
	Request:
		Length: 0
	Response:
		Length: 8
		Value:
			u32: width in pixels
			u32: height in pixels
	*/
	c = 1;
	mailbox_buffer[c++] = REQUEST_CODE;
	mailbox_buffer[c++] = GET_RESOLUTION;
	mailbox_buffer[c++] = 8; /*Buffser size*/
	mailbox_buffer[c++] = 0; /*Request size*/
	mailbox_buffer[c++] = 0; /*Horizontal resolution*/
	mailbox_buffer[c++] = 0; /*Vertical resolution*/
	mailbox_buffer[c++] = END_TAG;
	mailbox_buffer[0] = c * 4; /*Total size*/

	mailbox_send((uint32_t)mailbox_buffer, 8);
	mailbox_read(8);

	/* Valid response in data structure */
	if(mailbox_buffer[1] != RESPONSE_SUCCESSFUL)
	{
		uart_puts("ERROR: Getting screen resolution.\r\n");
		return;
	}

	/*Getting the screen resolution*/
	framebuffer.x = mailbox_buffer[5];
	framebuffer.y = mailbox_buffer[6];

	/*QEMU resolution*/
	if(framebuffer.x == 0 && framebuffer.y == 0)
	{
		framebuffer.x = 640;
		framebuffer.y = 480;
	}

	if(framebuffer.x == 0 || framebuffer.y == 0)
	{
		uart_puts("ERROR: Screen invalid resolution.\r\n");
		return;
	}


	/* SET UP SCREEN */
	c = 1;

	/*Set up physical width/height
	Tag: 0x00048003
	Request:
		Length: 8
		Value:
			u32: width in pixels
			u32: height in pixels
	Response:
		Length: 8
		Value:
			u32: width in pixels
			u32: height in pixels
	*/
	mailbox_buffer[c++] = REQUEST_CODE;
	mailbox_buffer[c++] = SET_PHYSICAL_RESOLUTION;
	mailbox_buffer[c++] = 8; /*Value buffer size (bytes)*/
	mailbox_buffer[c++] = 8; /*Req. + value length (bytes)*/
	mailbox_buffer[c++] = framebuffer.x;
	mailbox_buffer[c++] = framebuffer.y;

	/*Set up virtual width/height
	Tag: 0x00048004
	Request:
		Length: 8
		Value:
			u32: width in pixels
			u32: height in pixels
	Response:
		Length: 8
		Value:
			u32: width in pixels
			u32: height in pixels
	*/	
	mailbox_buffer[c++] = SET_VIRTUAL_RESOLUTION;
	mailbox_buffer[c++] = 8; /*Value buffer size (bytes)*/
	mailbox_buffer[c++] = 8; /*Req. + value length (bytes)*/
	mailbox_buffer[c++] = framebuffer.x;
	mailbox_buffer[c++] = framebuffer.y;

	/* Set depth
	Tag: 0x00048005
	Request:
		Length: 4
		Value:
			u32: bits per pixel
	Response:
		Length: 4
		Value:
			u32: bits per pixel
	*/
	mailbox_buffer[c++] = SET_DEPTH;
	mailbox_buffer[c++] = 4; /*Value buffer size (bytes)*/
	mailbox_buffer[c++] = 4; /*Req. + value length (bytes)*/
	mailbox_buffer[c++] = BITS_PER_PIXEL; /*R+G+B*/

	/*Allocate framebuffer
	Tag: 0x00040001
	Request:
		Length: 4
		Value:
			u32: alignment in bytes
	Response:
		Length: 8
		Value:
			u32: frame buffer base address in bytes
			u32: frame buffer size in bytes
	*/
	mailbox_buffer[c++] = ALLOCATE_FRAMEBUFFER;
	mailbox_buffer[c++] = 8; /*Value buffer size (bytes)*/
	mailbox_buffer[c++] = 4; /*Req. + value length (bytes)*/
	mailbox_buffer[c++] = 16; /*Alignment*/
	mailbox_buffer[c++] = 0; /*Space for response*/

	mailbox_buffer[c++] = END_TAG;
	mailbox_buffer[0] = c*4; /*Total size*/

	mailbox_send((uint32_t)mailbox_buffer, 8);
	mailbox_read(8);

	/* Valid response in data structure */
	if(mailbox_buffer[1] != RESPONSE_SUCCESSFUL)
	{
		uart_puts("ERROR: Setting up the framebuffer.\r\n");
		return;
	}

	/*Check the VideoCore response to the set up*/
	for(count = 2; mailbox_buffer[count] != ALLOCATE_FRAMEBUFFER; count += 3+(mailbox_buffer[count+1]>>2))
	{
		if(count>c)
		{
			uart_puts("ERROR: Invalid tags.\r\n");
			return;
		}
	}
	if(mailbox_buffer[count+2] != RESPONSE_INVALID_TAG)
	{
		uart_puts("ERROR: Invalid tag response.\r\n");
		return;
	}

	/* Getting the screenbase address and the screen size */
	framebuffer.screenbase = mailbox_buffer[count+3];
	framebuffer.screensize = mailbox_buffer[count+4];
	/*Checking errors*/
	if(framebuffer.screenbase == 0 || framebuffer.screensize == 0)
	{
		uart_puts("ERROR: Invalid screen address/size.\r\n");
		return;
	}

	/* Get the pitch (bytes per line) */
	c = 1;
	mailbox_buffer[c++] = REQUEST_CODE;
	mailbox_buffer[c++] = GET_PITCH;
	mailbox_buffer[c++] = 4; /*Buffer size*/
	mailbox_buffer[c++] = 0; /*Request size*/
	mailbox_buffer[c++] = 0; /*Space for pitch*/
	mailbox_buffer[c++] = END_TAG;
	mailbox_buffer[0] = c * 4; /*Total size*/

	mailbox_send((uint32_t)mailbox_buffer, 8);
	mailbox_read(8);

	/* Check the VideoCore response to the get pitch request*/
	if(mailbox_buffer[4] != RESPONSE_INVALID_PITCH)
	{
		uart_puts("ERROR: Invalid pitch response.\r\n");
		return;
	}
	/*Getting the screen pitch*/
	framebuffer.pitch = mailbox_buffer[5];
	if(framebuffer.pitch == 0)
	{
		uart_puts("ERROR: Invalid pitch value.\r\n");
		return;
	}

	/*Setting up the max dimensions (Used in to print' fucntions)*/
	framebuffer.max_x = idiv(framebuffer.x, (CHARSIZE_X*char_size));
	framebuffer.max_y = idiv(framebuffer.y, (CHARSIZE_Y*char_size));

	uart_puts("OK: Framebuffer initialized correctly.\r\n");
}




