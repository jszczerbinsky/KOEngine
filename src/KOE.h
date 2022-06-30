#ifndef KO_KOENGINE_H
#define KO_KOENGINE_H

#include "collisions.h"
#include "gameObjects.h"
#include "keys.h"
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
#include "debug.h"
#include "multithreading.h"
#include "audio.h"
#include "uiControls/button.h"
#include "uiControls/progressBar.h"

#define WINDOW_NORMAL 0
#define WINDOW_NO_BORDERS SDL_WINDOW_FULLSCREEN_DESKTOP
#define WINDOW_FULLSCREEN SDL_WINDOW_FULLSCREEN

struct Resolution
{
  unsigned int width;
  unsigned int height;
};

struct KOEngineSettings
{
  char *windowName;
  void (*onStartPtr)();
  void (*loopCallPtr)();
  void (*onMusicEndPtr)(Music *music);
  unsigned int audioChannels;
};

extern float Delay;
extern struct Resolution WindowResolution;

void KOEngineInit(const struct KOEngineSettings *s);
void KOEngineExit();

void SetResolution(unsigned int width, unsigned int height);
void SetWindowMode(unsigned int mode);
void SetBackground(Color *c);

#endif
