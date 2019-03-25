#include <address_solver.h>
#include <cache.h>
#include <gpu.h>
#include <io.h>
#include <malloc.h>
#include <network.h>
#include <stdlib.h>
#include <string.h>
#include <uart.h>
#include <stdint.h>
#include <stddef.h>

extern int idiv(uint32_t N, uint32_t D);

typedef struct
{
	uint32_t address;
} solver_t;

extern volatile uint32_t __address_solver;
solver_t * solver;

void add_solver(void * address)
{
	solver_t sol;
	bzero(&sol, sizeof(solver_t));
	sol.address = (uint32_t) address;
	memcpy(solver, &sol, sizeof(solver_t));
	solver++;
}

void init_address_solver()
{
	/* Setting the solver address*/
	solver = (solver_t *) &__address_solver;

	/*********/
	/* CACHE */
	/*********/
	/* void cleanCaches() */
	add_solver((void*)cleanCaches);

	/*******/
	/* GPU */
	/*******/
	/* void console_puts(char *text) */
	add_solver((void*)console_puts);
	/* void set_foreground_color(unsigned short int color) */
	add_solver((void*)set_foreground_color);
	/* void set_background_color(unsigned short int color) */
	add_solver((void*)set_background_color);

	/********************/
	/* Input Output (IO)*/
	/********************/
	/* void io_write(uint32_t addr, uint32_t data) */
	add_solver((void*)io_write);
	/* uint32_t io_read(uint32_t addr) */
	add_solver((void*)io_read);

	/**********/
	/* MALLOC */
	/**********/
	/* void * alloc_m(size_t size) */
	add_solver((void*)alloc_m);
	/* void * calloc_m(size_t count, size_t size) */
	add_solver((void*)calloc_m);
	/* void free_m(void * block) */
	add_solver((void*)free_m);

	/***********/
	/* NETWORK */
	/***********/
	/* Network configuration struct */
	add_solver((void*)&netConfiguration);
	/* int sendUDP(uint8_t DestIP[], uint16_t DestPort, void * message, uint32_t message_length) */
	add_solver((void*)sendUDP);
	/* int recv(uint16_t port, void * buffer, size_t buffer_length) */
	add_solver((void*)recv);
	/* void printIP(uint8_t IPAddress[]) */
	add_solver((void*)printIP);
	/* void printMAC(uint8_t MACAddress[]) */
	add_solver((void*)printMAC);
	/* void printARPTable() */
	add_solver((void*)printARPTable);
	/* void dumpPacket(uint8_t * packet, uint32_t size) */
	add_solver((void*)dumpPacket);

	/**********/
	/* STDLIB */
	/**********/
	/* void memset(void * dest, register int c, register size_t len) */
	add_solver((void*)memset);
	/* void memcpy(void * dest, const void * src, register size_t len) */
	add_solver((void*)memcpy);
	/*int memcmp(const void * buffer1, const void * buffer2, size_t nLength)*/
	add_solver((void*)memcmp);
	/* void bzero(void * dest, register size_t len) */
	add_solver((void*)bzero);
	/* int atoi(char * str) */
	add_solver((void*)atoi);

	/**********/
	/* STRING */
	/**********/
	/* char *uint2hex(uint32_t value) */
	add_solver((void*)uint2hex);
	/* char *uint2dec(uint32_t value) */
	add_solver((void*)uint2dec);
	/* char *char2hex(uint8_t value) */
	add_solver((void*)char2hex);
	/* size_t strlen (const char *string) */
	add_solver((void*)strlen);
	/* void strcpy (char * dest, const char * src) */
	add_solver((void*)strcpy);
	/* int strcmp (const char * str1, const char * str2) */
	add_solver((void*)strcmp);

	/********/
	/* UART */
	/********/
	/* void uart_putc(uint8_t c) */
	add_solver((void*)uart_putc);
	/* uint8_t uart_getc() */
	add_solver((void*)uart_getc);
	/* void uart_puts(char* str) */
	add_solver((void*)uart_puts);

	/********************/
	/* Integer division */
	/********************/
	/* int idiv(uint32_t N, uint32_t D) */
	add_solver((void*)idiv);
}

void * solveFunction(int pos)
{
	solver_t * sol;
	sol = (solver_t *) &__address_solver;
	return sol[pos].address;
}














