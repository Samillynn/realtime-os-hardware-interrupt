#ifndef UTILITIES_H
#define UTILITIES_H

#include "common.h"

void* memset(void* s, i32 c, usize n);
void* memcpy(void* restrict dest, const void* restrict src, usize n);

void i32_to_cstring(i32 number, cstring str, i32 base);
void u32_to_cstring(u32 number, cstring str, u32 base);
void i64_to_cstring(i64 number, cstring str, i32 base);
void u64_to_cstring(u64 number, cstring str, u32 base);
i32 cstring_to_i32(cstring str);
u32 cstring_to_u32(cstring str);
i32 cstring_len_to_i32(cstring str, usize str_len);
u32 cstring_len_to_u32(cstring str, usize str_len);

i32 i32_clamp(i32 value, i32 min, i32 max);

cstring cstring_get_token(const cstring line, usize line_len, usize idx,
                          usize *token_size);

u32 get_random_u32(u32 min, u32 max);

#endif
