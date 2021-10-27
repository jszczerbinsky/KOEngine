#include "ui.h"

Entity *CreateUIObject(int x, int y, unsigned short width, unsigned short height, SDL_Texture *texture, unsigned char layer, Font *font)
{
	Entity *ent = SpawnEntity(x,y,width,height,GenerateNullCollider(),texture, layer);

	ent->ui = calloc(1, sizeof(UIParameters));
	ent->ui->font = font;

	return ent;
}
