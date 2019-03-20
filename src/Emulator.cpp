/*
 * Emulator.cpp
 *
 * Created on: Mar 11, 2019
 * Author: Brian Green
 */

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const uint16_t CHIP8_LANGUAGE_OFFSET = 0x0000;
const uint16_t USER_PROGRAM_OFFSET = 0x0200;
const uint16_t STACK_OFFSET = 0x06A0;
const uint16_t ADDRESS_REGISTER_OFFSET = 0x06D0;
const uint16_t DATA_REGISTER_OFFSET = 0x06F0;
const uint16_t DISPLAY_REFRESH_OFFSET = 0X0700;
const int RAM_SIZE = 2048;
const int STACK_DEPTH = 32;
const int ARRAY_SIZE = 16;

typedef struct Sprite
{
	uint8_t s[5];
}Sprite;

typedef struct StateChip8
{
	// Memory
	uint8_t *ram;
	// Data
	uint8_t *v;
	// Registers
	uint8_t dt;
	uint8_t st;
	uint16_t sp;
	uint16_t pc;
	uint16_t i;
	// Display
	uint8_t *display;
	// Key input
	bool keyCurrent[ARRAY_SIZE];
	bool keyPrevious[ARRAY_SIZE];
	bool keyWaiting;

} StateChip8;

void initializeSprites(Sprite* sprite)
{
	for (short i = 0; i < ARRAY_SIZE; ++i)
	{
		switch(i)
		{
		case 0x00:
			sprite[i] = { 0xF0,0x90,0x90,0x90,0xF0 };
			break;
		case 0x01:
			sprite[i] = { 0x20,0x60,0x20,0x20,0x70 };
			break;
		case 0x02:
			sprite[i] = { 0xF0,0x10,0xF0,0x80,0xF0 };
			break;
		case 0x03:
			sprite[i] = { 0xF0,0x10,0xF0,0x10,0xF0 };
			break;
		case 0x04:
			sprite[i] = { 0x90,0x90,0xF0,0x10,0x10 };
			break;
		case 0x05:
			sprite[i] = { 0xF0,0x80,0xF0,0x10,0xF0 };
			break;
		case 0x06:
			sprite[i] = { 0xF0,0x80,0xF0,0x90,0xF0 };
			break;
		case 0x07:
			sprite[i] = { 0xF0,0x10,0x20,0x40,0x40 };
			break;
		case 0x08:
			sprite[i] = { 0xF0,0x90,0xF0,0x90,0xF0 };
			break;
		case 0x09:
			sprite[i] = { 0xF0,0x90,0x90,0x10,0xF0 };
			break;
		case 0x0A:
			sprite[i] = { 0xF0,0x90,0x90,0x90,0x90 };
			break;
		case 0x0B:
			sprite[i] = { 0xE0,0x90,0xE0,0x90,0xE0 };
			break;
		case 0x0C:
			sprite[i] = { 0xF0,0x80,0x80,0x80,0xF0 };
			break;
		case 0x0D:
			sprite[i] = { 0xE0,0x90,0x90,0x90,0xE0 };
			break;
		case 0x0E:
			sprite[i] = { 0xF0,0x80,0xF0,0x80,0xF0 };
			break;
		case 0x0F:
			sprite[i] = { 0xF0,0x80,0xF0,0x80,0x80 };
			break;
		}
	}
}

