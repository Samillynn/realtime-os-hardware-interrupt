#include "timer.h"
#include "bcm2711.h"

#define TIMER_FREQ 1000000 // 1MHz
#define REFRESH_RATE 10

struct SYSTEM_TIMER {
  u32 CS;
  u32 CLO;
  u32 CHI;
  u32 C[4];
};

static volatile struct SYSTEM_TIMER* const system_timer = (struct SYSTEM_TIMER*)(SYSTEM_TIMER_BASE);

void timer_init(Timer* self) {
  self->current_count = system_timer->CLO;
  self->last_count = self->current_count;
  self->delta_count = 0;

  self->sec10_count = 0;
  self->sec10 = 0;
}

void timer_update(Timer* self) {
  self->current_count = system_timer->CLO;
  self->delta_count = 0;

  if (self->current_count > self->last_count) {
    self->delta_count = self->current_count - self->last_count;

    // sleep for limiting refresh rate
    while (self->delta_count < TIMER_FREQ / REFRESH_RATE) {
      self->current_count = system_timer->CLO;
      self->delta_count = self->current_count - self->last_count;
    }

    self->sec10_count += self->delta_count;
    if (self->sec10_count >= TIMER_FREQ / 10) {
      self->sec10 += self->sec10_count / (TIMER_FREQ / 10);
      self->sec10_count = self->sec10_count % (TIMER_FREQ / 10);
    }
  }

  self->last_count = self->current_count;
}

u32 timer_get_sec(Timer *self) { return self->sec10 / 10; }

u32 timer_get_sec10(Timer *self) { return self->sec10; }

u32 timer_get_system_time_count() { return system_timer->CLO; }

u32 timer_count_to_ms(u32 count) {
  return count;
}
