#include "button.h"
#include "../mouse.h"

struct ButtonInstance
{
	Texture *texHover;
	Texture *texClick;
	void *clickParam;
	void (*clickAction)(GameObject *btn, void *p);
};

void ButtonLoopCall(GameObject *e)
{
	struct ButtonInstance *btnHook = (struct ButtonInstance*)e->extension;

	if(MouseOver(e))
	{
		if(ButtonDown(MouseLeft))
			e->actualTexture = btnHook->texClick;
		else if(ButtonUp(MouseLeft) && btnHook->clickAction)
			(*btnHook->clickAction)(e, btnHook->clickParam);
		else
			e->actualTexture = btnHook->texHover;
	}
	else 
		e->actualTexture = e->defaultTexture;
}

GameObject *CreateButton(const struct GameObjectSpawnSettings *ess, const struct UISpawnSettings *uis, const struct ButtonSettings *s)
{
	GameObject *e = CreateUIObject(ess, uis);
	e->extension = malloc(sizeof(struct ButtonInstance));
	e->extensionType = EXT_UICONTROLS_BUTTON;

	struct ButtonInstance *btnHook = (struct ButtonInstance*) e->extension;

	btnHook->texHover = s->texHover;
	btnHook->texClick = s->texClick;
	btnHook->clickAction = s->clickAction;
	btnHook->clickParam = s->clickParam;

	e->loopCall = &ButtonLoopCall;

	return e;
}
