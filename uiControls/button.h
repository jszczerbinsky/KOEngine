#ifndef BUTTON_H
#define BUTTON_H

#include "../types.h"
#include "../ui.h"
#include "extensions.h"

struct ButtonSettings
{
	Texture *texHover;
	Texture *texClick;
	void *clickParam;
	void (*clickAction)(Entity *btn, void *p);
};

Entity *CreateButton(const struct EntitySpawnSettings *ess, const struct UISpawnSettings *uis, const struct ButtonSettings *s);

#endif
