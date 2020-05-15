#include "cpu.h"

void handle_rom_input(int argc, char **argv){
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

void emulate_cycle(){
    // Fetch opcode
    unsigned short opcode;

    opcode = chip8_emu.memory[chip8_emu.pc] << 8 | chip8_emu.memory[chip8_emu.pc + 1];

    // Decode
    unsigned char byte_1, byte_2;
    byte_1 = (unsigned char) (opcode >> 8);
    byte_2 = (unsigned char) (opcode & 0xFF);
#if DEBUG_INPUT
    printf("byte_1: 0x%02x\nbyte_2: 0x%02x\n", byte_1, byte_2);
    printf("opcode: 0x%04x\n", opcode);
#endif
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
        // case 9XY0 - Skips the next instruction if VX doesn't equal VY.
        case 0x09:
            if (chip8_emu.V[byte_1 & 0x0F] != chip8_emu.V[byte_2 & 0xF0]){
                chip8_emu.pc += 4;
            } else {
                chip8_emu.pc += 2;
            }
            break;
        // case ANNN - sets I to the address NNN
        case 0x0A:
            chip8_emu.I = opcode & 0x0FFF;
            chip8_emu.pc += 2;
            break;
        // case BNNN - Jumps to the address NNN plus V0.
        case 0x0B:
            chip8_emu.pc = (opcode & 0x0FFF) + chip8_emu.V[0x00];
            break;
        // case CXNN - Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
        case 0x0C:
            chip8_emu.V[byte_1 & 0x0F] = (unsigned char) ((rand() % 255) & byte_2);
            break;
        // case DXYN - now shit gets real - draw sprite on screen
        case 0x0D: {
            /* Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
             Each row of 8 pixels is read as bit-coded starting from memory location I;
             I value doesn’t change after the execution of this instruction.
             As described above, VF is set to 1 if any screen pixels are flipped
             from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
            */
            unsigned char vx = chip8_emu.V[byte_1 & 0x0F];
            unsigned char vy = chip8_emu.V[byte_2 & 0xF0];
            unsigned char height = byte_2 & 0x0F;
            unsigned char pixel;

            int x;
            int y;

            chip8_emu.V[0xF] = 0;

            for (y = 0; y < height; y++){
                pixel = chip8_emu.memory[chip8_emu.I+y];
                for (x = 0; x < 8; x++){
                    if ((pixel & (0x80 >> x)) != 0){
                        if (chip8_emu.gfx[(vy + y)*64 + vx +x] == 1){
                            chip8_emu.V[0xF] = 1;
                        }
                        chip8_emu.gfx[(vy + y)*64 + vx +x] ^= 1;
                    }
                }
            }
            chip8_emu.draw_flag = 1;
            chip8_emu.pc += 2;

            break;
        }
        case 0x0E:
            // case EX9E - Skips the next instruction if the key stored in VX is pressed.
            switch (byte_2){
                case 0x9E:
                    if (chip8_emu.key[chip8_emu.V[byte_1 & 0x0F]]){
                        chip8_emu.pc += 4;
                    } else {
                        chip8_emu.pc += 2;
                    }
                    break;
            // case EXA1 - Skips the next instruction if the key stored in VX isn't pressed.
                case 0xA1:
                    if (chip8_emu.key[chip8_emu.V[byte_1 & 0x0F]] == 0){
                        chip8_emu.pc += 4;
                    } else {
                        chip8_emu.pc += 2;
                    }
                    break;
            }
            break;
        case 0x0F:
            switch (byte_2){
                // case FX07 - Sets VX to the value of the delay timer.
                case 0x07:
                    chip8_emu.V[byte_1 & 0x0F] = chip8_emu.delay_timer;
                    break;
                // case FX0A - A key press is awaited, and then stored in VX.
                case 0x0A: {
                    unsigned char key_pressed = 0;
                    int i;

                    for (i = 0; i < 16; i++){
                        if (chip8_emu.key[i] != 0){
                            chip8_emu.V[byte_1 & 0x0F] = chip8_emu.key[i];
                            key_pressed = 1;
                        }
                    }

                    if (key_pressed == 0){
                        return;
                    }

                    chip8_emu.pc += 2;

                    break;
                }
                // case FX15 - Sets the delay timer to VX.
                case 0x15:
                    chip8_emu.delay_timer = chip8_emu.V[byte_1 & 0x0F];
                    break;
                // case FX18 - Sets the sound timer to VX.
                case 0x18:
                    chip8_emu.sound_timer = chip8_emu.V[byte_1 & 0x0F];
                    break;
                // case FX1E - Adds VX to I. VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.
                case 0x1E:
                    if (chip8_emu.I > 0xFFF - chip8_emu.V[byte_1 & 0x0F]){
                        chip8_emu.V[0xF] = 1;
                    } else {
                        chip8_emu.V[0xF] = 0;
                    }
                    chip8_emu.I += chip8_emu.V[byte_1 & 0x0F];
                    chip8_emu.pc += 2;
                    break;
                // case FX29 - Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                case 0x29:
                    chip8_emu.I = chip8_emu.V[byte_1 & 0x0F] * 5; // not sure
                    chip8_emu.pc += 2;
                    break;
                // case FX33
                case 0x33:
                    /* Stores the binary-coded decimal representation of VX, with
                    the most significant of three digits at the address in I,
                    the middle digit at I plus 1, and the least significant digit at I plus 2.
                    (In other words, take the decimal representation of VX,
                    place the hundreds digit in memory at location in I,
                    the tens digit at location I+1, and the ones digit at location I+2.)
                    */
                    chip8_emu.memory[chip8_emu.I] = chip8_emu.V[byte_1 & 0x0F] / 100;
                    chip8_emu.memory[chip8_emu.I+1] = (chip8_emu.V[byte_1 & 0x0F] % 100) / 10;
                    chip8_emu.memory[chip8_emu.I+2] = chip8_emu.V[byte_1 & 0x0F] % 10;
                    chip8_emu.pc += 2;
                    break;
                // case FX55 - Stores V0 to VX (including VX) in memory starting at address I.
                // The offset from I is increased by 1 for each value written, but I itself is left unmodified.
                case 0x55: {
                    int i;
                    for (i = 0; i <= (byte_1 & 0x0F); i++){
                        chip8_emu.memory[chip8_emu.I + i] = chip8_emu.V[i];
                    }
                    chip8_emu.pc += 2;
                    break;
                }
                // case FX65 - Fills V0 to VX (including VX) with values from memory starting at address I.
                // The offset from I is increased by 1 for each value written, but I itself is left unmodified.
                case 0x65: {
                    int i;
                    for (i = 0; i <= (byte_1 & 0x0F); i++){
                        chip8_emu.V[i] = chip8_emu.memory[chip8_emu.I + i];
                    }
                    chip8_emu.pc += 2;
                    break;
                }
            }
            break;
    }

    // Update timers, adapted from:
    // http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
    if(chip8_emu.delay_timer > 0){
      chip8_emu.delay_timer--;
    }

    if(chip8_emu.sound_timer > 0)
    {
      if(chip8_emu.sound_timer == 1){
        printf("BEEP!\n");
        chip8_emu.sound_timer--;
        }
    }
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
