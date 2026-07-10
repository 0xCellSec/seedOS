#include <stdint.h>

#ifndef SERIAL_H
#define SERIAL_H

static inline void outb(uint16_t port, uint8_t value);
static inline unsigned char inb(unsigned short port);
void serial_char_output(unsigned char text);
void serial_print(const unsigned char *string);
void serial_clear_screen(void);

#endif
