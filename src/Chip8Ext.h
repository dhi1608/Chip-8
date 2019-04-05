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
 * make your CHIP-8 understand as if you pressed the [5] key.
 */
const char keys[] = {
    SDL_SCANCODE_1, // 0
    SDL_SCANCODE_2, // 1
    SDL_SCANCODE_3, // 2
    SDL_SCANCODE_4, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_R, // 7
    SDL_SCANCODE_A, // 8
    SDL_SCANCODE_S, // 9
    SDL_SCANCODE_D, // A
    SDL_SCANCODE_F, // B
    SDL_SCANCODE_Z, // C
    SDL_SCANCODE_X, // D
    SDL_SCANCODE_C, // E
    SDL_SCANCODE_V  // F
};

int initializeContext();
void removeContext();
void renderDisplay(State *state);
int isCloseRequested();
int isKeydown(char);

#endif /* CHIP8EXT_H */
