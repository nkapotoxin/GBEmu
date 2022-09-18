#pragma once

#include <common.h>

/**
 * 
    VRAM Sprite Attribute Table (OAM)

    GameBoy video controller can display up to 40 sprites either in 8x8 or in 8x16
    pixels. Because of a limitation of hardware, only ten sprites can be displayed
    per scan line. Sprite patterns have the same format as BG tiles, but they are
    taken from the Sprite Pattern Table located at $8000-8FFF and have unsigned
    numbering.

    Sprite attributes reside in the Sprite Attribute Table (OAM - Object Attribute
    Memory) at $FE00-FE9F. Each of the 40 entries consists of four bytes with the
    following meanings:

    Byte0 - Y Position
    Specifies the sprites vertical position on the screen (minus 16).
    An offscreen value (for example, Y=0 or Y>=160) hides the sprite.

    Byte1 - X Position
    Specifies the sprites horizontal position on the screen (minus 8).
    An offscreen value (X=0 or X>=168) hides the sprite, but the sprite
    still affects the priority ordering - a better way to hide a sprite is to set
    its Y-coordinate offscreen.

    Byte2 - Tile/Pattern Number
    Specifies the sprites Tile Number (00-FF). This (unsigned) value selects a
    tile from memory at 8000h-8FFFh. In CGB Mode this could be either in VRAM Bank
    0 or 1, depending on Bit 3 of the following byte.
    In 8x16 mode, the lower bit of the tile number is ignored. Ie. the upper 8x8
    tile is "NN AND FEh", and the lower 8x8 tile is "NN OR 01h".

    Byte3 - Attributes/Flags:
    Bit7   OBJ-to-BG Priority (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
            (Used for both BG and Window. BG color 0 is always behind OBJ)
    Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
    Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
    Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
    Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
    Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)

    Sprite Priorities and Conflicts
    When sprites with different x coordinate values overlap, the one with the
    smaller x coordinate (closer to the left) will have priority and appear above
    any others. This applies in Non CGB Mode only.
    When sprites with the same x coordinate values overlap, they have priority
    according to table ordering. (i.e. $FE00 - highest, $FE04 - next highest,
    etc.) In CGB Mode priorities are always assigned like this.

    Only 10 sprites can be displayed on any one line. When this limit is exceeded,
    the lower priority sprites (priorities listed above) won't be displayed. To
    keep unused sprites from affecting onscreen sprites set their Y coordinate to
    Y=0 or Y=>144+16. Just setting the X coordinate to X=0 or X=>160+8 on a sprite
    will hide it but it will still affect other sprites sharing the same lines.

    Writing Data to OAM Memory
    The recommened method is to write the data to normal RAM first, and to copy
    that RAM to OAM by using the DMA transfer function, initiated through DMA
    register (FF46).
    Beside for that, it is also possible to write data directly to the OAM area by
    using normal LD commands, this works only during the H-Blank and V-Blank
    periods. The current state of the LCD controller can be read out from the STAT
    register (FF41).
 */

typedef struct {
    u8 y;
    u8 x;
    u8 tile;

    u8 f_cgb_pn : 3;
    u8 f_cgb_vram_bank : 1;
    u8 f_pn : 1;
    u8 f_x_flip : 1;
    u8 f_y_flip : 1;
    u8 f_bgp : 1;

} oam_entry;

typedef struct {
    oam_entry oam_ram[40];
    u8 vram[0x2000];
} ppu_context;

void ppu_init();
void ppu_tick();

void ppu_oam_write(u16 address, u8 value);
u8 ppu_oam_read(u16 address);

void ppu_vram_write(u16 address, u8 value);
u8 ppu_vram_read(u16 address);
