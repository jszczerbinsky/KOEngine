#ifndef KO_COLLISION_H
#define KO_COLLISION_H

#include <stdbool.h>
#include "types.h"

Collider GenerateNullCollider();
bool checkCollision(Entity *ent1, Entity *ent2);

Collider RectCollider(short x, short y, unsigned short width, unsigned short height);

Collider RegularCollider(short x, short y, unsigned short size, unsigned char edges);

#endif
