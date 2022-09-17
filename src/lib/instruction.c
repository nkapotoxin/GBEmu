#include <common.h>
#include <instruction.h>

instruction instructions[0x100] = {
    [0x00] = {IN_NOP, AM_NOP},

    [0x05] = {IN_DEC, AM_R, RT_B},

    [0x0E] = {IN_LD, AM_R_D8, RT_C},

    [0xAF] = {IN_XOR, AM_R, RT_A},

    [0xC3] = {IN_JP, AM_D16},

    [0xF3] = {IN_DI}
};

instruction *instruction_by_opcode(u8 opcode) {
    return &instructions[opcode];
}

char *inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JP",
    "JR",
    "XOR",
    "DI",
};

char *inst_name(in_type type) {
    return inst_lookup[type];
}