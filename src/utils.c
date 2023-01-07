#ifndef KO_UTILS_H
#define KO_UTILS_H

#include "utils.h"

#include <math.h>

#include "KOE.h"

extern Resolution windowResolution;

int    Rand(int min, int max) { return rand() % (max - min) + min; }
double RandD() { return (double)rand() / RAND_MAX; }

float Clamp(float val, float min, float max)
{
  if (val > max) return max;
  if (val < min) return min;
  return val;
}

Vector2D PositionScreenToWorld(Vector2D pos)
{
  pos.x += GetCameraPosition().x;
  pos.y += GetCameraPosition().y;

  pos.x -= windowResolution.width / 2;
  pos.y -= windowResolution.height / 2;

  return pos;
}

Vector2D PositionWorldToScreen(Vector2D pos)
{
  pos.x -= GetCameraPosition().x;
  pos.y -= GetCameraPosition().y;

  pos.x += windowResolution.width / 2;
  pos.y += windowResolution.height / 2;

  return pos;
}

float Lerp(float f1, float f2, float t)
{
  if (t < 0)
    t = 0;
  else if (t > 1)
    t = 1;
  return f1 + (f2 - f1) * t;
}

Vector2D Lerp2D(Vector2D p1, Vector2D p2, float t)
{
  if (t < 0)
    t = 0;
  else if (t > 1)
    t = 1;

  Vector2D p;
  p.x = p1.x + (p2.x - p1.x) * t;
  p.y = p1.y + (p2.y - p1.y) * t;

  return p;
}

Timestamp GetTimestamp() { return SDL_GetTicks(); }

float GetTimeDiff(Timestamp t2, Timestamp t1) { return (t2 - t1) / 1000.0f; }

float Distance(Vector2D p1, Vector2D p2) { return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)); }

bool CheckInside(GameObject *ent, Vector2D startPos, Vector2D endPos)
{
  Vector2D entPos = GetPosition(ent);
  return entPos.x > startPos.x && entPos.x <= endPos.x && entPos.y > startPos.y &&
         entPos.y <= endPos.y;
}

#endif
