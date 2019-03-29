CC = g++
OBJS = Chip8Core.o Chip8Ext.o Chip8.o 
DEPS = Chip8Core.h Chip8Ext.h Chip8Font.h
BIN_DIR = ../bin
CHIP8_EXE = $(BIN_DIR)/Chip8.exe
CHIP8DECODER_EXE = $(BIN_DIR)/Chip8Decoder.exe
INC_DIR = /usr/include/SDL2/

all: $(CHIP8_EXE) $(CHIP8DECODER_EXE)

%.o: %.cpp $(DEPS)
	$(CC) -g -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	
$(CHIP8_EXE): $(BIN_DIR) $(OBJS)
	$(CC) -o $(CHIP8_EXE) $(OBJS) -lmingw32 -lSDL2main -lSDL2 -I$(INC_DIR)

$(CHIP8DECODER_EXE): $(BIN_DIR) Chip8Decoder.o
	$(CC) -o $(CHIP8DECODER_EXE) Chip8Decoder.o

clean:
	rm -rf *.o $(BIN_DIR)
