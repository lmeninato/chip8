CC = gcc
CFLAGS = -g -Wall
DIS_TARGET = disch8

dis:
	$(CC) $(CFLAGS) disassembler.c -o $(DIS_TARGET)

clean_dis:
	rm -rf disch8*
