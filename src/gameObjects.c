#include <stdarg.h>
#include <math.h>

#include "gameObjects.h"
#include "KOE.h"

extern bool checkCollision(GameObject *obj1, GameObject *obj2);

extern void renderGameObject  (GameObject *e, Vector2D pos, Vector2D camPos);
extern void renderLightTint();

GameObject **objList;
NetworkID nextNetworkID = 1;

unsigned int layersCount;
unsigned int lightLayer;

int killingAllGameObjects = 0;

void initGameObjects(unsigned int layers, unsigned int lightL)
{
	layersCount = layers;
	lightLayer = lightL;

	objList = malloc(layers*sizeof(GameObject*));
	for(unsigned char layer = 0; layer < layersCount; layer++)
		objList[layer] = NULL;
}

void freeGameObjects()
{
	free(objList);
}

Vector2D getNonRotatedPosition(GameObject *obj)
{
	Vector2D pos = obj->localPosition;
	if(obj->parent != NULL)
	{
		Vector2D parentNonRotatedPos = getNonRotatedPosition(obj->parent);
		pos.x += parentNonRotatedPos.x;
		pos.y += parentNonRotatedPos.y;	
	}

	return pos;
}

void inheritPosition(GameObject *obj, Vector2D *posPtr)
{
	double angleInRadians = obj->localRotation * (M_PI / 180);
	double cosTheta = cos(angleInRadians);
	double sinTheta = sin(angleInRadians);

	Vector2D nonRotatedPosition = getNonRotatedPosition(obj);

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

	if(obj->parent != NULL) inheritPosition(obj->parent, posPtr);
}

void inheritRotation(GameObject *obj, float *rotPtr)
{
	(*rotPtr) += obj->localRotation;
	if(obj->parent != NULL) inheritRotation(obj->parent, rotPtr);
}

Vector2D GetPosition(GameObject *obj)
{
	Vector2D pos = getNonRotatedPosition(obj);
	if(obj->parent != NULL) inheritPosition(obj->parent, &pos);
	return pos;
}

float GetRotation(GameObject *obj)
{
	float rot = obj->localRotation;
	if(obj->parent != NULL) inheritRotation(obj->parent, &rot);
	return rot;
}

bool CheckAnyCollision(GameObject *obj)
{
	if(obj->collider.verticesCount == 0) return false;

	GameObject *next;

	for(unsigned char layer = 0; layer < layersCount; layer++)
		for(GameObject *e = objList[layer]; e; e = next)
		{
			next = e->next;

			if(e == obj) continue;	

			if(checkCollision(e, obj))
				return true;
		}

	return false;
}

void Move(GameObject *obj, float x, float y)
{
	if(x != 0 && y != 0)
	{
		Move(obj, x, 0);
		Move(obj, 0, y);
		return;
	}

	obj->localPosition.x += x;
	obj->localPosition.y += y;

	if(CheckAnyCollision(obj)) 
	{
		obj->localPosition.x -= x;
		obj->localPosition.y -= y;
		return;
	}
}

void MoveLocal(GameObject *obj, float x, float y)
{
	float rads = GetRotation(obj) * M_PI/180;

	Move(obj,
			cos(rads)*x+
			sin(rads)*y,
			sin(rads)*x-
			cos(rads)*y
	);
}

void MoveTo(GameObject *obj, Vector2D dest, float speed)
{
	float xDistance = dest.x - GetPosition(obj).x;
	float yDistance = dest.y - GetPosition(obj).y;

	float distance = Distance(GetPosition(obj), dest);

	float xNormal = xDistance / distance;
	float yNormal = yDistance / distance;

	Move(obj, xNormal * speed, yNormal * speed);
}

void addGameObject(GameObject *obj, unsigned int layer)
{
	obj->next = objList[layer];

	if(objList[layer] != NULL)
		objList[layer]->prev = obj;

	objList[layer] = obj;

}

