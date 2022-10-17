#include "macro.S"

.macro switch_to_kernel
    save_user_regs current_task
    load_general_regs kernel_regs
.endm

.text
.align 4
.global synchronous_exception_entry

synchronous_exception_entry:
    switch_to_kernel

    // SYNCHRONOUS_EXCEPTION is 0
    mov x0, #0
    ret

.text
.align 4
.global irq_entry
irq_entry:
    switch_to_kernel

    // IRQ is 1
    mov x0, #1
    ret


.align 4
.text
.global activate_running_task
/** Context switch back to current_task **/

activate_running_task:
    save_general_regs kernel_regs
    load_user_regs current_task
    eret


.global handle_error
.align 4
handle_error:
    mrs x0, CurrentEL
    mrs x1, ELR_EL1
    mrs x2, ESR_EL1
    bl print_error
    ret
