#include "KOE.h"

extern SDL_Renderer *renderer;

TTF_Font *consoleFont = NULL;

struct ConsoleLine consoleLines[CONSOLE_LINES_MAX];

struct ConsoleHook
{
	struct ConsoleHookNamespace *namespace;
	const char *name;
	void (*procedure)();
	void *value;	
	int type;
	struct ConsoleHook *next;
};

struct ConsoleHookNamespace
{
	const char *name;
	struct ConsoleHookNamespace *next;
};

struct ConsoleHook *consoleHooks = NULL;
struct ConsoleHookNamespace *consoleHookNamespaces = NULL;

int ConsoleActive = 0;

struct ConsoleHookNamespace *findConsoleNamespace(const char *name)
{
	struct ConsoleHookNamespace *cHookN = consoleHookNamespaces;
	while(cHookN)
	{
		struct ConsoleHookNamespace *next = cHookN->next;

		if(strcmp(name, cHookN->name) == 0) return cHookN;

		cHookN = next;
	}
	return NULL;
}

void AddConsoleHook(const char *namespace, const char *name, int type, void (*proc)(), void *val)
{
	struct ConsoleHookNamespace *cHookN = findConsoleNamespace(namespace);

	if(cHookN == NULL)
	{
		cHookN = malloc(sizeof(struct ConsoleHookNamespace));
		cHookN->name = namespace;
		cHookN->next = consoleHookNamespaces;
		consoleHookNamespaces = cHookN;
	}

	struct ConsoleHook *hook = malloc(sizeof(struct ConsoleHook));
	hook->next = consoleHooks;
	consoleHooks = hook;

	hook->namespace = cHookN;
	hook->name = name;
	hook->procedure = proc;
	hook->value = val;
	hook->type = type;
}

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

void PrintConsoleLine(const char *line)
{
	for(int i = CONSOLE_LINES_MAX-1; i > 0; i--)
		strcpy(consoleLines[i].str, consoleLines[i-1].str);
	strcpy(consoleLines[1].str, line);
	updateConsoleLinesTex();
}

void consolePrintHelp(struct ConsoleHookNamespace *namespace)
{
	if(namespace)
	{
		struct ConsoleHook *currentHook = consoleHooks;

		while(currentHook)
		{
			if(currentHook->namespace == namespace)
			{
				const char *typeStr = "????";
				switch(currentHook->type)
				{
					case CONSOLEHOOK_PROC:
						typeStr = "Proc ";
						break;
					case CONSOLEHOOK_INT:
						typeStr = "Int  ";
						break;
					case CONSOLEHOOK_FLOAT:
						typeStr = "Float";
						break;

				}

				char str[CONSOLE_LINE_LENGTH];
				snprintf(str, CONSOLE_LINE_LENGTH, "  %s    %s.%s", typeStr, currentHook->namespace->name, currentHook->name);
				PrintConsoleLine(str);
			}
			currentHook = currentHook->next;
		}
	}
	else
	{
		struct ConsoleHookNamespace *currentHookN = consoleHookNamespaces;
		while(currentHookN)
		{
			char str[CONSOLE_LINE_LENGTH];
			snprintf(str, CONSOLE_LINE_LENGTH, "  %s", currentHookN->name);
			PrintConsoleLine(str);
			currentHookN = currentHookN->next;
		}
	}
}

void consoleExecute(const char *cmd)
{
	if((strlen(cmd) == 4 && strncmp(cmd, "HELP", 4) == 0) || (strlen(cmd) > 4 && strncmp(cmd, "HELP ", 5) == 0))

	{
		if(strlen(cmd) > 5)
		{
			consolePrintHelp(findConsoleNamespace(cmd+5));
		}
		else
			consolePrintHelp(NULL);
	}
	else
	{
		const char *ptr = cmd;
		int namespaceLength = 0;
		while(*ptr != '\0' && *ptr != '.')
		{
			ptr++;
			namespaceLength++;
		}
		if(*ptr != '\0')
		{
			char namespaceName[CONSOLE_LINE_LENGTH];
			strncpy(namespaceName, cmd, namespaceLength);
			namespaceName[namespaceLength] = '\0';
			struct ConsoleHookNamespace *namespace = findConsoleNamespace(namespaceName);
			if(namespace)
			{
				ptr++;
				int wordLength = 0;
				while(*ptr != '\0' && *ptr != ' ')
				{
					ptr++;
					wordLength++;
				}

				struct ConsoleHook *currentHook = consoleHooks;

				while(currentHook)
				{
					if(currentHook->namespace == namespace && strncmp(currentHook->name, cmd+namespaceLength+1, wordLength) == 0)
					{
						if(currentHook->type == CONSOLEHOOK_PROC)
							(*currentHook->procedure)();
						else
						{
							if(*ptr == ' ')
							{
								ptr++;
								if(currentHook->type == CONSOLEHOOK_INT)
									*((int*)currentHook->value) = atoi(ptr);
								else
									*((float*)currentHook->value) = atof(ptr);
							}
							else
							{
								char str[CONSOLE_LINE_LENGTH];
								if(currentHook->type == CONSOLEHOOK_INT)
									snprintf(str, CONSOLE_LINE_LENGTH, "%s = %d", currentHook->name, *((int*)currentHook->value));	
								else
									snprintf(str, CONSOLE_LINE_LENGTH, "%s = %f", currentHook->name, *((float*)currentHook->value));	
								PrintConsoleLine(str);
							}
						}
						return;	
					}
					currentHook = currentHook->next;
				}
			}
		}
		PrintConsoleLine("Command not found");
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
			consoleExecute(consoleLines[1].str);
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
	AddConsoleHook("ENGINE", "DEBUG-FLAGS", CONSOLEHOOK_INT, NULL, &DebugFlags);
	AddConsoleHook("ENGINE", "QUIT", CONSOLEHOOK_PROC, &KOEngineExit, NULL);
}

void freeConsole()
{
	for(int i = 0; i < CONSOLE_LINES_MAX; i++)
		if(consoleLines[i].tex)
			SDL_DestroyTexture(consoleLines[i].tex);

	struct ConsoleHook *cHook = consoleHooks;
	while(cHook)
	{
		struct ConsoleHook *next = cHook->next;
		free(cHook);
		cHook = next;
	}
	struct ConsoleHookNamespace *cHookN = consoleHookNamespaces;
	while(cHookN)
	{
		struct ConsoleHookNamespace *next = cHookN->next;
		free(cHook);
		cHookN = next;
	}
}

void OpenConsole()
{
	ConsoleActive = 1;
}

void CloseConsole()
{
	ConsoleActive = 0;
}
