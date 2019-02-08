#include "cache.h"

/* I-CACHE Solution */
/* https://community.arm.com/processors/b/blog/posts/caches-and-self-modifying-code */

/* ARM cache documentation */
/* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0301h/Babhejba.html */

void cleanCaches()
{
	/* Clean and Invalidate Entire Data Cache */
	__asm__("mov r0, #0");
	__asm__("mcr p15, 0, r0, c7, c14, 0");
	/* Invalidate Entire Instruction Cache */
	__asm__("mov r0, #0");
	__asm__("mcr p15, 0, r0, c7, c5, 0");
}