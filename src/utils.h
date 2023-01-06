#ifndef KO_UTILS_H
#define KO_UTILS_H

#include "types.h"

Vector2D PositionWorldToScreen(Vector2D pos);
Vector2D PositionScreenToWorld(Vector2D pos);

int    Rand(int min, int max);
double RandD();

float Clamp(float val, float min, float max);

float    Lerp(float f1, float f2, float t);
Vector2D Lerp2D(Vector2D p1, Vector2D p2, float t);

float Distance(Vector2D p1, Vector2D p2);

bool CheckInside(GameObject *ent, Vector2D startPos, Vector2D endPos);

#endif
