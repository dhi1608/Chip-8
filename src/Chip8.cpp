/*
 * Chip8.cpp
 *
 */
#include <iostream>
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
		printf("Error initializing SDL graphical context:\n");
		printf("%s\n", SDL_GetError());
		return 1;
	}

	/* Init emulator. */
	srand(time(NULL));
	State state;
	initialize(&state);
	state.keydown = &isKeydown;

	if (loadFile(argv[1], &state))
	{
		return 1;
	}

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
