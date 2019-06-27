#include "gpu.h"
#include "stdint.h"
#include "stdlib.h"
#include "framebuffer.h"
#include "uart.h"
#include "fonts.h"

#include "string.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define BMP_HEADER_SIZE 54


extern int idiv(uint32_t N, uint32_t D);


typedef struct 
{
	uint8_t * image;
	uint32_t width;
	uint32_t heigh;
	uint16_t size;
} BMPImage;


typedef struct 
{
	unsigned short int foreground_color;
	unsigned short int background_color;
	int x;
	int y;
	uint32_t char_size;
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

void gpu_init(uint8_t char_size)
{
	/*Init the frame buffer to access the structure*/
	framebuffer_init(char_size);
	/*Set up the colors*/
	set_foreground_color(WHITE);
	set_background_color(BLACK);
	/*Set up the character size (SMALL, NORMAL, BIG)*/
	console.char_size = char_size;
	/*Set up the cursor position*/
	console.x = 0;
	uart_puts("OK: GPU (VideoCore) initialized correctly.\r\n");
}

void move_cursor()
{
	register uint32_t rowsize = framebuffer.pitch * CHARSIZE_Y*console.char_size; /* (BytesPerLine)*lineHeight*characterSize */
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
		console.y--;
	}
}

void console_write_char(uint8_t character)
{
	volatile unsigned short int *ptr;

	uint32_t row, offset;
	uint32_t col;
	/*To avoid unnecessary operations in the second loop*/
	uint32_t sizeX = CHARSIZE_X*console.char_size;
	uint32_t sizeY = CHARSIZE_Y*console.char_size;
	for(row = 0; row < sizeY; row++)
	{
		offset = (row+console.y*sizeY)*framebuffer.pitch + console.x*sizeX*2;
		for(col = 0; col < sizeX; col++)
		{
				ptr = (unsigned short int *)(framebuffer.screenbase+offset);
				offset += BITS_PER_PIXEL/8; /*Pixel size*/
				if(row < sizeX && (font8x8_basic[character][idiv(row, console.char_size)] & (1 << idiv(col, console.char_size))))
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


/* Low level graphic utilities and functions */

uint16_t p24_to_p16(uint8_t * pixel)
{
	/* BGR to RGB */
	uint16_t result = 0x0000;

	result |= ((uint16_t) (pixel[2] >> 3)) << 11;
	result |= ((uint16_t) (pixel[1] >> 2)) << 5;
	result |= ((uint16_t) (pixel[0] >> 3));
	
	return result;
}

uint16_t p8_to_p16(uint8_t * pixel)
{
	uint16_t result = 0x0000;
	result |= ((uint16_t) (*pixel) & 0x03) << 3;
	result |= ((uint16_t)( (*pixel) & 0x1C) >> 2) << 8;
	result |= ((uint16_t)( (*pixel) & 0xE0) >> 5) << 13;

	return result;
}

void printPixel(uint16_t pixel, uint16_t x, uint16_t y)
{
	volatile unsigned short int *ptr;
	ptr = (unsigned short int *) framebuffer.screenbase + y*framebuffer.x + x;
	*ptr = pixel;
}

void drawImage24(BMPImage bmp, uint16_t x, uint16_t y)
{
	uint16_t i, j;
	uint16_t iIni, iEnd, jIni, jEnd;

	iIni = MAX(x, 0);
	iEnd = MIN(x + bmp.width, framebuffer.x);
	jIni = MAX(y, 0);
	jEnd = MIN(y + bmp.heigh, framebuffer.y);
	for(j = jEnd; j > jIni; j--)
	{
		for(i = iIni; i < iEnd; i++)
		{
			printPixel(p24_to_p16(bmp.image), i, j);
			bmp.image += 3;
		}
	}
}

void drawImage8(BMPImage bmp, uint16_t x, uint16_t y)
{
	uint16_t i, j;
	for(i = x; i < x + bmp.width && i < framebuffer.x; i++)
	{
		for(j = y; j < y + bmp.heigh && j < framebuffer.y; j++)
		{
			printPixel(p8_to_p16(bmp.image), i, j);
			bmp.image += 1;
		}
	}
}

void parseBMPHeader(uint8_t * image, BMPImage * bmp)
{
	bmp->image = image + BMP_HEADER_SIZE;
	bmp->width = *((uint32_t*)(image+18));
	bmp->heigh = *((uint32_t*)(image+22));
	bmp->size = *((uint16_t*)(image+28));

	console_puts("\n Width: ");
	console_puts(uint2dec(bmp->width));
	console_puts("\n Heigh: ");
	console_puts(uint2dec(bmp->heigh));
	console_puts("\n Size: ");
	console_puts(uint2dec(bmp->size));
}


void drawBMPImage(uint8_t * image, uint16_t x, uint16_t y)
{
	BMPImage bmp;
	parseBMPHeader(image, &bmp);
	if(bmp.size == 8)
		drawImage8(bmp, x, y);
	else if(bmp.size == 24)
		drawImage24(bmp, x, y);
	return;
}



