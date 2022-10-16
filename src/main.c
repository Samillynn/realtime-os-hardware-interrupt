#include "syscall.h"
#include "init_kernel.h"
#include "kernel.h"
#include "config.h"

i32 main() {
  config();
  init_kernel();
  kmain();

  return 0;
}
