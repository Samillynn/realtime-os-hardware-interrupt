#include "macro.S"
.text
.align 4
.global exception_entry

exception_entry:
    save_user_regs current_task
    load_general_regs kernel_regs

    mrs x0, esr_el1
    ret


.align 4
.text
.global activate_current_task
/** Context switch back to current_task **/

activate_current_task:
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