void printChip8(StateChip8* state)
{

	/**
	 * Print registers
	 */
	printf("PC:%04X       SP:%04X       IP:%04X       DT:%02X/ST:%02X/KW:%X\n",
			state->pc, state->sp, state->i, state->dt, state->st, state->keyWaiting);

	/**
	 * Print V[0:F]
	 */
	for (short i = 1; i <= ARRAY_SIZE; ++i)
	{
		printf("V[%X]:%02X", i - 1, state->v[i - 1]);
		if (i % 4 == 0)
			printf("\n");
		else
			printf("       ");
	}

	/**
	 * Print KC[0:F]
	 */
	for (short i = 1; i <= ARRAY_SIZE; ++i)
	{
		printf("KC[%X]:%02X", i - 1, state->keyCurrent[i - 1]);
		if (i % 4 == 0)
			printf("\n");
		else
			printf("       ");
	}

	/**
	 * Print KP[0:F]
	 */
	for (short i = 1; i <= ARRAY_SIZE; ++i)
	{
		printf("KP[%X]:%02X", i - 1, state->keyPrevious[i - 1]);
		if (i % 4 == 0)
			printf("\n");
		else
			printf("       ");
	}

	/**
	 * Print Stack
	 */
	for (short i = 2; i <= STACK_DEPTH; i += 2)
	{
		uint8_t byte1 = state->ram[STACK_OFFSET + i - 2];
		uint8_t byte2 = state->ram[STACK_OFFSET + i - 1];
		printf("S[%04X]:%02X%02X", STACK_OFFSET + i - 2, byte1, byte2);
		if (i % 8 == 0)
			printf("\n");
		else
			printf("  ");
	}

	/**
	 * Print Display[00:FF]
	 */
	for (short i = 1; i <= 256; ++i)
	{
		printf("D[%02X]:%04X", i - 1, state->display[i - 1]);
		if (i % 8 == 0)
			printf("\n");
		else
			printf("       ");
	}
	printf("\n");
}

/**
 * Safely increment stack pointer
 */
void incrementSP(StateChip8* state)
{
	if (STACK_OFFSET + STACK_DEPTH > state->sp)
		state->sp += 2;
	else
		printf("Warn: Max stack pointer reached can't increment\n");
}

/**
 * Safely decrement stack point
 */
void decrementSP(StateChip8* state)
{
	if (STACK_OFFSET < state->sp)
		state->sp -= 2;
	else
		printf("Warn: Min stack pointer reached can't decrement\n");
}

/**
 * Safely increment program counter
 */
void incrementPC(StateChip8* state)
{
	if (STACK_OFFSET > state->pc)
		state->pc += 2;
	else
		printf("Warn: Max program counter reached can't increment\n");
}

/**
 * Identify unimplemented instructions
 */
void unimplementedInstruction(StateChip8* state)
{
	printf("Warn: Unimplemented instruction %04X\n", state->pc);
	incrementPC(state);
}

/**
 * Method processes instruction 0 types
 *
 * state - chip 8 state data
 */
void processInstruction0(StateChip8* state)
{
	uint8_t byte1 = state->ram[state->pc] & 0xFF;
	uint8_t byte2 = state->ram[state->pc + 1] & 0xFF;

	if (byte1 == 0x00 && byte2 == 0xE0)
	{
		memset(state->display, 0, RAM_SIZE - DISPLAY_REFRESH_OFFSET);
		incrementPC(state);
	}
	else if (byte1 == 0x00 && byte2 == 0xEE)
	{
		decrementSP(state);
		state->pc = state->ram[state->sp] << 8 | state->ram[state->sp + 1];
		state->ram[state->sp] = 0;
		state->ram[state->sp + 1] = 0;
	}
	else
	{
		printf("TODO Machine Language subroutine?? Should this be ignored or jump into Chip8 ROM??");
		incrementPC(state);
	}
}

/**
 * Method processes instruction 8 types
 *
 * state - chip 8 state data
 */
void processInstruction8(StateChip8* state)
{
	uint8_t x = state->ram[state->pc] & 0x0F;
	uint8_t y = state->ram[state->pc + 1] >> 4 & 0x0F;
	uint8_t mode = state->ram[state->pc + 1] & 0x0F;
	uint16_t result;

	switch (mode)
	{
	case 0x0:
		state->v[x] = state->v[y];
		incrementPC(state);
		break;
	case 0x1:
		state->v[x] = state->v[x] | state->v[y];
		incrementPC(state);
		break;
	case 0x2:
		state->v[x] = state->v[x] & state->v[y];
		incrementPC(state);
		break;
	case 0x3:
		state->v[x] = state->v[x] ^ state->v[y];
		incrementPC(state);
		break;
	case 0x4:
		result = state->v[x] + state->v[y];
		state->v[x] = result & 0x00FF;
		state->v[0xF] = result >> 8;
		incrementPC(state);
		break;
	case 0x5:
		result = state->v[x] - state->v[y];
		state->v[x] = result & 0x00FF;
		state->v[0xF] = ~(result >> 8 & 0x01);
		incrementPC(state);
		break;
	case 0x6:
		state->v[x] =  state->v[x] >> 1;
		state->v[0xF] = state->v[x] & 0x01;
		incrementPC(state);
		break;
	case 0x7:
		result = state->v[y] - state->v[x];
		state->v[x] = result & 0x00FF;
		state->v[0xF] = ~(result >> 8 & 0x01);
		incrementPC(state);
		break;
	case 0xE:
		state->v[x] =  state->v[x] << 1;
		state->v[0xF] = state->v[x] & 0x80;
		incrementPC(state);
		break;
	default:
		unimplementedInstruction(state);
		break;
	}
}

