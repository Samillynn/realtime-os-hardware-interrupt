#include "utilities.h"
#include "common.h"
#include "bcm2711.h"

void* memset(void* s, i32 c, usize n) {
  for (u8* it = (u8*)s; n > 0; n --) {
    *it++ = (u8)c;
  }

  return s;
}

void* memcpy(void* restrict dest, const void* restrict src, usize n) {
  u8* sit = (u8*)src;
  u8* cdest = (u8*)dest;
  for (usize i = 0; i < n; i ++) {
    *(cdest++) = *(sit++);
  }

  return dest;
}

void i32_to_cstring(i32 number, cstring str, i32 base) {
  bool negative = false;

  if (number == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  if (number < 0 && base == 10) {
    negative = true;
    number = -number;
  }

  usize i = 0;
  while (number != 0) {
    i32 remainder = number % base;
    str[i ++] = (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0';
    number = number / base;
  }

  if (negative) {
    str[i ++] = '-';
  }

  usize front = 0;
  usize end = i - 1;
  while (front < end) {
    char tmp = str[front];
    str[front] = str[end];
    str[end] = tmp;
    front ++;
    end --;
  }

  str[i ++] = '\0';
}

void u32_to_cstring(u32 number, cstring str, u32 base) {
  if (number == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  usize i = 0;
  while (number != 0) {
    u32 remainder = number % base;
    str[i ++] = (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0';
    number = number / base;
  }

  usize front = 0;
  usize end = i - 1;
  while (front < end) {
    char tmp = str[front];
    str[front] = str[end];
    str[end] = tmp;
    front ++;
    end --;
  }

  str[i ++] = '\0';
}

void i64_to_cstring(i64 number, cstring str, i32 base) {
  bool negative = false;

  if (number == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  if (number < 0 && base == 10) {
    negative = true;
    number = -number;
  }

  usize i = 0;
  while (number != 0) {
    i64 remainder = number % base;
    str[i ++] = (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0';
    number = number / base;
  }

  if (negative) {
    str[i ++] = '-';
  }

  usize front = 0;
  usize end = i - 1;
  while (front < end) {
    char tmp = str[front];
    str[front] = str[end];
    str[end] = tmp;
    front ++;
    end --;
  }

  str[i ++] = '\0';
}

void u64_to_cstring(u64 number, cstring str, u32 base) {
  if (number == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  usize i = 0;
  while (number != 0) {
    u64 remainder = number % base;
    str[i ++] = (remainder > 9) ? (remainder - 10) + 'A' : remainder + '0';
    number = number / base;
  }

  usize front = 0;
  usize end = i - 1;
  while (front < end) {
    char tmp = str[front];
    str[front] = str[end];
    str[end] = tmp;
    front ++;
    end --;
  }

  str[i ++] = '\0';
}

i32 cstring_to_i32(cstring str) {
  i32 value = 0;
  usize str_len = strlen(str);

  if (str) {
    usize i = 0;
    if (str[i] == '-') i += 1;
    for (; i < str_len; i ++) {
      value = value * 10 + (str[i] - '0');
    }
    if (str[0] == '-') value *= -1;
  }
  
  return value;
}

u32 cstring_to_u32(cstring str) {
  u32 value = 0;
  usize str_len = strlen(str);

  if (str) {
    for (usize i = 0; i < str_len; i ++) {
      value = value * 10 + (str[i] - '0');
    }
  }

  return value;
}

i32 cstring_len_to_i32(cstring str, usize str_len) {
  i32 value = 0;

  if (str) {
    usize i = 0;
    if (str[i] == '-') i += 1;
    for (; i < str_len; i ++) {
      value = value * 10 + (str[i] - '0');
    }
    if (str[0] == '-') value *= -1;
  }
  
  return value;
}

u32 cstring_len_to_u32(cstring str, usize str_len) {
  u32 value = 0;

  if (str) {
    for (usize i = 0; i < str_len; i ++) {
      value = value * 10 + (str[i] - '0');
    }
  }

  return value;
}

i32 i32_clamp(i32 value, i32 min, i32 max) {
  if (value < min) {
    return min;
  } else if (value > max) {
    return max;
  }
  return value;
}

cstring cstring_get_token(const cstring line, usize line_len, usize idx, usize* token_size) {
  cstring token = NULL;
  *token_size = 0;

  char* ptr = &line[0];
  usize ptr_idx = 0;
  usize token_idx = 0;
  for (usize i = 0; i < line_len; i ++) {
    if (line[i] == ' ') {
      if (token_idx == idx) {
	token = ptr;
	*token_size = i - ptr_idx;
	return token;
      } else {
	token_idx += 1;
	ptr = &line[i + 1];
	ptr_idx = i + 1;
      }
    }
  }

  if (token_idx == idx) {
    token = ptr;
    *token_size = line_len - ptr_idx;
  }

  return token;
}

STRUCT(SYSTEM_TIMER) {
  u32 CS;
  u32 CLO;
  u32 CHI;
  u32 C[4];
};

static volatile SYSTEM_TIMER* const system_timer = (SYSTEM_TIMER*)(SYSTEM_TIMER_BASE);

u32 get_random_u32(u32 min, u32 max) {
  u32 v = system_timer->CLO;
  return v % (max - min) + min;
}
