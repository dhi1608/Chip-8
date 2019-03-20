CC = g++
OBJS = Emulator.o Disassembler.o
EMULATOR_EXE = ../bin/Emulator.exe
DISASSEMBLER_EXE = ../bin/Disassembler.exe
BIN_DIR = ../bin

all: $(EMULATOR_EXE) $(DISASSEMBLER_EXE)

$(BIN_DIR):
	mkdir -p ../bin
	
$(EMULATOR_EXE): $(BIN_DIR) Emulator.o
	$(CC) -o $(EMULATOR_EXE) Emulator.o

Emulator.o: Emulator.cpp
	$(CC) -g -c Emulator.cpp

$(DISASSEMBLER_EXE): $(BIN_DIR) Disassembler.o
	$(CC) -o $(DISASSEMBLER_EXE) Disassembler.o

Disassembler.o: Disassembler.cpp
	$(CC) -g -c Disassembler.cpp

clean:
	rm -rf $(OBJS) $(BIN_DIR)