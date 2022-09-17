#pragma once

#include <common.h>

typedef enum {
    RT_NONE,
    RT_A,
    RT_F,
    RT_D,
    RT_E,
    RT_H,
    RT_L,
    RT_B,
    RT_C,
    RT_AF,
    RT_BC,
    RT_DE,
    RT_HL,
    RT_PC,
    RT_SP,
} reg_type;

typedef enum {
    AM_IMP,
    AM_NOP,
    AM_R,
    AM_R_R,
    AM_MR_R,
    AM_R_MR,
    AM_R_HLI,
    AM_R_HLD, 
    AM_HLI_R,
    AM_HLD_R,
    AM_R_D8,
    AM_R_D16,
    AM_HL_D8,
    AM_A8_R,
    AM_A16_R,
    AM_R_A16,
    AM_HL_SPR,
    AM_D8,
    AM_MR_D8,
    AM_MR,
    AM_R_A8,
    AM_D16_R,
    AM_D16,
} addr_mode;

typedef enum {
    IN_NONE,
    IN_NOP,
    IN_LD,
    IN_INC,
    IN_DEC,
    IN_RLCA,
    IN_ADD,
    IN_RRCA,
    IN_STOP,
    IN_RLA,
    IN_JP,
    IN_JR,
    IN_XOR,
    IN_DI,
    IN_HALT,
    IN_LDH,
    IN_POP,
    IN_PUSH,
    IN_CALL,
    IN_RET,
    IN_RETI,
    IN_RST,
    IN_ADC,
    IN_SUB,
    IN_SBC,
    IN_OR,
    IN_AND,
    IN_CP,
    IN_CB,
} in_type;

typedef enum {
    CT_NONE,
    CT_NZ,
    CT_Z,
    CT_NC,
    CT_C,
} cond_type;

typedef struct {
    in_type type;
    addr_mode mode;
    reg_type reg_1;
    reg_type reg_2;
    cond_type cond;
    u8 param;
} instruction;

instruction *instruction_by_opcode(u8 opcode);
char *inst_name(in_type type);
