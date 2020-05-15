#pragma once

#include <stdio.h>
#include <stdlib.h>

#define DEBUG_INPUT 0
#define SCREEN_WIDTH 64
#define SCREEN_LENGTH 32

typedef struct chip8 {
    /*
    Systems memory map:
    0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
    0x200-0xFFF - Program ROM and work RAM
    */
    unsigned char memory[4096];
    unsigned char gfx[SCREEN_WIDTH * SCREEN_LENGTH];
    unsigned short stack[16];
    unsigned char V[16]; // 15 8 bit registers, last register for carry
    unsigned short sp; // stack pointer
    unsigned short I; // index register
    unsigned short pc; // program counter
    unsigned char key[16]; // keypad
    unsigned char draw_flag;
    unsigned char delay_timer;
    unsigned char sound_timer;
} chip8;

extern chip8 chip8_emu;

void handle_rom_input(int argc, char **argv);
void emulate_cycle();
void write_fontset_to_memory();
