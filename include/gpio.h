#ifndef GPIO_H
#define GPIO_H

#ifdef _RASPI2
/*For Raspberry Pi 2 and 3*/
#define GPIO_BASE 			0x3F200000
#define UART0_BASE 			0x3F201000
#else
/*For Raspberry Pi 1 and Zero*/
#define SYSTEM_TIMER_BASE	0x20003000
#define INTERRUPTS_BASE 	0x2000B000
#define GPIO_BASE 			0x20200000
#define UART0_BASE 			0x20201000
#endif

/*Controls actuation of pull up/down to ALL GPIO pins*/
#define GPPUD  			(GPIO_BASE + 0x94)

/*Controls actuation of pull up/down for specific GPIO pin*/
#define GPPUDCLK0		(GPIO_BASE + 0x98)

/*The offsets for reach register for the UART*/
#define UART0_DR    	(UART0_BASE + 0x00)
#define UART0_RSRECR	(UART0_BASE + 0x04)
#define UART0_FR    	(UART0_BASE + 0x18)
#define UART0_ILPR  	(UART0_BASE + 0x20)
#define UART0_IBRD  	(UART0_BASE + 0x24)
#define UART0_FBRD  	(UART0_BASE + 0x28)
#define UART0_LCRH  	(UART0_BASE + 0x2C)
#define UART0_CR    	(UART0_BASE + 0x30)
#define UART0_IFLS  	(UART0_BASE + 0x34)
#define UART0_IMSC  	(UART0_BASE + 0x38)
#define UART0_RIS   	(UART0_BASE + 0x3C)
#define UART0_MIS   	(UART0_BASE + 0x40)
#define UART0_ICR   	(UART0_BASE + 0x44)
#define UART0_DMACR 	(UART0_BASE + 0x48)
#define UART0_ITCR  	(UART0_BASE + 0x80)
#define UART0_ITIP  	(UART0_BASE + 0x84)
#define UART0_ITOP  	(UART0_BASE + 0x88)
#define UART0_TDR   	(UART0_BASE + 0x8C)

#endif