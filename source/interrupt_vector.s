// Based on Jake Sandler tutorial
.section ".text"

.global move_exception_vector

exception_vector:
	// We use this way of load addresses because when we compile with the -fpic (Position Independent Code)
	// GCC replaces the expected absolute address with a relative address
    ldr pc, reset_handler_abs_addr
    ldr pc, undefined_instruction_handler_abs_addr
    ldr pc, software_interrupt_handler_abs_addr
    ldr pc, prefetch_abort_handler_abs_addr
    ldr pc, data_abort_handler_abs_addr
    nop                                         // This one is reserved
    ldr pc, irq_handler_abs_addr
    ldr pc, fast_irq_handler_abs_addr

reset_handler_abs_addr:                 .word reset_handler
undefined_instruction_handler_abs_addr: .word undefined_instruction_handler
software_interrupt_handler_abs_addr:    .word software_interrupt_handler
prefetch_abort_handler_abs_addr:        .word prefetch_abort_handler
data_abort_handler_abs_addr:            .word data_abort_handler
irq_handler_abs_addr:                   .word irq_handler_asm_wrapper
fast_irq_handler_abs_addr:              .word fast_irq_handler


// This fucntion is used to move the handler functions to address 0 at runtime
move_exception_vector:
    push    {r4, r5, r6, r7, r8, r9} // Calling C functions expect these regs be saved from them
    ldr     r0, =exception_vector // Exception verctor address
    mov     r1, #0x0000 // Destination address
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9} //Move the r0 content to the regs and after storing each reg, increment the address for the next one
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9} //Store
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8}
    // Restore the saved regs
    pop     {r4, r5, r6, r7, r8, r9}
    blx     lr

irq_handler_asm_wrapper:
    sub     lr, lr, #4 //Return address to one instruction back
    srsdb   sp!, #0x13 //Store lr and spsr to the stack of supervisor mode 0x13 
    cpsid   if, #0x13 //Change to supervisor mode and disable the interrupts
    push    {r0-r3, r12, lr} //Preserve all regs
    and     r1, sp, #4 //Stack must be 8 byte aligned
    sub     sp, sp, r1
    push    {r1}
    bl      irq_handler //Custom handler
    pop     {r1} // Restoring stack and regs
    add     sp, sp, r1
    pop     {r0-r3, r12, lr}
    rfeia   sp! //Restore spsr and return to lr

