#include "printk.h"
#include "keyboard.h"
void kmain()
{	
	printk("%%");
	initKeyboard();
	while (1)
	{
		asm volatile("hlt");
	}
}
