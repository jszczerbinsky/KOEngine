#ifndef KO_RENDERING_H
#define KO_RENDERING_H

#include "KOEngine.h"

void renderCollider(App *app,Entity *e, Vector2D camPos);
void renderText(Vector2D pos, UIParameters *ui, App *app);
void renderEntity(Entity *e, Vector2D pos, Vector2D camPos, App *app);

#endif
