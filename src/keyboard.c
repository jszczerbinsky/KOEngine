#include "keyboard.h"

#include <SDL2/SDL.h>

static bool keysPressed[SDL_NUM_SCANCODES];
static bool keysDown[SDL_NUM_SCANCODES];
static bool keysUp[SDL_NUM_SCANCODES];

void resetKeys()
{
  for (unsigned short i = 0; i < SDL_NUM_SCANCODES; i++)
  {
    keysPressed[i] = 0;
    keysUp[i]      = 0;
  }
}
void setKeyUp(SDL_KeyboardEvent *event)
{
  keysUp[event->keysym.scancode]   = 1;
  keysDown[event->keysym.scancode] = 0;
}
void setKeyDown(SDL_KeyboardEvent *event)
{
  keysDown[event->keysym.scancode]    = 1;
  keysPressed[event->keysym.scancode] = 1;
}

bool KeyDown(Key key) { return keysDown[key]; }
bool KeyUp(Key key) { return keysUp[key]; }
bool KeyPress(Key key) { return keysPressed[key]; }
