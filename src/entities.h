#ifndef KO_ENTITIES_H
#define KO_ENTITIES_H

#include "types.h"

#define LAYER_MAX 64

#define NO_NETWORK_ID 0

struct EntitySpawnSettings
{
  float             x;
  float             y;
  unsigned int      width;
  unsigned int      height;
  Texture          *texture;
  unsigned int      layer;
};

Entity *SpawnEntity(const struct EntitySpawnSettings *s);
void KillEntity(Entity *entity);
void KillAllEntities();

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

#endif
