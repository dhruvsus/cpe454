#ifndef PRINTK_H
#define PRINTK_H
#define BASE_DEC_ASCII 48
int printk(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void reverse(char *str, int length);
char *itoa(long long num, char *str, int base);
#endif