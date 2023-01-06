#ifndef KO_CONSOLE_H
#define KO_CONSOLE_H

#include <SDL2/SDL.h>

#define CONSOLE_LINE_LENGTH 512
#define CONSOLE_LINES_MAX   100
#define CONSOLE_WIDTH       800
#define CONSOLE_HEIGHT      600
#define CONSOLE_BG          50, 50, 50, 255
#define CONSOLE_PADDING     10

#define CONSOLEHOOK_PROC  0
#define CONSOLEHOOK_INT   1
#define CONSOLEHOOK_FLOAT 2

struct ConsoleLine
{
  char         str[CONSOLE_LINE_LENGTH];
  SDL_Texture *tex;
};

extern int ConsoleActive;

void InitializeConsole(const char *path);
void AddConsoleHook(const char *namespaceName, const char *name, int type, void (*proc)(),
                    void *val);
void PrintConsoleLine(const char *line);
void OpenConsole();
void CloseConsole();

#endif
