/*
 * Chip8Core.h
 *
 * Defines the Chip8 data structures and
 * instruction implementations.
 */
#ifndef CHIP8CORE_H
#define CHIP8CORE_H

#include <cstdint>

/**
 * Flag to print state information
 */
const bool DEBUG = false;
/**
 * Flag to print decoded instructions
 */
const bool DEBUG1 = false;
/**
 * Starting memory location for Chip8 language (original interpreter)
 * and fonts
 */
const int CHIP8_LANGUAGE_OFFSET = 0x0000;
/**
 * Starting memory location for user programs
 */
const int USER_PROGRAM_OFFSET = 0x0200;
/**
 * Starting memory location for Chip8 stack
 */
const int STACK_OFFSET = 0x06A0;
/**
 * Starting memory location for Chip8 address registers
 */
const int ADDRESS_REGISTER_OFFSET = 0x06D0;
/**
 * Starting memory location for Chip8 data registers V[0] - V[F]
 */
const int DATA_REGISTER_OFFSET = 0x06F0;
/**
 * Starting memory location for Chip8 display
 */
const int DISPLAY_REFRESH_OFFSET = 0X0700;
/**
 * Chip8 ram size in bytes
 */
const int RAM_SIZE = 2048;
/**
 * Number of rows in Chip8 display
 */
const int DISPLAY_ROW_COUNT = 64;
/**
 * Number of columns in Chip8 display
 */
const int DISPLAY_COL_COUNT = 32;
/**
 * Chip8 display size
 */
const int DISPLAY_SIZE = DISPLAY_ROW_COUNT * DISPLAY_COL_COUNT;
/**
 * Chip8 stack size
 */
const int STACK_SIZE = 16;
/**
 * Chip8 keypad size
 */
const int KEYPAD_SIZE = 16;
/**
 * 60 hz clock rate
 */
const int CLOCK_RATE_MS = 1000 / 60;

/**
 * Keypad callback
 */
typedef int (*keypad_handler_t)(char);

/**
 * Structure for holding instruction variables
 */
typedef struct Variables
{
	uint16_t nnn; //A 12-bit value, the lowest 12 bits of the instruction
	uint8_t kk; // An 8-bit value, the lowest 8 bits of the instruction
	uint8_t mode; // A 4-bit value, the upper 4 bits of the high byte of the instruction
	uint8_t x; // A 4-bit value, the lower 4 bits of the high byte of the instruction
	uint8_t y; // A 4-bit value, the upper 4 bits of the low byte of the instruction
	uint8_t n; // A 4-bit value, the lowest 4 bits of the instruction

} Variables;

/**
 * Structure for holding Chip8 state values
 */
typedef struct State
{
	// Memory
	uint8_t ram[RAM_SIZE];
	// Data
	uint8_t v[KEYPAD_SIZE];
	// Registers
	uint8_t dt;
	uint8_t st;
	uint16_t pc;
	uint16_t i;
	// Stack
	uint16_t stack[KEYPAD_SIZE];
	uint8_t sp;
	// Display
	char display[DISPLAY_SIZE];

	// Keypad
    keypad_handler_t keydown;
    int keyWait;

} State;

void initialize(State *state);
void executeStep(State *state);
void updateTimerRegisters(State *state, int delta);
int loadFile(char *filename, State *state);

#endif /* CHIP8CORE_H */
