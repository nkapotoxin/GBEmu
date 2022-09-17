#pragma once

#include <common.h>

u8 stack_pop();
u16 stack_pop16();

void stack_push(u8 value);
void stack_push16(u16 value);
