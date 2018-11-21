//Simple implementation for unsigned division because the CPU ARM1176JZF doesn't provide it
//Approach to integer division that assumes that N and D are positive integers
.section ".text"
.global idiv 
idiv:
    /* r0 contains N */
    /* r1 contains D */
    mov r2, r1
    mov r1, r0
    mov r0, #0 /*Quotien*/
 
    b .check
    .loop:
       add r0, r0, #1
       sub r1, r1, r2
    .check:
       cmp r1, r2
       bhs .loop
 
    /* r0 -> quotient */
    /* r1 -> rest*/
    bx lr
    