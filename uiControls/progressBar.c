#include "../types.h"
#include "progressBar.h"

struct ProgressBarInstance
{
	Entity *progress;
	Entity *bg;
	float value;
	float minValue;
	float maxValue;
};

void freeProgressBarInstance(void *pbHookPtr, int killingAll)
{
	struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *)pbHookPtr;
	
	if(!killingAll)
	{
		KillEntity(pbHook->bg);
		KillEntity(pbHook->progress);
	}
}

void SetProgressBarValue(Entity *e, float val)
{
	struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *) e->extension;

	if(val < pbHook->minValue) val = pbHook->minValue;
	else if(val > pbHook->maxValue) val = pbHook->maxValue;

	pbHook->value = val;

	pbHook->progress->width =	e->width * (val - pbHook->minValue) / pbHook->maxValue;
	pbHook->progress->localPosition.x = pbHook->progress->width/2 - (float)e->width/2;
};

Entity *CreateProgressBar(const struct EntitySpawnSettings *ess, const struct UISpawnSettings *uis, const struct ProgressBarSettings *pbs)
{
	const struct UISpawnSettings noTextUis = {
		.font = NULL,
		.flags = 0
	};

	struct EntitySpawnSettings labelEss;
	memcpy(&labelEss, ess, sizeof(struct EntitySpawnSettings));
	labelEss.texture = NULL;
	labelEss.layer = pbs->labelLayer;

	Entity *e = CreateUIObject(&labelEss, uis);

	e->freeExtension = &freeProgressBarInstance;
	e->extension = malloc(sizeof(struct ProgressBarInstance));
	e->extensionType = EXT_UICONTROLS_PROGRESSBAR;
	
	struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *) e->extension;
	pbHook->minValue = pbs->minValue;
	pbHook->maxValue = pbs->maxValue;
	pbHook->value = 0;

	struct EntitySpawnSettings bgEss;
	memcpy(&bgEss, ess, sizeof(struct EntitySpawnSettings));
	bgEss.x = 0;
	bgEss.y = 0;

	pbHook->bg = CreateUIObject(&bgEss, &noTextUis);
	pbHook->bg->parent = e;

	struct EntitySpawnSettings pEss;
	memcpy(&pEss, ess, sizeof(struct EntitySpawnSettings));
	pEss.width = 0;
	pEss.x = 0;
	pEss.y = 0;
	pEss.texture = pbs->texProgress;
	pEss.layer = pbs->progressLayer;

	pbHook->progress = CreateUIObject(&pEss, &noTextUis);
	pbHook->progress->parent = e;

	return e;
}
