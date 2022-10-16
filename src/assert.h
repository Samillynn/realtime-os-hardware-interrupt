#ifndef __ASSERT_H__
#define __ASSERT_H__

#include "printf.h"

extern void exit_all();

#define assert(success) {\
    if(!success) {\
        printf("Assertion failed at line %d in %s\r\n", __LINE__, __FILE__);\
        exit_all();\
    }\
}

#endif