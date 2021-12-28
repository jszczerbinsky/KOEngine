#ifndef KO_UI_H
#define KO_UI_H

#include "entities.h"

Entity *CreateUIObject(
    int x, int y, 
    unsigned short width, unsigned short height, 
    Texture *texture, 
    unsigned short maxWidth, 
    unsigned char layer, 
    Font *font, 
    SDL_Color color
);
void SetUIText(Entity *ent, char *text);

#endif
