#include "KOE.h"
#include "console.h"
#include "debug.h"
#include "gameObjects.h"
#include "log.h"
#include "ui.h"

extern Resolution windowResolution;

extern SDL_Renderer *renderer;

extern SDL_Texture *lightTintTex;
extern SDL_Color    lightTintColor;

extern void     inheritPosition(GameObject *ent, Vector2D *posPtr);
extern Vector2D getNonRotatedPosition(GameObject *ent);

extern struct ConsoleLine consoleLines[CONSOLE_LINES_MAX];

void renderConsole()
{
  const SDL_Rect bgRect = {.x = windowResolution.width / 2 - 400 - CONSOLE_PADDING,
                           .y = windowResolution.height / 2 - 300 - CONSOLE_PADDING,
                           .w = 800 + CONSOLE_PADDING * 2,
                           .h = 600 + CONSOLE_PADDING * 2};

  SDL_SetRenderDrawColor(renderer, CONSOLE_BG);
  SDL_RenderFillRect(renderer, &bgRect);

  int y = bgRect.y + CONSOLE_PADDING + bgRect.h - CONSOLE_PADDING * 2;

  for (int i = 0; i < CONSOLE_LINES_MAX; i++)
  {
    int w, h;
    SDL_QueryTexture(consoleLines[i].tex, NULL, NULL, &w, &h);
    y -= h + 4;
    if (y < bgRect.y + CONSOLE_PADDING) return;

    SDL_Rect dest = {.x = bgRect.x + CONSOLE_PADDING, .y = y, .w = w, .h = h};

    SDL_RenderCopy(renderer, consoleLines[i].tex, NULL, &dest);
  }
}

void renderCollider(GameObject *e, Vector2D camPos)
{
  if (e->collider.verticesCount == 0) return;

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  SDL_Point vertices[e->collider.verticesCount];

  for (int i = 0; i < e->collider.verticesCount; i++)
  {
    Vector2D p                   = e->collider.vertices[i];
    Vector2D nonRotatedParentPos = getNonRotatedPosition(e);

    p.x += nonRotatedParentPos.x;
    p.y += nonRotatedParentPos.y;

    inheritPosition(e, &p);
    vertices[i].x = p.x - camPos.x + windowResolution.width / 2;
    vertices[i].y = p.y - camPos.y + windowResolution.height / 2;
  }

  SDL_RenderDrawLines(renderer, vertices, e->collider.verticesCount);
  SDL_RenderDrawLine(renderer, vertices[0].x, vertices[0].y,
                     vertices[e->collider.verticesCount - 1].x,
                     vertices[e->collider.verticesCount - 1].y);
}

void renderUIBorders(GameObject *e)
{
  SDL_SetRenderDrawColor(renderer, e->ui->debugColor.r, e->ui->debugColor.g, e->ui->debugColor.b,
                         255);

  SDL_Rect rect;
  rect.x = GetPosition(e).x - e->width / 2;
  rect.y = GetPosition(e).y - e->height / 2;
  rect.w = e->width;
  rect.h = e->height;
  SDL_RenderDrawRect(renderer, &rect);
}

void renderText(GameObject *ent)
{
  SDL_Rect dest;

  dest.h = ent->height;
  dest.w = ent->width;
  dest.x = GetPosition(ent).x - dest.w / 2;
  dest.y = GetPosition(ent).y - dest.h / 2;

  SDL_RenderCopy(renderer, ent->ui->textTexture, NULL, &dest);
}

void renderGameObject(GameObject *e, Vector2D pos, Vector2D camPos)
{
  SDL_Rect dest;

  if (e->ui)
  {
    dest.x = pos.x - e->width / 2;
    dest.y = pos.y - e->height / 2;
  }
  else
  {
    dest.x = pos.x - e->width / 2 - camPos.x + windowResolution.width / 2;
    dest.y = pos.y - e->height / 2 - camPos.y + windowResolution.height / 2;
  }
  dest.w = e->width;
  dest.h = e->height;

  if ((dest.x < (int)windowResolution.width) && (dest.x + dest.w > 0) &&
      (dest.y < (int)windowResolution.height) && (dest.y + dest.h > 0))
  {
    if ((DebugFlags & DEBUG_FLAG_SHOW_COLLIDERS) == DEBUG_FLAG_SHOW_COLLIDERS)
      renderCollider(e, camPos);

    SDL_RenderCopyEx(renderer, e->currentTexture, NULL, &dest, GetRotation(e), NULL, e->flip);
    if (e->ui)
    {
      renderText(e);
      if ((DebugFlags & DEBUG_FLAG_SHOW_UI_BORDERS) == DEBUG_FLAG_SHOW_UI_BORDERS)
        renderUIBorders(e);
    }
  }

  if (e->light)
  {
    SDL_Rect lightDest;

    if (e->ui)
    {
      lightDest.x = pos.x - e->light->width / 2;
      lightDest.y = pos.y - e->light->height / 2;
    }
    else
    {
      lightDest.x = pos.x - e->light->width / 2 - camPos.x + windowResolution.width / 2;
      lightDest.y = pos.y - e->light->height / 2 - camPos.y + windowResolution.height / 2;
    }
    lightDest.w = e->light->width;
    lightDest.h = e->light->height;

    if ((lightDest.x < (int)windowResolution.width) && (lightDest.x + lightDest.w > 0) &&
        (lightDest.y < (int)windowResolution.height) && (lightDest.y + lightDest.h > 0))
    {
      SDL_SetRenderTarget(renderer, lightTintTex);
      SDL_RenderCopyEx(renderer, e->light->tex, NULL, &lightDest, GetRotation(e), NULL, e->flip);
      SDL_SetRenderTarget(renderer, NULL);
    }
  }
}

