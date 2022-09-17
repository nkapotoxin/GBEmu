#include <stack.h>
#include <cpu.h>
#include <bus.h>

u8 stack_pop() {
    return bus_read(cpu_get_regs()->sp++);
}

void stack_push(u8 value) {
    cpu_get_regs()->sp--;
    
    bus_write(cpu_get_regs()->sp, value);
}

u16 stack_pop16() {
    u16 lo = stack_pop();
    u16 hi = stack_pop();

    return (hi << 8) | lo;
}

void stack_push16(u16 value) {
    stack_push((value >> 8) | 0xFF);
    stack_push(value & 0xFF);
}
