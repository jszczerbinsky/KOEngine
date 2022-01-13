#ifndef KO_ENTITIES_H
#define KO_ENTITIES_H

#include <pthread.h>
#include "types.h"
#include "camera.h"
#include "KOEngine.h"

#define LAYER_MAX 64

#define NO_NETWORK_ID 0

#define LOCK_ENTITIES()   pthread_mutex_lock(&entitiesLockHook)
#define UNLOCK_ENTITIES() pthread_mutex_unlock(&entitiesLockHook)

extern pthread_mutex_t entitiesLockHook;

Entity *SpawnEntity(
  float x,
  float y, 
  unsigned short width, 
  unsigned short height, 
  Collider collider, 
  SDL_Texture *texture, 
  unsigned char layer
);
void KillEntity(Entity *entity);

Vector2D GetPosition(Entity *ent);
float    GetRotation(Entity *ent);

void Move         (Entity *ent, float x, float y);
void MoveLocal    (Entity *ent, float x, float y);
void MoveTo       (Entity *ent, Vector2D dest, float speed);
void RotateTo     (Entity *ent, Vector2D p, float speed);
void SetRotationTo(Entity *ent, Vector2D p);

void Play       (Entity *ent, Animation *anim, SDL_RendererFlip flip);
void StopPlaying(Entity *ent);

bool CheckAnyCollision(Entity *ent);

NetworkID AssignNetworkID(Entity *ent);

void initEntities();
void freeEntities();

void addEntity(Entity *ent, unsigned char layer);

void updateEntities();
void updateEntities(App *app);

Vector2D getNonRotatedPosition(Entity *ent);
void inheritPosition(Entity *ent, Vector2D *posPtr);

#endif
