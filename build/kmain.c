#include "printk.h"
void kmain()
{	
	printk("%%");
	while (1)
	{
		asm volatile("hlt");
	}
}