GameObject *SpawnGameObject(const struct GameObjectSpawnSettings *s)
{
	GameObject *obj = malloc(sizeof(GameObject));

	obj->networkID 							= NO_NETWORK_ID;
	obj->layer 									= s->layer;
	obj->ui 										= NULL;
	obj->light 									= NULL;
	obj->parent 								= NULL;
	obj->localPosition.x 				= s->x;
	obj->localPosition.y 				= s->y;
	obj->localRotation 					= 0;
	obj->width 									= s->width;
	obj->height 								= s->height;
	obj->defaultTexture 				= s->texture;
	obj->currentTexture 					= s->texture;
	obj->animationCounter 			= 0;
	obj->currentAnimation 				= NULL;
	obj->onAnimationEnd 				= NULL;
	obj->colliderMode 					= COLLIDER_MODE_NORMAL;
	obj->onCollision 						= NULL;
	obj->flip 									= SDL_FLIP_NONE;
	obj->loopCall 							= NULL;
	obj->extensionType 					= 0;
	obj->extension 							= NULL;
	obj->freeExtension 					= NULL;

	obj->prev = NULL;
	obj->next = NULL;

	NullCollider(&obj->collider);

	addGameObject(obj, s->layer);	

	return objList[s->layer];
}

void KillGameObject(GameObject *gameObject)
{
	if(gameObject->ui != NULL)
	{
		if(gameObject->ui->textTexture)
			SDL_DestroyTexture(gameObject->ui->textTexture);
		free(gameObject->ui);
	}

	if(gameObject->light != NULL)
		free(gameObject->light);

	if(gameObject->prev != NULL)
		gameObject->prev->next = gameObject->next;
	else
		objList[gameObject->layer] = gameObject->next;
	if(gameObject->next != NULL)
		gameObject->next->prev = gameObject->prev;

	if(gameObject->collider.vertices != NULL)
		free(gameObject->collider.vertices);

	if(gameObject->extension)
	{
		if(gameObject->freeExtension)
			(*gameObject->freeExtension)(gameObject->extension, killingAllGameObjects);
		free(gameObject->extension);
	}

	free(gameObject);
}

void SetRotationTo(GameObject *obj, Vector2D p)
{
	float rot = (float)((180 / M_PI) * atan2(p.y - GetPosition(obj).y, p.x - GetPosition(obj).x));
	if (rot < 0)
	{
			rot = 360 - (-rot);
	}
	rot += 90;

	float increaseRot = GetRotation(obj) - obj->localRotation;
	obj->localRotation = rot - increaseRot;
}

void RotateTo(GameObject *obj, Vector2D p, float speed)
{
	float rot = GetRotation(obj);
	SetRotationTo(obj, p);

	float diff = GetRotation(obj) - rot;

	while(diff < 0) diff = 360 + diff;
	while(diff > 360) diff -= 360;

	if(abs(diff) < speed) return;

	if(diff < 180) obj->localRotation = rot+speed;
	else obj->localRotation = rot-speed;
}

NetworkID AssignNetworkID(GameObject *obj)
{
	obj->networkID = nextNetworkID;
	nextNetworkID++;
	return nextNetworkID-1;
}

void updateGameObjects()
{
	Vector2D camPos = GetCameraPosition();

	for(unsigned char layer = 0; layer < layersCount; layer++)
	{
		GameObject *next = objList[layer];

		while(next)
		{
			GameObject *e = next;
			next = e->next;

			Vector2D pos = GetPosition(e);

			if(e->currentAnimation != NULL)
			{
				e->animationCounter += Delay * e->currentAnimation->speed;
				bool ended = false;
				while(e->animationCounter >= e->currentAnimation->texturesCount)
				{
					e->animationCounter -= e->currentAnimation->texturesCount;
					ended = true;
				}
				if(ended && e->onAnimationEnd != NULL) (*e->onAnimationEnd)(e, e->currentAnimation);
				if(e->currentAnimation) e->currentTexture = e->currentAnimation->textures[(int)(e->animationCounter)];
			}

			renderGameObject(e, pos, camPos);

			if(e->loopCall != NULL)
				(*e->loopCall)(e);	
		}

		if(layer == lightLayer)
			renderLightTint();
	}
}

void KillAllGameObjects()
{
	killingAllGameObjects = 1;

	for(unsigned char layer = 0; layer < layersCount; layer++)
		while(objList[layer] != NULL)
			KillGameObject(objList[layer]);

	killingAllGameObjects = 0;
}

void Play(GameObject *obj, Animation *anim, SDL_RendererFlip flip)
{
	obj->currentAnimation = anim;
	obj->animationCounter = 0;
	obj->flip = flip;
}

void StopPlaying(GameObject *obj)
{
	obj->currentAnimation = NULL;
	obj->flip = SDL_FLIP_NONE;
	obj->currentTexture = obj->defaultTexture;
}
