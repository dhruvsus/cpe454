#include "printk.h"
#include "keyboard.h"
#include "vga.h"
void kmain()
{
	char c;
	int i=0;
	VGA_clear();
	initKeyboard();
	while (1)
	{
		c = pollKeyboard();
		if (c != 0)
		{
			printk("character from keyboard %c\n", c);
		}
	}
	while (1)
	{
		asm volatile("hlt");
	}
}