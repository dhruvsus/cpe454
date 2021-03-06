#ifndef USER_H_
#define USER_H_

//#define USER_STACK_BOTTOM 0xF000000000000000
//#define USER_STACK_BOTTOM 0x7000000000000000 
#define USER_STACK_BOTTOM (0x80000000000 - 4096)
// TODO increase this later and use huge page frames
#define USER_STACK_SIZE (4096 * 32)
#define USER_STACK_TOP (USER_STACK_BOTTOM - USER_STACK_SIZE)

#define DPL_USER 3
#define DPL_KERNEL 0
#define GDT_USER_CS 0x10
#define GDT_USER_DS 0x18

#endif  // USER_H_
