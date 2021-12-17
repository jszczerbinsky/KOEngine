#ifndef KO_UTILS_H
#define KO_UTILS_H

#include "types.h"

Vector2D Lerp(Vector2D p1, Vector2D p2, float t);
float Distance(Vector2D p1, Vector2D p2);
bool CheckInside(Entity *ent, Vector2D startPos, Vector2D endPos);

#endif
