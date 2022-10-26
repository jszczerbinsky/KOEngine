#include "KOE.h"

extern SDL_Renderer *renderer;

TTF_Font *consoleFont = NULL;

struct ConsoleLine consoleLines[CONSOLE_LINES_MAX];

int ConsoleActive = 0;

void updateConsoleLinesTex()
{
	const SDL_Color foreground = {
		.r = 255,
		.g = 255,
		.b = 255
	};

	for(int i = 0; i < CONSOLE_LINES_MAX; i++)
	{
		if(consoleLines[i].tex) SDL_DestroyTexture(consoleLines[i].tex);
		char consoleLineBuff[CONSOLE_LINE_LENGTH+1];
		strcpy(consoleLineBuff, consoleLines[i].str);
		if(i == 0) strcat(consoleLineBuff, "_");
		SDL_Surface *line = TTF_RenderText_Solid(
				consoleFont, 
				strlen(consoleLineBuff) == 0 ? " " : consoleLineBuff, 
				foreground
		);	
		consoleLines[i].tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, CONSOLE_WIDTH, line->h);
		SDL_Texture *buffTex = SDL_CreateTextureFromSurface(renderer, line);
		SDL_SetRenderTarget(renderer, consoleLines[i].tex);
		SDL_SetRenderDrawColor(renderer, CONSOLE_BG);
		SDL_RenderClear(renderer);
		SDL_Rect dest = {
			.x = 0,
			.y = 0,
			.w = line->w,
			.h = line->h
		};
		SDL_RenderCopy(renderer, buffTex, NULL, &dest);
		SDL_SetRenderTarget(renderer, NULL);
		SDL_DestroyTexture(buffTex);
		SDL_FreeSurface(line);
	}
}

void consolePutChar(char c)
{
	int length = strlen(consoleLines[0].str);

	switch(c)
	{
		case 8:
			if(length >= 1)
				consoleLines[0].str[length-1] = '\0';
			break;
		case '\n':
			for(int i = CONSOLE_LINES_MAX-1; i > 0; i--)
				strcpy(consoleLines[i].str, consoleLines[i-1].str);
			consoleLines[0].str[0] = '\0';
			break;
		default:
			if(length+1 < CONSOLE_LINE_LENGTH)
			{
				consoleLines[0].str[length] = c;
				consoleLines[0].str[length+1] = '\0';
			}
			break;
	}
	updateConsoleLinesTex();
}

void InitializeConsole(const char *path)
{
	consoleFont = TTF_OpenFont(path, 12);

	for(int i = 0; i < CONSOLE_LINES_MAX; i++)
	{
		consoleLines[i].str[0] = '\0';
		consoleLines[i].tex = NULL;
	}

	updateConsoleLinesTex();
}

void OpenConsole()
{
	ConsoleActive = 1;
}

void CloseConsole()
{
	ConsoleActive = 0;
}
