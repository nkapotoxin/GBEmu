#include <bus.h>
#include <cart.h>
#include <ram.h>
#include <cpu.h>
#include <gbio.h>

/**
General Memory Map
  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
  FE00-FE9F   Sprite Attribute Table (OAM)
  FEA0-FEFF   Not Usable
  FF00-FF7F   I/O Ports
  FF80-FFFE   High RAM (HRAM)
  FFFF        Interrupt Enable Register
 */

u8 bus_read(u16 address) {
    if (address < 0x8000) {
        return cart_read(address);
    } else if (address < 0xA000) {
        // Vedio RAM
        // TODO
        printf("Unimplement yet! bus read %04X\n", address);
        NO_IMPL
    } else if (address < 0xC000) {
        return cart_read(address);
    } else if (address < 0xE000) {
        // Work RAM
        // TODO
        return wram_read(address);
    } else if (address < 0xFE00) {
        //  Echo RAM
        return 0;
    } else if (address < 0xFEA0) {
        // OAM
        // TODO
        printf("Unimplement yet! bus read %04X\n", address);
        return 0x0;
    } else if (address < 0xFF00) {
        return 0;
    } else if (address < 0xFF80) {
        // IO port
        return io_read(address);
    } else if (address == 0xFFFF) {
        return cpu_get_ie_register();
    }

    return hram_read(address);
}

void bus_write(u16 address, u8 value) {
    if (address < 0x8000) {
        cart_write(address, value);
    } else if (address < 0xA000) {
        // Vedio RAM
        // TODO
        printf("Unimplement yet! bus read %04X\n", address);
    } else if (address < 0xC000) {
        cart_write(address, value);
    } else if (address < 0xE000) {
        // Work RAM
        wram_write(address, value);
    } else if (address < 0xFE00) {
        //  Echo RAM
    } else if (address < 0xFEA0) {
        // OAM
        // TODO
        printf("Unimplement yet! bus read %04X\n", address);
    } else if (address < 0xFF00) {

    } else if (address < 0xFF80) {
        // IO Port
        io_write(address, value);
    } else if (address == 0xFFFF) {
        cpu_set_ie_register(value);
    } else {
        hram_write(address, value);
    }
}

u16 bus_read16(u16 address) {
    u16 lo = bus_read(address);
    u16 hi = bus_read(address + 1);
    
    return (hi << 8) | lo;
}

void bus_write16(u16 address, u16 value) {
    bus_write(address + 1, (value >> 8) & 0xFF);
    bus_write(address, value & 0xFF);
}