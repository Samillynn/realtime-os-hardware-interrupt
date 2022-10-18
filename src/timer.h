#ifndef TIMER_H
#define TIMER_H

#include "common.h"
#define TIMER_FREQ 1000000 // 1MHz

STRUCT(Timer) {
  u32 current_count;
  u32 last_count;
  u32 delta_count;

  u32 sec10_count;
  u32 sec10;
};

void sys_timer_clear_match(u8 comparator_id);
void sys_timer_set_comparator(u8 comparator_id, u32 compare_count);
void timer_init(Timer* self);
void timer_update(Timer* self);
u32 timer_get_sec(Timer* self);
u32 timer_get_sec10(Timer *self);

u32 timer_get_system_time_count();
u64 timer_full_system_time_count();
u64 timer_measurement_error();
u32 timer_count_to_ms(u32 count);

#endif
