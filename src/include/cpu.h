#pragma once

#include <common.h>
#include <instruction.h>

/**
    The Flag Register (lower 8bit of AF register)
    Bit  Name  Set Clr  Expl.
    7    zf    Z   NZ   Zero Flag
    6    n     -   -    Add/Sub-Flag (BCD)
    5    h     -   -    Half Carry Flag (BCD)
    4    cy    C   NC   Carry Flag
    3-0  -     -   -    Not used (always zero)
    Conatins the result from the recent instruction which has affected flags.

    The Zero Flag (Z)
    This bit becomes set (1) if the result of an operation has been zero (0). Used
    for conditional jumps.

    The Carry Flag (C, or Cy)
    Becomes set when the result of an addition became bigger than FFh (8bit) or
    FFFFh (16bit). Or when the result of a subtraction or comparision became less
    than zero (much as for Z80 and 80x86 CPUs, but unlike as for 65XX and ARM
    CPUs). Also the flag becomes set when a rotate/shift operation has shifted-out
    a "1"-bit.
    Used for conditional jumps, and for instructions such like ADC, SBC, RL, RLA,
    etc.

    The BCD Flags (N, H)
    These flags are (rarely) used for the DAA instruction only, N Indicates
    whether the previous instruction has been an addition or subtraction, and H
    indicates carry for lower 4bits of the result, also for DAA, the C flag must
    indicate carry for upper 8bits.
    After adding/subtracting two BCD numbers, DAA is intended to convert the
    result into BCD format; BCD numbers are ranged from 00h to 99h rather than 00h
    to FFh.
    Because C and H flags must contain carry-outs for each digit, DAA cannot be
    used for 16bit operations (which have 4 digits), or for INC/DEC operations
    (which do not affect C-flag).
 */
typedef struct {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;
} cpu_registers;

typedef struct {
    cpu_registers regs;

    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 cur_opcode;
    instruction *cur_inst;

    bool halted;
    bool stepping;
} cpu_context;

void cpu_init();

bool cpu_step();
