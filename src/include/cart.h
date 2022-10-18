#pragma once

#include <common.h>

typedef struct {
    /**
        0100-0103 - Entry Point
        After displaying the Nintendo Logo, the built-in boot procedure jumps to this
        address (100h), which should then jump to the actual main program in the
        cartridge. Usually this 4 byte area contains a NOP instruction, followed by a
        JP 0150h instruction. But not always. 
     */
    u8 entry_point[0x4];
    /**
        0104-0133 - Nintendo Logo
        These bytes define the bitmap of the Nintendo logo that is displayed when the
        gameboy gets turned on. The hexdump of this bitmap is:
        CE ED 66 66 CC 0D 00 0B 03 73 00 83 00 0C 00 0D
        00 08 11 1F 88 89 00 0E DC CC 6E E6 DD DD D9 99
        BB BB 67 63 6E 0E EC CC DD DC 99 9F BB B9 33 3E
        The gameboys boot procedure verifies the content of this bitmap (after it has
        displayed it), and LOCKS ITSELF UP if these bytes are incorrect. A CGB
        verifies only the first 18h bytes of the bitmap, but others (for example a
        pocket gameboy) verify all 30h bytes.
     */
    u8 nintendo_logo[0x30];
    /**
        0134-0143 - Title
        Title of the game in UPPER CASE ASCII. If it is less than 16 characters then
        the remaining bytes are filled with 00's. When inventing the CGB, Nintendo has
        reduced the length of this area to 15 characters, and some months later they
        had the fantastic idea to reduce it to 11 characters only. The new meaning of
        the ex-title bytes is described below.
     */
    char title[0x10];
    /**
        0144-0145 - New Licensee Code
        Specifies a two character ASCII licensee code, indicating the company or
        publisher of the game. These two bytes are used in newer games only (games
        that have been released after the SGB has been invented). Older games are
        using the header entry at 014B instead.
     */
    u16 new_license_code;
    /**
        0146 - SGB Flag
        Specifies whether the game supports SGB functions, common values are:
        00h = No SGB functions (Normal Gameboy or CGB only game)
        03h = Game supports SGB functions
        The SGB disables its SGB functions if this byte is set to another value than
        03h.
     */
    u8 sgb_flag;
    /**
        0147 - Cartridge Type
        Specifies which Memory Bank Controller (if any) is used in the cartridge, and
        if further external hardware exists in the cartridge.
        00h  ROM ONLY                 13h  MBC3+RAM+BATTERY
        01h  MBC1                     15h  MBC4
        02h  MBC1+RAM                 16h  MBC4+RAM
        03h  MBC1+RAM+BATTERY         17h  MBC4+RAM+BATTERY
        05h  MBC2                     19h  MBC5
        06h  MBC2+BATTERY             1Ah  MBC5+RAM
        08h  ROM+RAM                  1Bh  MBC5+RAM+BATTERY
        09h  ROM+RAM+BATTERY          1Ch  MBC5+RUMBLE
        0Bh  MMM01                    1Dh  MBC5+RUMBLE+RAM
        0Ch  MMM01+RAM                1Eh  MBC5+RUMBLE+RAM+BATTERY
        0Dh  MMM01+RAM+BATTERY        FCh  POCKET CAMERA
        0Fh  MBC3+TIMER+BATTERY       FDh  BANDAI TAMA5
        10h  MBC3+TIMER+RAM+BATTERY   FEh  HuC3
        11h  MBC3                     FFh  HuC1+RAM+BATTERY
        12h  MBC3+RAM
     */
    u8 cartiage_type;
    /**
        0148 - ROM Size
        Specifies the ROM Size of the cartridge. Typically calculated as "32KB shl N".
        00h -  32KByte (no ROM banking)
        01h -  64KByte (4 banks)
        02h - 128KByte (8 banks)
        03h - 256KByte (16 banks)
        04h - 512KByte (32 banks)
        05h -   1MByte (64 banks)  - only 63 banks used by MBC1
        06h -   2MByte (128 banks) - only 125 banks used by MBC1
        07h -   4MByte (256 banks)
        52h - 1.1MByte (72 banks)
        53h - 1.2MByte (80 banks)
        54h - 1.5MByte (96 banks)
     */
    u8 rom_size;
    /**
        0149 - RAM Size
        Specifies the size of the external RAM in the cartridge (if any).
        00h - None
        01h - 2 KBytes
        02h - 8 Kbytes
        03h - 32 KBytes (4 banks of 8KBytes each)
        When using a MBC2 chip 00h must be specified in this entry, even though the
        MBC2 includes a built-in RAM of 512 x 4 bits.
     */
    u8 ram_size;
    /**
        014A - Destination Code
        Specifies if this version of the game is supposed to be sold in japan, or
        anywhere else. Only two values are defined.
        00h - Japanese
        01h - Non-Japanese
     */
    u8 destination_code;
    /**
        014B - Old Licensee Code
        Specifies the games company/publisher code in range 00-FFh. A value of 33h
        signalizes that the New License Code in header bytes 0144-0145 is used
        instead.
        (Super GameBoy functions won't work if <> $33.)
     */
    u8 old_license_code;
    /**
        014C - Mask ROM Version number
        Specifies the version number of the game. That is usually 00h.
     */
    u8 mask_rom_version_number;
    /**
        014D - Header Checksum
        Contains an 8 bit checksum across the cartridge header bytes 0134-014C. The
        checksum is calculated as follows:
        x=0:FOR i=0134h TO 014Ch:x=x-MEM[i]-1:NEXT
        The lower 8 bits of the result must be the same than the value in this entry.
        The GAME WON'T WORK if this checksum is incorrect.
     */
    u8 header_checksum;
    /**
        014E-014F - Global Checksum
        Contains a 16 bit checksum (upper byte first) across the whole cartridge ROM.
        Produced by adding all bytes of the cartridge (except for the two checksum
        bytes). The Gameboy doesn't verify this checksum.
     */
    u16 global_checksum;
} rom_header;

typedef struct {
    char filename[1024];
    u32 rom_size;
    u8 *rom_data;
    rom_header *header;

    //mbc1 related data
    bool ram_enabled;
    bool ram_banking;

    u8 *rom_bank_x;
    u8 banking_mode;

    u8 rom_bank_value;
    u8 ram_bank_value;

    u8 *ram_bank; //current selected ram bank
    u8 *ram_banks[16]; //all ram banks

    //for battery
    bool battery; //has battery
    bool need_save; //should save battery backup.

    //battery
    u8 ext_ram[(size_t)(128) * 1024];
    u32 ext_ram_size;
} cart_context;

cart_context *cart_get_context();

bool cart_init(void* rom_data, size_t rom_size);
bool cart_load(char *cart);

u8 cart_read(u16 address);
void cart_write(u16 address, u8 value);

void cart_battery_load();
void cart_battery_save();
bool cart_need_save();
void cart_save_ext_ram();
void cart_load_ext_ram();
