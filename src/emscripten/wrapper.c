#include <cart.h>
#include <timer.h>
#include <cpu.h>
#include <ppu.h>
#include <dma.h>
#include <sound.h>
#include <gamepad.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    bool paused;
    bool running;
    bool die;
    u64 ticks;
} emu_context;

struct Emulator {
    emu_context *emu_ctx;
    cart_context *cart_ctx;
    timer_context *time_ctx;
    cpu_context *cpu_ctx;
    ppu_context *ppu_ctx;
    gamepad_context *gamepad_ctx;
    sound_context *sound_ctx;
    u32 event;
};

typedef struct {
  u8* data;
  size_t size;
} FileData;

typedef struct Emulator Emulator;

static emu_context emu_ctx;
static Emulator* e;

emu_context *emu_get_context() {
    return &emu_ctx;
}

void emu_cycles(int cpu_cycles) {
    for (int i=0; i<cpu_cycles; i++) {
        for (int n=0; n<4; n++) {
            emu_ctx.ticks++;
            timer_tick();
            ppu_tick();
        }

        sound_tick(2);
        dma_tick();
    }
}

u64 get_ticks() {
    return emu_ctx.ticks;
}

void delay(u32 ms) {
    usleep(1000 * ms);
}

void emu_init() {
    emu_ctx.running = true;
    emu_ctx.paused = false;
    emu_ctx.ticks = 0;
}

Emulator* emulator_new(void* rom_data, size_t rom_size,
                       int audio_frequency, int audio_frames) {
    e = calloc(1, sizeof(Emulator));
    e->emu_ctx = emu_get_context();
    e->cart_ctx = cart_get_context();
    e->time_ctx = timer_get_context();
    e->cpu_ctx = cpu_get_context();
    e->ppu_ctx = ppu_get_context();
    e->gamepad_ctx = gamepad_get_context();
    e->sound_ctx = sound_get_context();

    if (!cart_init(rom_data, rom_size)) {
        printf("Failed to load ROM file");
        return NULL;
    }

    timer_init();
    cpu_init();
    ppu_init();
    emu_init();
    sound_init(audio_frequency, audio_frames);

    return e;
}

void emulator_delete(Emulator *e) {
    if (e) {
        free(e);
    }
}

bool emulator_read_ext_ram(Emulator *e, const FileData *file_data) {
    if (!e->cart_ctx->battery || !e->cart_ctx->ram_bank) {
        return 1;
    }

    memcpy(e->cart_ctx->ram_bank, file_data->data, file_data->size);
    return 1;
}

bool emulator_write_ext_ram(Emulator *e, const FileData *file_data) {
    if (!e->cart_ctx->ext_ram) {
        return 1;
    }

    memcpy(file_data->data, e->cart_ctx->ext_ram, file_data->size);
    return 1;
}

double emulator_get_ticks(Emulator *e) {
    return (double)e->emu_ctx->ticks;
}

u8 emulator_run_until(Emulator *e, double until_ticks) {
    u64 until_ticks_u = (u64) until_ticks;
    u32 prev_frame = e->ppu_ctx->current_frame;
    e->event = 0x0;
    while(e->emu_ctx->running && (e->event == 0)) {
        if (e->emu_ctx->ticks > until_ticks_u) {
            e->event |= 0x4;
        }

        if (e->sound_ctx->pos >= e->sound_ctx->frames * 2 + e->sound_ctx->skip_frames * 2) {
            e->event |= 0x2;
            e->sound_ctx->pos = 0;
        }

        // run one cpu step
        cpu_step();

        if (prev_frame != e->ppu_ctx->current_frame) {
            e->event |= 0x1;
        }
    }

    return e->event;
}

bool emulator_was_ext_ram_updated(Emulator *e) {
    bool result = e->cart_ctx->need_save;
    if (e->cart_ctx->need_save) {
        cart_save_ext_ram();
    }

    e->cart_ctx->need_save = false;
    return result;
}

FileData* ext_ram_file_data_new(Emulator *e) {
    FileData* file_data = malloc(sizeof(FileData));
    e->cart_ctx->ext_ram_size = 0x2000;
    file_data->size = e->cart_ctx->ext_ram_size;
    file_data->data = malloc(file_data->size);
    return file_data;
}

void* get_file_data_ptr(FileData *file_data) {
    return file_data->data;
}

size_t get_file_data_size(FileData* file_data) {
  return file_data->size;
}

void file_data_delete(FileData* file_data) {
    free(file_data->data);
    free(file_data);
}

void* get_audio_buffer_ptr(Emulator* e) {
    return e->sound_ctx->buf;
}

size_t get_audio_buffer_capacity(Emulator* e) {
    return e->sound_ctx->len;
}

void* get_frame_buffer_ptr(Emulator* e) {
    return e->ppu_ctx->video_buffer;
}

size_t get_frame_buffer_size(Emulator* e) {
    return YRES * XRES * sizeof(u32);
}

void set_joyp_down(Emulator *e, bool set) {
    e->gamepad_ctx->controller.down = set;
}
void set_joyp_up(Emulator *e, bool set) {
    e->gamepad_ctx->controller.up = set;
}
void set_joyp_left(Emulator *e, bool set) {
    e->gamepad_ctx->controller.left = set;
}
void set_joyp_right(Emulator *e, bool set) {
    e->gamepad_ctx->controller.right = set;
}
void set_joyp_select(Emulator *e, bool set) {
    e->gamepad_ctx->controller.select = set;
}
void set_joyp_start(Emulator *e, bool set) {
    e->gamepad_ctx->controller.start = set;
}
void set_joyp_b(Emulator *e, bool set) {
    e->gamepad_ctx->controller.b = set;
}
void set_joyp_a(Emulator *e, bool set) {
    e->gamepad_ctx->controller.a = set;
}