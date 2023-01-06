#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <time.h>

#include "KOE.h"
#include "log.h"

extern void setKeyUp(SDL_KeyboardEvent *event);
extern void setKeyDown(SDL_KeyboardEvent *event);
extern void resetKeys();

extern void resetButtons();
extern void setButtonUp(MouseButton btn);
extern void setButtonDown(MouseButton btn);

extern void initGameObjects(unsigned int layersCount, unsigned int lightL);
extern void freeGameObjects();
extern void updateGameObjects();

extern void updateClients();

extern void initMultithreading();
extern void freeMultithreading();

extern void reloadLightResolution();
extern void freeLight();

extern void   initSound();
extern Music *lastMusic;
extern int    musicEnded;

extern void consolePutChar(char c);
extern void freeConsole();

SDL_Window   *window;
SDL_Renderer *renderer;

Resolution windowResolution = {.width = 800, .height = 600};

Color windowBg = {.r = 0, .g = 0, .b = 0};

float Delay = 0;

Resolution GetResolution() { return windowResolution; }
void       SetResolution(unsigned int width, unsigned int height)
{
  windowResolution.width  = width;
  windowResolution.height = height;

  if (window) SDL_SetWindowSize(window, width, height);

  reloadLightResolution();
}

void SetWindowMode(unsigned int mode) { SDL_SetWindowFullscreen(window, mode); }

void SetBackground(Color *c) { memcpy(&windowBg, c, sizeof(Color)); }

static void initApp(char *windowName, unsigned int audioChannels)
{
  window   = NULL;
  renderer = NULL;

  int rendererFlags = SDL_RENDERER_ACCELERATED;
  int windowFlags   = 0;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    Log("ERROR, can't initialize SDL: %s", SDL_GetError());
    exit(1);
  }

  Log("Initializing window");
  window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            windowResolution.width, windowResolution.height, windowFlags);

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

  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
  {
    Log("ERROR, can't initialize SDL_mixer: %s", Mix_GetError());
    exit(1);
  }
  Mix_AllocateChannels(audioChannels);
}

static void frameInit()
{
  SDL_SetRenderDrawColor(renderer, windowBg.r, windowBg.g, windowBg.b, 255);
  SDL_RenderClear(renderer);
}

static void frameShow() { SDL_RenderPresent(renderer); }

static void getInput()
{
  resetKeys();
  resetButtons();
  SDL_Event event;

  if (SDL_WaitEventTimeout(&event, 1000 / 60) != 0)
  {
    switch (event.type)
    {
      case SDL_QUIT:
        KOEngineExit();
        break;
      case SDL_KEYDOWN:
        if (ConsoleActive)
        {
          const char *keyName;
          switch (event.key.keysym.scancode)
          {
            case SDL_SCANCODE_ESCAPE:
            case SDL_SCANCODE_GRAVE:
              CloseConsole();
              break;
            case SDL_SCANCODE_SPACE:
              consolePutChar(' ');
              break;
            case SDL_SCANCODE_BACKSPACE:
              consolePutChar(8);
              break;
            case SDL_SCANCODE_RETURN:
              consolePutChar('\n');
              break;
            default:
              keyName = SDL_GetKeyName(event.key.keysym.sym);
              if (strlen(keyName) == 1) consolePutChar(keyName[0]);
              break;
          }
        }
        else
          setKeyDown(&event.key);
        break;
      case SDL_KEYUP:
        if (!ConsoleActive) setKeyUp(&event.key);
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

void KOEngineInit(const KOEngineSettings *s)
{
  Log("Initializing");
  srand((unsigned int)time(NULL));

  initApp(s->windowName, s->audioChannels);

  if (TTF_Init() == -1)
  {
    Log("ERROR, can't initialize SDL_ttf: %s", TTF_GetError());
    exit(1);
  }

  unsigned int timer = SDL_GetTicks();

  initMultithreading();
  initGameObjects(s->layersCount, s->lightLayer);
  initSound();

  reloadLightResolution();

  Log("Done, starting game");
  (*s->onStartPtr)();

  while (1)
  {
    frameInit();
    getInput();
    int ticks = SDL_GetTicks();
    Delay     = (ticks - timer) / 1000.0f;
    timer     = SDL_GetTicks();

    LOCK();

    if (musicEnded)
    {
      if (s->onMusicEndPtr) (*s->onMusicEndPtr)(lastMusic);
      musicEnded = 0;
    }

    (*s->loopCallPtr)();
    updateGameObjects();

    if (NetworkRole == ROLE_HOST && SocketWorking()) updateClients();

    UNLOCK();

    frameShow();
  }
}

void KOEngineExit()
{
  Log("Closing");
  KillAllGameObjects();
  freeGameObjects();
  freeConsole();
  freeMultithreading();
  freeLight();
  Mix_CloseAudio();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  TTF_Quit();
  exit(0);
}
