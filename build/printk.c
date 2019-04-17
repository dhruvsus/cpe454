#include "printk.h"
#include "vga.h"
#include <stdarg.h>
int printk(const char *fmt, ...)
{
    va_list args;
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
            }
        }
    }
}
