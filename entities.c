#include <stdarg.h>

#include "entities.h"
#include "collisions.h"
#include "debug.h"

Entity *entList[LAYER_MAX];

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

	if(CheckAnyCollision(ent)) return;

	ent->localPosition.x += x;
	ent->localPosition.y += y;
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

void addEntity(Entity *ent, unsigned char layer)
{
	ent->next = entList[layer];

	if(entList[layer] != NULL)
		entList[layer]->prev = ent;

	entList[layer] = ent;

}

Entity *SpawnEntity(float x, float y, unsigned short width, unsigned short height, Collider collider, SDL_Texture *texture, unsigned char layer)
{
	Entity *ent = malloc(sizeof(Entity));
	ent->layer = layer;
	ent->ui = NULL;
	ent->parent = NULL;
	ent->localPosition.x = x;
	ent->localPosition.y = y;
	ent->localRotation = 0;
	ent->width = width;
	ent->height = height;
	ent->collider = collider;
	ent->defaultTexture = texture;
	ent->actualTexture = texture;
	ent->animationCounter = 0;
	ent->actualAnimation = NULL;
	ent->onAnimationEnd = NULL;
	ent->colliderMode = COLLIDER_MODE_NORMAL;
	ent->onCollision = NULL;
	ent->flip = SDL_FLIP_NONE;
	ent->loopCall = NULL;
	ent->extensionType = 0;
	ent->extension = NULL;

	ent->prev = NULL;
	ent->next = NULL;

	addEntity(ent, layer);	

	return entList[layer];
}

void KillEntity(Entity *entity)
{
	if(entity->ui != NULL) free(entity->ui);

	if(entity->prev != NULL)
		entity->prev->next = entity->next;
	else
		entList[entity->layer] = entity->next;
	if(entity->next != NULL)
		entity->next->prev = entity->prev;

	if(entity->collider.vertices != NULL)
		free(entity->collider.vertices);

	if(entity->extension)
		free(entity->extension);

	free(entity);
}

void RotateTo(Entity *ent, Vector2D p)
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

void renderText(Vector2D pos, unsigned short height, UIParameters *ui, App *app)
{
	if(ui->text == NULL) return;

	char * buff = calloc(strlen(ui->text) + 3, sizeof(char));
	sprintf(buff, " %s ", ui->text);
	SDL_Surface* text_surf = TTF_RenderText_Solid(ui->font, buff, ui->color);
	free(buff);
	Texture *tex = SDL_CreateTextureFromSurface(app->renderer, text_surf);

	SDL_Rect dest;

	dest.h = height;
	dest.w = height * (text_surf->w/text_surf->h);
	dest.x = pos.x - dest.w/2;
	dest.y = pos.y - dest.h/2;

	SDL_RenderCopy(app->renderer, tex, NULL, &dest);

	SDL_DestroyTexture(tex);
	SDL_FreeSurface(text_surf);
}

void renderCollider(App *app,Entity *e, Vector2D camPos)
{
	if(e->collider.verticesCount == 0) return;

	SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);

	SDL_Point vertices[e->collider.verticesCount];

	for(int i = 0; i < e->collider.verticesCount; i++)
	{
		Vector2D p = e->collider.vertices[i];
		Vector2D nonRotatedParentPos = getNonRotatedPosition(e);

		p.x += nonRotatedParentPos.x;
		p.y += nonRotatedParentPos.y;

		inheritPosition(e, &p);
		vertices[i].x = p.x - camPos.x + app->resX/2;
		vertices[i].y = p.y - camPos.y + app->resY/2;
	}

	SDL_RenderDrawLines(app->renderer, vertices, e->collider.verticesCount);
	SDL_RenderDrawLine(app->renderer, 
			vertices[0].x, vertices[0].y,
			vertices[e->collider.verticesCount-1].x,
			vertices[e->collider.verticesCount-1].y
	);
}

void renderEntities(App *app)
{
	Vector2D camPos = GetCameraPosition();

	for(unsigned char layer = 0; layer < LAYER_MAX; layer++)
	{
		for(Entity *e = entList[layer]; e; e = e->next)
		{
			Vector2D pos = GetPosition(e);

			if(!e->ui &&
					(
						pos.x + e->width/2 < camPos.x - app->resX/2 ||
						pos.x - e->width/2 > camPos.x + app->resX/2 ||
						pos.y + e->height/2 < camPos.y - app->resY/2 ||
						pos.y - e->height/2 > camPos.y + app->resY/2
					)
				) continue;

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

			SDL_Rect dest;

			if(e->ui)
			{
				dest.x = pos.x - e->width/2;
				dest.y = pos.y - e->height/2;
			}else
			{
				dest.x = pos.x - e->width/2 - camPos.x + app->resX/2;
				dest.y = pos.y - e->height/2 - camPos.y + app->resY/2;
			}
			dest.w = e->width;
			dest.h = e->height;

			if((DebugFlags & DEBUG_FLAG_SHOW_COLLIDERS) == DEBUG_FLAG_SHOW_COLLIDERS)
				renderCollider(app, e, camPos);

			SDL_RenderCopyEx(app->renderer, e->actualTexture, NULL, &dest, GetRotation(e), NULL, e->flip);

			if(e->ui) renderText(pos, e->height, e->ui, app);

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
