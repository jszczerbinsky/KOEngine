#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include "animations.h"
#include "textures.h"

Animation * CreateAnimation(float speed, int count, ...)
{
	va_list ap;
	va_start(ap, count);

	Animation *anim = malloc(sizeof(Animation));

	anim->texturesCount = count;
	anim->speed = speed;
	anim->textures = malloc(count * sizeof(SDL_Texture*));

	for(unsigned char i = 0; i < count; i++)
	{
		anim->textures[i] = va_arg(ap, SDL_Texture*);
	}

	va_end(ap);

	return anim;
}

Animation * LoadAnimation(float speed, int count, const char * path)
{
	Animation *anim = malloc(sizeof(Animation));

	anim->texturesCount = count;
	anim->speed = speed;
	anim->textures = malloc(count * sizeof(SDL_Texture*));

	for(int i = 0; i < count; i++)
	{
		char * framePath = malloc(strlen(path) + 10);	
		snprintf(framePath, strlen(path) + 10, "%s/%d.bmp", path, i+1);

		anim->textures[i] = LoadTexture(framePath);

		free(framePath);
	}	

	return anim;
}

void FreeAnimation(Animation *anim)
{
	free(anim->textures);
	free(anim);
}

