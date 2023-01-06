#ifndef KO_ANIMATIONS_H
#define KO_ANIMATIONS_H

#include "types.h"

Animation *CreateAnimation(float speed, int count, ...);
Animation *LoadAnimation(float speed, int count, const char *path);

void FreeAnimation(Animation *anim);

#endif
