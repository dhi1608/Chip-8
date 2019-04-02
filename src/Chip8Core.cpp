/*
 * Chip8Core.cpp
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include "Chip8Core.h"
#include "Chip8Font.h"

using namespace std;

int currDelta;
Variables var;

/**
 * Initialize Chip8 memory and states
 */
void initialize(State *state)
{
	memset(state, 0, sizeof(State));
	memcpy(&state->ram[CHIP8_LANGUAGE_OFFSET], &Font4x5, sizeof(Font4x5));
	state->pc = USER_PROGRAM_OFFSET;
	state->keyWait = -1;
	currDelta = 0;
	memset(&var, 0, sizeof(Variables));
}

/**
 * Loads the Chip8 file and places it in memory
 */
int loadFile(char *filename, State *state)
{
	/**
	 * Open the binary file
	 */
	std::ifstream rom;
	rom.open(filename, std::ios::in | std::ios::binary);
	if (!rom)
	{
		printf("Unable to open %s\n", filename);
		return 1;
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
		return 1;
	}

	/**
	 * Read ROM into memory and close file
	 */
	rom.read((char *) &state->ram[USER_PROGRAM_OFFSET], romSize);
	rom.close();
	return 0;
}

/**
 * Updates the time registers
 */
void updateTimerRegisters(State *state, int delta)
{
	currDelta += delta;
	while (currDelta > CLOCK_RATE_MS)
	{
		currDelta -= CLOCK_RATE_MS;
		if (state->dt > 0)
		{
			--state->dt;
		}
		if (state->st > 0)
		{
			if (--state->st == 0)
			{
				printf("BEEP!\n");
				fflush(stdout);
			}
		}
	}
}

/**
 * Sets the instruction variables for the given instruction
 */
void setVariables(State *state)
{
//	uint16_t nnn; //A 12-bit value, the lowest 12 bits of the instruction
//	uint8_t kk; // An 8-bit value, the lowest 8 bits of the instruction
//	uint8_t mode; // A 4-bit value, the upper 4 bits of the high byte of the instruction
//	uint8_t x; // A 4-bit value, the lower 4 bits of the high byte of the instruction
//	uint8_t y; // A 4-bit value, the upper 4 bits of the low byte of the instruction
//	uint8_t n; // A 4-bit value, the lowest 4 bits of the instruction
//	var.nnn =
//
//			uint8_t byte1 = state->ram[state->pc] & 0x0F;
//			uint8_t byte2 = state->ram[state->pc + 1];
//			uint8_t mode = state->ram[state->pc] >> 4 & 0x0F;

}

/**
 * Enabled by setting DEBUG=true
 * Prints the registers and stack values
 */
void printState(State *state)
{
	/**
	 * Print registers
	 */
	printf(
			"PC:%04X       SP:%02X         IP:%04X       DT:%02X/ST:%02X/KW:%X\n",
			state->pc, state->sp, state->i, state->dt, state->st,
			state->keyWait);

	/**
	 * Print V[0:F]
	 */
	for (short i = 1; i <= KEYPAD_SIZE; ++i)
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
	for (short i = 1; i <= STACK_SIZE; ++i)
	{
		printf("S[%04X]:%04X", STACK_OFFSET + i * 2, state->stack[i - 1]);
		if (i % 4 == 0)
			printf("\n");
		else
			printf("  ");
	}
	printf("\n");
}

/**
 * Safely increment stack pointer
 */
void incrementSP(State *state)
{
	if (state->sp < STACK_SIZE - 1)
		++state->sp;
	else
		printf("Warn: Max stack pointer reached can't increment\n");
}

/**
 * Safely decrement stack pointer
 */
void decrementSP(State *state)
{
	if (state->sp)
		--state->sp;
	else
		printf("Warn: Min stack pointer reached can't decrement\n");
}

/**
 * Safely increment program counter
 */
void incrementPC(State *state)
{
	if (STACK_OFFSET > state->pc)
		state->pc += 2;
	else
		printf("Warn: Max program counter reached can't increment\n");
}

/**
 * Identify unimplemented instructions
 */
void unimplementedInstruction(State *state)
{
	printf("Warn: Unimplemented instruction %04X\n", state->pc);
	printState(state);
	exit(0);
}

/**
 * Method processes instruction 0 types
 *
 * state - chip 8 state data
 */
