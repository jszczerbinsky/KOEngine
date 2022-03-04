#include <stdarg.h>

#include "entities.h"
#include "KOEngine.h"

extern bool checkCollision(Entity *ent1, Entity *ent2);

extern void renderEntity  (Entity *e, Vector2D pos, Vector2D camPos);

Entity *entList[LAYER_MAX];
NetworkID nextNetworkID = 1;

void initEntities()
{
	for(unsigned char layer = 0; layer < LAYER_MAX; layer++)
		entList[layer] = NULL;
}

Vector2D getNonRotatedPosition(Entity *ent)
{
	Vector2D pos = ent->localPosition;
	if(ent->parent != NULL)
	{
		Vector2D parentNonRotatedPos = getNonRotatedPosition(ent->parent);
		pos.x += parentNonRotatedPos.x;
		pos.y += parentNonRotatedPos.y;	
	}

	return pos;
}

void inheritPosition(Entity *ent, Vector2D *posPtr)
{
	double angleInRadians = ent->localRotation * (M_PI / 180);
	double cosTheta = cos(angleInRadians);
	double sinTheta = sin(angleInRadians);

	Vector2D nonRotatedPosition = getNonRotatedPosition(ent);

	Vector2D posPtrBackup;
	posPtrBackup.x = posPtr->x;
	posPtrBackup.y = posPtr->y;

	posPtr->x = 
		(cosTheta * (posPtrBackup.x - nonRotatedPosition.x)) - 
		(sinTheta * (posPtrBackup.y - nonRotatedPosition.y)) +
		nonRotatedPosition.x;

	posPtr->y = 
		(sinTheta * (posPtrBackup.x - nonRotatedPosition.x)) + 
		(cosTheta * (posPtrBackup.y - nonRotatedPosition.y)) +
		nonRotatedPosition.y;

	if(ent->parent != NULL) inheritPosition(ent->parent, posPtr);
}

void inheritRotation(Entity *ent, float *rotPtr)
{
	(*rotPtr) += ent->localRotation;
	if(ent->parent != NULL) inheritRotation(ent->parent, rotPtr);
}

Vector2D GetPosition(Entity *ent)
{
	Vector2D pos = getNonRotatedPosition(ent);
	if(ent->parent != NULL) inheritPosition(ent->parent, &pos);
	return pos;
}

float GetRotation(Entity *ent)
{
	float rot = ent->localRotation;
	if(ent->parent != NULL) inheritRotation(ent->parent, &rot);
	return rot;
}

bool CheckAnyCollision(Entity *ent)
{
	if(ent->collider.verticesCount == 0) return false;

	Entity *next;

	for(unsigned char layer = 0; layer < LAYER_MAX; layer++)
		for(Entity *e = entList[layer]; e; e = next)
		{
			next = e->next;

			if(e == ent) continue;	

			if(checkCollision(e, ent))
				return true;
		}

	return false;
}

void Move(Entity *ent, float x, float y)
{
	if(x != 0 && y != 0)
	{
		Move(ent, x, 0);
		Move(ent, 0, y);
		return;
	}

	ent->localPosition.x += x;
	ent->localPosition.y += y;

	if(CheckAnyCollision(ent)) 
	{
		ent->localPosition.x -= x;
		ent->localPosition.y -= y;
		return;
	}
}

void MoveLocal(Entity *ent, float x, float y)
{
	float rads = GetRotation(ent) * M_PI/180;

	Move(ent,
			cos(rads)*x+
			sin(rads)*y,
			sin(rads)*x-
			cos(rads)*y
	);
}

void MoveTo(Entity *ent, Vector2D dest, float speed)
{
	float xDistance = dest.x - GetPosition(ent).x;
	float yDistance = dest.y - GetPosition(ent).y;

	float distance = Distance(GetPosition(ent), dest);

	float xNormal = xDistance / distance;
	float yNormal = yDistance / distance;

	Move(ent, xNormal * speed, yNormal * speed);
}

void addEntity(Entity *ent, unsigned int layer)
{
	ent->next = entList[layer];

	if(entList[layer] != NULL)
		entList[layer]->prev = ent;

	entList[layer] = ent;

}

