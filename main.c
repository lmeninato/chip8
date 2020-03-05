#include <stdio.h>
#include <stdlib.h>

typedef struct chip8 {
    /*
    Systems memory map:
    0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
    0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
    0x200-0xFFF - Program ROM and work RAM
    */
    unsigned char memory[4096];
    unsigned char gfx[64 * 32];
    unsigned short stack[16];
    unsigned char V[16]; // 15 8 bit registers, last register for carry
    unsigned short sp; // stack pointer
    unsigned short I; // index register
    unsigned short pc; // program counter
    unsigned char key[16]; // keypad
    unsigned char delay_timer;
    unsigned char sound_timer;
} chip8;

void initialize_chip8(int argc, char **argv);
void handle_input(int argc, char **argv);
void write_fontset_to_memory();

chip8 chip8_emu;
int main(int argc, char **argv){
    if (argc != 2){
        printf("Incorrect number of arguments\n");
    }
    FILE *f = fopen(argv[1], "rb");
    if (f==NULL){
        printf("error: Couldn't open %s\n", argv[1]);
        exit(1);
    }

    // Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer=malloc(fsize);

    fread(buffer, fsize, 2, f);
    // Write buffer to 0x200 in memory
    fclose(f);

    free(buffer);
    return 0;
}

void initialize_chip8(int argc, char **argv){
    // chip8_emu is global object
    handle_input(argc, argv);
    write_fontset_to_memory();
}

void handle_input(int argc, char **argv){
    // parse the input

    // write ROM data to 0x200-0xFFF
}

void write_fontset_to_memory(){
    // writes fontset to the following memory addresses: 0x050-0x0A0
    unsigned char chip8_fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    int i;
    for (i = 0; i < 80; ++i){
        chip8_emu.memory[i+0x050] = chip8_fontset[i];
    }
}
