CC = gcc
CFLAGS = -g -Wall
DIS_TARGET = bin/disch8
TARGET = bin/chip8_emu
HEADERS = include/chip8/

chip8: bin/main.o bin/cpu.o bin/graphics.o
	$(CC) $(CFLAGS) bin/main.o bin/cpu.o bin/graphics.o -o $(TARGET) -I$(HEADERS) -L lib -l SDL2

bin/main.o: src/main.c bin/graphics.o bin/cpu.o
	$(CC) $(CFLAGS) -c src/main.c -o bin/main.o -I$(HEADERS)

bin/graphics.o: src/graphics.c include/chip8/graphics.h
	$(CC) $(CFLAGS) -c src/graphics.c -o bin/graphics.o -I$(HEADERS)
	rm -rf bin/*dSYM/

bin/cpu.o: src/cpu.c include/chip8/cpu.h
	$(CC) $(CFLAGS) -c src/cpu.c -o bin/cpu.o -I$(HEADERS)
	rm -rf bin/*dSYM/

dis:
	$(CC) $(CFLAGS) src/disassembler.c -o $(DIS_TARGET)
	rm -rf bin/*dSYM/

clean:
	rm -f bin/disch8 bin/chip8_emu
	rm -f bin/*.o
	rm -rf bin/*dSYM/
