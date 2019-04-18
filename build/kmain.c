#include "printk.h"
#include "keyboard.h"
#include "vga.h"
void kmain()
{	
	VGA_clear();
	printk("%u",-5);
	initKeyboard();
	while (1)
	{
		asm volatile("hlt");
	}
}
