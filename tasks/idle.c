/**
while (1) {
    set end time
    calculate time diff
    add time diff - clock_interval to total time
    disable interrupt by set DAIF
    enter power saving mode
    set start time
    enable interrupt by clear DAIF (will this trigger interrupt right after?)
}
*/

#include "../src/timer.h"
#include "../src/config.h"
#include "../src/interrupt.h"
#include "../src/printf.h"
#include "../src/assert.h"

#define REPORT_PERIOD 50

u64 execution_time = 0, enter_time = 0, last_exit_time = 0, begin_time = 0;
int idle_count = 0;

void idle_setup() {
    idle_count = 0;
    last_exit_time = sys_clock_boot_time(1);
    begin_time = sys_clock_boot_time(1);
    assert(last_exit_time == begin_time);
}

void idle_after_enter() {
    enter_time = sys_timer_full_count();
    execution_time += enter_time - last_exit_time;
    if (idle_count % REPORT_PERIOD == 0) {
        printf("Performance before the %dth IDLE: %d%%\r\n", idle_count,
            execution_time / ((enter_time - begin_time) / 100));

        execution_time = 0;
        begin_time = sys_timer_full_count();
    }
    idle_count += 1;
}

/* Try to minimize execution time before exiting IDLE to make the interrupt responsive */
void idle_before_exit() {
    last_exit_time = sys_timer_full_count();
}

void idle() {
    idle_setup();
    while (1) {
        el_disable_interrupt();
        idle_after_enter();
        power_standby();
        idle_before_exit();
        el_enable_interrupt();
    }
}