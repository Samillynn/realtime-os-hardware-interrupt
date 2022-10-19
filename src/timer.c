#include "timer.h"
#include "assert.h"
#include "debug.h"
#include "bcm2711.h"
#include "printf.h"

#define REFRESH_RATE 10
#define NUM_TIMER_ERROR_EXPIREMENTS 100

// =============== System Timer ============ //
u64 _timer_measurement_error = 0;

STRUCT(SystemTimer) {
  u32 CS;
  u32 CLO;
  u32 CHI;
  u32 C[4];
};

static volatile SystemTimer* const system_timer = (SystemTimer*)SYSTEM_TIMER_BASE;

void sys_timer_clear_match(u8 comparator_id) {
  assert(0 <= comparator_id && comparator_id < 4);
  debug("Clear match %d\r\n", comparator_id);  
  system_timer->CS = 1 << comparator_id;
}

void sys_timer_set_comparator(u8 comparator_id, u32 compare_count) {
  if(comparator_id > 3) {
    printf("Assertion Failed: Timer comparator id can only be 0-2\r\n");
  }

  system_timer->C[comparator_id] = compare_count;
}

u32 sys_timer_half_count() { return system_timer->CLO; }

u64 sys_timer_full_count() {
  unsigned long long hi = system_timer->CHI;
  return (hi << 32) + system_timer->CLO;
}

u64 sys_timer_msrmt_error() {
  if (_timer_measurement_error != 0) 
    return _timer_measurement_error;

  u64 error = 0;
  for(int i=0; i<NUM_TIMER_ERROR_EXPIREMENTS; i++) {
    error += sys_timer_full_count() - sys_timer_full_count();
  }
  _timer_measurement_error = error / NUM_TIMER_ERROR_EXPIREMENTS;
  printf("Measure error is %d\r\n", _timer_measurement_error);
  return _timer_measurement_error;
}


// ======== Timer ============ //
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

// ================ System Clock ============== //
static Clock sys_clocks[4];

u64 sys_clock_boot_time(int clock_id) {
  return sys_clocks[clock_id].boot_time;
}

u64 sys_clock_time_since_boot(int clock_id) {
  return sys_timer_full_count() - sys_clock_boot_time(clock_id);
}

void sys_clock_init(int clock_id, int interval) {
  assert(0 <= clock_id && clock_id < 4);
  Clock* sys_clock = &sys_clocks[clock_id];

  {
    sys_clock->boot_time = sys_timer_full_count();
    sys_clock->tick_count = 0;
    sys_clock->interval = interval;
    sys_clock->next_tick_time = sys_clock->boot_time + sys_clock->interval;
  }

  sys_timer_set_comparator(clock_id, sys_clock->next_tick_time);
}

void sys_clock_notify_tick(int clock_id) {
  sys_timer_clear_match(clock_id);

  Clock* sys_clock = &sys_clocks[clock_id];
  {
    sys_clock->tick_count += 1;
    sys_clock->next_tick_time += sys_clock->interval;
  }
  sys_timer_set_comparator(clock_id, sys_clock->next_tick_time);

  assert_p(sys_clock->next_tick_time > sys_timer_full_count(), "A Tick is missed on clock %d\r\n", clock_id);
}