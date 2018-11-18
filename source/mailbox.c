#include "stdint.h"
#include "mailbox.h"

/*
 * Data synchronisation barrier
 * No instruction after the DSB can run until all instructions before it have
 * completed
 */
#define dmb() asm volatile ("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0) )
/*
 * Data synchronisation barrier
 * No instruction after the DSB can run until all instructions before it have
 * completed
 */
#define dsb() asm volatile ("mcr p15, #0, %[zero], c7, c10, #4" : : [zero] "r" (0) )
/*
 * Clean and invalidate entire cache
 * Flush pending writes to main memory
 * Remove all data in data cache
 */
#define flushcache() asm volatile ("mcr p15, #0, %[zero], c7, c14, #0" : : [zero] "r" (0) )
#define MAILBOX_FULL 0x80000000 /* Bit 31 set in status register if the write mailbox is full */
#define MAILBOX_EMPTY 0x40000000 /* Bit 30 set in status register if the read mailbox is empty */


static volatile unsigned int *MAILBOX0READ = (unsigned int *) MAILBOX_READ;
static volatile unsigned int *MAILBOX0STATUS = (unsigned int *) MAILBOX_STATUS;
static volatile unsigned int *MAILBOX0WRITE = (unsigned int *) MAILBOX_WRITE;


uint32_t mailbox_read(uint32_t channel)
{
	unsigned int count = 0;
	unsigned int data;

	/* Loop until something is received from channel
	 * If nothing recieved, it eventually give up and returns 0xffffffff
	 */
	while(1)
	{
		while (*MAILBOX0STATUS & MAILBOX_EMPTY)
		{
			/* Need to check if this is the right thing to do */
			flushcache();

			/* This is an arbritarily large number */
			if(count++ >(1<<25))
			{
				return 0xffffffff;
			}
		}
		/* Read the data
		 * Data memory barriers as we've switched peripheral
		 */
		dmb();
		data = *MAILBOX0READ;
		dmb();

		if ((data & 15) == channel)
			return data;
	}
}

void mailbox_send(uint32_t msg, uint32_t channel)
{
	/* Wait for mailbox to be not full */
	while (*MAILBOX0STATUS & MAILBOX_FULL)
	{
		/* Need to check if this is the right thing to do */
		flushcache();
	}

	dmb();
	*MAILBOX0WRITE = (msg | channel);
}