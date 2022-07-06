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
	void (*clickAction)(GameObject *btn, void *p);
	void (*rightClickAction)(GameObject *btn, void *p);
};

GameObject *CreateButton(const struct GameObjectSpawnSettings *ess, const struct UISpawnSettings *uis, const struct ButtonSettings *s);

#endif
