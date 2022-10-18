#pragma once

#include <common.h>

typedef struct {
    bool start;
    bool select;
    bool a;
    bool b;
    bool up;
    bool down;
    bool left;
    bool right;
} gamepad_state;

typedef struct {
    bool button_sel;
    bool dir_sel;
    gamepad_state controller;
} gamepad_context;

gamepad_context *gamepad_get_context();
void gamepad_init();
bool gamepad_button_sel();
bool gamepad_dir_sel();
void gamepad_set_sel(u8 value);

gamepad_state *gamepad_get_state();
u8 gamepad_get_output();
