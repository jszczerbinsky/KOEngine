#include "camera.h"

Vector2D cameraPosition = {.x=0, .y=0};

void SetCameraPosition(float x, float y)
{
	cameraPosition.x = x;
	cameraPosition.y = y;
}

void MoveCamera(float x, float y)
{
	cameraPosition.x += x;
	cameraPosition.y += y;
}

Vector2D GetCameraPosition()
{
	return cameraPosition;
}
