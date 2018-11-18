#include "gpu.h"
#include "stdint.h"
#include "stdlib.h"
#include "framebuffer.h"
#include "uart.h"
#include "fonts.h"

typedef struct 
{
	unsigned short int foreground_color;
	unsigned short int background_color;
	int x;
	int y;
} console_t;

console_t console;

void set_foreground_color(unsigned short int color)
{
	console.foreground_color = color;
}

void set_background_color(unsigned short int color)
{
	console.background_color = color;
}

void gpu_init()
{
	/*Init the frame buffer to access the structure*/
	framebuffer_init();
	/*Set up the colors*/
	set_foreground_color(WHITE);
	set_background_color(BLACK);
	/*Set up the cursor position*/
	console.x = 0;
	console.y = 0;
	uart_puts("OK: GPU (VideoCore) initialized correctly.\r\n");
}

/* Current console text cursor position (ie. where the next character will
 * be written
*/


void move_cursor()
{
	register uint32_t rowsize = framebuffer.pitch * CHARSIZE_Y; /* (BytesPerLine)*lineHeight */
	uint32_t src;
	console.x = 0;
	console.y++;
	if (console.y >= (int)(framebuffer.max_y-1))
	{
		src = framebuffer.screenbase + rowsize; /*Begin of the second line*/
		/*Move [1, end] lines to [0, end-1]*/
		memcpy((void *)framebuffer.screenbase, (void *)src, (framebuffer.max_y-1)*rowsize);
		/*Clean last line*/
		bzero((void *)(framebuffer.screenbase + (framebuffer.max_y-1)*rowsize), rowsize);
	}
}

void console_write_char(uint8_t c)
{
	volatile unsigned short int *ptr;

	uint32_t row, offset;
	int col;

	for(row=0; row<CHARSIZE_Y; row++)
	{
		offset = (row+console.y*CHARSIZE_Y)*framebuffer.pitch + console.x*CHARSIZE_X*2;
		for(col = 0; col < (CHARSIZE_X-1); col++)
		{
			ptr = (unsigned short int *)(framebuffer.screenbase+offset);
			offset += BITS_PER_PIXEL/8; /*Pixel size*/
			if(row < (CHARSIZE_Y-1) && (font8x8_basic[c][row] & (1<<col)))
				*ptr = console.foreground_color;
			else
				*ptr = console.background_color;
		}
		ptr = (unsigned short int *)(framebuffer.screenbase+offset);
		*ptr = console.background_color;
	}
	console.x++; /*Increase the x cursor in 1*/
}

void console_write(uint8_t c)
{
	uint8_t i;
	/*Normal character*/
	if(c != '\n' && c != '\t')
	{
		console_write_char(c);
	}
	else if (c == '\n')
	{
		move_cursor();
	}
	else if (c == '\t')
	{
		for(i = 0; i < 4; i++)
		{
			console_write_char(' ');
		}
	}

	/*Check if the horizontal cursor is bigger than the max defined by the GPU*/
	if(console.x >=(int)framebuffer.max_x)
	{
		move_cursor();
	}
}

void console_puts(char * str)
{
	while(*str != '\0')
		console_write((uint8_t)*str++);
}

