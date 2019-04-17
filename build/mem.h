#ifndef MEM_H
#define MEM_H
#include <stdint.h>
void* memset(void* destination, unsigned char value, int num_bytes);
void* memcpy(void* destination, const void* src, int num_bytes);
int memcmp(const void* one, const void* two, uint64_t num_bytes);
int strcmp(const void* one, const void* two);
void strncpy(void* destination, const void* src, int num_bytes);
#endif
