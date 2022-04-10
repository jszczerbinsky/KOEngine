#ifndef KO_UTILS_H
#define KO_UTILS_H

#include <math.h>
#include "KOE.h"
#include "utils.h"

Vector2D PositionScreenToWorld(Vector2D pos)
{
	pos.x += GetCameraPosition().x;
	pos.y += GetCameraPosition().y;

	pos.x -= WindowResolution.width/2;
	pos.y -= WindowResolution.height/2;

	return pos;
}

Vector2D PositionWorldToScreen(Vector2D pos)
{
	pos.x -= GetCameraPosition().x;
	pos.y -= GetCameraPosition().y;

	pos.x += WindowResolution.width/2;
	pos.y += WindowResolution.height/2;

	return pos;
}

Vector2D Lerp(Vector2D p1, Vector2D p2, float t)
{
	if(t < 0 ) t = 0;
	else if(t > 1) t = 1;

	Vector2D p;
	p.x = p1.x + (p2.x - p1.x) * t;
	p.y = p1.y + (p2.y - p1.y) * t;

	return p;
}

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
