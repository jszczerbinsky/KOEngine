#ifndef KO_KOENGINE_H
#define KO_KOENGINE_H

#include "animations.h"
#include "audio.h"
#include "camera.h"
#include "collisions.h"
#include "console.h"
#include "debug.h"
#include "gameObjects.h"
#include "keyboard.h"
#include "keys.h"
#include "light.h"
#include "log.h"
#include "mouse.h"
#include "multithreading.h"
#include "networking.h"
#include "textures.h"
#include "types.h"
#include "ui.h"
#include "uiControls/button.h"
#include "uiControls/progressBar.h"
#include "utils.h"

#define WINDOW_NORMAL     0
#define WINDOW_NO_BORDERS SDL_WINDOW_FULLSCREEN_DESKTOP
#define WINDOW_FULLSCREEN SDL_WINDOW_FULLSCREEN

typedef struct
{
  unsigned int width;
  unsigned int height;
} Resolution;

typedef struct
{
  char *windowName;
  void (*onStartPtr)();
  void (*loopCallPtr)();
  void (*onMusicEndPtr)(Music *music);
  unsigned int audioChannels;
  unsigned int layersCount;
  unsigned int lightLayer;
} KOEngineSettings;

extern float Delay;

void KOEngineInit(const KOEngineSettings *s);
void KOEngineExit();

Resolution GetResolution();

void SetResolution(unsigned int width, unsigned int height);
void SetWindowMode(unsigned int mode);
void SetBackground(Color *c);

#endif
