#ifndef BUTTON_H
#define BUTTON_H

#include "../types.h"
#include "../ui.h"
#include "extensions.h"

typedef struct
{
  Texture *texHover;
  Texture *texClick;
  void    *clickParam;
  void (*clickAction)(GameObject *btn, void *p);
  void (*rightClickAction)(GameObject *btn, void *p);
} ButtonSettings;

GameObject *SpawnButton(float x, float y, unsigned int w, unsigned int h, Texture *tex,
                        unsigned int layer, unsigned int uiFlags, Font *font, ButtonSettings *s);
void        UpdateButtonSettings(GameObject *obj, const ButtonSettings *s);

#endif
