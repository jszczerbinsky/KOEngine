#include "KOE.h"
#include "light.h"

extern SDL_Renderer *renderer;

SDL_Texture *lightTintTex = NULL;
SDL_Color lightTintColor;

void freeLight()
{
	if(lightTintTex) SDL_DestroyTexture(lightTintTex);
	lightTintTex = NULL;
}

void reloadLightResolution()
{
	freeLight();

	lightTintTex = SDL_CreateTexture(
		renderer, 
		SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, 
		WindowResolution.width, 
		WindowResolution.height
	);

	SDL_SetTextureBlendMode(lightTintTex, SDL_BLENDMODE_MOD);
}

void RemoveLightSource(GameObject *obj)
{
	if(obj->light)
		free(obj->light);
	obj->light = NULL;
}

void AssignLightSource(GameObject *obj, struct LightSettings *s)
{
	RemoveLightSource(obj);
	obj->light = malloc(sizeof(LightParameters));
	obj->light->width = s->width;
	obj->light->height = s->height;
	obj->light->tex = s->texture;
}

void SetLightTint(Color color)
{
	lightTintColor = color;
}
