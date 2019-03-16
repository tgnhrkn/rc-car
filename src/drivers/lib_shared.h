#ifndef LIB_SHARED_H
#define LIB_SHARED_H

#include <stdint.h>

#define BIT(x) (1 << (x))

#define bit(b) (1UL << (b))

void swap_char(uint8_t *, uint8_t *);
uint8_t byte(unsigned int);

#endif	// LIB_SHARED_H
