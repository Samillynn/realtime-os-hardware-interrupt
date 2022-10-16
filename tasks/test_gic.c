#include "../src/gic_v2.h"
#include "../src/printf.h"
#include "../src/timer.h"
#include "../src/syscall.h"
#include "../src/assert.h"
#include "../src/debug.h"

void test_gic() {
  debug("should debug\r\n");

  // assert(1==0);

  printf("Start test_gic\r\n");
  sys_timer_set_comparator(1, 3 * TIMER_FREQ);
  printf("Comparator set successfully\r\n");
  for(int i=0; i<20 ;i++) {
    int x=0; for(int j=0; j<=300000; j++) 10+10;
    Yield();
    printf("%d\r\n", i);
  }

  sys_timer_set_comparator(1, 2 * TIMER_FREQ);
  for(int i=0; i<20 ;i++) {
    int x=0; for(int j=0; j<=300000; j++) 10+10;
    Yield();
    printf("%d\r\n", i);
  }
}