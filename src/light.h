#ifndef KO_LIGHT_H
#define KO_LIGHT_H

#include "types.h"

struct LightSettings
{
  int      width;
  int      height;
  Texture *texture;
};

void AssignLightSource(GameObject *obj, struct LightSettings *s);
void RemoveLightSource(GameObject *obj);
void SetLightTint(Color color);

#endif
