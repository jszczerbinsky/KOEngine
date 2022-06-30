#include "../types.h"
#include "progressBar.h"

struct ProgressBarInstance
{
	GameObject *progress;
	GameObject *bg;
	float value;
	float minValue;
	float maxValue;
};

void freeProgressBarInstance(void *pbHookPtr, int killingAll)
{
	struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *)pbHookPtr;
	
	if(!killingAll)
	{
		KillGameObject(pbHook->bg);
		KillGameObject(pbHook->progress);
	}
}

void SetProgressBarValue(GameObject *e, float val)
{
	struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *) e->extension;

	if(val < pbHook->minValue) val = pbHook->minValue;
	else if(val > pbHook->maxValue) val = pbHook->maxValue;

	pbHook->value = val;

	pbHook->progress->width =	e->width * (val - pbHook->minValue) / pbHook->maxValue;
	pbHook->progress->localPosition.x = pbHook->progress->width/2 - (float)e->width/2;
};

GameObject *CreateProgressBar(const struct GameObjectSpawnSettings *ess, const struct UISpawnSettings *uis, const struct ProgressBarSettings *pbs)
{
	const struct UISpawnSettings noTextUis = {
		.font = NULL,
		.flags = 0
	};

	struct GameObjectSpawnSettings labelEss;
	memcpy(&labelEss, ess, sizeof(struct GameObjectSpawnSettings));
	labelEss.texture = NULL;
	labelEss.layer = pbs->labelLayer;

	GameObject *e = CreateUIObject(&labelEss, uis);

	e->freeExtension = &freeProgressBarInstance;
	e->extension = malloc(sizeof(struct ProgressBarInstance));
	e->extensionType = EXT_UICONTROLS_PROGRESSBAR;
	
	struct ProgressBarInstance *pbHook = (struct ProgressBarInstance *) e->extension;
	pbHook->minValue = pbs->minValue;
	pbHook->maxValue = pbs->maxValue;
	pbHook->value = 0;

	struct GameObjectSpawnSettings bgEss;
	memcpy(&bgEss, ess, sizeof(struct GameObjectSpawnSettings));
	bgEss.x = 0;
	bgEss.y = 0;

	pbHook->bg = CreateUIObject(&bgEss, &noTextUis);
	pbHook->bg->parent = e;

	struct GameObjectSpawnSettings pEss;
	memcpy(&pEss, ess, sizeof(struct GameObjectSpawnSettings));
	pEss.width = 0;
	pEss.x = 0;
	pEss.y = 0;
	pEss.texture = pbs->texProgress;
	pEss.layer = pbs->progressLayer;

	pbHook->progress = CreateUIObject(&pEss, &noTextUis);
	pbHook->progress->parent = e;

	return e;
}
