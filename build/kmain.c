#include "printk.h"
#include "keyboard.h"
#include "vga.h"
void kmain()
{
	char c='\0';
	int i = 0;
	VGA_clear();
	initKeyboard();
	while(c!='\0'){
		c=pollKeyboard();
		VGA_display_char(c);
	}
	while (1)
	{
		asm volatile("hlt");
	}
}