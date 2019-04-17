#ifndef VGA_H
#define VGA_H
#define VGA_BASE 0xb8000
void VGA_char_display(char c);
void VGA_clear(void);
#endif
