#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#define TIMER_FREQ 1000000 // 1MHz

// ======== Timer ============ //
STRUCT(Timer) {
  u32 current_count;
  u32 last_count;
  u32 delta_count;

  u32 sec10_count;
  u32 sec10;
};

void timer_init(Timer* self);
void timer_update(Timer* self);
u32 timer_get_sec(Timer* self);
u32 timer_get_sec10(Timer *self);
u32 timer_count_to_ms(u32 count);

// ============ System Timer =========== //
void sys_timer_clear_match(u8 comparator_id);
void sys_timer_set_comparator(u8 comparator_id, u32 compare_count);
u32 sys_timer_half_count();
u64 sys_timer_full_count();
u64 sys_timer_msrmt_error();

// =========== System Clock ============ //
STRUCT(Clock) {
  u64 boot_time;
  u64 next_tick_time;
  int interval;
  int tick_count;
};

void sys_clock_init(int clock_id, int interval);
void sys_clock_notify_tick(int clock_id);
u64 sys_clock_boot_time(int clock_id);
u64 sys_clock_time_since_boot(int clock_id);

#endif
