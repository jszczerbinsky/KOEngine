#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>

#include "KOEngine.h"
#include "log.h"

extern void setKeyUp  (SDL_KeyboardEvent *event);
extern void setKeyDown(SDL_KeyboardEvent *event);
extern void resetKeys ();

extern void resetButtons ();
extern void setButtonUp  (MouseButton btn);
extern void setButtonDown(MouseButton btn);

extern void initEntities();
extern void updateEntities();

extern void updateClients();

extern void initMultithreading();
extern void freeMultithreading();

extern void initSound();
extern Music *lastMusic;
extern int musicEnded;

SDL_Window   *window;
SDL_Renderer *renderer;

struct Resolution WindowResolution = {
	.width  = 800,
	.height = 600
};

Color windowBg = {
	.r = 0,
	.g = 0,
	.b = 0
};

float Delay = 0;

void SetResolution(unsigned int width, unsigned int height)
{
	WindowResolution.width 	= width;
	WindowResolution.height = height;

	if(window)
		SDL_SetWindowSize(window, width, height);
}

void SetWindowMode(unsigned int mode)
{
  SDL_SetWindowFullscreen(window, mode);
}

void SetBackground(Color *c)
{
	memcpy(&windowBg, c, sizeof(Color));
}

void initApp(char * windowName, unsigned int audioChannels)
{
	window = NULL;
	renderer = NULL;

  int rendererFlags = SDL_RENDERER_ACCELERATED; 
  int windowFlags = 0;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		Log("ERROR, can't initialize SDL: %s", SDL_GetError());
		exit(1);
	}

  window = SDL_CreateWindow(
      windowName, 
      SDL_WINDOWPOS_UNDEFINED, 
      SDL_WINDOWPOS_UNDEFINED, 
      WindowResolution.width, 
      WindowResolution.height, 
      windowFlags);

  if (!window)
	{
		Log("ERROR, failed to open window: %s", SDL_GetError());
		exit(1);
	}

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

	renderer = SDL_CreateRenderer(window, -1, rendererFlags);

	if (!renderer)
	{
		Log("ERROR, failed to create renderer: %s", SDL_GetError());
		exit(1);
	}

	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		Log("ERROR, can't initialize SDL_mixer: %s", Mix_GetError());
		exit(1);
	}
	Mix_AllocateChannels(audioChannels);
}

void frameInit()
{
  SDL_SetRenderDrawColor(renderer, windowBg.r, windowBg.g, windowBg.b, 255);
	SDL_RenderClear(renderer);
}

void frameShow()
{
  SDL_RenderPresent(renderer);
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

void KOEngineInit(const struct KOEngineSettings *s)
{
	Log("Initializing");
	srand((unsigned int)time(NULL));

	initApp(s->windowName, s->audioChannels);
	
	if(TTF_Init() == -1)
	{
		Log("ERROR, can't initialize SDL_ttf: %s", TTF_GetError());
		exit(1);
	}

	unsigned int timer = SDL_GetTicks();

	initMultithreading();
	initEntities();
	initSound();

	Log("Done, starting game");
	(*s->onStartPtr)();

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

		LOCK();

		if(musicEnded)
		{
			if(s->onMusicEndPtr)
				(*s->onMusicEndPtr)(lastMusic);
			musicEnded = 0;
		}

		(*s->loopCallPtr)();
		updateEntities();

		if(NetworkRole == ROLE_HOST && SocketWorking())
			updateClients();

		UNLOCK();

		frameShow();
	}
}


void KOEngineExit()
{
	Log("Closing");
	KillAllEntities();
	freeMultithreading();
	Mix_CloseAudio();
	SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
	TTF_Quit();
	exit(0);
}