void processInstruction0(State *state)
{
	uint8_t byte1 = state->ram[state->pc] & 0xFF;
	uint8_t byte2 = state->ram[state->pc + 1] & 0xFF;

	if (byte1 == 0x00 && byte2 == 0xE0)
	{
		if (DEBUG1)
			printf("CLS\n");
		memset(state->display, 0, DISPLAY_SIZE);
	}
	else if (byte1 == 0x00 && byte2 == 0xEE)
	{
		if (DEBUG1)
			printf("RET\n");
		decrementSP(state);
		state->pc = state->stack[state->sp];
		state->stack[state->sp] = 0;
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
void processInstruction8(State *state)
{
	uint8_t x = state->ram[state->pc] & 0x0F;
	uint8_t y = state->ram[state->pc + 1] >> 4 & 0x0F;
	uint8_t mode = state->ram[state->pc + 1] & 0x0F;
	uint16_t result;

	switch (mode)
	{
	case 0x0:
		if (DEBUG1)
			printf("LD V%X, V%X\n", x, y);
		state->v[x] = state->v[y];
		break;
	case 0x1:
		if (DEBUG1)
			printf("OR V%X, V%X\n", x, y);
		state->v[x] = state->v[x] | state->v[y];
		break;
	case 0x2:
		if (DEBUG1)
			printf("AND V%X, V%X\n", x, y);
		state->v[x] = state->v[x] & state->v[y];
		break;
	case 0x3:
		if (DEBUG1)
			printf("XOR V%X, V%X\n", x, y);
		state->v[x] = state->v[x] ^ state->v[y];
		break;
	case 0x4:
		if (DEBUG1)
			printf("ADD V%X, V%X\n", x, y);
		result = state->v[x] + state->v[y];
		state->v[x] = result & 0x00FF;
		state->v[0xF] = result >> 8;
		break;
	case 0x5:
		if (DEBUG1)
			printf("SUB V%X, V%X\n", x, y);
		result = state->v[x] - state->v[y];
		state->v[x] = result & 0x00FF;
		state->v[0xF] = ~(result >> 8 & 0x01);
		break;
	case 0x6:
		if (DEBUG1)
			printf("SHR V%X, {V%X}\n", x, y);
		state->v[x] =  state->v[x] >> 1;
		state->v[0xF] = state->v[x] & 0x01;
		break;
	case 0x7:
		if (DEBUG1)
			printf("SUBN V%X, V%X\n", x, y);
		result = state->v[y] - state->v[x];
		state->v[x] = result & 0x00FF;
		state->v[0xF] = ~(result >> 8 & 0x01);
		break;
	case 0xE:
		if (DEBUG1)
			printf("SHL V%X, {V%X}\n", x, y);
		state->v[x] =  state->v[x] << 1;
		state->v[0xF] = state->v[x] & 0x80;
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
void processInstructionE(State *state)
{
	uint8_t x = state->ram[state->pc] & 0x0F;
	uint8_t mode = state->ram[state->pc + 1] & 0xFF;
	uint8_t key = state->v[x];
	switch (mode)
	{
	case 0x9E:
		if (DEBUG1)
			printf("SKP V%X\n", x);
		if (state->keydown && state->keydown(key & 0xF))
			incrementPC(state);
		break;
	case 0xA1:
		if (DEBUG1)
			printf("SKNP V%X\n", x);
		if (state->keydown && !state->keydown(key & 0xF))
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
void processInstructionF(State *state)
{
	uint8_t x = state->ram[state->pc] & 0x0F;
	uint8_t mode = state->ram[state->pc + 1] & 0xFF;

	switch (mode)
	{
	case 0x07:
		if (DEBUG1)
			printf("LD V%X, DT\n", x);
		state->v[x] = state->dt;
		break;
	case 0x0A:
		if (DEBUG1)
			printf("LD V%X, K\n", x);
		state->keyWait = x;
		break;
	case 0x15:
		if (DEBUG1)
			printf("LD DT, V%X\n", x);
		state->dt = state->v[x];
		break;
	case 0x18:
		if (DEBUG1)
			printf("LD ST, V%X\n", x);
		state->st = state->v[x];
		break;
	case 0x1E:
		if (DEBUG1)
			printf("ADD I, V%X\n", x);
		state->i = state->i + state->v[x];
		break;
	case 0x29:
		if (DEBUG1)
			printf("LD F, V%X\n", x);
		state->i = CHIP8_LANGUAGE_OFFSET
				+ (state->v[x] * (sizeof(Font4x5) / KEYPAD_SIZE));
		break;
	case 0x33:
	{
		if (DEBUG1)
			printf("LD B, V%X\n", x);
		state->ram[state->i + 2] = state->v[x] % 10;
		state->ram[state->i + 1] = (state->v[x] / 10) % 10;
		state->ram[state->i] = state->v[x] / 100;

	}
		break;
	case 0x55:
	{
		if (DEBUG1)
			printf("LD [I], V%X\n", x);
		for (short i = 0; i <= x; ++i)
			state->ram[state->i + i] = state->v[i];
	}
		break;
	case 0x65:
	{
		if (DEBUG1)
			printf("LD V%X, [I]\n", x);
		for (short i = 0; i <= x; ++i)
			state->v[i] = state->ram[state->i + i];
	}
		break;
	default:
		unimplementedInstruction(state);
		break;

	}
}

/**
 * Step through Chip8 instructions
 * state - chip 8 state data
 */
void executeStep(State *state)
{
	uint8_t byte1 = state->ram[state->pc] & 0x0F;
	uint8_t byte2 = state->ram[state->pc + 1];
	uint8_t mode = state->ram[state->pc] >> 4 & 0x0F;
	/* check for key press event */
	if (state->keyWait != -1 && state->keydown)
	{
		for (int i = 0; i < 16; i++)
		{
			int status = state->keydown(i);
			if (status)
			{
				/* Key was down. Restore system. */
				state->v[state->keyWait] = i;
				state->keyWait = -1;
				break;
			}
		}
		/* Test again. If we are still waiting for a key, don't fetch. */
		if (state->keyWait != -1)
		{
			return;
		}
	}

	if (DEBUG)
		printState(state);
	switch (mode)
	{
	case 0x0:
		processInstruction0(state);
		break;
	case 0x1:
		if (DEBUG1)
			printf("JP %02X%02X\n", byte1, byte2);
		state->pc = (byte1 << 8 | byte2) - 2;
		break;
	case 0x2:
		if (DEBUG1)
			printf("CALL %02X%02X\n", byte1, byte2);
		state->stack[state->sp] = state->pc;
		state->pc = (byte1 << 8 | byte2) - 2;
		incrementSP(state);
		break;
	case 0x3:
		if (DEBUG1)
			printf("SE V%X, %02X\n", byte1, byte2);
		if (state->v[byte1] == byte2)
			incrementPC(state);
		break;
	case 0x4:
		if (DEBUG1)
			printf("SNE V%X, %02X\n", byte1, byte2);
		if (state->v[byte1] != byte2)
			incrementPC(state);
		break;
	case 0x5:
		if (DEBUG1)
			printf("SE V%X, V%X\n", byte1, byte2 >> 4 & 0x0F);
		if (state->v[byte1] == state->v[byte2 >> 4 & 0x0F])
			incrementPC(state);
		break;
	case 0x6:
		if (DEBUG1)
			printf("LD V%X, %02X\n", byte1, byte2);
		state->v[byte1] = byte2;
		break;
	case 0x7:
		if (DEBUG1)
			printf("ADD V%X, %02X\n", byte1, byte2);
		state->v[byte1] += byte2;
		break;
	case 0x8:
		processInstruction8(state);
		break;
	case 0x9:
		if (DEBUG1)
			printf("SNE V%X, V%X\n", byte1, byte2 >> 4 & 0x0F);
		if (state->v[byte1] != state->v[byte2 >> 4 & 0x0F])
			incrementPC(state);
		break;
	case 0xA:
		if (DEBUG1)
			printf("LD I, %02X%02X\n", byte1, byte2);
		state->i = byte1 << 8 | byte2;
		break;
	case 0xB:
		if (DEBUG1)
			printf("JP V0 %02X%02X\n", byte1, byte2);
		state->pc = (byte1 << 8 | byte2) + state->v[0];
		break;
	case 0xC:
		if (DEBUG1)
			printf("RND V%X, %02X\n", byte1, byte2);
		state->v[byte1] = (rand() % 256) & byte2;
		break;
	case 0xD:
	{
		if (DEBUG1)
			printf("DRW V%X, V%X %X\n", byte1, byte2 >> 4 & 0x0F, byte2 & 0x0F);
		uint8_t x = byte1;
		uint8_t y = byte2 >> 4 & 0x0F;
		uint8_t n = byte2 & 0x0F;

		for (int j = 0; j < n; j++)
		{
			uint8_t sprite = state->ram[state->i + j];
			for (int i = 0; i < 8; i++)
			{
				int px = (state->v[x] + i) % DISPLAY_ROW_COUNT;
				int py = (state->v[y] + j) % DISPLAY_COL_COUNT;
				int pos = DISPLAY_ROW_COUNT * py + px;
				int pixel = (sprite & (1 << (7 - i))) != 0;
				state->v[0xF] |= (state->display[pos] & pixel);
				state->display[pos] ^= pixel;
			}
		}
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
	incrementPC(state);

}
