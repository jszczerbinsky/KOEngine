#ifndef KO_TEXTURES_H
#define KO_TEXTURES_H

#include <SDL2/SDL.h>
#include "KOEngine.h"

Texture* LoadTexture       (const char * path);
Texture* CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void FreeTexture(Texture *tex);

#endif
