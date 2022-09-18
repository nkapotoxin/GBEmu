#pragma once

#include <common.h>

typedef struct {
    //registers...
    u8 lcdc;
    u8 lcds;
    u8 scroll_y;
    u8 scroll_x;
    u8 ly;
    u8 ly_compare;
    u8 dma;
    u8 bg_palette;
    u8 obj_palette[2];
    u8 win_y;
    u8 win_x;

    //other data...
    u32 bg_colors[4];
    u32 sp1_colors[4];
    u32 sp2_colors[4];

} lcd_context;

typedef enum {
    MODE_HBLANK,
    MODE_VBLANK,
    MODE_OAM,
    MODE_XFER
} lcd_mode;

lcd_context *lcd_get_context();

/**
  FF41 - STAT - LCDC Status   (R/W)
    Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
    Bit 5 - Mode 2 OAM Interrupt         (1=Enable) (Read/Write)
    Bit 4 - Mode 1 V-Blank Interrupt     (1=Enable) (Read/Write)
    Bit 3 - Mode 0 H-Blank Interrupt     (1=Enable) (Read/Write)
    Bit 2 - Coincidence Flag  (0:LYC<>LY, 1:LYC=LY) (Read Only)
    Bit 1-0 - Mode Flag       (Mode 0-3, see below) (Read Only)
                0: During H-Blank
                1: During V-Blank
                2: During Searching OAM-RAM
                3: During Transfering Data to LCD Driver

    The two lower STAT bits show the current status of the LCD controller.
    Mode 0: The LCD controller is in the H-Blank period and
            the CPU can access both the display RAM (8000h-9FFFh)
            and OAM (FE00h-FE9Fh)

    Mode 1: The LCD contoller is in the V-Blank period (or the
            display is disabled) and the CPU can access both the
            display RAM (8000h-9FFFh) and OAM (FE00h-FE9Fh)

    Mode 2: The LCD controller is reading from OAM memory.
            The CPU <cannot> access OAM memory (FE00h-FE9Fh)
            during this period.

    Mode 3: The LCD controller is reading from both OAM and VRAM,
            The CPU <cannot> access OAM and VRAM during this period.
            CGB Mode: Cannot access Palette Data (FF69,FF6B) either.

    The following are typical when the display is enabled:
    Mode 2  2_____2_____2_____2_____2_____2___________________2____
    Mode 3  _33____33____33____33____33____33__________________3___
    Mode 0  ___000___000___000___000___000___000________________000
    Mode 1  ____________________________________11111111111111_____
 */

#define LCDC_BGW_ENABLE (BIT(lcd_get_context()->lcdc, 0))
#define LCDC_OBJ_ENABLE (BIT(lcd_get_context()->lcdc, 1))
#define LCDC_OBJ_HEIGHT (BIT(lcd_get_context()->lcdc, 2) ? 16 : 8)
#define LCDC_BG_MAP_AREA (BIT(lcd_get_context()->lcdc, 3) ? 0x9C00 : 0x9800)
#define LCDC_BGW_DATA_AREA (BIT(lcd_get_context()->lcdc, 4) ? 0x8000 : 0x8800)
#define LCDC_WIN_ENABLE (BIT(lcd_get_context()->lcdc, 5))
#define LCDC_WIN_MAP_AREA (BIT(lcd_get_context()->lcdc, 6) ? 0x9C00 : 0x9800)
#define LCDC_LCD_ENABLE (BIT(lcd_get_context()->lcdc, 7))

#define LCDS_MODE ((lcd_mode)(lcd_get_context()->lcds & 0b11))
#define LCDS_MODE_SET(mode) { lcd_get_context()->lcds &= ~0b11; lcd_get_context()->lcds |= mode; }

#define LCDS_LYC (BIT(lcd_get_context()->lcds, 2))
#define LCDS_LYC_SET(b) (BIT_SET(lcd_get_context()->lcds, 2, b))

typedef enum {
    SS_HBLANK = (1 << 3),
    SS_VBLANK = (1 << 4),
    SS_OAM = (1 << 5),
    SS_LYC = (1 << 6),
} stat_src;

#define LCDS_STAT_INT(src) (lcd_get_context()->lcds & src)

void lcd_init();

u8 lcd_read(u16 address);
void lcd_write(u16 address, u8 value);
