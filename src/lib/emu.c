#include <stdio.h>
#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include <ui.h>
#include <timer.h>
#include <dma.h>
#include <ppu.h>
#include <sound.h>

//TODO Add Windows Alternative...
#include <pthread.h>
#include <unistd.h>

static emu_context ctx;
pthread_t current_game;

emu_context *emu_get_context() {
    return &ctx;
}

void *cpu_run(void *p) {
    timer_init();
    cpu_init();
	ppu_init();
    sound_init(0, 0);

    ctx.running = true;
    ctx.paused = false;
    ctx.ticks = 0;

    while(ctx.running) {
        if (ctx.paused) {
            delay(10);
            continue;
        }

        if (!cpu_step()) {
            printf("CPU Stopped\n");
            return 0;
        }
    }

    return 0;
}

int emu_run(int argc, char **argv) {
    ui_init();
    ctx.die = false;
    u32 prev_frame = 0;
    while (!ctx.die) {
        usleep(1000);
        ui_handle_events();

        if (prev_frame != ppu_get_context()->current_frame) {
            ui_update();
        }
        prev_frame = ppu_get_context()->current_frame;
    }

    ctx.die = true;
    return 0;
}

int run_game(char *romfile) {
    ctx.running = false;
    usleep(1000);

    if (current_game) {
        //pthread_kill(current_game, 9);
        usleep(1000);
    }

    if (!cart_load(romfile)) {
        printf("Failed to load ROM file: %s\n", romfile);
        return -1;
    }

    if (pthread_create(&current_game, NULL, cpu_run, NULL)) {
        fprintf(stderr, "FAILED TO START MAIN CPU THREAD!\n");
        return -2;
    }

    return 0;
}

void emu_cycles(int cpu_cycles) {
    for (int i=0; i<cpu_cycles; i++) {
        for (int n=0; n<4; n++) {
            ctx.ticks++;
            timer_tick();
            ppu_tick();
        }
        
        sound_tick(2);
        dma_tick();
    }
}
