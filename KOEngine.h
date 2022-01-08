#ifndef KO_KOENGINE_H
#define KO_KOENGINE_H

#include <stdint.h>
#include "collisions.h"
#include "entities.h"
#include "keyboard.h"
#include "types.h"
#include "camera.h"
#include "textures.h"
#include "animations.h"
#include "mouse.h"
#include "ui.h"
#include "utils.h"
#include "log.h"
#include "networking.h"

extern float Delay;

App * getAppInfo();
void SetResolution(unsigned short width, unsigned short height);
void SetBackground(uint8_t r, uint8_t g, uint8_t b);
void KOEngineInit(char *windowName, void (*onStartPtr)(), void (*loopCallPtr)());
void KOEngineExit();

#endif
