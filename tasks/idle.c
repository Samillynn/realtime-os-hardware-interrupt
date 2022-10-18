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
#include "../src/interrupt.h"
#include "../src/printf.h"

void idle() {
    u64 error = timer_measurement_error();
    u64 total_running_time = 0, end_time = 0, start_time = 0;
    while (1) {
        end_time = timer_full_system_time_count();
        total_running_time += end_time - start_time - error;
        printf("Time Used by program: %u/%u\r\n", total_running_time, end_time);

        el_disable_interrupt();
        printf("Interrupt in IDLE is disabled\r\n");
        power_standby();
        start_time = timer_full_system_time_count();
        printf("Enabling interrupt in IDLE\r\n");
        el_enable_interrupt();
    }
}