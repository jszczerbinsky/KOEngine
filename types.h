#ifndef KO_TYPES_H
#define KO_TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>

#define FONT_GLYPH_MIN 20
#define FONT_GLYPH_MAX 128

typedef SDL_Texture Texture;
typedef SDL_Color   Color;
typedef SDL_Rect    Rect;
typedef Mix_Music   Music;

typedef struct {
  Texture *glyphs             [FONT_GLYPH_MAX-FONT_GLYPH_MIN];
  unsigned int glyphWidths  [FONT_GLYPH_MAX-FONT_GLYPH_MIN];
  unsigned int height;
} Font;

typedef struct {
  int channel;
  Mix_Chunk *chunk;
} SoundEffect;

typedef struct {
  float x;
  float y;
} Vector2D;

typedef struct {
  Vector2D *vertices;
  unsigned int verticesCount;
} Collider;

typedef struct {
  unsigned int texturesCount;
  SDL_Texture **textures;
  float speed;
} Animation;

typedef enum
{
  MouseLeft   = SDL_BUTTON_LEFT,
  MouseMiddle = SDL_BUTTON_MIDDLE,
  MouseRight  = SDL_BUTTON_RIGHT,
  MouseX1     = SDL_BUTTON_X1,
  MouseX2     = SDL_BUTTON_X2
} MouseButton;

typedef struct
{
  Texture *textTexture;
  SDL_Color debugColor;
  Font *font;
  unsigned int flags;
} UIParameters;

typedef unsigned int NetworkID;

typedef struct Entity_{
  NetworkID networkID;

  unsigned int layer;

  UIParameters *ui;

  struct Entity_ *next;
  struct Entity_ *prev;

  struct Entity_ *parent;

  Vector2D localPosition;
  float localRotation;

  unsigned int width;
  unsigned int height;
  
  SDL_Texture *actualTexture;
  SDL_Texture *defaultTexture;

  Animation *actualAnimation;
  float animationCounter;

  void (*onAnimationEnd)(struct Entity_ *ent, Animation* anim);

  SDL_RendererFlip flip;

  Collider collider;  
  int colliderMode;
  void (*onCollision)(struct Entity_ *ent, struct Entity_ *ent2);

  void (*loopCall)(struct Entity_ *ent);

  void (*freeExtension)(void *extension);

  unsigned int extensionType;
  void *extension;
} Entity;

typedef struct NetworkClient_
{
  struct sockaddr address;
  time_t lastDatagram;

  void *extension;

  struct NetworkClient_ *next;
  struct NetworkClient_ *prev;
} NetworkClient;

#endif
