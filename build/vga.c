#include "vga.h"
static unsigned short *vgaBuff = (unsigned short*)VGA_BASE;
static int width = 80;
static int height = 25;
static int cursor = 0;
static unsigned char color = (0x00<<4) | 0x02;

void VGA_clear(void){
	for(int i=0;i<width*height;i++){
		vgaBuff[i]=0;
	}
}
void VGA_display_char(char c){
	vgaBuff[cursor]=(color<<8)|c;
}