Entity *SpawnEntity(const struct EntitySpawnSettings *s)
{
	Entity *ent = malloc(sizeof(Entity));

	ent->networkID 							= NO_NETWORK_ID;
	ent->layer 									= s->layer;
	ent->ui 										= NULL;
	ent->parent 								= NULL;
	ent->localPosition.x 				= s->x;
	ent->localPosition.y 				= s->y;
	ent->localRotation 					= 0;
	ent->width 									= s->width;
	ent->height 								= s->height;
	ent->defaultTexture 				= s->texture;
	ent->actualTexture 					= s->texture;
	ent->animationCounter 			= 0;
	ent->actualAnimation 				= NULL;
	ent->onAnimationEnd 				= NULL;
	ent->colliderMode 					= COLLIDER_MODE_NORMAL;
	ent->onCollision 						= NULL;
	ent->flip 									= SDL_FLIP_NONE;
	ent->loopCall 							= NULL;
	ent->extensionType 					= 0;
	ent->extension 							= NULL;
	ent->freeExtension 					= NULL;

	ent->prev = NULL;
	ent->next = NULL;

	NullCollider(&ent->collider);

	addEntity(ent, s->layer);	

	return entList[s->layer];
}

void KillEntity(Entity *entity)
{
	if(entity->ui != NULL)
	{
		if(entity->ui->textTexture)
			SDL_DestroyTexture(entity->ui->textTexture);
		free(entity->ui);
	}

	if(entity->prev != NULL)
		entity->prev->next = entity->next;
	else
		entList[entity->layer] = entity->next;
	if(entity->next != NULL)
		entity->next->prev = entity->prev;

	if(entity->collider.vertices != NULL)
		free(entity->collider.vertices);

	if(entity->extension)
	{
		if(entity->freeExtension)
			(*entity->freeExtension)(entity->extension);
		free(entity->extension);
	}

	free(entity);
}

void SetRotationTo(Entity *ent, Vector2D p)
{
	float rot = (float)((180 / M_PI) * atan2(p.y - GetPosition(ent).y, p.x - GetPosition(ent).x));
	if (rot < 0)
	{
			rot = 360 - (-rot);
	}
	rot += 90;

	float increaseRot = GetRotation(ent) - ent->localRotation;
	ent->localRotation = rot - increaseRot;
}

void RotateTo(Entity *ent, Vector2D p, float speed)
{
	float rot = GetRotation(ent);
	SetRotationTo(ent, p);

	float diff = GetRotation(ent) - rot;

	while(diff < 0) diff = 360 + diff;
	while(diff > 360) diff -= 360;

	if(abs(diff) < speed) return;

	if(diff < 180) ent->localRotation = rot+speed;
	else ent->localRotation = rot-speed;
}

NetworkID AssignNetworkID(Entity *ent)
{
	ent->networkID = nextNetworkID;
	nextNetworkID++;
	return nextNetworkID-1;
}

void updateEntities()
{
	Vector2D camPos = GetCameraPosition();

	for(unsigned char layer = 0; layer < LAYER_MAX; layer++)
	{
		Entity *next = entList[layer];

		while(next)
		{
			Entity *e = next;
			next = e->next;

			Vector2D pos = GetPosition(e);

			if(e->actualAnimation != NULL)
			{
				e->animationCounter += Delay * e->actualAnimation->speed;
				bool ended = false;
				while(e->animationCounter >= e->actualAnimation->texturesCount)
				{
					e->animationCounter -= e->actualAnimation->texturesCount;
					ended = true;
				}
				if(ended && e->onAnimationEnd != NULL) (*e->onAnimationEnd)(e, e->actualAnimation);
				if(e->actualAnimation) e->actualTexture = e->actualAnimation->textures[(int)(e->animationCounter)];
			}

			renderEntity(e, pos, camPos);

			if(e->loopCall != NULL)
				(*e->loopCall)(e);	
		}
	}
}

void freeEntities()
{
	for(unsigned char layer = 0; layer < LAYER_MAX; layer++)
		while(entList[layer] != NULL)
			KillEntity(entList[layer]);
}

void Play(Entity *ent, Animation *anim, SDL_RendererFlip flip)
{
	ent->actualAnimation = anim;
	ent->animationCounter = 0;
	ent->flip = flip;
}

void StopPlaying(Entity *ent)
{
	ent->actualAnimation = NULL;
	ent->flip = SDL_FLIP_NONE;
	ent->actualTexture = ent->defaultTexture;
}
