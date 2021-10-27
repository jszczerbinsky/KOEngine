#ifndef GAME_COLLISIONS_H
#define GAME_COLLISIONS_H

#include "collisions.h"
#include "types.h"

Collider GenerateNullCollider()
{
	Collider col;
	col.verticesCount = 0;
	col.vertices = NULL;
	return col;
}

bool checkCollision(Entity *ent1, Entity *ent2)
{
	if(ent1->collider.verticesCount == 0 || ent2->collider.verticesCount == 0) return false;

	Entity *ents[] = { ent1, ent2 };

	for (int x = 0; x < 2; x++)
	{
		Collider col = ents[x]->collider;
		Entity *ent = ents[x];

		for (int i1 = 0; i1 < col.verticesCount; i1++)
		{ 
			int i2 = (i1 + 1) % col.verticesCount;

			Vector2D p1 = col.vertices[i1];
			Vector2D p2 = col.vertices[i2];

			p1.x += ent->localPosition.x;
			p1.y += ent->localPosition.y;
			p2.x += ent->localPosition.x;
			p2.y += ent->localPosition.y;

			Vector2D normal;
			normal.x = p2.y - p1.y;
			normal.y = p1.x - p2.x;

			float minA = INFINITY;
			float maxA = -INFINITY;

			for(int j = 0; j < ent1->collider.verticesCount; j++)
			{
				Vector2D p = ent1->collider.vertices[j];
				p.x += ent1->localPosition.x;
				p.y += ent1->localPosition.y;

				float projected = normal.x * p.x + normal.y * p.y;

				if (projected < minA)
						minA = projected;
				if (projected > maxA)
						maxA = projected;
			}

			float minB = INFINITY;
			float maxB = -INFINITY;

			for(int j = 0; j < ent2->collider.verticesCount; j++)
			{
				Vector2D p = ent2->collider.vertices[j];
				p.x += ent2->localPosition.x;
				p.y += ent2->localPosition.y;

				float projected = normal.x * p.x + normal.y * p.y;

				if (projected < minB)
						minB = projected;
				if (projected > maxB)
						maxB = projected;
			}

			if (maxA < minB || maxB < minA)
					return false;
		}
	}

	return true;
}

Collider RectCollider(short x, short y, unsigned short width, unsigned short height)
{
	Collider col;
	col.verticesCount = 4;
	col.vertices = malloc(4*sizeof(Vector2D));

	col.vertices[0].x = x-width/2;
	col.vertices[0].y = y+height/2;
	col.vertices[1].x = x+width/2;
	col.vertices[1].y = y+height/2;
	col.vertices[2].x = x+width/2;
	col.vertices[2].y = y-height/2;
	col.vertices[3].x = x-width/2;
	col.vertices[3].y = y-height/2;

	return col;

}

Collider RegularCollider(short x, short y, unsigned short size, unsigned char edges)
{
	Collider col;
	col.verticesCount = edges;
	col.vertices = malloc(edges*sizeof(Vector2D));

	float angle = M_PI * 2 /edges;

	for(unsigned char i = 0; i < edges; i++)
	{
		col.vertices[i].x = size*sin(i*angle)+x;
		col.vertices[i].y = size*cos(i*angle)+y;
	}

	return col;
}

#endif
