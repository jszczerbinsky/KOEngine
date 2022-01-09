#ifndef KO_COLLISION_H
#define KO_COLLISION_H

#include <stdbool.h>
#include "types.h"

#define COLLIDER_MODE_NORMAL 0
#define COLLIDER_MODE_EVENTS_ONLY 1

Collider GenerateNullCollider();

Collider RectCollider   (short x, short y, unsigned short width, unsigned short height);
Collider RegularCollider(short x, short y, unsigned short size, unsigned char edges);

bool checkCollision(Entity *ent1, Entity *ent2);

#endif
