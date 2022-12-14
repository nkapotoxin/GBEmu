#include <gamepad.h>
#include <string.h>

static gamepad_context ctx = {0};

gamepad_context *gamepad_get_context() {
    return &ctx;
}

bool gamepad_button_sel() {
    return ctx.button_sel;
}

bool gamepad_dir_sel() {
    return ctx.dir_sel;
}

void gamepad_set_sel(u8 value) {
    ctx.button_sel = value & 0x20;
    ctx.dir_sel = value & 0x10;
}

gamepad_state *gamepad_get_state() {
    return &ctx.controller;
}

u8 gamepad_get_output() {
    u8 output = 0xCF;

    if (!gamepad_button_sel()) {
        if (gamepad_get_state()->start) {
            output &= ~(1 << 3);
        } else if (gamepad_get_state()->select) {
            output &= ~(1 << 2);
        } else if (gamepad_get_state()->a) {
            output &= ~(1 << 0);
        } else if (gamepad_get_state()->b) {
            output &= ~(1 << 1);
        }
    }

    if (!gamepad_dir_sel()) {
        if (gamepad_get_state()->left) {
            output &= ~(1 << 1);
        } else if (gamepad_get_state()->right) {
            output &= ~(1 << 0);
        } else if (gamepad_get_state()->up) {
            output &= ~(1 << 2);
        } else if (gamepad_get_state()->down) {
            output &= ~(1 << 3);
        }
    }

    return output;
}
