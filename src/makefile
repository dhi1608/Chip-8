CC = g++
OBJS = Emulator.o Disassembler.o


all: compile link

compile: 
	$(CC) -g -c Emulator.cpp
	$(CC) -g -c Disassembler.cpp
	
link: 
	mkdir -p ../bin
	$(CC) -o ../bin/Emulator.exe Emulator.o
	$(CC) -o ../bin/Disassembler.exe Disassembler.o
	
clean:
	rm $(OBJS) ../bin/Emulator.exe ../bin/Disassembler.exe