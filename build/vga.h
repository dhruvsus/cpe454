#ifndef VGA_H
#define VGA_H
#include <stdint.h>
#define VGA_BASE 0xb8000
void VGA_clear(void);
void VGA_char_display(char c);
#endif
