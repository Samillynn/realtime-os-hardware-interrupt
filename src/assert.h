#ifndef __ASSERT_H__
#define __ASSERT_H__

int printf(const char *format, ...);
void exit_all();

#ifndef TEST

#define assert(success) {\
    if(!(success)) {\
        printf("Assertion failed at line %d in %s\r\n", __LINE__, __FILE__);\
        exit_all();\
    }\
}

#define assert_p(success, ...) {\
    if(!(success)) {\
        printf("Assertion failed at line %d in %s\r\n", __LINE__, __FILE__);\
        printf("    ");\
        printf(__VA_ARGS__);\
        printf("\r\n");\
        exit_all();\
    }\
}

#else

#include <assert.h>
#define assert_p(success, ...) assert(success)

#endif // TEST

#endif // __ASSERT_H__