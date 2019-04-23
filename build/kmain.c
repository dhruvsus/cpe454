#include "printk.h"
#include "keyboard.h"
#include "vga.h"
void kmain()
{	
	char str[100]={'\0'};
	int i=0;
	//while(i==0);
	VGA_clear();
	printk("%%");
	initKeyboard();
	while (1)
	{
		asm volatile("hlt");
	}
}