#ifndef TIMER_H
#define TIMER_H

#include "common.h"

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

u32 timer_get_system_time_count();
u32 timer_count_to_ms(u32 count);

#endif
