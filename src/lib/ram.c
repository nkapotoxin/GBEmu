#include <ram.h>

static ram_context ctx;

u8 wram_read(u16 address) {
    address -= 0xC000;
    if (address > 0x2000) {
        printf("Invalid ram address %04X\n", address + 0xC000);
        exit(-8);
    }

    return ctx.wram[address];
}

void wram_write(u16 address, u8 value) {
    address -= 0xC000;

    ctx.wram[address] = value;
}

u8 hram_read(u16 address) {
    address -= 0xFF80;

    return ctx.hram[address];
}

void hram_write(u16 address, u8 value) {
    address -= 0xFF80;

    ctx.hram[address] = value;
}
