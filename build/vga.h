#ifndef VGA_H
#define VGA_H
#include <stdint.h>
#define VGA_BASE 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
void VGA_clear(void);
void VGA_display_char(char c);
void VGA_display_str(const char* str);
#endif
