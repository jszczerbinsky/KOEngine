#ifndef KO_KEYBOARD_H
#define KO_KEYBOARD_H

#include <stdbool.h>

#include "keys.h"

bool KeyPress(Key key);
bool KeyDown(Key key);
bool KeyUp(Key key);

#endif
