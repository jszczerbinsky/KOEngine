#ifndef KO_TEXTURES_H
#define KO_TEXTURES_H

#include <SDL2/SDL.h>
#include "KOEngine.h"

SDL_Texture * LoadTexture(const char * path);
SDL_Texture * CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

#endif
