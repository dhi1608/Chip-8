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
	var.nnn = ((state->ram[state->pc] & 0xF) << 8) | state->ram[state->pc + 1];
    var.kk = state->ram[state->pc + 1];
    var.mode = (state->ram[state->pc] >> 4) & 0xF;
    var.x = state->ram[state->pc] & 0xF;
    var.y = (state->ram[state->pc + 1] >> 4) & 0xF;
    var.n = state->ram[state->pc + 1] & 0xF;
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
 * returns false on failure
 */
bool incrementSP(State *state)
{
	if (state->sp < (STACK_SIZE - 1))
		++state->sp;
	else
	{
		printf("Warn: Max stack pointer reached can't increment\n");
		return false;
	}
	return true;
}

/**
 * Safely decrement stack pointer
 * returns false on failure
 */
bool decrementSP(State *state)
{
	if (state->sp)
		--state->sp;
	else
	{
		printf("Warn: Min stack pointer reached can't decrement\n");
		return false;
	}
	return true;
}

/**
 * Safely increment program counter
 * return false on failure
 */
bool incrementPC(State *state)
{
	if (STACK_OFFSET > state->pc)
		state->pc += 2;
	else
	{
		printf("Warn: Max program counter reached can't increment\n");
		return false;
	}
	return true;
}

/**
 * Identify unimplemented instructions
 */
void unimplementedInstruction(State *state)
{
	printf("Error: Unimplemented instruction PC[%04X]=>%X%03X\n", state->pc, var.mode, var.nnn);
	printf("Halt system!\n");
	printState(state);
	exit(0);
}

/**
 * Step through Chip8 instructions
 * state - chip 8 state data
 */
