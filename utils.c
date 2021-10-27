#ifndef KO_UTILS_H
#define KO_UTILS_H

#include "KOEngine.h"
#include "utils.h"

float Distance(Vector2D p1, Vector2D p2)
{
	return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y,2 ));
}

bool CheckInside(Entity *ent, Vector2D startPos, Vector2D endPos)
{
	Vector2D entPos = GetPosition(ent);
	return
		entPos.x > startPos.x &&
		entPos.x <= endPos.x &&
		entPos.y > startPos.y &&
		entPos.y <= endPos.y;
}

#endif
