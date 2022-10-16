.text
.global init_vector_table
.align 4

init_vector_table:
    // adr x0, vector_table
    ldr x0, =vector_table
    msr VBAR_EL1, x0
    ret