void executeStep(State *state)
{
	if (DEBUG)
		printState(state);
	setVariables(state);
	incrementPC(state);

	/* check for key press event */
	if (state->keyWait != -1 && state->keydown)
	{
		for (int i = 0; i < 16; ++i)
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

	switch (var.mode)
	{
	case 0x0:
	{
		switch (var.nnn)
		{
		case 0x0E0:
			if (DEBUG1)
				printf("CLS\n");
			memset(state->display, 0, DISPLAY_SIZE);
			break;
		case 0x0EE:
			if (DEBUG1)
				printf("RET\n");
			if (decrementSP(state))
			{
				state->pc = state->stack[state->sp];
				state->stack[state->sp] = 0;
			}
			break;
		default:
			unimplementedInstruction(state);
			break;
		}
	}
		break;
	case 0x1:
		if (DEBUG1)
			printf("JP %03X\n", var.nnn);
		state->pc = var.nnn;
		break;
	case 0x2:
		if (DEBUG1)
			printf("CALL %03X\n", var.nnn);
		state->stack[state->sp] = state->pc;
		if (incrementSP(state))
		{
			state->pc = var.nnn;
		}
		break;
	case 0x3:
		if (DEBUG1)
			printf("SE V%X, %02X\n", var.x, var.kk);
		if (state->v[var.x] == var.kk)
			incrementPC(state);
		break;
	case 0x4:
		if (DEBUG1)
			printf("SNE V%X, %02X\n", var.x, var.kk);
		if (state->v[var.x] != var.kk)
			incrementPC(state);
		break;
	case 0x5:
		if (DEBUG1)
			printf("SE V%X, V%X\n", var.x, var.y);
		if (state->v[var.x] == state->v[var.y])
			incrementPC(state);
		break;
	case 0x6:
		if (DEBUG1)
			printf("LD V%X, %02X\n", var.x, var.kk);
		state->v[var.x] = var.kk;
		break;
	case 0x7:
		if (DEBUG1)
			printf("ADD V%X, %02X\n", var.x, var.kk);
		state->v[var.x] = state->v[var.x] + var.kk;
		break;
	case 0x8:
	{
		switch (var.n)
		{
		case 0x0:
			if (DEBUG1)
				printf("LD V%X, V%X\n", var.x, var.y);
			state->v[var.x] = state->v[var.y];
			break;
		case 0x1:
			if (DEBUG1)
				printf("OR V%X, V%X\n", var.x, var.y);
			state->v[var.x] = state->v[var.x] | state->v[var.y];
			break;
		case 0x2:
			if (DEBUG1)
				printf("AND V%X, V%X\n", var.x, var.y);
			state->v[var.x] = state->v[var.x] & state->v[var.y];
			break;
		case 0x3:
			if (DEBUG1)
				printf("XOR V%X, V%X\n", var.x, var.y);
			state->v[var.x] = state->v[var.x] ^ state->v[var.y];
			break;
		case 0x4:
			if (DEBUG1)
				printf("ADD V%X, V%X\n", var.x, var.y);
			state->v[0xF] = (state->v[var.x] > ((state->v[var.x] + state->v[var.y]) & 0xFF));
			state->v[var.x] = state->v[var.x] + state->v[var.y];
			break;
		case 0x5:
			if (DEBUG1)
				printf("SUB V%X, V%X\n", var.x, var.y);
			state->v[0xF] = (state->v[var.x] > state->v[var.y]);
			state->v[var.x] = state->v[var.x] - state->v[var.y];

			break;
		case 0x6:
			if (DEBUG1)
				printf("SHR V%X, {V%X}\n", var.x, var.y);
			state->v[0xF] = state->v[var.x] & 0x1;
			state->v[var.x] = state->v[var.x] >> 1;
			break;
		case 0x7:
			if (DEBUG1)
				printf("SUBN V%X, V%X\n", var.x, var.y);
			state->v[0xF] = (state->v[var.x] < state->v[var.y]);
			state->v[var.x] = state->v[var.y] - state->v[var.x];
			break;
		case 0xE:
			if (DEBUG1)
				printf("SHL V%X, {V%X}\n", var.x, var.y);
			state->v[0xF] = ((state->v[var.x] & 0x80) != 0);
			state->v[var.x] = state->v[var.x] << 1;
			break;
		default:
			unimplementedInstruction(state);
			break;
		}
	}
		break;
	case 0x9:
		if (DEBUG1)
			printf("SNE V%X, V%X\n", var.x, var.y);
		if (state->v[var.x] != state->v[var.y])
			incrementPC(state);
		break;
	case 0xA:
		if (DEBUG1)
			printf("LD I, %03X\n", var.nnn);
		state->i = var.nnn;
		break;
	case 0xB:
		if (DEBUG1)
			printf("JP V0 %03X\n", var.nnn);
		state->pc = var.nnn + state->v[0];
		break;
	case 0xC:
		if (DEBUG1)
			printf("RND V%X, %02X\n", var.x, var.kk);
		state->v[var.x] = (rand() % 256) & var.kk;
		break;
	case 0xD:
	{
		if (DEBUG1)
			printf("DRW V%X, V%X %X\n", var.x, var.y, var.n);
		for (int j = 0; j < var.n; ++j)
		{
			uint8_t sprite = state->ram[state->i + j];
			for (int i = 0; i < 8; ++i)
			{
				int px = (state->v[var.x] + i) % DISPLAY_ROW_COUNT;
				int py = (state->v[var.y] + j) % DISPLAY_COL_COUNT;
				int pos = DISPLAY_ROW_COUNT * py + px;
				int pixel = (sprite & (1 << (7 - i))) != 0;
				state->v[0xF] = state->v[0xF] | (state->display[pos] & pixel);
				state->display[pos] = state->display[pos] ^ pixel;
			}
		}
	}
		break;
	case 0xE:
		{
			char key = state->v[var.x];
			switch (var.kk)
				{
				case 0x9E:
					if (DEBUG1)
						printf("SKP V%X\n", var.x);
					if (state->keydown && state->keydown(key & 0xF))
						incrementPC(state);
					break;
				case 0xA1:
					if (DEBUG1)
						printf("SKNP V%X\n", var.x);
					if (state->keydown && !state->keydown(key & 0xF))
						incrementPC(state);
					break;
				default:
					unimplementedInstruction(state);
					break;
				}
		}
		break;
	case 0xF:
		{
			switch (var.kk)
			{
			case 0x07:
				if (DEBUG1)
					printf("LD V%X, DT\n", var.x);
				state->v[var.x] = state->dt;
				break;
			case 0x0A:
				if (DEBUG1)
					printf("LD V%X, K\n", var.x);
				state->keyWait = var.x;
				break;
			case 0x15:
				if (DEBUG1)
					printf("LD DT, V%X\n", var.x);
				state->dt = state->v[var.x];
				break;
			case 0x18:
				if (DEBUG1)
					printf("LD ST, V%X\n", var.x);
				state->st = state->v[var.x];
				break;
			case 0x1E:
				if (DEBUG1)
					printf("ADD I, V%X\n", var.x);
				state->i = state->i + state->v[var.x];
				break;
			case 0x29:
				if (DEBUG1)
					printf("LD F, V%X\n", var.x);
				state->i = CHIP8_LANGUAGE_OFFSET
						+ (state->v[var.x] * (sizeof(Font4x5) / KEYPAD_SIZE));
				break;
			case 0x33:
			{
				if (DEBUG1)
					printf("LD B, V%X\n", var.x);
				state->ram[state->i + 2] = state->v[var.x] % 10;
				state->ram[state->i + 1] = (state->v[var.x] / 10) % 10;
				state->ram[state->i] = state->v[var.x] / 100;

			}
				break;
			case 0x55:
			{
				if (DEBUG1)
					printf("LD [I], V%X\n", var.x);
				for (short i = 0; i <= var.x; ++i)
					state->ram[state->i + i] = state->v[i];
			}
				break;
			case 0x65:
			{
				if (DEBUG1)
					printf("LD V%X, [I]\n", var.x);
				for (short i = 0; i <= var.x; ++i)
					state->v[i] = state->ram[state->i + i];
			}
				break;
			default:
				unimplementedInstruction(state);
				break;

			}
		}
		break;
	default:
		unimplementedInstruction(state);
		break;

	}
}
