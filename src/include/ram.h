#include <common.h>

typedef struct {
    u8 wram[0x2000];
    u8 hram[0x80];
} ram_context;

u8 wram_read(u16 address);
void wram_write(u16 address, u8 value);

u8 hram_read(u16 address);
void hram_write(u16 address, u8 value);
