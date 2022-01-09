#ifndef KO_KEYBOARD_H
#define KO_KEYBOARD_H

#include <stdbool.h>
#include <SDL2/SDL.h>

void setKeyUp  (SDL_KeyboardEvent *event);
void setKeyDown(SDL_KeyboardEvent *event);
void resetKeys ();

bool KeyPress(SDL_Scancode key);
bool KeyDown (SDL_Scancode key);
bool KeyUp   (SDL_Scancode key);

#endif
