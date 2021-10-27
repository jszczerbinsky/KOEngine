#include "mouse.h"

#define BUTTONS_COUNT 5

bool buttonsPressed[BUTTONS_COUNT];
bool buttonsDown[BUTTONS_COUNT];
bool buttonsUp[BUTTONS_COUNT];

void resetButtons()
{
	for(unsigned short i = 0; i < BUTTONS_COUNT; i++)
	{
		 buttonsPressed[i] = 0;	
		 buttonsUp[i] = 0;	
	}
}
void setButtonUp(MouseButton btn)
{
	buttonsUp[btn-1] = 1;
	buttonsDown[btn-1] = 0;
}
void setButtonDown(MouseButton btn)
{
	buttonsDown[btn-1] = 1;
	buttonsPressed[btn-1] = 1;
}

bool ButtonDown(MouseButton btn)
{
	return buttonsDown[btn-1];
}
bool ButtonUp(MouseButton btn)
{
	return buttonsUp[btn-1];
}
bool ButtonPress(MouseButton btn)
{
	return buttonsPressed[btn-1];
}


Vector2D GetMousePosition()
{
	int x, y;

	SDL_PumpEvents();
	SDL_GetMouseState(&x, &y);

	Vector2D pos;

	pos.x = x;
	pos.y = y;

	return pos;
}

Vector2D GetMouseWorldPosition()
{
	Vector2D mousePos = GetMousePosition();

	mousePos.x += GetCameraPosition().x;
	mousePos.y += GetCameraPosition().y;

	App * app = getAppInfo();
	mousePos.x -= app->resX/2;
	mousePos.y -= app->resY/2;

	return mousePos;
}

bool MouseOver(Entity *ent)
{
	Vector2D mousePos;

	if(ent->ui)
		mousePos = GetMousePosition();
	else
		mousePos = GetMouseWorldPosition();

	Vector2D entPos = GetPosition(ent);

	return
		mousePos.x >= entPos.x - ent->width/2 &&
		mousePos.x < entPos.x + ent->width/2 &&
		mousePos.y >= entPos.y - ent->height/2 &&
		mousePos.y < entPos.y + ent->height/2;
}

bool MouseClick(Entity *ent, MouseButton btn)
{
	return MouseOver(ent) && ButtonPress(btn);
}
