#include <stdio.h>
#include <stdlib.h>

#define DEBUG_INPUT 0

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
    unsigned char draw_flag;
    unsigned char delay_timer;
    unsigned char sound_timer;
} chip8;

void initialize_chip8(int argc, char **argv);
void handle_input(int argc, char **argv);
void write_fontset_to_memory();
void emulate_cycle();

chip8 chip8_emu;
int main(int argc, char **argv){
    // Loads game ROM and fontset into chip8_emu memory
    initialize_chip8(argc, argv);



    return 0;
}

void initialize_chip8(int argc, char **argv){
    handle_input(argc, argv);
    write_fontset_to_memory();
    chip8_emu.pc = 0x200;  // Program counter starts at 0x200
    chip8_emu.I = 0;       // Reset index register
    chip8_emu.sp = 0;      // Reset stack pointer

    int i;
    for (i = 0; i < 5; i++){
        emulate_cycle();
        chip8_emu.pc += 2;
    }
}

void handle_input(int argc, char **argv){
    // parse the input
    if (argc != 2){
        printf("Incorrect number of arguments\n");
        exit(1);
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
    int loc;

    fread(buffer, fsize, 1, f);
    // Write buffer to 0x200 in chip8_emu memory
    unsigned char byte;
    for(loc = 0; loc <= fsize; loc++){
        byte = buffer[loc];
        chip8_emu.memory[0x200 + loc] = byte;
    }
#if DEBUG_INPUT
    int i;
    for (i = 0; i < 15; i++){
        printf("chip8_emu memory at index 0x%03x: 0x%02x\n", 0x200+i, chip8_emu.memory[0x200 + i]);
    }
    for (i = 0; i < 11; i++){
        printf("chip8_emu memory at index 0x%03x: 0x%02x\n", 0x200+235+i, chip8_emu.memory[0x200+235+i]);
    }
#endif
    fclose(f);

    free(buffer);
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

void emulate_cycle(){
    // Fetch opcode
    unsigned short opcode;

    opcode = chip8_emu.memory[chip8_emu.pc] << 8 | chip8_emu.memory[chip8_emu.pc + 1];

    // Decode
    unsigned char byte_1, byte_2;
    byte_1 = (unsigned char) (opcode >> 8);
    byte_2 = (unsigned char) (opcode & 0xFF);
    printf("byte_1: 0x%02x\nbyte_2: 0x%02x\n", byte_1, byte_2);
    printf("opcode: 0x%04x\n", opcode);
    unsigned short I = 0xFFFF;
    // Execute

    // Switch on first nibble
    switch (byte_1 >> 4)
    {
        case 0x00:
            switch (byte_2){
                // case 00E0 - clear screen
                case 0xE0: {
                    int i;
                    for (i = 0; i < 2048; i++){
                        chip8_emu.gfx[i] = 0;
                    }
                    chip8_emu.draw_flag = 1;
                    chip8_emu.pc += 2;
                    break;
                }
                // case 00EE - return from subroutine
                case 0xEE:
                    --chip8_emu.sp;
                    chip8_emu.pc = chip8_emu.stack[chip8_emu.sp];
                    chip8_emu.pc += 2;
                    break;
                default:
                    printf("Calls RCA 1802 program - usually not necessary");
                    break;
            }
            break;
        // case 1NNN - jump to address NNN
        case 0x01:
            chip8_emu.pc = opcode & 0x0FFF;
            break;
        // case 2NNN - call subroutine at NNN
        case 0x02:
            chip8_emu.stack[chip8_emu.sp] = chip8_emu.pc;
            ++chip8_emu.sp;
            chip8_emu.pc = opcode & 0x0FFF;
            break;
        // case 3XNN - skips next instruction if VX == NN
        case 0x03:
            if (chip8_emu.V[byte_1 & 0x0F] == byte_2){
                chip8_emu.pc += 4;
            } else {
                chip8_emu.pc += 2;
            }
            break;
        // case 4XNN - skips the next instruction if VX != NN
        case 0x04:
            if (chip8_emu.V[byte_1 & 0x0F] != byte_2){
                chip8_emu.pc += 4;
            } else {
                chip8_emu.pc += 2;
            }
            break;
        // case 5XY0 - skips the next instruction if VX == VY
        case 0x05:
            if (chip8_emu.V[byte_1 & 0x0F] == chip8_emu.V[byte_2 & 0xF0]){
                chip8_emu.pc += 4;
            } else {
                chip8_emu.pc += 2;
            }
            break;
        // case 6XNN - sets VX to NN
        case 0x06:
            chip8_emu.V[byte_1 & 0x0F] = byte_2;
            chip8_emu.pc += 2;
            break;
        // case 7XNN - adds NN to VX;
        case 0x07:
            chip8_emu.V[byte_1 & 0x0F] += byte_2;
            chip8_emu.pc += 2;
            break;
        case 0x08:
            switch (byte_2 & 0x0F){
                // case 8XY0 - sets VX = VY
                case 0x00:
                    chip8_emu.V[byte_1 & 0x0F] = chip8_emu.V[byte_2 & 0xF0];
                    chip8_emu.pc += 2;
                    break;
                // case 8XY1 - sets VX  = VX | VY
                case 0x01:
                    chip8_emu.V[byte_1 & 0x0F] |= chip8_emu.V[byte_2 & 0xF0];
                    chip8_emu.pc += 2;
                    break;
                // case 8XY2 - sets VX = VX & VY
                case 0x02:
                    chip8_emu.V[byte_1 & 0x0F] &= chip8_emu.V[byte_2 & 0xF0];
                    chip8_emu.pc += 2;
                    break;
                // case 8XY3 - sets VX = VX ^ VY
                case 0x03:
                    chip8_emu.V[byte_1 & 0x0F] ^= chip8_emu.V[byte_2 & 0xF0];
                    chip8_emu.pc += 2;
                    break;
                // case 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                case 0x04:
                    if (chip8_emu.V[byte_1 & 0x0F] > 0xFF - chip8_emu.V[byte_2 & 0xF0]){
                        chip8_emu.V[0xF] = 1;
                    } else {
                        chip8_emu.V[0xF] = 0;
                    }
                    chip8_emu.V[byte_1 & 0x0F] += chip8_emu.V[byte_2 & 0xF0];
                    chip8_emu.pc += 2;
                    break;
                // case 8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                case 0x05:
                    if (chip8_emu.V[byte_1 & 0x0F] < chip8_emu.V[byte_2 & 0xF0]){
                        chip8_emu.V[0xF] = 0;
                    } else {
                        chip8_emu.V[0xF] = 1;
                    }
                    chip8_emu.V[byte_1 & 0x0F] -= chip8_emu.V[byte_2 & 0xF0];
                    chip8_emu.pc += 2;
                    break;
                // case 8XY6 - Stores the least significant bit of VX in VF and then shifts VX to the right by 1
                case 0x06:
                    chip8_emu.V[0xF] = (chip8_emu.V[byte_1 & 0x0F] & 0x01);
                    chip8_emu.V[byte_1 & 0x0F] >>= 1;
                    chip8_emu.pc += 2;
                    break;
                // case 8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                case 0x07:
                    if (chip8_emu.V[byte_1 & 0x0F] > chip8_emu.V[byte_2 & 0xF0]){
                        chip8_emu.V[0xF] = 0;
                    } else {
                        chip8_emu.V[0xF] = 1;
                    }
                    chip8_emu.V[byte_1 & 0x0F] = chip8_emu.V[byte_2 & 0xF0] - chip8_emu.V[byte_1 & 0x0F];
                    chip8_emu.pc += 2;
                    break;
                // case 8XYE - Stores the most significant bit of VX in VF and then shifts VX to the left by 1
                case 0x0E:
                    chip8_emu.V[0xF] = (chip8_emu.V[byte_1 & 0x0F] & 0x80);
                    chip8_emu.V[byte_1 & 0x0F] <<= 1;
                    chip8_emu.pc += 2;
                    break;
            }
            break;
        case 0x09:
            printf("Skips the next instruction if VX doesn't equal VY.");
            break;
        case 0x0A:
            printf("Sets I to the address 0x%03x", I);
            break;
        case 0x0B:
            printf("Jumps to the address 0x%03x plus V0", I);
            break;
        case 0x0C:
            printf("performs Vx=rand()&%02x", byte_2);
            break;
        case 0x0D:
            printf("performs draw(Vx,Vy,N)");
            break;
        case 0x0E:
            switch (byte_2){
                case 0x9E:
                    printf("Skips the next instruction if the key stored in VX is pressed.");
                    break;
                case 0xA1:
                    printf("Skips the next instruction if the key stored in VX isn't pressed.");
                    break;
            }
            break;
        case 0x0F:
            switch (byte_2){
                case 0x07:
                    printf("Sets VX to the value of the delay timer.");
                    break;
                case 0x0A:
                    printf("A key press is awaited, and then stored in VX.");
                    break;
                case 0x15:
                    printf("Sets the delay timer to VX.");
                    break;
                case 0x18:
                    printf("Sets the sound timer to VX.");
                    break;
                case 0x1E:
                    printf("Adds VX to I.");
                    break;
                case 0x29:
                    printf("Sets I to the location of the sprite for the character in VX. ");
                    break;
                case 0x33:
                    printf("compute binary-coded decimal");
                    break;
                case 0x55:
                    printf("Stores V0 to VX (including VX) in memory starting at address I.");
                    break;
                case 0x65:
                    printf("Fills V0 to VX (including VX) with values from memory starting at address I.");
                    break;
            }
            break;
    }

    // Update timers, adapted from:
    // http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
    if(chip8_emu.delay_timer > 0){
      --chip8_emu.delay_timer;
    }

    if(chip8_emu.sound_timer > 0)
    {
      if(chip8_emu.sound_timer == 1)
        printf("BEEP!\n");
      --chip8_emu.sound_timer;
    }
}
