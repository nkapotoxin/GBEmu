#include <cart.h>
#include <string.h>

static cart_context ctx;

cart_context *cart_get_context() {
    return &ctx;
}

bool cart_need_save() {
    return ctx.need_save;
}

bool cart_mbc1() {
    return BETWEEN(ctx.header->cartiage_type, 1, 3);
}

bool cart_battery() {
    //mbc1 only for now...
    return ctx.header->cartiage_type == 3;
}

static const char *ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "MBC2",
    "MBC2+BATTERY",
    "ROM+RAM",
    "ROM+RAM+BATTERY",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY",
    "MBC3",
    "MBC3+RAM",
    "MBC3+RAM+BATTERY",
    "MBC4",
    "MBC4+RAM",
    "MBC4+RAM+BATTERY",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "POCKET CAMERA",
    "BANDAI TAMA5",
    "HuC3",
    "HuC1+RAM+BATTERY"
};

static const char *LIC_CODE[0xA5] = {
    [0x00] = "None",
    [0x01] = "Nintendo R&D1",
    [0x08] = "Capcom",
    [0x13] = "Electronic Arts",
    [0x18] = "Hudson Soft",
    [0x19] = "b-ai",
    [0x20] = "kss",
    [0x22] = "pow",
    [0x24] = "PCM Complete",
    [0x25] = "san-x",
    [0x28] = "Kemco Japan",
    [0x29] = "seta",
    [0x30] = "Viacom",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Ocean/Acclaim",
    [0x34] = "Konami",
    [0x35] = "Hector",
    [0x37] = "Taito",
    [0x38] = "Hudson",
    [0x39] = "Banpresto",
    [0x41] = "Ubi Soft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "angel",
    [0x47] = "Bullet-Proof",
    [0x49] = "irem",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American sammy",
    [0x54] = "Konami",
    [0x55] = "Hi tech entertainment",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x58] = "Mattel",
    [0x59] = "Milton Bradley",
    [0x60] = "Titus",
    [0x61] = "Virgin",
    [0x64] = "LucasArts",
    [0x67] = "Ocean",
    [0x69] = "Electronic Arts",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "sculptured",
    [0x75] = "sci",
    [0x78] = "THQ",
    [0x79] = "Accolade",
    [0x80] = "misawa",
    [0x83] = "lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x87] = "Tsukuda Original",
    [0x91] = "Chunsoft",
    [0x92] = "Video system",
    [0x93] = "Ocean/Acclaim",
    [0x95] = "Varie",
    [0x96] = "Yonezawa/s’pal",
    [0x97] = "Kaneko",
    [0x99] = "Pack in soft",
    [0xA4] = "Konami (Yu-Gi-Oh!)"
};

const char *cart_lic_name() {
    if (ctx.header->new_license_code <= 0xA4) {
        return LIC_CODE[ctx.header->old_license_code];
    }

    return "UNKNOWN";
}

const char *cart_type_name() {
    if (ctx.header->cartiage_type <= 0x22) {
        return ROM_TYPES[ctx.header->cartiage_type];
    }

    return "UNKNOWN";
}

void cart_setup_banking() {
    for (int i=0; i<16; i++) {
        ctx.ram_banks[i] = 0;

        if ((ctx.header->ram_size == 2 && i == 0) ||
            (ctx.header->ram_size == 3 && i < 4) || 
            (ctx.header->ram_size == 4 && i < 16) || 
            (ctx.header->ram_size == 5 && i < 8)) {
            ctx.ram_banks[i] = malloc(0x2000);
            memset(ctx.ram_banks[i], 0, 0x2000);
        }
    }

    ctx.ram_bank = ctx.ram_banks[0];
    ctx.rom_bank_x = ctx.rom_data + 0x4000; //rom bank 1
}

void cart_save_ext_ram() {
    if (!ctx.ram_bank) {
        return;
    }

    memcpy(ctx.ext_ram, ctx.ram_bank, ctx.ext_ram_size);
}


void cart_load_ext_ram() {
    if (!ctx.ram_bank) {
        return;
    }

    memcpy(ctx.ram_bank, ctx.ext_ram, ctx.ext_ram_size);
}

bool cart_init(void* rom_data, size_t rom_size) {
    ctx.rom_size = rom_size;
    ctx.rom_data = rom_data;

    ctx.header = (rom_header *)(ctx.rom_data + 0x100);
    ctx.header->title[15] = 0;
    ctx.battery = cart_battery();
    ctx.need_save = false;
    ctx.ext_ram_size = 0;

    printf("Cartridge loaded:\n");
    printf("\t Title        : %s\n", ctx.header->title);
    printf("\t Type         : %2.2X (%s)\n", ctx.header->cartiage_type, cart_type_name());
    printf("\t Rom Size     : %2.2X %d KB\n", ctx.header->rom_size, 32 << ctx.header->rom_size);
    printf("\t Ram Size     : %2.2X\n", ctx.header->ram_size);
    printf("\t Lic Code     : %2.2X (%s)\n", ctx.header->new_license_code, cart_lic_name());
    printf("\t Rom Version  : %2.2X\n", ctx.header->mask_rom_version_number);

	cart_setup_banking();

    // Check sum
    u16 x = 0;
    for (u16 i=0x0134; i<=0x014C; i++) {
        x = x - ctx.rom_data[i] - 1;
    }
    printf("\t CheckSum: %s\n", (x & 0xFF)? "PASSED":"FAILED");

    return true;
}

