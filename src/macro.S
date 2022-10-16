#ifndef __MACRO_S__
#define __MACRO_S__

.macro load_global reg_name var_name
    ldr \reg_name, =\var_name
    ldr \reg_name, [\reg_name]
.endm

.macro save_general_regs var
    // store x0 to SP, so that x0 can be used temporarily
    str x0, [sp, #-16]!

    load_global x0, \var

    /** debug for x30 **/
    str x30, [x0, #(8*34)]
    /** end debug for x30 **/

    add x0, x0, #8

    // save x1 - x30
    stp x1, x2, [x0], #16
    stp x3, x4, [x0], #16
    stp x5, x6, [x0], #16
    stp x7, x8, [x0], #16
    stp x9, x10, [x0], #16
    stp x11, x12, [x0], #16
    stp x13, x14, [x0], #16
    stp x15, x16, [x0], #16
    stp x17, x18, [x0], #16
    stp x19, x20, [x0], #16
    stp x21, x22, [x0], #16
    stp x23, x24, [x0], #16
    stp x25, x26, [x0], #16
    stp x27, x28, [x0], #16
    stp x29, x30, [x0], #16

    // save x0
    ldr x1, [sp], #16
    sub x0, x0, #(8*31)
    str x1, [x0], #(8*31)

.endm

.macro load_general_regs var

    load_global x0, \var
    add x0, x0, #(8*31)

    ldp x29, x30, [x0, #-16]!
    ldp x27, x28, [x0, #-16]!
    ldp x25, x26, [x0, #-16]!
    ldp x23, x24, [x0, #-16]!
    ldp x21, x22, [x0, #-16]!
    ldp x19, x20, [x0, #-16]!
    ldp x17, x18, [x0, #-16]!
    ldp x15, x16, [x0, #-16]!
    ldp x13, x14, [x0, #-16]!
    ldp x11, x12, [x0, #-16]!
    ldp x9, x10, [x0, #-16]!
    ldp x7, x8, [x0, #-16]!
    ldp x5, x6, [x0, #-16]!
    ldp x3, x4, [x0, #-16]!
    ldp x1, x2, [x0, #-16]!

    sub x0, x0, #8
    ldr x0, [x0]
.endm

.macro load_user_regs var
    load_global x0, \var

    // load x1 - x30
    add x0, x0, #(8*31)

    // load sp
    msr spsel, #0
    ldr x1, [x0], #8
    mov sp, x1
    msr spsel, #1

    // load spsr
    ldr x1, [x0], #8
    msr spsr_el1, x1

    // load pc
    ldr x1, [x0], #8
    msr elr_el1, x1


    // load x30 to x0
    load_general_regs \var
.endm


.macro save_user_regs var
    save_general_regs \var

    // save sp
    msr spsel, #0
    mov x1, sp
    str x1, [x0], #8
    msr spsel, #1

    // str spsr
    mrs x1, spsr_el1
    str x1, [x0], #8

    // str pc
    mrs x1, elr_el1
    str x1, [x0], #8

.endm

#endif