void renderLightTint()
{
  SDL_Rect dest = {.x = 0, .y = 0, .w = windowResolution.width, .h = windowResolution.height};

  SDL_RenderCopy(renderer, lightTintTex, NULL, &dest);
  SDL_SetRenderDrawColor(renderer, lightTintColor.r, lightTintColor.g, lightTintColor.b,
                         lightTintColor.a);
  SDL_SetRenderTarget(renderer, lightTintTex);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, NULL);
}

void renderTextLineOnTextTexture(unsigned int *y, GameObject *ent, const char **ptr, int countOnly)
{
  unsigned short totalWidth = 0;

  const char *ptr2 = *ptr;

  while (*ptr2 != '\n' && *ptr2 != '\0')
  {
    totalWidth += ent->ui->font->glyphWidths[(*ptr2) - FONT_GLYPH_MIN];
    if (totalWidth > ent->width)
    {
      totalWidth = ent->width;
      break;
    }
    ptr2++;
  }

  SDL_Rect dest;
  dest.y = *y;
  dest.h = ent->ui->font->height;

  switch ((ent->ui->flags & UI_PARAMETERS_FLAGS_HORIZONTAL_ALIGN_MASK))
  {
    case TEXT_ALIGN_H_LEFT:
      dest.x = 0;
      break;
    case TEXT_ALIGN_H_RIGHT:
      dest.x = ent->width - totalWidth;
      break;
    case TEXT_ALIGN_H_CENTER:
      dest.x = ent->width / 2 - totalWidth / 2;
      break;
  }

  while (**ptr != '\n' && **ptr != '\0')
  {
    unsigned short width = ent->ui->font->glyphWidths[(**ptr) - FONT_GLYPH_MIN];
    if ((ent->ui->flags & UI_PARAMETERS_FLAGS_TEXTWRAP_MASK) == TEXT_WRAP_WORD_BREAK &&
        dest.x + width > ent->width)
    {
      (*ptr)--;
      return;
    }

    if (**ptr < FONT_GLYPH_MIN || **ptr >= FONT_GLYPH_MAX)
    {
      Log("WARNING, incorrect char: %c (%d)\n", **ptr, **ptr);
      (*ptr)++;
      continue;
    }
    dest.w = ent->ui->font->glyphWidths[(**ptr) - FONT_GLYPH_MIN];
    if (!countOnly)
      SDL_RenderCopy(renderer, ent->ui->font->glyphs[(**ptr) - FONT_GLYPH_MIN], NULL, &dest);
    dest.x += width;
    (*ptr)++;

    if ((*((*ptr) - 1)) == ' ' &&
        (ent->ui->flags & UI_PARAMETERS_FLAGS_TEXTWRAP_MASK) == TEXT_WRAP_NORMAL)
    {
      ptr2                   = *ptr;
      unsigned int wordWidth = 0;
      while (*ptr2 != ' ' && *ptr2 != '\0' && *ptr2 != '\n')
      {
        wordWidth += ent->ui->font->glyphWidths[(*ptr2) - FONT_GLYPH_MIN];
        ptr2++;
      }
      if (dest.x + wordWidth > ent->width)
      {
        (*ptr)--;
        return;
      }
    }
  }
}

void renderTextOnTextTexture(GameObject *ent, const char *text)
{
  SDL_SetRenderTarget(renderer, ent->ui->textTexture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);

  if (text == NULL || strlen(text) < 1)
  {
    SDL_SetRenderTarget(renderer, NULL);
    return;
  }

  unsigned int totalHeight = ent->ui->font->height;
  const char  *ptr         = text;

  unsigned int y = 0;

  while ((*ptr) != '\0')
  {
    renderTextLineOnTextTexture(&y, ent, &ptr, 1);
    if (*ptr == '\0') break;
    y += ent->ui->font->height;
    ptr++;
    totalHeight += ent->ui->font->height;
  }

  y   = 0;
  ptr = text;

  switch (ent->ui->flags & UI_PARAMETERS_FLAGS_VERTICAL_ALIGN_MASK)
  {
    case TEXT_ALIGN_V_TOP:
      y = 0;
      break;
    case TEXT_ALIGN_V_BOTTOM:
      y = ent->height - totalHeight;
      break;
    case TEXT_ALIGN_V_CENTER:
      y = ent->height / 2 - totalHeight / 2;
      break;
  }

  while ((*ptr) != '\0')
  {
    renderTextLineOnTextTexture(&y, ent, &ptr, 0);
    if (*ptr == '\0') break;
    y += ent->ui->font->height;
    ptr++;
  }

  SDL_SetRenderTarget(renderer, NULL);
}
