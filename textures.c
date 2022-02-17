#include "textures.h"
#include "log.h"

extern SDL_Renderer *renderer;

SDL_Texture * LoadTexture(const char * path)
{
	SDL_Surface * surf = SDL_LoadBMP(path);

	if(surf == NULL)
		Log("WARNING, can't load texture surface: %s", SDL_GetError());

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surf);

	if(texture == NULL)
		Log("WARNING, can't initialize texture: %s", SDL_GetError());

	SDL_FreeSurface(surf);

	return texture;
}

SDL_Texture * CreateColorTexture(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1,1);

	SDL_SetRenderTarget(renderer, texture);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);

	return texture;
}
