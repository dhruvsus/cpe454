#include "keyboard.h"
#include "asm.h"
#include "vga.h"
#include "printk.h"
char scancodeMap[0xFE] = {0};
static void initScanCodes(void)
{
    scancodeMap[0x0D] = '\t';
    scancodeMap[0x0E] = '`';
    scancodeMap[0x15] = 'q';
    scancodeMap[0x16] = '1';
    scancodeMap[0x1A] = 'z';
    scancodeMap[0x1B] = 's';
    scancodeMap[0x1C] = 'a';
    scancodeMap[0x1D] = 'w';
    scancodeMap[0x1E] = '2';
    scancodeMap[0x21] = 'c';
    scancodeMap[0x22] = 'x';
    scancodeMap[0x23] = 'd';
    scancodeMap[0x24] = 'e';
    scancodeMap[0x25] = '4';
    scancodeMap[0x26] = '3';
    scancodeMap[0x29] = ' ';
    scancodeMap[0x2A] = 'v';
    scancodeMap[0x2B] = 'f';
    scancodeMap[0x2C] = 't';
    scancodeMap[0x2D] = 'r';
    scancodeMap[0x2E] = '5';
    scancodeMap[0x31] = 'n';
    scancodeMap[0x32] = 'b';
    scancodeMap[0x33] = 'h';
    scancodeMap[0x34] = 'g';
    scancodeMap[0x35] = 'y';
    scancodeMap[0x36] = '6';
    scancodeMap[0x3A] = 'm';
    scancodeMap[0x3B] = 'j';
    scancodeMap[0x3C] = 'u';
    scancodeMap[0x3D] = '7';
    scancodeMap[0x3E] = '8';
    scancodeMap[0x41] = ',';
    scancodeMap[0x42] = 'k';
    scancodeMap[0x43] = 'i';
    scancodeMap[0x44] = 'o';
    scancodeMap[0x45] = '0';
    scancodeMap[0x46] = '9';
    scancodeMap[0x49] = '.';
    scancodeMap[0x4A] = '/';
    scancodeMap[0x4B] = 'l';
    scancodeMap[0x4C] = ';';
    scancodeMap[0x4D] = 'p';
    scancodeMap[0x4E] = '-';
    scancodeMap[0x52] = '\'';
    scancodeMap[0x54] = '[';
    scancodeMap[0x55] = '=';
    scancodeMap[0x5A] = '\n';
    scancodeMap[0x5B] = ']';
    scancodeMap[0x5D] = '\\';
    scancodeMap[0x0D] = '\t';
    scancodeMap[0x0E] = '~';
    scancodeMap[0x15] = 'Q';
    scancodeMap[0x16] = '!';
    scancodeMap[0x1A] = 'Z';
    scancodeMap[0x1B] = 'S';
    scancodeMap[0x1C] = 'A';
    scancodeMap[0x1D] = 'W';
    scancodeMap[0x1E] = '@';
    scancodeMap[0x21] = 'C';
    scancodeMap[0x22] = 'X';
    scancodeMap[0x23] = 'D';
    scancodeMap[0x24] = 'E';
    scancodeMap[0x25] = '$';
    scancodeMap[0x26] = '#';
    scancodeMap[0x29] = ' ';
    scancodeMap[0x2A] = 'V';
    scancodeMap[0x2B] = 'F';
    scancodeMap[0x2C] = 'T';
    scancodeMap[0x2D] = 'R';
    scancodeMap[0x2E] = '%';
    scancodeMap[0x31] = 'N';
    scancodeMap[0x32] = 'B';
    scancodeMap[0x33] = 'H';
    scancodeMap[0x34] = 'G';
    scancodeMap[0x35] = 'Y';
    scancodeMap[0x36] = '^';
    scancodeMap[0x3A] = 'M';
    scancodeMap[0x3B] = 'J';
    scancodeMap[0x3C] = 'U';
    scancodeMap[0x3D] = '&';
    scancodeMap[0x3E] = '*';
    scancodeMap[0x41] = '<';
    scancodeMap[0x42] = 'K';
    scancodeMap[0x43] = 'I';
    scancodeMap[0x44] = 'O';
    scancodeMap[0x45] = ')';
    scancodeMap[0x46] = '(';
    scancodeMap[0x49] = '>';
    scancodeMap[0x4A] = '?';
    scancodeMap[0x4B] = 'L';
    scancodeMap[0x4C] = ':';
    scancodeMap[0x4D] = 'P';
    scancodeMap[0x4E] = '_';
    scancodeMap[0x52] = '"';
    scancodeMap[0x54] = '{';
    scancodeMap[0x55] = '+';
    scancodeMap[0x5A] = '\n';
    scancodeMap[0x5B] = '}';
    scancodeMap[0x5D] = '|';
}

