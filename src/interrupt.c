#include "interrupt.h"
#include "printf.h"
#include "task_scheduler.h"

#define GIC_BASE_ADDR (0xFF840000)
#define GICD_ADDR (GIC_BASE_ADDR + 0x1000)
#define GICC_ADDR (GIC_BASE_ADDR + 0x2000)

#define DEFAULT_CORE 0
#define VC_INTID_OFFSET 96
#define SYS_TIMER_INITID VC_INTID_OFFSET

volatile GICC* const gicc = (GICC*) GICC_ADDR;
volatile GICD* const gicd = (GICD*) GICD_ADDR;

// ================ GICD ==================== //
void gicd_enable_interrupt(u16 intid) {
    int index = intid >> 5;
    int bit_pos = intid && 31;
    gicd->isenable[index] |= (1 << bit_pos);
}

void gicd_disable_interrupt(u16 intid) {
    gicd->icenable[intid>>5] |= (1 << (intid && 31));
}

void setup_gicd_targets() {
    // todo: make 255 be read from itlinenum instead
    memset((void*)&(gicd->itargets[8]), 1 << DEFAULT_CORE, (255-8)*4);
}


void setup_gicd() {
    gicd->ctl |= 1;
    setup_gicd_targets();
    gicd_enable_interrupt(SYS_TIMER_INITID + 1);
}

// ================ GICC ==================== //
void setup_gicc() {
    gicc->ctl |= 1;
    gicc->pm = 0;
}

u32 ack_interrupt() {
    return gicc->ia;
}

void clear_interrupt(u32 intid) {
    gicc->eoi = intid;
}

// =============== CPU ==================== //
void el_enable_interrupt() {
    asm volatile("msr DAIFClr, #0b1111");
}

void el_disable_interrupt() {
    asm volatile("msr DAIFSet, #0b1111");
}

void power_standby() {
    asm volatile("wfi");
}

// =============== SYSTEM =================== // 
void init_gic() {
    setup_gicc();
    setup_gicd();
}

void sys_await_event() {
    change_task_state(current_task, AWATIEVENT);
    int intid = current_task->x[0];
    add_to_irq_queue(intid, current_task);
}