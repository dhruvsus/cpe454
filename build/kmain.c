#include "vga.h"
void kmain()
{
	VGA_clear();
	while (1)
	{
		asm volatile("hlt");
	}
}
