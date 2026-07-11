#include "lib/serial.h"

void kernel_main(void) {  
  
  serial_clear_screen();
  serial_print("Hello World!\n");

  for (;;) {
    __asm__ __volatile__("hlt");
  }
}