void initKeyboard(void)
{
    initScanCodes();
    // disable ports
    outb(PS2_CMD, DISABLE_PORT_1);
    outb(PS2_CMD, DISABLE_PORT_2);
    // get current configuration
    outb(PS2_CMD, CMD_GET_CONFIGURATION);
    uint8_t config = getData();
    // write new configuration with bit 0,4 =0
    config = config & PORT1_INT_CLOCK;
    outb(PS2_CMD, CMD_SET_CONFIGURATION);
    sendData(config);
    //controller self test
    outb(PS2_CMD, CONTROLLER_TEST);
    uint8_t testResult = getData();
    //printk("Controller self test result: %d\n", testResult);
    // enable port 1
    outb(PS2_CMD, ENABLE_PORT_1);
    // reset and self test the keyboard
    sendData(RESET);
    testResult = getData();
    // check if testResult was ACK
    while (testResult != ACK)
    {
        if (testResult == RESEND)
        {
            sendData(RESET);
        }
        testResult = getData();
    }
    //printk("Keyboard self test result %d\n", testResult);
    /* // done testing
    // set scan code
    sendData(SCAN_CODE);
    sendData(SCAN_CODE_2);
    testResult = getData();
    // check if testResult was ACK
    // ie scan code was set
    while (testResult != ACK)
    {
        if (testResult == RESEND)
        {
            sendData(SCAN_CODE);
            sendData(SCAN_CODE_2);
        }
        testResult = getData();
    }
    printk("Scan code set result %d\n", testResult);
    // check scan code set
    sendData(SCAN_CODE);
    sendData(0);
    testResult=getData();
    while (testResult != ACK)
    {
        if (testResult == RESEND)
        {
            sendData(SCAN_CODE);
            sendData(0);
        }
        testResult = getData();
    }
    printk("Scan code set %d\n", getData());
     */// echo check
    sendData(ECHO);
    testResult = getData();
    // check if testResult was ECHO
    while (testResult != ECHO)
    {
        if (testResult == RESEND)
        {
            sendData(ECHO);
        }
        testResult = getData();
    }
    //printk("Echo recieved %d\n", testResult);
}
char pollKeyboard()
{
    // poll to check bit 0 of status register
    uint8_t status = inb(PS2_STATUS);
    while (!status & PS2_STATUS_OUTPUT)
    {
        status = inb(PS2_STATUS);
    }
    // bit 0 is now set
    uint8_t scanCode = inb(PS2_DATA);
    // get the character key pressed
    return scancodeMap[scanCode];
}
static uint8_t getData()
{
    uint8_t status = inb(PS2_STATUS);
    while (!(status & PS2_STATUS_OUTPUT))
    {
        status = inb(PS2_STATUS);
    }
    return inb(PS2_DATA);
}
static void sendData(uint8_t data)
{
    uint8_t status = inb(PS2_STATUS);
    while (status & PS2_STATUS_INPUT)
    {
        status = inb(PS2_STATUS);
    }
    outb(PS2_DATA, data);
}