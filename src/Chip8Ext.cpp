/*
 * Chip8Ext.cpp
 */

#include <cmath>
#include "Chip8Core.h"
#include "Chip8Ext.h"

#define TEXTURE_PIXEL(x, y) (128 * (y) + (x))
/**
 * Maps Chip8 display to bitmap
 */
void mapDisplay(char* from, Uint32* to)
{

	int x = 0, y = 0;
	for (int i = 0; i < DISPLAY_SIZE; i++)
	{
		Uint32 val = from[i] ? -1 : 0;
		to[TEXTURE_PIXEL(2 * x + 0, 2 * y + 0)] = val;
		to[TEXTURE_PIXEL(2 * x + 1, 2 * y + 0)] = val;
		to[TEXTURE_PIXEL(2 * x + 0, 2 * y + 1)] = val;
		to[TEXTURE_PIXEL(2 * x + 1, 2 * y + 1)] = val;
		if (++x == DISPLAY_ROW_COUNT)
		{
			x = 0;
			y++;
		}
	}
}

/**
 * Initialize SDL context
 */
int initializeContext()
{
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        return 1;
    }
    window = SDL_CreateWindow("CHIP8",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 320, SDL_WINDOW_SHOWN);
    if (window == NULL) {
    	removeContext();
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
    	removeContext();
        return 1;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STREAMING, 128, 64);
    if (texture == NULL) {
    	removeContext();
        return 1;
    }
    return 0;
}

/**
 * Removes SDL context
 */
void removeContext()
{
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
	}
	if (renderer != NULL)
	{
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window != NULL)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}
	SDL_Quit();
}

/**
 * Checks for close requests
 */
int isCloseRequested()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            return 1;
        }
    }
    return 0;
}

/**
 * Renders the display
 */
void renderDisplay(State *state)
{
    void*   pixels;
    int     pitch;

    /* Update SDL Texture with current data in CPU. */
    SDL_LockTexture(texture, NULL, &pixels, &pitch);
    mapDisplay(state->display, (Uint32 *) pixels);
    SDL_UnlockTexture(texture);

    /* Render the texture. */
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

/**
 * Checks if a given key is pressed. This function acceps a CHIP-8 key in
 * range 0-F. It will check using SDL if the PC keyboard mapped to that
 * CHIP-8 key is acutally being pressed or not.
 *
 * @param key CHIP-8 key to be checked.
 * @return 0 if that key is not down; != 0 if that key IS down.
 */
int isKeydown(char key)
{
    const Uint8* sdl_keys; // SDL key array information
    Uint8 real_key; // Mapped SDL scancode for the given key
    if (key < 0 || key > 15) return 0; // check those bounds.

    sdl_keys = SDL_GetKeyboardState(NULL);
    real_key = keys[(int) key];
    return sdl_keys[real_key];
}
