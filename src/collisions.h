#ifndef KO_COLLISION_H
#define KO_COLLISION_H

#include <stdbool.h>

#include "types.h"

#define COLLIDER_MODE_NORMAL      0
#define COLLIDER_MODE_EVENTS_ONLY 1

typedef struct
{
  int centerX;
  int centerY;
  int width;
  int height;
} RectColliderSettings;

typedef struct
{
  int centerX;
  int centerY;
  int radius;
  int edges;
} RegularColliderSettings;

void NullCollider(Collider *col);
void RectCollider(Collider *col, const RectColliderSettings *s);
void RegularCollider(Collider *col, const RegularColliderSettings *s);

#endif
