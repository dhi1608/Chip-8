/*
 * Chip8Ext.h
 */
#ifndef CHIP8EXT_H
#define CHIP8EXT_H

#include <SDL2/SDL.h>
#include "Chip8Core.h"

/**
 * SDL window
 */
static SDL_Window* window = NULL;
/**
 * SDL renderer
 */
static SDL_Renderer* renderer = NULL;
/**
 * SDL texture
 */
static SDL_Texture* texture = NULL;

/**
 * This array maps a SDL scancode to the CHIP-8 key that serves as index
 * for the array. For instance, pressing key keys[5] on your keyboard will
 * make your CHIP-8 understand as if you pressed the [5] key. Remember that
 * CHIP-8 uses a 16-key keyboard with keys labeled 0..F.
 */
const char keys[] = {
    SDL_SCANCODE_0, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_4, // 4
    SDL_SCANCODE_5, // 5
    SDL_SCANCODE_6, // 6
    SDL_SCANCODE_7, // 7
    SDL_SCANCODE_8, // 8
    SDL_SCANCODE_9, // 9
    SDL_SCANCODE_A, // A
    SDL_SCANCODE_B, // B
    SDL_SCANCODE_C, // C
    SDL_SCANCODE_D, // D
    SDL_SCANCODE_E, // E
    SDL_SCANCODE_F  // F
};

int initializeContext();
void removeContext();
void renderDisplay(State *state);
int isCloseRequested();
int isKeydown(char);

#endif /* CHIP8EXT_H */