/**
 * Method processes instruction E types
 *
 * state - chip 8 state data
 */
void processInstructionE(StateChip8* state)
{
	uint8_t x = state->ram[state->pc] & 0x0F;
	uint8_t mode = state->ram[state->pc + 1] & 0xFF;

	switch (mode)
	{
	case 0x9E:
		if (state->keyCurrent[state->v[x]] != 0)
			incrementPC(state);
		break;
	case 0xA1:
		if (state->keyCurrent[state->v[x]] == 0)
			incrementPC(state);
		break;
	default:
		unimplementedInstruction(state);
		break;
	}
}

/**
 * Method processes instruction F types
 *
 * state - chip 8 state data
 */
void processInstructionF(StateChip8* state)
{
	uint8_t x = state->ram[state->pc] & 0x0F;
	uint8_t mode = state->ram[state->pc + 1] & 0xFF;

	switch (mode)
	{
	case 0x07:
		state->v[x] =  state->dt;
		incrementPC(state);
		break;
	case 0x0A:
	{
		memcpy(state->keyPrevious, state->keyCurrent, ARRAY_SIZE);
		if (!state->keyWaiting)
		{
			state->keyWaiting = true;
		}
		else
		{
			for (short i = 0; i < ARRAY_SIZE; ++i)
			{
				if (!state->keyPrevious && state->keyCurrent)
				{
					state->keyWaiting = false;
					state->v[x] = i;
					incrementPC(state);
					break;
				}
			}
		}
	}
		break;
	case 0x15:
		state->dt =  state->v[x];
		incrementPC(state);
		break;
	case 0x18:
		state->st =  state->v[x];
		incrementPC(state);
		break;
	case 0x1E:
		state->i =  state->i +state->v[x];
		incrementPC(state);
		break;
	case 0x29:
		state->i = CHIP8_LANGUAGE_OFFSET + (x * sizeof(Sprite));
		incrementPC(state);
		break;
	case 0x33:
	{
		uint8_t hundreds;
		uint8_t tens;
		uint8_t ones;
		tens = state->v[x];
		ones = state->v[x] - tens * 10;
		hundreds = tens / 10;
		tens = tens - hundreds * 10;
		state->ram[state->i] = hundreds;
		state->ram[state->i + 1] = tens;
		state->ram[state->i + 2] = ones;
		incrementPC(state);
	}
		break;
	case 0x55:
	{
		for (short i = 0; i < ARRAY_SIZE; ++i)
			state->ram[state->i + i] = state->v[i];
		incrementPC(state);
	}
		break;
	case 0x65:
	{
		for (short i = 0; i < ARRAY_SIZE; ++i)
			state->v[i] = state->ram[state->i + i];
		incrementPC(state);
	}
		break;
	default:
		unimplementedInstruction(state);
		break;

	}
}

/**
 * Method emulates the Chip-8 operations
 * state - chip8 state data
 */
