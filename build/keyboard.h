#ifndef KEYBOARD_H
#define KEYBOARD_H
#define PS2_DATA 0x60
#define PS2_CMD 0x64
#define PS2_STATUS PS2_CMD
#define PS2_STATUS_OUTPUT 1
#define PS2_STATUS_INPUT (1 << 1)
#define DISABLE_PORT_1 0xAD
#define DISABLE_PORT_2 0xA7
#define CMD_GET_CONFIGURATION 0x20

void initKeyboard(void);
static void initScanCodes(void);
#endif