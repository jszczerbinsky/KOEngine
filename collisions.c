#include "collisions.h"
#include "types.h"

void NullCollider(Collider *col)
{
	col->verticesCount = 0;
	col->vertices = NULL;
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

	if(ent1->onCollision)
		(*ent1->onCollision)(ent1, ent2);

	if(ent2->onCollision)
		(*ent2->onCollision)(ent2, ent1);


	if(ent1->colliderMode == COLLIDER_MODE_EVENTS_ONLY &&
		ent2->colliderMode == COLLIDER_MODE_EVENTS_ONLY)
		return false;

	return true;
}

void RectCollider(Collider *col, struct RectColliderSettings *s)
{
	col->verticesCount = 4;
	col->vertices = malloc(4*sizeof(Vector2D));

	col->vertices[0].x = s->centerX - s->width  /2;
	col->vertices[0].y = s->centerY + s->height /2;
	col->vertices[1].x = s->centerX + s->width  /2;
	col->vertices[1].y = s->centerY + s->height	/2;
	col->vertices[2].x = s->centerX + s->width	/2;
	col->vertices[2].y = s->centerY - s->height	/2;
	col->vertices[3].x = s->centerX - s->width	/2;
	col->vertices[3].y = s->centerY - s->height	/2;
}

void RegularCollider(Collider *col, struct RegularColliderSettings *s)
{
	col->verticesCount = s->edges;
	col->vertices = malloc(s->edges*sizeof(Vector2D));

	float angle = M_PI * 2 /s->edges;

	for(unsigned char i = 0; i < s->edges; i++)
	{
		col->vertices[i].x = s->radius*sin(i*angle)+s->centerX;
		col->vertices[i].y = s->radius*cos(i*angle)+s->centerY;
	}
}
