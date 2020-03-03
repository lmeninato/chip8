#include <stdio.h>

int Disassemble_ch8(unsigned char *codebuffer, int pc){
    unsigned char *code = &codebuffer[pc];
    int opbytes = 2;
    printf ("%04x ", pc);
    unsigned char byte_1, byte_2;
    byte_1 = code[0];
    byte_2 = code[1];

    printf("opcode: %02hhx %02hhx\t ", byte_1, byte_2);

    switch (byte_1 >> 4)
    {
        case 0x00:
            switch (byte_2){
                case 0xE0:
                    printf("clear screen");
                    break;
                case 0xEE:
                    printf("return from subroutine");
                    break;
                default:
                    printf("Calls RCA 1802 program at address NNN. Not necessary for most ROMs.");
                    break;
            }
            break;
        case 0x01:
            printf("Jumps to address NNN.");
            break;
        case 0x02:
            printf("Calls subroutine at NNN.");
            break;
        case 0x03:
            printf("Skips the next instruction if VX equals NN.");
            break;
        case 0x04:
            printf("Skips the next instruction if VX doesn't equal NN.");
            break;
        case 0x05:
            printf("Skips the next instruction if VX equals VY.");
            break;
        case 0x06:
            printf("Sets VX to NN.");
            break;
        case 0x07:
            printf("Adds NN to VX.");
            break;
        case 0x08:
            switch (byte_2 & 0x0F){
                case 0x00:
                    printf("Sets VX to the value of VY.");
                    break;
                case 0x01:
                    printf("Sets VX to VX or VY.");
                    break;
                case 0x02:
                    printf("Sets VX to VX and VY.");
                    break;
                case 0x03:
                    printf("Sets VX to VX xor VY.");
                    break;
                case 0x04:
                    printf("Sets VX to the value of VY.");
                    break;
                case 0x05:
                    printf("Adds VY to VX.");
                    break;
                case 0x06:
                    printf("VY is subtracted from VX.");
                    break;
                case 0x07:
                    printf("performs Vx>>=1");
                    break;
                case 0x0E:
                    printf("performs Vx=Vy-Vx");
                    break;
            }
            break;
        case 0x09:
            printf("Skips the next instruction if VX doesn't equal VY.");
            break;
        case 0x0A:
            printf("Sets I to the address NNN.");
            break;
        case 0x0B:
            printf("Jumps to the address NNN plus V0.");
            break;
        case 0x0C:
            printf("performs Vx=rand()&NN");
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

    printf("\n");

    return opbytes;
}

int main(int argc, char **argv){
    //Following the code at http://www.emulator101.com/disassembler-pt-1.html
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

    int pc = 0;

    while (pc < fsize)
    {
        pc += Disassemble_ch8(buffer, pc);
    }
    return 0;
}
