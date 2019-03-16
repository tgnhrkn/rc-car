#include "lib_shared.h"

void swap_char(uint8_t * x, uint8_t * y) {
  uint8_t t = *x;
  *x = *y;
  *y = t;
}

uint8_t byte(unsigned int x) {
  uint8_t y = 0;
  for(int i = 0; i < 8 && x; i++, x/=10)
    y |= (x%10==1 ? (1 << (i)) : 0);
   return y;
}