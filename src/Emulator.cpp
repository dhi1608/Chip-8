/*
 * Emulator.cpp
 *
 * Created on: Mar 11, 2019
 * Author: Brian Green
 */

#include <iostream>
#include <fstream>
using namespace std;

typedef struct DataRegisters
{
	uint8_t v0;
	uint8_t v1;
	uint8_t v2;
	uint8_t v3;
	uint8_t v4;
	uint8_t v5;
	uint8_t v6;
	uint8_t v7;
	uint8_t v8;
	uint8_t v9;
	uint8_t vA;
	uint8_t vB;
	uint8_t vC;
	uint8_t vD;
	uint8_t vE;
	uint8_t vF;

} DataRegisters;

typedef struct InterpreterRegisters
{
	uint8_t stackPtr;
	uint16_t programCntr;
	uint8_t delayTimer;
	uint8_t soundTimer;
	uint16_t iPtr:12;
	uint16_t resv:4;
} InterpreterRegisters;

//TODO create a state structure with memory and registers
/******************************************
 * Main method
 ******************************************/
int main(const int argc, const char **argv)
{
	const size_t CHIP8_LANGUAGE_OFFSET = 0x0000;
	const size_t USER_PROGRAM_OFFSET = 0x0200;
	const size_t STACK_OFFSET = 0x06A0;
	const size_t ADDRESS_REGISTER_OFFSET = 0x06D0;
	const size_t DATA_REGISTER_OFFSET = 0x06F0;
	const size_t DISPLAY_REFRESH_OFFSET = 0X0700;

	const int RAM_SIZE = 2048;
	uint8_t *ram = new uint8_t[RAM_SIZE];
	/**
	 * Confirm user has provided correct inputs and provide
	 * help if they did not
	 */
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	/**
	 * Open the binary file
	 */
	ifstream rom;
	rom.open(argv[1], ios::in | ios::binary);
	if (!rom)
	{
		printf("Unable to open %s\n", argv[1]);
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
	rom.read((char *) &ram[USER_PROGRAM_OFFSET], romSize);
	rom.close();

	//TODO place holder to verify data is stored in correct location
	// need to add structures next
	int pc = USER_PROGRAM_OFFSET - 2;
	while (pc < STACK_OFFSET)
	{
		printf("%04X\t", pc);
		printf("%02X%02X\n", ram[pc], ram[pc + 1]);
		pc += 2;
	}

	/**
	 * Cleanup dynamic memory allocation
	 */
	delete[] ram;
}
