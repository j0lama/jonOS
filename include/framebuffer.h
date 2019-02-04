#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "stdint.h"

#define BITS_PER_PIXEL 16

typedef struct 
{
	uint32_t screenbase;
	uint32_t screensize;
	uint32_t x;
	uint32_t y;
	uint32_t pitch;
	uint32_t max_x;
	uint32_t max_y;
} framebuffer_t;

framebuffer_t framebuffer;

void framebuffer_init(uint32_t char_size);

int function(char * c);

#endif