#ifndef KO_TEXTURES_H
#define KO_TEXTURES_H

#include <SDL2/SDL.h>
#include "KOE.h"

#define TEX_FLIP_NONE SDL_FLIP_NONE
#define TEX_FLIP_H SDL_FLIP_HORIZONTAL
#define TEX_FLIP_V SDL_FLIP_VERTICAL

Texture* LoadTexture       (const char * path);
Texture* CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void FreeTexture(Texture *tex);

#endif
