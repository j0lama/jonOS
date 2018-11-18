/* Based on Jake Sandler implementation */

#include "interrupt.h"
#include "gpio.h"
#include "stdint.h"
#include "uart.h"

typedef struct
{
    uint8_t timer0_matched: 1;
    uint8_t timer1_matched: 1;
    uint8_t timer2_matched: 1;
    uint8_t timer3_matched: 1;
    uint32_t reserved: 28;
} timer_control_reg_t;

typedef struct
{
    timer_control_reg_t control;
    uint32_t counter_low;
    uint32_t counter_high;
    uint32_t timer0;
    uint32_t timer1;
    uint32_t timer2;
    uint32_t timer3;
} timer_registers_t;

static timer_registers_t * timer_regs;

void timer_set(uint32_t usecs)
{
    timer_regs->timer1 = timer_regs->counter_low + usecs;
}

void timer(uint32_t seconds)
{
	timer_set(seconds*1000000);
}

void mtimer(uint32_t mseconds)
{
	timer_set(mseconds*1000);
}

void utimer(uint32_t useconds)
{
	timer_set(useconds);
}

static void timer_irq_handler(void)
{
    uart_puts("Timer IRQ\r\n");
    timer(3);
}

static void timer_irq_clearer(void)
{
    timer_regs->control.timer1_matched = 1;
}

void timer_init(void) {
    timer_regs = (timer_registers_t *) SYSTEM_TIMER_BASE;
    register_irq_handler(SYSTEM_TIMER_1, timer_irq_handler, timer_irq_clearer);
    uart_puts("OK: Timer initialized correctly.\r\n");
}