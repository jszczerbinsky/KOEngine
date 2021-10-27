#ifndef KO_UI_H
#define KO_UI_H

#include "entities.h"

Entity *CreateUIObject(int x, int y, unsigned short width, unsigned short height, SDL_Texture *texture, unsigned char layer, Font *font);

#endif
