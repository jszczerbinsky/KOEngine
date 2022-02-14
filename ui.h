#ifndef KO_UI_H
#define KO_UI_H

#include "entities.h"
#include "ui.h"

Font *LoadFont(char *path, int size, SDL_Color color);
void CloseFont(Font *font);

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
