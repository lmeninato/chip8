CC = gcc
CFLAGS = -g -Wall
DIS_TARGET = bin/disch8
TARGET = bin/chip8_emu

all:
	$(CC) $(CFLAGS) main.c -o $(TARGET)
	rm -rf bin/*dSYM/

dis:
	$(CC) $(CFLAGS) disassembler.c -o $(DIS_TARGET)
	rm -rf bin/*dSYM/

clean:
	rm -f bin/disch8 bin/chip8_emu
	rm -rf bin/*dSYM/

dis_test:
	./bin/disch8 games/Pong.ch8
