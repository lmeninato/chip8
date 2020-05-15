#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "graphics.h"

chip8 chip8_emu;

void initialize_chip8(int argc, char **argv, SDL_OBJECTS* graphics_container);

int main(int argc, char **argv){
    // Loads game ROM and fontset into chip8_emu memory
    SDL_OBJECTS graphics_container;
    initialize_chip8(argc, argv, &graphics_container);

    for (;;){
        emulate_cycle();
        if (chip8_emu.draw_flag){
            update_graphics(&graphics_container);
        }
        handle_key_press();
    }

    // cleanup SDL_Window and SDL_Renderer
    SDL_DestroyRenderer(graphics_container.rend);
    SDL_DestroyWindow(graphics_container.win);

    return 0;
}

void initialize_chip8(int argc, char **argv, SDL_OBJECTS* graphics_container){
    setup_graphics(graphics_container);
    handle_rom_input(argc, argv);
    write_fontset_to_memory();
    chip8_emu.pc = 0x200;  // Program counter starts at 0x200
    chip8_emu.I = 0;       // Reset index register
    chip8_emu.sp = 0;      // Reset stack pointer
}
