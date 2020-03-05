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

    //Get the file size and read it into a memory buffer
    fseek(f, 0L, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    unsigned char *buffer=malloc(fsize);

    fread(buffer, fsize, 2, f);
    fclose(f);
    
    free(buffer);
    return 0;
}
