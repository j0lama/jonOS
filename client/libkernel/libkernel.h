#ifndef KERNEL_CALLS_H
#define KERNEL_CALLS_H
#include "stdint.h"
#include "stddef.h"

/* NETWORK */
#define sendUDP ((int(*))((uint8_t*)(uint16_t)(void*)(uint32_t))0xb158)

/* SCREEN */
#define console_puts ((void(*)(char*))0x8bd4)

#define function ((void(*)(char*))0x8048)


#endif