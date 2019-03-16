/*
 * Emulator.cpp
 *
 * Created on: Mar 11, 2019
 * Author: Brian Green
 */

#include <iostream>
#include <fstream>
using namespace std;

const uint16_t CHIP8_LANGUAGE_OFFSET = 0x0000;
const uint16_t USER_PROGRAM_OFFSET = 0x0200;
const uint16_t STACK_OFFSET = 0x06A0;
const uint16_t ADDRESS_REGISTER_OFFSET = 0x06D0;
const uint16_t DATA_REGISTER_OFFSET = 0x06F0;
const uint16_t DISPLAY_REFRESH_OFFSET = 0X0700;
const int RAM_SIZE = 2048;
const int STACK_DEPTH = 32;
const int V_REGISTER_CNT = 16;

typedef struct StateChip8
{
	// Memory
	uint8_t *ram;
	// Data
	uint8_t v[V_REGISTER_CNT] =
	{ 0 };
	// Registers
	uint8_t dt = 0;
	uint8_t st = 0;
	uint16_t sp = 0;
	uint16_t pc = 0;
	uint16_t i = 0;
} StateChip8;

/**
 * Identify unimplemented instructions
 */
void unimplementedInstruction(StateChip8* state)
{
	printf("Error: Unimplemented instruction %04X\n", state->pc);
	exit(1);
}

void printChip8(StateChip8* state)
{

	/**
	 * Print registers
	 */
	printf("PC:%04X       SP:%04X       IP:%04X       DT:%02X/ST:%02X\n",
			state->pc, state->sp, state->i, state->dt, state->st);

	/**
	 * Print V[0:F]
	 */
	for (short i = 1; i <= V_REGISTER_CNT; ++i)
	{
		printf("V[%X]:%02X", i - 1, state->v[i - 1]);
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
		//TODO save this for display logic implementation
		unimplementedInstruction(state);
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
		printf("TODO Machine Language subroutine??");
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
	uint8_t key;
	switch (mode)
	{
	case 0x9E:
		printf("TODO check for key [%X] pressed", x);
		key = state->v[x];
		if (state->v[x] == key)
			incrementPC(state);
		break;
	case 0xA1:
		printf("TODO check for key [%X] pressed", x);
		key = state->v[x];
		if (state->v[x] != key)
			incrementPC(state);
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
	uint8_t buffer[RAM_SIZE] =
	{ 0 };
	StateChip8 state;
	state.ram = buffer;
	state.sp = STACK_OFFSET;
	state.pc = USER_PROGRAM_OFFSET;

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
