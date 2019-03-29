/*
 * Chip8.cpp
 *
 */
#include <iostream>
#include <fstream>
#include <ctime>
#include "Chip8Core.h"
#include "Chip8Ext.h"

/******************************************
 * Main method
 ******************************************/
int main(int argc, char** argv)
{
	/**
	 * Confirm user has provided correct inputs and provide
	 * help if they did not
	 */
	if (argc != 2)
	{
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	/* Initialize SDL Context. */
	if (initializeContext())
	{
		fprintf(stderr, "Error initializing SDL graphical context:\n");
		fprintf(stderr, "%s\n", SDL_GetError());
		return 1;
	}

	/* Init emulator. */
	srand(time(NULL));
	State state;
	initialize(&state);
	state.keydown = &isKeydown;

	/**
	 * Open the binary file
	 */
	std::ifstream rom;
	rom.open(argv[1], std::ios::in | std::ios::binary);
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
	rom.read((char *) &state.ram[USER_PROGRAM_OFFSET], romSize);
	rom.close();

	int lastTicks = SDL_GetTicks();
	int lastDelta = 0;
	int stepDelta = 0;
	int renderDelta = 0;

	while (!isCloseRequested())
	{
		/* Update timers. */
		lastDelta = SDL_GetTicks() - lastTicks;
		lastTicks = SDL_GetTicks();
		stepDelta += lastDelta;
		renderDelta += lastDelta;

		while (stepDelta >= 1)
		{
//	TODO step thru code
//			char k;
//			std::cin >> k;
			executeStep(&state);
			stepDelta--;
		}

		updateTimerRegisters(&state, lastDelta);

		/* Render frame every 1/60th of second. */
		while (renderDelta >= CLOCK_RATE_MS)
		{
			renderDisplay(&state);
			renderDelta -= CLOCK_RATE_MS;
		}
		SDL_Delay(1);
	}

	removeContext();
	return 0;

}