void emulateChip8(StateChip8* state)
{
	/**
	 * Print operation based on instruction type
	 */
	uint8_t byte1 = state->ram[state->pc] & 0x0F;
	uint8_t byte2 = state->ram[state->pc + 1];
	uint8_t mode = state->ram[state->pc] >> 4 & 0x0F;
	switch (mode)
	{
	case 0x0:
		processInstruction0(state);
		break;
	case 0x1:
		state->pc = byte1 << 8 | byte2;
		break;
	case 0x2:
		state->ram[state->sp] = byte1;
		state->ram[state->sp + 1] = byte2;
		state->pc = byte1 << 8 | byte2;
		incrementSP(state);
		break;
	case 0x3:
		if (state->v[byte1] == byte2)
			incrementPC(state);
		break;
	case 0x4:
		if (state->v[byte1] != byte2)
			incrementPC(state);
		break;
	case 0x5:
		if (state->v[byte1] == state->v[byte2 >> 4 & 0x0F])
			incrementPC(state);
		break;
	case 0x6:
		state->v[byte1] = byte2;
		incrementPC(state);
		break;
	case 0x7:
		state->v[byte1] += byte2;
		incrementPC(state);
		break;
	case 0x8:
		processInstruction8(state);
		break;
	case 0x9:
		if (state->v[byte1] != state->v[byte2 & 0xF0])
			incrementPC(state);
		break;
	case 0xA:
		state->i = byte1 << 8 | byte2;
		incrementPC(state);
		break;
	case 0xB:
		state->pc = (byte1 << 8 | byte2) + state->v[0];
		break;
	case 0xC:
		state->v[byte1] = (rand() % 256) & byte2;
		incrementPC(state);
		break;
	case 0xD:
	{
		uint8_t x = byte1;
		uint8_t y = byte2 >> 4 & 0x0F;
		uint8_t n = byte2 & 0x0F;
		state->v[0xF] = 0;
		for (short i = 0; i < n && i < ARRAY_SIZE; ++i)
		{
			uint8_t newSprite = state->ram[state->i + i];
			uint8_t oldSprite = state->display[state->v[x] + state->v[y] * ARRAY_SIZE];
			if (newSprite & oldSprite)
				state->v[0xF] = 1;
			newSprite ^= oldSprite;
			state->display[state->v[x] + state->v[y] * ARRAY_SIZE] = newSprite;
		}
		incrementPC(state);
	}
		break;
	case 0xE:
		processInstructionE(state);
		break;
	case 0xF:
		processInstructionF(state);
		break;
	default:
		unimplementedInstruction(state);
		break;

	}
}

/******************************************
 * Main method
 ******************************************/
int main(const int argc, const char **argv)
{
	const uint16_t CHIP8_LANGUAGE_OFFSET = 0x0000;
	const uint16_t USER_PROGRAM_OFFSET = 0x0200;
	const uint16_t STACK_OFFSET = 0x06A0;
	const uint16_t ADDRESS_REGISTER_OFFSET = 0x06D0;
	const uint16_t DATA_REGISTER_OFFSET = 0x06F0;
	const uint16_t DISPLAY_REFRESH_OFFSET = 0X0700;

	const int RAM_SIZE = 2048;
	/**
	 * Confirm user has provided correct inputs and provide
	 * help if they did not
	 */
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	/**
	 * Open the binary file
	 */
	ifstream rom;
	rom.open(argv[1], ios::in | ios::binary);
	if (!rom)
	{
		printf("Unable to open %s\n", argv[1]);
		exit(1);
	}

	/**
	 * Get size of the ROM
	 */
	rom.seekg(0, rom.end);
	int romSize = rom.tellg();
	rom.seekg(0, rom.beg);

	/**
	 * Verify rom is not too large for system
	 */
	int availableMemory = STACK_OFFSET - USER_PROGRAM_OFFSET;
	if (romSize > availableMemory)
	{
		printf(
				"User program exceeds memory capacity requesting %d available %d",
				romSize, availableMemory);
		exit(1);
	}

	/**
	 * Initialize data structures
	 */
	StateChip8 state;
	memset(&state, 0, sizeof(StateChip8));
	state.ram = (uint8_t*)calloc(RAM_SIZE, 1);
	state.v = &state.ram[DATA_REGISTER_OFFSET];
	state.display = &state.ram[DISPLAY_REFRESH_OFFSET];
	state.sp = STACK_OFFSET;
	state.pc = USER_PROGRAM_OFFSET;
	Sprite *sprites = (Sprite*)&state.ram[CHIP8_LANGUAGE_OFFSET];
	initializeSprites(sprites);

	/**
	 * Read ROM into memory and close file
	 */
	rom.read((char *) &state.ram[USER_PROGRAM_OFFSET], romSize);
	rom.close();

	printChip8(&state);
	while (state.pc < STACK_OFFSET)
	{
		emulateChip8(&state);
		printChip8(&state);
	}
}
