#include "KOE.h"

extern void renderTextOnTextTexture(GameObject *obj, const char *text);

extern SDL_Renderer *renderer;

Font *LoadFont(char *path, int size, SDL_Color color)
{
  TTF_Font *ttf  = TTF_OpenFont(path, size);
  Font     *font = malloc(sizeof(Font));
  font->height   = TTF_FontHeight(ttf);

  for (int i = FONT_GLYPH_MIN; i < FONT_GLYPH_MAX; ++i)
  {
    SDL_Surface *surf                     = TTF_RenderGlyph_Blended(ttf, i, color);
    font->glyphs[i - FONT_GLYPH_MIN]      = SDL_CreateTextureFromSurface(renderer, surf);
    font->glyphWidths[i - FONT_GLYPH_MIN] = surf->w;
    SDL_FreeSurface(surf);
  }

  TTF_CloseFont(ttf);

  return font;
}

void CloseFont(Font *font)
{
  for (int i = FONT_GLYPH_MIN; i < FONT_GLYPH_MAX; ++i)
    SDL_DestroyTexture(font->glyphs[i - FONT_GLYPH_MIN]);
  free(font);
}

GameObject *SpawnUiObject(float x, float y, unsigned int w, unsigned int h, Texture *tex,
                          unsigned int layer, unsigned int uiFlags, Font *font)
{
  GameObject *obj = SpawnGameObject(x, y, w, h, tex, layer);

  obj->ui = calloc(1, sizeof(UIParameters));

  obj->ui->debugColor.r = rand() % 256;
  obj->ui->debugColor.g = rand() % 256;
  obj->ui->debugColor.b = rand() % 256;

  obj->ui->flags = uiFlags;

  obj->ui->font = font;

  if (font)
  {
    if (!(obj->ui->textTexture =
              SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,
                                obj->width, obj->height)))
      Log("WARNING, can't initialize UI text texture: %s", SDL_GetError());
  }

  SDL_SetTextureBlendMode(obj->ui->textTexture, SDL_BLENDMODE_BLEND);
  SDL_SetRenderTarget(renderer, obj->ui->textTexture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, NULL);

  return obj;
}

void SetUiText(GameObject *obj, const char *text)
{
  if (!obj->ui || !obj->ui->textTexture)
  {
    Log("WARNING, trying to print text on UI object, that has no font assigned");
    return;
  }
  renderTextOnTextTexture(obj, text);
}
