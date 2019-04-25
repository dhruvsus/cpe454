#include "printk.h"
#include "vga.h"
#include <stdarg.h>

int printk(const char *fmt, ...)
{
    va_list args;
    char str[100] = {'\0'};
    va_start(args, fmt);
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case '%':
                VGA_display_char('%');
                break;
            case 'c':
                VGA_display_char((char)va_arg(args, int));
                break;
            case 's':
                VGA_display_str(va_arg(args, const char *));
            case 'd':
                VGA_display_str(itoa(va_arg(args, int), str, 10));
                break;
            case 'u':
                VGA_display_str(itoa(va_arg(args, unsigned int), str, 10));
                break;
            case 'x':
                VGA_display_str("0x");
                VGA_display_str(itoa(va_arg(args, unsigned int), str, 16));
                break;
            case 'p':
                // print value at pointer
                VGA_display_str("0x");
                VGA_display_str(itoa(va_arg(args, long long int), str, 16));
            }
        }
        else
        {
            VGA_display_char(*fmt);
            fmt++;
        }
    }
}
void reverse(char *str, int length)
{
    int start = 0;
    int end = length - 1;
    char temp;
    while (start < end)
    {
        temp = *(str + start);
        *(str + start) = *(str + end);
        *(str + end) = temp;
        start++;
        end--;
    }
}
char *itoa(long long num, char *str, int base)
{
    int i = 0;
    int isNegative = 0;

    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}