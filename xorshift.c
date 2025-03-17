#include <stdint.h>
#include <time.h>

#include "xorshift.h"

static uint64_t xorshift64_state = 0;

void xorshift64_init(uint64_t seed)
{
    if (seed == 0)
        seed = (uint64_t) time(NULL);
    ;
    xorshift64_state = seed;
}

uint64_t xorshift64_rand(void)
{
    xorshift64_state ^= xorshift64_state << 13;
    xorshift64_state ^= xorshift64_state >> 7;
    xorshift64_state ^= xorshift64_state << 17;
    return xorshift64_state;
}