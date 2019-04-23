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
                break;
            case 'd':
                break;
            }
        }
    }
}
void reverse(char* str, int length){
    int start=0;
    int end=length-1;
    char temp;
    while(start<end){
        temp=*(str+start);
        *(str+start)=*(str+end);
        *(str+end)=temp;
        start++;
        end--;
    }
}

void print_char(char c)
{
    VGA_display_char(c);
}
char* itoa(int num, char* str, int base) 
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
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0'; // Append string terminator 
  
    // Reverse the string 
    reverse(str, i); 
  
    return str; 
}
void print_str(const char *c)
{
    //itoa
    //seperate for unsigned
    //speerate for hex
}