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

// for audio
#define RI_NR10 0x10
#define RI_NR11 0x11
#define RI_NR12 0x12
#define RI_NR13 0x13
#define RI_NR14 0x14
#define RI_NR21 0x16
#define RI_NR22 0x17
#define RI_NR23 0x18
#define RI_NR24 0x19
#define RI_NR30 0x1A
#define RI_NR31 0x1B
#define RI_NR32 0x1C
#define RI_NR33 0x1D
#define RI_NR34 0x1E
#define RI_NR41 0x20
#define RI_NR42 0x21
#define RI_NR43 0x22
#define RI_NR44 0x23
#define RI_NR50 0x24
#define RI_NR51 0x25
#define RI_NR52 0x26

#define R_NR10 ctx.snd_mem[(RI_NR10)]
#define R_NR11 ctx.snd_mem[(RI_NR11)]
#define R_NR12 ctx.snd_mem[(RI_NR12)]
#define R_NR13 ctx.snd_mem[(RI_NR13)]
#define R_NR14 ctx.snd_mem[(RI_NR14)]
#define R_NR21 ctx.snd_mem[(RI_NR21)]
#define R_NR22 ctx.snd_mem[(RI_NR22)]
#define R_NR23 ctx.snd_mem[(RI_NR23)]
#define R_NR24 ctx.snd_mem[(RI_NR24)]
#define R_NR30 ctx.snd_mem[(RI_NR30)]
#define R_NR31 ctx.snd_mem[(RI_NR31)]
#define R_NR32 ctx.snd_mem[(RI_NR32)]
#define R_NR33 ctx.snd_mem[(RI_NR33)]
#define R_NR34 ctx.snd_mem[(RI_NR34)]
#define R_NR41 ctx.snd_mem[(RI_NR41)]
#define R_NR42 ctx.snd_mem[(RI_NR42)]
#define R_NR43 ctx.snd_mem[(RI_NR43)]
#define R_NR44 ctx.snd_mem[(RI_NR44)]
#define R_NR50 ctx.snd_mem[(RI_NR50)]
#define R_NR51 ctx.snd_mem[(RI_NR51)]
#define R_NR52 ctx.snd_mem[(RI_NR52)]