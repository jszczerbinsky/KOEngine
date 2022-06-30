#ifndef KO_ANIMATIONS_H
#define KO_ANIMATIONS_H

#include "types.h"

#define ANIM_FLIP_NONE SDL_FLIP_NONE
#define ANIM_FLIP_H SDL_FLIP_HORIZONTAL
#define ANIM_FLIP_V SDL_FLIP_VERTICAL

Animation *CreateAnimation(float speed, int count, ...);
Animation *LoadAnimation  (float speed, int count, const char * path);

void FreeAnimation(Animation *anim);

#endif
