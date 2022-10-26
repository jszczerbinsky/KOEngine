#include "KOE.h"

#define CONSOLE_LINE_LENGTH 1024

extern SDL_Renderer *renderer;

TTF_Font *consoleFont = NULL;

char consoleLine[CONSOLE_LINE_LENGTH] = "";
SDL_Texture *consoleLineTex = NULL;

int ConsoleActive = 0;

void updateConsoleLineTex()
{
	SDL_Color foreground = {
		.r = 255,
		.g = 255,
		.b = 255
	};

	if(consoleLineTex) SDL_DestroyTexture(consoleLineTex);
	char consoleLineBuff[CONSOLE_LINE_LENGTH+1];
	strcpy(consoleLineBuff, consoleLine);
	strcat(consoleLineBuff, "_");
	SDL_Surface *line = TTF_RenderText_Solid(consoleFont, consoleLineBuff, foreground);	
	consoleLineTex = SDL_CreateTextureFromSurface(renderer, line);
	SDL_FreeSurface(line);
}

void consolePutChar(char c)
{
	int length = strlen(consoleLine);

	if(c == 8)
	{
		if(length >= 1)
			consoleLine[length-1] = '\0';
	}
	else
	{
		if(length+1 < CONSOLE_LINE_LENGTH)
		{
			consoleLine[length] = c;
			consoleLine[length+1] = '\0';
		}
	}
	updateConsoleLineTex();
}

void InitializeConsole(const char *path)
{
	consoleFont = TTF_OpenFont(path, 16);
	updateConsoleLineTex();
}

void OpenConsole()
{
	ConsoleActive = 1;
}

void CloseConsole()
{
	ConsoleActive = 0;
}
