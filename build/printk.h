#ifndef PRINTK_H
#define PRINTK_H
#define BASE_DEC_ASCII 48
int printk(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#endif