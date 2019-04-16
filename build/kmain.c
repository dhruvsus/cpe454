extern void VGA_clear();
void kmain(){
	VGA_display_char('y');
	while(1){
		asm volatile("hlt");
	}	
}
