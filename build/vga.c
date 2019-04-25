#include "vga.h"
#include "mem.h"
#include <stdint.h>

static uint16_t *vgaBuff = (unsigned short *)VGA_BASE;
static int cursor = 0;
static uint16_t color = (0x00 << 4) | 0x02;

void VGA_clear(void)
{
	memset(vgaBuff, '\0', VGA_WIDTH * VGA_HEIGHT);
}
// returns location of beginning of line that cursor is on
static int line(int cursor)
{
	return cursor - (cursor % VGA_WIDTH);
}
static void scroll()
{
	static uint16_t *vgaBuffTemp[VGA_WIDTH * VGA_HEIGHT] = {0};
	int vga_size = VGA_WIDTH * VGA_HEIGHT*sizeof(uint16_t);
	memcpy(vgaBuffTemp, vgaBuff, vga_size);
	memcpy(vgaBuff, vgaBuffTemp + VGA_WIDTH, vga_size - VGA_WIDTH * sizeof(uint16_t));
	memset(vgaBuff + vga_size - VGA_WIDTH, '\0', VGA_WIDTH * sizeof(uint16_t));
}
void VGA_display_char(char c)
{
	if (c == '\n')
	{
		cursor = line(cursor) + VGA_WIDTH;
		if (cursor >= VGA_HEIGHT * VGA_WIDTH)
		{
			scroll();
			cursor -= VGA_WIDTH;
		}
	}
	else if (c == '\r')
	{
		cursor = line(cursor);
	}
	else
	{
		vgaBuff[cursor] = (color << 8) | (uint16_t)c;
		if ((cursor % VGA_WIDTH) < (VGA_WIDTH - 1))
		{
			cursor++;
		}
	}
}
void VGA_display_str(const char *str)
{
	while (*str)
	{
		VGA_display_char(*str);
		str++;
	}
}