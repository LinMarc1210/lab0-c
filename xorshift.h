#ifndef LAB0_XORSHIFT_H
#define LAB0_XORSHIFT_H

#include <stdint.h>

void xorshift64_init(uint64_t seed);
uint64_t xorshift64_rand(void);

#endif  // LAB0_XORSHIFT_H