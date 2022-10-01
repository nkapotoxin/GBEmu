#pragma once

#include <common.h>

static const int SCREEN_WIDTH = 160 * 4;
static const int SCREEN_HEIGHT = 144 * 4;

void ui_init();
void ui_handle_events();
void ui_update();
void systemShowSpeed(int);
void systemSetTitle(const char* title);
