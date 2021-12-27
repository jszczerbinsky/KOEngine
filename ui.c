#include "ui.h"

Entity *CreateUIObject(int x, int y, unsigned short width, unsigned short height, Texture *texture, unsigned short linesOffset, unsigned char layer, Font *font, unsigned char fontSize, SDL_Color color)
{
	Entity *ent = SpawnEntity(
		x,y,
		width,height,
		GenerateNullCollider(),
		texture,
		layer
	);

	ent->ui = calloc(1, sizeof(UIParameters));
	ent->ui->font = font;
	ent->ui->fontSize = fontSize;
	ent->ui->linesOffset = linesOffset;
	ent->ui->color = color;

	return ent;
}

void SetUIText(Entity *ent, char *text)
{
	if(ent->ui->textTexture) SDL_DestroyTexture(ent->ui->textTexture);

	char *lines = malloc(strlen(text)+1);
	strcpy(lines, text);

	int linesCount = 1;

	char *ptr = lines;

	while(*ptr)
	{
		if(*ptr == '\n')
		{
			*ptr = '\0';
			linesCount++;
		}
		ptr++;
	}

	ptr = lines;

	App *app = getAppInfo();

	Texture *linesTextures[linesCount];
	float ratios[linesCount];
	float biggestRatio = 0;

	for(int i = 0; i < linesCount; i++)
	{
		SDL_Surface *surf = TTF_RenderText_Solid(
			ent->ui->font,
			ptr,
			ent->ui->color
		);

		if(surf->w/surf->h > biggestRatio) biggestRatio = surf->w/surf->h;

		ratios[i] = surf->w/surf->h;
		linesTextures[i] = SDL_CreateTextureFromSurface(app->renderer, surf);
		SDL_FreeSurface(surf);

		do
		{
			ptr++;
		} while(*(ptr-1) != '\0');
	}

	SDL_SetRenderDrawColor(app->renderer, 0,0,0,0);
	ent->ui->textTexture = SDL_CreateTexture(
			app->renderer,
			SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_TARGET,
			ent->ui->fontSize * biggestRatio,
			linesCount * (ent->ui->fontSize + ent->ui->linesOffset)
	);	
	SDL_SetTextureBlendMode(ent->ui->textTexture, SDL_BLENDMODE_BLEND);
	ent->ui->textTextureHeight = (ent->ui->fontSize +ent->ui->linesOffset)* linesCount;
	ent->ui->textTextureWidth = ent->ui->fontSize *biggestRatio;

	SDL_SetRenderTarget(app->renderer, ent->ui->textTexture);
	SDL_RenderClear(app->renderer);

	for(int i = 0; i < linesCount; i++) 
	{
		SDL_Rect dest;
		dest.x = 0;
		dest.y = i*(ent->ui->fontSize + ent->ui->linesOffset);
		dest.h = ent->ui->fontSize;
		dest.w = dest.h * ratios[i];
		SDL_RenderCopy(
			app->renderer,
			linesTextures[i],
			NULL,
			&dest
		);
		SDL_DestroyTexture(linesTextures[i]);
	}
	SDL_SetRenderTarget(app->renderer, NULL);
	free(lines);
}
