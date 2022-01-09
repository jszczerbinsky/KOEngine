#ifndef KO_MOUSE_H
#define KO_MOUSE_H

#include <SDL2/SDL.h>
#include "KOEngine.h"

Vector2D GetMousePosition     ();
Vector2D GetMouseWorldPosition();

void resetButtons ();
void setButtonUp  (MouseButton btn);
void setButtonDown(MouseButton btn);

bool ButtonDown (MouseButton btn);
bool ButtonUp   (MouseButton btn);
bool ButtonPress(MouseButton btn);

bool MouseOver  (Entity *ent);
bool MouseClick (Entity *ent, MouseButton btn);

#endif
