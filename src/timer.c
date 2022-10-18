#include "timer.h"
#include "bcm2711.h"
#include "printf.h"

#define REFRESH_RATE 10
#define NUM_TIMER_ERROR_EXPIREMENTS 100

u64 _timer_measurement_error = 0;

STRUCT(SystemTimer) {
  u32 CS;
  u32 CLO;
  u32 CHI;
  u32 C[4];
};

static volatile SystemTimer* const system_timer = (SystemTimer*)SYSTEM_TIMER_BASE;

void sys_timer_clear_match(u8 comparator_id) {
  if(comparator_id > 3) {
    printf("Assertion Failed: Timer comparator id can only be 0-2\r\n");
  }
  printf("Clear match %d\r\n", comparator_id);  
  system_timer->CS = 1 << comparator_id;
}

void sys_timer_set_comparator(u8 comparator_id, u32 compare_count) {
  if(comparator_id > 3) {
    printf("Assertion Failed: Timer comparator id can only be 0-2\r\n");
  }

  system_timer->C[comparator_id] = system_timer->CLO + compare_count;
}

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

u32 timer_count_to_ms(u32 count) {
  return count;
}

u32 timer_get_system_time_count() { return system_timer->CLO; }

u64 timer_full_system_time_count() {
  unsigned long long hi = system_timer->CHI;
  return (hi << 32) + system_timer->CLO;
}

u64 timer_measurement_error() {
  if (_timer_measurement_error) 
    return _timer_measurement_error;

  u64 error = 0;
  for(int i=0; i<NUM_TIMER_ERROR_EXPIREMENTS; i++) {
    error += timer_full_system_time_count() - timer_full_system_time_count();
  }
  _timer_measurement_error = error / NUM_TIMER_ERROR_EXPIREMENTS;
  return _timer_measurement_error;
}