#ifndef __CLOCK_SERVER_H__
#define __CLOCK_SERVER_H__

#include "common.h"

void clock_server();

i32 Time(i32 tid);
i32 Delay(i32 tid, i32 ticks);
i32 DelayUntil(i32 tid, i32 ticks);

#endif
