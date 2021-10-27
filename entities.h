#ifndef KO_ENTITIES_H
#define KO_ENTITIES_H

#include "types.h"
#include "camera.h"
#include "KOEngine.h"

#define LAYER_MAX 15

void initEntities();

Entity *SpawnEntity(float x, float y, unsigned short width, unsigned short height, Collider collider, SDL_Texture *texture, unsigned char layer);
void KillEntity(Entity *entity);
void freeEntities();

void addEntity(Entity *ent, unsigned char layer);
void updateEntities();

void renderEntities(App *app);

bool CheckAnyCollision(Entity *ent);

void Move(Entity *ent, float x, float y);
void MoveTo(Entity *ent, Vector2D dest, float speed);
void RotateTo(Entity *ent, Vector2D p);

Vector2D GetPosition(Entity *ent);
float GetRotation(Entity *ent);

void Play(Entity *ent, Animation *anim, SDL_RendererFlip flip);
void StopPlaying(Entity *ent);

#endif
