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
  int width;
  int height;
};

struct RegularColliderSettings
{
  int centerX;
  int centerY;
  int radius;
  int edges;
};

void NullCollider     ( Collider *col                                          );
void RectCollider     ( Collider *col, const struct RectColliderSettings *s    );
void RegularCollider  ( Collider *col, const struct RegularColliderSettings *s );

#endif