bool cart_load(char *cart) {
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart);

    FILE *fp = fopen(cart, "r");

    if (!fp) {
        printf("Failed to open file: %s\n", ctx.filename);
        return false;
    }

    printf("Opened file: %s\n", ctx.filename);

    fseek(fp, 0, SEEK_END);
    ctx.rom_size = ftell(fp);

    rewind(fp);

    ctx.rom_data = malloc(ctx.rom_size);
    fread(ctx.rom_data, ctx.rom_size, 1, fp);
    fclose(fp);

    ctx.header = (rom_header *)(ctx.rom_data + 0x100);

    // TODO(nkaptx)
    ctx.header->title[15] = 0;
    ctx.battery = cart_battery();
    ctx.need_save = false;

    printf("Cartridge loaded:\n");
    printf("\t Title        : %s\n", ctx.header->title);
    printf("\t Type         : %2.2X (%s)\n", ctx.header->cartiage_type, cart_type_name());
    printf("\t Rom Size     : %2.2X %d KB\n", ctx.header->rom_size, 32 << ctx.header->rom_size);
    printf("\t Ram Size     : %2.2X\n", ctx.header->ram_size);
    printf("\t Lic Code     : %2.2X (%s)\n", ctx.header->new_license_code, cart_lic_name());
    printf("\t Rom Version  : %2.2X\n", ctx.header->mask_rom_version_number);

	cart_setup_banking();

    // Check sum
    u16 x = 0;
    for (u16 i=0x0134; i<=0x014C; i++) {
        x = x - ctx.rom_data[i] - 1;
    }
    printf("\t CheckSum: %s\n", (x & 0xFF)? "PASSED":"FAILED");

    if (ctx.battery) {
        cart_battery_load();
    }
    return true;
}

void cart_battery_load() {
    if (!ctx.ram_bank) {
        return;
    }

    char fn[1048];
    sprintf(fn, "%s.battery", ctx.filename);
    FILE *fp = fopen(fn, "rb");

    if (!fp) {
        fprintf(stderr, "FAILED TO OPEN: %s\n", fn);
        return;
    }

    fread(ctx.ram_bank, 0x2000, 1, fp);
    fclose(fp);
}

void cart_battery_save() {
    if (!ctx.ram_bank) {
        return;
    }

    char fn[1048];
    sprintf(fn, "%s.battery", ctx.filename);
    FILE *fp = fopen(fn, "wb");

    if (!fp) {
        fprintf(stderr, "FAILED TO OPEN: %s\n", fn);
        return;
    }

    fwrite(ctx.ram_bank, 0x2000, 1, fp);
    fclose(fp);
}

u8 cart_read(u16 address) {
    if (!cart_mbc1() || address < 0x4000) {
        return ctx.rom_data[address];
    }

    if ((address & 0xE000) == 0xA000) {
        if (!ctx.ram_enabled) {
            return 0xFF;
        }

        if (!ctx.ram_bank) {
            return 0xFF;
        }

        return ctx.ram_bank[address - 0xA000];
    }

    return ctx.rom_bank_x[address - 0x4000];
}

void cart_write(u16 address, u8 value) {
    if (!cart_mbc1()) {
        return;
    }

    if (address < 0x2000) {
        ctx.ram_enabled = ((value & 0xF) == 0xA);
    }

    if ((address & 0xE000) == 0x2000) {
        //rom bank number
        if (value == 0) {
            value = 1;
        }

        value &= 0b11111;

        ctx.rom_bank_value = value;
        ctx.rom_bank_x = ctx.rom_data + (0x4000 * ctx.rom_bank_value);
    }

    if ((address & 0xE000) == 0x4000) {
        //ram bank number
        ctx.ram_bank_value = value & 0b11;

        if (ctx.ram_banking) {
            if (cart_need_save()) {
                cart_save_ext_ram();
            }

            ctx.ram_bank = ctx.ram_banks[ctx.ram_bank_value];
        }
    }

    if ((address & 0xE000) == 0x6000) {
        //banking mode select
        ctx.banking_mode = value & 1;

        ctx.ram_banking = ctx.banking_mode;

        if (ctx.ram_banking) {
            if (cart_need_save()) {
                cart_save_ext_ram();
            }

            ctx.ram_bank = ctx.ram_banks[ctx.ram_bank_value];
        }
    }

    if ((address & 0xE000) == 0xA000) {
        if (!ctx.ram_enabled) {
            return;
        }

        if (!ctx.ram_bank) {
            return;
        }

        ctx.ram_bank[address - 0xA000] = value;

        if (ctx.battery) {
            ctx.need_save = true;
        }
    }
}
