#ifndef KO_ENTITIES_H
#define KO_ENTITIES_H

#include "types.h"

#define LAYER_MAX 64

#define NO_NETWORK_ID 0

struct GameObjectSpawnSettings
{
  float             x;
  float             y;
  unsigned int      width;
  unsigned int      height;
  Texture          *texture;
  unsigned int      layer;
};

GameObject *SpawnGameObject(const struct GameObjectSpawnSettings *s);
void KillGameObject(GameObject *objity);
void KillAllGameObjects();

Vector2D GetPosition(GameObject *obj);
float    GetRotation(GameObject *obj);

void Move         (GameObject *obj, float x, float y);
void MoveLocal    (GameObject *obj, float x, float y);
void MoveTo       (GameObject *obj, Vector2D dest, float speed);
void RotateTo     (GameObject *obj, Vector2D p, float speed);
void SetRotationTo(GameObject *obj, Vector2D p);

void Play       (GameObject *obj, Animation *anim, SDL_RendererFlip flip);
void StopPlaying(GameObject *obj);

bool CheckAnyCollision(GameObject *obj);

NetworkID AssignNetworkID(GameObject *obj);

#endif
