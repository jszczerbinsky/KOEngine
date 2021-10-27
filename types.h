#ifndef KO_TYPES_H
#define KO_TYPES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdbool.h>

typedef SDL_Texture Texture;
typedef SDL_Color Color;
typedef TTF_Font Font;

typedef struct {
  float x;
  float y;
} Vector2D;

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  unsigned short resX;
  unsigned short resY;
} App;

typedef struct {
  Vector2D *vertices;
  unsigned short verticesCount;
} Collider;

typedef struct {
  unsigned char texturesCount;
  SDL_Texture **textures;
  float speed;
} Animation;

typedef enum
{
  MouseLeft = SDL_BUTTON_LEFT,
  MouseMiddle = SDL_BUTTON_MIDDLE,
  MouseRight = SDL_BUTTON_RIGHT,
  MouseX1 = SDL_BUTTON_X1,
  MouseX2 = SDL_BUTTON_X2
} MouseButton;

typedef struct
{
  TTF_Font *font;
  char *text;
  SDL_Color color;
} UIParameters;

typedef struct Entity_{
  unsigned char layer;

  UIParameters *ui;

  struct Entity_ *next;
  struct Entity_ *prev;

  struct Entity_ *parent;

  Vector2D localPosition;
  float localRotation;

  unsigned short width;
  unsigned short height;
  
  SDL_Texture *actualTexture;
  SDL_Texture *defaultTexture;

  Animation *actualAnimation;
  float animationCounter;

  void (*onAnimationEnd)(struct Entity_ *ent, Animation* anim);

  SDL_RendererFlip flip;

  Collider collider;  

  void (*loopCall)(struct Entity_ *ent);
} Entity;

#endif
