/*
 * Chip8Ext.h
 */
#ifndef CHIP8EXT_H
#define CHIP8EXT_H

#include <SDL2/SDL.h>
#include "Chip8Core.h"

int initializeContext();
void removeContext();
void renderDisplay(State *state);
int isCloseRequested();
int isKeydown(char);

#endif /* CHIP8EXT_H */
