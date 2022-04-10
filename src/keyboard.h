#ifndef KO_KEYBOARD_H
#define KO_KEYBOARD_H

#include <stdbool.h>
#include <SDL2/SDL.h>

bool KeyPress(SDL_Scancode key);
bool KeyDown (SDL_Scancode key);
bool KeyUp   (SDL_Scancode key);

#endif
