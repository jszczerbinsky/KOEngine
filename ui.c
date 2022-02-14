#include "ui.h"
#include "rendering.h"

Font *LoadFont(char *path, int size, SDL_Color color)
{
	TTF_Font *ttf = TTF_OpenFont(path, size);
	Font *font = malloc(sizeof(Font));
	font->height = TTF_FontHeight(ttf);

	App *app = getAppInfo();

	for(int i = FONT_GLYPH_MIN; i < FONT_GLYPH_MAX; ++i)
	{
		SDL_Surface *surf =	TTF_RenderGlyph_Blended(ttf, i, color);
		font->glyphs[i-FONT_GLYPH_MIN] = SDL_CreateTextureFromSurface(app->renderer, surf);
		font->glyphWidths[i-FONT_GLYPH_MIN] = surf->w;
		SDL_FreeSurface(surf);
	}

	TTF_CloseFont(ttf);

	return font;
}

void CloseFont(Font *font)
{
	for(int i = FONT_GLYPH_MIN; i < FONT_GLYPH_MAX; ++i)
		SDL_DestroyTexture(font->glyphs[i]);
	free(font);
}

Entity *CreateUIObject(int x, int y, unsigned short width, unsigned short height, Texture *texture, unsigned short maxWidth, unsigned char layer, Font *font, SDL_Color color)
{
	Entity *ent = SpawnEntity(
		x,y,
		width,height,
		GenerateNullCollider(),
		texture,
		layer
	);

	App *app = getAppInfo();

	ent->ui = calloc(1, sizeof(UIParameters));

	ent->ui->debugColor.r = rand()%256;
	ent->ui->debugColor.g = rand()%256;
	ent->ui->debugColor.b = rand()%256;

	ent->ui->font = font;
	ent->ui->textTexture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, ent->width, ent->height);

	SDL_SetTextureBlendMode(ent->ui->textTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(app->renderer, ent->ui->textTexture);
	SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 0);
	SDL_RenderClear(app->renderer);
	SDL_SetRenderTarget(app->renderer, NULL);

	ent->ui->color = color;

	return ent;
}

void SetUIText(Entity *ent, char *text)
{
	renderTextOnTextTexture(ent, getAppInfo(), text);
}
