#include <stdint.h>
#include "serial.h"

#define COM1 0x3F8

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port)); 
  // mov al, [val]
  // mov dx, [port]
  // outb al, dx
}

static inline unsigned char inb(unsigned short port) {
  unsigned char ret;
  __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) :  "Nd"(port)); 
  return ret;
  // mov dx, [port]
  // inb dx, al 
  // mov [ret], al
} 

int serial_probe (unsigned short port) {
  outb(port + 7, 0xAE);
  return inb(port + 7) == 0xAE;
}

void serial_char_output(unsigned char text) {
  outb(COM1, text);
}

void serial_print(const unsigned char *string) {
  unsigned char status = inb(COM1 + 5);
  while (*string != '\0') {
    // line status register check to see if its ready to send the next byte
    while ((status & 0x20) == 0) {
      status = inb(COM1 + 5); 
    }
    outb(COM1, (unsigned char)*string);
    string++;
      
  }
}

void serial_clear_screen(void) {
  // these are two commands: clear the screen and return to row 1 column 1 
  serial_print("\x1b[2J\x1b[H");
}

