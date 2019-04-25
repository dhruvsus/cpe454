#include "printk.h"
#include "keyboard.h"
#include "vga.h"
void kmain()
{
	char c;
	VGA_clear();
	initKeyboard();
	while (1)
	{
		c=pollKeyboard();
		printk("character from keyboard %c",c);
	}
	while (1)
	{
		asm volatile("hlt");
	}
}