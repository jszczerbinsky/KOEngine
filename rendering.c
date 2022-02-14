#include "entities.h"
#include "debug.h"

void renderCollider(App *app,Entity *e, Vector2D camPos)
{
	if(e->collider.verticesCount == 0) return;

	SDL_SetRenderDrawColor(app->renderer, 255, 255, 255, 255);

	SDL_Point vertices[e->collider.verticesCount];

	for(int i = 0; i < e->collider.verticesCount; i++)
	{
		Vector2D p = e->collider.vertices[i];
		Vector2D nonRotatedParentPos = getNonRotatedPosition(e);

		p.x += nonRotatedParentPos.x;
		p.y += nonRotatedParentPos.y;

		inheritPosition(e, &p);
		vertices[i].x = p.x - camPos.x + app->resX/2;
		vertices[i].y = p.y - camPos.y + app->resY/2;
	}

	SDL_RenderDrawLines(app->renderer, vertices, e->collider.verticesCount);
	SDL_RenderDrawLine(app->renderer, 
			vertices[0].x, vertices[0].y,
			vertices[e->collider.verticesCount-1].x,
			vertices[e->collider.verticesCount-1].y
	);
}

void renderUIBorders(App *app,Entity *e)
{
	SDL_SetRenderDrawColor(
		app->renderer, 
		e->ui->debugColor.r,
		e->ui->debugColor.g,
		e->ui->debugColor.b,
		255
	);

	SDL_Rect rect;
	rect.x = GetPosition(e).x - e->width/2;
	rect.y = GetPosition(e).y - e->height/2;
	rect.w = e->width;
	rect.h = e->height;
	SDL_RenderDrawRect(app->renderer, &rect);
}


void renderText(Entity *ent, App *app)
{
	SDL_Rect dest;

	dest.h = ent->height;
	dest.w = ent->width;
	dest.x = GetPosition(ent).x - dest.w/2;
	dest.y = GetPosition(ent).y - dest.h/2;

	SDL_RenderCopy(app->renderer, ent->ui->textTexture, NULL, &dest);
}

void renderEntity(Entity *e, Vector2D pos, Vector2D camPos, App *app)
{
	SDL_Rect dest;

	if(e->ui)
	{
		dest.x = pos.x - e->width/2;
		dest.y = pos.y - e->height/2;
	}else
	{
		dest.x = pos.x - e->width/2 - camPos.x + app->resX/2;
		dest.y = pos.y - e->height/2 - camPos.y + app->resY/2;
	}
	dest.w = e->width;
	dest.h = e->height;

	if(
			(dest.x < app->resX) &&
			(dest.x + dest.w > 0) &&
			(dest.y < app->resY) &&
			(dest.x + dest.h > 0)
	)
	{

		if((DebugFlags & DEBUG_FLAG_SHOW_COLLIDERS) == DEBUG_FLAG_SHOW_COLLIDERS)
			renderCollider(app, e, camPos);

		SDL_RenderCopyEx(app->renderer, e->actualTexture, NULL, &dest, GetRotation(e), NULL, e->flip);
		if(e->ui)
		{
			renderText(e, app);
			if((DebugFlags & DEBUG_FLAG_SHOW_UI_BORDERS) == DEBUG_FLAG_SHOW_UI_BORDERS)
				renderUIBorders(app, e);
		}
	}
}
