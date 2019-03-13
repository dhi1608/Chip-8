/*
 * Emulator.cpp
 *
 * Created on: Mar 11, 2019
 * Author: Brian Green
 */

#include <iostream>
#include <fstream>
using namespace std;

typedef struct StateChip8
{
	// Memory
	uint8_t *ram;
	// Data
	uint8_t v[16];
	// Registers
	uint8_t dt;
	uint8_t st;
	uint16_t sp;
	uint16_t pc;
	uint16_t i;
} StateChip8;

const uint16_t CHIP8_LANGUAGE_OFFSET = 0x0000;
const uint16_t USER_PROGRAM_OFFSET = 0x0200;
const uint16_t STACK_OFFSET = 0x06A0;
const uint16_t ADDRESS_REGISTER_OFFSET = 0x06D0;
const uint16_t DATA_REGISTER_OFFSET = 0x06F0;
const uint16_t DISPLAY_REFRESH_OFFSET = 0X0700;
const int RAM_SIZE = 2048;
const int STACK_DEPTH = 16;

/**
 * Identify unimplemented instructions
 */
void unimplementedInstruction(StateChip8* state)
{
	printf("Error: Unimplemented instruction %04X\n", state->pc);
	exit(1);
}


void incrementSP(StateChip8* state)
{
	if (STACK_OFFSET + STACK_DEPTH > state->sp )
	{
		++state->sp;
	}
	else
	{
		printf("Error: Max stack pointer reached can't increment\n");
	}
}
void decrementSP(StateChip8* state)
{
	if (STACK_OFFSET < state->sp )
	{
		--state->sp;
	}
	else
	{
		printf("Error: Min stack pointer reached can't decrement\n");
	}
}
/**
 * Method processes instruction 0 types
 *
 * state - chip 8 state data
 */
void processInstruction0(StateChip8* state)
{
	unsigned char byte1 = state->ram[state->pc] & 0xFF;
	unsigned char byte2 = state->ram[state->pc + 1] & 0xFF;

	if (byte1 == 0x00 && byte2 == 0xE0)
	{
		unimplementedInstruction(state);
	}
	else if (byte1 == 0x00 && byte2 == 0xEE)
	{
		unimplementedInstruction(state);
	}
	else
	{
		unimplementedInstruction(state);
	}
}

/**
 * Method processes instruction 8 types
 *
 * state - chip 8 state data
 */
void processInstruction8(StateChip8* state)
{
	unsigned char x = state->ram[state->pc] & 0x0F;
	unsigned char y = state->ram[state->pc + 1] >> 4 & 0x0F;
	unsigned char mode = state->ram[state->pc + 1] & 0x0F;

	switch (mode)
	{
	case 0x0:
		unimplementedInstruction(state);
		break;
	case 0x1:
		unimplementedInstruction(state);
		break;
	case 0x2:
		unimplementedInstruction(state);
		break;
	case 0x3:
		unimplementedInstruction(state);
		break;
	case 0x4:
		unimplementedInstruction(state);
		break;
	case 0x5:
		unimplementedInstruction(state);
		break;
	case 0x6:
		unimplementedInstruction(state);
		break;
	case 0x7:
		unimplementedInstruction(state);
		break;
	case 0xE:
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
	unsigned char x = state->ram[state->pc] & 0x0F;
	unsigned char mode = state->ram[state->pc + 1] & 0xFF;

	switch (mode)
	{
	case 0x9E:
		unimplementedInstruction(state);
		break;
	case 0xA1:
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
	unsigned char x = state->ram[state->pc] & 0x0F;
	unsigned char mode = state->ram[state->pc + 1] & 0xFF;

	switch (mode)
	{
	case 0x07:
		unimplementedInstruction(state);
		break;
	case 0x0A:
		unimplementedInstruction(state);
		break;
	case 0x15:
		unimplementedInstruction(state);
		break;
	case 0x18:
		unimplementedInstruction(state);
		break;
	case 0x1E:
		unimplementedInstruction(state);
		break;
	case 0x29:
		unimplementedInstruction(state);
		break;
	case 0x33:
		unimplementedInstruction(state);
		break;
	case 0x55:
		unimplementedInstruction(state);
		break;
	case 0x65:
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
	unsigned char byte1 = state->ram[state->pc] & 0x0F;
	unsigned char byte2 = state->ram[state->pc + 1];
	unsigned char mode = state->ram[state->pc] >> 4 & 0x0F;
	switch (mode)
	{
	case 0x0:
		processInstruction0(state);
		break;
	case 0x1:
		unimplementedInstruction(state);
		break;
	case 0x2:
		incrementSP(state);
		state->ram[state->sp] = byte1;
		state->ram[state->sp +1] = byte2;
		state->pc = byte1 << 8 | byte2;
		break;
	case 0x3:
		unimplementedInstruction(state);
		break;
	case 0x4:
		if (state->v[byte1] != byte2)
		{
			state->pc += 2;
		}
		break;
	case 0x5:
		unimplementedInstruction(state);
		break;
	case 0x6:
		state->v[byte1] = byte2;
		state->pc += 2;
		break;
	case 0x7:
		unimplementedInstruction(state);
		break;
	case 0x8:
		processInstruction8(state);
		break;
	case 0x9:
		unimplementedInstruction(state);
		break;
	case 0xA:
		unimplementedInstruction(state);
		break;
	case 0xB:
		unimplementedInstruction(state);
		break;
	case 0xC:
		unimplementedInstruction(state);
		break;
	case 0xD:
		unimplementedInstruction(state);
		break;
	case 0xE:
		unimplementedInstruction(state);
		break;
	case 0xF:
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
	uint8_t *buffer = new uint8_t[RAM_SIZE];
	StateChip8 state;
	state.ram = buffer;
	state.sp = STACK_OFFSET;
	state.pc = USER_PROGRAM_OFFSET;

	/**
	 * Read ROM into memory and close file
	 */
	rom.read((char *) &state.ram[USER_PROGRAM_OFFSET], romSize);
	rom.close();

	while (state.pc < STACK_OFFSET)
	{
		printf("%04X\n", state.pc);
		emulateChip8(&state);
	}

	/**
	 * Cleanup dynamic memory allocation
	 */
	delete[] buffer;
}
