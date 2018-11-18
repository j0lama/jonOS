#ifndef GPU_H
#define GPU_H

/* Console colors */
#define RED 	0xF800
#define GREEN 	0x7E0
#define BLUE 	0x001F
#define YELLOW 	0xFFE0
#define MAGENTA 0xF81F
#define CYAN 	0x07FF
#define WHITE 	0xFFFF
#define BLACK 	0x0000
#define HALF 	0x7BEF

/* Character cells are 8x8 */
#define CHARSIZE_X	8
#define CHARSIZE_Y	8

void gpu_init();
void console_puts(char *text);
void set_foreground_color(unsigned short int color);
void set_background_color(unsigned short int color);

#endif