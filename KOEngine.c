#include <SDL2/SDL.h>
#include <time.h>

#include "KOEngine.h"

App app = { .resX = 800, .resY = 600};

uint8_t bgR = 0;
uint8_t bgG = 0;
uint8_t bgB = 0;

float Delay = 0;

App * getAppInfo()
{
	return &app;
}

void SetResolution(unsigned short width, unsigned short height)
{
	app.resX = width;
	app.resY = height;

	if(app.window)
		SDL_SetWindowSize(app.window, width, height);
}

void SetBackground(uint8_t r, uint8_t g, uint8_t b)
{
	bgR = r;
	bgG = g;
	bgB = b;
}

void initApp(char * windowName)
{
	app.window = NULL;
	app.renderer = NULL;

  int rendererFlags = SDL_RENDERER_ACCELERATED; 
  int windowFlags = 0;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

  app.window = SDL_CreateWindow(
      windowName, 
      SDL_WINDOWPOS_UNDEFINED, 
      SDL_WINDOWPOS_UNDEFINED, 
      app.resX, 
      app.resY, 
      windowFlags);

  if (!app.window)
	{
		printf("Failed to open window: %s\n", SDL_GetError());
		exit(1);
	}

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	if (!app.renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}
}

void frameInit()
{
  SDL_SetRenderDrawColor(app.renderer, bgR, bgG, bgB, 255);
	SDL_RenderClear(app.renderer);
}

void frameShow()
{
  SDL_RenderPresent(app.renderer);
}

void getInput()
{
	resetKeys();
	resetButtons();
  SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				KOEngineExit();	
				break;
			case SDL_KEYDOWN:
				setKeyDown(&event.key);
				break;
			case SDL_KEYUP:
				setKeyUp(&event.key);
				break;
			case SDL_MOUSEBUTTONDOWN:
				setButtonDown(event.button.button);
				break;
			case SDL_MOUSEBUTTONUP:
				setButtonUp(event.button.button);
				break;
			default:
				break;
		}
	}
}

void KOEngineInit(char *windowName, void (*onStartPtr)(), void (*loopCallPtr)())
{
	srand((unsigned int)time(NULL));
	initApp(windowName);

	TTF_Init();

	unsigned int timer = SDL_GetTicks();

	initEntities();

	(*onStartPtr)();

	while (1)
	{
    frameInit();
		getInput();
		int ticks = SDL_GetTicks();
		Delay = (ticks-timer)/1000.0f;
		timer = SDL_GetTicks();	
		int waitFor = 8-ticks;
		if(waitFor < 0) waitFor = 0;
		SDL_Delay(waitFor);
		(*loopCallPtr)();
		renderEntities(&app);
		frameShow();
	}
}


void KOEngineExit()
{
	SDL_DestroyRenderer(app.renderer);
  SDL_DestroyWindow(app.window);
  SDL_Quit();
	TTF_Quit();
	freeEntities();
	exit(0);
}
