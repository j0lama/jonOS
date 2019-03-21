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

typedef struct
{
	char name[32];
	uint32_t address;
} solver_t;

extern volatile uint32_t __address_solver;
solver_t * solver;

void add_solver(char * name, void * address)
{
	solver_t sol;
	bzero(&sol, sieof(solver_t));
	strcpy(sol.name, name);
	sol.address = (uint32_t) address;
	memcpy(solver, sol, sizeof(solver_t));
	solver++;
}

void init_address_solver()
{
	solver = (solver_t *) &__address_solver;
	
}