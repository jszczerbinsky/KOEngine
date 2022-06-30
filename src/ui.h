#ifndef KO_UI_H
#define KO_UI_H

#include "gameObjects.h"

#define UI_PARAMETERS_FLAGS_HORIZONTAL_ALIGN_MASK 0b00000011
#define UI_PARAMETERS_FLAGS_VERTICAL_ALIGN_MASK   0b00001100
#define UI_PARAMETERS_FLAGS_TEXTWRAP_MASK         0b00110000

#define TEXT_ALIGN_H_LEFT     0b00000000
#define TEXT_ALIGN_H_RIGHT    0b00000001
#define TEXT_ALIGN_H_CENTER   0b00000010
#define TEXT_ALIGN_V_TOP      0b00000000
#define TEXT_ALIGN_V_BOTTOM   0b00000100 
#define TEXT_ALIGN_V_CENTER   0b00001000
#define TEXT_WRAP_NO_WRAP     0b00000000
#define TEXT_WRAP_WORD_BREAK  0b00010000
#define TEXT_WRAP_NORMAL      0b00100000

struct UISpawnSettings
{
  unsigned int  flags;
  Font         *font;
};

Font *LoadFont(char *path, int size, SDL_Color color);
void CloseFont(Font *font);

GameObject *CreateUIObject(const struct GameObjectSpawnSettings *s, const struct UISpawnSettings *uis);
void SetUIText(GameObject *ent, char *text);

#endif
