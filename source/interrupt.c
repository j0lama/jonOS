/* Based on Jake Sandler implementation */

/*  NOTES:
    The idea of this module is to create some functions that allow us to initialize any interrupt 

    __attribute__ ((interrupt ("ABORT"))) header makes that GCC compile the fucntion with
    a special epilogue and prologue

    The pending flag cannot be cleared using the interrupt peripheral, it must be cleared in the 
    hardware peripheral

    Warnings, errors and messages will be displayed on the UART
*/

#include "stdint.h"
#include "interrupt.h"
#include "stdlib.h"
#include "gpio.h"
#include "uart.h"

static interrupt_registers_t * interrupt_regs;

static interrupt_handler_f handlers[IRQ_NUMBER];
static interrupt_clearer_f clearers[IRQ_NUMBER];

/*From interrupt_vector.s*/
extern void move_exception_vector(void);
extern uint32_t exception_vector;

int interrupts_enabled(void)
{
    /*If the bit 7 of CPSR register (Current Program Status Register), the interrupts are enabled*/
    int res;
    __asm__ __volatile__("mrs %[res], CPSR": [res] "=r" (res)::);
    return ((res >> 7) & 1) == 0;
}

void interrupts_init(void) {
    interrupt_regs = (interrupt_registers_t *)(INTERRUPTS_BASE + 0x200); /*Initialize the interrupt register struct*/
    bzero(handlers, sizeof(interrupt_handler_f) * IRQ_NUMBER); /*Clean all handlers*/
    bzero(clearers, sizeof(interrupt_clearer_f) * IRQ_NUMBER); /*Clean all handlers*/
    interrupt_regs->irq_basic_disable = 0xffffffff; /* Disable all interrupts */
    interrupt_regs->irq_gpu_disable1 = 0xffffffff;
    interrupt_regs->irq_gpu_disable2 = 0xffffffff;
    move_exception_vector(); /*Copy the exception vector to address 0*/
    /*Enable interrupts*/
    if (!interrupts_enabled())
    { /*If not enabled*/
        __asm__ __volatile__("cpsie i"); /*Enable the interrupts with CPS (Change Processor State)*/
    }
    uart_puts("OK: interrupts initialized correctly.\r\n");
}

void register_irq_handler(irq_number_t irq_num, interrupt_handler_f handler, interrupt_clearer_f clearer)
{
    uint32_t irq_pos;
    if (IRQ_IS_BASIC(irq_num))
    {
        irq_pos = irq_num - 64;
        handlers[irq_num] = handler;
        clearers[irq_num] = clearer;
        interrupt_regs->irq_basic_enable |= (1 << irq_pos);
    }
    else if (IRQ_IS_GPU2(irq_num))
    {
        irq_pos = irq_num - 32;
        handlers[irq_num] = handler;
        clearers[irq_num] = clearer;
        interrupt_regs->irq_gpu_enable2 |= (1 << irq_pos);
    }
    else if (IRQ_IS_GPU1(irq_num))
    {
        irq_pos = irq_num;
        handlers[irq_num] = handler;
        clearers[irq_num] = clearer;
        interrupt_regs->irq_gpu_enable1 |= (1 << irq_pos);
    }
}

void unregister_irq_handler(irq_number_t irq_num)
{
    uint32_t irq_pos;
    if (IRQ_IS_BASIC(irq_num))
    {
        irq_pos = irq_num - 64;
        handlers[irq_num] = 0;
        clearers[irq_num] = 0;
        // Setting the disable bit clears the enabled bit
        interrupt_regs->irq_basic_disable |= (1 << irq_pos);
    }
    else if (IRQ_IS_GPU2(irq_num))
    {
        irq_pos = irq_num - 32;
        handlers[irq_num] = 0;
        clearers[irq_num] = 0;
        interrupt_regs->irq_gpu_disable2 |= (1 << irq_pos);
    }
    else if (IRQ_IS_GPU1(irq_num))
    {
        irq_pos = irq_num;
        handlers[irq_num] = 0;
        clearers[irq_num] = 0;
        interrupt_regs->irq_gpu_disable1 |= (1 << irq_pos);
    }
}


void irq_handler(void)
{
    int j; 
    for (j = 0; j < IRQ_NUMBER; j++)
    {
        // If the interrupt is pending and there is a handler, run the handler
        if (IRQ_IS_PENDING(interrupt_regs, j)  && (handlers[j] != 0))
        {
            clearers[j]();
            /*Enable interrupts*/
            if (!interrupts_enabled())
            { /*If not enabled*/
                __asm__ __volatile__("cpsie i"); /*Enable the interrupts with CPS (Change Processor State)*/
            }
            handlers[j]();
            /*Disable interrupts*/
            if (interrupts_enabled())
            { /*If enabled*/
                __asm__ __volatile__("cpsid i"); /*Disable the interrupts with CPS (Change Processor State)*/
            }
            return;
        }
    }
}

void __attribute__ ((interrupt ("ABORT"))) reset_handler(void)
{
    uart_puts("RESET HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("ABORT"))) prefetch_abort_handler(void)
{
    uart_puts("PREFETCH ABORT HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("ABORT"))) data_abort_handler(void)
{
    uart_puts("DATA ABORT HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("UNDEF"))) undefined_instruction_handler(void)
{
    uart_puts("UNDEFINED INSTRUCTION HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("SWI"))) software_interrupt_handler(void)
{
    uart_puts("SWI HANDLER\n");
    while(1);
}
void __attribute__ ((interrupt ("FIQ"))) fast_irq_handler(void)
{
    uart_puts("FIQ HANDLER\n");
    while(1);
}