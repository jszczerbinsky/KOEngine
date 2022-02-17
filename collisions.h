#ifndef KO_COLLISION_H
#define KO_COLLISION_H

#include <stdbool.h>
#include "types.h"

#define COLLIDER_MODE_NORMAL 0
#define COLLIDER_MODE_EVENTS_ONLY 1

struct RectColliderSettings
{
  int centerX;
  int centerY;
  unsigned int width;
  unsigned int height;
};

struct RegularColliderSettings
{
  int centerX;
  int centerY;
  unsigned int radius;
  unsigned int edges;
};

void NullCollider     ( Collider *col                                    );
void RectCollider     ( Collider *col, struct RectColliderSettings *s    );
void RegularCollider  ( Collider *col, struct RegularColliderSettings *s );

#endif
