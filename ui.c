#include "ui.h"

Entity *CreateUIObject(int x, int y, unsigned short width, unsigned short height, Texture *texture, unsigned short maxWidth, unsigned char layer, Font *font, SDL_Color color)
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
	ent->ui->maxWidth = maxWidth;
	ent->ui->color = color;

	return ent;
}

void SetUIText(Entity *ent, char *text)
{
	if(ent->ui->textTexture) SDL_DestroyTexture(ent->ui->textTexture);

	SDL_Surface *surf = TTF_RenderText_Blended_Wrapped(
			ent->ui->font,
			text,
			ent->ui->color,
			ent->ui->maxWidth	
		);

	App *app = getAppInfo();

	ent->ui->textTexture = SDL_CreateTextureFromSurface(app->renderer, surf);
	ent->ui->textTextureWidth = surf->w;
	ent->ui->textTextureHeight = surf->h;
	SDL_FreeSurface(surf);
}
