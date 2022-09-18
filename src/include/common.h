#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

void delay(u32 ms);
u32 get_ticks();

#define BIT(a, n) ((a & ( 1 << n )) ? 1 : 0)

#define BIT_SET(a, n, on) { if (on) a |= (1 << n); else a &= ~(1 << n);}

#define BETWEEN(a, b, c) ((a >= b) && (a <= c))


#define NO_IMPL { fprintf(stderr, "Not implemented yet.\n"); exit(-5); }
