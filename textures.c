#include "textures.h"
#include "log.h"

SDL_Texture * LoadTexture(const char * path)
{
	SDL_Surface * surf = SDL_LoadBMP(path);

	if(surf == NULL)
		Log("WARNING, can't load texture surface: %s", SDL_GetError());

	App * app = getAppInfo();

	SDL_Texture * texture = SDL_CreateTextureFromSurface(app->renderer, surf);

	if(texture == NULL)
		Log("WARNING, can't initialize texture: %s", SDL_GetError());

	SDL_FreeSurface(surf);

	return texture;
}

SDL_Texture * CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	App *app = getAppInfo();

	SDL_Texture * texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1,1);

	SDL_SetRenderTarget(app->renderer, texture);
	SDL_SetRenderDrawColor(app->renderer, r, g, b, a);
	SDL_RenderClear(app->renderer);
	SDL_SetRenderTarget(app->renderer, NULL);

	return texture;
}
