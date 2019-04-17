#include "vga.h"
#include "mem.h"

static unsigned short *vgaBuff = (unsigned short *)VGA_BASE;
static int width = 80;
static int height = 25;
static int cursor = 0;
static unsigned char color = (0x00 << 4) | 0x02;

void VGA_clear(void)
{
	memset(vgaBuff, 0, width * height);
}
void VGA_display_char(char c)
{
	vgaBuff[cursor] = (color << 8) | c;
}
