#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "cpu.h"

extern chip8 chip8_emu;

typedef struct SDL_OBJECTS {
    SDL_Window* win;
    SDL_Renderer* rend;
    SDL_Surface* screen_surface;
    SDL_Rect rects[SCREEN_WIDTH*SCREEN_LENGTH];
} SDL_OBJECTS;

void setup_graphics(SDL_OBJECTS* graphics_container);
void update_graphics(SDL_OBJECTS* graphics_container);
void handle_key_press();
